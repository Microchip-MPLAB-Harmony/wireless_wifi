/*******************************************************************************
  WINC Driver Network Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_netif.h

  Summary:
    WINC wireless driver network interface header file.

  Description:
    This interface provides functionality required for network interface operations.
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

#ifndef WDRV_WINC_NETIF_H
#define WDRV_WINC_NETIF_H

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

#include "wdrv_winc.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

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
    WDRV_WINC_NETIF_IDX_INVALID = -1,
    WDRV_WINC_NETIF_IDX_0 = 0,
    WDRV_WINC_NETIF_IDX_1 = 1
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

    /* Network interface IP address update/change. */
    WDRV_WINC_NETIF_EVENT_ADDR_UPDATE,
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
    /* No valid type. */
    WDRV_WINC_NETIF_INFO_INVALID,

    /* MAC address type. */
    WDRV_WINC_NETIF_INFO_MAC_ADDR
} WDRV_WINC_NETIF_INFO_TYPE;

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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

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
/*
  Function:
    void (*WDRV_WINC_DHCP_ADDRESS_EVENT_HANDLER)
    (
        DRV_HANDLE handle,
        uint32_t ipAddress
    )

  Summary:
    Pointer to a DHCP address event handler function.

  Description:
    This data type defines a function event handler to receive notification
    of allocation of IP address via DHCP.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    ipAddress   - IPv4 address.

  Returns:
    None.

  Remarks:
    Only supported with the socket mode WINC driver.

    See WDRV_WINC_IPSetUseDHCP and WDRV_WINC_IPDHCPServerConfigure.

*/

typedef void (*WDRV_WINC_DHCP_ADDRESS_EVENT_HANDLER)
(
    DRV_HANDLE handle,
    uint32_t ipAddress
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Network Interface Routines
// *****************************************************************************
// *****************************************************************************

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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx   - Interface index.
    aacMode - Auto-assignment configuration mode.

  Returns:
    WDRV_WINC_STATUS_OK            - Callback registered.
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
        WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
        int scope
    )

  Summary:
    Set the IP address of a network interface.

  Description:
    Defines the network IP address and scope of an interface.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx  - Interface index.
    type   - IP address type, either IPv4 or IPv6.
    pAddr  - Pointer to IP address to set.
    scope  - Scope of network for IP address.

  Returns:
    WDRV_WINC_STATUS_OK            - Callback registered.
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
    WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
    int scope
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPRouteSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_IP_ADDRESS_TYPE type,
        WDRV_WINC_IP_MULTI_ADDRESS *pNetwork,
        int scope,
        WDRV_WINC_IP_MULTI_ADDRESS *pDest
    )

  Summary:
    Set a route on a network interface.

  Description:
    Defines a route to a network interface.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle   - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx    - Interface index.
    type     - IP address type, either IPv4 or IPv6.
    pNetwork - Pointer to network IP address.
    scope    - Scope of network for network IP address.
    pDest    - Pointer to IP address of destination.

  Returns:
    WDRV_WINC_STATUS_OK            - Callback registered.
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
    WDRV_WINC_IP_MULTI_ADDRESS *pNetwork,
    int scope,
    WDRV_WINC_IP_MULTI_ADDRESS *pDest
);

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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle        - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx         - Interface index.
    pfNetIfInfoCB - Pointer to callback function to receive MAC address.

  Returns:
    WDRV_WINC_STATUS_OK            - Callback registered.
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

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_NETIF_H */
