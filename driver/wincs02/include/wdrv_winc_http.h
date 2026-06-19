/*******************************************************************************
  WINC Wireless Driver HTTP Client Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_http.h

  Summary:
    WINC wireless driver HTTP client interface.

  Description:
    Provides an interface to configure the HTTP client on the WINC.
 *******************************************************************************/

/*
Copyright (C) 2025-26 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#ifndef WDRV_WINC_HTTP_H
#define WDRV_WINC_HTTP_H

#ifndef WDRV_WINC_MOD_DISABLE_HTTP

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver HTTP Client Defines
// *****************************************************************************
// *****************************************************************************

/* Invalid configuration ID value. */
#define WDRV_WINC_HTTP_CONF_ID_INVALID      (WINC_CMD_PARAM_MIN_HTTP_CONF-1)

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver HTTP Client Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  HTTP Events.

  Summary:
    Possible HTTP events.

  Description:
    Possible HTTP events used in callbacks to application.

  Remarks:
    None.

*/

typedef enum
{
    /* The HTTP message line has been received. */
    WDRV_WINC_HTTP_EVENT_MSG_LINE,

    /* A response header value has been received. */
    WDRV_WINC_HTTP_EVENT_RSP_HEADER,

    /* Data from the HTTP body has been received. */
    WDRV_WINC_HTTP_EVENT_BODY_DATA,

    /* Reception of the HTTP body is now complete. */
    WDRV_WINC_HTTP_EVENT_BODY_COMPLETE,

    /* The HTTP session has been closed. */
    WDRV_WINC_HTTP_EVENT_CLOSE,

    /* An error has occurred. */
    WDRV_WINC_HTTP_EVENT_ERROR
} WDRV_WINC_HTTP_EVENT_TYPE;

// *****************************************************************************
/* HTTP Check Type

  Summary:
    HTTP check algorithm types.

  Description:
    Possible hashing algorithms to use for checking HTTP payloads.

  Remarks:
    None.

*/

typedef enum
{
    WDRV_WINC_HTTP_CHECK_MODE_NONE = WINC_CFG_PARAM_OPT_HTTP_CTX_CHECK_MODE_NONE,

    /* SHA1. */
    WDRV_WINC_HTTP_CHECK_MODE_SHA1 = WINC_CFG_PARAM_OPT_HTTP_CTX_CHECK_MODE_SHA,

    /* SHA-224. */
    WDRV_WINC_HTTP_CHECK_MODE_SHA224 = WINC_CFG_PARAM_OPT_HTTP_CTX_CHECK_MODE_SHA224,

    /* SHA-256. */
    WDRV_WINC_HTTP_CHECK_MODE_SHA256 = WINC_CFG_PARAM_OPT_HTTP_CTX_CHECK_MODE_SHA256,

    /* SHA-384. */
    WDRV_WINC_HTTP_CHECK_MODE_SHA384 = WINC_CFG_PARAM_OPT_HTTP_CTX_CHECK_MODE_SHA384,

    /* SHA-512. */
    WDRV_WINC_HTTP_CHECK_MODE_SHA512 = WINC_CFG_PARAM_OPT_HTTP_CTX_CHECK_MODE_SHA512
} WDRV_WINC_HTTP_CHECK_MODE_TYPE;

// *****************************************************************************
/*  HTTP Event Information.

  Summary:
    Compound structure for HTTP event feedback.

  Description:
    Contains structures for each HTTP event.

  Remarks:
    None.

*/

typedef struct
{
    union
    {
        /* WDRV_WINC_HTTP_EVENT_MSG_LINE structure. */
        struct
        {
            /* HTTP protocol major version. */
            uint8_t protoMajor;

            /* HTTP protocol minor version. */
            uint8_t protoMinor;

            /* HTTP response status code. */
            uint16_t rspStatusCode;
        } msgLine;

        /* WDRV_WINC_HTTP_EVENT_RSP_HEADER structure. */
        struct
        {
            /* Pointer to header name string. */
            const char *pName;

            /* Pointer to header value string. */
            const char *pValue;
        } rspHeader;

        /* WDRV_WINC_HTTP_EVENT_BODY_DATA structure. */
        struct
        {
            /* Number of pending data byte which could be read. */
            uint16_t pendingData;

            /* Length of data pointed to by pData. */
            uint16_t dataLength;

            /* Offset within the body of the data pointed to by pData. */
            uint32_t dataOffset;

            /* Data which has been read. */
            const uint8_t *pData;
        } bodyData;

        /* WDRV_WINC_HTTP_EVENT_BODY_COMPLETE structure. */
        struct
        {
            /* Total length of body received. */
            uint32_t totalDataLength;

            /* Hash type used on the HTTP body. */
            WDRV_WINC_HTTP_CHECK_MODE_TYPE hashType;

            /* Length of hash value. */
            uint8_t hashLength;

            /* Hash of body data. */
            uint8_t hash[CRYPT_SHA384_DIGEST_SIZE];
        } bodyEnd;

        /* WDRV_WINC_HTTP_EVENT_ERROR structure. */
        struct
        {
            /* Error status code. */
            uint16_t status;
        } error;
    };
} WDRV_WINC_HTTP_EVENT_INFO;

// *****************************************************************************
/* HTTP Status Callback Function Pointer

  Function:
    void (*WDRV_WINC_HTTP_STATUS_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_HTTP_EVENT_TYPE event,
        WDRV_WINC_HTTP_EVENT_INFO *pInfo
    )

  Summary:
    Pointer to an HTTP status callback function.

  Description:
    This defines an HTTP status function callback type which can be passed
    into certain HTTP functions to receive feedback.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    event  - HTTP event being notified to application.
    pInfo  - Pointer to event information structure.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_HTTP_STATUS_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_HTTP_EVENT_TYPE event,
    WDRV_WINC_HTTP_EVENT_INFO *pInfo
);

// *****************************************************************************
/*  HTTP State.

  Summary:
    HTTP operation state.

  Description:
    Contains HTTP state variables.

  Remarks:
    None.

*/

typedef struct
{
    /* WINCS02 HTTP configuration ID for the session. */
    uint8_t confId;

    /* Callback registered by WDRV_WINC_HTTPDownloadFromURL. */
    WDRV_WINC_HTTP_STATUS_CALLBACK pfEventCB;
} WDRV_WINC_HTTP_STATE;

// *****************************************************************************
/*  HTTP Options

  Summary:
    Structure containing HTTP options.

  Description:
    Contains HTTP options.

  Remarks:
    None.
*/

typedef struct
{
    /* HTTP session inactivity timeout, in seconds. */
    uint8_t timeout;

    /* Pointer to additional request headers to send. */
    const char *pRegHdrs;

    /* Body hash check algorithm to use. */
    WDRV_WINC_HTTP_CHECK_MODE_TYPE checkMode;
} WDRV_WINC_HTTP_OPTIONS;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver HTTP Client Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_HTTPProcessAEC
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

void WDRV_WINC_HTTPProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_HTTPDownloadFromURL
    (
        DRV_HANDLE handle,
        const char *pURL,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const WDRV_WINC_FILE_DCPT_TYPE *pDestFile,
        const WDRV_WINC_HTTP_STATUS_CALLBACK pfEventCB
    )

  Summary:
    Perform an HTTP download from the URL provided.

  Description:
    Using the URL provided the WINC will attempt to download the file.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    pURL      - Pointer to string containing URL of file.
    tlsHandle - TLS handle if required.
    pDestFile - Pointer to optional file structure.
    pfEventCB - Callback to indicate events.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    If pDestFile is NULL the HTTP data will be notified through the callback
    and can be read using WDRV_WINC_HTTPBodyRead.
    If pDestFile is not NULL the HTTP date will be directed to the file
    name and type in the WINCS02 filesystem.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPDownloadFromURL
(
    DRV_HANDLE handle,
    const char *pURL,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const WDRV_WINC_FILE_DCPT_TYPE *pDestFile,
    const WDRV_WINC_HTTP_STATUS_CALLBACK pfEventCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_HTTPClose(DRV_HANDLE handle)

  Summary:
    Closes an active HTTP session.

  Description:
    Requests an active HTTP session is closed.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    The close will be confirmed via the callback registered with WDRV_WINC_HTTPDownloadFromURL
    using the WDRV_WINC_HTTP_EVENT_CLOSE event.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPClose(DRV_HANDLE handle);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_HTTPOptionsSet
    (
        DRV_HANDLE handle,
        const WDRV_WINC_HTTP_OPTIONS *const pHTTPOptions
    )

  Summary:
    Sets the HTTP options.

  Description:
    Configures HTTP options.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    pHTTPOptions - Pointer to new HTTP options to configure.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPOptionsSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_HTTP_OPTIONS *const pHTTPOptions
);

//*******************************************************************************
/*
  Function:
        WDRV_WINC_STATUS WDRV_WINC_HTTPBodyRead
        (
            DRV_HANDLE handle,
            uint16_t numBytes
        )

  Summary:
    Read HTTP body data.

  Description:
    Requests that HTTP body data be read.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    Data will be read via the callback registered with WDRV_WINC_HTTPDownloadFromURL
    using the WDRV_WINC_HTTP_EVENT_BODY_DATA event.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPBodyRead
(
    DRV_HANDLE handle,
    uint16_t numBytes
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_MOD_DISABLE_HTTP */
#endif /* WDRV_WINC_HTTP_H */
