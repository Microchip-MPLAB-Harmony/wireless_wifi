/*******************************************************************************
  WINC Wireless Driver Provisioning Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_prov.h

  Summary:
    WINC wireless driver provisioning header file.

  Description:
    This interface provides functionality required for the provisioning service,
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

#ifndef WDRV_WINC_PROV_H
#define WDRV_WINC_PROV_H

#ifndef WDRV_WINC_MOD_DISABLE_PROV

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Provisioning Defines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Provisioning Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  Provisioning Service Client ID

  Summary:
    A handle representing a client ID.

  Description:
    A client ID associated with clients to the provisioning service.

  Remarks:
    None.

*/

typedef int WDRV_WINC_PROV_CLIENT_ID;

// *****************************************************************************
/* Provisioning Protocol

  Summary:
    Provisioning protocol type.

  Description:
    Provisioning protocol type.

  Remarks:
    None.
*/

typedef enum
{
    /* Binary command request/responses and AECs. */
    WDRV_WINC_PROV_PROTOCOL_BINARY = 0,

    /* ASCII AT style interface for commands/responses and AECs. */
    WDRV_WINC_PROV_PROTOCOL_ASCII = 1,
} WDRV_WINC_PROV_PROTOCOL_TYPE;

// *****************************************************************************
/* Provisioning Attach Event Callback

  Function:
    void (*WDRV_WINC_PROV_ATTACH_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_PROV_CLIENT_ID clientId,
        WDRV_WINC_IP_MULTI_TYPE_ADDRESS *pClientAddr
    )

  Summary:
    Pointer to a callback function for provisioning events.

  Description:
    This defines a function pointer for a callback to receive provisioning events.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    clientId    - Client ID.
    pClientAddr - Pointer to client IP address.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_PROV_ATTACH_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_PROV_CLIENT_ID clientId,
    WDRV_WINC_IP_MULTI_TYPE_ADDRESS *pClientAddr
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Provisioning Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_ProvProcessAEC
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

void WDRV_WINC_ProvProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_ProvServiceStart
    (
        DRV_HANDLE handle,
        WDRV_WINC_PROV_PROTOCOL_TYPE type,
        uint16_t port,
        WDRV_WINC_IP_ADDRESS_TYPE ipType,
        WDRV_WINC_PROV_ATTACH_CALLBACK pfProvAttachCB
    )

  Summary:
    Start the provisioning service.

  Description:
    Starts a provisioning service on the specified port and protocol.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    type           - Interface protocol type.
    port           - Network port for service.
    ipType         - IP protocol type.
    pfProvAttachCB - Pointer to callback to receive events.

  Returns:
    WDRV_WINC_STATUS_OK             - A service start request was initiated.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_BUSY           - The service is already running.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_ProvServiceStart
(
    DRV_HANDLE handle,
    WDRV_WINC_PROV_PROTOCOL_TYPE type,
    uint16_t port,
    WDRV_WINC_IP_ADDRESS_TYPE ipType,
    WDRV_WINC_PROV_ATTACH_CALLBACK pfProvAttachCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_ProvServiceStop
    (
        DRV_HANDLE handle
    )

  Summary:
    Stop the provisioning service.

  Description:
    Stops a provisioning service.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    WDRV_WINC_STATUS_OK             - A service start request was initiated.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_ProvServiceStop
(
    DRV_HANDLE handle
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_MOD_DISABLE_PROV */
#endif /* WDRV_WINC_PROV_H */
