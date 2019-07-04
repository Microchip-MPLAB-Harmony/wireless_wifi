/*******************************************************************************
  WINC Example Application

  File Name:
    tcp_client.c

  Summary:
    WINC TCP client example.

  Description:
    This example demonstrates the use of TCP client socket with the WINC.

    The configuration defines for this demo are:
        WLAN_SSID           -- BSS to search for
        WLAN_AUTH           -- Security for the BSS
        WLAN_PSK            -- Passphrase for WPA security
        TCP_SERVER_IP_ADDR  -- IP address for the TCP server
        TCP_SERVER_PORT_NUM -- Port number for the TCP server
        TCP_BUFFER_SIZE     -- Size of the socket buffer holding the receive data
        TCP_SEND_MESSAGE    -- Customizable TCP packet content
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

#include "app.h"
#include "wdrv_winc_client_api.h"
#include "example_conf.h"

typedef enum
{
    /* Example's state machine's initial state. */
    EXAMP_STATE_INIT=0,
    EXAMP_STATE_CONNECTING,
    EXAMP_STATE_CONNECTED,
    EXAMP_STATE_SOCKET_OPEN,
    EXAMP_STATE_DISCONNECTED,
    EXAMP_STATE_ERROR,
} EXAMP_STATES;

static EXAMP_STATES state;
static SOCKET clientSocket;
static uint8_t recvBuffer[TCP_BUFFER_SIZE];

static void APP_ExampleSocketEventCallback(SOCKET socket, uint8_t messageType, void *pMessage)
{
    if (socket != clientSocket)
    {
        return;
    }

    switch(messageType)
    {
        case SOCKET_MSG_CONNECT:
        {
            tstrSocketConnectMsg *pConnectMessage = (tstrSocketConnectMsg*)pMessage;

            if ((NULL != pConnectMessage) && (pConnectMessage->s8Error >= 0))
            {
                APP_DebugPrintf("Socket %d connect success\r\n", socket);
                recv(clientSocket, recvBuffer, TCP_BUFFER_SIZE, 0);
                send(clientSocket, TCP_SEND_MESSAGE, sizeof(TCP_SEND_MESSAGE), 0);
            }
            else
            {
                APP_DebugPrintf("Socket %d connect error %d\r\n", socket, pConnectMessage->s8Error);

                if (-1 != clientSocket)
                {
                    shutdown(clientSocket);
                    clientSocket = -1;
                }

                state = EXAMP_STATE_ERROR;
            }
            break;
        }

        case SOCKET_MSG_RECV:
        {
            tstrSocketRecvMsg *pRecvMessage = (tstrSocketRecvMsg*)pMessage;

            if ((NULL != pRecvMessage) && (pRecvMessage->s16BufferSize > 0))
            {
                APP_DebugPrintf("Socket receive, size = %d bytes [", pRecvMessage->s16BufferSize);

                if (pRecvMessage->s16BufferSize > 20)
                {
                    pRecvMessage->pu8Buffer[20] = '\0';
                }
                else
                {
                    pRecvMessage->pu8Buffer[pRecvMessage->s16BufferSize] = '\0';
                }

                APP_DebugPrintf((char*)pRecvMessage->pu8Buffer);

                if (pRecvMessage->s16BufferSize > 20)
                {
                    APP_DebugPrintf("...");
                }

                APP_DebugPrintf("]\r\n");

                recv(socket, recvBuffer, TCP_BUFFER_SIZE, 0);
            }
            else
            {
                APP_DebugPrintf("Receive on socket %d failed\r\n", socket);

                if (-1 != clientSocket)
                {
                    shutdown(clientSocket);
                    clientSocket = -1;
                }
            }
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

static void APP_ExampleDHCPAddressEventCallback(DRV_HANDLE handle, uint32_t ipAddress)
{
    char s[20];

    APP_DebugPrintf("IP address is %s\r\n", inet_ntop(AF_INET, &ipAddress, s, sizeof(s)));
}

static void APP_ExampleConnectNotifyCallback(DRV_HANDLE handle, WDRV_WINC_CONN_STATE currentState, WDRV_WINC_CONN_ERROR errorCode)
{
    if (WDRV_WINC_CONN_STATE_CONNECTED == currentState)
    {
        APP_DebugPrintf("Connected\r\n");

        state = EXAMP_STATE_CONNECTED;
    }
    else if (WDRV_WINC_CONN_STATE_DISCONNECTED == currentState)
    {
        if (EXAMP_STATE_CONNECTING == state)
        {
            APP_DebugPrintf("Failed to connect\r\n");

            state = EXAMP_STATE_INIT;
        }
        else
        {
            APP_DebugPrintf("Disconnected\r\n");

            if (-1 != clientSocket)
            {
                shutdown(clientSocket);
                clientSocket = -1;
            }

            state = EXAMP_STATE_DISCONNECTED;
        }
    }
}

void APP_ExampleInitialize(DRV_HANDLE handle)
{
    APP_DebugPrintf("\r\n");
    APP_DebugPrintf("===========================\r\n");
    APP_DebugPrintf("WINC TCP Client Example\r\n");
    APP_DebugPrintf("===========================\r\n");

    state = EXAMP_STATE_INIT;

    clientSocket = -1;
}

void APP_ExampleTasks(DRV_HANDLE handle)
{
    switch (state)
    {
        case EXAMP_STATE_INIT:
        {
            WDRV_WINC_AUTH_CONTEXT authCtx;
            WDRV_WINC_BSS_CONTEXT  bssCtx;

            /* Enable use of DHCP for network configuration, DHCP is the default
             but this also registers the callback for notifications. */

            WDRV_WINC_IPUseDHCPSet(handle, &APP_ExampleDHCPAddressEventCallback);

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

            /* Register callback for socket events. */

            WDRV_WINC_SocketRegisterEventCallback(handle, &APP_ExampleSocketEventCallback);
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

            state = EXAMP_STATE_ERROR;

            /* Create the client socket. */

            clientSocket = socket(AF_INET, SOCK_STREAM, 0);

            if (clientSocket >= 0)
            {
                struct sockaddr_in addr;

                /* Connect the socket to the server. */

                addr.sin_family = AF_INET;
                addr.sin_port = _htons(TCP_SERVER_PORT_NUM);
                addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IP_ADDR);

                if (connect(clientSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) >= 0)
                {
                    state = EXAMP_STATE_SOCKET_OPEN;
                }
                else if (-1 != clientSocket)
                {
                    shutdown(clientSocket);
                    clientSocket = -1;
                }
            }

            break;
        }

        case EXAMP_STATE_SOCKET_OPEN:
        {
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
