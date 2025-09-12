/*******************************************************************************
  WINC Wireless Driver External Crypto Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_extcrypto.h

  Summary:
    WINC wireless driver external crypto interface.

  Description:
    This interface manages crypto operations which are performed externally,
    from the perspective of the WINC IC.
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

#ifndef WDRV_WINC_EXTCRYPTO_H
#define WDRV_WINC_EXTCRYPTO_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
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
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

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

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_EXTCRYPTOProcessAEC
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

void WDRV_WINC_EXTCRYPTOProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_EXTCRYPTOSignResult
    (
        DRV_HANDLE handle,
        uintptr_t extCryptoCxt,
        bool status,
        const uint8_t *const pSignature,
        size_t lenSignature
    );

  Summary:
    Provide the result of an external signing operation

  Description:
    Provide the signature for a signing operation which has been performed
    externally, from the perspective of the WINC IC.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen must have been called to obtain a valid TLS handle.
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
    const uint8_t *const pSignature,
    size_t lenSignature
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_EXTCRYPTO_H */
