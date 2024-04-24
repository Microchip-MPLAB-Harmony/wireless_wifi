/*******************************************************************************
  WINC Driver External Crypto Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_extcrypto.h

  Summary:
    WINC wireless driver external crypto header file.

  Description:
    This interface manages crypto operations which are performed externally,
    from the perspective of the WINC IC.
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

#ifndef WDRV_WINC_EXTCRYPTO_H
#define WDRV_WINC_EXTCRYPTO_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver External Crypto Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  Signature algorithms

  Summary:
    List of supported signature algorithms.

  Description:
    This type defines the signature algorithms which may be used by external
    crypto operations.

  Remarks:
    None.
*/

typedef enum
{
    /* Invalid. */
    WDRV_WINC_EXTCRYPTO_SIG_ALGO_INVALID = 0,

    /* ECDSA secp256r1. */
    WDRV_WINC_EXTCRYPTO_SIG_ALGO_ECDSA_SECP256R1,

    /* ECDSA secp384r1. */
    WDRV_WINC_EXTCRYPTO_SIG_ALGO_ECDSA_SECP384R1,

    /* ECDSA secp521r1. */
    WDRV_WINC_EXTCRYPTO_SIG_ALGO_ECDSA_SECP521R1
} WDRV_WINC_EXTCRYPTO_SIG_ALGO;

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_EXTCRYPTO_SIGN_CB)
    (
        DRV_HANDLE handle,
        uintptr_t signCbCtx,
        WDRV_WINC_EXTCRYPTO_SIG_ALGO signAlgo,
        uint8_t *pSignValue,
        size_t signValueLen,
        uintptr_t extCryptoCxt
    )

  Summary:
    Callback to receive external signing requests.

  Description:
    Provides information for an external cryptographic signing operation.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    signCbCtx    - User context supplied to WDRV_WINC_TLSCtxSetSignCallback.
    signAlgo     - Signing algorithm.
    pSignValue   - Pointer to signing value.
    signValueLen - Length of signing value.
    extCryptoCxt - External crypto context to be passed to WDRV_WINC_EXTCRYPTOSignResult.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_EXTCRYPTO_SIGN_CB)
(
    DRV_HANDLE handle,
    uintptr_t signCbCtx,
    WDRV_WINC_EXTCRYPTO_SIG_ALGO signAlgo,
    uint8_t *pSignValue,
    size_t signValueLen,
    uintptr_t extCryptoCxt
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver External Crypto Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_EXTCRYPTOProcessAEC
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

void WDRV_WINC_EXTCRYPTOProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_DEV_EVENT_RSP_ELEMS *pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_EXTCRYPTOSignResult
    (
        DRV_HANDLE handle,
        uintptr_t extCryptoCxt,
        bool status,
        uint8_t *pSignature,
        size_t lenSignature
    );

  Summary:
    Provide the result of an external signing operation

  Description:
    Provide the signature for a signing operation which has been performed
    externally, from the perspective of the WINC IC.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.
    A request for an external signing operation must have been received via a
    WDRV_WINC_EXTCRYPTO_SIGN_CB function.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    extCryptoCxt - Context provided in the request.
    status       - True if operation succeeded, false otherwise.
    pSignature   - Pointer to the signature.
    lenSignature - Length of the signature.

    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_EXTCRYPTOSignResult
(
    DRV_HANDLE handle,
    uintptr_t extCryptoCxt,
    bool status,
    uint8_t *pSignature,
    size_t lenSignature
);

#endif /* WDRV_WINC_EXTCRYPTO_H */
