/*******************************************************************************
  Wi-Fi System Service Implementation

  File Name:
    sys_wifi.c

  Summary:
    Source code for the Wi-Fi system service implementation.

  Description:
    This file contains the source code for the Wi-Fi system service
    implementation.
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

#include <stdlib.h>
#include "definitions.h"
#include "wdrv_pic32mzw_client_api.h"
#include "tcpip_manager_control.h"
#include "system/wifi/sys_wifi.h"
#include "configuration.h"
<#if SYS_WIFI_PROVISION_ENABLE == true>
#include "system/wifiprov/sys_wifiprov.h"
</#if>
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

typedef struct 
{
    /* The WiFi service current status */
    SYS_WIFI_STATUS wifiSrvcStatus;
    
    /* Wi-Fi Service handle */
    DRV_HANDLE wifiSrvcDrvHdl;
    
    /* Wi-Fi Service Auth Context */
    WDRV_PIC32MZW_AUTH_CONTEXT wifiSrvcAuthCtx;
    
    /* Wi-Fi Service BSS Context */
    WDRV_PIC32MZW_BSS_CONTEXT wifiSrvcBssCtx;

} SYS_WIFI_OBJ; /* Wi-Fi System Service Object */

/* Wi-Fi STA Mode, maximum auto connect retry */
#define MAX_AUTO_CONNECT_RETRY                5

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* Storing Wi-Fi Service Callbacks */
static    SYS_WIFI_CALLBACK     g_wifiSrvcCallBack[SYS_WIFI_MAX_CBS];

/* Wi-Fi Service Object */
static    SYS_WIFI_OBJ          g_wifiSrvcObj = {SYS_WIFI_STATUS_NONE,NULL};

/* Wi-Fi Driver ASSOC Handle */
static WDRV_PIC32MZW_ASSOC_HANDLE g_wifiSrvcDrvAssocHdl = WDRV_PIC32MZW_ASSOC_HANDLE_INVALID;
<#if SYS_WIFI_AP_ENABLE == true>

/* In Ap mode, Connected STA MAC Address */
static WDRV_PIC32MZW_MAC_ADDR   g_wifiSrvcApBssId;

/* In Ap mode, Connection status */
static    bool                  g_wifiSrvcApConnectStatus;
</#if>
<#if  SYS_WIFI_STA_ENABLE == true>

/* Wi-Fi DHCP handler */
static    TCPIP_DHCP_HANDLE     g_wifiSrvcDhcpHdl = NULL;

/* Wi-Fi STA Mode, Auto connect retry count */
static    uint32_t              g_wifiSrvcAutoConnectRetry = 0;
</#if>

/* Wi-Fi  Service Configuration Structure */
static    SYS_WIFI_CONFIG       g_wifiSrvcConfig;

/* Wi-Fi Service cookie */
static    void *                g_wifiSrvcCookie ;

/* Wi-Fi Service init enable wait status */
static    bool                  g_wifiSrvcInit = false;

<#if SYS_WIFI_PROVISION_ENABLE == true>
/* Cookie for Wi-Fi Provision */
static    char                  g_wifiSrvcProvCookieVal = 100;

/* Wi-Fi Provision Service Object */
static    SYS_MODULE_OBJ        g_wifiSrvcProvObj;
</#if>

/* Semaphore for Critical Section */
static    OSAL_SEM_HANDLE_TYPE  g_wifiSrvcSemaphore;
// *****************************************************************************

<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
/* Wi-Fi service APP debug handle */
SYS_MODULE_OBJ                  g_wifiSrvcAppDebugHdl = SYS_MODULE_OBJ_INVALID;

/* APP debug configuration handle */
SYS_APPDEBUG_CONFIG                       g_wifiSrvcAppDbgCfg;

/* App Debug Print Flows */
#define    WIFI_CFG                       0x1
#define    WIFI_CONNECT                   0x2
#define    WIFI_PROVISIONING              0x3
#define    WIFI_PROVISIONINGCMD           0x4
#define    WIFI_PROVISIONINGSOCKET        0x5

</#if>
// *****************************************************************************

static     uint8_t               SYS_WIFI_DisConnect(void);
static     SYS_WIFI_RESULT       SYS_WIFI_ConnectReq(void);
static     SYS_WIFI_RESULT       SYS_WIFI_SetScan(uint8_t channel,bool active);


<#if SYS_WIFI_PROVISION_ENABLE == true>
static void  SYS_WIFI_WIFIPROVCallBack(uint32_t event, void * data,void *cookie);
</#if>


// *****************************************************************************
// *****************************************************************************
// Section: Local Functions
// *****************************************************************************
// *****************************************************************************

static inline void SYS_WIFI_CallBackFun
(
    uint32_t event, 
    void * data, void *cookie
) 
{
    uint8_t idx;
    
    for (idx = 0; idx < SYS_WIFI_MAX_CBS; idx++) 
    {
        if (g_wifiSrvcCallBack[idx]) 
        {  
            /* Call client register functions */
            (g_wifiSrvcCallBack[idx])(event, data, cookie);
        }
    }
}

static inline SYS_WIFI_RESULT SYS_WIFI_REGCB
(
    SYS_WIFI_CALLBACK callback
) 
{
    SYS_WIFI_RESULT ret = SYS_WIFI_FAILURE;
    uint8_t idx;
    
    for (idx = 0; idx < SYS_WIFI_MAX_CBS; idx++) 
    {
        if (!g_wifiSrvcCallBack[idx]) 
        {
            /* Copy the client function pointer */
            g_wifiSrvcCallBack[idx] = callback;
            ret = SYS_WIFI_SUCCESS;
            break;
        }
    }
    return ret;
}

<#if SYS_WIFI_PROVISION_ENABLE == false>
static inline void SYS_WIFI_InitConfig
(
    SYS_WIFI_CONFIG *config
) 
{   
    
    if (!config) 
    {   
        /* User has not set configuration during Wi-Fi Service initialization,
           Copy the MHC configuration into Wi-Fi Service structure */
        g_wifiSrvcConfig.mode = SYS_WIFI_DEVMODE;
        g_wifiSrvcConfig.saveConfig = 0;
        memcpy(g_wifiSrvcConfig.countrycode, SYS_WIFI_COUNTRYCODE, strlen(SYS_WIFI_COUNTRYCODE));
<#if SYS_WIFI_STA_ENABLE == true>

        /* STA Mode Configuration */
        g_wifiSrvcConfig.staConfig.channel = 0;        
        g_wifiSrvcConfig.staConfig.autoConnect = SYS_WIFI_STA_AUTOCONNECT;
        g_wifiSrvcConfig.staConfig.authType = SYS_WIFI_STA_AUTHTYPE;
        memcpy(g_wifiSrvcConfig.staConfig.ssid, SYS_WIFI_STA_SSID, sizeof(SYS_WIFI_STA_SSID));        
        memcpy(g_wifiSrvcConfig.staConfig.psk, SYS_WIFI_STA_PWD, sizeof(SYS_WIFI_STA_PWD));
</#if>
<#if SYS_WIFI_AP_ENABLE == true>

        /* AP Mode Configuration */
        g_wifiSrvcConfig.apConfig.channel = SYS_WIFI_AP_CHANNEL;
        g_wifiSrvcConfig.apConfig.ssidVisibility = SYS_WIFI_AP_SSIDVISIBILE;
        g_wifiSrvcConfig.apConfig.authType = SYS_WIFI_AP_AUTHTYPE;
        memcpy(g_wifiSrvcConfig.apConfig.ssid, SYS_WIFI_AP_SSID, sizeof(SYS_WIFI_AP_SSID));
        memcpy(g_wifiSrvcConfig.apConfig.psk, SYS_WIFI_AP_PWD, sizeof(SYS_WIFI_AP_PWD));     
</#if>
    } 
    else 
    {
        /* User has set configuration during Wi-Fi Service initialization,
           Copy the user configuration into Wi-Fi Service structure  */
        memcpy(&g_wifiSrvcConfig, config, sizeof(SYS_WIFI_CONFIG));
    }
}
</#if>
 
static inline void SYS_WIFI_SetCookie
(
    void *cookie
) 
{
    g_wifiSrvcCookie = cookie;
}

static inline SYS_WIFI_MODE SYS_WIFI_GetMode(void)
{
    return g_wifiSrvcConfig.mode;
}

static inline bool SYS_WIFI_GetSaveConfig(void)
{
    return g_wifiSrvcConfig.saveConfig;
}

static inline uint8_t * SYS_WIFI_GetSSID(void)  
{
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    if (SYS_WIFI_STA == SYS_WIFI_GetMode())
    {
        return g_wifiSrvcConfig.staConfig.ssid;
    }
    else
    {
        return g_wifiSrvcConfig.apConfig.ssid;
    }
<#elseif SYS_WIFI_STA_ENABLE == true>
    return g_wifiSrvcConfig.staConfig.ssid;
<#elseif SYS_WIFI_AP_ENABLE == true>
    return g_wifiSrvcConfig.apConfig.ssid;
</#if>
}

static inline uint8_t SYS_WIFI_GetSSIDLen(void)  
{
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    if (SYS_WIFI_STA == SYS_WIFI_GetMode())
    {
        return strlen((const char *)g_wifiSrvcConfig.staConfig.ssid);
    }
    else
    {
        return strlen((const char *)g_wifiSrvcConfig.apConfig.ssid);
    }
<#elseif SYS_WIFI_STA_ENABLE == true>
    return strlen((const char *)g_wifiSrvcConfig.staConfig.ssid);
<#elseif SYS_WIFI_AP_ENABLE == true>
    return strlen((const char *)g_wifiSrvcConfig.apConfig.ssid);
</#if>
}

static inline uint8_t SYS_WIFI_GetChannel(void)
{
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    if (SYS_WIFI_STA == SYS_WIFI_GetMode())
    {
        return g_wifiSrvcConfig.staConfig.channel;
    }
    else
    {
        return g_wifiSrvcConfig.apConfig.channel;
    }
<#elseif SYS_WIFI_STA_ENABLE == true>
    return (g_wifiSrvcConfig.staConfig.channel);
<#elseif SYS_WIFI_AP_ENABLE == true>
    return (g_wifiSrvcConfig.apConfig.channel);
</#if>
}

static inline uint8_t SYS_WIFI_GetAuthType(void)
{
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    if (SYS_WIFI_STA == SYS_WIFI_GetMode())
    {
        return g_wifiSrvcConfig.staConfig.authType;
    }
    else
    {
        return g_wifiSrvcConfig.apConfig.authType;
    }
<#elseif SYS_WIFI_STA_ENABLE == true>
    return g_wifiSrvcConfig.staConfig.authType;
<#elseif SYS_WIFI_AP_ENABLE == true>
    return g_wifiSrvcConfig.apConfig.authType;
</#if>
}

static inline uint8_t *SYS_WIFI_GetPsk(void)
{
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    if (SYS_WIFI_STA == SYS_WIFI_GetMode())
    {
        return g_wifiSrvcConfig.staConfig.psk;
    }
    else
    {
        return g_wifiSrvcConfig.apConfig.psk;
    }
<#elseif SYS_WIFI_STA_ENABLE == true>
    return g_wifiSrvcConfig.staConfig.psk;
<#elseif SYS_WIFI_AP_ENABLE == true>
    return g_wifiSrvcConfig.apConfig.psk;
</#if>
}

static inline uint8_t SYS_WIFI_GetPskLen(void)
{
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    if (SYS_WIFI_STA == SYS_WIFI_GetMode())
    {
        return strlen((const char *)g_wifiSrvcConfig.staConfig.psk);
    }
    else
    {
        return strlen((const char *)g_wifiSrvcConfig.apConfig.psk);
    }
<#elseif SYS_WIFI_STA_ENABLE == true>
    return strlen((const char *)g_wifiSrvcConfig.staConfig.psk);
<#elseif SYS_WIFI_AP_ENABLE == true>
    return strlen((const char *)g_wifiSrvcConfig.apConfig.psk);
</#if>    
}

static inline bool SYS_WIFI_GetSSIDVisibility(void)
{
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    if (SYS_WIFI_AP == SYS_WIFI_GetMode())
    {
        return g_wifiSrvcConfig.apConfig.ssidVisibility;
    }
    else
    {
        return 0;
    }
<#elseif SYS_WIFI_STA_ENABLE == true>
    return 0;
<#elseif SYS_WIFI_AP_ENABLE == true>
    return g_wifiSrvcConfig.apConfig.ssidVisibility;
</#if>     
}

static inline const char *SYS_WIFI_GetCountryCode(void)
{
    return (const char *) g_wifiSrvcConfig.countryCode;
}

static inline bool SYS_WIFI_GetAutoConnect(void)
{
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    
    /* In AP mode, autoConnect is always enabled */
    if (SYS_WIFI_AP == SYS_WIFI_GetMode())
    {
        return true;
    } 
    else if (SYS_WIFI_STA == SYS_WIFI_GetMode())
    {
        return g_wifiSrvcConfig.staConfig.autoConnect;
    }
    else
    {
        return false;
    }

<#elseif SYS_WIFI_STA_ENABLE == true>
    return g_wifiSrvcConfig.staConfig.autoConnect;        
<#elseif SYS_WIFI_AP_ENABLE == true>
    
    /* In AP mode, autoConnect is always enabled */
    return true;
</#if>
}

static inline void SYS_WIFI_SetTaskstatus
(
    SYS_WIFI_STATUS status
)
{
    g_wifiSrvcObj.wifiSrvcStatus = status;
}

static inline SYS_WIFI_STATUS SYS_WIFI_GetTaskstatus(void)
{
    return g_wifiSrvcObj.wifiSrvcStatus;
}

static inline void SYS_WIFI_PrintConfig(void)
{
    SYS_CONSOLE_PRINT("\r\n mode=%d (0-STA,1-AP) saveConfig=%d \r\n ", g_wifiSrvcConfig.mode, g_wifiSrvcConfig.saveConfig);
<#if SYS_WIFI_STA_ENABLE == true>
    if (g_wifiSrvcConfig.mode == SYS_WIFI_STA) 
    {
        SYS_CONSOLE_PRINT("\r\n STA Configuration :\r\n channel=%d \r\n autoConnect=%d \r\n ssid=%s \r\n passphase=%s \r\n authentication type=%d (1-Open,2-WEP,3-Mixed mode(WPA/WPA2),4-WPA2,5-Mixed mode(WPA2/WPA3),6-WPA3)\r\n", g_wifiSrvcConfig.staConfig.channel, g_wifiSrvcConfig.staConfig.autoConnect, g_wifiSrvcConfig.staConfig.ssid, g_wifiSrvcConfig.staConfig.psk, g_wifiSrvcConfig.staConfig.authType);
    }
</#if>
<#if SYS_WIFI_AP_ENABLE == true>
    if (g_wifiSrvcConfig.mode == SYS_WIFI_AP)
    {
        SYS_CONSOLE_PRINT("\r\n AP Configuration :\r\n channel=%d \r\n ssidVisibility=%d \r\n ssid=%s \r\n passphase=%s \r\n authentication type=%d (1-Open,2-WEP,3-Mixed mode(WPA/WPA2),4-WPA2,5-Mixed mode(WPA2/WPA3),6-WPA3) \r\n", g_wifiSrvcConfig.apConfig.channel, g_wifiSrvcConfig.apConfig.ssidVisibility, g_wifiSrvcConfig.apConfig.ssid, g_wifiSrvcConfig.apConfig.psk, g_wifiSrvcConfig.apConfig.authType);
    }
</#if>

}

<#if SYS_WIFI_AP_ENABLE == true>
static void SYS_WIFI_APConnCallBack
(
    DRV_HANDLE handle, 
    WDRV_PIC32MZW_ASSOC_HANDLE assocHandle, 
    WDRV_PIC32MZW_CONN_STATE currentState
) 
{
    switch (currentState)
    {
        case WDRV_PIC32MZW_CONN_STATE_CONNECTED:
        {
            /* When STA connected to PIC32MZW1 AP, 
               Wi-Fi driver updates Connected event */
            if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_AssocPeerAddressGet(assocHandle, &g_wifiSrvcApBssId)) 
            {
                g_wifiSrvcApConnectStatus = true;

                /* Updating Wi-Fi service associate handle on receiving 
                   driver Connection event */
                g_wifiSrvcDrvAssocHdl = assocHandle;
                SYS_CONSOLE_PRINT("Connected STA MAC Address=%x:%x:%x:%x:%x:%x\r\n", g_wifiSrvcApBssId.addr[0], g_wifiSrvcApBssId.addr[1], g_wifiSrvcApBssId.addr[2], g_wifiSrvcApBssId.addr[3], g_wifiSrvcApBssId.addr[4], g_wifiSrvcApBssId.addr[5]);

                /* When STA connected to PIC32MZW1 AP, Set the state to wait for 
                   IP to updated the connected STA IP address */
                SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_WAIT_FOR_STA_IP);
            }
            break;
        }

        case WDRV_PIC32MZW_CONN_STATE_DISCONNECTED: 
        {
            /* When STA Disconnect from PIC32MZW1 AP, 
               Wi-Fi driver updates disconnect event */
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, " Connected STA is disconnected\r\n");
</#if>
            if (true == g_wifiSrvcApConnectStatus) 
            {
                /* Updating Wi-Fi service Associate handle on receiving 
                   driver disconnection event */
                g_wifiSrvcDrvAssocHdl = WDRV_PIC32MZW_ASSOC_HANDLE_INVALID;
                g_wifiSrvcApConnectStatus = false;
                
                /* Update the application on receiving Disconnect event */
                SYS_WIFI_CallBackFun(SYS_WIFI_DISCONNECT, NULL, g_wifiSrvcCookie);
            }
            break;
        }

        default:
        {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, " AP Callback received with invalid state\r\n");
</#if>
            break;
        }

    }

}

</#if>
<#if SYS_WIFI_STA_ENABLE == true>
static void SYS_WIFI_STAConnCallBack
(
    DRV_HANDLE handle, 
    WDRV_PIC32MZW_ASSOC_HANDLE assocHandle,
    WDRV_PIC32MZW_CONN_STATE currentState
) 
{
    switch (currentState) 
    {
        case WDRV_PIC32MZW_CONN_STATE_CONNECTED:
        {
            /* When HOMEAP connect to PIC32MZW1 STA, Wi-Fi driver updated connected event */
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "STA Connected\r\n");
</#if>
            /* Updating Wi-Fi service associate handle on receiving driver 
               connection event */
            g_wifiSrvcDrvAssocHdl = assocHandle;
            g_wifiSrvcAutoConnectRetry = 0;
            break;
        }

        case WDRV_PIC32MZW_CONN_STATE_FAILED:
        {
            /* When user provided HOMEAP configuration is not matching with near 
               by available HOMEAPs,Wi-Fi driver updated event Fail. */
            SYS_CONSOLE_PRINT(" Trying to connect to SSID : %s \r\n STA Connection failed. \r\n \r\n", SYS_WIFI_GetSSID());
            
            /* check user has enable the Auto connect feature in the STA
               mode,Auto connect Retry count is less then user configured 
               auto connect retry then request Wi-Fi driver for connection request */
            if ((true == SYS_WIFI_GetAutoConnect()) && (g_wifiSrvcAutoConnectRetry < MAX_AUTO_CONNECT_RETRY)) 
            {
                SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_CONNECT_REQ);
                g_wifiSrvcAutoConnectRetry++;
            } 
            else if (g_wifiSrvcAutoConnectRetry == MAX_AUTO_CONNECT_RETRY) 
            {
                /* Auto connect Retry count is equal to Maximum connection attempt,
                   then set the Wi-Fi Service status to connection error and wait 
                   for user to re-configuration. */
                SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_CONNECT_ERROR);
            }
            g_wifiSrvcDrvAssocHdl = WDRV_PIC32MZW_ASSOC_HANDLE_INVALID;
            break;
        }

        case WDRV_PIC32MZW_CONN_STATE_DISCONNECTED:
        {
            /* when PIC32MZW1 STA disconnected from connected HOMEAP,Wi-Fi driver 
               updated event disconnected. */
            SYS_CONSOLE_PRINT("STA DisConnected\r\n");
            if (true == SYS_WIFI_GetAutoConnect()) 
            {
                SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_CONNECT_REQ);
            }

            /* Updating Wi-Fi service associate handle on receiving disconnection
               event */
            g_wifiSrvcDrvAssocHdl = WDRV_PIC32MZW_ASSOC_HANDLE_INVALID;

            /* Reset the Auto Connection retry to zero on receiving disconnect 
               event */
            g_wifiSrvcAutoConnectRetry = 0;
            break;
        }

        /*case WDRV_PIC32MZW_CONN_STATE_CONNECTING:
            break;*/
        default:
        {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "STA Callback received with invalid state\r\n");
</#if>
            break;
        }
    }       
}
</#if>

/* Wi-Fi driver callback received on setting Regulatory domain */
static void SYS_WIFI_SetRegDomainCallback
(
    DRV_HANDLE handle, 
    uint8_t index, 
    uint8_t ofTotal, 
    bool isCurrent, 
    const WDRV_PIC32MZW_REGDOMAIN_INFO * const pRegDomInfo
) 
{
    if ((1 != index) || (1 != ofTotal) || (false == isCurrent) || (NULL == pRegDomInfo)) 
    {
        SYS_CONSOLE_MESSAGE("Regulatory domain set unsuccessful\r\n");
    } 
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    else 
    {
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Regulatory domain set successful\r\n");
    }
</#if>
}

/* Wi-Fi driver update the Scan result on callback*/
static void SYS_WIFI_ScanHandler
(
    DRV_HANDLE handle, 
    uint8_t index, 
    uint8_t ofTotal, 
    WDRV_PIC32MZW_BSS_INFO *pBSSInfo
) 
{
    if (0 == ofTotal) 
    {
        SYS_CONSOLE_MESSAGE("No AP Found Rescan\r\n");
    } 
    else 
    {
        if (1 == index) 
        {
            char cmdTxt[8];
            sprintf(cmdTxt, "SCAN#%02d", ofTotal);
            SYS_CONSOLE_PRINT("#%02d\r\n", ofTotal);
            SYS_CONSOLE_MESSAGE(">>#  RI  Sec  Recommend CH BSSID             SSID\r\n");
            SYS_CONSOLE_MESSAGE(">>#      Cap  Auth Type\r\n>>#\r\n");
        }
        SYS_CONSOLE_PRINT(">>%02d %d 0x%02x ", index, pBSSInfo->rssi, pBSSInfo->secCapabilities);
        switch (pBSSInfo->authTypeRecommended) 
        {
            case WDRV_PIC32MZW_AUTH_TYPE_OPEN:
            {
                SYS_CONSOLE_MESSAGE("OPEN     ");
                break;
            }

            case WDRV_PIC32MZW_AUTH_TYPE_WEP:
            {
                SYS_CONSOLE_MESSAGE("WEP      ");
                break;
            }

            case WDRV_PIC32MZW_AUTH_TYPE_WPAWPA2_PERSONAL:
            {
                SYS_CONSOLE_MESSAGE("WPA/2 PSK");
                break;
            }

            case WDRV_PIC32MZW_AUTH_TYPE_WPA2_PERSONAL:
            {
                SYS_CONSOLE_MESSAGE("WPA2 PSK ");
                break;
            }

            default:
            {
                SYS_CONSOLE_MESSAGE("Not Avail");
                break;
            }

        }
        SYS_CONSOLE_PRINT(" %02d %02X:%02X:%02X:%02X:%02X:%02X %s\r\n", pBSSInfo->ctx.channel,
                pBSSInfo->ctx.bssid.addr[0], pBSSInfo->ctx.bssid.addr[1], pBSSInfo->ctx.bssid.addr[2],
                pBSSInfo->ctx.bssid.addr[3], pBSSInfo->ctx.bssid.addr[4], pBSSInfo->ctx.bssid.addr[5],
                pBSSInfo->ctx.ssid.name);
    }
}

<#if SYS_WIFI_STA_ENABLE == true>
<#if (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT)?has_content && (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT) == true>   
static void SYS_WIFI_TCPIP_DHCP_EventHandler
(
    TCPIP_NET_HANDLE hNet, 
    TCPIP_DHCP_EVENT_TYPE evType, 
    const void* param
) 
{
    IPV4_ADDR ipAddr;
    IPV4_ADDR gateWayAddr;
<#if SYS_WIFI_PROVISION_ENABLE == true>
    bool provConnStatus = false;
</#if>            
    switch (evType) 
    {
        case DHCP_EVENT_BOUND:
        {

            /* TCP/IP Stack BOUND event indicates
               PIC32MZW1 has received the IP address from connected HOMEAP */
            ipAddr.Val = TCPIP_STACK_NetAddress(hNet);
            if (ipAddr.Val) 
            {
                gateWayAddr.Val = TCPIP_STACK_NetAddressGateway(hNet);
                SYS_CONSOLE_PRINT("IP address obtained = %d.%d.%d.%d \r\n",
                        ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
                SYS_CONSOLE_PRINT("Gateway IP address = %d.%d.%d.%d \r\n",
                        gateWayAddr.v[0], gateWayAddr.v[1], gateWayAddr.v[2], gateWayAddr.v[3]);

                /* Update the application(client) on receiving IP address */
                SYS_WIFI_CallBackFun(SYS_WIFI_CONNECT, &ipAddr, g_wifiSrvcCookie);
<#if SYS_WIFI_PROVISION_ENABLE == true>
                provConnStatus = true;

                /* Update the Wi-Fi provisioning service on receiving the IP Address, 
                   The Wi-Fi provisioning service has to start the TCP server socket
                   when IP address is assigned from HOMEAP to STA.only applicable 
                   if user has enable TCP Socket configuration from MHC */
                SYS_WIFIPROV_CtrlMsg(g_wifiSrvcProvObj,SYS_WIFIPROV_CONNECT,&provConnStatus,sizeof(bool));
</#if>
            }
            break;
        }

        case DHCP_EVENT_CONN_ESTABLISHED:
        {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "connection to the DHCP server established \r\n");
</#if>
            break;
        }

        case DHCP_EVENT_CONN_LOST:
        {

            /* Received the TCP/IP Stack Connection Lost event,
               PIC32MZW1 has lost IP address from connected HOMEAP */
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "connection to the DHCP server lost \r\n");
</#if>

            /* Update the application(client) on lost IP address */
            SYS_WIFI_CallBackFun(SYS_WIFI_DISCONNECT,NULL,g_wifiSrvcCookie);
<#if SYS_WIFI_PROVISION_ENABLE == true>
            provConnStatus = false;

            /* Update the Wi-Fi provisioning service on lost IP Address, 
               The Wi-Fi provisioning service has to stop the TCP server socket.
               only applicable if user has enable TCP Socket configuration 
               from MHC */
            SYS_WIFIPROV_CtrlMsg(g_wifiSrvcProvObj,SYS_WIFIPROV_CONNECT,&provConnStatus,sizeof(bool));
</#if>
            break;
        }

        default:
        {
            break;
        }
    }
}
</#if>
</#if>

static SYS_WIFI_RESULT SYS_WIFI_SetScan
(
    uint8_t channel, 
    bool active
) 
{
    uint8_t ret = SYS_WIFI_FAILURE;
    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSFindFirst(g_wifiSrvcObj.wifiSrvcDrvHdl, channel, active, (WDRV_PIC32MZW_BSSFIND_NOTIFY_CALLBACK) SYS_WIFI_ScanHandler)) 
    {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, " Wi-Fi Scan request is successful with channel number:%d and scan type(active=true,passive=false):%d\r\n",channel,active);
</#if>
        ret = SYS_WIFI_SUCCESS ;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    } 
    else
    {
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, " Wi-Fi Scan request is unsuccessful with channel number:%d and scan type(active=true,passive=false):%d\r\n",channel,active);
</#if>
    }
    return ret;
}

static SYS_WIFI_RESULT SYS_WIFI_SetChannel(void)
{
    uint8_t ret = SYS_WIFI_FAILURE;
    uint8_t channel = SYS_WIFI_GetChannel();

    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSCtxSetChannel(&g_wifiSrvcObj.wifiSrvcBssCtx, channel)) 
    {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, " Wi-Fi channel request is successful with channel number:%d \r\n",channel);
</#if>
        ret = SYS_WIFI_SUCCESS;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    } 
    else
    {
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, " Wi-Fi channel request is unsuccessful with channel number:%d \r\n",channel);
</#if>
    }
    return ret;
}

static uint8_t SYS_WIFI_DisConnect(void)
{
    uint8_t ret = SYS_WIFI_FAILURE;

    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSDisconnect(g_wifiSrvcObj.wifiSrvcDrvHdl)) 
    {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, " Wi-Fi disconnect request is successful \r\n");
</#if>
        ret = SYS_WIFI_SUCCESS;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    }
    else
    {
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, " Wi-Fi channel request is unsuccessful \r\n");
</#if>
    }
    return ret;
}

static SYS_WIFI_RESULT SYS_WIFI_ConnectReq(void)
{
    SYS_WIFI_RESULT ret = SYS_WIFI_CONNECT_FAILURE;
    
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>
    SYS_WIFI_MODE devMode = SYS_WIFI_GetMode();
    if (SYS_WIFI_STA == devMode) 
    {
        if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSConnect(g_wifiSrvcObj.wifiSrvcDrvHdl, &g_wifiSrvcObj.wifiSrvcBssCtx, &g_wifiSrvcObj.wifiSrvcAuthCtx, SYS_WIFI_STAConnCallBack)) 
        {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "Wi-Fi Driver STA connect request is successful\r\n");
</#if>
            ret = SYS_WIFI_SUCCESS;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        }
        else
        {
            SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, " Wi-Fi Driver STA connect request is unsuccessful \r\n");
</#if>
        }
    } 
    else if (SYS_WIFI_AP == devMode)
    {
        if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_APStart(g_wifiSrvcObj.wifiSrvcDrvHdl, &g_wifiSrvcObj.wifiSrvcBssCtx, &g_wifiSrvcObj.wifiSrvcAuthCtx, SYS_WIFI_APConnCallBack)) 
        {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "Wi-Fi Driver AP start request is successful\r\n");
</#if>
            ret = SYS_WIFI_SUCCESS;        
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        } 
        else 
        {
            SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "Wi-Fi Driver AP start request is unsuccessful \r\n");
</#if>
        }
    }
<#elseif SYS_WIFI_STA_ENABLE == true>
    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSConnect(g_wifiSrvcObj.wifiSrvcDrvHdl, &g_wifiSrvcObj.wifiSrvcBssCtx, &g_wifiSrvcObj.wifiSrvcAuthCtx, SYS_WIFI_STAConnCallBack)) 
    {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "Wi-Fi Driver STA connect request is successful\r\n");
</#if>
        ret = SYS_WIFI_SUCCESS ;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    } 
    else 
    {
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, " Wi-Fi Driver STA connect request is unsuccessful \r\n");
</#if>
    }
<#elseif SYS_WIFI_AP_ENABLE == true>
    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_APStart(g_wifiSrvcObj.wifiSrvcDrvHdl, &g_wifiSrvcObj.wifiSrvcBssCtx, &g_wifiSrvcObj.wifiSrvcAuthCtx, SYS_WIFI_APConnCallBack)) 
    {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "Wi-Fi Driver AP start request is successful\r\n");
</#if>
        ret = SYS_WIFI_SUCCESS ;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    } 
    else 
    {
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CONNECT, "Wi-Fi Driver AP start request is unsuccessful \r\n");
</#if>
    }
</#if>
    
    if (SYS_WIFI_CONNECT_FAILURE == ret) 
    {
       SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_CONNECT_ERROR);	
    }
    return ret;
}

static SYS_WIFI_RESULT SYS_WIFI_SetSSID(void)
{
    SYS_WIFI_RESULT ret = SYS_WIFI_CONFIG_FAILURE;
    uint8_t * ssid = SYS_WIFI_GetSSID();
    uint8_t ssidLen = SYS_WIFI_GetSSIDLen();

    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSCtxSetSSID(&g_wifiSrvcObj.wifiSrvcBssCtx, ssid, ssidLen)) 
    {
<#if SYS_WIFI_AP_ENABLE == true>
        if (SYS_WIFI_AP == SYS_WIFI_GetMode()) 
        {
            WDRV_PIC32MZW_BSSCtxSetSSIDVisibility(&g_wifiSrvcObj.wifiSrvcBssCtx, SYS_WIFI_GetSSIDVisibility());
        }
</#if>
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Wi-Fi Driver SSID request is successful\r\n");
</#if>        
        ret = SYS_WIFI_SUCCESS;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    } 
    else 
    {
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Wi-Fi Driver SSID request is unsuccessful \r\n");
</#if>
    }
    return ret;
}

static SYS_WIFI_RESULT SYS_WIFI_ConfigReq(void)
{
    SYS_WIFI_RESULT ret = SYS_WIFI_SUCCESS;
    uint8_t authType = SYS_WIFI_GetAuthType();
    uint8_t * const pwd = SYS_WIFI_GetPsk();
    uint8_t pwdLen = SYS_WIFI_GetPskLen();

<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "authType=%d,password=%s,password len=%d\r\n",authType,pwd,pwdLen);
</#if>
    if (SYS_WIFI_SUCCESS == SYS_WIFI_SetSSID()) 
    {
        switch (authType) 
        {
            case SYS_WIFI_OPEN:
            {
                if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetOpen(&g_wifiSrvcObj.wifiSrvcAuthCtx)) 
                {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                    SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, " Unable to set Authentication to Open \r\n");
</#if>
                    ret = SYS_WIFI_CONFIG_FAILURE;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                } 
                else 
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "set Authentication to Open \r\n");
</#if>
                }
                break;
            }

            case SYS_WIFI_WPA2:
            {                
                if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiSrvcObj.wifiSrvcAuthCtx, pwd, pwdLen, WDRV_PIC32MZW_AUTH_TYPE_WPA2_PERSONAL)) 
                {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                    SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Unable to set authentication to WPA2 PSK\r\n");
</#if>
                    ret = SYS_WIFI_CONFIG_FAILURE;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                } 
                else 
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "set authentication to WPA2 PSK\r\n");
</#if>
                }
                break;
            }

            case SYS_WIFI_WPAWPA2MIXED:
            {
                if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiSrvcObj.wifiSrvcAuthCtx, pwd, pwdLen, WDRV_PIC32MZW_AUTH_TYPE_WPAWPA2_PERSONAL)) 
                {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                    SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Unable to set authentication to WPAWPA2 MIXED PSK\r\n");
</#if>
                    ret = SYS_WIFI_CONFIG_FAILURE;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                } 
                else
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "set authentication to WPAWPA2 MIXED PSK\r\n");
</#if>
                }
                break;
            }
<#if (drvWifiPic32mzw1.DRV_WIFI_PIC32MZW1_SUPPORT_WPA3)?has_content && (drvWifiPic32mzw1.DRV_WIFI_PIC32MZW1_SUPPORT_WPA3) == true>

            case SYS_WIFI_WPA2WPA3MIXED:
            {
                if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiSrvcObj.wifiSrvcAuthCtx, pwd, pwdLen, WDRV_PIC32MZW_AUTH_TYPE_WPA2WPA3_PERSONAL)) 
                {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                    SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Unable to set authentication to WPA2WPA3 MIXED \r\n");
</#if>
                    ret = SYS_WIFI_CONFIG_FAILURE;
                }
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                else
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "set authentication to WPA2WPA3 MIXED \r\n");
                }
</#if>
                break;
            }

            case SYS_WIFI_WPA3:
            {
                if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiSrvcObj.wifiSrvcAuthCtx, pwd, pwdLen, WDRV_PIC32MZW_AUTH_TYPE_WPA3_PERSONAL)) 
                {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                    SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Unable to set authentication to WPA3 PSK \r\n");
</#if>
                    ret = SYS_WIFI_CONFIG_FAILURE;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                } 
                else 
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "set authentication to WPA3 PSK \r\n");
</#if>
                }
                break;
            }
</#if>

            case SYS_WIFI_WEP:
            {
               ret = SYS_WIFI_CONFIG_FAILURE; 
              /* Wi-Fi service doesn't support WEP */
                break;
            }

            default:
            {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "set valid authentication type\r\n");
</#if>
                ret = SYS_WIFI_CONFIG_FAILURE;
            }
        }
    }

    if (SYS_WIFI_CONFIG_FAILURE == ret) 
    {
        SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_CONFIG_ERROR);
    }

    return ret;
}

static SYS_WIFI_RESULT SYS_WIFI_SetConfig
(
    SYS_WIFI_CONFIG *wifi_config, 
    SYS_WIFI_STATUS status
) 
{
<#if SYS_WIFI_PROVISION_ENABLE == true>
    /* When user has enabled Save config option,request Wi-Fi provisioning service 
       to store the configuration before making the connection request to user */
    if (true == SYS_WIFI_GetSaveConfig()) 
    {
        return SYS_WIFIPROV_CtrlMsg(g_wifiSrvcProvObj,SYS_WIFIPROV_SETCONFIG,wifi_config,sizeof(SYS_WIFI_CONFIG));
    } 
    else 
    {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Save configuration is disabled\r\n");
</#if>

        /*  When user has not enabled Save config option */
        SYS_WIFI_RESULT ret = SYS_WIFI_SUCCESS;

        /* Copy the user Wi-Fi configuration and make connection request */
        memcpy(&g_wifiSrvcConfig,wifi_config,sizeof(SYS_WIFI_CONFIG));
        SYS_WIFI_SetTaskstatus(status);
        return ret;
    }
<#else>

    /* When Wi-Fi provisioning service is not enable by user from MHC configuration */
    SYS_WIFI_RESULT ret = SYS_WIFI_SUCCESS;

    /* Copy the user Wi-Fi configuration and make connection request */
    memcpy(&g_wifiSrvcConfig,wifi_config,sizeof(SYS_WIFI_CONFIG));
    SYS_WIFI_SetTaskstatus(status);
    return ret;	
</#if>
}

static uint32_t SYS_WIFI_ExecuteBlock
(
    SYS_MODULE_OBJ object
) 
{
    SYS_STATUS                   tcpIpStat;
    static TCPIP_NET_HANDLE      netHdl;
    SYS_WIFI_OBJ *               wifiSrvcObj = (SYS_WIFI_OBJ *) object;
    uint8_t                      ret =  SYS_WIFIPROV_OBJ_INVALID;
<#if SYS_WIFI_AP_ENABLE == true>
    IPV4_ADDR                    apLastIp = {-1};
    IPV4_ADDR                    apIpAddr;
</#if>
 
    if (&g_wifiSrvcObj == (SYS_WIFI_OBJ*) wifiSrvcObj)
    {    
        switch (wifiSrvcObj->wifiSrvcStatus) 
        {
            case SYS_WIFI_STATUS_INIT:
            { 
                if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
                {
                    /* When g_wifiSrvcInit set to true :
                        Case 1: When Wi-Fi provisioning service is enabled by user in the MHC, 
                               and when Wi-Fi Configuration read from NVM 
                               using Wi-Fi provisioning is valid  
                    
                        Case 2: When Wi-Fi provisioning service is disabled by user in the MHC, 
                               g_wifiSrvcInit set true by Wi-Fi service initialization */

                    if (true == g_wifiSrvcInit) 
                    {
                        if (SYS_STATUS_READY == WDRV_PIC32MZW_Status(sysObj.drvWifiPIC32MZW1)) 
                        {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Wi-Fi Driver is ready\r\n");
</#if>
                            wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_WDRV_OPEN_REQ;                
                        }
                    }
                    OSAL_SEM_Post(&g_wifiSrvcSemaphore);
                }
                break;
            }

            case SYS_WIFI_STATUS_WDRV_OPEN_REQ:
            {
                if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
                {
                    wifiSrvcObj->wifiSrvcDrvHdl = WDRV_PIC32MZW_Open(0, 0);
                    if (DRV_HANDLE_INVALID != wifiSrvcObj->wifiSrvcDrvHdl) 
                    {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Wi-Fi Driver open successfully\r\n");
</#if>
                        wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_AUTOCONNECT_WAIT;
                    }
                    OSAL_SEM_Post(&g_wifiSrvcSemaphore);
                }
                break;
            }

            case SYS_WIFI_STATUS_AUTOCONNECT_WAIT:
            {
                if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
                {
                    /* When user has enabled the auto connect feature of the Wi-Fi service in MHC(STA mode).
                       or in AP mode, below condition will be always true */
                    if (true == SYS_WIFI_GetAutoConnect()) 
                    {
                        if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_RegDomainSet(wifiSrvcObj->wifiSrvcDrvHdl, SYS_WIFI_GetCountryCode(), SYS_WIFI_SetRegDomainCallback)) 
                        {
                            SYS_WIFI_PrintConfig();
                            wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_TCPIP_WAIT_FOR_TCPIP_INIT;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                        } 
                        else
                        {
                            SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Wi-Fi driver Regulatory domain set failed\r\n");
</#if>
                        }
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                    } 
                    else
                    {
                        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "Auto connect is disabled\r\n");
</#if>
                    }
                    OSAL_SEM_Post(&g_wifiSrvcSemaphore);
                }
                break;
            }

            case SYS_WIFI_STATUS_TCPIP_WAIT_FOR_TCPIP_INIT:
            {
                if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
                {
                    tcpIpStat = TCPIP_STACK_Status(sysObj.tcpip);
                    if (tcpIpStat < 0) 
                    {
                        SYS_CONSOLE_MESSAGE("  TCP/IP stack initialization failed!\r\n");
                        wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_TCPIP_ERROR;
                    } 
                    else if (tcpIpStat == SYS_STATUS_READY) 
                    {
                        /* PIC32MZW1 network handle*/
                        netHdl = TCPIP_STACK_NetHandleGet("PIC32MZW1");
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>                  
<#if (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER)?has_content && (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER) == true>
<#if (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT)?has_content && (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT) == true>
                        /* STA Mode */
                        if (SYS_WIFI_STA == SYS_WIFI_GetMode()) 
                        {
                            if (true == TCPIP_DHCPS_IsEnabled(netHdl)) 
                            {
                                TCPIP_DHCPS_Disable(netHdl);
                            }
                            if ((true == TCPIP_DHCP_Enable(netHdl)) &&
                                (TCPIP_STACK_ADDRESS_SERVICE_DHCPC == TCPIP_STACK_AddressServiceSelect(_TCPIPStackHandleToNet(netHdl), TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON))) 
                            {
                                    g_wifiSrvcDhcpHdl = TCPIP_DHCP_HandlerRegister(netHdl, SYS_WIFI_TCPIP_DHCP_EventHandler, NULL);
                            }
                        } 
                        else if (SYS_WIFI_AP == SYS_WIFI_GetMode()) /*AP Mode*/
                        {
                            if (true == TCPIP_DHCP_IsEnabled(netHdl)) 
                            {
                                TCPIP_DHCP_Disable(netHdl);
                            }
                            TCPIP_DHCPS_Enable(netHdl); /*Enable DHCP Server in AP mode*/
                        }
</#if>
</#if>
<#elseif SYS_WIFI_STA_ENABLE == true>
<#if (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER)?has_content && (tcpipDhcps.TCPIP_STACK_USE_DHCP_SERVER) == true>
                        /* STA Mode*/
                        if (true == TCPIP_DHCPS_IsEnabled(netHdl)) 
                        {
                            TCPIP_DHCPS_Disable(netHdl);
                        }
</#if>
                        if ((true == TCPIP_DHCP_Enable(netHdl)) && 
                            (TCPIP_STACK_ADDRESS_SERVICE_DHCPC == TCPIP_STACK_AddressServiceSelect(_TCPIPStackHandleToNet(netHdl), TCPIP_NETWORK_CONFIG_DHCP_CLIENT_ON))) 
                        {
                            g_wifiSrvcDhcpHdl= TCPIP_DHCP_HandlerRegister (netHdl, SYS_WIFI_TCPIP_DHCP_EventHandler, NULL);
                        }
<#elseif SYS_WIFI_AP_ENABLE == true>
<#if (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT)?has_content && (tcpipDhcp.TCPIP_STACK_USE_DHCP_CLIENT) == true>
                        /* AP Mode*/
                        if (true == TCPIP_DHCP_IsEnabled(netHdl)) 
                        {
                            TCPIP_DHCP_Disable(netHdl);
                        }
</#if>
                        /* Enable DHCP Server in AP mode */
                        TCPIP_DHCPS_Enable(netHdl);
</#if>
                    }                
                    wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_CONNECT_REQ;
                    OSAL_SEM_Post(&g_wifiSrvcSemaphore);
                }
                break;
            }

            case SYS_WIFI_STATUS_CONNECT_REQ:
            {
                if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
                {
                    if (SYS_WIFI_SUCCESS == SYS_WIFI_SetChannel()) 
                    {
                        if (SYS_WIFI_SUCCESS == SYS_WIFI_ConfigReq()) 
                        {
                            if (SYS_WIFI_SUCCESS == SYS_WIFI_ConnectReq()) 
                            {
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>	
                                wifiSrvcObj->wifiSrvcStatus = (SYS_WIFI_STA == SYS_WIFI_GetMode()) ? SYS_WIFI_STATUS_TCPIP_READY : SYS_WIFI_STATUS_WAIT_FOR_AP_IP;
<#elseif SYS_WIFI_STA_ENABLE == true>
                                wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_TCPIP_READY;
<#elseif SYS_WIFI_AP_ENABLE == true>
                                wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_WAIT_FOR_AP_IP;
</#if>
                            }
                        }
                    }
                    OSAL_SEM_Post(&g_wifiSrvcSemaphore);
                }
                break;
            }
<#if SYS_WIFI_AP_ENABLE == true>

            case SYS_WIFI_STATUS_WAIT_FOR_AP_IP:
            {
                apIpAddr.Val = TCPIP_STACK_NetAddress(netHdl);
                if (apLastIp.Val != apIpAddr.Val)
                {
                    if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
                    {
                        apLastIp.Val = apIpAddr.Val;
                        SYS_CONSOLE_MESSAGE(TCPIP_STACK_NetNameGet(netHdl));
                        SYS_CONSOLE_MESSAGE(" AP Mode IP Address: ");
                        SYS_CONSOLE_PRINT("%d.%d.%d.%d \r\n", apIpAddr.v[0], apIpAddr.v[1], apIpAddr.v[2], apIpAddr.v[3]);
                        wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_TCPIP_READY;
                        OSAL_SEM_Post(&g_wifiSrvcSemaphore);
<#if SYS_WIFI_PROVISION_ENABLE == true>

                        /* In AP mode, Update AP mode start event to
                            Wi-Fi Provisioning service.Wi-Fi Provisioning
                            service has to open TCP server */
                        bool provConnStatus = true;
                        SYS_WIFIPROV_CtrlMsg(g_wifiSrvcProvObj, SYS_WIFIPROV_CONNECT, &provConnStatus, sizeof (bool));
</#if>
                    }
                }
                break;
            }

            case SYS_WIFI_STATUS_WAIT_FOR_STA_IP:
            {
                TCPIP_DHCPS_LEASE_HANDLE dhcpsLease = 0;
                TCPIP_DHCPS_LEASE_ENTRY dhcpsLeaseEntry;

                if ((true == g_wifiSrvcApConnectStatus) && (true == g_wifiSrvcApBssId.valid)) 
                {
                    /* When STA connected to PIC32MZW1 AP, 
                       get the connected STA IP address */
                    dhcpsLease = TCPIP_DHCPS_LeaseEntryGet(netHdl, &dhcpsLeaseEntry, dhcpsLease);
                    if ((0 != dhcpsLease) && (0 == memcmp(&dhcpsLeaseEntry.hwAdd, g_wifiSrvcApBssId.addr, WDRV_PIC32MZW_MAC_ADDR_LEN))) 
                    {
                        if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
                        {        
                            SYS_CONSOLE_PRINT("\r\n Connected STA IP:%d.%d.%d.%d \r\n", dhcpsLeaseEntry.ipAddress.v[0], dhcpsLeaseEntry.ipAddress.v[1], dhcpsLeaseEntry.ipAddress.v[2], dhcpsLeaseEntry.ipAddress.v[3]);
                            apIpAddr.Val = dhcpsLeaseEntry.ipAddress.Val;
                            wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_TCPIP_READY;
                            OSAL_SEM_Post(&g_wifiSrvcSemaphore);
                        }

                        /* updates the application with received STA IP address*/
                        SYS_WIFI_CallBackFun(SYS_WIFI_CONNECT, &apIpAddr, g_wifiSrvcCookie);
                    }
                }
                break;
            }
</#if>

            case SYS_WIFI_STATUS_TCPIP_READY:
            {
                break;
            }

            case SYS_WIFI_STATUS_TCPIP_ERROR:
            {
                SYS_STATUS tcpIpStat;
                tcpIpStat = TCPIP_STACK_Status(sysObj.tcpip);
            
                if (tcpIpStat < 2) 
                {
                    wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_TCPIP_ERROR;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
                SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, "TCP/IP Stack is not in ready state\r\n");
</#if>
                } 
                else 
                {
                    wifiSrvcObj->wifiSrvcStatus = SYS_WIFI_STATUS_TCPIP_WAIT_FOR_TCPIP_INIT;
                }
                break;
            }

            default:
            {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
            //SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG, " Wi-Fi service is not in invalid state\r\n");
</#if>
                break;
            }
        }
<#if SYS_WIFI_PROVISION_ENABLE == true>
        SYS_WIFIPROV_Tasks (g_wifiSrvcProvObj);
</#if>
        ret = wifiSrvcObj->wifiSrvcStatus;
    }
    return ret;
}

<#if SYS_WIFI_PROVISION_ENABLE == true>
static void SYS_WIFI_WIFIPROVCallBack
(
    uint32_t event, 
    void * data, 
    void *cookie
) 
{
    char *wifiSrvcCookie = (void *) cookie;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>       
    /* Validate client with verifying the cookie*/
    if (g_wifiSrvcProvCookieVal == *wifiSrvcCookie) 
    {
        switch (event) 
        {

            /* Wi-Fi Provisioning service has updated Wi-Fi configuration 
               with SETCONFIG event */
            case SYS_WIFIPROV_SETCONFIG:
            {
                SYS_WIFIPROV_CONFIG *wifiConfig = (SYS_WIFIPROV_CONFIG*) data;
                if ( (wifiConfig) && (false == g_wifiSrvcInit)) 
                {

                    /* Set g_wifiSrvcInit true, Wi-Fi Service can be started now 
                       after receiving configuration from Wi-Fi Provisioning 
                       service */
                    g_wifiSrvcInit = true;
                    memcpy(&g_wifiSrvcConfig, wifiConfig, sizeof(SYS_WIFIPROV_CONFIG));
                }
                else 
                {
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>	
                    if ((SYS_WIFIPROV_STA == (SYS_WIFIPROV_MODE) SYS_WIFI_GetMode()) && (SYS_WIFIPROV_STA == wifiConfig->mode)) 
                    {

                        /* Copy received configuration into Wi-Fi service structure */
                        memcpy(&g_wifiSrvcConfig, wifiConfig, sizeof (SYS_WIFIPROV_CONFIG));

                        /* In STA mode, check PIC32MZW1 connection status to HOMEAP */
                        if (g_wifiSrvcDrvAssocHdl == WDRV_PIC32MZW_ASSOC_HANDLE_INVALID) 
                        {

                            /* When Auto connected is enable by user and 
                              Auto connect retry has not reach to maximum limit 
                              then make connection requested */
                            if ((true == SYS_WIFI_GetAutoConnect()) && (g_wifiSrvcAutoConnectRetry == MAX_AUTO_CONNECT_RETRY)) 
                            {
                                /* Make a connection request */
                                SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_CONNECT_REQ);
                                g_wifiSrvcAutoConnectRetry = 0;
                            }
                        } 
                        else 
                        {
                            /* In STA mode, PIC32MZW1 is already connected to HOMEAP,
                               so disconnect before applying new received configuration */
                            SYS_WIFI_DisConnect();
                        }
                    } 
                    else 
                    {
                        SYS_CONSOLE_MESSAGE("######################################Rebooting the Device ###############################\r\n");
                        /* reboot the device */
                        SYS_RESET_SoftwareReset();
                    }
                    if (data) 
                    {

                        /* Update application(client), on receiving new 
                          Provisioning configuration */
                        SYS_WIFI_CallBackFun(SYS_WIFI_PROVCONFIG, data, g_wifiSrvcCookie);
                    }
                }
<#elseif SYS_WIFI_STA_ENABLE == true>
                    if ((SYS_WIFIPROV_STA == (SYS_WIFIPROV_MODE)SYS_WIFI_GetMode()) && (SYS_WIFIPROV_STA == wifiConfig->mode))
                    {

                        /* Copy received configuration into Wi-Fi service structure */
                        memcpy(&g_wifiSrvcConfig,wifiConfig,sizeof(SYS_WIFIPROV_CONFIG));

                        /* In STA mode, PIC32MZW1 is not connected to HOMEAP */
                        if(g_wifiSrvcDrvAssocHdl == WDRV_PIC32MZW_ASSOC_HANDLE_INVALID)
                        {

                            /* Auto connected is enable by user and Auto connect 
                               retry has not reach to maximum limit */
                            if((true == SYS_WIFI_GetAutoConnect()) && (g_wifiSrvcAutoConnectRetry == MAX_AUTO_CONNECT_RETRY))
                            {
                                SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_CONNECT_REQ);
                                g_wifiSrvcAutoConnectRetry = 0;
                            }
                        } 
                        else 
                        {

                            /* In STA mode, PIC32MZW1 is already connected to HOMEAP,
                               so disconnect before applying new received configuration */
                            SYS_WIFI_DisConnect();
                        }
                    }
                    if (data)
                    {

                        /* Update application(client), on receiving new Provisioning configuration */
                        SYS_WIFI_CallBackFun(SYS_WIFI_PROVCONFIG,data,g_wifiSrvcCookie);
                    }
                }
<#elseif SYS_WIFI_AP_ENABLE == true>
                        SYS_CONSOLE_MESSAGE("######################################Rebooting the Device ###############################\r\n"); 
                        SYS_RESET_SoftwareReset();
            }
                if (data) 
                {

                    /* Update application(client), on receiving new Provisioning configuration */
                    SYS_WIFI_CallBackFun(SYS_WIFI_PROVCONFIG,data,g_wifiSrvcCookie);
                }
</#if>
                break;
            }

            case SYS_WIFIPROV_GETCONFIG:
            {
                if (data) 
                {
                    SYS_WIFI_CallBackFun(SYS_WIFI_GETCONFIG, data, g_wifiSrvcCookie);
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>
}
</#if>

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WIFI Initialize Interface 
// *****************************************************************************
// *****************************************************************************
SYS_MODULE_OBJ SYS_WIFI_Initialize
(
    SYS_WIFI_CONFIG *config, 
    SYS_WIFI_CALLBACK callback, 
    void *cookie
) 
{

<#if SYS_WIFI_APPDEBUG_ENABLE  == true>

    /* Set user log level and log flow, user can modified this 
       configuration with command */
    g_wifiSrvcAppDbgCfg.logLevel = (APP_LOG_ERROR_LVL & SYS_WIFI_APPDEBUG_ERR_LEVEL_ENABLE) | (APP_LOG_DBG_LVL & SYS_WIFI_APPDEBUG_DBG_LEVEL_ENABLE) | 
                                (APP_LOG_INFO_LVL & SYS_WIFI_APPDEBUG_INFO_LEVEL_ENABLE) | (APP_LOG_FN_EE_LVL & SYS_WIFI_APPDEBUG_FUNC_LEVEL_ENABLE);
    g_wifiSrvcAppDbgCfg.logFlow =  (WIFI_CFG & SYS_WIFI_APPDEBUG_CFG_FLOW) | 
                                (WIFI_CONNECT & SYS_WIFI_APPDEBUG_CONNECT_FLOW)| 
                                (WIFI_PROVISIONING & SYS_WIFI_APPDEBUG_PROVISIONING_FLOW) |
                                (WIFI_PROVISIONINGCMD & SYS_WIFI_APPDEBUG_PROVISIONINGCMD_FLOW) | 
                                (WIFI_PROVISIONINGSOCKET & SYS_WIFI_APPDEBUG_PROVISIONINGSOCK_FLOW);
    g_wifiSrvcAppDbgCfg.prefixString = SYS_WIFI_DEBUG_PRESTR;
    g_wifiSrvcAppDebugHdl = SYS_APPDEBUG_Open(&g_wifiSrvcAppDbgCfg);
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG);
</#if>
    if (SYS_WIFI_STATUS_NONE == SYS_WIFI_GetTaskstatus()) 
    {
        if (OSAL_SEM_Create(&g_wifiSrvcSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE) 
        {
            SYS_CONSOLE_MESSAGE("Failed to Initialize Wi-Fi Service as Semaphore NOT created\r\n");
            return SYS_MODULE_OBJ_INVALID;
        }
        if (callback != NULL) 
        {
            SYS_WIFI_REGCB(callback);
        }
        WDRV_PIC32MZW_BSSCtxSetDefaults(&g_wifiSrvcObj.wifiSrvcBssCtx);
        WDRV_PIC32MZW_AuthCtxSetDefaults(&g_wifiSrvcObj.wifiSrvcAuthCtx);

        /* Set Wi-Fi service state to init */
        SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_INIT);
        SYS_WIFI_SetCookie(cookie);
<#if SYS_WIFI_PROVISION_ENABLE == false>

        /* Wi-Fi provisioning service is disabled */
        SYS_WIFI_InitConfig(config);
        g_wifiSrvcInit = true;
<#else>

        /* User has enabled Wi-Fi provisioning service using MHC */
        g_wifiSrvcProvObj= SYS_WIFIPROV_Initialize ((SYS_WIFIPROV_CONFIG *)config,SYS_WIFI_WIFIPROVCallBack,&g_wifiSrvcProvCookieVal);
</#if>
        return (SYS_MODULE_OBJ) &g_wifiSrvcObj;
    }
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG);
</#if>
    return SYS_MODULE_OBJ_INVALID;
}

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WIFI Deinitialize Interface 
// *****************************************************************************
// *****************************************************************************
SYS_WIFI_RESULT SYS_WIFI_Deinitialize
(
    SYS_MODULE_OBJ object
) 
{
    uint32_t ret = SYS_WIFI_OBJ_INVALID;
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG);
</#if>
    
    if (&g_wifiSrvcObj == (SYS_WIFI_OBJ *) object) 
    {
<#if (SYS_WIFI_STA_ENABLE == true) && (SYS_WIFI_AP_ENABLE == true)>

        /* STA Mode Configuration */
        if (SYS_WIFI_STA == SYS_WIFI_GetMode()) 
        {
            /* Before Wi-Fi De-initialize service,
               Disconnected PIC32MZW1 STA from HOMEAP */
            SYS_WIFI_DisConnect();
            TCPIP_DHCP_HandlerDeRegister(g_wifiSrvcDhcpHdl);
        } 
        else if (SYS_WIFI_AP == SYS_WIFI_GetMode()) 
        {
            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_APStop(g_wifiSrvcObj.wifiSrvcDrvHdl)) 
            {
                SYS_CONSOLE_MESSAGE(" AP mode Stop Failed \n");
            }
        }
<#elseif SYS_WIFI_STA_ENABLE == true>
        SYS_WIFI_DisConnect();
        TCPIP_DHCP_HandlerDeRegister(g_wifiSrvcDhcpHdl);
<#elseif SYS_WIFI_AP_ENABLE == true>
        if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_APStop(g_wifiSrvcObj.wifiSrvcDrvHdl)) 
        {
            SYS_CONSOLE_MESSAGE(" AP mode Stop Failed \n");
        }
</#if>
        WDRV_PIC32MZW_Close(g_wifiSrvcObj.wifiSrvcDrvHdl);
        g_wifiSrvcInit = false;
        memset(&g_wifiSrvcObj,0,sizeof(SYS_WIFI_OBJ));
        memset(g_wifiSrvcCallBack,0,sizeof(g_wifiSrvcCallBack));
        SYS_WIFI_SetTaskstatus(SYS_WIFI_STATUS_NONE);
<#if SYS_WIFI_PROVISION_ENABLE == true>		
        SYS_WIFIPROV_Deinitialize(g_wifiSrvcProvObj);
</#if>
        if (OSAL_SEM_Delete(&g_wifiSrvcSemaphore) != OSAL_RESULT_TRUE) 
        {
            SYS_CONSOLE_MESSAGE("Failed to Delete Wi-Fi Service Semaphore \r\n");
        }
        ret = SYS_WIFI_SUCCESS;
    }
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG);
</#if>
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
//SYS_APPDEBUG_Close(g_wifiSrvcAppDebugHdl);
</#if>
    return ret;
}

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WiFi Get Status Interface
// *****************************************************************************
// *****************************************************************************
uint8_t SYS_WIFI_GetStatus
(
    SYS_MODULE_OBJ object
) 
{
    uint8_t ret = SYS_WIFI_OBJ_INVALID;
    
    if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
    {
        if (&g_wifiSrvcObj == (SYS_WIFI_OBJ *) object) 
        {
            /* Provide current status of Wi-Fi service to client */
            ret = ((SYS_WIFI_OBJ *) object)->wifiSrvcStatus;
        }
        OSAL_SEM_Post(&g_wifiSrvcSemaphore);
    }
    return ret;
}

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WiFi Tasks Interface
// *****************************************************************************
// *****************************************************************************
uint8_t SYS_WIFI_Tasks
(
    SYS_MODULE_OBJ object
) 
{
    uint8_t ret = SYS_WIFI_OBJ_INVALID;

    if (&g_wifiSrvcObj == (SYS_WIFI_OBJ *) object) 
    {
        ret = SYS_WIFI_ExecuteBlock(object);
    }
    return ret;
}

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WiFi Control Message Interface
// *****************************************************************************
// *****************************************************************************
SYS_WIFI_RESULT SYS_WIFI_CtrlMsg
(
    SYS_MODULE_OBJ object, 
    uint32_t event, 
    void *buffer, 
    uint32_t length
) 
{
    uint8_t ret = SYS_WIFI_OBJ_INVALID;
    uint8_t *channel;
    bool *scanType;

    if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&g_wifiSrvcSemaphore, OSAL_WAIT_FOREVER)) 
    {
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG);
</#if>    
        if (&g_wifiSrvcObj == (SYS_WIFI_OBJ *)object) 
        {
            switch (event) 
            {

                case SYS_WIFI_CONNECT:
                {
                    /* if service is already processing pending request from 
                       client then ignore new request */
                    if (SYS_WIFI_STATUS_CONNECT_REQ != g_wifiSrvcObj.wifiSrvcStatus) 
                    {
                        if ((buffer) && (length == sizeof (SYS_WIFI_CONFIG)))
                        {
                            ret = SYS_WIFI_SetConfig((SYS_WIFI_CONFIG *) buffer, SYS_WIFI_STATUS_CONNECT_REQ);
                        }
                    } 
                    else
                    {
                        /* Client is currently processing pending connection request */
                        ret = SYS_WIFI_CONNECT_FAILURE;
                    }
                    break;
                }

                case SYS_WIFI_REGCALLBACK:
                {
                    SYS_WIFI_CALLBACK g_wifiSrvcFunPtr = buffer;
                    if ((g_wifiSrvcFunPtr) && (length == sizeof(g_wifiSrvcFunPtr))) 
                    {
                        /* Register the client callback function */
                        ret = SYS_WIFI_REGCB(g_wifiSrvcFunPtr);
                    }
                    break;
                }

                case SYS_WIFI_DISCONNECT:
                {
                    /* Client has made disconnect request */
                    ret = SYS_WIFI_DisConnect();
                    break;
                }

                case SYS_WIFI_GETCONFIG:
                {
                    if (true == g_wifiSrvcInit) 
                    {
                        if ((buffer) && (length == sizeof (SYS_WIFI_CONFIG))) 
                        {

                            /* Client has request Wi-Fi service configuration,
                               Copy Wi-Fi configuration into client structure */
                            memcpy(buffer, &g_wifiSrvcConfig, sizeof (g_wifiSrvcConfig));
                            ret = SYS_WIFI_SUCCESS;
                        }
                    } 
                    else
                    {

                        /* Wi-Fi service is not started */
                        ret = SYS_WIFI_SERVICE_UNINITIALIZE;
                    }
                    break;
                }

                case SYS_WIFI_SCANREQ:
                {

                    /* if service is already processing pending connection 
                       request from client then ignore new request */
                    if ((SYS_WIFI_STATUS_CONNECT_REQ != g_wifiSrvcObj.wifiSrvcStatus) && (buffer) && (length == 2)) 
                    {
                        channel = (uint8_t *) buffer;
                        scanType = (bool *) buffer + 1;
                        ret = SYS_WIFI_SetScan(*channel, *scanType);
                    }
                    break;
                }
            }
        }
<#if SYS_WIFI_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_CFG);
</#if>
        OSAL_SEM_Post(&g_wifiSrvcSemaphore);
    }
    return ret;
}
/* *****************************************************************************
 End of File
 */
