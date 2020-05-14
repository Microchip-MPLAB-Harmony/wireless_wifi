/*******************************************************************************
  Sample Application

  File Name:
    wdrv_pic32mzw_cmdintf.c

  Summary:
    This file contains functions to process the commands and trigger the
    corresponding API's in WiFi library.

  Description:

 *******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2020 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END
#include <ctype.h>
#include "system_config.h"
#include "system_definitions.h"
#include "wdrv_pic32mzw_client_api.h"

#if defined(TCPIP_STACK_COMMAND_ENABLE)

typedef void (*WifiCmdFunction)( ) ;

#define MACADDRESS_LENGTH   6
#define CONFIG_LOCATION     0xB00FB000
#define OTA_JUMP_ADDRESS    0x900FD000

typedef struct {
    char *WiFiCmd;
    WifiCmdFunction WiFiCmdFn;
}wifiCmd;

typedef struct wifiConfiguration{
    WDRV_PIC32MZW_AUTH_CONTEXT authCtx;
    WDRV_PIC32MZW_BSS_CONTEXT  bssCtx;
}wifiConfig;

typedef struct
{
    bool (*_WiFiCmdIntf_ScanCallback)(DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, WDRV_PIC32MZW_BSS_INFO *pBSSInfo);
    bool (*_WiFiCmdIntf_RssiCallback)(DRV_HANDLE handle, WDRV_PIC32MZW_ASSOC_HANDLE assocHandle, int8_t rssi);
    bool (*PIC32_NVM_PageEraseConfigWriteCallback)(uint8_t *buf, size_t size, uint32_t destinationAddress);
}drvCallback;


static int WLANCMDProcessing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static int WLANCMDHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);

static void _WiFiCmdIntf_SSID(int argc,char *argv[]);
static void _WiFiCmdIntf_Authentication(int argc,char *argv[]);
static void _WiFiCmdIntf_Passphrase(int argc,char *argv[]);
static void _WiFiCmdIntf_MAC_Address(int argc,char *argv[]);
static void _WiFiCmdIntf_Scan(int argc,char *argv[]);
static void _WiFiCmdIntf_Channel(int argc,char *argv[]);
static void _WiFiCmdIntf_RSSI(int argc,char *argv[]);
static void _WiFiCmdIntf_Connect();
static void _WiFiCmdIntf_Config(int argc,char *argv[]);
static void _WiFiCmdIntf_Register_Handlers(drvCallback *drvCallbackObj);
static bool _WiFiCmdIntf_BSSCtxIsValid(void);
static bool _WiFiCmdIntf_Initialization(DRV_HANDLE wdrvHandle);
static void _WiFiCmdIntf_BSS_Set_Defaults(void);
static bool _WiFiCmdIntf_Command_Process(int argc,char *argv[]);
static void _WiFiCmdIntf_Command_Help();

static const SYS_CMD_DESCRIPTOR    WiFiCmdTbl[]=
{
    {"wlan",     WLANCMDProcessing,              ": WLAN MAC commands processing"},
    {"wlanhelp",  WLANCMDHelp,                   ": WLAN MAC commands help "},
};

/*******************************************************************************
* Global data
*******************************************************************************/
extern uint8_t g_macaddress[6];

/*******************************************************************************
* Local data
*******************************************************************************/

static wifiConfig g_wifiConfig;
static DRV_HANDLE wifiDrvHandle;
static WDRV_PIC32MZW_AUTH_TYPE authenticationType;
static WDRV_PIC32MZW_ASSOC_HANDLE drvAssocHandle;
static drvCallback *drvCallbackReg;
static char bufferNVM[1024];

static wifiCmd wifiCmdTable []=
{
    {"ssid",            _WiFiCmdIntf_SSID           },
    {"channel",         _WiFiCmdIntf_Channel        },
    {"authentication",  _WiFiCmdIntf_Authentication },
    {"passphrase",      _WiFiCmdIntf_Passphrase     },
    {"macaddr",         _WiFiCmdIntf_MAC_Address    },
    {"scan",            _WiFiCmdIntf_Scan           },
    {"rssi",            _WiFiCmdIntf_RSSI           },
    {"config",          _WiFiCmdIntf_Config         }
};

/*******************************************************************************
* Function definitions
*******************************************************************************/

static char *_WiFiCmdIntf_ReadNVM(unsigned long location){

    memset(bufferNVM, '\0', 1024);

    volatile unsigned char *readPointer;
    readPointer = (unsigned char *)location;
    int i=0;
    for(i=0;i<1024;i++){
        bufferNVM[i] = *(readPointer+i);
    }

    return bufferNVM;
}

static void _WiFiCmdIntf_Register_Handlers(drvCallback *drvCb)
{
    drvCallbackReg = drvCb;
}

static void _WiFiCmdIntf_ConnectCallback(DRV_HANDLE handle, WDRV_PIC32MZW_ASSOC_HANDLE assocHandle, WDRV_PIC32MZW_CONN_STATE currentState)
{
    drvAssocHandle = assocHandle;
}

static void _WiFiCmdIntf_BSS_Set_Defaults()
{
    /* Initialize the BSS context to use default values. */

    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSCtxSetDefaults(&g_wifiConfig.bssCtx))
    {
        SYS_CONSOLE_MESSAGE("\r\n ERROR: Unable to set BSS defaults");
    }
}

static void _WiFiCmdIntf_Scan(int argc,char *argv[])
{
    uint8_t channel;

    if(strcmp(argv[1],"start") || argv[3] == NULL)
    {
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan start scan <CHANNEL NEMBER>>");
        SYS_CONSOLE_MESSAGE("\r\n CHANNEL NEMBER - The channel number (1 to 13) to scan on, or 255 for all channels");
        return;
    }
    channel = strtol(argv[3], NULL, 0);

    if ((channel > 13) && (channel < 255))
    {
        SYS_CONSOLE_MESSAGE("\r\n Enter the channel to scan: 1-13 or 255");
        return;
    }

    if (255 == channel)
    {
        channel = WDRV_PIC32MZW_CID_ANY;
    }

    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSFindFirst(wifiDrvHandle, channel, true, drvCallbackReg->_WiFiCmdIntf_ScanCallback))
    {
        SYS_CONSOLE_MESSAGE("\r\n ERROR: Unable to start scan");
    }
}

static void _WiFiCmdIntf_SSID(int argc,char *argv[])
{
    uint8_t ssidLength;

    if(strcmp(argv[1],"set") || argv[3] == NULL)
    {
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan set ssid <SSID>>");
        SYS_CONSOLE_MESSAGE("\r\n SSID - The name of AP to connect");
        return;
    }

    ssidLength = strlen(argv[3]);

    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSCtxSetSSID(&g_wifiConfig.bssCtx, argv[3], ssidLength))
    {
        SYS_CONSOLE_MESSAGE("\n\r ERROR: Unable to set SSID");
    }
}

static void _WiFiCmdIntf_Channel(int argc,char *argv[])
{
    uint8_t channel;

    if(strcmp(argv[1],"set") || argv[3] == NULL)
    {
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan set channel <CHANNEL NEMBER>>");
        SYS_CONSOLE_MESSAGE("\r\n CHANNEL NEMBER - The channel number 1 to 13, sets the device in the selected channel");
        SYS_CONSOLE_MESSAGE("\r\n                - or 255 to search for AP in channels from 1 to 13");
        return;
    }

    channel = strtol(argv[3], NULL, 0);

    if ((channel > 13) && (channel < 255))
    {
        SYS_CONSOLE_MESSAGE("\r\n Set the channel number between 1 to 13 or 255>");
        return;
    }

    if (255 == channel)
    {
        channel = WDRV_PIC32MZW_CID_ANY;
    }

    if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_BSSCtxSetChannel(&g_wifiConfig.bssCtx, channel))
    {
        SYS_CONSOLE_MESSAGE("\n\r ERROR: Unable to set Channel");
    }

}

static void _WiFiCmdIntf_Authentication(int argc,char *argv[])
{
    WDRV_PIC32MZW_AUTH_TYPE authType;

    if(strcmp(argv[1],"set") || argv[3] == NULL)
    {
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan set authentication <AUTHENTICATION TYPE>>");
        SYS_CONSOLE_MESSAGE("\r\n AUTHENTICATION TYPE - 1 - Open");
        SYS_CONSOLE_MESSAGE("\r\n                     - 2 - WEP");
        SYS_CONSOLE_MESSAGE("\r\n                     - 3 - WPA/WPA2");
        SYS_CONSOLE_MESSAGE("\r\n                     - 4 - WPA2");
        return;
    }

    authType = strtol(argv[3], NULL, 0);

    if(authType < WDRV_PIC32MZW_AUTH_TYPE_OPEN || authType > WDRV_PIC32MZW_AUTH_TYPE_MAX)
    {
        SYS_CONSOLE_MESSAGE("\r\n Enter valid Authentication type");
        SYS_CONSOLE_MESSAGE("\r\n AUTHENTICATION TYPE - 1 - Open");
        SYS_CONSOLE_MESSAGE("\r\n                     - 2 - WEP");
        SYS_CONSOLE_MESSAGE("\r\n                     - 3 - WPA/WPA2");
        SYS_CONSOLE_MESSAGE("\r\n                     - 4 - WPA2");
        return;
    }
    else
    {
        authenticationType = authType;
    }
}

static void _WiFiCmdIntf_Passphrase(int argc,char *argv[])
{

    if(strcmp(argv[1],"set") || argv[3] == NULL)
    {
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan set passphrase <PASSPHRASE>>");
        SYS_CONSOLE_MESSAGE("\r\n PASSPHRASE - The passphrase of AP");
        return;
    }

    switch (authenticationType)
    {
        case 1:
        {
            /* Initialize the authentication context for open mode. */

            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetOpen(&g_wifiConfig.authCtx))
            {
                SYS_CONSOLE_MESSAGE("\n\r ERROR: Unable to set Authentication to Open");
            }
            break;
        }

        case 2:
        {
            char* pWEPIdx;
            char* pWEPKey;
            uint8_t wepIdx;

            pWEPIdx = strtok(argv[3], "*");

            if (NULL == pWEPIdx)
            {
                SYS_CONSOLE_MESSAGE("\n\r ERROR: WEP index NULL");
                return;
            }

            pWEPKey = strtok(NULL, "\0");

            if (NULL == pWEPKey)
            {
                SYS_CONSOLE_MESSAGE("\n\r ERROR: WEP key error");
                return;;
            }

            /* Initialize the authentication context for WEP. */

            wepIdx = strtol(pWEPIdx, NULL, 0);

            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetWEP(&g_wifiConfig.authCtx, wepIdx, (uint8_t*)pWEPKey, strlen(pWEPKey)))
            {
                SYS_CONSOLE_MESSAGE("\n\r ERROR: Unable to set authentication to WEP");
            }
            break;
        }

        case 3:
        {
            /* Initialize the authentication context for WPA. */

            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, (uint8_t*)argv[3], strlen(argv[3]), WDRV_PIC32MZW_AUTH_TYPE_WPAWPA2_PERSONAL))
            {
                SYS_CONSOLE_MESSAGE("\n\r ERROR: Unable to set authentication to WPA/WPA2");
            }
            break;
        }

        case 4:
        {
            /* Initialize the authentication context for WPA2. */

            if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, (uint8_t*)argv[3], strlen(argv[3]), WDRV_PIC32MZW_AUTH_TYPE_WPA2_PERSONAL))
            {
                SYS_CONSOLE_MESSAGE("\n\r ERROR: Unable to set authentication to WPA2");
            }
            break;
        }

        default:
        {
            SYS_CONSOLE_PRINT("\n\r ERROR: Set Authentication type");
        }
    }

}

static void _WiFiCmdIntf_MAC_Address(int argc,char *argv[])
{
    char* argv_ptr;
    uint8_t i;

    if(strcmp(argv[1],"set") || argv[3] == NULL)
    {
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan set macaddr <MAC ADDRESS>>");
        SYS_CONSOLE_MESSAGE("\r\n MAC ADDRESS - The MAC address is expected in the format - 11:22:33:44:55:66");
        return;
    }

    argv_ptr = argv[3];

    for(i=0; i<6; i++)
    {
        g_macaddress[6-i-1] = strtol((argv_ptr + i*3), NULL, 16);
    }
}

static void _WiFiCmdIntf_RSSI(int argc,char *argv[])
{
    uint8_t status;
    if(strcmp(argv[1],"get"))
    {
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan get rssi>");
        return;
    }
    if(drvAssocHandle == NULL)
    {
        SYS_CONSOLE_MESSAGE("\r\n Device is not associated yet!");
        return;
    }
    status = WDRV_PIC32MZW_AssocRSSIGet(drvAssocHandle, NULL, (WDRV_PIC32MZW_ASSOC_RSSI_CALLBACK)drvCallbackReg->_WiFiCmdIntf_RssiCallback);
}

static void _WiFiCmdIntf_Config(int argc,char *argv[])
{
    uint8_t status;
    WDRV_PIC32MZW_AUTH_CONTEXT _authCtx = g_wifiConfig.authCtx;
    WDRV_PIC32MZW_BSS_CONTEXT  _bssCtx = g_wifiConfig.bssCtx;

    if(argv[2] == NULL)
    {
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan config save>  - save the configuration (SSID, Channel, Passphrase) to NVM");
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan config read>  - read the configuration stored in NVM");
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan config clear> - clear the configuration stored in NVM");
        SYS_CONSOLE_MESSAGE("\r\n Usage: <wlan apply config> - The device attempts to connect to the set configuration");
        return;
    }

    if(!strcmp(argv[1],"config"))
    {
        if(!strcmp(argv[2],"save"))
        {
            drvCallbackReg->PIC32_NVM_PageEraseConfigWriteCallback((unsigned char *)&g_wifiConfig,sizeof(wifiConfig),CONFIG_LOCATION);
            SYS_CONSOLE_PRINT("\n\rSaved Configuration into NVM!!!");
        }
        else if(!strcmp("clear",argv[2]))
        {
            wifiConfig   tempwifiConfig;
            memset((void *)&tempwifiConfig, -1, sizeof(wifiConfig));
            if (drvCallbackReg->PIC32_NVM_PageEraseConfigWriteCallback((uint8_t *)&tempwifiConfig, sizeof(wifiConfig),CONFIG_LOCATION ))
                SYS_CONSOLE_MESSAGE("\n\rSuccessfully Config Deleted from NVM");
            else
                SYS_CONSOLE_MESSAGE("\n\rFailed Config Deletion from NVM!!");
        }
        else if(!strcmp("read",argv[2]))
        {
            wifiConfig *temp;
            temp = (wifiConfig  *) _WiFiCmdIntf_ReadNVM(CONFIG_LOCATION);

            SYS_CONSOLE_PRINT("\n\rRead from NVM");
            SYS_CONSOLE_PRINT("\n\r--------------");
            SYS_CONSOLE_PRINT("\n\rSSID:%s" \
                    "\n\rSSID Length :%d" \
                    "\n\rChannel:%d" ,
                    temp->bssCtx.ssid.name,temp->bssCtx.ssid.length,temp->bssCtx.channel);
            SYS_CONSOLE_PRINT("\n\r--------------");
        }
    }

    if(!strcmp(argv[1],"apply"))
    {
        if(!strcmp(argv[2],"config"))
        {
            status = WDRV_PIC32MZW_BSSConnect(wifiDrvHandle, &_bssCtx, &_authCtx, _WiFiCmdIntf_ConnectCallback);
        }
    }
}

static void _WiFiCmdIntf_Connect()
{
    uint8_t status;
    status = WDRV_PIC32MZW_BSSConnect(wifiDrvHandle, &g_wifiConfig.bssCtx, &g_wifiConfig.authCtx, _WiFiCmdIntf_ConnectCallback);
}

static bool _WiFiCmdIntf_BSSCtxIsValid(void)
{
    return WDRV_PIC32MZW_BSSCtxIsValid(&g_wifiConfig.bssCtx, true);
}

bool WDRV_PIC32MZW_CmdIntfInit(DRV_HANDLE wdrvHandle)
{
    wifiDrvHandle = wdrvHandle;
    
    if (!SYS_CMD_ADDGRP(WiFiCmdTbl, sizeof(WiFiCmdTbl)/sizeof(*WiFiCmdTbl), "wlan", ": WLAN commands"))
    {
        SYS_ERROR(SYS_ERROR_ERROR, "Failed to create WLAN Commands\r\n", 0);
        return false;
    }

    SYS_CONSOLE_PRINT("WDRV_PIC32MZW_CmdIntfInit:\r\n");

    return true;
}

static void _WiFiCmdIntf_Command_Help()
{
    SYS_CONSOLE_MESSAGE("\r\n Usage information:");
    SYS_CONSOLE_MESSAGE("\r\n wlan set ssid <SSID>               - sets the SSID");
    SYS_CONSOLE_MESSAGE("\r\n wlan set authentication <SSID>     - sets the authentication type");
    SYS_CONSOLE_MESSAGE("\r\n wlan set passphrase <PASSPHRASE>   - sets the passphrase");
    SYS_CONSOLE_MESSAGE("\r\n wlan set channel <CHANNEL NUMBER>  - sets the channel to connect to an AP");
    SYS_CONSOLE_MESSAGE("\r\n wlan apply config                  - connect to the set SSID");
    SYS_CONSOLE_MESSAGE("\r\n                                    - configure SSID, authentication, passphrase and channel before <wlan apply connfig>");
    SYS_CONSOLE_MESSAGE("\r\n wlan config save                   - save the wlan configuration in NVM");
    SYS_CONSOLE_MESSAGE("\r\n wlan config read                   - reads the wlan configuration stored in NVM");
    SYS_CONSOLE_MESSAGE("\r\n wlan config clear                  - deletes the wlan configuration stored in NVM");
    SYS_CONSOLE_MESSAGE("\r\n wlan start scan <CHANNEL NUMBER>   - sets the channel to scan");
    SYS_CONSOLE_MESSAGE("\r\n wlan set macaddr <MACADDR>         - sets the MAC address of WiFI interface");
    SYS_CONSOLE_MESSAGE("\r\n wlan get rssi                      - gets the RSSI of associated AP");

    SYS_CONSOLE_MESSAGE("\r\n\r\n **************Enter the 1st and 3rd argument to get more information about the command and how to use it");
    SYS_CONSOLE_MESSAGE("\r\n **************Example: <wlan macaddr> would give more information on how to use it \r\n");
}

static bool _WiFiCmdIntf_Command_Process(int argc,char *argv[])
{
    uint8_t i=0;

    if(argc >= 2)
    {
        for(i=0; i<sizeof(wifiCmdTable)/sizeof(*wifiCmdTable); i++)
        {
            if(!strcmp(argv[1],wifiCmdTable[i].WiFiCmd))
            {
                wifiCmdTable[i].WiFiCmdFn(argc, argv);
                return;
            }
        }
        if(argc > 2)
            for(i=0; i<sizeof(wifiCmdTable)/sizeof(*wifiCmdTable); i++)
            {
                if(!strcmp(argv[2],wifiCmdTable[i].WiFiCmd))
                {
                    wifiCmdTable[i].WiFiCmdFn(argc, argv);
                    return;
                }
            }
    }
    else
    {
        // Do nothing
    }

    _WiFiCmdIntf_Command_Help();
}

static int WLANCMDProcessing(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {

    _WiFiCmdIntf_Command_Process(argc,argv);

    return 0;
}

static static int WLANCMDHelp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv){

    _WiFiCmdIntf_Command_Help();
    return 0;
}

#endif
