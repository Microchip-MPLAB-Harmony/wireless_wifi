/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "config/sam_d21_xpro_winc1500/driver/winc/include/drv/socket/socket.h"
#include "config/sam_d21_xpro_winc1500/system/console/sys_debug.h"
#include "config/sam_d21_xpro_winc1500/driver/winc/include/drv/common/nm_common.h"
#include "config/sam_d21_xpro_winc1500/driver/winc/include/drv/driver/m2m_wifi.h"

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- WINC1500 weather client example --"STRING_EOL	\
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/** IP address of host. */
uint32_t gu32HostIp = 0;

/** TCP client socket handlers. */
static SOCKET tcp_client_socket = -1;

/** Receive buffer definition. */
static uint8_t gau8ReceivedBuffer[MAIN_WIFI_M2M_BUFFER_SIZE] = {0};

/** Wi-Fi status variable. */
static bool gbConnectedWifi = false;

/** Get host IP status variable. */
static bool gbHostIpByName = false;

/** TCP Connection status variable. */
static bool gbTcpConnection = false;

static tstrM2MAPConfig gstrM2MAPConfig = {
	MAIN_M2M_DEVICE_NAME, 1, 0, WEP_40_KEY_STRING_SIZE, MAIN_M2M_AP_WEP_KEY, (uint8_t)MAIN_M2M_AP_SEC, MAIN_M2M_AP_SSID_MODE
};

static int8_t gacHttpProvDomainName[] = MAIN_HTTP_PROV_SERVER_DOMAIN_NAME;

static uint8_t gau8MacAddr[] = MAIN_MAC_ADDRESS;
static int8_t gacDeviceName[] = MAIN_M2M_DEVICE_NAME;


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/**
 * \brief Callback function of IP address.
 *
 * \param[in] hostName Domain name.
 * \param[in] hostIp Server IP.
 *
 * \return None.
 */
static void resolve_cb(uint8_t *hostName, uint32_t hostIp)
{
	gu32HostIp = hostIp;
	gbHostIpByName = true;
	SYS_PRINT("resolve_cb: %s IP address is %d.%d.%d.%d\r\n\r\n", hostName,
			(int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
			(int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
}

/**
 * \brief Callback function of TCP client socket.
 *
 * \param[in] sock socket handler.
 * \param[in] u8Msg Type of Socket notification
 * \param[in] pvMsg A structure contains notification informations.
 *
 * \return None.
 */
static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
	/* Check for socket event on TCP socket. */
	if (sock == tcp_client_socket) {
		switch (u8Msg) {
		case SOCKET_MSG_CONNECT:
		{
			if (gbTcpConnection) {
				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				sprintf((char *)gau8ReceivedBuffer, "%s%s%s", MAIN_PREFIX_BUFFER, (char *)MAIN_CITY_NAME, MAIN_POST_BUFFER);

				tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;
				if (pstrConnect && pstrConnect->s8Error >= SOCK_ERR_NO_ERROR) {
					send(tcp_client_socket, gau8ReceivedBuffer, strlen((char *)gau8ReceivedBuffer), 0);

					memset(gau8ReceivedBuffer, 0, MAIN_WIFI_M2M_BUFFER_SIZE);
					recv(tcp_client_socket, &gau8ReceivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
				} else {
					SYS_PRINT("\nsocket_cb: connect error!\r\n");
					gbTcpConnection = false;
                    shutdown(tcp_client_socket);
					tcp_client_socket = -1;
				}
			}
		}
		break;

		case SOCKET_MSG_RECV:
		{
			char *pcIndxPtr;
			char *pcEndPtr;

			tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
			if (pstrRecv && pstrRecv->s16BufferSize > 0) {

				/* Get city name. */
				pcIndxPtr = strstr((char *)pstrRecv->pu8Buffer, "name=");
				SYS_PRINT("City: ");
				if (NULL != pcIndxPtr) {
					pcIndxPtr = pcIndxPtr + strlen("name=") + 1;
					pcEndPtr = strstr(pcIndxPtr, "\">");
					if (NULL != pcEndPtr) {
						*pcEndPtr = 0;
					}

					SYS_PRINT("%s\r\n", pcIndxPtr);
				} else {
					SYS_PRINT("N/A\r\n");
					break;
				}

				/* Get temperature. */
				pcIndxPtr = strstr(pcEndPtr + 1, "temperature value");
				SYS_PRINT("Temperature: ");
				if (NULL != pcIndxPtr) {
					pcIndxPtr = pcIndxPtr + strlen("temperature value") + 2;
					pcEndPtr = strstr(pcIndxPtr, "\" ");
					if (NULL != pcEndPtr) {
						*pcEndPtr = 0;
					}

					SYS_PRINT("%s\r\n", pcIndxPtr);
				} else {
					SYS_PRINT("N/A\r\n");
					break;
				}

				/* Get weather condition. */
				pcIndxPtr = strstr(pcEndPtr + 1, "weather number");
				if (NULL != pcIndxPtr) {
					SYS_PRINT("Weather Condition: ");
					pcIndxPtr = pcIndxPtr + strlen("weather number") + 14;
					pcEndPtr = strstr(pcIndxPtr, "\" ");
					if (NULL != pcEndPtr) {
						*pcEndPtr = 0;
					}
					SYS_PRINT("\ns\r\n", pcIndxPtr);
					
					/* Response processed, now close connection. */
                    shutdown(tcp_client_socket);
					tcp_client_socket = -1;
					//port_pin_set_output_level(LED_0_PIN, false);
					break;
				}

				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				recv(tcp_client_socket, &gau8ReceivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
			} else {
				SYS_PRINT("\nsocket_cb: recv error!\r\n");
                shutdown(tcp_client_socket);
				tcp_client_socket = -1;
			}
		}
		break;

		default:
			break;
		}
	}
}
static void set_dev_name_to_mac(uint8_t *name, uint8_t *mac_addr)
{
	/* Name must be in the format WINC1500_00:00 */
	uint16_t len;

    len = strlen((char*)name);
	if (len >= 5) {
		name[len - 1] = MAIN_HEX2ASCII((mac_addr[5] >> 0) & 0x0f);
		name[len - 2] = MAIN_HEX2ASCII((mac_addr[5] >> 4) & 0x0f);
		name[len - 4] = MAIN_HEX2ASCII((mac_addr[4] >> 0) & 0x0f);
		name[len - 5] = MAIN_HEX2ASCII((mac_addr[4] >> 4) & 0x0f);
	}
}


/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType Type of Wi-Fi notification.
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters.
 *
 * \return None.
 */
static void wifi_cb(uint8_t u8MsgType, void const *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			SYS_PRINT("wifi_cb: M2M_WIFI_CONNECTED\r\n");
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			SYS_PRINT("wifi_cb: M2M_WIFI_DISCONNECTED\r\n");
			gbConnectedWifi = false;
		}

		break;
	}

	case M2M_WIFI_REQ_DHCP_CONF:
	{
		uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
		SYS_PRINT("wifi_cb: IP address is %u.%u.%u.%u\r\n",
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		gbConnectedWifi = true;
		/* Obtain the IP Address by network name */
        gethostbyname(MAIN_WEATHER_SERVER_NAME);
		break;
	}

	case M2M_WIFI_RESP_PROVISION_INFO:
	{
		tstrM2MProvisionInfo *pstrProvInfo = (tstrM2MProvisionInfo *)pvMsg;
		SYS_PRINT("\nwifi_cb: M2M_WIFI_RESP_PROVISION_INFO\r\n");

		if (pstrProvInfo->u8Status == M2M_SUCCESS) {
			m2m_wifi_connect((char *)pstrProvInfo->au8SSID, strlen((char *)pstrProvInfo->au8SSID), pstrProvInfo->u8SecType,
					pstrProvInfo->au8Password, M2M_WIFI_CH_ALL);
		} else {
			SYS_PRINT("\nwifi_cb: provision failed!\r\n");
		}
	}
	break;

	default:
	{
		break;
	}
	}
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    tstrWifiInitParam param;
	int8_t ret;
	uint8_t mac_addr[6];
	uint8_t u8IsMacAddrValid;
	struct sockaddr_in addr_in;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {
                /* Initialize Wi-Fi parameters structure. */
                memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));
                
                /* Initialize Wi-Fi driver with data and status callbacks. */
                param.pfAppWifiCb = wifi_cb;
                ret = m2m_wifi_init(&param);
                if (M2M_SUCCESS != ret) 
                {
                    SYS_PRINT("\nmain: m2m_wifi_init call error!(%d)\r\n", ret);
                    appData.state = APP_STATE_ERROR;
                    break;
                }
                socketInit();
                registerSocketCallback(socket_cb, resolve_cb);
                
                m2m_wifi_get_otp_mac_address(mac_addr, &u8IsMacAddrValid);
                if (!u8IsMacAddrValid) {
                    m2m_wifi_set_mac_address(gau8MacAddr);
                }
                
                	/* Retrieve MAC address of the WINC and use it for AP name. */
                m2m_wifi_get_mac_address(gau8MacAddr);
                set_dev_name_to_mac((uint8_t *)gacDeviceName, gau8MacAddr);
                set_dev_name_to_mac((uint8_t *)gstrM2MAPConfig.au8SSID, gau8MacAddr);
                m2m_wifi_set_device_name((uint8_t *)gacDeviceName, (uint8_t)strlen((char *)gacDeviceName));
                gstrM2MAPConfig.au8DHCPServerIP[0] = 0xC0; /* 192 */
                gstrM2MAPConfig.au8DHCPServerIP[1] = 0xA8; /* 168 */
                gstrM2MAPConfig.au8DHCPServerIP[2] = 0x01; /* 1 */
                gstrM2MAPConfig.au8DHCPServerIP[3] = 0x01; /* 1 */

                /* Start web provisioning mode. */
                m2m_wifi_start_provision_mode((tstrM2MAPConfig *)&gstrM2MAPConfig, (char *)gacHttpProvDomainName, 1);
                SYS_PRINT("\r\nProvision Mode started.\r\nConnect to [%s] via AP[%s] and fill up the page.\r\n\r\n",MAIN_HTTP_PROV_SERVER_DOMAIN_NAME, gstrM2MAPConfig.au8SSID);
                
                appData.state = APP_STATE_SERVICE_TASKS;
                SYS_PRINT("\nAPP_STATE_INIT\n");
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            m2m_wifi_handle_events();

            if (gbConnectedWifi && !gbTcpConnection) 
            {
                if (gbHostIpByName) 
                {
				/* Open TCP client socket. */
                    if (tcp_client_socket < 0) 
                    {
                        if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                        {
                            SYS_PRINT("main: failed to create TCP client socket error!\r\n");
                            //continue; saga
                            break;
                        }
                    }

                    /* Connect TCP client socket. */
                    addr_in.sin_family = AF_INET;
                    addr_in.sin_port = _htons(MAIN_SERVER_PORT);
                    addr_in.sin_addr.s_addr = gu32HostIp;
                    if (connect(tcp_client_socket, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) 
                    {   
                        SYS_PRINT("main: failed to connect socket error!\r\n");
                        break;
                    }
                    gbTcpConnection = true;
                }
            }
            //SYS_PRINT("\nAPP_STATE_SERVICE_TASKS\n");
            break;
        }

        /* TODO: implement your application state machine.*/
        case APP_STATE_ERROR:
        {
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
