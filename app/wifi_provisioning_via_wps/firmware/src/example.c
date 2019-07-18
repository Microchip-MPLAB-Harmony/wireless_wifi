/*******************************************************************************
  WINC Example Application

  File Name:
    wifi_provisioning_via_wps.c

  Summary:
    Wi-Fi Provisioning via WPS Example.

  Description:
    This example demonstrates the use of the WINC1500 with the SAMD21 Xplained Pro
    board to start Wi-Fi provisioning mode via WPS

    The configuration defines for this demo are:
        MAIN_WPS_PUSH_BUTTON_FEATURE           -- Select to test WPS push button mode or WPS pin mode
        MAIN_WPS_PIN_NUMBER        -- Set the WPS pin number 
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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

/** \mainpage
 * \section intro Introduction
 * This example demonstrates the use of the WINC1500 with the SAMD21 Xplained Pro board
 * to connect to AP with WPS Security.<br>
 * It uses the following hardware:
 * - the SAMD21 Xplained Pro.
 * - the WINC1500 on EXT1.
 *
 *
 * \section usage Usage
 * -# Assemble the devices and connect to USB debug cable from PC.
 * -# On the computer, open and configure a terminal application as the follows.
 * \code
 *    Baud Rate : 115200
 *    Data : 8bit
 *    Parity bit : none
 *    Stop bit : 1bit
 *    Flow control : none
 * \endcode
 *
 * -# Prepare an AP that supports Wi-Fi Protected Setup(WPS)
 *
 * -# To test WPS button method, Use case 1 in example.c function.
 * -# Configure below code in the example_conf.h for WPS push button feature.
 * \code
 *    #define MAIN_WPS_PUSH_BUTTON_FEATURE              "true"
 * \endcode
 * -# Press SW0  button at SAMD21 XPRO
 * -# Press WPS button at the AP. ( For more information, please refer to AP product documentation )
 * -# Run the application. The WINC1500 will be connected to the AP automatically without security information.
 * \code
 * ===================================
 * Wi-Fi Provisioning via WPS Example
 * ===================================
 * WPS Push Button Test
 * SW0 button pressed
 * Device is connecting using WPS Push Button option
 * [APP_ExampleWPSPushButtonDiscoveryCallback] In
 * SSID : xxxxx, authtyp : x pw : xxxxxxxx
 * Connect AP, SSID = xxxxx
 * STA mode: Station connected
 * STA Mode: Station IP address is xxx.xxx.xxx.xxx

 * \endcode
 *
 * -# To test WPS PIN method, Use case 2 in example.c function.
 * -# Configure below code in the main.h for WPS PIN number and WPS push button feature.
 * \code
 *    #define WPS_PIN_NUMBER                            "12345670"
 *    #define MAIN_WPS_PUSH_BUTTON_FEATURE              "false"
 * \endcode
 * -# Enter WPS PIN number in the AP setup menu and start the AP. ( For more information, please refer to AP product documentation )
 * -# Run the application. The WINC1500 will be connected to the AP automatically without security information.
 * \code
 * ===================================
 * Wi-Fi Provisioning via WPS Example
 * ===================================
 * WPS Pin Test, the pin number is: 12345670
 * [APP_ExampleWPSPinDiscoveryCallback] In
 * SSID : xxxxx, authtyp : x pw : xxxxxxxx
 * Connect AP, SSID = xxxxx
 * STA mode: Station connected
 * STA Mode: Station IP address is xxx.xxx.xxx.xxx
 * \endcode
 */
 
#include "app.h"
#include "wdrv_winc_client_api.h"
#include "example_conf.h"

typedef enum
{
    /* Example's state machine's initial state. */
    EXAMP_STATE_INIT=0,
    EXAMP_STATE_WPS_PIN,
    EXAMP_STATE_WPS_PUSH_BUTTON, 
    EXAMP_STATE_WPS_PUSH_BUTTON_WAITING,
    EXAMP_STATE_WPS_CHECK_BUTTON,
    EXAMP_STATE_CONNECT_AP,    
    EXAMP_STATE_CONNECTING_AP,
    EXAMP_STATE_CONNECTED_AP,
    EXAMP_STATE_DISCONNECTED,
    EXAMP_STATE_ERROR,
} EXAMP_STATES;

static EXAMP_STATES state;
static WDRV_WINC_BSS_CONTEXT  bssCtx;
static WDRV_WINC_AUTH_CONTEXT authCtx;
static bool gbPressButton = false;



/**
 * \brief button pressed callback function.
 */
static void btn_press(uint8_t btn, uint8_t press)
{
	if (MAIN_SW0 == btn) {
            if (press ==  0) {
                APP_DebugPrintf("SW0 button pressed\r\n");
                state = EXAMP_STATE_WPS_PUSH_BUTTON;
            }
	}
}

/**
 * \brief Initialize buttons driver.
 */
static void btn_init(void)
{
    if (GPIO_SW0_GET() == 0)
    {
        btn_press(MAIN_SW0, 0);
        gbPressButton = true;
    }
}

static void APP_ExampleWPSPinDiscoveryCallback(DRV_HANDLE handle, WDRV_WINC_BSS_CONTEXT *pBSSCtx, WDRV_WINC_AUTH_CONTEXT *pAuthCtx)
{
   APP_DebugPrintf("[APP_ExampleWPSPinDiscoveryCallback] In\r\n"); 
    APP_DebugPrintf("SSID : %s, authtyp : %d pw : %s\r\n", pBSSCtx->ssid.name, pAuthCtx->authType, pAuthCtx->authInfo.WPAPerPSK.key);

    if (pAuthCtx == NULL) {
        APP_DebugPrintf("WPS is not enabled OR Timedout\r\n");
        return;
    }    
   memcpy(&bssCtx, pBSSCtx, sizeof(WDRV_WINC_BSS_CONTEXT));
   memcpy(&authCtx, pAuthCtx, sizeof(WDRV_WINC_AUTH_CONTEXT));
   state = EXAMP_STATE_CONNECT_AP;
}

static void APP_ExampleWPSPushButtonDiscoveryCallback(DRV_HANDLE handle, WDRV_WINC_BSS_CONTEXT *pBSSCtx, WDRV_WINC_AUTH_CONTEXT *pAuthCtx)
{
    APP_DebugPrintf("[APP_ExampleWPSPushButtonDiscoveryCallback] In\r\n");
    APP_DebugPrintf("SSID : %s, authtyp : %d pw : %s\r\n", pBSSCtx->ssid.name, pAuthCtx->authType, pAuthCtx->authInfo.WPAPerPSK.key);

    
    if (pAuthCtx == NULL) {
        APP_DebugPrintf("WPS is not enabled OR Timedout\r\n");
        state = EXAMP_STATE_WPS_CHECK_BUTTON;
        gbPressButton = false;
        return;
    }    
    memcpy(&bssCtx, pBSSCtx, sizeof(WDRV_WINC_BSS_CONTEXT));
    memcpy(&authCtx, pAuthCtx, sizeof(WDRV_WINC_AUTH_CONTEXT));
    state = EXAMP_STATE_CONNECT_AP;

}


static void APP_ExampleSTAConnectNotifyCallback(DRV_HANDLE handle, WDRV_WINC_CONN_STATE currentState, WDRV_WINC_CONN_ERROR errorCode)
{
    if (WDRV_WINC_CONN_STATE_CONNECTED == currentState)
    {
        APP_DebugPrintf("STA mode: Station connected\r\n");
        state = EXAMP_STATE_CONNECTED_AP;
        
      
        
        
    }
    else if (WDRV_WINC_CONN_STATE_DISCONNECTED == currentState)
    {
        APP_DebugPrintf("STA mode: Station disconnected\r\n");


        state = EXAMP_STATE_DISCONNECTED;
    }
}


static void APP_ExampleSTAModeDHCPAddressEventCallback(DRV_HANDLE handle, uint32_t ipAddress)
{
    char s[20];

    APP_DebugPrintf("STA Mode: Station IP address is %s\r\n", inet_ntop(AF_INET, &ipAddress, s, sizeof(s)));
}

void APP_ExampleInitialize(DRV_HANDLE handle)
{
    APP_DebugPrintf("\r\n");
    APP_DebugPrintf("===================================\r\n");
    APP_DebugPrintf("Wi-Fi Provisioning via WPS Example\r\n");
    APP_DebugPrintf("===================================\r\n");

    state = EXAMP_STATE_INIT;

}

void APP_ExampleTasks(DRV_HANDLE handle)
{

    switch (state)
    {
        case EXAMP_STATE_INIT:
        {
           
            /* Preset the error state incase any following operations fail. */

            state = EXAMP_STATE_ERROR; 
            
            char devName[] = "WINC1500_WPS";
            
            
            if (WDRV_WINC_STATUS_OK != WDRV_WINC_InfoDeviceNameSet(handle, devName))
            {
                break;
            }


            if (!MAIN_WPS_PUSH_BUTTON_FEATURE) {
                
                 APP_DebugPrintf("WPS Pin Test, the pin number is: %d\r\n", MAIN_WPS_PIN_NUMBER);
                /* case 2 WPS PIN method */
                if (WDRV_WINC_STATUS_OK != WDRV_WINC_WPSEnrolleeDiscoveryStartPIN(handle, MAIN_WPS_PIN_NUMBER, &APP_ExampleWPSPinDiscoveryCallback))
                {
                    break;
                }
                
                state = EXAMP_STATE_WPS_PIN;
                break;
            }
            
             APP_DebugPrintf("WPS Push Button Test\r\n");
            /* case 1 WPS Push Button method. */
            state = EXAMP_STATE_WPS_CHECK_BUTTON;
            break;
        }

        case EXAMP_STATE_WPS_PIN:
        {
           
            break;
        }

        case EXAMP_STATE_WPS_CHECK_BUTTON:
        {
            if (!gbPressButton) 
            {
                btn_init();
            }
            break;
        }
        
        case EXAMP_STATE_WPS_PUSH_BUTTON:
        {
            if (WDRV_WINC_STATUS_OK != WDRV_WINC_WPSEnrolleeDiscoveryStartPBC(handle, APP_ExampleWPSPushButtonDiscoveryCallback))
            {
                break;
            }
            
            APP_DebugPrintf("Device is connecting using WPS Push Button option\r\n");
            state = EXAMP_STATE_WPS_PUSH_BUTTON_WAITING;
            
            break;
        }

        case EXAMP_STATE_CONNECT_AP:
        {
            if (WDRV_WINC_STATUS_OK != WDRV_WINC_IPUseDHCPSet(handle, &APP_ExampleSTAModeDHCPAddressEventCallback))
            {
                APP_DebugPrintf("WDRV_WINC_IPUseDHCPSet() fail ...\r\n");
                break;
            }
            
            APP_DebugPrintf("Connect AP, SSID = %s\r\n",bssCtx.ssid.name);
            if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSConnect(handle, &bssCtx, &authCtx, APP_ExampleSTAConnectNotifyCallback))
            {
                break;
            }
            state = EXAMP_STATE_CONNECTING_AP;
            break;
        }

        case EXAMP_STATE_ERROR:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}

// DOM-IGNORE-END
