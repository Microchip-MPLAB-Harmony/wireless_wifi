/*******************************************************************************
  WINC Driver DHCP Server Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_dhcps.h

  Summary:
    WINC wireless driver DHCP server header file.

  Description:
    This interface provides functionality required for DHCP server operations.
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

#ifndef WDRV_WINC_DHCPS_H
#define WDRV_WINC_DHCPS_H

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
// *****************************************************************************
// Section: WINC Driver DHCP Server Routines
// *****************************************************************************
// *****************************************************************************

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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

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

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_DHCPSProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        WINC_DEV_EVENT_RSP_ELEMS *pElems
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

void WDRV_WINC_DHCPSProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_DEV_EVENT_RSP_ELEMS *pElems
);

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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    poolIdx        - Pool index to set state on.
    enabled        - Flag indicating if the pool should be enabled or disabled.
    pfDHCPSEventCB - Pointer to callback to receive event notifications.

  Returns:
    WDRV_WINC_STATUS_OK             - A scan was initiated.
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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    poolIdx    - Pool index to set state on.
    pStartAddr - Pointer to starting IP address.

  Returns:
    WDRV_WINC_STATUS_OK             - A scan was initiated.
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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    poolIdx - Pool index to set state on.
    pGWAddr - Pointer to gateway IP address.

  Returns:
    WDRV_WINC_STATUS_OK             - A scan was initiated.
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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    poolIdx - Pool index to set state on.
    ifIdx   - Network interface index.

  Returns:
    WDRV_WINC_STATUS_OK             - A scan was initiated.
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

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_DHCPS_H */
