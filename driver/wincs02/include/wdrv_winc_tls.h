/*******************************************************************************
  WINC Driver TLS Context Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_tls.h

  Summary:
    WINC wireless driver TLS context header file.

  Description:
    This interface manages the TLS contexts.
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

#ifndef WDRV_WINC_TLS_H
#define WDRV_WINC_TLS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"
#include "wdrv_winc_extcrypto.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver TLS Context Defines
// *****************************************************************************
// *****************************************************************************

#define WDRV_WINC_TLS_INVALID_HANDLE                    0
#define WDRV_WINC_TLS_CS_INVALID_HANDLE                 0

#define WDRV_WINC_TLSCtxHandleToCfgIdx(HANDLE)          (HANDLE)

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver TLS Context Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* TLS Context Handle

  Summary:
    TLS context handle.

  Description:
    Handle representing TLS contexts.

  Remarks:
    None.
*/

typedef uint8_t WDRV_WINC_TLS_HANDLE;

// *****************************************************************************
/* TLS Cipher Suite Context Handle

  Summary:
    TLS cipher suite handle.

  Description:
    Handle representing TLS cipher suites.

  Remarks:
    None.
*/

typedef uint8_t WDRV_WINC_TLS_CS_HANDLE;

// *****************************************************************************
/* TLS Context Information

  Summary:
    TLS context information structure.

  Description:
    Structure containing information about a TLS context.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if context is in use. */
    bool idxInUse;

    /* Callback for signing operations. */
    WDRV_WINC_EXTCRYPTO_SIGN_CB pfSignCB;

    /* User supplied context passed to signing callback. */
    uintptr_t signCbCtx;

    /* TLS cipher suite handle */
    int tlsCsHandle;
} WDRV_WINC_TLSCTX_INFO;

//*******************************************************************************
/*
  Function:
    void (*WDRV_WINC_TLS_CS_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
        uint16_t *pAlgorithms,
        uint8_t numAlgorithms,
        bool success
    )

  Summary:
    TLS cipher suite callback.

  Description:
    Callback will be called to provide TLS cipher suite information.

  Precondition:
    WDRV_WINC_TLSCipherSuiteAlgGet must be called to register the callback.

  Parameters:
    handle        - Client handle obtained by a call to WDRV_WINC_Open.
    tlsCsHandle   - TLS cipher suite handle obtained by a call to WDRV_WINC_TLSCipherSuiteOpen.
    pAlgorithms   - Pointer to buffer to receive cipher suite algorithm list.
    numAlgorithms - Number of cipher suite algorithms in the buffer provided.
    success       - Flag indicating if the operation was a success.

  Returns:
    None.

  Remarks:
    When used with WDRV_WINC_TLSCipherSuiteAlgSet, if any individual cipher
    suites are in error this callback will be called with pAlgorithms pointing
    to the cipher suite that failed (and numAlgorithms will be 1). The overall
    success or failure of the operation will be indicated by a final call to the
    callback with pAlgorithms being NULL.

    When used with WDRV_WINC_TLSCipherSuiteAlgGet, the overall success or
    failure of the operation will be indicated by a single call to the callback.
*/

typedef void (*WDRV_WINC_TLS_CS_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
    uint16_t *pAlgorithms,
    uint8_t numAlgorithms,
    bool success
);

// *****************************************************************************
/* TLS Cipher Suite Context Information

  Summary:
    TLS cipher suite information structure.

  Description:
    Structure containing information about a TLS cipher suite.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if context is in use. */
    bool idxInUse;

    /* Set operation command request handle. */
    WINC_CMD_REQ_HANDLE setCmdReqHandle;
} WDRV_WINC_TLSCSCTX_INFO;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver TLS Context Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_TLS_HANDLE WDRV_WINC_TLSCtxOpen(DRV_HANDLE handle)

  Summary:
    Open a TLS context.

  Description:
    Obtains a handle to a free TLS context.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    TLS handle or WDRV_WINC_TLS_INVALID_HANDLE.

  Remarks:
    None.

*/

WDRV_WINC_TLS_HANDLE WDRV_WINC_TLSCtxOpen(DRV_HANDLE handle);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_TLSCtxClose(DRV_HANDLE handle, WDRV_WINC_TLS_HANDLE tlsHandle)

  Summary:
    Closes a TLS context.

  Description:
    Frees a TLS context handle for other uses.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    tlsHandle - TLS handle obtained by a call to WDRV_WINC_TLSCtxOpen.

  Returns:
    None.

  Remarks:
    None.

*/

void WDRV_WINC_TLSCtxClose(DRV_HANDLE handle, WDRV_WINC_TLS_HANDLE tlsHandle);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxCACertFileSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pCACertName,
        bool peerAuth
    )

  Summary:
    Sets the certificate authority field of a TLS context.

  Description:
    Sets the certificate authority field of a TLS context.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    tlsHandle   - TLS handle obtained by a call to WDRV_WINC_TLSCtxOpen.
    pCACertName - Pointer to certificate authority name.
    peerAuth    - Flag indicating if peer authentication is enabled.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    To ensure proper authentication checks are performed the value of peerAuth
    should always be 'true', setting this parameter to 'false' will weaken
    authentication checks.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxCACertFileSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pCACertName,
    bool peerAuth
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxCertFileSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pCertName
    )

  Summary:
    Sets the certificate field of a TLS context.

  Description:
    Sets the certificate field of a TLS context.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    tlsHandle - TLS handle obtained by a call to WDRV_WINC_TLSCtxOpen.
    pCertName - Pointer to certificate name.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxCertFileSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pCertName
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxPrivKeySet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pPrivKeyName,
        const char *pPrivKeyPassword
    )

  Summary:
    Sets the private key name and password fields of a TLS context.

  Description:
    Sets the private key name and password fields of a TLS context.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    tlsHandle        - TLS handle obtained by a call to WDRV_WINC_TLSCtxOpen.
    pPrivKeyName     - Pointer to private key name.
    pPrivKeyPassword - Pointer to private key password.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxPrivKeySet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pPrivKeyName,
    const char *pPrivKeyPassword
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxSNISet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pServerName
    )

  Summary:
    Sets the SNI field of a TLS context.

  Description:
    Sets the SNI field of a TLS context.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    tlsHandle   - TLS handle obtained by a call to WDRV_WINC_TLSCtxOpen.
    pServerName - Pointer to SNI name.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxSNISet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pServerName
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxHostnameCheckSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pDomainName,
        bool verifyDomain
    )

  Summary:
    Sets the hostname check field of a TLS context.

  Description:
    Sets the hostname check field of a TLS context.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    tlsHandle    - TLS handle obtained by a call to WDRV_WINC_TLSCtxOpen.
    pDomainName  - Pointer to host domain name.
    verifyDomain - Flag indicating if domain verification is enabled.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    To ensure proper domain checks are performed the value of verifyDomain
    should always be 'true', setting this parameter to 'false' will weaken
    verification checks.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxHostnameCheckSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pDomainName,
    bool verifyDomain
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxSetSignCallback
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        WDRV_WINC_EXTCRYPTO_SIGN_CB pfSignCb,
        uintptr_t signCbCtx
    )

  Summary:
    Set the signing callback.

  Description:
    Sets the signing callback for external cryptographic functions.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    tlsHandle - TLS handle obtained by a call to WDRV_WINC_TLSCtxOpen.
    pfSignCb  - Pointer to user callback for signing operations.
    signCbCtx - User supplied context to pass to signing callback.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxSetSignCallback
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    WDRV_WINC_EXTCRYPTO_SIGN_CB pfSignCb,
    uintptr_t signCbCtx
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxCipherSuiteSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle
    )

  Summary:
    Set the TLS cipher suite.

  Description:
    Sets the TLS cipher suite.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    tlsHandle   - TLS handle obtained by a call to WDRV_WINC_TLSCtxOpen.
    tlsCsHandle - TLS cipher suite handle obtained by a call to WDRV_WINC_TLSCipherSuiteOpen.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxCipherSuiteSet(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_TLS_HANDLE WDRV_WINC_TLSCipherSuiteOpen(DRV_HANDLE handle)

  Summary:
    Open a TLS cipher suite.

  Description:
    Obtains a handle to a free TLS cipher suite. Used for configuring
    specific cipher algorithms for a TLS connection.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    TLS cipher suite handle or WDRV_WINC_TLS_INVALID_HANDLE.

  Remarks:
    None.

*/

WDRV_WINC_TLS_CS_HANDLE WDRV_WINC_TLSCipherSuiteOpen(DRV_HANDLE handle);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_TLSCipherSuiteClose
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle
    )

  Summary:
    Closes a TLS cipher suite.

  Description:
    Frees a TLS cipher suite handle for other uses.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCipherSuiteOpen should have been called to obtain a valid TLS cipher suite handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    tlsCsHandle - TLS cipher suite handle obtained by a call to WDRV_WINC_TLSCipherSuiteOpen.

  Returns:
    None.

  Remarks:
    None.

*/

void WDRV_WINC_TLSCipherSuiteClose
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
        uint16_t *pAlgorithms,
        uint8_t numAlgorithms,
        const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB
    )

  Summary:
    Set a list of ciphers suite algorithms requested to the TLS cipher suite.

  Description:
    Set a list of ciphers suite algorithms requested to the TLS cipher suite.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle            - Client handle obtained by a call to WDRV_WINC_Open.
    tlsCsHandle       - TLS handle obtained by a call to WDRV_WINC_TLSCipherSuiteOpen.
    pAlgorithms       - Pointer to list of cipher suite algorithms to set.
    numAlgorithms     - Number of cipher suite algorithms to set.
    pfTlsCsResponseCB - Pointer to user callback for the set response.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.
    WDRV_WINC_STATUS_RETRY_REQUEST - Another request is being processed, try again.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgSet(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
    uint16_t *pAlgorithms,
    uint8_t numAlgorithms,
    const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
        const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB,
        bool getDefaults
    )

  Summary:
    Gets the current or default cipher suite algorithms set for a given context.

  Description:
     Gets the current or default cipher suite algorithms set for a given context.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_TLSCtxOpen should have been called to obtain a valid TLS handle.

  Parameters:
    handle            - Client handle obtained by a call to WDRV_WINC_Open.
    tlsCsHandle       - TLS cipher suite handle obtained by a call to WDRV_WINC_TLSCipherSuiteOpen.
    pfTlsCsResponseCB - Pointer to user callback for the get response.
    getDefaults       - Flag indicating if default values should be returned.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.
    WDRV_WINC_STATUS_RETRY_REQUEST - Another request is being processed, try again.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgGet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
    const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB,
    bool getDefaults
);

#endif /* WDRV_WINC_TLS_H */
