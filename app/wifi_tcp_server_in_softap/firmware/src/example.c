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
    Wi-Fi TCP Server with soft AP example.

  Description:
    This example demonstrates the use of the WINC1500 with the SAMD21 Xplained Pro
    board to start TCP server on WINC1500 running as soft AP

    The configuration defines for this demo are:
        WLAN_SSID           -- Soft AP SSID to create
        WLAN_CHANNEL        -- Channel on which to beacon
        WLAN_AUTH           -- Security for the BSS
        WLAN_WEB_KEY        -- WEP key
        WLAN_WEB_KEY_INDEX  -- WEP key index
        WLAN_DHCP_SRV_ADDR  -- IP address of DHCP server to create
        TCP_BUFFER_SIZE     -- Size of the socket buffer holding the receive data
*******************************************************************************/

/** \mainpage
 * \section intro Introduction
 * This example demonstrates the use of the WINC1500 with the SAMD21 Xplained Pro board
 * to setup a TCP Server in softap mode.<br>
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
 * -# 1. Power on the board, the board will enter softAP mode
 *  # 2. Connect your personal computer to the network named defined by macro "WLAN_SSID" in example_conf.h file
 * -# 3. For creating a TCP connection, you can use any open source programs (e.g. packet sender or Tera Term)

 * \code
 * ===========================================
 * WINC WiFi TCP Server Soft AP Example
 * ===========================================
 * 
 * AP started, you can connect to WINC1500_SOFT_AP
 * On the connected device, start a TCP client connection to 192.168.1.1 on port 6666
 * 
 * AP Mode: Station connected
 * AP Mode: Station IP address is 192.168.1.100
 * Bind on socket 0 successful, server_socket = 0
 * Listen on socket 0 successful
 * Connection from  192.168.1.100:55686
 * Receive on socket 1 successful
 * Client sent 13 bytes
 * Client sent Hello Server!
 * Sending a test message to client
 * Socket 1 send completed
 * TCP Server Test Complete!
 * Closing sockets
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
    EXAMP_STATE_WAIT_FOR_STATION,
    EXAMP_STATE_START_TCP_SERVER,
    EXAMP_STATE_SOCKET_LISTENING,
    EXAMP_STATE_DONE,
    EXAMP_STATE_ERROR,
} EXAMP_STATES;

/** Message format definitions. */
typedef struct s_msg_wifi_product {
	uint8_t name[9];
} t_msg_wifi_product;

/** Message format declarations. */
static t_msg_wifi_product msg_wifi_product = {
	.name = WLAN_PRODUCT_NAME,
};

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
                
                if (tcp_client_socket > 0) // close any open client (only one client supported at one time)
                {
                    shutdown(tcp_client_socket);
                }
                
                tcp_client_socket = pAcceptMessage->sock;

                APP_DebugPrintf("Connection from  %s:%d\r\n", inet_ntop(AF_INET, &pAcceptMessage->strAddr.sin_addr.s_addr, s, sizeof(s)), _ntohs(pAcceptMessage->strAddr.sin_port));

                memset(recvBuffer, 0, TCP_BUFFER_SIZE);
                recv(tcp_client_socket, recvBuffer, TCP_BUFFER_SIZE, 0);
                
            }
            else
            {
                APP_DebugPrintf("Accept on socket %d failed\r\n", socket);

                shutdown(serverSocket);
                serverSocket =  -1;
                state = EXAMP_STATE_ERROR;
            }
            break;
        }

        case SOCKET_MSG_RECV:
        {
            tstrSocketRecvMsg *pRecvMessage = (tstrSocketRecvMsg*)pMessage;
            
            if ((NULL != pRecvMessage) && (pRecvMessage->s16BufferSize > 0))
            {
                APP_DebugPrintf("Receive on socket %d successful\r\n", socket);
                APP_DebugPrintf("Client sent %d bytes\r\n", pRecvMessage->s16BufferSize);
                APP_DebugPrintf("Client sent %s\r\n", pRecvMessage->pu8Buffer);
                APP_DebugPrintf("Sending a test message to client\r\n");
                
                send(tcp_client_socket, &msg_wifi_product, sizeof(t_msg_wifi_product), 0);
            }
            else
            {
                APP_DebugPrintf("Receive on socket %d failed\r\n", socket);

                shutdown(serverSocket);
                serverSocket = -1;
                state = EXAMP_STATE_ERROR;
            }
            break;
        }

        case SOCKET_MSG_SEND:
        {
            APP_DebugPrintf("Socket %d send completed\r\n", socket);
            APP_DebugPrintf("TCP Server Test Complete!\r\n", socket);
            APP_DebugPrintf("Closing sockets\r\n", socket);
            shutdown(tcp_client_socket);
            shutdown(serverSocket);
            state = EXAMP_STATE_DONE;
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


#if defined(WLAN_DHCP_SRV_ADDR) && defined(WLAN_DHCP_SRV_NETMASK)
static void APP_ExampleDHCPAddressEventCallback(DRV_HANDLE handle, uint32_t ipAddress)
{
    char s[20];

    APP_DebugPrintf("AP Mode: Station IP address is %s\r\n", inet_ntop(AF_INET, &ipAddress, s, sizeof(s)));
    state = EXAMP_STATE_START_TCP_SERVER;
}
#endif


void APP_ExampleInitialize(DRV_HANDLE handle)
{
    APP_DebugPrintf("\r\n");
    APP_DebugPrintf("===========================================\r\n");
    APP_DebugPrintf("WINC WiFi TCP Server Soft AP Example\r\n");
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
            /* Create authentication context for open authentication. */

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
                APP_DebugPrintf("On the connected device, start a TCP client connection to %s on port %d\r\n\r\n", WLAN_DHCP_SRV_ADDR, TCP_LISTEN_PORT);

                state = EXAMP_STATE_WAIT_FOR_STATION;
            }
            break;
        }

        case EXAMP_STATE_WAIT_FOR_STATION:
        {
            break;
        }

        case EXAMP_STATE_START_TCP_SERVER:
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

                if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
                {
                    APP_DebugPrintf("Socket bind error\r\n");
                    state = EXAMP_STATE_ERROR;
                    break;
                }
                
                state = EXAMP_STATE_SOCKET_LISTENING;
            }
            else
            {
                APP_DebugPrintf("Socket creation error\r\n");
                state = EXAMP_STATE_ERROR;
                break;
            }
            break;
        }

        case EXAMP_STATE_SOCKET_LISTENING:
        {
            break;
        }

        case EXAMP_STATE_DONE:
        {
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


