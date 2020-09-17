/*******************************************************************************
  Application to Demo custom HTTP


  File Name
    custom_http_app.c

  Summary
    Application to Demo custom HTTP

  Description
    This file defines the interface to communicate to HTTP webpage.

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2020 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"
#include "system/wifi/sys_wifi.h"
#if defined(TCPIP_STACK_USE_HTTP_SERVER)
#include "tcpip/tcpip.h"
#include "tcpip/src/common/helpers.h"

/****************************************************************************
  Section:
    Definitions
 ****************************************************************************/
#define HTTP_APP_IPV4_ADDRESS_BUFFER_SIZE 20

/****************************************************************************
  Section:
    Function Prototypes
 ****************************************************************************/
static HTTP_IO_RESULT HTTPPostConfig_wifi(HTTP_CONN_HANDLE connHandle);

/****************************************************************************
  Section:
    Variables
 ****************************************************************************/
 
static uint8_t s_buf_ipv4addr[HTTP_APP_IPV4_ADDRESS_BUFFER_SIZE];

static SYS_WIFI_CONFIG wifiConfig;
#define CONVERT_TO_NUMBER(a) (a-'0')
#define CONVERT_TO_ASCII(a) (a+'0')

// network configuration/information storage space
static struct
{
    TCPIP_NET_HANDLE    currNet;            // current working interface + valid flag
    char                ifName[10 + 1];     // interface name
    char                nbnsName[16 + 1];   // host name
    char                ifMacAddr[17 + 1];  // MAC address
    char                ipAddr[15 +1];      // IP address
    char                ipMask[15 + 1];     // mask
    char                gwIP[15 + 1];       // gateway IP address
    char                dns1IP[15 + 1];     // DNS IP address
    char                dns2IP[15 + 1];     // DNS IP address

    TCPIP_NETWORK_CONFIG   netConfig;  // configuration in the interface requested format
}httpNetData;
inline uint8_t Findstring_lenth(uint8_t *ptr)
{
    uint8_t len=0;
    for(;*ptr != '&';ptr++,len++);
    return len;
}
/****************************************************************************
  Section:
    GET Form Handlers
 ****************************************************************************/

/****************************************************************************
  Function:
    HTTP_IO_RESULT TCPIP_HTTP_GetExecute(HTTP_CONN_HANDLE connHandle)

  Internal:
    See documentation in the TCP/IP Stack APIs or http.h for details.
 ****************************************************************************/
HTTP_IO_RESULT TCPIP_HTTP_GetExecute(HTTP_CONN_HANDLE connHandle)
{
    return HTTP_IO_DONE;
}

/****************************************************************************
  Function:
    HTTP_IO_RESULT TCPIP_HTTP_PostExecute(HTTP_CONN_HANDLE connHandle)

  Internal:
    See documentation in the TCP/IP Stack APIs or http.h for details.
 ****************************************************************************/
HTTP_IO_RESULT TCPIP_HTTP_PostExecute(HTTP_CONN_HANDLE connHandle)
{
    // Resolve which function to use and pass along
    uint8_t filename[20];

    // Load the file name
    // Make sure uint8_t filename[] above is large enough for your longest name
    SYS_FS_FileNameGet(TCPIP_HTTP_CurrentConnectionFileGet(connHandle), filename, sizeof(filename));
   
    if(!memcmp(filename, "config.htm", 10))
    {
        return HTTPPostConfig_wifi(connHandle);
    }

    return HTTP_IO_DONE;
}
/****************************************************************************
  Section:
    Authorization Handlers
 ****************************************************************************/

/****************************************************************************
  Function:
    uint8_t TCPIP_HTTP_FileAuthenticate(HTTP_CONN_HANDLE connHandle, uint8_t *cFile)

  Internal:
    See documentation in the TCP/IP Stack APIs or http.h for details.
 ****************************************************************************/
uint8_t TCPIP_HTTP_FileAuthenticate(HTTP_CONN_HANDLE connHandle, uint8_t *cFile)
{
    return 0x80; // No authentication required.
}
/****************************************************************************
  Function:
    uint8_t TCPIP_HTTP_UserAuthenticate(uint8_t *cUser, uint8_t *cPass)

  Internal:
    See documentation in the TCP/IP Stack APIs or http.h for details.
 ****************************************************************************/

uint8_t TCPIP_HTTP_UserAuthenticate(HTTP_CONN_HANDLE connHandle, uint8_t *cUser, uint8_t *cPass)
{
    return 0x00;            // Provided user/pass is invalid
}
/*******************************************************************************
  Function:
    static HTTP_IO_RESULT HTTPPostConfig_wifi(HTTP_CONN_HANDLE connHandle)

  Summary:
    Processes the Wi-Fi configuration form on config.htm

  Description:
    Accepts configuration parameters from the form, saves them to a
    temporary location in RAM and Provide to Wi-Fi service for updating configuration.

  Precondition:
    None

  Parameters:
    connHandle  - HTTP connection handle

  Return Values:
    HTTP_IO_DONE - all parameters have been processed
    HTTP_IO_NEED_DATA - data needed by this function has not yet arrived
 *******************************************************************************/

static HTTP_IO_RESULT HTTPPostConfig_wifi(HTTP_CONN_HANDLE connHandle)
{
   // bool bConfigFailure = false;
    uint8_t httpDataBuff[256];
    uint16_t Index=0;
    uint32_t byteCount;
    TCP_SOCKET sktHTTP;
    
    SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_GETCONFIG, &wifiConfig, sizeof(SYS_WIFI_CONFIG));
    httpNetData.currNet = 0; // forget the old settings
    
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
    byteCount = TCPIP_HTTP_CurrentConnectionByteCountGet(connHandle);
    sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    if(byteCount > TCPIP_TCP_GetIsReady(sktHTTP) + TCPIP_TCP_FifoRxFreeGet(sktHTTP))
    {   // Configuration Failure
        TCPIP_HTTP_CurrentConnectionStatusSet(connHandle, HTTP_REDIRECT);
        return HTTP_IO_DONE;
    }
    
    // Ensure that all data is waiting to be parsed.  If not, keep waiting for
    // all of it to arrive.
    if(TCPIP_TCP_GetIsReady(sktHTTP) < byteCount)
        return HTTP_IO_NEED_DATA;

    // Read all browser POST data
    TCPIP_TCP_ArrayGet(sktHTTP,(uint8_t*)httpDataBuff,256);
          
     
    if(!strncmp((const char *)httpDataBuff+Index, (const char *)"devmode=",strlen("devmode=")))
    {
        Index += strlen("devmode=");
        wifiConfig.mode = CONVERT_TO_NUMBER(httpDataBuff[Index]);
    }
    Index += 2;
    
    if(!strncmp((char *)httpDataBuff+Index, (const char *)"saveconfig=",strlen("saveconfig=")))
    {
        Index += strlen("saveconfig=");
        wifiConfig.saveConfig = CONVERT_TO_NUMBER(httpDataBuff[Index]);        
    }
    Index += 2;

    if(!strncmp((char *)httpDataBuff+Index, (const char *)"countrycode=",strlen("countrycode=")))
    {
        uint8_t country_code ;
        Index += strlen("countrycode=");
        country_code = CONVERT_TO_NUMBER(httpDataBuff[Index]);
        switch(country_code)
        {
            case 0: //GEN
            {
                strcpy((char *)wifiConfig.countryCode,(const char *)"GEN");
                break;
            }
            case 1: //USA
            {
                strcpy((char *)wifiConfig.countryCode,(const char *)"USA");
                break;
            }
            case 2: //EMEA
            {
                strcpy((char *)wifiConfig.countryCode,(const char *)"EMEA");
                break;
            }
            case 3: //CUST1
            {
                strcpy((char *)wifiConfig.countryCode,(const char *)"CUST1");
                break;
            }
            case 4: //CUST2
            {
                strcpy((char *)wifiConfig.countryCode,(const char *)"CUST2");
                break;
            }
        }
     }
<#if SYS_WIFIPROV_STA_ENABLE == true>
     Index += 2 ;
     
    if(!strncmp((char *)httpDataBuff+Index, (const char *)"stassid=",strlen("stassid=")))
    {

        uint8_t len ;
        Index += strlen("stassid=");
        len = Findstring_lenth(httpDataBuff+Index);
        strncpy((char *)wifiConfig.staConfig.ssid,(const char *)httpDataBuff+Index,len);
        wifiConfig.staConfig.ssid[len] ='\0';
        
        Index += len ;
    }
     Index += 1;

    if(!strncmp((char *)httpDataBuff+Index, (const char *)"stapwd=",strlen("stapwd=")))
    {
        uint8_t len ;
        Index += strlen("stapwd=");
        len = Findstring_lenth(httpDataBuff+Index);
        strncpy((char *)wifiConfig.staConfig.psk,(const char *)httpDataBuff+Index,len);
        wifiConfig.staConfig.psk[len] ='\0';
        
        Index += len ;
    }
     Index += 1;


    if(!strncmp((char *)httpDataBuff+Index, (const char *)"staauth=",strlen("staauth=")))
    {

        Index += strlen("staauth=");
        wifiConfig.staConfig.authType = CONVERT_TO_NUMBER(httpDataBuff[Index]);
        
    }
     Index += 2;

    if(!strncmp((char *)httpDataBuff+Index, (const char *)"stach=",strlen("stach=")))
    {

        Index += strlen("stach=");
        wifiConfig.staConfig.channel =CONVERT_TO_NUMBER(httpDataBuff[Index]);
    }
          Index += 2;
    if(!strncmp((char *)httpDataBuff+Index, (const char *)"staauto=",strlen("staauto=")))
    {

        Index += strlen("staauto=");
        wifiConfig.staConfig.autoConnect = CONVERT_TO_NUMBER(httpDataBuff[Index]);
    }
</#if>
<#if SYS_WIFIPROV_AP_ENABLE == true>
     Index += 2;
    if(!strncmp((char *)httpDataBuff+Index, (const char *)"apssid=",strlen("apssid=")))
    {
        uint8_t len ;
        Index += strlen("apssid=");
        len = Findstring_lenth(httpDataBuff+Index);
        strncpy((char *)wifiConfig.apConfig.ssid,(const char *)httpDataBuff+Index,len);
        wifiConfig.apConfig.ssid[len] ='\0';
        Index += len;
    }
     Index += 1;
    if(!strncmp((char *)httpDataBuff+Index, (const char *)"appwd=",strlen("appwd=")))
    {
        uint8_t len ;
        Index += strlen("appwd=");
        len = Findstring_lenth(httpDataBuff+Index);
        strncpy((char *)wifiConfig.apConfig.psk,(const char *)httpDataBuff+Index,len);
        wifiConfig.apConfig.psk[len] ='\0';
        Index += len;
    }
     Index += 1;

    if(!strncmp((char *)httpDataBuff+Index, (const char *)"apauth=",strlen("apauth=")))
    {

        Index += strlen("apauth=");
        wifiConfig.apConfig.authType = CONVERT_TO_NUMBER(httpDataBuff[Index]);

    }
     Index += 2;

    if(!strncmp((char *)httpDataBuff+Index, (const char *)"apch=",strlen("apch=")))
    {

        Index += strlen("apch=");
        wifiConfig.apConfig.channel =CONVERT_TO_NUMBER(httpDataBuff[Index]);    
    }
     Index += 2;

    if(!strncmp((char *)httpDataBuff+Index, (const char *)"ssidv=",strlen("ssidv=")))
    {

        Index += strlen("ssidv=");
        wifiConfig.apConfig.ssidVisibility = CONVERT_TO_NUMBER(httpDataBuff[Index]);
    }
</#if> 
    SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_CONNECT, &wifiConfig, sizeof(SYS_WIFI_CONFIG));
    return HTTP_IO_DONE;
}

/****************************************************************************
  Section:
    Dynamic Variable Callback Functions
 ****************************************************************************/

/****************************************************************************
  Function:
    void TCPIP_HTTP_Print_varname(void)

  Internal:
    See documentation in the TCP/IP Stack APIs or http.h for details.
 ****************************************************************************/
void TCPIP_HTTP_Print_hellomsg(HTTP_CONN_HANDLE connHandle)
{
    const uint8_t *ptr;
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);

    ptr = TCPIP_HTTP_ArgGet(TCPIP_HTTP_CurrentConnectionDataBufferGet(connHandle), (const uint8_t *)"name");
    // We omit checking for space because this is the only data being written
    if(ptr != NULL)
    {
        TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"Hello, ");
        TCPIP_TCP_StringPut(sktHTTP, ptr);
    }
}

void TCPIP_HTTP_Print_builddate(HTTP_CONN_HANDLE connHandle)
{
    TCP_SOCKET sktHTTP;
    sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);

    TCPIP_HTTP_CurrentConnectionCallbackPosSet(connHandle, 0x01);
    if(TCPIP_TCP_PutIsReady(sktHTTP) < strlen((const char *)__DATE__" "__TIME__))
        return;

    TCPIP_HTTP_CurrentConnectionCallbackPosSet(connHandle, 0x00);
    TCPIP_TCP_StringPut(sktHTTP, (const void *)__DATE__" "__TIME__);
}

void TCPIP_HTTP_Print_version(HTTP_CONN_HANDLE connHandle)
{    
    SYS_WIFI_CtrlMsg(sysObj.syswifi, SYS_WIFI_GETCONFIG, &wifiConfig, sizeof(SYS_WIFI_CONFIG));
    TCPIP_TCP_StringPut(TCPIP_HTTP_CurrentConnectionSocketGet(connHandle), (const void *)TCPIP_STACK_VERSION_STR);
}

void TCPIP_HTTP_Print_pot(HTTP_CONN_HANDLE connHandle)
{
    uint8_t AN0String[8];
    uint16_t ADval;

    ADval = (uint16_t)SYS_RANDOM_PseudoGet();
    uitoa(ADval, (uint8_t *)AN0String);
    TCPIP_TCP_StringPut(TCPIP_HTTP_CurrentConnectionSocketGet(connHandle), AN0String);
}

void TCPIP_HTTP_Print_config_hostname(HTTP_CONN_HANDLE connHandle)
{
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_TCP_StringPut(sktHTTP, (uint8_t *)TCPIP_STACK_NetBIOSName(TCPIP_TCP_SocketNetGet(sktHTTP)));
}

void TCPIP_HTTP_Print_config_ip(HTTP_CONN_HANDLE connHandle)
{
    IPV4_ADDR ipAddress;
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_NET_HANDLE netH = TCPIP_TCP_SocketNetGet(sktHTTP);

    ipAddress.Val = TCPIP_STACK_NetAddress(netH);
    if (TCPIP_Helper_IPAddressToString(&ipAddress, (char *)s_buf_ipv4addr, HTTP_APP_IPV4_ADDRESS_BUFFER_SIZE))
    {
        TCPIP_TCP_StringPut(sktHTTP, s_buf_ipv4addr);
    }
}

void TCPIP_HTTP_Print_config_gw(HTTP_CONN_HANDLE connHandle) // gateway
{
    IPV4_ADDR gwAddress;
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_NET_HANDLE netH = TCPIP_TCP_SocketNetGet(sktHTTP);

    gwAddress.Val = TCPIP_STACK_NetAddressGateway(netH);
    if (TCPIP_Helper_IPAddressToString(&gwAddress, (char *)s_buf_ipv4addr, HTTP_APP_IPV4_ADDRESS_BUFFER_SIZE))
    {
        TCPIP_TCP_StringPut(sktHTTP, s_buf_ipv4addr);
    }
}

void TCPIP_HTTP_Print_config_subnet(HTTP_CONN_HANDLE connHandle)
{
    IPV4_ADDR ipMask;
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_NET_HANDLE netH = TCPIP_TCP_SocketNetGet(sktHTTP);

    ipMask.Val = TCPIP_STACK_NetMask(netH);
    if (TCPIP_Helper_IPAddressToString(&ipMask, (char *)s_buf_ipv4addr, HTTP_APP_IPV4_ADDRESS_BUFFER_SIZE))
    {
        TCPIP_TCP_StringPut(sktHTTP, s_buf_ipv4addr);
    }
}

void TCPIP_HTTP_Print_config_dns1(HTTP_CONN_HANDLE connHandle)
{
    IPV4_ADDR priDnsAddr;
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_NET_HANDLE netH = TCPIP_TCP_SocketNetGet(sktHTTP);

    priDnsAddr.Val = TCPIP_STACK_NetAddressDnsPrimary(netH);
    if (TCPIP_Helper_IPAddressToString(&priDnsAddr, (char *)s_buf_ipv4addr, HTTP_APP_IPV4_ADDRESS_BUFFER_SIZE))
    {
        TCPIP_TCP_StringPut(sktHTTP, s_buf_ipv4addr);
    }
}

void TCPIP_HTTP_Print_config_dns2(HTTP_CONN_HANDLE connHandle)
{
    IPV4_ADDR secondDnsAddr;
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_NET_HANDLE netH = TCPIP_TCP_SocketNetGet(sktHTTP);

    secondDnsAddr.Val = TCPIP_STACK_NetAddressDnsSecond(netH);
    if (TCPIP_Helper_IPAddressToString(&secondDnsAddr, (char *)s_buf_ipv4addr, HTTP_APP_IPV4_ADDRESS_BUFFER_SIZE))
    {
        TCPIP_TCP_StringPut(sktHTTP, s_buf_ipv4addr);
    }
}

void TCPIP_HTTP_Print_config_mac(HTTP_CONN_HANDLE connHandle)
{
    uint8_t i;
    TCP_SOCKET sktHTTP;
    TCPIP_NET_HANDLE hNet;
    const uint8_t *pMacAdd;

    sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);

    if(TCPIP_TCP_PutIsReady(sktHTTP) < 18u)
    {   // need 17 bytes to write a MAC
        TCPIP_HTTP_CurrentConnectionCallbackPosSet(connHandle, 0x01);
        return;
    }

    hNet = TCPIP_TCP_SocketNetGet(sktHTTP);
    pMacAdd = TCPIP_STACK_NetAddressMac(hNet);
    // Write each byte
    for(i = 0; i < 6u; ++i)
    {
        if(i)
            TCPIP_TCP_Put(sktHTTP, ':');
        TCPIP_TCP_Put(sktHTTP, btohexa_high(pMacAdd[i]));
        TCPIP_TCP_Put(sktHTTP, btohexa_low(pMacAdd[i]));
    }
    // Indicate that we're done
    TCPIP_HTTP_CurrentConnectionCallbackPosSet(connHandle, 0x00);
    return;
}

void TCPIP_HTTP_Print_config_devmode(HTTP_CONN_HANDLE connHandle,uint16_t val)
{
    
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    //TCPIP_TCP_Put(sktHTTP, CONVERT_TO_ASCII(wifiConfig.mode));
    if(val == wifiConfig.mode)
    {
        TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");
    }
}
void TCPIP_HTTP_Print_config_saveconfig(HTTP_CONN_HANDLE connHandle,uint16_t val)
{
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    if(val == wifiConfig.saveConfig)
    {
        TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");
    }
}
void TCPIP_HTTP_Print_config_countrycode(HTTP_CONN_HANDLE connHandle,uint16_t val)
{
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    switch(val)
    {
        case 0: //GEN
        {
            if(!strcmp((const char *)wifiConfig.countryCode,"GEN"))
            {
                TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");            
            }
            break; 
        }
        case 1:  //USA
        {
            if(!strcmp((const char *)wifiConfig.countryCode,"USA"))
            {
                TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");            
            }
            break;
        }
        case 2:  //EMEA
        {
            if(!strcmp((const char *)wifiConfig.countryCode,"EMEA"))
            {
                TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");            
            }
            break;
        }
        case 3:  //CUST1
        {
            if(!strcmp((const char *)wifiConfig.countryCode,"CUST1"))
            {
                TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");            
            }
            break;
        }
        case 4:  //CUST2
        {
            if(!strcmp((const char *)wifiConfig.countryCode,"CUST2"))
            {
                TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");            
            }
            break;
        }
    }
}
void TCPIP_HTTP_Print_config_stassid(HTTP_CONN_HANDLE connHandle)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)wifiConfig.staConfig.ssid);
</#if>
}
void TCPIP_HTTP_Print_config_stapwd(HTTP_CONN_HANDLE connHandle)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_TCP_StringPut(sktHTTP,(const uint8_t *)wifiConfig.staConfig.psk);
</#if> 
}
void TCPIP_HTTP_Print_config_stachannel(HTTP_CONN_HANDLE connHandle)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_TCP_Put(sktHTTP,CONVERT_TO_ASCII(wifiConfig.staConfig.channel));
</#if>
}
void TCPIP_HTTP_Print_config_autoconnect(HTTP_CONN_HANDLE connHandle,uint16_t val)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    if(val == wifiConfig.staConfig.autoConnect)
        TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");
</#if>
}

void TCPIP_HTTP_Print_config_staauth(HTTP_CONN_HANDLE connHandle,uint16_t val)
{
<#if SYS_WIFIPROV_STA_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    if(val == wifiConfig.staConfig.authType)
        TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");
</#if> 

}
void TCPIP_HTTP_Print_config_apssid(HTTP_CONN_HANDLE connHandle)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)wifiConfig.apConfig.ssid);

</#if>
}

void TCPIP_HTTP_Print_config_appwd(HTTP_CONN_HANDLE connHandle)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)wifiConfig.apConfig.psk);
</#if>
}
void TCPIP_HTTP_Print_config_apauth(HTTP_CONN_HANDLE connHandle,uint16_t val)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    if(val == wifiConfig.apConfig.authType)
        TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");
</#if>
}
void TCPIP_HTTP_Print_config_apchannel(HTTP_CONN_HANDLE connHandle)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    TCPIP_TCP_Put(sktHTTP,CONVERT_TO_ASCII(wifiConfig.apConfig.channel));
</#if>
}
void TCPIP_HTTP_Print_config_ssidv(HTTP_CONN_HANDLE connHandle,uint16_t val)
{
<#if SYS_WIFIPROV_AP_ENABLE == true>
    TCP_SOCKET sktHTTP = TCPIP_HTTP_CurrentConnectionSocketGet(connHandle);
    if(val == wifiConfig.apConfig.ssidVisibility)
        TCPIP_TCP_StringPut(sktHTTP, (const uint8_t *)"SELECTED");
</#if>
}
#endif // #if defined(TCPIP_STACK_USE_HTTP_SERVER)
