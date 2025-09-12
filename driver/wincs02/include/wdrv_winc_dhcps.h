/*******************************************************************************
  WINC Wireless Driver DHCP Server Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_dhcps.h

  Summary:
    WINC wireless driver DHCP server interface.

  Description:
    This interface provides functionality required for DHCP server operations.
 *******************************************************************************/

/*
Copyright (C) 2024-25 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#ifndef WDRV_WINC_DHCPS_H
#define WDRV_WINC_DHCPS_H

#ifndef WDRV_WINC_MOD_DISABLE_DHCPS

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

#include "wdrv_winc.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver DHCP Server Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* DHCP Server Index

  Summary:
    DHCP server index type.

  Description:
    Type describing possible DHCP server pool indexes.

  Remarks:
    None.
*/

typedef enum
{
    /* Invalid DHCP server index. */
    WDRV_WINC_DHCPS_IDX_INVALID = -1,

    /* Index of DHCP server configuration 0. */
    WDRV_WINC_DHCPS_IDX_0 = 0,
} WDRV_WINC_DHCPS_IDX;

// *****************************************************************************
/* DHCP Server Events.

  Summary:
    DHCP server event values.

  Description:
    Possible values for the DHCP server events.

  Remarks:
    None.
*/

typedef enum
{
    /* DHCP lease assigned to client. */
    WDRV_WINC_DHCPS_EVENT_LEASE_ASSIGNED,
} WDRV_WINC_DHCPS_EVENT_TYPE;

// *****************************************************************************
/* DHCP Server Event Information.

  Summary:
    DHCP server event information.

  Description:
    Structure containing DHCP server event information.

  Remarks:
    None.
*/

typedef union
{
    /* Lease assignment event information. */
    struct
    {
        /* IPv4 address assigned to client. */
        WDRV_WINC_IPV4_ADDR ipAddr;

        /* MAC address of client. */
        WDRV_WINC_MAC_ADDR  macAddr;
    } leaseAssignment;
} WDRV_WINC_DHCPS_EVENT_INFO;

// *****************************************************************************
/*
  Function:
    typedef void (*WDRV_WINC_DHCPS_EVENT_HANDLER)
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_EVENT_TYPE eventType,
        void *pEventInfo
    )

  Summary:
    DHCP server event callback.

  Description:
    Callback to be used to convey DHCP server events.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    eventType  - Event type.
    pEventInfo - Pointer to event specific context information.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_DHCPS_EVENT_HANDLER)
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_EVENT_TYPE eventType,
    void *pEventInfo
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver DHCP Server Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DHCPSEnableSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_IDX poolIdx,
        bool enabled,
        WDRV_WINC_DHCPS_EVENT_HANDLER pfDHCPSEventCB
    )

  Summary:
    Set the enable state of the DHCP server.

  Description:
    Enables or disables the DHCP server.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    poolIdx        - Pool index to set state on.
    enabled        - Flag indicating if the pool should be enabled or disabled.
    pfDHCPSEventCB - Pointer to callback to receive event notifications.

  Returns:
    WDRV_WINC_STATUS_OK             - DHCP server state was set.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DHCPSEnableSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_IDX poolIdx,
    bool enabled,
    WDRV_WINC_DHCPS_EVENT_HANDLER pfDHCPSEventCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DHCPSPoolAddressSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_IDX poolIdx,
        WDRV_WINC_IPV4_ADDR *pStartAddr
    )

  Summary:
    Sets the DHCP server pool start address.

  Description:
    Configures the starting IP address of the selected DHCP server pool.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    poolIdx    - Pool index to set state on.
    pStartAddr - Pointer to starting IP address.

  Returns:
    WDRV_WINC_STATUS_OK             - DHCP server pool was set.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DHCPSPoolAddressSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_IDX poolIdx,
    WDRV_WINC_IPV4_ADDR *pStartAddr
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DHCPSGatewaySet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_IDX poolIdx,
        WDRV_WINC_IPV4_ADDR *pGWAddr
    )

  Summary:
    Sets the DHCP server pool default gateway address.

  Description:
    Configures the default gateway IP address of a DHCP server pool.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    poolIdx - Pool index to set state on.
    pGWAddr - Pointer to gateway IP address.

  Returns:
    WDRV_WINC_STATUS_OK             - The DHCP server gateways was set.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DHCPSGatewaySet
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_IDX poolIdx,
    WDRV_WINC_IPV4_ADDR *pGWAddr
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DHCPSNetIfBind
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_IDX poolIdx,
        WDRV_WINC_NETIF_IDX ifIdx
    )

  Summary:
    Sets the network interface index of the DHCP server pool.

  Description:
    Assigns a DHCP server pool to the specified network interface.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    poolIdx - Pool index to set state on.
    ifIdx   - Network interface index.

  Returns:
    WDRV_WINC_STATUS_OK             - The network interface was updated.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DHCPSNetIfBind
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_IDX poolIdx,
    WDRV_WINC_NETIF_IDX ifIdx
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_MOD_DISABLE_DHCPS */
#endif /* WDRV_WINC_DHCPS_H */
