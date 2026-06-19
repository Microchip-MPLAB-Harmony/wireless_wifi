/*******************************************************************************
  WINC Wireless Driver Network Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_netif.h

  Summary:
    WINC wireless driver network interface interface.

  Description:
    This interface provides functionality required for network interface
    operations.
 *******************************************************************************/

/*
Copyright (C) 2024-26 Microchip Technology Inc. and its subsidiaries. All rights reserved.

Subject to your compliance with these terms, you may use this Microchip software and any derivatives
exclusively with Microchip products. You are responsible for complying with third party license terms
applicable to your use of third party software (including open source software) that may accompany this
Microchip software. SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR
STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-
INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL
MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS,
DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER
CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY
TO MICROCHIP FOR THIS SOFTWARE.
*/

#ifndef WDRV_WINC_NETIF_H
#define WDRV_WINC_NETIF_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Network Interface Defines
// *****************************************************************************
// *****************************************************************************

/* The number of global IPv6 addresses supported. */
#define WDRV_WINC_NETIF_INFO_NUM_GLO_IPV6           WINC_CFG_PARAM_NUM_NETIF_IPV6_GLO_ADDR

/* Request bitmask value for network information - MAC address. */
#define WDRV_WINC_NETIF_REQ_INFO_MAC_ADDR           (1 << WDRV_WINC_NETIF_INFO_MAC_ADDR)

/* Request bitmask value for network information - Interface. */
#define WDRV_WINC_NETIF_REQ_INFO_INTF               (1 << WDRV_WINC_NETIF_INFO_INTF)
/* Request bitmask value for network information - DHCP client. */
#define WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE         (1 << WDRV_WINC_NETIF_INFO_DHCP_LEASE)
/* Request bitmask value for network information - IPv4. */
#define WDRV_WINC_NETIF_REQ_INFO_IPV4               (1 << WDRV_WINC_NETIF_INFO_IPV4)
/* Request bitmask value for network information - IPv6. */
#define WDRV_WINC_NETIF_REQ_INFO_IPV6               (1 << WDRV_WINC_NETIF_INFO_IPV6)

/* Request bitmask value for all groups of network information. */
#define WDRV_WINC_NETIF_REQ_INFO_ALL                (((1<<WDRV_WINC_NUM_NETIF_INFO_TYPE)-1)^WDRV_WINC_NETIF_REQ_INFO_MAC_ADDR)

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Network Interface Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Network Interface Index.

  Summary:
    Network interface index values.

  Description:
    Possible values for the network interface index.

  Remarks:
    None.
*/

typedef enum
{
    /* Invalid network index. */
    WDRV_WINC_NETIF_IDX_INVALID = -1,

    /* Network interface index 0. */
    WDRV_WINC_NETIF_IDX_0 = 0,

    /* Default network interface index. */
    WDRV_WINC_NETIF_IDX_DEFAULT = WDRV_WINC_NETIF_IDX_0,

    /* Network interface index 1. */
    WDRV_WINC_NETIF_IDX_1 = 1,

    /* Number of supported network interfaces. */
    WDRV_WINC_NUM_NETIFS
} WDRV_WINC_NETIF_IDX;

// *****************************************************************************
/* Network Interface Events.

  Summary:
    Network interface event values.

  Description:
    Possible values for network interface events.

  Remarks:
    None.
*/

typedef enum
{
    /* Network interface up. */
    WDRV_WINC_NETIF_EVENT_INTF_UP,

    /* Network interface down. */
    WDRV_WINC_NETIF_EVENT_INTF_DOWN,

#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
    /* Network interface IP address update/change. */
    WDRV_WINC_NETIF_EVENT_ADDR_UPDATE,
#endif
} WDRV_WINC_NETIF_EVENT_TYPE;

// *****************************************************************************
/* Network Interface Information Type.

  Summary:
    Network interface information type values.

  Description:
    Possible values for network interface information.

  Remarks:
    None.
*/

typedef enum
{
    /* Not valid information type. */
    WDRV_WINC_NETIF_INFO_INVALID,

    /* MAC address information type. */
    WDRV_WINC_NETIF_INFO_MAC_ADDR,

    /* Interface information type. */
    WDRV_WINC_NETIF_INFO_INTF,

    /* DHCP information type. */
    WDRV_WINC_NETIF_INFO_DHCP_LEASE,

    /* IPv4 information type. */
    WDRV_WINC_NETIF_INFO_IPV4,

    /* IPv6 information type. */
    WDRV_WINC_NETIF_INFO_IPV6,

    /* Number of information types. */
    WDRV_WINC_NUM_NETIF_INFO_TYPE,
} WDRV_WINC_NETIF_INFO_TYPE;

// *****************************************************************************
/* Network Information - Interface

  Summary:
    Type to describe network interface information.

  Description:
    Structure to contain network interface information.

  Remarks:
    None.
*/

typedef struct
{
    /* Bitmask indicating which fields are valid. */
    union
    {
        struct
        {
            uint8_t name : 1;
            uint8_t macAddr : 1;
            uint8_t hostname : 1;
        };
        uint8_t all;
    } valid;

    /* Interface name. */
    char name[WINC_CFG_PARAM_SZ_NETIF_NAME+1];

    /* MAC address. */
    uint8_t macAddr[WDRV_WINC_MAC_ADDR_LEN];

    /* Hostname. */
    char hostname[WINC_CFG_PARAM_SZ_NETIF_HOSTNAME+1];
} WDRV_WINC_NETIF_INFO_STORE_INTF_TYPE;

#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
// *****************************************************************************
/* Address Auto-assignment Configuration.

  Summary:
    Address auto-assignment configuration values.

  Description:
    Possible values for address auto-assignment configuration.

  Remarks:
    None.
*/

typedef enum
{
    /* No auto-assignment. */
    WDRV_WINC_NETIF_AAC_NONE        = 0,

    /* Support for IPv4 DHCP assignment. */
    WDRV_WINC_NETIF_AAC_V4_DHCP     = 1,

    /* Support for IPv4 autoconf assignment. */
    WDRV_WINC_NETIF_AAC_V4_AUTOCONF = 2,

    /* Support for IPv6 DHCP6 assignment. */
    WDRV_WINC_NETIF_AAC_V6_DHCP     = 4,

    /* Support for IPv6 SLAAC assignment. */
    WDRV_WINC_NETIF_AAC_V6_SLAAC    = 8
} WDRV_WINC_NETIF_AAC_TYPE;

// *****************************************************************************
/* Network Interface Address Update Type.

  Summary:
    Type to describe IP address updates.

  Description:
    Structure to contain an IP address update.

  Remarks:
    None.
*/

typedef struct
{
    /* Type of address being updated. */
    WDRV_WINC_IP_ADDRESS_TYPE type;

    /* IP address update. */
    WDRV_WINC_IP_MULTI_ADDRESS addr;
} WDRV_WINC_NETIF_ADDR_UPDATE_TYPE;

// *****************************************************************************
/* Network Information - DHCP

  Summary:
    Type to describe DHCP network information.

  Description:
    Structure to contain DHCP network information.

  Remarks:
    None.
*/

typedef struct
{
    /* Bitmask indicating which fields are valid. */
    union
    {
        struct
        {
            uint8_t enabled : 1;
            uint8_t leaseObtainedUTC : 1;
            uint8_t leaseExpiresUTC : 1;
            uint8_t serverAddr : 1;
        };
        uint8_t all;
    } valid;

    /* Flag indicating if DHCP client is enabled. */
    bool enabled;

    /* UTC time DHCP lease was obtained. */
    uint32_t leaseObtainedUTC;

    /* UTC time DHCP lease expires. */
    uint32_t leaseExpiresUTC;

    /* IPv4 address of DHCP server issuing lease. */
    WDRV_WINC_IPV4_ADDR serverAddr;
} WDRV_WINC_NETIF_INFO_STORE_DHCP_TYPE;

// *****************************************************************************
/* Network Information - IPv4

  Summary:
    Type to describe IPv4 network information.

  Description:
    Structure to contain IPv4 network information.

  Remarks:
    None.
*/

typedef struct
{
    /* Bitmask indicating which fields are valid. */
    union
    {
        struct
        {
            uint8_t addr : 1;
            uint8_t mask : 1;
            uint8_t gateway : 1;
        };
        uint8_t all;
    } valid;

    /* Address. */
    WDRV_WINC_IPV4_ADDR addr;

    /* Netmask. */
    WDRV_WINC_IPV4_ADDR mask;

    /* Default gateway. */
    WDRV_WINC_IPV4_ADDR gateway;
} WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE;

// *****************************************************************************
/* Network Information - IPv6

  Summary:
    Type to describe IPv6 network information.

  Description:
    Structure to contain IPv6 network information.

  Remarks:
    None.
*/

typedef struct
{
    /* Bitmask indicating which fields are valid. */
    union
    {
        struct
        {
            uint8_t linkLocal : 1;
            uint8_t global : 1;
            uint8_t gateway : 1;
        };
        uint8_t all;
    } valid;

    /* Link local address. */
    WDRV_WINC_IPV6_PREFIX_ADDR linkLocal;

    /* Global addresses. */
    WDRV_WINC_IPV6_PREFIX_ADDR global[WDRV_WINC_NETIF_INFO_NUM_GLO_IPV6];

    /* Primary gateway. */
    WDRV_WINC_IPV6_ADDR gateway;
} WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE;

#endif /* WDRV_WINC_DISABLE_L3_SUPPORT */
// *****************************************************************************
/* Network Information

  Summary:
    Type to describe network information.

  Description:
    Structure to contain all network information.

  Remarks:
    None.
*/

typedef struct
{
    /* Interface information. */
    WDRV_WINC_NETIF_INFO_STORE_INTF_TYPE intf;

#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
    /* DHCP client information. */
    WDRV_WINC_NETIF_INFO_STORE_DHCP_TYPE dhcp;

    /* IPv4 address information. */
    WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE ipv4;

    /* IPv6 address information. */
    WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE ipv6;
#endif
} WDRV_WINC_NETIF_INFO_STORE_TYPE;

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_NETIF_EVENT_HANDLER)
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_NETIF_EVENT_TYPE eventType,
        void *pEventInfo
    )

  Summary:
    Network interface event callback.

  Description:
    Callback to be used to convey network interface events.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx      - Network interface index.
    eventType  - Event type.
    pEventInfo - Pointer to event specific context information.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_NETIF_EVENT_HANDLER)
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_NETIF_EVENT_TYPE eventType,
    void *pEventInfo
);

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_NETIF_INFO_HANDLER)
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_NETIF_EVENT_TYPE eventType,
        void *pEventInfo
    )

  Summary:
    Network interface information callback.

  Description:
    Callback to be used to convey network interface information.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle   - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx    - Network interface index.
    infoType - Information type.
    pInfo    - Pointer to information requested.
    status   - Status of request operation.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_NETIF_INFO_HANDLER)
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_NETIF_INFO_TYPE infoType,
    void *pInfo,
    WDRV_WINC_STATUS status
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Network Interface Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_NETIFProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        const WINC_DEV_EVENT_RSP_ELEMS *const pElems
    )

  Summary:
    AEC process callback.

  Description:
    Callback will be called to process any AEC messages received.

  Precondition:
    WINC_DevAECCallbackRegister must be called to register the callback.

  Parameters:
    context   - Pointer to user context supplied when callback was registered.
    devHandle - WINC device handle.
    pElems    - Pointer to element structure.

  Returns:
    None.

  Remarks:
    Callback should call WINC_CmdReadParamElem to extract elements.

*/

void WDRV_WINC_NETIFProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfRegisterEventCallback
    (
        DRV_HANDLE handle,
        const WDRV_WINC_NETIF_EVENT_HANDLER pfNetIfEventCallback
    )

  Summary:
    Register event callback.

  Description:
    Function to register an event callback function.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle               - Client handle obtained by a call to WDRV_WINC_Open.
    pfNetIfEventCallback - Pointer to callback function.

  Returns:
    WDRV_WINC_STATUS_OK          - Callback registered.
    WDRV_WINC_STATUS_NOT_OPEN    - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfRegisterEventCallback
(
    DRV_HANDLE handle,
    const WDRV_WINC_NETIF_EVENT_HANDLER pfNetIfEventCallback
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfInfoSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_NETIF_INFO_TYPE infoType,
        void *pInfo
    )

  Summary:
    Set the information about a network interface.

  Description:
    Configures the information about a network interface.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle   - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx    - Interface index.
    infoType - Information type.
    pInfo    - Pointer to information structure, see WDRV_WINC_NETIF_INFO_STORE_*_TYPE.

  Returns:
    WDRV_WINC_STATUS_OK            - MAC address request accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.
    WDRV_WINC_STATUS_RETRY_REQUEST - Another request is pending, retry.

  Remarks:
   | infoType                           | pInfo                                |
   -----------------------------------------------------------------------------
    WDRV_WINC_NETIF_INFO_INTF:          | WDRV_WINC_NETIF_INFO_STORE_INTF_TYPE
        - hostname

    WDRV_WINC_NETIF_INFO_DHCP_LEASE:    | WDRV_WINC_NETIF_INFO_STORE_DHCP_TYPE
    WDRV_WINC_NETIF_INFO_IPV4:          | WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE
    WDRV_WINC_NETIF_INFO_IPV6:          | WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfInfoSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_NETIF_INFO_TYPE infoType,
    void *pInfo
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfInfoGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        uint32_t reqInfoMask,
        WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB
    )

  Summary:
    Get information about a network interface.

  Description:
    Requests the information of a network interface, to be return via a callback.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle        - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx         - Interface index.
    reqInfoMask   - Request bit mask, see WDRV_WINC_NETIF_REQ_INFO_*.
    pfNetIfInfoCB - Pointer to callback function to receive the information.

  Returns:
    WDRV_WINC_STATUS_OK            - MAC address request accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.
    WDRV_WINC_STATUS_RETRY_REQUEST - Another request is pending, retry.

  Remarks:
   | reqInfoMask                         | Callback pInfo                      |
   -----------------------------------------------------------------------------
    WDRV_WINC_NETIF_REQ_INFO_MAC_ADDR:   | char*
    WDRV_WINC_NETIF_REQ_INFO_INTF:       | WDRV_WINC_NETIF_INFO_STORE_INTF_TYPE
    WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE: | WDRV_WINC_NETIF_INFO_STORE_DHCP_TYPE
    WDRV_WINC_NETIF_REQ_INFO_IPV4:       | WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE
    WDRV_WINC_NETIF_REQ_INFO_IPV6:       | WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE

    WDRV_WINC_NETIF_REQ_INFO_MAC_ADDR can not be used with any other requests.

    Multiple request can be made, this will result in one callback per type.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfInfoGet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    uint32_t reqInfoMask,
    WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB
);

#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPAutoConfModeSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_NETIF_AAC_TYPE aacMode
    )

  Summary:
    Set IP address auto-configuration mode.

  Description:
    Sets how the IP address of the network interface can be assigned.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx   - Interface index.
    aacMode - Auto-assignment configuration mode.

  Returns:
    WDRV_WINC_STATUS_OK            - Mode set.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfIPAutoConfModeSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_NETIF_AAC_TYPE aacMode
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPAddrSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_IP_ADDRESS_TYPE type,
        const WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
        unsigned int scope
    )

  Summary:
    Set the IP address of a network interface.

  Description:
    Defines the network IP address and scope of an interface.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx  - Interface index.
    type   - IP address type, either IPv4 or IPv6.
    pAddr  - Pointer to IP address to set.
    scope  - Scope of network for IP address.

  Returns:
    WDRV_WINC_STATUS_OK            - IP address set.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfIPAddrSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_IP_ADDRESS_TYPE type,
    const WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
    unsigned int scope
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPRouteSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_IP_ADDRESS_TYPE type,
        const WDRV_WINC_IP_MULTI_ADDRESS *pNetwork,
        int scope,
        const WDRV_WINC_IP_MULTI_ADDRESS *pDest
    )

  Summary:
    Set a route on a network interface.

  Description:
    Defines a route to a network interface.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle   - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx    - Interface index.
    type     - IP address type, either IPv4 or IPv6.
    pNetwork - Pointer to network IP address.
    scope    - Scope of network for network IP address.
    pDest    - Pointer to IP address of destination.

  Returns:
    WDRV_WINC_STATUS_OK            - Route set.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    Currently this only supports setting the default gateway for the interface.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfIPRouteSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_IP_ADDRESS_TYPE type,
    const WDRV_WINC_IP_MULTI_ADDRESS *pNetwork,
    int scope,
    const WDRV_WINC_IP_MULTI_ADDRESS *pDest
);
#endif /* WDRV_WINC_DISABLE_L3_SUPPORT */

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfMACAddrGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB
    )

  Summary:
    Get the MAC address of a network interface.

  Description:
    Requests the MAC address of a network interface, to be return via a callback.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle        - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx         - Interface index.
    pfNetIfInfoCB - Pointer to callback function to receive MAC address.

  Returns:
    WDRV_WINC_STATUS_OK            - MAC address request accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.
    WDRV_WINC_STATUS_RETRY_REQUEST - Another request is pending, retry.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfMACAddrGet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfHostnameSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        const char* pHostname
    )

  Summary:
    Set the hostname of a network interface.

  Description:
    Configures the hostname of a network interface.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx     - Interface index.
    pHostname - Pointer to hostname.

  Returns:
    WDRV_WINC_STATUS_OK            - MAC address request accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.
    WDRV_WINC_STATUS_RETRY_REQUEST - Another request is pending, retry.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfHostnameSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    const char* pHostname
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_NETIF_H */
