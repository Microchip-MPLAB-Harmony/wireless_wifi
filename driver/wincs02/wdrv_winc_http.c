/*******************************************************************************
  WINC Wireless Driver HTTP Client Source File

  File Name:
    wdrv_winc_http.c

  Summary:
    WINC wireless driver HTTP client implementation.

  Description:
    Provides an interface to manage the HTTP client.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_winc.h"

#ifndef WDRV_WINC_MOD_DISABLE_HTTP

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver HTTP Client Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_STATUS httpInProgress(const WDRV_WINC_DCPT *const pDcpt)

  Summary:
    Check if an HTTP operation is in progress.

  Description:
    Checks if an HTTP operation is in progress and other handle/pointer validation.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    pDcpt - Descriptor pointer obtained from WDRV_WINC_Open handle.

  Returns:
    WDRV_WINC_STATUS_OK            - There is no HTTP operation in progress.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The HTTP operation is in progress.

  Remarks:
    None.

*/

static WDRV_WINC_STATUS httpInProgress(const WDRV_WINC_DCPT *const pDcpt)
{
    /* Ensure the driver handle and user pointer is valid. */
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Ensure a download is not in progress. */

    if (WDRV_WINC_HTTP_CONF_ID_INVALID != pDcpt->pCtrl->httpState.confId)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    static void httpProcessCmdRsp
    (
        const WDRV_WINC_DCPT *const pDcpt,
        uint16_t rspId,
        WINC_CMD_REQ_HANDLE cmdReqHandle,
        const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
        int numElems,
        const WINC_DEV_PARAM_ELEM *const pElems
    )

  Summary:
    Process command responses.

  Description:
    Processes command responses received via WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED events.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    pDcpt        - Pointer to device descriptor.
    rspId        - Response command ID.
    cmdReqHandle - Command request handle.
    pSrcCmd      - Pointer to source command.
    numElems     - Number of elements in response.
    pElems       - Pointer to response elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void httpProcessCmdRsp
(
    const WDRV_WINC_DCPT *const pDcpt,
    uint16_t rspId,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems) || (NULL == pSrcCmd))
    {
        return;
    }

    switch (rspId)
    {
        case WINC_CMD_ID_HTTPRD:
        {
            uint8_t confId;
            WDRV_WINC_HTTP_EVENT_INFO eventInfo;

            if (numElems < 4U)
            {
                break;
            }

            if (NULL == pDcpt->pCtrl->httpState.pfEventCB)
            {
                break;
            }

            (void)memset(&eventInfo, 0, sizeof(eventInfo));

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER_UNSIGNED, &confId, sizeof(confId));

            if (confId != pDcpt->pCtrl->httpState.confId)
            {
                return;
            }

            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER_UNSIGNED, &eventInfo.bodyData.dataLength, sizeof(eventInfo.bodyData.dataLength));
            (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER_UNSIGNED, &eventInfo.bodyData.dataOffset, sizeof(eventInfo.bodyData.dataOffset));

            if (WINC_TYPE_BYTE_ARRAY == pElems[3].type)
            {
                eventInfo.bodyData.pData = pElems[3].pData;
            }

            pDcpt->pCtrl->httpState.pfEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_HTTP_EVENT_BODY_DATA, &eventInfo);

            break;
        }

        default:
        {
            /* Do nothing. */
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void httpProcessAEC
    (
        const WDRV_WINC_DCPT *const pDcpt,
        uint16_t aecId,
        int numElems,
        const WINC_DEV_PARAM_ELEM *const pElems
    )

  Summary:
    Process AECs.

  Description:
    Processes AECs for this module.

  Precondition:
    None.

  Parameters:
    pDcpt    - Pointer to device descriptor.
    aecId    - AEC ID.
    numElems - Number of elements.
    pElems   - Pointer to elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void httpProcessAEC
(
    const WDRV_WINC_DCPT *const pDcpt,
    uint16_t aecId,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    uint8_t confId = 0;
    WDRV_WINC_HTTP_EVENT_INFO eventInfo;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    if (WINC_MOD_ID_HTTP == (aecId >> 8))
    {
        if (numElems >= 1)
        {
            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER_UNSIGNED, &confId, sizeof(confId));

            if (confId != pDcpt->pCtrl->httpState.confId)
            {
                return;
            }
        }
    }

    (void)memset(&eventInfo, 0, sizeof(eventInfo));

    switch (aecId)
    {
        case WINC_AEC_ID_HTTPRSPCODE:
        {
            WINC_DEV_FRACT_INT_TYPE protoVer;

            if (numElems < 3)
            {
                break;
            }

            if (NULL == pDcpt->pCtrl->httpState.pfEventCB)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER_FRAC, &protoVer, sizeof(protoVer));

            eventInfo.msgLine.protoMajor = protoVer.i;
            eventInfo.msgLine.protoMinor = protoVer.f;

            (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER_UNSIGNED, &eventInfo.msgLine.rspStatusCode, sizeof(eventInfo.msgLine.rspStatusCode));

            pDcpt->pCtrl->httpState.pfEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_HTTP_EVENT_MSG_LINE, &eventInfo);

            break;
        }

        case WINC_AEC_ID_HTTPRSPFIELD:
        {
            if (numElems < 3)
            {
                break;
            }

            if (NULL == pDcpt->pCtrl->httpState.pfEventCB)
            {
                break;
            }

            if ((WINC_TYPE_STRING != pElems[1].type) || (WINC_TYPE_STRING != pElems[2].type))
            {
                break;
            }

            eventInfo.rspHeader.pName  = (char*)pElems[1].pData;
            eventInfo.rspHeader.pValue = (char*)pElems[2].pData;

            pDcpt->pCtrl->httpState.pfEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_HTTP_EVENT_RSP_HEADER, &eventInfo);

            break;
        }

        case WINC_AEC_ID_HTTPBODYRX:
        {
            if (numElems < 2)
            {
                break;
            }

            if (NULL == pDcpt->pCtrl->httpState.pfEventCB)
            {
                break;
            }

            if (2 == numElems)
            {
                (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER_UNSIGNED, &eventInfo.bodyData.pendingData, sizeof(eventInfo.bodyData.pendingData));

                pDcpt->pCtrl->httpState.pfEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_HTTP_EVENT_BODY_DATA, &eventInfo);
            }

            break;
        }

        case WINC_AEC_ID_HTTPBODYEND:
        {
            if (numElems < 2)
            {
                break;
            }

            if (NULL == pDcpt->pCtrl->httpState.pfEventCB)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER_UNSIGNED, &eventInfo.bodyEnd.totalDataLength, sizeof(eventInfo.bodyEnd.totalDataLength));

            if (numElems >= 4)
            {
                (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER_UNSIGNED, &eventInfo.bodyEnd.hashType, sizeof(eventInfo.bodyEnd.hashType));

                eventInfo.bodyEnd.hashLength = WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_BYTE_ARRAY, eventInfo.bodyEnd.hash, sizeof(eventInfo.bodyEnd.hash));
            }

            pDcpt->pCtrl->httpState.pfEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_HTTP_EVENT_BODY_COMPLETE, &eventInfo);

            break;
        }

        case WINC_AEC_ID_HTTPCL:
        {
            WDRV_WINC_HTTP_STATUS_CALLBACK pfEventCB;

            pDcpt->pCtrl->httpState.confId = WDRV_WINC_HTTP_CONF_ID_INVALID;

            if (NULL == pDcpt->pCtrl->httpState.pfEventCB)
            {
                break;
            }

            pfEventCB = pDcpt->pCtrl->httpState.pfEventCB;

            pDcpt->pCtrl->httpState.pfEventCB = NULL;

            pfEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_HTTP_EVENT_CLOSE, NULL);

            break;
        }

        case WINC_AEC_ID_HTTPERR:
        {
            if (numElems < 2)
            {
                break;
            }

            if (NULL == pDcpt->pCtrl->httpState.pfEventCB)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_STATUS, &eventInfo.error.status, sizeof(eventInfo.error.status));

            pDcpt->pCtrl->httpState.pfEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_HTTP_EVENT_ERROR, &eventInfo);

            break;
        }

        case WINC_AEC_ID_FSUP:
        {
            break;
        }

        default:
        {
            /* Do nothing. */
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void httpCmdRspCallbackHandler
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        WINC_CMD_REQ_HANDLE cmdReqHandle,
        WINC_DEV_CMDREQ_EVENT_TYPE event,
        uintptr_t eventArg
    )

  Summary:
    Command response callback handler.

  Description:
    Receives command responses for command requests originating from this module.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    context      - Context provided to WDRV_WINC_CmdReqInit for callback.
    devHandle    - WINC device handle.
    cmdReqHandle - Command request handle.
    event        - Command request event being raised.
    eventArg     - Optional event specific information.

  Returns:
    None.

  Remarks:
    Events:
        WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
            Command request transmission has been completed.

        WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
            Command request has completed, all commands processed.

            The command request has completed and is no longer active.
            Any memory associated with the request can be reclaimed.

        WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
            A command within the request has received a command status response.

            eventArg points to an WINC_DEV_EVENT_STATUS_ARGS structure.
            This structure contains the response ID, status, sequence number
            and an WINC_DEV_EVENT_SRC_CMD detailing the source command, the index
            within the request, number and location of parameters.

            Source command parameters can be retrieved using WINC_DevUnpackElements.
            Parameter content can then be retrieved using WINC_CmdReadParamElem.

        WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
            A command within the request has received a command response.

            eventArg points to an WINC_DEV_EVENT_RSP_ELEMS structure.
            This structure contains the response ID, number and contents of
            response elements and an WINC_DEV_EVENT_SRC_CMD detailing the
            source command, the index within the request, number and location
            of parameters.

            Response elements can be retrieved using WINC_CmdReadParamElem.

            Source command parameters can be retrieved using WINC_DevUnpackElements.
            Parameter content can then be retrieved using WINC_CmdReadParamElem.

*/

static void httpCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)context;

    if (NULL == pDcpt)
    {
        return;
    }

//    WDRV_DBG_INFORM_PRINT("HTTP CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

    switch (event)
    {
        case WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
        {
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
        {
            OSAL_Free((WINC_COMMAND_REQUEST*)cmdReqHandle);
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
        {
            /* Do nothing. */
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                httpProcessCmdRsp(pDcpt, pRspElems->rspId, cmdReqHandle, &pRspElems->srcCmd, pRspElems->numElems, pRspElems->elems);
            }

            break;
        }

        default:
        {
            /* Do nothing. */
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver HTTP Client Implementation
// *****************************************************************************
// *****************************************************************************

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

  Remarks:
    See wdrv_winc_http.h for usage information.

*/

void WDRV_WINC_HTTPProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)context;

    if ((NULL == pDcpt) || (NULL == pElems))
    {
        return;
    }

    httpProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
}

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

  Remarks:
    See wdrv_winc_http.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPDownloadFromURL
(
    DRV_HANDLE handle,
    const char *pURL,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const WDRV_WINC_FILE_DCPT_TYPE *pDestFile,
    const WDRV_WINC_HTTP_STATUS_CALLBACK pfEventCB
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;
    uint8_t tlsIdx;
    size_t urlLength;
    WDRV_WINC_HTTP_URL_ELEMS_TYPE urlElems;
    size_t extraDataLen;
    size_t destFileLen = 0;

    /* Ensure the driver is open and no HTTP operation is in progress. */
    status = httpInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    /* Ensure the user pointer is valid. */
    if (NULL == pURL)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    tlsIdx = WDRV_WINC_TLSCtxHandleToCfgIdx(tlsHandle);

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE != tlsHandle) && ((tlsIdx > WDRV_WINC_TLS_CTX_NUM) || (0 == tlsIdx) || (false == pDcpt->pCtrl->tlscInfo[tlsIdx-1U].idxInUse)))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    urlLength = strlen(pURL);

    status = WDRV_WINC_HTTPParseURL(pURL, urlLength, &urlElems);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    extraDataLen = urlLength;

    if (NULL != pDestFile)
    {
        if (NULL == pDestFile->pFilename)
        {
            pDestFile = NULL;
        }
        else
        {
            destFileLen = strnlen(pDestFile->pFilename, WINC_CMD_PARAM_SZ_FS_FILENAME+1);

            if (destFileLen > WINC_CMD_PARAM_SZ_FS_FILENAME)
            {
                return WDRV_WINC_STATUS_INVALID_ARG;
            }

            extraDataLen += destFileLen;
        }
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(8, extraDataLen, &httpCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if ((urlElems.userinfoLen > 0U) || (urlElems.hostLen > 0U))
    {
        /* Authority is being updated. */

        if (urlElems.userinfoLen > 0U)
        {
            (void)urlElems.pUserinfo; /* Suppress unused warning if debugging turned off. */
            WDRV_DBG_INFORM_PRINT("Auth  : %.*s\r\n", urlElems.userinfoLen, urlElems.pUserinfo);
        }

        if (urlElems.hostLen > 0U)
        {
            WDRV_DBG_INFORM_PRINT("Host  : %.*s : %d\r\n", urlElems.hostLen, urlElems.pHost, urlElems.authorityPort);

            (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_HOST, WINC_TYPE_STRING, (uintptr_t)urlElems.pHost, urlElems.hostLen);

            if (WDRV_WINC_IP_ADDRESS_TYPE_ANY != urlElems.ipAddrType)
            {
                WDRV_DBG_INFORM_PRINT("IP    : %d\r\n", urlElems.ipAddrType);
            }
        }
    }

    (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_PORT, WINC_TYPE_INTEGER_UNSIGNED, urlElems.authorityPort, 0);

    if (urlElems.pathLen > 0U)
    {
        /* Path and/or file is being updated. */

        const char *pFinalPath;
        size_t finalPathLen;

        /* Extract final element of path so we have path/file. */

        pFinalPath = strrchr(urlElems.pPath, (int)'/');

        if (NULL == pFinalPath)
        {
            pFinalPath       = urlElems.pPath;
            finalPathLen     = urlElems.pathLen;
            urlElems.pathLen = 0;
        }
        else
        {
            pFinalPath++;
            finalPathLen = urlElems.pathLen - (pFinalPath - urlElems.pPath);
            urlElems.pathLen -= finalPathLen;
        }

        if (urlElems.pathLen > 0U)
        {
            WDRV_DBG_INFORM_PRINT("Path  : %.*s\r\n", urlElems.pathLen, urlElems.pPath);
            (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_PATH, WINC_TYPE_STRING, (uintptr_t)urlElems.pPath, urlElems.pathLen);
        }

        if (finalPathLen > 0U)
        {
            WDRV_DBG_INFORM_PRINT("File  : %.*s\r\n", finalPathLen, pFinalPath);
            (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_FILE, WINC_TYPE_STRING, (uintptr_t)pFinalPath, finalPathLen);
        }
    }

    (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_TLS_CONF, WINC_TYPE_INTEGER, tlsIdx, 0);

    if (NULL != pDestFile)
    {
        (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_FILENAME, WINC_TYPE_STRING, (uintptr_t)pDestFile->pFilename, destFileLen);
        (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_FILETYPE, WINC_TYPE_INTEGER, pDestFile->type, 0);
    }
    else
    {
        (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_FILENAME, WINC_TYPE_STRING, (uintptr_t)NULL, 0);
        (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_FILETYPE, WINC_TYPE_INTEGER, WINC_CFG_PARAM_OPT_HTTP_CTX_FILETYPE_USER, 0);
    }

    (void)WINC_CmdHTTPGET(cmdReqHandle, 1, NULL, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->httpState.pfEventCB = pfEventCB;
    pDcpt->pCtrl->httpState.confId    = WINC_CMD_PARAM_MIN_HTTP_CONF;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_HTTPClose(DRV_HANDLE handle)

  Summary:
    Closes an active HTTP session.

  Description:
    Requests an active HTTP session is closed.

  Remarks:
    See wdrv_winc_http.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPClose(DRV_HANDLE handle)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;

    /* Ensure the driver is open and an HTTP operation is in progress. */
    status = httpInProgress(pDcpt);

    if (WDRV_WINC_STATUS_REQUEST_ERROR != status)
    {
        return status;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, &httpCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdHTTPCL(cmdReqHandle, 1);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_http.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_HTTPOptionsSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_HTTP_OPTIONS *const pHTTPOptions
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;
    size_t extraDataLen;
    size_t reqHdrsLen = 0;

    /* Ensure the driver is open and no HTTP operation is in progress. */
    status = httpInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    if (NULL == pHTTPOptions)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    extraDataLen = 0;

    if (NULL != pHTTPOptions->pRegHdrs)
    {
        reqHdrsLen = strlen(pHTTPOptions->pRegHdrs);

        if (reqHdrsLen > 0)
        {
            extraDataLen += reqHdrsLen;
        }
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(3, extraDataLen, &httpCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_TIMEOUT, WINC_TYPE_INTEGER_UNSIGNED, pHTTPOptions->timeout, 0);

    (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_REQ_HEADERS, WINC_TYPE_STRING, (uintptr_t)pHTTPOptions->pRegHdrs, reqHdrsLen);

    (void)WINC_CmdHTTPC(cmdReqHandle, 1, WINC_CFG_PARAM_ID_HTTP_CTX_CHECK_MODE, WINC_TYPE_INTEGER_UNSIGNED, pHTTPOptions->checkMode, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_http.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPBodyRead
(
    DRV_HANDLE handle,
    uint16_t numBytes
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (WDRV_WINC_HTTP_CONF_ID_INVALID == pDcpt->pCtrl->httpState.confId)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, &httpCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdHTTPRD(cmdReqHandle, 1, numBytes);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_HTTP */
