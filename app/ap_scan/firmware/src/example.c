/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*******************************************************************************
  WINC Example Application

  File Name:
    example.c

  Summary:
    WINC scan and connection example

  Description:
     This example performs the following steps:
        1) Scans all channels looking for the specified BSS
        2) Displays all found BSSs
        3) Connects to the specified BSS
        4) Sends one or more ICMP echo requests to a known IP address

    The configuration options for this example are:
        WLAN_SSID           -- BSS to search for
        WLAN_AUTH           -- Security for the BSS
        WLAN_PSK            -- Passphrase for WPA security
        ICMP_ECHO_TARGET    -- IP address to send ICMP echo request to
        ICMP_ECHO_COUNT     -- Number of ICMP echo requests to send
        ICMP_ECHO_INTERVAL  -- Time between ICMP echo requests, in milliseconds
*******************************************************************************/

#include "app.h"
#include "driver/winc/include/wdrv_winc_client_api.h"
#include "m2m_wifi.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_gpio.h"


#define WLAN_SSID           "MicrochipDemoAp"
#define WLAN_AUTH_WPA_PSK    M2M_WIFI_SEC_WPA_PSK
#define WLAN_PSK            "12345678"

//#define ICMP_ECHO_TARGET    "192.168.1.1"
//#define ICMP_ECHO_COUNT     3
//#define ICMP_ECHO_INTERVAL  100
void APP_DebugPrintf(const char* format, ...);

typedef enum
{
    /* Example's state machine's initial state. */
    EXAMP_STATE_INIT=0,
    EXAMP_STATE_SCANNING,
    EXAMP_STATE_SCAN_GET_RESULTS,
    EXAMP_STATE_SCAN_DONE,
    EXAMP_STATE_CONNECTING,
    EXAMP_STATE_CONNECTED,
    EXAMP_STATE_ICMP_ECHO_REQUEST_SENT,
    EXAMP_STATE_DISCONNECTED,
    EXAMP_STATE_ERROR,
} EXAMP_STATES;

static EXAMP_STATES state;
static bool foundBSS;
#ifdef ICMP_ECHO_TARGET
static uint8_t icmpEchoCount;
static uint32_t icmpEchoReplyTime;
#endif

#ifdef ICMP_ECHO_TARGET
static void APP_ExampleICMPEchoResponseCallback(DRV_HANDLE handle, uint32_t ipAddress, uint32_t RTT, WDRV_WINC_ICMP_ECHO_STATUS statusCode)
{
    /* Report the results of the ICMP echo request. */

    switch (statusCode)
    {
        case WDRV_WINC_ICMP_ECHO_STATUS_SUCCESS:
        {
            APP_DebugPrintf("ICMP echo request successful; RTT = %ld ms\r\n", RTT);
            break;
        }

        case WDRV_WINC_ICMP_ECHO_STATUS_UNREACH:
        {
            APP_DebugPrintf("ICMP echo request failed; destination unreachable\r\n");
            break;
        }

        case WDRV_WINC_ICMP_ECHO_STATUS_TIMEOUT:
        {
            APP_DebugPrintf("ICMP echo request failed; timeout\r\n");
            break;
        }
    }

    /* Reduce the number of ICMP echo requests remaining and store the time
     this response was received. */

    icmpEchoCount--;

    if (icmpEchoCount > 0)
    {
        icmpEchoReplyTime = SYS_TMR_TickCountGet();
    }
}
#endif

static void APP_ExampleDHCPAddressEventCallback(DRV_HANDLE handle, uint32_t ipAddress)
{
    char s[20];

    APP_DebugPrintf("IP address is %s\r\n", inet_ntop(AF_INET, &ipAddress, s, sizeof(s)));
}

static void APP_ExampleConnectNotifyCallback(DRV_HANDLE handle, WDRV_WINC_CONN_STATE currentState, WDRV_WINC_CONN_ERROR errorCode)
{
    if (WDRV_WINC_CONN_STATE_CONNECTED == currentState)
    {
        /* When connected reset the ICMP echo request counter and state. */
        APP_DebugPrintf("Connected\r\n");

#ifdef ICMP_ECHO_TARGET
        icmpEchoCount     = ICMP_ECHO_COUNT;
        icmpEchoReplyTime = 0;
#endif

        state = EXAMP_STATE_CONNECTED;
    }
    else if (WDRV_WINC_CONN_STATE_DISCONNECTED == currentState)
    {
        APP_DebugPrintf("Disconnected\r\n");

        state = EXAMP_STATE_DISCONNECTED;
    }
}

void APP_ExampleInitialize(DRV_HANDLE handle)
{
    APP_DebugPrintf("\r\n");
    APP_DebugPrintf("=========================\r\n");
    APP_DebugPrintf("WINC AP Scan Example\r\n");
    APP_DebugPrintf("=========================\r\n");

    state = EXAMP_STATE_INIT;
}

void APP_ExampleTasks(DRV_HANDLE handle)
{
    WDRV_WINC_STATUS status;

    switch (state)
    {
        case EXAMP_STATE_INIT:
        {
            /* Enable use of DHCP for network configuration, DHCP is the default
             but this also registers the callback for notifications. */

            WDRV_WINC_IPUseDHCPSet(handle, &APP_ExampleDHCPAddressEventCallback);

            /* Start a BSS find operation on all channels. */

            if (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSFindFirst(handle, WDRV_WINC_ALL_CHANNELS, true, NULL))
            {
                state = EXAMP_STATE_SCANNING;
                foundBSS = false;
            }
            break;
        }

        case EXAMP_STATE_SCANNING:
        {
            /* Wait for BSS find operation to complete, then report the number
             of results found. */

            if (false == WDRV_WINC_BSSFindInProgress(handle))
            {
                APP_DebugPrintf("Scan complete, %d AP(s) found\r\n", WDRV_WINC_BSSFindGetNumBSSResults(handle));
                state = EXAMP_STATE_SCAN_GET_RESULTS;
            }
            break;
        }

        case EXAMP_STATE_SCAN_GET_RESULTS:
        {
            WDRV_WINC_BSS_INFO BSSInfo;

            /* Request the current BSS find results. */

            if (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSFindGetInfo(handle, &BSSInfo))
            {
                APP_DebugPrintf("AP found: RSSI: %d %s\r\n", BSSInfo.rssi, BSSInfo.ssid.name);

                /* Check if this SSID matches the search target SSID. */

                if (((sizeof(WLAN_SSID)-1) == BSSInfo.ssid.length) && (0 == memcmp(BSSInfo.ssid.name, WLAN_SSID, BSSInfo.ssid.length)))
                {
                    foundBSS = true;
                }

                /* Request the next set of BSS find results. */

                status = WDRV_WINC_BSSFindNext(handle, NULL);

                if (WDRV_WINC_STATUS_BSS_FIND_END == status)
                {
                    /* If there are no more results available check if the target
                     SSID has been found. */

                    if (true == foundBSS)
                    {
                        APP_DebugPrintf("Target AP found, trying to connect\r\n");
                        state = EXAMP_STATE_SCAN_DONE;
                    }
                    else
                    {
                        APP_DebugPrintf("Target BSS not found\r\n");
                        state = EXAMP_STATE_ERROR;
                    }
                }
                else if ((WDRV_WINC_STATUS_NOT_OPEN == status) || (WDRV_WINC_STATUS_INVALID_ARG == status))
                {
                    /* An error occurred requesting results. */

                    state = EXAMP_STATE_ERROR;
                }
            }
            break;
        }

        case EXAMP_STATE_SCAN_DONE:
        {
            WDRV_WINC_AUTH_CONTEXT authCtx;
            WDRV_WINC_BSS_CONTEXT  bssCtx;

            /* Preset the error state incase any following operations fail. */

            state = EXAMP_STATE_ERROR;

            /* Initialize the BSS context to use default values. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetDefaults(&bssCtx))
            {
                break;
            }

            /* Update BSS context with target SSID for connection. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetSSID(&bssCtx, (uint8_t*)WLAN_SSID, strlen(WLAN_SSID)))
            {
                break;
            }

#if defined(WLAN_AUTH_OPEN)
            /* Initialize the authentication context for open mode. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetOpen(&authCtx))
            {
                break;
            }
#elif defined(WLAN_AUTH_WPA_PSK)
            /* Initialize the authentication context for WPA. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetWPA(&authCtx, (uint8_t*)WLAN_PSK, strlen(WLAN_PSK)))
            {
                break;
            }
#endif

            /* Connect to the target BSS with the chosen authentication. */

            if (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSConnect(handle, &bssCtx, &authCtx, &APP_ExampleConnectNotifyCallback))
            {
                state = EXAMP_STATE_CONNECTING;
            }
            break;
        }

        case EXAMP_STATE_CONNECTING:
        {
            /* Wait for the BSS connect to trigger the callback and update
             the connection state. */
            break;
        }

        case EXAMP_STATE_CONNECTED:
        {
            /* Wait for the IP link to become active. */

            if (false == WDRV_WINC_IPLinkActive(handle))
            {
                break;
            }
#ifdef ICMP_ECHO_TARGET
            /* Send an ICMP echo request to the target IP address. */

            if (WDRV_WINC_STATUS_OK == WDRV_WINC_ICMPEchoRequest(handle, inet_addr(ICMP_ECHO_TARGET), 0, &APP_ExampleICMPEchoResponseCallback))
            {
                APP_DebugPrintf("ICMP echo request sent\r\n");

                state = EXAMP_STATE_ICMP_ECHO_REQUEST_SENT;
            }
#endif
            break;
        }

        case EXAMP_STATE_ICMP_ECHO_REQUEST_SENT:
        {
#ifdef ICMP_ECHO_TARGET
            /* Wait for the callback to set the ICMP echo reply time. */

            if (0 != icmpEchoReplyTime)
            {
                /* Wait for the requested number of milliseconds before returning
                 to connected state to send the next ICMP echo request. */

                uint32_t icmpEchoIntervalTime = (ICMP_ECHO_INTERVAL * SYS_TMR_TickCounterFrequencyGet()) / 1000ul;

                if ((SYS_TMR_TickCountGet() - icmpEchoReplyTime) >= icmpEchoIntervalTime)
                {
                    state = EXAMP_STATE_CONNECTED;
                    icmpEchoReplyTime = 0;
                }
            }
#endif
            break;
        }

        case EXAMP_STATE_DISCONNECTED:
        {
            /* If we become disconnected, trigger a reconnection. */

            if (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSReconnect(handle, &APP_ExampleConnectNotifyCallback))
            {
                state = EXAMP_STATE_CONNECTING;
            }
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
