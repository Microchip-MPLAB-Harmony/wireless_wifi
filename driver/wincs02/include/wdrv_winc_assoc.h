/*******************************************************************************
  WINC Driver Association Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_assoc.h

  Summary:
    WINC wireless driver association header file.

  Description:
    This interface provides information about the current association with a
    peer device.
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

#ifndef WDRV_WINC_ASSOC_H
#define WDRV_WINC_ASSOC_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"
#include "wdrv_winc_authctx.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Association Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  BSS Information

  Summary:
    Structure containing information about an association.

  Description:
    This structure contains the association information for a connection between
      an AP and STA.

  Remarks:
    None.
*/

typedef struct
{
    /* Primary driver handle. */
    DRV_HANDLE handle;

    /* MAC address of peer device. */
    WDRV_WINC_MAC_ADDR peerAddress;

    /* Authentication type used. */
    WDRV_WINC_AUTH_TYPE authType;

    /* Last RSSI value read. */
    int8_t rssi;

    /* Association ID. */
    uint16_t assocID;
} WDRV_WINC_ASSOC_INFO;

// *****************************************************************************
/*  Association RSSI Callback.

  Summary:
    A callback to provide the current RSSI of the current association.

  Description:
    This callback provides details of the signal strength (RSSI) of the current
      association.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    assocHandle - Association handle.
    rssi        - The current RSSI of the association.

  Returns:
    None.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_ASSOC_RSSI_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_ASSOC_HANDLE assocHandle,
    int8_t rssi
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Association Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AssocPeerAddressGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETWORK_ADDRESS *const pPeerAddress
    )

  Summary:
    Retrieve the current association peer device network address.

  Description:
    Attempts to retrieve the network address of the peer device in the
      current association.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    A peer device needs to be connected and associated.

  Parameters:
    assocHandle         - Association handle.
    pPeerAddress        - Pointer to structure to receive the network address.

  Returns:
    WDRV_WINC_STATUS_OK             - pPeerAddress will contain the network address.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected
                                            or there is no current association.
    WDRV_WINC_STATUS_RETRY_REQUEST  - The network address is not available
                                            but it will be requested from the WINC.
    WDRV_WINC_STATUS_NOT_CONNECTED  - Not currently connected.

  Remarks:
    If the network address is not currently known to the driver (stored within the
      WINC) a request will be sent to the WINC and the return status
      will be WDRV_WINC_STATUS_RETRY_REQUEST.

*/

WDRV_WINC_STATUS WDRV_WINC_AssocPeerAddressGet
(
    WDRV_WINC_ASSOC_HANDLE assocHandle,
    WDRV_WINC_MAC_ADDR *const pPeerAddress
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AssocRSSIGet
    (
        WDRV_WINC_ASSOC_HANDLE assocHandle,
        int8_t *const pRSSI,
        WDRV_WINC_ASSOC_RSSI_CALLBACK const pfAssociationRSSICB
    )

  Summary:
    Retrieve the current association RSSI.

  Description:
    Attempts to retrieve the RSSI of the current association.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    A peer device needs to be connected and associated.

  Parameters:
    assocHandle         - Association handle.
    pRSSI               - Pointer to variable to receive RSSI if available.
    pfAssociationRSSICB - Pointer to callback function to be used when
                            RSSI value is available.

  Returns:
    WDRV_WINC_STATUS_OK             - pRSSI will contain the RSSI.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected
                                            or there is no current association.
    WDRV_WINC_STATUS_RETRY_REQUEST  - The RSSI is not available but it will
                                            be requested from the WINC.
    WDRV_WINC_STATUS_NOT_CONNECTED  - Not currently connected.

  Remarks:
    If the RSSI is not currently known to the driver (stored within the
      WINC) a request will be sent to the WINC and the return status
      will be WDRV_WINC_STATUS_RETRY_REQUEST. The callback function
      pfAssociationRSSICB can be provided which will be called when the WINC
      provides the RSSI information to the driver. Alternatively the caller
      may poll this function until the return status is WDRV_WINC_STATUS_OK
      to obtain the RSSI in pRSSI.

*/

WDRV_WINC_STATUS WDRV_WINC_AssocRSSIGet
(
    WDRV_WINC_ASSOC_HANDLE assocHandle,
    int8_t *const pRSSI,
    WDRV_WINC_ASSOC_RSSI_CALLBACK const pfAssociationRSSICB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AssocDisconnect(WDRV_WINC_ASSOC_HANDLE assocHandle)

  Summary:
    Disconnects an association.

  Description:
    This API disconnects the STA associated with softAP based on the input association handle.
    Association handle is updated to the application whenever a new STA is connected to the softAP
    This API can also be used in STA mode to disconnect the STA from an AP.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_APStart should have been called to start softAP in softAP mode.
    WDRV_WINC_BSSConnect should have been called to connect to an AP in STA mode.

  Parameters:
    assocHandle - WDRV_WINC_ASSOC_HANDLE obtained when STA joins an AP or an association is done.

  Returns:
    WDRV_WINC_STATUS_OK                         - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN                   - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG                - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR              - The request to the WINC was rejected.
    WDRV_WINC_STATUS_NOT_CONNECTED              - The association isn't valid.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_AssocDisconnect(WDRV_WINC_ASSOC_HANDLE assocHandle);

#endif /* WDRV_WINC_ASSOC_H */
