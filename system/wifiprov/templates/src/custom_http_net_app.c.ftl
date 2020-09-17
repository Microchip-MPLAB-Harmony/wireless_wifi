/*******************************************************************************
  Application to Demo HTTP NET Server

  Summary:
    Support for HTTP NET module in Microchip TCP/IP Stack

  Description:
    -Implements the application
 *******************************************************************************/

/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/


#include "system_config.h"
#include "system_definitions.h"
#include "http_net_print.h"
#include "crypto/crypto.h"
#include "net_pres/pres/net_pres_socketapi.h"
#include "system/sys_random_h2_adapter.h"
#include "system/sys_time_h2_adapter.h"
#include "tcpip/tcpip.h"
#include "tcpip/src/common/helpers.h"
#include "system/wifi/sys_wifi.h"

/****************************************************************************
  Section:
    Definitions
 ****************************************************************************/

static SYS_WIFI_CONFIG g_wifiConfigHTTPNet;
static uint8_t val[2];
#define CONVERT_TO_NUMBER(a) (a - '0')
#define CONVERT_TO_ASCII(a) (a + '0')

/****************************************************************************
Section:
Function Prototypes and Memory Globalizers
 ****************************************************************************/
// Sticky status message variable.
// This is used to indicated whether or not the previous POST operation was
// successful.  The application uses these to store status messages when a
// POST operation redirects.  This lets the application provide status messages
// after a redirect, when connection instance data has already been lost.
static bool lastSuccess = false;

// Sticky status message variable.  See lastSuccess for details.
static bool lastFailure = false;

// Wi-Fi Provisioning Data Ready Status
static bool g_wifiProvDataReady = false;

/****************************************************************************
  Section:
    Customized HTTP NET Functions
 ****************************************************************************/

// processing the HTTP buffer acknowledgment
void TCPIP_HTTP_NET_DynAcknowledge(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const void *buffer, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = (HTTP_APP_DYNVAR_BUFFER*)((const uint8_t *)buffer - offsetof(HTTP_APP_DYNVAR_BUFFER, data));

    pDynBuffer->busy = 0;
}

// processing the HTTP reported events
void TCPIP_HTTP_NET_EventReport(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_NET_EVENT_TYPE evType, const void *evInfo, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    char *evMsg = (char *)evInfo;

    if(evType < 0)
    {   // display errors only
        if(evMsg == 0)
        {
            evMsg = "none";
        }
        SYS_CONSOLE_PRINT("HTTP event: %d, info: %s\r\n", evType, evMsg);
    }
}

// example of processing an SSI notification
// return false for standard processing of this SSI command by the HTTP module
// return true if the processing is done by you and HTTP need take no further action
bool TCPIP_HTTP_NET_SSINotification(TCPIP_HTTP_NET_CONN_HANDLE connHandle, TCPIP_HTTP_SSI_NOTIFY_DCPT *pSSINotifyDcpt, const struct _tag_TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    static  int newVarCount = 0;

    char    *cmdStr, *varName;
    char    newVarVal[] = "Page Visits: ";
    char    scratchBuff[100];

    cmdStr = pSSINotifyDcpt->ssiCommand;

    if(strcmp(cmdStr, "include") == 0)
    {   // here a standard SSI include directive is processed
        if( g_wifiProvDataReady == true) 
        {
            SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_CONNECT, &g_wifiConfigHTTPNet, sizeof(SYS_WIFI_CONFIG));
            g_wifiProvDataReady = false;
        }
        return false;
    }

    if(strcmp(cmdStr, "set") == 0)
    {   // a SSI variable is set; let the standard processing take place
        return false;
    }

    if(strcmp(cmdStr, "echo") == 0)
    {   // SSI echo command
        // inspect the variable name
        varName = pSSINotifyDcpt->pAttrDcpt->value;
        if(strcmp(varName, "myStrVar") == 0)
        {   // change the value of this variable
            sprintf(scratchBuff, "%s%d", newVarVal, ++newVarCount);

            if(!TCPIP_HTTP_NET_SSIVariableSet(varName, TCPIP_HTTP_DYN_ARG_TYPE_STRING, scratchBuff, 0))
            {
                SYS_CONSOLE_MESSAGE("SSI set myStrVar failed!!!\r\n");
            }
            // else success
            return false;
        }
    }

    return false;
}

/****************************************************************************
  Section:
    GET Form Handlers
 ****************************************************************************/

/*****************************************************************************
  Function:
    TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionGetExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)

  Internal:
    See documentation in the TCP/IP Stack APIs or http_net.h for details.
 ****************************************************************************/
TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionGetExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    return TCPIP_HTTP_NET_IO_RES_DONE;
}

/****************************************************************************
  Section:
    POST Form Handlers
 ****************************************************************************/
#if defined(TCPIP_HTTP_NET_USE_POST)

/*****************************************************************************
  Function:
    TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionPostExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)

  Internal:
    See documentation in the TCP/IP Stack APIs or http_net.h for details.
 ****************************************************************************/
TCPIP_HTTP_NET_IO_RESULT TCPIP_HTTP_NET_ConnectionPostExecute(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    uint8_t *httpDataBuff = 0;
    uint16_t httpBuffSize = 0;
    uint32_t byteCount = 0;
    uint8_t  offset = 0;

    // Check to see if the browser is attempting to submit more data than we
    // can parse at once.  This function needs to receive all updated
    // parameters and validate them all before committing them to memory so that
    // orphaned configuration parameters do not get written (for example, if a
    // static IP address is given, but the subnet mask fails parsing, we
    // should not use the static IP address).  Everything needs to be processed
    // in a single transaction.  If this is impossible, fail and notify the user.
    // As a web devloper, if you add parameters to the network info and run into this
    // problem, you could fix this by to splitting your update web page into two
    // seperate web pages (causing two transactional writes).  Alternatively,
    // you could fix it by storing a static shadow copy of network info someplace
    // in memory and using it when info is complete.
    // Lastly, you could increase the TCP RX FIFO size for the HTTP server.
    // This will allow more data to be POSTed by the web browser before hitting this limit.
    byteCount = TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle);
    //SYS_CONSOLE_PRINT("Received byteCount=%d \n",byteCount);
    if(byteCount)
    {
        if(byteCount > TCPIP_HTTP_NET_ConnectionReadBufferSize(connHandle))
        {   // Configuration Failure
            lastFailure = true;
            TCPIP_HTTP_NET_ConnectionStatusSet(connHandle, TCPIP_HTTP_NET_STAT_REDIRECT);
            return TCPIP_HTTP_NET_IO_RES_DONE;
        }
    
        // Ensure that all data is waiting to be parsed.  If not, keep waiting for
        // all of it to arrive.
        if(TCPIP_HTTP_NET_ConnectionReadIsReady(connHandle) < byteCount)
        {
            return TCPIP_HTTP_NET_IO_RES_NEED_DATA;
        }
        // Use current config in non-volatile memory as defaults
        httpDataBuff = TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle);
        httpBuffSize = TCPIP_HTTP_NET_ConnectionDataBufferSizeGet(connHandle);

        offset = 10;  //Length of config.htm

        // Read all browser POST data
        while(TCPIP_HTTP_NET_ConnectionByteCountGet(connHandle))
        {
            // Read a form field name
            if(TCPIP_HTTP_NET_ConnectionPostNameRead(connHandle, httpDataBuff, offset) != TCPIP_HTTP_NET_READ_OK)
            {
                break;
            }
            // Read a form field value
            if(TCPIP_HTTP_NET_ConnectionPostValueRead(connHandle, httpDataBuff + offset, httpBuffSize-offset-2) != TCPIP_HTTP_NET_READ_OK)
            {
                break;
            }
            // Parse the value that was read
            if(!strcmp((char *)httpDataBuff, (const char *)"devmode"))
            {
                g_wifiConfigHTTPNet.mode = httpDataBuff[offset] == '1' ? 1: 0;
                offset += (strlen("devmode")+2);
            
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"saveconfig"))
            {
                g_wifiConfigHTTPNet.saveConfig = httpDataBuff[offset] == '1' ? 1: 0;
                offset += (strlen("saveconfig")+2);
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"countrycode"))
            {
                uint8_t countryCodeVal = (uint8_t) CONVERT_TO_NUMBER(httpDataBuff[offset]);
                switch(countryCodeVal)
                {
                    case 0: //GEN
                    {
                        strcpy((char *)g_wifiConfigHTTPNet.countryCode,(const char *)"GEN");
                        break;
                    }
                    case 1: //USA
                    {
                        strcpy((char *)g_wifiConfigHTTPNet.countryCode,(const char *)"USA");
                        break;
                    }
                    case 2: //EMEA
                    {
                        strcpy((char *)g_wifiConfigHTTPNet.countryCode,(const char *)"EMEA");
                        break;
                    }
                    case 3: //CUST1
                    {
                        strcpy((char *)g_wifiConfigHTTPNet.countryCode,(const char *)"CUST1");
                        break;
                    }
                    case 4: //CUST2
                    {
                        strcpy((char *)g_wifiConfigHTTPNet.countryCode,(const char *)"CUST2");
                        break;
                    }
                }
                offset += (strlen("countrycode")+2);
            }
<#if SYS_WIFIPROV_STA_ENABLE == true>
            else if(!strcmp((char *)httpDataBuff, (const char *)"stassid"))
            {
                uint8_t len = strlen((const char *)httpDataBuff + offset);
                strncpy((char *)g_wifiConfigHTTPNet.staConfig.ssid, (char *)httpDataBuff + offset, len+1);
                offset += (strlen("stassid")+len+2);
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"stapwd"))
            {
                uint8_t len = strlen((const char *)httpDataBuff + offset);
                strncpy((char *)g_wifiConfigHTTPNet.staConfig.psk, (char *)httpDataBuff + offset, len+1);
                offset += strlen("stapwd") + len + 2;
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"staauth"))
            {   
               g_wifiConfigHTTPNet.staConfig.authType = CONVERT_TO_NUMBER(httpDataBuff[offset]);
                offset += strlen("staauth")+2;
            } 
            else if(!strcmp((char *)httpDataBuff, (const char *)"stach"))
            {   
                g_wifiConfigHTTPNet.staConfig.channel = CONVERT_TO_NUMBER(httpDataBuff[offset]);
                offset += strlen("stach")+2;;
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"staauto"))
            {
                g_wifiConfigHTTPNet.staConfig.autoConnect = httpDataBuff[offset] == '1' ? 1: 0;
                offset += strlen("staauto")+2;;
            }
</#if>
<#if SYS_WIFIPROV_AP_ENABLE == true>
            else if(!strcmp((char *)httpDataBuff, (const char *)"apssid"))
            {
                uint8_t apSsidLen = strlen((const char *)httpDataBuff + offset);
                strncpy((char *)g_wifiConfigHTTPNet.apConfig.ssid, (char *)httpDataBuff + offset, apSsidLen+1);
                offset += strlen("apssid") + apSsidLen + 2;
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"appwd"))
            {
                uint8_t apPwdLen = strlen((const char *)httpDataBuff + offset);
                strncpy((char *)g_wifiConfigHTTPNet.apConfig.psk, (char *)httpDataBuff + offset, apPwdLen+1);
                offset += strlen("appwd") + apPwdLen + 2;
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"apauth"))
            {
                g_wifiConfigHTTPNet.apConfig.authType = CONVERT_TO_NUMBER(httpDataBuff[offset]);
                offset += strlen("apauth")+2;;
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"apch"))
            {
                g_wifiConfigHTTPNet.apConfig.channel = CONVERT_TO_NUMBER(httpDataBuff[offset]);
                offset += strlen("apch")+2;;
            }
            else if(!strcmp((char *)httpDataBuff, (const char *)"ssidv"))
            {
                g_wifiConfigHTTPNet.apConfig.ssidVisibility = httpDataBuff[offset] == '1' ? 1: 0;
                offset += strlen("ssidv")+2;;
            }
</#if>
        }
        g_wifiProvDataReady = true;
    }
    return TCPIP_HTTP_NET_IO_RES_DONE;
}

/****************************************************************************
  Section:
    Authorization Handlers
 ****************************************************************************/

/*****************************************************************************
  Function:
    uint8_t TCPIP_HTTP_NET_ConnectionFileAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, uint8_t *cFile, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)

  Internal:
    See documentation in the TCP/IP Stack APIs or http_net.h for details.
 ****************************************************************************/
#if defined(TCPIP_HTTP_NET_USE_AUTHENTICATION)
uint8_t TCPIP_HTTP_NET_ConnectionFileAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const char *cFile, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{
    if(memcmp(cFile, "index.htm", 9) == 0)
    {
        memset((void *)&g_wifiConfigHTTPNet,0,sizeof(g_wifiConfigHTTPNet));
        SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_GETCONFIG, &g_wifiConfigHTTPNet, sizeof(SYS_WIFI_CONFIG));
    }
    return 0x80; // No authentication required
}
#endif

/*****************************************************************************
  Function:
    uint8_t TCPIP_HTTP_NET_ConnectionUserAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const char *cUser, const char *cPass, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)

  Internal:
    See documentation in the TCP/IP Stack APIs or http_net.h for details.
 ****************************************************************************/
#if defined(TCPIP_HTTP_NET_USE_AUTHENTICATION)
uint8_t TCPIP_HTTP_NET_ConnectionUserAuthenticate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const char *cUser, const char *cPass, const TCPIP_HTTP_NET_USER_CALLBACK *pCBack)
{

    // You can add additional user/pass combos here.
    // If you return specific "realm" values above, you can base this
    //   decision on what specific file or folder is being accessed.
    // You could return different values (0x80 to 0xff) to indicate
    //   various users or groups, and base future processing decisions
    //   in TCPIP_HTTP_NET_ConnectionGetExecute/Post or HTTPPrint callbacks on this value.
    return 0x00;            // Provided user/pass is invalid
}
#endif

/****************************************************************************
  Section:
    Dynamic Variable Callback Functions
 ****************************************************************************/
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_hellomsg(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const uint8_t *ptr;

    ptr = TCPIP_HTTP_NET_ArgGet(TCPIP_HTTP_NET_ConnectionDataBufferGet(connHandle), (const uint8_t *)"name");

    if(ptr != NULL)
    {
        size_t nChars;
        HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
        if(pDynBuffer == 0)
        {   // failed to get a buffer; retry
            return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
        }

        nChars = sprintf(pDynBuffer->data, "Hello, %s", ptr);
        TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    }

    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_builddate(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, __DATE__" "__TIME__, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_version(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)TCPIP_STACK_VERSION_STR, false);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}


TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_pot(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    uint16_t RandVal;
    size_t nChars;

    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    RandVal = (uint16_t)SYS_RANDOM_PseudoGet();
    nChars = sprintf(pDynBuffer->data, "%d", RandVal);
    TCPIP_HTTP_NET_DynamicWrite(vDcpt, pDynBuffer->data, nChars, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_status_ok(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *statMsg = lastSuccess ? "block" : "none";
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, statMsg, false);
    lastSuccess = false;
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_status_fail(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    const char *statMsg = lastFailure ? "block" : "none";
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, statMsg, false);
    lastFailure = false;
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_hostname(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_NET_HANDLE hNet;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer;
    const char *nbnsName;

    hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    nbnsName = TCPIP_STACK_NetBIOSName(hNet);

    if(nbnsName == 0)
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "Failed to get a Host name", false);
    }
    else
    {
        pDynBuffer = HTTP_APP_GetDynamicBuffer();
        if(pDynBuffer == 0)
        {   // failed to get a buffer; retry
            return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
        }
        strncpy(pDynBuffer->data, nbnsName, HTTP_APP_DYNVAR_BUFFER_SIZE);
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, pDynBuffer->data, true);
    }

    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_ip(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR ipAddress;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    ipAddress.Val = TCPIP_STACK_NetAddress(hNet);

    TCPIP_Helper_IPAddressToString(&ipAddress, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_gw(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR gwAddress;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    gwAddress.Val = TCPIP_STACK_NetAddressGateway(hNet);
    TCPIP_Helper_IPAddressToString(&gwAddress, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_subnet(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR ipMask;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    ipMask.Val = TCPIP_STACK_NetMask(hNet);
    TCPIP_Helper_IPAddressToString(&ipMask, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_dns1(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR priDnsAddr;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;
    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    priDnsAddr.Val = TCPIP_STACK_NetAddressDnsPrimary(hNet);
    TCPIP_Helper_IPAddressToString(&priDnsAddr, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_dns2(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    IPV4_ADDR secondDnsAddr;
    char *ipAddStr;
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer = HTTP_APP_GetDynamicBuffer();
    if(pDynBuffer == 0)
    {   // failed to get a buffer; retry
        return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
    }

    ipAddStr = pDynBuffer->data;

    TCPIP_NET_HANDLE hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    secondDnsAddr.Val = TCPIP_STACK_NetAddressDnsSecond(hNet);
    TCPIP_Helper_IPAddressToString(&secondDnsAddr, ipAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, ipAddStr, true);
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_mac(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    TCPIP_NET_HANDLE hNet;
    const TCPIP_MAC_ADDR *pMacAdd;
    char macAddStr[20];
    HTTP_APP_DYNVAR_BUFFER *pDynBuffer;

    hNet = TCPIP_HTTP_NET_ConnectionNetHandle(connHandle);
    pMacAdd = (const TCPIP_MAC_ADDR*)TCPIP_STACK_NetAddressMac(hNet);
    if(pMacAdd)
    {
        TCPIP_Helper_MACAddressToString(pMacAdd, macAddStr, sizeof(macAddStr));
        pDynBuffer = HTTP_APP_GetDynamicBuffer();
        if(pDynBuffer == 0)
        {   // failed to get a buffer; retry
            return TCPIP_HTTP_DYN_PRINT_RES_AGAIN;
        }
        strncpy(pDynBuffer->data, macAddStr, HTTP_APP_DYNVAR_BUFFER_SIZE);
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, pDynBuffer->data, true);
    }
    else
    {
        TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "Failed to get a MAC address", false);
    }

    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_devmode(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        if(vDcpt->dynArgs->argInt32 == (int) g_wifiConfigHTTPNet.mode)
        {
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);
        }
    }
   return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_saveconfig(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        if(vDcpt->dynArgs->argInt32 == (int) g_wifiConfigHTTPNet.saveConfig)
        {
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);
        }
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_countrycode(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        switch(vDcpt->dynArgs->argInt32)
        {
            case 0: //GEN
            {
                if(!memcmp((const char *)&g_wifiConfigHTTPNet.countryCode,"GEN",3))
                {
                     TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);          
                }
                break; 
            }

            case 1:  //USA
            {
                if(!memcmp((const char *)&g_wifiConfigHTTPNet.countryCode,"USA",3))
                {
                    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);         
                }
                 break;
            }
          
            case 2:  //EMEA
            {
                if(!strncmp((const char *)g_wifiConfigHTTPNet.countryCode,"EMEA",4))
                {
                    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);         
                }
                 break;
            }
            case 3:  //CUST1
            {
                if(!strncmp((const char *)g_wifiConfigHTTPNet.countryCode,"CUST1",5))
                {
                    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);         
                }
                 break;
            }
            case 4:  //CUST2
            {
                if(!strncmp((const char *)g_wifiConfigHTTPNet.countryCode,"CUST2",5))
                {
                    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);         
                }
                 break;
            }
        }
    }
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_stassid(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)g_wifiConfigHTTPNet.staConfig.ssid, true);
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_stapwd(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)g_wifiConfigHTTPNet.staConfig.psk, true);
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_stachannel(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    val[0]= CONVERT_TO_ASCII(g_wifiConfigHTTPNet.staConfig.channel);
    val[1]= 0;
    
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)val, true);
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_autoconnect(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        if(vDcpt->dynArgs->argInt32 == (int) g_wifiConfigHTTPNet.staConfig.autoConnect)
        {
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);
        }
    }
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_staauth(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        if(vDcpt->dynArgs->argInt32 == g_wifiConfigHTTPNet.staConfig.authType)
        {
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);
        }
    }
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_apssid(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)g_wifiConfigHTTPNet.apConfig.ssid, false);
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_appwd(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)g_wifiConfigHTTPNet.apConfig.psk, false);
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_apauth(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        if(vDcpt->dynArgs->argInt32 == g_wifiConfigHTTPNet.apConfig.authType)
        {
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);
        }
    }
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_apchannel(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    val[0]= CONVERT_TO_ASCII(g_wifiConfigHTTPNet.apConfig.channel);
    val[1]= 0;  
    TCPIP_HTTP_NET_DynamicWriteString(vDcpt, (const char *)val, false);
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}
TCPIP_HTTP_DYN_PRINT_RES TCPIP_HTTP_Print_config_ssidv(TCPIP_HTTP_NET_CONN_HANDLE connHandle, const TCPIP_HTTP_DYN_VAR_DCPT *vDcpt)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    if(vDcpt->nArgs != 0 && vDcpt->dynArgs->argType == TCPIP_HTTP_DYN_ARG_TYPE_INT32)
    {
        if(vDcpt->dynArgs->argInt32 == (int) g_wifiConfigHTTPNet.apConfig.ssidVisibility)
        {
            TCPIP_HTTP_NET_DynamicWriteString(vDcpt, "SELECTED", false);
        }
    }
</#if>
    return TCPIP_HTTP_DYN_PRINT_RES_DONE;
}

#endif // #if defined(TCPIP_STACK_USE_HTTP_SERVER)
