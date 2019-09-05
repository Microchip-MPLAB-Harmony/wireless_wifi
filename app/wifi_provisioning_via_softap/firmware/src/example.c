/*******************************************************************************
  WINC Example Application

  File Name:
    tcp_server_ap.c

  Summary:
    Wi-Fi Provisioning via soft AP example.

  Description:
    This example demonstrates the use of the WINC1500 with the SAMD21 Xplained Pro
    board to start Wi-Fi provisioning mode via soft AP

    The configuration defines for this demo are:
        WLAN_SSID           -- Soft AP SSID to create
        WLAN_CHANNEL        -- Channel on which to beacon
        WLAN_AUTH           -- Security for the BSS
        WLAN_WEB_KEY        -- WEP key
        WLAN_WEB_KEY_INDEX  -- WEP key index
        WLAN_DHCP_SRV_ADDR  -- IP address of DHCP server to create
        TCP_BUFFER_SIZE     -- Size of the socket buffer holding the receive data
*******************************************************************************/

// DOM-IGNORE-BEGIN
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

/** \mainpage
 * \section intro Introduction
 * This example demonstrates the use of the WINC1500 with the SAMD21 Xplained Pro board
 * to connect to AP via softap mode.<br>
 * It uses the following hardware:
 * - the SAMD21 Xplained Pro.
 * - the WINC1500 on EXT1. / WINC3400 on EXT1
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
 * -# 1. Power on the board, the board enter softAP mode
 *  # 2. On the mobile phone, go to the WiFi setting page to to connect to the AP "WINC1500_PROVISION_AP"
 * -# 3. Launch the mobile APP, click "Connect to WINC1500" button, click "SSID/ Security/ PW" button,  enter SSID and password on the mobile APP
 *  # 4. The board will change to STA mode and connect to the target AP
 * -# Configure below code in the example_conf.h for WPS push button feature.

 * \code
 * ===========================================
 * WINC WiFi Provisioning Soft AP Example
 * ===========================================
 *
 * AP started, you can connect to WINC1500_PROVISION_AP
 * On the android device, connect to WINC1500_PROVISION_AP then run setting app
 * Bind on socket 0 successful, server_socket = 0
 * Listen on socket 0 successful
 * AP Mode: Station connected
 * AP Mode: Station IP address is 192.168.1.100
 * Connection from  192.168.1.100:47600
 * Disable to AP.
 * AP Mode: Station disconnected
 * STA mode: Station connected
 * STA Mode: Station IP address is 192.168.0.103
 *
 * \endcode
 *
 */

#include "app.h"
#include "wdrv_winc_client_api.h"
#include "example_conf.h"

typedef enum
{
    /* Example's state machine's initial state. */
    EXAMP_STATE_INIT=0,
    EXAMP_STATE_AP_STARTED,
    EXAMP_STATE_AP_STOPPED,
    EXAMP_STATE_STA_STARTED,
    EXAMP_STATE_CLI_STOPPED,
    EXAMP_STATE_SOCKET_LISTENING,
    EXAMP_STATE_CONNECTING,
    EXAMP_STATE_CONNECTED,
    EXAMP_STATE_DISCONNECTED,
    EXAMP_STATE_IP_GET,
    EXAMP_STATE_ERROR,
} EXAMP_STATES;

static EXAMP_STATES state;
static SOCKET serverSocket = -1;
static SOCKET tcp_client_socket = -1;
static uint8_t recvBuffer[TCP_BUFFER_SIZE];
static WDRV_WINC_BSS_CONTEXT  bssCtx;
static WDRV_WINC_AUTH_CONTEXT authCtx;

static void APP_ExampleSocketEventCallback(SOCKET socket, uint8_t messageType, void *pMessage)
{
    switch(messageType)
    {
        case SOCKET_MSG_BIND:
        {
            tstrSocketBindMsg *pBindMessage = (tstrSocketBindMsg*)pMessage;

            if ((NULL != pBindMessage) && (0 == pBindMessage->status))
            {
                APP_DebugPrintf("Bind on socket %d successful, server_socket = %d\r\n", socket, serverSocket);
                listen(serverSocket, 0);
            }
            else
            {
                APP_DebugPrintf("Bind on socket %d failed\r\n", socket);

                shutdown(serverSocket);
                serverSocket =  -1;
                state = EXAMP_STATE_ERROR;
            }
            break;
        }

        case SOCKET_MSG_LISTEN:
        {
            tstrSocketListenMsg *pListenMessage = (tstrSocketListenMsg*)pMessage;

            if ((NULL != pListenMessage) && (0 == pListenMessage->status))
            {
                APP_DebugPrintf("Listen on socket %d successful\r\n", socket);
                accept(serverSocket, NULL, NULL);
            }
            else
            {
                APP_DebugPrintf("Listen on socket %d failed\r\n", socket);

                shutdown(serverSocket);
                serverSocket =  -1;
                state = EXAMP_STATE_ERROR;
            }
            break;
        }

        case SOCKET_MSG_ACCEPT:
        {
            tstrSocketAcceptMsg *pAcceptMessage = (tstrSocketAcceptMsg*)pMessage;

            if (NULL != pAcceptMessage)
            {
                char s[20];

                accept(serverSocket, NULL, 0);
                tcp_client_socket = pAcceptMessage->sock;

                APP_DebugPrintf("Connection from  %s:%d\r\n", inet_ntop(AF_INET, &pAcceptMessage->strAddr.sin_addr.s_addr, s, sizeof(s)), _ntohs(pAcceptMessage->strAddr.sin_port));

                recv(tcp_client_socket, recvBuffer, TCP_BUFFER_SIZE, 0);
                
            }
            break;
        }

        case SOCKET_MSG_RECV:
        {
            tstrSocketRecvMsg *pRecvMessage = (tstrSocketRecvMsg*)pMessage;
            tstrM2mWifiWepParams wep_parameters;
            
            if ((NULL != pRecvMessage) && (pRecvMessage->s16BufferSize > 0))
            {
                char *p;

                p = strtok((char *)pRecvMessage->pu8Buffer, ",");
                if (p != NULL && !strncmp(p, "apply", 5)) {
                    char str_ssid[M2M_MAX_SSID_LEN], str_pw[M2M_MAX_PSK_LEN];
                    unsigned char sec_type = 0;

                    p = strtok(NULL, ",");
                    if (p) {
                        strcpy(str_ssid, p);
                    }

                    p = strtok(NULL, ",");
                    if (p) {
                        sec_type = atoi(p);
                    }

                    p = strtok(NULL, ",");
                    if (p) {
                        strcpy(str_pw, p);
                    }
                    if(sec_type == M2M_WIFI_SEC_WEP){
                        wep_parameters.u8KeyIndx=1;
                        wep_parameters.u8KeySz = strlen(str_pw)+1;
                        memcpy(wep_parameters.au8WepKey,str_pw,wep_parameters.u8KeySz);
                    }
                    
                    state = EXAMP_STATE_ERROR;
                    
                    
                    APP_DebugPrintf("Disable to AP.\r\n");
                    shutdown(serverSocket);
                    
                    state = EXAMP_STATE_AP_STOPPED;
                  
                    /* Initialize the BSS context to use default values. */

                    if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetDefaults(&bssCtx))
                    {
                        break;
                    }
                    
                    /* Update BSS context with target SSID for connection. */

                    if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetSSID(&bssCtx, (uint8_t*)str_ssid, strlen((char *)str_ssid)))
                    {
                        APP_DebugPrintf("Fail to set SSID....\r\n");
                        break;
                    }
                    
                    switch (sec_type)
                    {
                        case WDRV_WINC_AUTH_TYPE_OPEN:
                        {        
                                if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetOpen(&authCtx))
                                {
                                    APP_DebugPrintf("Fail to set Auth Open....\r\n");
                                    break;
                                }
                                
                                break;
                        }
                        
                        case WDRV_WINC_AUTH_TYPE_WPA_PSK:
                        {        
                                if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetWPA(&authCtx, (uint8_t*)str_pw, strlen(str_pw)))
                                {
                                    APP_DebugPrintf("Fail to set Auth WPAPSK....\r\n");
                                    break;
                                }
                                
                                break;
                        }
                        
                        case WDRV_WINC_AUTH_TYPE_WEP:
                        {        
                            
                                if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetWEP(&authCtx, wep_parameters.u8KeyIndx, wep_parameters.au8WepKey, wep_parameters.u8KeySz))
                                {
                                    APP_DebugPrintf("Fail to set Auth WEP....\r\n");
                                    break;
                                }

                                break;
                        }
                    }

                    break;
                }
            }
            else
            {
                APP_DebugPrintf("Receive on socket %d failed\r\n", socket);
                shutdown(socket);
            }
            
                memset(recvBuffer, 0, TCP_BUFFER_SIZE);
                recv(tcp_client_socket, recvBuffer, TCP_BUFFER_SIZE, 0);
            break;
        }

        case SOCKET_MSG_SEND:
        {
            APP_DebugPrintf("Socket %d send completed\r\n", socket);
            break;
        }

        default:
        {
            break;
        }
    }
}

static void APP_ExampleAPConnectNotifyCallback(DRV_HANDLE handle, WDRV_WINC_CONN_STATE currentState, WDRV_WINC_CONN_ERROR errorCode)
{
    if (WDRV_WINC_CONN_STATE_CONNECTED == currentState)
    {
        APP_DebugPrintf("AP Mode: Station connected\r\n");
        
        if (-1 == serverSocket)
        {
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);

            if (serverSocket >= 0)
            {
                struct sockaddr_in addr;

                /* Listen on the socket. */

                addr.sin_family = AF_INET;
                addr.sin_port = _htons(TCP_LISTEN_PORT);
                addr.sin_addr.s_addr = 0;

                bind(serverSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));

                state = EXAMP_STATE_SOCKET_LISTENING;
            }
        }
        
    }
    else if (WDRV_WINC_CONN_STATE_DISCONNECTED == currentState)
    {
        APP_DebugPrintf("AP Mode: Station disconnected\r\n");

        if (-1 != serverSocket)
        {
            shutdown(serverSocket);
            serverSocket = -1;
        }

    }
}

static void APP_ExampleSTAConnectNotifyCallback(DRV_HANDLE handle, WDRV_WINC_CONN_STATE currentState, WDRV_WINC_CONN_ERROR errorCode)
{
    if (WDRV_WINC_CONN_STATE_CONNECTED == currentState)
    {
        APP_DebugPrintf("STA mode: Station connected\r\n");
        state = EXAMP_STATE_CONNECTED;
        
      
        
        
    }
    else if (WDRV_WINC_CONN_STATE_DISCONNECTED == currentState)
    {
        APP_DebugPrintf("STA mode: Station disconnected\r\n");


        state = EXAMP_STATE_STA_STARTED;
    }
}

#if defined(WLAN_DHCP_SRV_ADDR) && defined(WLAN_DHCP_SRV_NETMASK)
static void APP_ExampleDHCPAddressEventCallback(DRV_HANDLE handle, uint32_t ipAddress)
{
    char s[20];

    APP_DebugPrintf("AP Mode: Station IP address is %s\r\n", inet_ntop(AF_INET, &ipAddress, s, sizeof(s)));
}
#endif

#if 1
static void APP_ExampleSTAModeDHCPAddressEventCallback(DRV_HANDLE handle, uint32_t ipAddress)
{
    char s[20];

    APP_DebugPrintf("STA Mode: Station IP address is %s\r\n", inet_ntop(AF_INET, &ipAddress, s, sizeof(s)));
}
#endif
void APP_ExampleInitialize(DRV_HANDLE handle)
{
    APP_DebugPrintf("\r\n");
    APP_DebugPrintf("===========================================\r\n");
    APP_DebugPrintf("WINC WiFi Provisioning Soft AP Example\r\n");
    APP_DebugPrintf("===========================================\r\n");
    APP_DebugPrintf("\r\n");
    
    state = EXAMP_STATE_INIT;

    serverSocket = -1;
}

void APP_ExampleTasks(DRV_HANDLE handle)
{
    switch (state)
    {
        case EXAMP_STATE_INIT:
        {

            /* Preset the error state incase any following operations fail. */

            state = EXAMP_STATE_ERROR;

            /* Create the BSS context using default values and then set SSID
             and channel. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetDefaults(&bssCtx))
            {
                break;
            }

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetSSID(&bssCtx, (uint8_t*)WLAN_SSID, strlen(WLAN_SSID)))
            {
                break;
            }

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_BSSCtxSetChannel(&bssCtx, WLAN_CHANNEL))
            {
                break;
            }

#if defined(WLAN_AUTH_OPEN)
            /* Create authentication context for WPA. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetOpen(&authCtx))
            {
                break;
            }
#elif defined(WLAN_AUTH_WEP)
            /* Create authentication context for WEP. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_AuthCtxSetWEP(&authCtx, WLAN_WEB_KEY_INDEX, (uint8_t*)WLAN_WEB_KEY, strlen(WLAN_WEB_KEY)))
            {
                break;
            }
#endif

#if defined(WLAN_DHCP_SRV_ADDR) && defined(WLAN_DHCP_SRV_NETMASK)
            /* Enable use of DHCP for network configuration, DHCP is the default
             but this also registers the callback for notifications. */

            if (WDRV_WINC_STATUS_OK != WDRV_WINC_IPDHCPServerConfigure(handle, inet_addr(WLAN_DHCP_SRV_ADDR), inet_addr(WLAN_DHCP_SRV_NETMASK), &APP_ExampleDHCPAddressEventCallback))
            {
                break;
            }
#endif
            /* Register callback for socket events. */

            WDRV_WINC_SocketRegisterEventCallback(handle, &APP_ExampleSocketEventCallback);

            /* Create the AP using the BSS and authentication context. */

            if (WDRV_WINC_STATUS_OK == WDRV_WINC_APStart(handle, &bssCtx, &authCtx, NULL, &APP_ExampleAPConnectNotifyCallback))
            {
                APP_DebugPrintf("AP started, you can connect to %s\r\n", WLAN_SSID);
                APP_DebugPrintf("On the android device, connect to %s then run setting app\r\n", WLAN_SSID);

                state = EXAMP_STATE_AP_STARTED;
            }
            break;
        }

        case EXAMP_STATE_AP_STARTED:
        {
            /* Create the server socket. */

            serverSocket = socket(AF_INET, SOCK_STREAM, 0);

            if (serverSocket >= 0)
            {
                struct sockaddr_in addr;

                /* Listen on the socket. */

                addr.sin_family = AF_INET;
                addr.sin_port = _htons(TCP_LISTEN_PORT);
                addr.sin_addr.s_addr = 0;

                bind(serverSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));

                state = EXAMP_STATE_SOCKET_LISTENING;
            }
            break;
        }

        case EXAMP_STATE_SOCKET_LISTENING:
        {
            break;
        }
        
        case EXAMP_STATE_AP_STOPPED:
        {
            if (WDRV_WINC_STATUS_OK == WDRV_WINC_APStop(handle))
            {
                state = EXAMP_STATE_STA_STARTED;
            
                if (-1 != serverSocket)
                {
                    shutdown(serverSocket);
                    serverSocket = -1;
                }
            }
             
            WDRV_MSDelay(500);
            break;
        }

        case EXAMP_STATE_STA_STARTED:
        {
            
              if (WDRV_WINC_STATUS_OK != WDRV_WINC_IPUseDHCPSet(handle, &APP_ExampleSTAModeDHCPAddressEventCallback))
            {
                APP_DebugPrintf("WDRV_WINC_IPUseDHCPSet() fail ...\r\n");
                break;
            }
            
            WDRV_WINC_BSSConnect(handle, &bssCtx, &authCtx, &APP_ExampleSTAConnectNotifyCallback);
            state = EXAMP_STATE_CONNECTING;
            break;
        }
        
        case EXAMP_STATE_CONNECTED:
        {
          
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


