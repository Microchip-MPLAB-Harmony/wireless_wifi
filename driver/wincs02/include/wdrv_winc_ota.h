/*******************************************************************************
  WINC Wireless Driver OTA Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_ota.h

  Summary:
    WINC wireless driver OTA header file.

  Description:
    This interface provides functionality required for the OTA service.
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

#ifndef WDRV_WINC_OTA_H
#define WDRV_WINC_OTA_H

#ifndef WDRV_WINC_MOD_DISABLE_OTA

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
// Section: WINC Driver OTA Defines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver OTA Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  OTA Update Status

  Summary:
    OTA update statuses.

  Description:
    List of possible OTA update statuses.

  Remarks:
    None.

*/

typedef enum
{
    /* The OTA operation has started. */
    WDRV_WINC_OTA_STATUS_STARTED,

    /* OTA completed successfully. */
    WDRV_WINC_OTA_STATUS_COMPLETE,

    /* OTA failed. */
    WDRV_WINC_OTA_STATUS_FAIL,

    /* The OTA URL was invalid. */
    WDRV_WINC_OTA_STATUS_INVALID_URL,

    /* The flash is too small for the update. */
    WDRV_WINC_OTA_STATUS_INSUFFICIENT_FLASH,

    /* An OTA operation is in progress. */
    WDRV_WINC_OTA_STATUS_BUSY,

    /* Verification of the OTA image failed. */
    WDRV_WINC_OTA_STATUS_VERIFY_FAILED,

    /* An error occurred during the connection. */
    WDRV_WINC_OTA_STATUS_CONN_ERROR,

    /* The server returned an error. */
    WDRV_WINC_OTA_STATUS_SERVER_ERROR,
} WDRV_WINC_OTA_UPDATE_STATUS;

// *****************************************************************************
/*  OTA Operation

  Summary:
    OTA operation types.

  Description:
    List of possible OTA operation types.

  Remarks:
    None.

*/

typedef enum
{
    /* No operation. */
    WDRV_WINC_OTA_OPERATION_NONE,

    /* Download and verify OTA image. */
    WDRV_WINC_OTA_OPERATION_DOWNLOAD_VERIFY,

    /* Verify OTA image. */
    WDRV_WINC_OTA_OPERATION_VERIFY,

    /* Activate OTA image. */
    WDRV_WINC_OTA_OPERATION_ACTIVATE,

    /* Invalidate image. */
    WDRV_WINC_OTA_OPERATION_INVALIDATE,
} WDRV_WINC_OTA_OPERATION_TYPE;

// *****************************************************************************
/* OTA Status Callback Function Pointer

  Function:
    void (*WDRV_WINC_OTA_STATUS_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_OTA_OPERATION_TYPE operation,
        uint8_t opId,
        WDRV_WINC_OTA_UPDATE_STATUS status
    )

  Summary:
    Pointer to an OTA status callback function.

  Description:
    This defines an OTA status function callback type which can be passed
    into certain OTA functions to receive feedback.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    operation - Indicates what operation the status refers to.
    opId      - Operation ID.
    status    - A status value.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_OTA_STATUS_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_OTA_OPERATION_TYPE operation,
    uint8_t opId,
    WDRV_WINC_OTA_UPDATE_STATUS status
);

// *****************************************************************************
/*  OTA Operation State

  Summary:
    OTA operation state structure.

  Description:
    Structure containing details relating to an OTA operation.

  Remarks:
    None.

*/

typedef struct
{
    /* Indicate what OTA operation is in progress. */
    WDRV_WINC_OTA_OPERATION_TYPE operation;

    /* Current operation ID. */
    uint8_t opId;

    /* Callback to use for events relating to firmware update downloads. */
    WDRV_WINC_OTA_STATUS_CALLBACK pfOperationStatusCB;
} WDRV_WINC_OTA_OPERATION_STATE;

// *****************************************************************************
/*  OTA Options

  Summary:
    Structure containing OTA options.

  Description:
    Contains OTA options.

  Remarks:
    None.
*/

typedef struct
{
    uint8_t timeout;
} WDRV_WINC_OTA_OPTIONS;

// *****************************************************************************
// Section: WINC Driver OTA Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_OTAProcessAEC
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

void WDRV_WINC_OTAProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_OTAUpdateFromURL
    (
        DRV_HANDLE handle,
        const char *pURL,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Perform OTA from URL provided.

  Description:
    Using the URL provided the WINC will attempt to download and store the
      updated firmware image.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    pURL             - Pointer to string containing URL of firmware image.
    tlsHandle        - TLS handle if required.
    pfUpdateStatusCB - Callback to indicate update status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAUpdateFromURL
(
    DRV_HANDLE handle,
    const char *pURL,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_OTAImageVerify
    (
        DRV_HANDLE handle,
        const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Verifies an OTA image.

  Description:
    Verifies an OTA image located in the NVM memory to check if it can be
      activated.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    pfUpdateStatusCB - Callback to indicate update status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAImageVerify
(
    DRV_HANDLE handle,
    const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_OTAImageActivate
    (
        DRV_HANDLE handle,
        const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Marks a firmware image as active.

  Description:
    Marks a firmware image in NVM memory as active so that it will be used on
      the next boot.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    pfUpdateStatusCB - Callback to indicate update status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAImageActivate
(
    DRV_HANDLE handle,
    const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_OTAImageInvalidate
    (
        DRV_HANDLE handle,
        const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Mark a firmware image as invalid.

  Description:
    Mark a firmware image located in NVM memory as invalid to allow the
      alternative image to be used on next boot.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    pfUpdateStatusCB - Callback to indicate update status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAImageInvalidate
(
    DRV_HANDLE handle,
    const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_OTAOptionsSet
    (
        DRV_HANDLE handle,
        const WDRV_WINC_OTA_OPTIONS *const pOTAOptions
    )

  Summary:
    Sets the OTA options.

  Description:
    Configures OTA options.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    pOTAOptions - Pointer to new OTA options to configure.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAOptionsSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_OTA_OPTIONS *const pOTAOptions
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_MOD_DISABLE_OTA */
#endif /* WDRV_WINC_OTA_H */
