/*******************************************************************************
  WINC Driver Common Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_common.h

  Summary:
    WINC wireless driver common header file.

  Description:
    This file provides common elements of the WINC driver API.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
// DOM-IGNORE-END

#ifndef WDRV_WINC_COMMON_H
#define WDRV_WINC_COMMON_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "configuration.h"
#include "definitions.h"
#include "driver/driver.h"
#include "osal/osal.h"
#include "wdrv_winc_debug.h"
#include "wdrv_winc_utils.h"
#include "winc_dev.h"
#include "winc_cmd_req.h"
#include "winc_socket.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Common Data Types
// *****************************************************************************
// *****************************************************************************

/* Maximum length of an SSID. */
#define WDRV_WINC_MAX_SSID_LEN              32U

/* Address of a MAC address. */
#define WDRV_WINC_MAC_ADDR_LEN              6

/* Length of 40 bit WEP key. */
#define WDRV_WINC_WEP_40_KEY_STRING_SIZE    10

/* Length of 104 bit WEP key. */
#define WDRV_WINC_WEP_104_KEY_STRING_SIZE   26

/* Length of PSK (ASCII encoded binary). */
#define WDRV_WINC_PSK_LEN                   64

/* Maximum length of a WPA Personal Password. */
#define WDRV_WINC_MAX_PSK_PASSWORD_LEN      63

/* Minimum length of a WPA Personal Password. */
#define WDRV_WINC_MIN_PSK_PASSWORD_LEN      8

#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
/* The maximum length (in ASCII characters) of domain name + username (including '@' or '\')
   for authentication with Enterprise methods.
*/
#define WDRV_WINC_ENT_AUTH_IDENTITY_LEN_MAX         255
/* The maximum length (in ASCII characters) of server domain name for server certificates validation
   during enterprise connection.
*/
#define WDRV_WINC_ENT_AUTH_SERVER_DOMAIN_LEN_MAX    255
/* The maximum length (in ASCII characters) of username for enterprise authentication.
*/
#define WDRV_WINC_ENT_AUTH_USERNAME_LEN_MAX         255
/* The maximum length (in ASCII characters) of password for enterprise authentication.
*/
#define WDRV_WINC_ENT_AUTH_PASSWORD_LEN_MAX         255
#endif

// *****************************************************************************
/*  WiFi Channels

  Summary:
    A list of supported WiFi channels.

  Description:
    A list of supported WiFi channels.

  Remarks:
    None.

*/

typedef enum
{
    /* Any valid channel. */
    WDRV_WINC_CID_ANY,

    /* 2.4 GHz channel 1 - 2412 MHz. */
    WDRV_WINC_CID_2_4G_CH1,

    /* 2.4 GHz channel 2 - 2417 MHz. */
    WDRV_WINC_CID_2_4G_CH2,

    /* 2.4 GHz channel 3 - 2422 MHz. */
    WDRV_WINC_CID_2_4G_CH3,

    /* 2.4 GHz channel 4 - 2427 MHz. */
    WDRV_WINC_CID_2_4G_CH4,

    /* 2.4 GHz channel 5 - 2432 MHz. */
    WDRV_WINC_CID_2_4G_CH5,

    /* 2.4 GHz channel 6 - 2437 MHz. */
    WDRV_WINC_CID_2_4G_CH6,

    /* 2.4 GHz channel 7 - 2442 MHz. */
    WDRV_WINC_CID_2_4G_CH7,

    /* 2.4 GHz channel 8 - 2447 MHz. */
    WDRV_WINC_CID_2_4G_CH8,

    /* 2.4 GHz channel 9 - 2452 MHz. */
    WDRV_WINC_CID_2_4G_CH9,

    /* 2.4 GHz channel 10 - 2457 MHz. */
    WDRV_WINC_CID_2_4G_CH10,

    /* 2.4 GHz channel 11 - 2462 MHz. */
    WDRV_WINC_CID_2_4G_CH11,

    /* 2.4 GHz channel 12 - 2467 MHz. */
    WDRV_WINC_CID_2_4G_CH12,

    /* 2.4 GHz channel 13 - 2472 MHz. */
    WDRV_WINC_CID_2_4G_CH13
} WDRV_WINC_CHANNEL_ID;

// *****************************************************************************
/*  Common API Return Status Code

  Summary:
    API return status codes.

  Description:
    All API functions which return a status code will use one of these to be
      consistent.

  Remarks:
    None.

*/

typedef enum
{
    /* Operation was successful. */
    WDRV_WINC_STATUS_OK = 0,

    /* Driver instance has not been opened. */
    WDRV_WINC_STATUS_NOT_OPEN,

    /* The arguments supplied are not valid. */
    WDRV_WINC_STATUS_INVALID_ARG,

    /* A scan operation is currently in progress. */
    WDRV_WINC_STATUS_SCAN_IN_PROGRESS,

    /* No BSS information is available. */
    WDRV_WINC_STATUS_NO_BSS_INFO,

    /* No more BSS scan results are available. */
    WDRV_WINC_STATUS_BSS_FIND_END,

    /* The connection attempt has failed. */
    WDRV_WINC_STATUS_CONNECT_FAIL,

    /* The disconnection attempt has failed. */
    WDRV_WINC_STATUS_DISCONNECT_FAIL,

    /* The requested operation could not be completed. */
    WDRV_WINC_STATUS_REQUEST_ERROR,

    /* The context being referenced is invalid. */
    WDRV_WINC_STATUS_INVALID_CONTEXT,

    /* Request could not complete, but may if tried again. */
    WDRV_WINC_STATUS_RETRY_REQUEST,

    /* Out of space in resource. */
    WDRV_WINC_STATUS_NO_SPACE,

    /* No Ethernet buffer was available. */
    WDRV_WINC_STATUS_NO_ETH_BUFFER,

    /* Not currently connected. */
    WDRV_WINC_STATUS_NOT_CONNECTED,

    /* The requested operation is not supported. */
    WDRV_WINC_STATUS_OPERATION_NOT_SUPPORTED,

    /* The subject of the status is busy. */
    WDRV_WINC_STATUS_BUSY
} WDRV_WINC_STATUS;

// *****************************************************************************
/*  Extended system status

  Summary:
    Defines extended system status.

  Description:
    An extended status

  Remarks:
    None.

*/

typedef enum
{
    WDRV_WINC_SYS_STATUS_ERROR_DEVICE_NOT_FOUND = (SYS_STATUS_ERROR_EXTENDED-1),

    WDRV_WINC_SYS_STATUS_ERROR_DEVICE_FAILURE   = (SYS_STATUS_ERROR_EXTENDED-2)
} WDRV_WINC_SYS_STATUS;

// *****************************************************************************
/*  Connection State

  Summary:
    Defines possible connection states.

  Description:
    A connection can currently either be connected or disconnect.

  Remarks:
    None.

*/

typedef enum
{
    /* Association state is disconnected. */
    WDRV_WINC_CONN_STATE_DISCONNECTED,

    /* Association state is connecting. */
    WDRV_WINC_CONN_STATE_CONNECTING,

    /* Association state is connected. */
    WDRV_WINC_CONN_STATE_CONNECTED,

    /* Association state is connection failed. */
    WDRV_WINC_CONN_STATE_FAILED,

    /* Association state is roamed. */
    WDRV_WINC_CONN_STATE_ROAMED,
} WDRV_WINC_CONN_STATE;

// *****************************************************************************
/*  Debug UART

  Summary:
    Defines values for debug UARTs.

  Description:
    Values to indicate debug UART selection.

  Remarks:
    None.

*/

typedef enum
{
    /* Off/invalid UART selection. */
    WDRV_WINC_DEBUG_UART_OFF = 0,

    /* UART1. */
    WDRV_WINC_DEBUG_UART_1 = 1,

    /* UART2. */
    WDRV_WINC_DEBUG_UART_2 = 2
} WDRV_WINC_DEBUG_UART;

// *****************************************************************************
/*  SSID

  Summary:
    Structure to hold an SSID.

  Description:
    The SSID consist of a buffer and a length field.

  Remarks:
    None.

*/

typedef struct
{
    /* SSID name, up to WDRV_WINC_MAX_SSID_LEN characters long. */
    uint8_t name[WDRV_WINC_MAX_SSID_LEN];

    /* Length of SSID name. */
    uint8_t length;
} WDRV_WINC_SSID;

// *****************************************************************************
/*  SSID Linked List

  Summary:
    Structure to hold an SSID linked list element.

  Description:
    An element structure which can form part of an SSID linked list.

  Remarks:
    None.

*/

typedef struct WDRV_WINC_STR_SSID_LIST
{
    /* Pointer to next SSID element in list. */
    struct WDRV_WINC_STR_SSID_LIST *pNext;

    /* SSID structure. */
    WDRV_WINC_SSID ssid;
} WDRV_WINC_SSID_LIST;

// *****************************************************************************
/*  MAC Address

  Summary:
    Structure to hold a MAC address.

  Description:
    The MAC address consist of a buffer and a valid flag.

  Remarks:
    None.

*/

typedef struct
{
    /* MAC address, must be WDRV_WINC_MAC_ADDR_LEN characters long. */
    uint8_t addr[WDRV_WINC_MAC_ADDR_LEN];

    /* Is the address valid? */
    bool valid;
} WDRV_WINC_MAC_ADDR;

// *****************************************************************************
/*  Association Handle

  Summary:
    A handle representing an association instance.

  Description:
    An association handle references a single association instance between AP and STA.

  Remarks:
    None.

*/

typedef uintptr_t WDRV_WINC_ASSOC_HANDLE;

// *****************************************************************************
/* Invalid Association Handle

 Summary:
    Invalid association handle.

 Description:
    Defines a value for an association handle which isn't yet valid.

 Remarks:
    None.
*/

#define WDRV_WINC_ASSOC_HANDLE_INVALID  (((WDRV_WINC_ASSOC_HANDLE) -1))

// *****************************************************************************
/* All Association Handles

 Summary:
    All association handles.

 Description:
    Defines a value which refers to all associations.

 Remarks:
    None.
*/

#define WDRV_WINC_ASSOC_HANDLE_ALL  (((WDRV_WINC_ASSOC_HANDLE) -2))

// *****************************************************************************
/*  Connection Notify Callback

  Summary:
    Callback to notify the user of a change in connection state.

  Description:
    When the connection state changes this callback enable the driver to signal
      the user about that event and reason.

  Parameters:
    handle          - Client handle obtained by a call to WDRV_WINC_Open.
    assocHandle     - Association handle.
    currentState    - Current connection state.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_BSSCON_NOTIFY_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_ASSOC_HANDLE assocHandle,
    WDRV_WINC_CONN_STATE currentState
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_COMMON_H */
