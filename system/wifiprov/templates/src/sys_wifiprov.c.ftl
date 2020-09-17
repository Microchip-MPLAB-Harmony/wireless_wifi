/*******************************************************************************
  Wi-Fi Provision System Service Implementation

  File Name:
    sys_wifiprov.c

  Summary:
    Source code for the Wi-Fi Provision system service implementation.

  Description:
    This file contains the source code for the Wi-Fi Provision system service
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
#include "configuration.h"
#include "system/wifiprov/sys_wifiprov.h"
<#if SYS_WIFIPROV_SOCKET == true>
#include "system/wifiprov/sys_wifiprov_json.h"
</#if>
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
#include "system/appdebug/sys_appdebug.h"
</#if>
<#if (SYS_WIFIPROV_HTTP == true) && (SYS_WIFIPROV_ENABLE_HTTPNET == true)>
#include "system/wifiprov/http_net_print.h"
</#if>
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

typedef enum 
{
    /* Wi-Fi Provisioning service NVM write state */
    SYS_WIFIPROV_NVM_WRITE = 0,

    /* Wi-Fi Provisioning service NVM erase state */
    SYS_WIFIPROV_NVM_ERASE,

    /* Wi-Fi Provisioning service NVM read state */
    SYS_WIFIPROV_NVM_READ,

    /* Wi-Fi Provisioning service invalid state */
    SYS_WIFIPROV_NONE = 255
} SYS_WIFIPROV_NVMTYPEOPER; //NVM Operation

typedef struct 
{
    /* The WiFi service current status */
    SYS_WIFIPROV_STATUS status;

    /* The WiFi service NVM type operation  */
    SYS_WIFIPROV_NVMTYPEOPER nvmTypeOfOperation;
} SYS_WIFIPROV_OBJ; /*Wi-Fi Provision system service Object*/

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
/*Wi-Fi Provisioning Object */
static  SYS_WIFIPROV_OBJ      g_wifiProvSrvcObj = {SYS_WIFIPROV_STATUS_NONE, SYS_WIFIPROV_NONE};

/* Wi-Fi Provisioning Configuration */
static  SYS_WIFIPROV_CONFIG   g_wifiProvSrvcConfig CACHE_ALIGN;

/* Wi-Fi Provisioning read Configuration*/
static  SYS_WIFIPROV_CONFIG   g_wifiProvSrvcConfigRead;

/* Wi-Fi Provisioning Callback */
static  SYS_WIFIPROV_CALLBACK g_wifiProvSrvcCallBack;

/* Wi-Fi Provisioning Cookie */
static  void *                g_wifiProvSrvcCookie;
<#if SYS_WIFIPROV_SOCKET == true>

/* Wi-Fi Provisioning Socket */
static  TCP_SOCKET            g_wifiProvSrvcSocket = INVALID_SOCKET;

/* Wi-Fi Provisioning TCP Handle */
static  TCPIP_TCP_SIGNAL_HANDLE g_wifiProvSrvcHdl;
</#if>
<#if SYS_WIFIPROV_APPDEBUG_ENABLE == true>

/* Wi-Fi Provisioning AppDebug Handle */
extern  SYS_MODULE_OBJ        g_wifiSrvcAppDebugHdl;

/* Wi-Fi Provisioning AppDebug Config */
extern  SYS_APPDEBUG_CONFIG   g_wifiSrvcAppDbgCfg;

/* App Debug Print Flows */
#define     WIFI_PROVISIONING           0x3
#define     WIFI_PROVISIONINGCMD        0x4
#define     WIFI_PROVISIONINGSOCKET     0x5
</#if>
// *****************************************************************************
static      void   SYS_WIFIPROV_WriteConfig(void);
<#if SYS_WIFIPROV_CMD == true>
static      bool   SYS_WIFIPROV_CMDInit(void);
static      int    SYS_WIFIPROV_CMDProcess
(
    SYS_CMD_DEVICE_NODE* pCmdIO, 
    int argc, char** argv
);
static      int    SYS_WIFIPROV_CMDHelp
(
    SYS_CMD_DEVICE_NODE* pCmdIO, 
    int argc, char** argv
);
</#if>
<#if SYS_WIFIPROV_SOCKET == true>
static      void   SYS_WIFIPROV_InitSocket(void);
static      void   SYS_WIFIPROV_DeInitSocket(void);
</#if>
<#if SYS_WIFIPROV_CMD == true>
static     uint8_t parser_data
(
    uint8_t sbuff[], uint8_t dbuff[], 
    uint8_t dbufflen, uint8_t val, 
    uint8_t offset
);
static      void   SYS_WIFIPROV_PrintConfig(void);
</#if>
// *****************************************************************************
// *****************************************************************************
// Section: Local Functions
// *****************************************************************************
// *****************************************************************************
static inline void SYS_WIFIPROV_SetTaskstatus
(
    SYS_WIFIPROV_STATUS val
) 
{
    g_wifiProvSrvcObj.status = val;
}

static inline SYS_WIFIPROV_STATUS SYS_WIFIPROV_GetTaskstatus(void) 
{
    return g_wifiProvSrvcObj.status;
}

static inline void SYS_WIFIPROV_CallBackFun
(
    uint32_t event, 
    void * data, 
    void *cookie
) 
{
    if (g_wifiProvSrvcCallBack)
    {
        /* call client register function */
        g_wifiProvSrvcCallBack(event, data, cookie);
    }
}

static void inline SYS_WIFIPROV_SetCookie(void *cookie) 
{
    g_wifiProvSrvcCookie = cookie;
}

static void inline SYS_WIFIPROV_InitConfig(SYS_WIFIPROV_CONFIG *config) 
{
    if (!config) 
    {
        /* User has not set configuration during Wi-Fi Provisioning Service 
           initialization,so copy the MHC configuration into 
           Wi-Fi Service structure */
        g_wifiProvSrvcConfig.mode = SYS_WIFI_DEVMODE;
        g_wifiProvSrvcConfig.saveConfig = SYS_WIFIPROV_SAVECONFIG;
        memcpy(g_wifiProvSrvcConfig.countryCode, SYS_WIFI_COUNTRYCODE, strlen(SYS_WIFI_COUNTRYCODE));
<#if SYS_WIFIPROV_STA_ENABLE == true>
        
        /* STA Mode Configuration */
        g_wifiProvSrvcConfig.staConfig.channel = 0;
        g_wifiProvSrvcConfig.staConfig.autoConnect = SYS_WIFI_STA_AUTOCONNECT;
        g_wifiProvSrvcConfig.staConfig.authType = SYS_WIFI_STA_AUTHTYPE;
        memcpy(g_wifiProvSrvcConfig.staConfig.ssid, SYS_WIFI_STA_SSID, sizeof (SYS_WIFI_STA_SSID));
        memcpy(g_wifiProvSrvcConfig.staConfig.psk, SYS_WIFI_STA_PWD, sizeof (SYS_WIFI_STA_PWD));
</#if>
<#if SYS_WIFIPROV_AP_ENABLE == true>
        
        /* AP Mode Configuration */
        g_wifiProvSrvcConfig.apConfig.channel = SYS_WIFI_AP_CHANNEL;
        g_wifiProvSrvcConfig.apConfig.ssidVisibility = SYS_WIFI_AP_SSIDVISIBILE;
        g_wifiProvSrvcConfig.apConfig.authType = SYS_WIFI_AP_AUTHTYPE;
        memcpy(g_wifiProvSrvcConfig.apConfig.ssid, SYS_WIFI_AP_SSID, sizeof (SYS_WIFI_AP_SSID));
        memcpy(g_wifiProvSrvcConfig.apConfig.psk, SYS_WIFI_AP_PWD, sizeof (SYS_WIFI_AP_PWD));      
</#if>
<#if SYS_WIFIPROV_HTTP == true>
        SYS_WIFIPROV_SetTaskstatus(SYS_WIFIPROV_STATUS_MPFS_MOUNT);
<#else>	
        SYS_WIFIPROV_SetTaskstatus(SYS_WIFIPROV_STATUS_NVM_READ);
</#if>
    } 
    else 
    {
        /* User has set configuration during Wi-Fi provisioning Service 
           initialization, copy the user configuration into Wi-Fi Service 
           structure */
        memcpy(&g_wifiProvSrvcConfig, config, sizeof (SYS_WIFIPROV_CONFIG));
        SYS_WIFIPROV_WriteConfig();
    }
}

static void SYS_WIFIPROV_CheckConfig(void) 
{
    /* when NVM read provide empty data, the save config value will be 0xFF */
    if (0xFF != g_wifiProvSrvcConfigRead.saveConfig) 
    {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "NVM Wi-Fi Configuration has valid data \r\n");
</#if>
        SYS_WIFIPROV_SetTaskstatus(SYS_WIFIPROV_STATUS_WAITFORREQ);
        memcpy(&g_wifiProvSrvcConfig, &g_wifiProvSrvcConfigRead, sizeof (SYS_WIFIPROV_CONFIG));
        memset(&g_wifiProvSrvcConfigRead, 0, sizeof (SYS_WIFIPROV_CONFIG));
        SYS_WIFIPROV_CallBackFun(SYS_WIFIPROV_SETCONFIG, &g_wifiProvSrvcConfig, g_wifiProvSrvcCookie);
    } 
    else 
    {     /* Write valid Wi-Fi Config into NVM */
          /* NVM write command sequence: first Erase than write */
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "NVM Wi-Fi Configuration is empty \r\n");
</#if>
        SYS_WIFIPROV_SetTaskstatus(SYS_WIFIPROV_STATUS_NVM_ERASE);
    }
}

static inline void SYS_WIFIPROV_NVMRead(void) 
{
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "Reading Wi-Fi configuration from NVM\r\n");
</#if>
    /* Start the NVM read operation */
    g_wifiProvSrvcObj.nvmTypeOfOperation = SYS_WIFIPROV_NVM_READ;
    NVM_Read((uint32_t *)&g_wifiProvSrvcConfigRead, sizeof (g_wifiProvSrvcConfigRead), SYS_WIFIPROV_NVMADDR);
}

static inline void SYS_WIFIPROV_NVMWrite(void) 
{
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "Writing Wi-Fi configuration to NVM\r\n");
</#if>
    /* Start the NVM Write operation */
    g_wifiProvSrvcObj.nvmTypeOfOperation = SYS_WIFIPROV_NVM_WRITE;
    NVM_RowWrite((uint32_t *)&g_wifiProvSrvcConfig, SYS_WIFIPROV_NVMADDR);
}

static inline void SYS_WIFIPROV_NVMErase(void) 
{
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "Erasing Wi-Fi configuration from NVM\r\n");
</#if>
    /* Start the NVM Erase operation */
    g_wifiProvSrvcObj.nvmTypeOfOperation = SYS_WIFIPROV_NVM_ERASE;
    NVM_PageErase(SYS_WIFIPROV_NVMADDR);
}

<#if SYS_WIFIPROV_CMD == true>
static void SYS_WIFIPROV_PrintConfig(void) 
{
    SYS_CONSOLE_PRINT("\r\n mode=%d (0-STA,1-AP) saveConfig=%d countryCode=%s\r\n ", g_wifiProvSrvcConfig.mode, g_wifiProvSrvcConfig.saveConfig, g_wifiProvSrvcConfig.countryCode);
<#if SYS_WIFIPROV_STA_ENABLE == true>
    SYS_CONSOLE_PRINT("\r\n STA Configuration :\r\n channel=%d \r\n autoConnect=%d \r\n ssid=%s \r\n passphase=%s \r\n authentication type=%d (1-Open,2-WEP,3-Mixed mode(WPA/WPA2),4-WPA2,5-Mixed mode(WPA2/WPA3),6-WPA3) \r\n", g_wifiProvSrvcConfig.staConfig.channel, g_wifiProvSrvcConfig.staConfig.autoConnect, g_wifiProvSrvcConfig.staConfig.ssid, g_wifiProvSrvcConfig.staConfig.psk, g_wifiProvSrvcConfig.staConfig.authType);
</#if>
<#if SYS_WIFIPROV_AP_ENABLE == true>
    SYS_CONSOLE_PRINT("\r\n AP Configuration :\r\n  channel=%d \r\n ssidVisibility=%d \r\n ssid=%s \r\n passphase=%s \r\n authentication type=%d (1-Open,2-WEP,3-Mixed mode(WPA/WPA2),4-WPA2,5-Mixed mode(WPA2/WPA3),6-WPA3) \r\n", g_wifiProvSrvcConfig.apConfig.channel, g_wifiProvSrvcConfig.apConfig.ssidVisibility, g_wifiProvSrvcConfig.apConfig.ssid, g_wifiProvSrvcConfig.apConfig.psk, g_wifiProvSrvcConfig.apConfig.authType);
</#if>
}
</#if>
static void SYS_WIFIPROV_WriteConfig(void) 
{
    if (true == g_wifiProvSrvcConfig.saveConfig) 
    {
        /* User has enabled Save Config,
           so first copy the Wi-Fi configuration into NVM flash */
        SYS_WIFIPROV_SetTaskstatus(SYS_WIFIPROV_STATUS_NVM_ERASE);
    } 
    else 
    {
        /* User has not enabled Save Config,
           so no need to copy the Wi-Fi configuration into NVM flash */
        /* Just generate the callback to Wi-Fi service for connection request */
        SYS_WIFIPROV_CallBackFun(SYS_WIFIPROV_SETCONFIG, &g_wifiProvSrvcConfig, g_wifiProvSrvcCookie);
    }
}

static SYS_WIFIPROV_STATUS SYS_WIFIPROV_ExecuteBlock
(
    SYS_MODULE_OBJ object
)
{
    SYS_WIFIPROV_OBJ *wifiProvSrvcObj = (SYS_WIFIPROV_OBJ *) object;
    uint8_t ret = SYS_WIFIPROV_OBJ_INVALID;
    
    if (&g_wifiProvSrvcObj == (SYS_WIFIPROV_OBJ*) wifiProvSrvcObj)
    {
        switch (wifiProvSrvcObj->status) 
        {
<#if SYS_WIFIPROV_HTTP == true>
            case SYS_WIFIPROV_STATUS_MPFS_MOUNT:
            {
<#if SYS_WIFIPROV_ENABLE_HTTPNET == false>
                /* Mount the file system for webpage*/
                if (SYS_FS_Mount("/dev/nvma1", TCPIP_HTTP_WEB_DIR, MPFS2, 0, NULL) == SYS_FS_RES_SUCCESS) 
                {
</#if>
<#if SYS_WIFIPROV_ENABLE_HTTPNET == true>
                /* Mount the file system for webpage*/
                if (SYS_FS_Mount("/dev/nvma1", TCPIP_HTTP_NET_WEB_DIR, MPFS2, 0, NULL) == SYS_FS_RES_SUCCESS) 
                {
</#if>
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "The %s File System is mounted \r\n","MPFS2");
</#if>
                    wifiProvSrvcObj->status = SYS_WIFIPROV_STATUS_NVM_READ;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
                } 
                else 
                {
                  SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "The %s File System is mounted failed \r\n", "MPFS2");
</#if>
                }
                break;
            }
</#if>

           case SYS_WIFIPROV_STATUS_NVM_READ:
            {
                /* Check if NVM flash performing any operation */
                if (!NVM_IsBusy()) 
                {
                    /* Start the NVM Read Operation */
                    SYS_WIFIPROV_NVMRead();                           
                    wifiProvSrvcObj->status = SYS_WIFIPROV_STATUS_CONFIG_CHECK;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
                } 
                else 
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "NVM is busy in NVM read state\r\n");	
</#if>
                }
                break;
            }

            case SYS_WIFIPROV_STATUS_CONFIG_CHECK:
            {
                /* verify the NVM Read output, based on NVM Read 
                   output set new state */
                SYS_WIFIPROV_CheckConfig();
                break;
            }

            case SYS_WIFIPROV_STATUS_NVM_ERASE:
            {
                /* Check if NVM flash performing any operation */
                if (!NVM_IsBusy()) 
                {
                    /* Start the NVM Erase Operation */
                    SYS_WIFIPROV_NVMErase();                      
                    wifiProvSrvcObj->status = SYS_WIFIPROV_STATUS_NVM_WRITE;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
                } 
                else 
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "NVM is busy in NVM erase state\r\n");
</#if>
                }
                break;
            }

            case SYS_WIFIPROV_STATUS_NVM_WRITE:
            {
                /* Check if NVM flash performing any operation */
                if (!NVM_IsBusy()) 
                {
                    /* Start the NVM Write operation */
                    SYS_WIFIPROV_NVMWrite();                      
                    wifiProvSrvcObj->status = SYS_WIFIPROV_STATUS_WAITFORWRITE;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
                } 
                else 
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "NVM is busy in NVM write state\r\n");
</#if>       
                }
                break;
            }

            /* Wait for NVM Write to complete */
            case SYS_WIFIPROV_STATUS_WAITFORWRITE:
            {
                /* Check if NVM flash performing any operation */
                if (!NVM_IsBusy()) 
                {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "Updating Wi-Fi provisioning data to Wi-Fi service \r\n");
</#if> 
                    /* Update the Wi-Fi configuration to Wi-Fi service 
                       with the callback */
                    SYS_WIFIPROV_CallBackFun(SYS_WIFIPROV_SETCONFIG, &g_wifiProvSrvcConfig, g_wifiProvSrvcCookie);
                    wifiProvSrvcObj->status = SYS_WIFIPROV_STATUS_WAITFORREQ;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
                } 
                else 
                {
                    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "NVM is busy in NVM write complete state\r\n");
</#if>
                }
                break;
            }

            case SYS_WIFIPROV_STATUS_WAITFORREQ:
            default:
            {
                break;
            }
        }
        ret = wifiProvSrvcObj->status;
    }
    return ret;
}

<#if (SYS_WIFIPROV_CMD == true ) || (SYS_WIFIPROV_SOCKET == true) >
static bool SYS_WIFIPROV_ConfigValidate(SYS_WIFIPROV_CONFIG wifiProvSrvcConfig) 
{
<#if (SYS_WIFIPROV_STA_ENABLE == true) && (SYS_WIFIPROV_AP_ENABLE == true)>  
    if (!((wifiProvSrvcConfig.mode == SYS_WIFIPROV_STA) || (wifiProvSrvcConfig.mode == SYS_WIFIPROV_AP))) 
    {
<#elseif SYS_WIFIPROV_STA_ENABLE == true>
    if (!((wifiProvSrvcConfig.mode == SYS_WIFIPROV_STA))) 
    {
<#elseif SYS_WIFIPROV_AP_ENABLE == true>
    if (!(wifiProvSrvcConfig.mode == SYS_WIFIPROV_AP)) 
    {
</#if>
        SYS_CONSOLE_MESSAGE(" set valid boot mode \r\n");
        return true;
    }    
    if (!((wifiProvSrvcConfig.saveConfig == true) || (wifiProvSrvcConfig.saveConfig == false))) 
    {
        SYS_CONSOLE_MESSAGE(" set valid save config value \r\n");
        return true;
    }
<#if SYS_WIFIPROV_STA_ENABLE == true>
    if (SYS_WIFIPROV_STA == (SYS_WIFIPROV_MODE) wifiProvSrvcConfig.mode) 
    {
        if (!((wifiProvSrvcConfig.staConfig.channel >= 0) && (wifiProvSrvcConfig.staConfig.channel <= 13))) 
        {
            SYS_CONSOLE_MESSAGE(" set valid station mode channel number \r\n");
            return true;
        }
        if (!((wifiProvSrvcConfig.staConfig.autoConnect == true) || (wifiProvSrvcConfig.staConfig.autoConnect == false))) 
        {
            SYS_CONSOLE_MESSAGE(" set valid station mode Auto config value \r\n");
            return true;
        }
        if (!(((wifiProvSrvcConfig.staConfig.authType == SYS_WIFIPROV_OPEN) ||
             ((wifiProvSrvcConfig.staConfig.authType >= SYS_WIFIPROV_WPAWPA2MIXED) && (wifiProvSrvcConfig.staConfig.authType <= SYS_WIFIPROV_WPA3)))))  //ignore WEP as not support 
        {

            SYS_CONSOLE_MESSAGE(" set valid station mode Auth value \r\n");
            return true;
        }
        if ((wifiProvSrvcConfig.staConfig.authType >= SYS_WIFIPROV_WPAWPA2MIXED) && (wifiProvSrvcConfig.staConfig.authType <= SYS_WIFIPROV_WPA3)) 
        {
            if (strlen((const char *) wifiProvSrvcConfig.staConfig.psk) < 8) 
            {
                SYS_CONSOLE_MESSAGE(" set valid station mode passphase \r\n");
                return true;
            }
        }
    }
</#if>
<#if SYS_WIFIPROV_AP_ENABLE == true>
    if (SYS_WIFIPROV_AP == (SYS_WIFIPROV_MODE) wifiProvSrvcConfig.mode) 
    {
        if (!((wifiProvSrvcConfig.apConfig.channel >= 1) && (wifiProvSrvcConfig.apConfig.channel <= 13))) 
        {
            SYS_CONSOLE_MESSAGE(" set valid access point mode channel number \r\n");
            return true;
        }
        if (!((wifiProvSrvcConfig.apConfig.ssidVisibility == true) || (wifiProvSrvcConfig.apConfig.ssidVisibility == false))) 
        {
            SYS_CONSOLE_MESSAGE(" set valid access point mode SSID visibility \r\n");
            return true;
        }
        if (!(((wifiProvSrvcConfig.apConfig.authType == SYS_WIFIPROV_OPEN) ||
             ((wifiProvSrvcConfig.apConfig.authType >= SYS_WIFIPROV_WPAWPA2MIXED) && (wifiProvSrvcConfig.apConfig.authType <= SYS_WIFIPROV_WPA3)))))  //ignore WEP as not support 
        {
            SYS_CONSOLE_MESSAGE(" set valid access point mode Auth value \r\n");
            return true;
        }
        
        if ((wifiProvSrvcConfig.apConfig.authType >= SYS_WIFIPROV_WPAWPA2MIXED) && (wifiProvSrvcConfig.apConfig.authType <= SYS_WIFIPROV_WPA3)) 
        {
            if (strlen((const char *) wifiProvSrvcConfig.apConfig.psk) < 8) 
            {
                SYS_CONSOLE_MESSAGE(" set valid access point mode passphase \r\n");
                return true;
            }
        }
    }
 </#if>	
    return false;
}
</#if>

<#if SYS_WIFIPROV_CMD == true>
static const SYS_CMD_DESCRIPTOR WiFiCmdTbl[] =
{
    {"wifiprov", (SYS_CMD_FNC) SYS_WIFIPROV_CMDProcess, ": WiFi provision commands processing"},
    {"wifiprovhelp", (SYS_CMD_FNC) SYS_WIFIPROV_CMDHelp, ": WiFi provision commands help "},
};

static bool SYS_WIFIPROV_CMDInit(void) 
{
    bool ret = SYS_WIFIPROV_SUCCESS;
    /* Register Wi-Fi provisioning service commands */
   if (!SYS_CMD_ADDGRP(WiFiCmdTbl, sizeof (WiFiCmdTbl) / sizeof (*WiFiCmdTbl), "wifiprov", ": WiFi provision commands")) 
    {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGCMD, "Failed to create WiFi provision Commands \r\n");
</#if>
        ret = SYS_WIFIPROV_FAILURE;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    } 
    else 
    {
        SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGCMD, "successfully created WiFi provision Commands \r\n");
</#if>
    }
    return ret;
}

static int SYS_WIFIPROV_CMDProcess
(
    SYS_CMD_DEVICE_NODE* pCmdIO, 
    int argc, 
    char** argv
) 
{
    char val = '"';
    SYS_WIFIPROV_CONFIG wifiProvSrvcConfig;
    bool error = false;

    memset(&wifiProvSrvcConfig, 0, sizeof (SYS_WIFIPROV_CONFIG));
    if ((argc >= 7) && (!strcmp(argv[1], "set"))) 
    {
<#if (SYS_WIFIPROV_STA_ENABLE == true) && (SYS_WIFIPROV_AP_ENABLE == true)>
        if (SYS_WIFIPROV_STA == (SYS_WIFIPROV_MODE) strtol(argv[2], NULL, 0)) 
        {
            wifiProvSrvcConfig.mode = strtol(argv[2], NULL, 0);
            wifiProvSrvcConfig.saveConfig = strtol(argv[3], NULL, 0);
            if (strlen((const char *) argv[4]) <= sizeof (wifiProvSrvcConfig.countryCode)) 
            {
                parser_data((uint8_t *) argv[4], wifiProvSrvcConfig.countryCode, sizeof(wifiProvSrvcConfig.countryCode), val, 0);
            } 
            else 
            {
                error = true;
            }
            wifiProvSrvcConfig.staConfig.channel = strtol(argv[5], NULL, 0);
            wifiProvSrvcConfig.staConfig.autoConnect = strtol(argv[6], NULL, 0);
            wifiProvSrvcConfig.staConfig.authType = strtol(argv[7], NULL, 0);
            if (strlen((const char *) argv[8]) <= sizeof (wifiProvSrvcConfig.staConfig.ssid)) 
            {
                parser_data((uint8_t *) argv[8], wifiProvSrvcConfig.staConfig.ssid, sizeof(wifiProvSrvcConfig.staConfig.ssid), val, 0);
            } 
            else 
            {
                error = true;
            }

            if (argc == 10) 
            {
                if (strlen((const char *) argv[9]) <= sizeof (wifiProvSrvcConfig.staConfig.psk)) 
                {
                    parser_data((uint8_t *) argv[9], wifiProvSrvcConfig.staConfig.psk, sizeof(wifiProvSrvcConfig.staConfig.psk), val, 0);
                } 
                else 
                {
                    error = true;
                }
            } 
            else 
            {
                memset(wifiProvSrvcConfig.staConfig.psk, 0, sizeof (wifiProvSrvcConfig.staConfig.psk));
            }

            if ((!error) && (!SYS_WIFIPROV_ConfigValidate(wifiProvSrvcConfig))) 
            {
                g_wifiProvSrvcConfig.mode = wifiProvSrvcConfig.mode;
                g_wifiProvSrvcConfig.saveConfig = wifiProvSrvcConfig.saveConfig;
                memcpy(&g_wifiProvSrvcConfig.staConfig, &wifiProvSrvcConfig.staConfig, sizeof(SYS_WIFIPROV_STA_CONFIG));
                SYS_WIFIPROV_WriteConfig();
                //SYS_WIFIPROV_PrintConfig();
            } 
            else 
            {
                SYS_CONSOLE_PRINT(" Wrong Command\n");
            }
        } else if (SYS_WIFIPROV_AP == (SYS_WIFIPROV_MODE) strtol(argv[2], NULL, 0)) 
        {
            wifiProvSrvcConfig.mode = strtol(argv[2], NULL, 0);
            wifiProvSrvcConfig.saveConfig = strtol(argv[3], NULL, 0);
            if (strlen((const char *) argv[4]) <= sizeof (wifiProvSrvcConfig.countryCode)) 
            {
                parser_data((uint8_t *) argv[4], wifiProvSrvcConfig.countryCode, sizeof(wifiProvSrvcConfig.countryCode), val, 0);
            } 
            else 
            {
                error = true;
            }
            wifiProvSrvcConfig.apConfig.channel = strtol(argv[5], NULL, 0);
            wifiProvSrvcConfig.apConfig.ssidVisibility = strtol(argv[6], NULL, 0);
            wifiProvSrvcConfig.apConfig.authType = strtol(argv[7], NULL, 0);

            if (strlen((const char *) argv[8]) <= sizeof (wifiProvSrvcConfig.apConfig.ssid)) 
            {
                parser_data((uint8_t *) argv[8], wifiProvSrvcConfig.apConfig.ssid, sizeof(wifiProvSrvcConfig.apConfig.ssid), val, 0);
            } 
            else 
            {
                error = true;
            }
            
            if (argc == 10) 
            {
                if (strlen((const char *)argv[9]) <= sizeof(wifiProvSrvcConfig.apConfig.psk))
                {
                    parser_data((uint8_t *)argv[9],wifiProvSrvcConfig.apConfig.psk,sizeof(wifiProvSrvcConfig.apConfig.psk),val,0);
                } 
                else 
                {
                    error = true;
                }
            } 
            else 
            {
                memset(wifiProvSrvcConfig.apConfig.psk, 0, sizeof(wifiProvSrvcConfig.apConfig.psk));
            }

            if ((!error) &&(!SYS_WIFIPROV_ConfigValidate(wifiProvSrvcConfig))) 
            {
                g_wifiProvSrvcConfig.mode = wifiProvSrvcConfig.mode;
                g_wifiProvSrvcConfig.saveConfig = wifiProvSrvcConfig.saveConfig;
                memcpy(&g_wifiProvSrvcConfig.apConfig, &wifiProvSrvcConfig.apConfig, sizeof(SYS_WIFIPROV_AP_CONFIG));
                SYS_WIFIPROV_WriteConfig();
                //SYS_WIFIPROV_PrintConfig();
            } 
            else 
            {
                SYS_CONSOLE_PRINT(" Wrong Command\n");
            }
        } 
        else 
        {  
            SYS_CONSOLE_PRINT(" Wrong Command\n");
        }
<#elseif SYS_WIFIPROV_STA_ENABLE == true>
        if (SYS_WIFIPROV_STA == (SYS_WIFIPROV_MODE) strtol(argv[2], NULL, 0)) 
        {
            wifiProvSrvcConfig.mode = strtol(argv[2], NULL, 0);
            wifiProvSrvcConfig.saveConfig = strtol(argv[3], NULL, 0);
            if (strlen((const char *) argv[4]) <= sizeof (wifiProvSrvcConfig.countryCode)) 
            {
                parser_data((uint8_t *) argv[4], wifiProvSrvcConfig.countryCode, sizeof(wifiProvSrvcConfig.countryCode), val, 0);
            } 
            else
            {
                error = true;
            }
            wifiProvSrvcConfig.staConfig.channel = strtol(argv[5], NULL, 0);
            wifiProvSrvcConfig.staConfig.autoConnect = strtol(argv[6], NULL, 0);
            wifiProvSrvcConfig.staConfig.authType = strtol(argv[7], NULL, 0);
            if (strlen((const char *) argv[8]) <= sizeof (wifiProvSrvcConfig.staConfig.ssid)) 
            {
                parser_data((uint8_t *) argv[8], wifiProvSrvcConfig.staConfig.ssid, sizeof(wifiProvSrvcConfig.staConfig.ssid), val, 0);
            } 
            else 
            {
                error = true;
            }

            if (argc == 10) 
            {
                if (strlen((const char *) argv[9]) <= sizeof (wifiProvSrvcConfig.staConfig.psk)) 
                {
                    parser_data((uint8_t *) argv[9], wifiProvSrvcConfig.staConfig.psk, sizeof(wifiProvSrvcConfig.staConfig.psk), val, 0);
                } 
                else
                {
                    error = true;
                }
            } 
            else 
            {
                memset(wifiProvSrvcConfig.staConfig.psk, 0, sizeof (wifiProvSrvcConfig.staConfig.psk));
            }

            if ((!error) && (!SYS_WIFIPROV_ConfigValidate(wifiProvSrvcConfig))) 
            {
                g_wifiProvSrvcConfig.mode = wifiProvSrvcConfig.mode;
                g_wifiProvSrvcConfig.saveConfig = wifiProvSrvcConfig.saveConfig;
                memcpy(&g_wifiProvSrvcConfig.staConfig, &wifiProvSrvcConfig.staConfig, sizeof(SYS_WIFIPROV_STA_CONFIG));
                SYS_WIFIPROV_WriteConfig();
                //SYS_WIFIPROV_PrintConfig();
            } 
            else 
            {
                SYS_CONSOLE_PRINT(" Wrong Command\n");
            }
        }
<#elseif SYS_WIFIPROV_AP_ENABLE == true>
        if (SYS_WIFIPROV_AP == (SYS_WIFIPROV_MODE) strtol(argv[2], NULL, 0)) 
        {
            wifiProvSrvcConfig.mode = strtol(argv[2], NULL, 0);
            wifiProvSrvcConfig.saveConfig = strtol(argv[3], NULL, 0);
            if (strlen((const char *) argv[4]) <= sizeof (wifiProvSrvcConfig.countryCode)) 
            {
                parser_data((uint8_t *) argv[4], wifiProvSrvcConfig.countryCode, sizeof(wifiProvSrvcConfig.countryCode), val, 0);
            } 
            else 
            {
                error = true;
            }
            wifiProvSrvcConfig.apConfig.channel = strtol(argv[5], NULL, 0);
            wifiProvSrvcConfig.apConfig.ssidVisibility = strtol(argv[6], NULL, 0);
            wifiProvSrvcConfig.apConfig.authType = strtol(argv[7], NULL, 0);

            if (strlen((const char *) argv[8]) <= sizeof (wifiProvSrvcConfig.apConfig.ssid)) 
            {
                parser_data((uint8_t *) argv[8], wifiProvSrvcConfig.apConfig.ssid, sizeof(wifiProvSrvcConfig.apConfig.ssid), val, 0);
            }
            else 
            {
                error = true;
            }
            
            if (argc == 10) 
            {
                if (strlen((const char *)argv[9]) <= sizeof(wifiProvSrvcConfig.apConfig.psk)) 
                {
                    parser_data((uint8_t *)argv[9],wifiProvSrvcConfig.apConfig.psk,sizeof(wifiProvSrvcConfig.apConfig.psk),val,0);
                } 
                else 
                {
                    error = true;
                }
            } 
            else 
            {
                memset(wifiProvSrvcConfig.apConfig.psk, 0, sizeof (wifiProvSrvcConfig.apConfig.psk));
            }

            if ((!error) &&(!SYS_WIFIPROV_ConfigValidate(wifiProvSrvcConfig))) 
            {
                g_wifiProvSrvcConfig.mode = wifiProvSrvcConfig.mode;
                g_wifiProvSrvcConfig.saveConfig = wifiProvSrvcConfig.saveConfig;
                memcpy(&g_wifiProvSrvcConfig.apConfig, &wifiProvSrvcConfig.apConfig, sizeof(SYS_WIFIPROV_AP_CONFIG));
                SYS_WIFIPROV_WriteConfig();
                //SYS_WIFIPROV_PrintConfig();
            } 
            else 
            {
                SYS_CONSOLE_PRINT(" Wrong Command\n");
            }
        } 
        else 
        {  
            SYS_CONSOLE_PRINT(" Wrong Command\n");
        }
</#if>
    } 
    else if ((argc == 2) && (!strcmp(argv[1], "get"))) 
    {
        SYS_WIFIPROV_PrintConfig();
<#if SYS_WIFIPROV_APPDEBUG_ENABLE == true>
    }
    else if((argc >= 2) && (!strcmp((char*)argv[1],"debug"))) 
    {
        if ((argv[2] == NULL) || (!strcmp((char*)argv[2],"?"))) 
        {
            SYS_CONSOLE_MESSAGE("\n\rFollowing debug Command:");
            SYS_CONSOLE_MESSAGE("\n\r\t* wifiprov debug level <value_level_in_hex>");
            SYS_CONSOLE_MESSAGE("\n\r\t* wifiprov debug flow <value_flow_in_hex>");
        } 
        else if ((!strcmp((char*)argv[2],"level"))) 
        {
            int32_t appDebugRet = SYS_APPDEBUG_FAILURE;
            g_wifiSrvcAppDbgCfg.logLevel = strtoul( argv[3], 0, 16);
            appDebugRet = SYS_APPDEBUG_CtrlMsg(g_wifiSrvcAppDebugHdl, 
                    SYS_APPDEBUG_CTRL_MSG_TYPE_SET_LEVEL, 
                    &g_wifiSrvcAppDbgCfg.logLevel, 4);
            if (appDebugRet != SYS_APPDEBUG_SUCCESS) 
            {
                SYS_CONSOLE_PRINT("Failed to set the Level as 0x%x\r\n", g_wifiSrvcAppDbgCfg.logLevel);
            }
            SYS_CONSOLE_MESSAGE("\r\n");
        } 
        else if ((!strcmp((char*)argv[2],"flow"))) 
        {
            int32_t appDebugRet = SYS_APPDEBUG_FAILURE;
            g_wifiSrvcAppDbgCfg.logFlow = strtoul( argv[3], 0, 16);
            appDebugRet = SYS_APPDEBUG_CtrlMsg(g_wifiSrvcAppDebugHdl, 
                    SYS_APPDEBUG_CTRL_MSG_TYPE_SET_FLOW, 
                    &g_wifiSrvcAppDbgCfg.logFlow, 4);
            if (appDebugRet != SYS_APPDEBUG_SUCCESS) 
            {
                SYS_CONSOLE_PRINT("Failed to set the Flow as 0x%x\r\n", g_wifiSrvcAppDbgCfg.logFlow);
            }
            SYS_CONSOLE_MESSAGE("\r\n");
        }
</#if>
    } 
    else 
    {
        SYS_CONSOLE_PRINT(" Wrong Command\n"); 
    }
    return SYS_WIFIPROV_SUCCESS;
}

static int SYS_WIFIPROV_CMDHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) 
{
    SYS_CONSOLE_MESSAGE("\r\nUsage information:");
    SYS_CONSOLE_MESSAGE("\r\nwifiprov set <bootmode> <save config> <countryCode> <channel> <autoConnect>/<ssidVisibility> <authtype> <ssid_name> <psk_name>\r\n\r\n");
    SYS_CONSOLE_MESSAGE("bootmode                     : 0 -STA,1 - AP \r\n\r\n"
                        "save config                  : 1- save wi-fi config in NVM\r\n"
                        "                               0- Don't save config in NVM\r\n\r\n"
                        "country code                 : Regulatory domain country code configuration \r\n\r\n"
                        "                               GEN - General(Used for Production Test)\r\n"
                        "                               USA - North America \r\n\r\n"
                        "                               EMEA - Europe \r\n\r\n"
                        "                               JPN - Japan \r\n\r\n"            
                        "channel                      : 0 - Enable all channel(STA mode Only)\r\n"
                        "                               1 to 13 - set specific channel \r\n\r\n"
                        "auto connect (STA mode only) : 1- auto connect to HomeAP\r\n"
                        "                               0- Wait for user input \r\n"
                        "ssidVisibility(AP mode only): 1-broadcast SSID\r\n"
                        "                               0-hidden SSID \r\n"
                        "authtype                     : 1-Open\r\n"
                        "                               2-WEP\r\n"
                        "                               3-Mixed mode(WPA/WPA2)\r\n"
                        "                               4-WPA2 \r\n"
                        "                               5-Mixed mode(WPA2/WPA3) \r\n"
                        "                               6-WPA3 \r\n"
                        "ssid_name                    : SSID name \r\n"
                        "psk                          : Passphrase\r\n\r\n");
    SYS_CONSOLE_MESSAGE("Example STA Mode             : wifiprov set 0 1 \"GEN\" 1 1 1 \"DEMO_AP\" \"password\" \r\n");
    SYS_CONSOLE_MESSAGE("Example AP Mode              : wifiprov set 1 1 \"GEN\" 1 1 1 \"DEMO_SOFTAP\" \"password\" \r\n");
    SYS_CONSOLE_MESSAGE("wifiprov get                 : Get WiFi Provision Configuration \r\n");
    SYS_CONSOLE_MESSAGE("wifiprov debug level <value> : Set WiFi Provision Debug level value in hex \r\n");
    SYS_CONSOLE_MESSAGE("wifiprov debug flow <value>  : Set WiFi Provision Debug flow value in hex \r\n");
    return SYS_WIFIPROV_SUCCESS;
}
</#if>

<#if SYS_WIFIPROV_CMD == true >

/* Parse the command data(sbuff) and update in the buffer(dbuff) */
static uint8_t parser_data
(
    uint8_t sbuff[], 
    uint8_t dbuff[], 
    uint8_t dbufflen, 
    uint8_t val, 
    uint8_t offset
) 
{
    uint8_t idx1 = offset + 1, idx2 = offset, idx3 = 0;
    memset(dbuff, 0, dbufflen);

    for (; (sbuff[idx2] == val) && (sbuff[idx1] != val);) 
    {
        dbuff[idx3++] = sbuff[idx1++];
        /* SYS CMD service replacing space with NULL */
        if (dbuff[idx3 - 1] == 0x00) /* check NUll in the data string */
        {
            /* Replace NULL(0x00) with space in the data string */
            dbuff[idx3 - 1] = 0x20;  
        }
    }
    dbuff[idx3] = '\0';
    return idx3 + 1;
}
</#if>

<#if SYS_WIFIPROV_SOCKET == true>
static void SYS_WIFIPROV_DataUpdate(uint8_t buffer[]) 
{
    struct json_obj root, child, sub;
    SYS_WIFIPROV_CONFIG wifiProvSrvcConfig;
    bool error = false;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET, "received JSON data=%s \r\n",buffer);
</#if>
   
    if (buffer) 
    {
        /* Creating JSON object to parse incoming JSON data */
        if (!json_create(&root, (const char*) buffer, strlen((const char*) buffer))) 
        {
            memset(&wifiProvSrvcConfig, 0, sizeof (SYS_WIFIPROV_CONFIG));
            /* Verifying JSON  "mode" field */
            if (!json_find(&root, "mode", &child)) 
            {
                wifiProvSrvcConfig.mode = child.value.b;
            } 
            else
            {
                error = true;
            }

            /* Verifying JSON  "save_config" field */
            if (!json_find(&root, "save_config", &child)) 
            {
                wifiProvSrvcConfig.saveConfig = child.value.b;
            } 
            else
            {
                error = true;
            }

            /* Verifying JSON  "countrycode" field */
            if (!json_find(&root, "countrycode", &child)) 
            {
                memcpy(wifiProvSrvcConfig.countryCode,child.value.s,strlen(child.value.s));
            } 
            else
            {
                error = true;
            }

<#if SYS_WIFIPROV_STA_ENABLE == true>
            /* Verifying JSON  "STA" field */
            if (!json_find(&root, "STA", &child)) 
            {
                if (!json_find(&child, "ch", &sub)) 
                {
                    wifiProvSrvcConfig.staConfig.channel = sub.value.i;
                } 
                else
                {
                    error = true;
                }

                if (!json_find(&child, "auto", &sub)) 
                {
                    wifiProvSrvcConfig.staConfig.autoConnect = sub.value.b;
                } 
                else 
                {
                    error = true;
                }

                if (!json_find(&child, "auth", &sub)) 
                {
                    wifiProvSrvcConfig.staConfig.authType = sub.value.i;
                }
                else
                {
                    error = true;
                }

                if (!json_find(&child, "SSID", &sub)) 
                {
                    if (strlen(sub.value.s) <= sizeof (wifiProvSrvcConfig.staConfig.ssid)) 
                    {
                        memcpy(wifiProvSrvcConfig.staConfig.ssid, sub.value.s, strlen(sub.value.s));
                    } 
                    else
                    {
                        error = true;
                    }
                } 
                else
                {
                    error = true;
                }

                if (!json_find(&child, "PWD", &sub)) 
                {
                    if (strlen(sub.value.s) <= sizeof (wifiProvSrvcConfig.staConfig.psk)) 
                    {
                        memcpy(wifiProvSrvcConfig.staConfig.psk, sub.value.s, strlen(sub.value.s));
                    } 
                    else
                    {
                        error = true;
                    }
                } 
                else
                {
                    error = true;
                }
            }
</#if>
<#if SYS_WIFIPROV_AP_ENABLE == true>
            /* Verifying JSON  "AP" field */
            if (!json_find(&root, "AP", &child)) 
            {
                if (!json_find(&child, "ch", &sub)) 
                {
                    wifiProvSrvcConfig.apConfig.channel = sub.value.i;
                }
                else
                {
                    error = true;
                }

                if (!json_find(&child, "ssidv", &sub)) 
                {
                    wifiProvSrvcConfig.apConfig.ssidVisibility = sub.value.b;
                }
                else
                {
                    error = true;
                }

                if (!json_find(&child, "auth", &sub)) 
                {
                    wifiProvSrvcConfig.apConfig.authType = sub.value.i;
                }
                else
                {
                    error = true;
                }

                if (!json_find(&child, "SSID", &sub)) 
                {
                    if (strlen(sub.value.s) <= sizeof (wifiProvSrvcConfig.apConfig.ssid)) 
                    {
                        memcpy(wifiProvSrvcConfig.apConfig.ssid, sub.value.s, strlen(sub.value.s));
                    } 
                    else
                    {
                        error = true;
                    }
                } 
                else
                {
                    error = true;
                }

                if (!json_find(&child, "PWD", &sub)) 
                {
                    if (strlen(sub.value.s) <= sizeof (wifiProvSrvcConfig.apConfig.psk)) 
                    {
                        memcpy(wifiProvSrvcConfig.apConfig.psk, sub.value.s, strlen(sub.value.s));
                    } 
                    else
                    {
                        error = true;
                    }
                } 
                else
                {
                    error = true;
                }
            }
</#if>
            /* Verifying JSON object error */
            if ((!error) && (!SYS_WIFIPROV_ConfigValidate(wifiProvSrvcConfig))) 
            {
                g_wifiProvSrvcConfig.mode = wifiProvSrvcConfig.mode;
                g_wifiProvSrvcConfig.saveConfig = wifiProvSrvcConfig.saveConfig;
                memcpy(g_wifiProvSrvcConfig.countryCode, wifiProvSrvcConfig.countryCode, sizeof (wifiProvSrvcConfig.countryCode));
<#if SYS_WIFIPROV_STA_ENABLE == true>
                memcpy(&g_wifiProvSrvcConfig.staConfig, &wifiProvSrvcConfig.staConfig, sizeof (SYS_WIFIPROV_STA_CONFIG));
</#if>
<#if SYS_WIFIPROV_AP_ENABLE == true>
                memcpy(&g_wifiProvSrvcConfig.apConfig, &wifiProvSrvcConfig.apConfig, sizeof (SYS_WIFIPROV_AP_CONFIG));
</#if>
                //SYS_WIFIPROV_PrintConfig();
                /* Updating Configuration into Wi-Fi Provisioning structure */
                SYS_WIFIPROV_WriteConfig();
            } else {
                SYS_CONSOLE_PRINT(" Wrong Command\n");
            }
<#if SYS_WIFIPROV_STA_ENABLE == true> 
        }  /* Parsing mobile application data format apply,<ssid>,<authtype>,<psk>, */
    else if (!strncmp((const char *) buffer, "apply", 5)) 
    {        
            char * p = strtok((char *) buffer, ",");
            p = strtok(NULL, ",");
            if (p)
            {
                strcpy((char *) wifiProvSrvcConfig.staConfig.ssid, p);
            }

            p = strtok(NULL, ",");
            if (p) 
                {
                char appAuthType = *p - '0';
                if (appAuthType == 1) /* 1-Open */
                { 
                    wifiProvSrvcConfig.staConfig.authType = SYS_WIFIPROV_OPEN;
                } 
                else if (appAuthType == 2) /* 2-WPA2 */
                {
                    wifiProvSrvcConfig.staConfig.authType = SYS_WIFIPROV_WPAWPA2MIXED;
                    p = strtok(NULL, ",");
                    if (p) 
                    {
                        strcpy((char *) wifiProvSrvcConfig.staConfig.psk, p);
                    }
                } 
                else
                {
                    error = true;
                }
            }

            /* Verifying received data error */
            if (!error) 
            {
                g_wifiProvSrvcConfig.mode = SYS_WIFIPROV_STA;
                g_wifiProvSrvcConfig.saveConfig = true;
                g_wifiProvSrvcConfig.staConfig.autoConnect = true;
                g_wifiProvSrvcConfig.staConfig.channel = 0;
                g_wifiProvSrvcConfig.staConfig.authType = wifiProvSrvcConfig.staConfig.authType;
                memcpy(g_wifiProvSrvcConfig.staConfig.ssid, wifiProvSrvcConfig.staConfig.ssid, sizeof (wifiProvSrvcConfig.staConfig.ssid));
                memcpy(g_wifiProvSrvcConfig.staConfig.psk, wifiProvSrvcConfig.staConfig.psk, sizeof (wifiProvSrvcConfig.staConfig.psk));
                /* Updating Configuration into Wi-Fi Provisioning structure */
                SYS_WIFIPROV_WriteConfig();
            }
            else 
            {
                SYS_CONSOLE_PRINT(" Wrong Command\n");
            }
</#if>
        }
    }
}

/* This function will be invoke when TCP client send any data to 
   PIC32MZW1 TCP server. */
static void SYS_WIFIPROV_Socket_CB
(
    TCP_SOCKET hTCP, 
    TCPIP_NET_HANDLE hNet, 
    TCPIP_TCP_SIGNAL_TYPE sigType, 
    const void* param
) 
{
    uint8_t buffer[512];
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET);
</#if>
    switch (sigType) 
    {
        case TCPIP_TCP_SIGNAL_RX_DATA:
        {
            memset(buffer, 0, sizeof (buffer));
            /* Reading the TCP Rx data */
            uint8_t byte = TCPIP_TCP_ArrayGet(g_wifiProvSrvcSocket, buffer, sizeof (buffer));
            if (byte) 
            {
                SYS_WIFIPROV_DataUpdate(buffer);
            }
            TCPIP_TCP_Discard(g_wifiProvSrvcSocket);
            break;
        }

        case TCPIP_TCP_SIGNAL_RX_FIN:
        {
            /* TCP client has sent the TCP RX FIN received  */
            TCPIP_TCP_Close(g_wifiProvSrvcSocket);
            g_wifiProvSrvcSocket = INVALID_SOCKET;
            /* Reopen the Socket to enable TCP Server again */
            SYS_WIFIPROV_InitSocket();
            break;
        }

        default:
        {
            break;
        }
    }
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET);
</#if>
}

static void SYS_WIFIPROV_InitSocket(void) 
{
    TCPIP_NET_HANDLE netHdl = TCPIP_STACK_NetHandleGet("PIC32MZW1");
    IPV4_ADDR ipAddr;
    ipAddr.Val = TCPIP_STACK_NetAddress(netHdl); 
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET);
</#if>            
    if (ipAddr.Val) 
    {
        /* Closed the socket if it's already open */
        if (g_wifiProvSrvcSocket != INVALID_SOCKET) 
        {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET, "Wi-Fi Provision service server socket is already open \r\n");
</#if>
            TCPIP_TCP_Close(g_wifiProvSrvcSocket);
            g_wifiProvSrvcSocket = INVALID_SOCKET;
        }
        /* Open the TCP server socket */
        g_wifiProvSrvcSocket = TCPIP_TCP_ServerOpen(IP_ADDRESS_TYPE_IPV4, SYS_WIFIPROV_SOCKETPORT, 0);
        if (g_wifiProvSrvcSocket == INVALID_SOCKET) 
        {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET, "Couldn't open Wi-Fi Provision service server socket \r\n");
<#else>
            SYS_CONSOLE_MESSAGE("Couldn't open Wi-Fi Provision service server socket \r\n");
</#if>
        }
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
        else
        {
            SYS_APPDEBUG_DBG_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET, "Wi-Fi Provision service server socket(%d) created successfully  \r\n",g_wifiProvSrvcSocket);
        }
</#if>
        /* Register TCP data callback with enabling event  
           TCPIP_TCP_SIGNAL_RX_DATA and TCPIP_TCP_SIGNAL_RX_FIN  */
        g_wifiProvSrvcHdl = TCPIP_TCP_SignalHandlerRegister(g_wifiProvSrvcSocket, TCPIP_TCP_SIGNAL_RX_DATA | TCPIP_TCP_SIGNAL_RX_FIN, SYS_WIFIPROV_Socket_CB, NULL);
        if (g_wifiProvSrvcHdl == NULL) 
        {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
            SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET, "Couldn't create socket handle\r\n");
<#else>
            SYS_CONSOLE_MESSAGE("Couldn't create socket handle\r\n");
</#if>
        }
    }
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET);
</#if>
}

static void SYS_WIFIPROV_DeInitSocket(void) 
{
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET);
</#if>
    /* De-register the data callback */
    if (TCPIP_TCP_SignalHandlerDeregister(g_wifiProvSrvcSocket, g_wifiProvSrvcHdl)) 
    {
        /* Closed the Socket */
        TCPIP_TCP_Close(g_wifiProvSrvcSocket);
    }
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONINGSOCKET);
</#if>    
}
</#if>

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WiFi Provisioning Initialize Interface
// *****************************************************************************
// *****************************************************************************
SYS_MODULE_OBJ SYS_WIFIPROV_Initialize
(
    SYS_WIFIPROV_CONFIG *config, 
    SYS_WIFIPROV_CALLBACK callback, 
    void *cookie
) 
{
    SYS_WIFIPROV_OBJ *wifiProvObj = (SYS_WIFIPROV_OBJ *) & g_wifiProvSrvcObj;
    g_wifiProvSrvcCallBack = callback;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>
    
    if (SYS_WIFIPROV_STATUS_NONE == SYS_WIFIPROV_GetTaskstatus()) 
    {
        /* Set Wi-Fi provisioning service cookie */
        SYS_WIFIPROV_SetCookie(cookie);
        SYS_WIFIPROV_InitConfig(config);
<#if SYS_WIFIPROV_CMD == true>
        /* Wi-Fi Provisioning Service Command line initialization */
        if (SYS_WIFIPROV_FAILURE == SYS_WIFIPROV_CMDInit()) 
        {
            SYS_CONSOLE_MESSAGE("Failed to create WiFi provision Commands\r\n");
        }
</#if>
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>
        return (SYS_MODULE_OBJ) wifiProvObj;
    }
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>
    return SYS_MODULE_OBJ_INVALID;
}

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WiFi Provisioning De-initialize Interface
// *****************************************************************************
// *****************************************************************************
SYS_WIFIPROV_RESULT SYS_WIFIPROV_Deinitialize
(
    SYS_MODULE_OBJ object
)
{
    uint32_t ret = SYS_WIFIPROV_FAILURE;
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>    
    if (&g_wifiProvSrvcObj != (SYS_WIFIPROV_OBJ *) object) 
    {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "Invalid object\r\n");
</#if>
        ret = SYS_WIFIPROV_OBJ_INVALID;
    } 
    else
    {
        memset(&g_wifiProvSrvcObj, 0, sizeof (SYS_WIFIPROV_OBJ));
        memset(&g_wifiProvSrvcConfig, 0, sizeof (SYS_WIFIPROV_CONFIG));
        memset(&g_wifiProvSrvcConfigRead, 0, sizeof (SYS_WIFIPROV_CONFIG));
        SYS_WIFIPROV_SetTaskstatus(SYS_WIFIPROV_STATUS_NONE);
        ret = SYS_WIFIPROV_SUCCESS;
    }
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>
    return ret;
}


// *****************************************************************************
// *****************************************************************************
// Section:  SYS WiFi Provisioning Get Status Interface
// *****************************************************************************
// *****************************************************************************
uint8_t SYS_WIFIPROV_GetStatus
(
    SYS_MODULE_OBJ object
)
{
    if (&g_wifiProvSrvcObj != (SYS_WIFIPROV_OBJ *) object) 
    {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "Invalid object\r\n");
</#if>
        return SYS_WIFIPROV_OBJ_INVALID;
    } 
    else
    {
        /* provide current status of Wi-Fi Provisioning service to client */
        return ((SYS_WIFIPROV_OBJ *) object)->status;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WiFi Provisioning Tasks Interface
// *****************************************************************************
// *****************************************************************************
uint8_t SYS_WIFIPROV_Tasks
(
    SYS_MODULE_OBJ object
)
{
    uint8_t ret = SYS_WIFIPROV_OBJ_INVALID;
    
    if (&g_wifiProvSrvcObj == (SYS_WIFIPROV_OBJ *) object) 
    {
        ret = SYS_WIFIPROV_ExecuteBlock(object);
    }
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>	 
    else
    {
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "Invalid object\r\n");
    }
</#if>
    return ret;
}

// *****************************************************************************
// *****************************************************************************
// Section:  SYS WiFi Provisioning Control Message Interface
// *****************************************************************************
// *****************************************************************************
SYS_WIFIPROV_RESULT SYS_WIFIPROV_CtrlMsg
(
    SYS_MODULE_OBJ object, 
    uint32_t event, 
    void *buffer, 
    uint32_t length
) 
{
    SYS_WIFIPROV_RESULT ret = SYS_WIFIPROV_SUCCESS;
<#if SYS_WIFIPROV_SOCKET == true>
    uint8_t *connStatus = NULL;
</#if>
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_ENTER_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>    

    if (&g_wifiProvSrvcObj != (SYS_WIFIPROV_OBJ *) object) 
    {
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
        SYS_APPDEBUG_ERR_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING, "Invalid object\r\n");
</#if>
        ret = SYS_WIFIPROV_OBJ_INVALID;
    }
    else
    {
        switch (event) 
        {
            case SYS_WIFIPROV_SETCONFIG:
            {
                if (buffer) 
                {
                    /* Client has set new Wi-Fi configuration */
                    memcpy(&g_wifiProvSrvcConfig, (SYS_WIFIPROV_CONFIG *) buffer, sizeof(SYS_WIFIPROV_CONFIG));
                }
                SYS_WIFIPROV_WriteConfig();
                break;
            }

            case SYS_WIFIPROV_GETCONFIG:
            {
                /* Client request for Get Configuration */
                SYS_WIFIPROV_CallBackFun(SYS_WIFIPROV_GETCONFIG, &g_wifiProvSrvcConfig, g_wifiProvSrvcCookie);
                break;
            }

            case SYS_WIFIPROV_CONNECT:
            {
<#if SYS_WIFIPROV_SOCKET == true>
                connStatus = (uint8_t *) buffer;
                if (*connStatus == true)
                {
                    /* Wi-Fi Service has establish the connection,so 
                       initialization the TCP Server socket. */
                    SYS_WIFIPROV_InitSocket();
</#if>
<#if (SYS_WIFIPROV_HTTP == true) && (SYS_WIFIPROV_ENABLE_HTTPNET == true)>
                    HTTP_APP_Initialize();
</#if>
<#if SYS_WIFIPROV_SOCKET == true>
                }
                else
                {
                    /* Wi-Fi Service has lost the connection,
                       so de-initialization the TCP Server socket. */
                    SYS_WIFIPROV_DeInitSocket();
                }
</#if>

                break;
            }

            default:
            {
                break;
            }
        }
    }
<#if SYS_WIFIPROV_APPDEBUG_ENABLE  == true>
    SYS_APPDEBUG_FN_EXIT_PRINT(g_wifiSrvcAppDebugHdl, WIFI_PROVISIONING);
</#if>
    return ret;
}
/* *****************************************************************************
 End of File
 */
