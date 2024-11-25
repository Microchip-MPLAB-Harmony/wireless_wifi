/*******************************************************************************
  WINC Wireless Driver OTA Implementation

  File Name:
    wdrv_winc_ota.c

  Summary:
    WINC wireless driver OTA implementation.

  Description:
    This interface provides functionality required for the OTA service.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_ota.h"

#ifndef WDRV_WINC_MOD_DISABLE_OTA

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver OTA Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void otaProcessCmdRsp(DRV_HANDLE handle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)

  Summary:
    Process command responses.

  Description:
    Processes command responses received via WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED events.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    handle - WINC device handle.
    pElems - Pointer to command response elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void otaProcessCmdRsp(DRV_HANDLE handle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_CMD_ID_OTADL:
        case WINC_CMD_ID_OTAVFY:
        case WINC_CMD_ID_OTAACT:
        case WINC_CMD_ID_OTAINV:
        {
            if (1U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->otaState.opId, sizeof(pDcpt->pCtrl->otaState.opId));
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("OTA CmdRspCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void otaCmdRspCallbackHandler
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

static void otaCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)context;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return;
    }

    //WDRV_DBG_INFORM_PRINT("OTA CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
            /* Status response received for command. */

            const WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (const WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (NULL != pStatusInfo)
            {
                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_OTADL:
                    case WINC_CMD_ID_OTAVFY:
                    case WINC_CMD_ID_OTAACT:
                    case WINC_CMD_ID_OTAINV:
                    {
                        if (WINC_STATUS_OK != pStatusInfo->status)
                        {
                            if (NULL != pDcpt->pCtrl->otaState.pfOperationStatusCB)
                            {
                                pDcpt->pCtrl->otaState.pfOperationStatusCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->otaState.operation, pDcpt->pCtrl->otaState.opId, WDRV_WINC_OTA_STATUS_FAIL);
                            }

                            pDcpt->pCtrl->otaState.operation = WDRV_WINC_OTA_OPERATION_NONE;
                        }
                        else
                        {
                            if (NULL != pDcpt->pCtrl->otaState.pfOperationStatusCB)
                            {
                                pDcpt->pCtrl->otaState.pfOperationStatusCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->otaState.operation, pDcpt->pCtrl->otaState.opId, WDRV_WINC_OTA_STATUS_STARTED);
                            }
                        }

                        break;
                    }

                    default:
                    {
                        WDRV_DBG_VERBOSE_PRINT("OTA CmdRspCB %08x ID %04x status %04x not handled\r\n", cmdReqHandle, pStatusInfo->rspCmdId, pStatusInfo->status);
                        break;
                    }
                }
            }
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                otaProcessCmdRsp((DRV_HANDLE)pDcpt, pRspElems);
            }

            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("OTA CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_STATUS otaInProgress(WDRV_WINC_DCPT *const pDcpt)

  Summary:
    Check if an OTA operation is in progress.

  Description:
    Checks if an OTA operation is in progress and other handle/pointer validation.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    pDcpt - Descriptor pointer obtained from WDRV_WINC_Open handle.

  Returns:
    WDRV_WINC_STATUS_OK            - There is no OTA operation in progress.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.

  Remarks:
    None.

*/

static WDRV_WINC_STATUS otaInProgress(WDRV_WINC_DCPT *const pDcpt)
{
    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == (DRV_HANDLE)pDcpt) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Ensure an update is not in progress. */
    if (WDRV_WINC_OTA_OPERATION_NONE != pDcpt->pCtrl->otaState.operation)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_ota.h for usage information.

*/

void WDRV_WINC_OTAProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)context;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_OTA:
        case WINC_AEC_ID_OTAERR:
        {
            WDRV_WINC_OTA_UPDATE_STATUS otaStatus = WDRV_WINC_OTA_STATUS_FAIL;
            uint16_t status;
            uint8_t opId;
            bool isComplete = true;

            if (WDRV_WINC_OTA_OPERATION_NONE == pDcpt->pCtrl->otaState.operation)
            {
                break;
            }

            if (2U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_UNSIGNED, &opId, sizeof(opId));
            (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_STATUS, &status, sizeof(status));

            if (opId != pDcpt->pCtrl->otaState.opId)
            {
                break;
            }

            switch (status)
            {
                case WINC_STATUS_ERASE_DONE:
                case WINC_STATUS_WRITE_DONE:
                {
                    isComplete = false;
                    break;
                }

                case WINC_STATUS_VERIFY_DONE:
                case WINC_STATUS_ACTIVATE_DONE:
                case WINC_STATUS_INVALIDATE_DONE:
                {
                    otaStatus = WDRV_WINC_OTA_STATUS_COMPLETE;
                    break;
                }

                case WINC_STATUS_OTA_ERROR:
                {
                    otaStatus = WDRV_WINC_OTA_STATUS_FAIL;
                    break;
                }

                case WINC_STATUS_OTA_NO_STA_CONN:
                {
                    otaStatus = WDRV_WINC_OTA_STATUS_CONN_ERROR;
                    break;
                }

                case WINC_STATUS_OTA_PROTOCOL_ERROR:
                case WINC_STATUS_OTA_TLS_ERROR:
                {
                    otaStatus = WDRV_WINC_OTA_STATUS_SERVER_ERROR;
                    break;
                }

                case WINC_STATUS_OTA_IMG_TOO_LARGE:
                {
                    otaStatus = WDRV_WINC_OTA_STATUS_INSUFFICIENT_FLASH;
                    break;
                }

                case WINC_STATUS_OTA_TIMEOUT:
                {
                    otaStatus = WDRV_WINC_OTA_STATUS_CONN_ERROR;
                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("OTA AECCB status %d not handled\r\n", status);
                    break;
                }
            }

            if (true == isComplete)
            {
                WDRV_WINC_OTA_OPERATION_TYPE otaOperation = pDcpt->pCtrl->otaState.operation;

                pDcpt->pCtrl->otaState.operation = WDRV_WINC_OTA_OPERATION_NONE;

                if (NULL != pDcpt->pCtrl->otaState.pfOperationStatusCB)
                {
                    pDcpt->pCtrl->otaState.pfOperationStatusCB((DRV_HANDLE)pDcpt, otaOperation, pDcpt->pCtrl->otaState.opId, otaStatus);

                    if (WDRV_WINC_OTA_OPERATION_NONE == pDcpt->pCtrl->otaState.operation)
                    {
                        pDcpt->pCtrl->otaState.pfOperationStatusCB = NULL;
                    }
                }
            }

            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("OTA AECCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

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

  Remarks:
    See wdrv_winc_ota.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAUpdateFromURL
(
    DRV_HANDLE handle,
    const char *pURL,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;
    size_t urlLength;
    uint16_t authorityPort = 0;
    const char *pScheme;
    size_t schemeLen;
    const char *pUserinfo;
    size_t userinfoLen;
    const char *pHost;
    size_t hostLen;
    const char *pPath;
    size_t pathLen;
    uint8_t tlsIdx;
    WDRV_WINC_IP_MULTI_ADDRESS ipAddr;
    WDRV_WINC_IP_ADDRESS_TYPE ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_ANY;

    /* Ensure the driver is open and no OTA operation is in progress. */
    status = otaInProgress(pDcpt);

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

    cmdReqHandle = WDRV_WINC_CmdReqInit(6, urlLength, otaCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /*
        URI       = scheme ":" ["/" "/" authority ] path ["?" query] ["#" fragment]
        authority = [userinfo "@"] host [":" port]
    */

    pScheme   = pURL;
    schemeLen = 0;

    /* Search URL looking for ':' or the end. */

    while(('\0' != *pURL) && (urlLength > 0U) && (':' != *pURL))
    {
        schemeLen++;
        pURL++;
        urlLength--;
    }

    /* Scheme must be present and terminated with ':'. */

    if ((0U == schemeLen) || (':' != *pURL))
    {
        WDRV_DBG_ERROR_PRINT("URL: Invalid scheme\r\n");
        WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Skip ':'. */

    pURL++;
    urlLength--;

    pUserinfo   = NULL;
    userinfoLen = 0;
    pHost       = NULL;
    hostLen     = 0;

    /* Optional authority must start with '/' '/'. */

    if ((urlLength >= 2U) && (0 == strncmp(pURL, "//", 2)))
    {
        const char *pEndUserInfoPtr;
        char ipAddrBuf[64];

        pURL      += 2U;
        urlLength -= 2U;

        /* Extract userinfo component if present. */

        pEndUserInfoPtr = strchr(pURL, (int)'@');

        if ((NULL != pEndUserInfoPtr) && (pEndUserInfoPtr > pURL))
        {
            pUserinfo   = pURL;
            userinfoLen = (pEndUserInfoPtr - pURL);

            pURL = pURL + (userinfoLen + 1U);
            urlLength -= userinfoLen + 1U;
        }

        if ('[' == *pURL)
        {
            /* IP addresses are enclosed in square brackets. */

            pURL++;
            urlLength--;

            pHost = pURL;

            /* Search for ending square bracket. */

            while(('\0' != *pURL) && (urlLength > 0U) && (']' != *pURL))
            {
                hostLen++;
                pURL++;
                urlLength--;
            }

            /* If no end is found or the IP address is too long, error. */

            if ((']' != *pURL) || (hostLen >= sizeof(ipAddrBuf)))
            {
                WDRV_DBG_ERROR_PRINT("URL: IP address not terminated\r\n");
                WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
                return WDRV_WINC_STATUS_INVALID_ARG;
            }

            pURL++;
            urlLength--;

            /* Decode the IP address. */

            if (hostLen > 0U)
            {
                (void)memcpy(ipAddrBuf, pHost, hostLen);
                ipAddrBuf[hostLen] = '\0';

                if (true == WDRV_WINC_UtilsStringToIPv6Address(ipAddrBuf, &ipAddr.v6))
                {
                    ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_IPV6;
                }
                else
                {
                    WDRV_DBG_ERROR_PRINT("URL: Unknown IP address format\r\n");
                    WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
                    return WDRV_WINC_STATUS_INVALID_ARG;
                }
            }
        }
        else
        {
            pHost = pURL;

            /* Extract the host component, terminated by the send of the string,
             or '/' for path start or ':' for port start. */

            while(('\0' != *pURL) && (urlLength > 0U) && ('/' != *pURL) && (':' != *pURL))
            {
                hostLen++;
                pURL++;
                urlLength--;
            }

            if ((hostLen > 0U) && (hostLen <= 15U))
            {
                (void)memcpy(ipAddrBuf, pHost, hostLen);
                ipAddrBuf[hostLen] = '\0';

                if (true == WDRV_WINC_UtilsStringToIPAddress(ipAddrBuf, &ipAddr.v4))
                {
                    ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_IPV4;
                }
            }
        }

        if (':' == *pURL)
        {
            char *pEndPtr;

            /* If port component is present, extract it. */

            pURL++;
            urlLength--;

            errno = 0;
            authorityPort = (uint16_t)strtoul(pURL, &pEndPtr, 10);

            if (0 != errno)
            {
                WDRV_DBG_ERROR_PRINT("URL: Invalid port number\r\n");
                WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
                return WDRV_WINC_STATUS_INVALID_ARG;
            }

            urlLength -= (pEndPtr-pURL);
            pURL = pEndPtr;
        }
    }

    pPath   = pURL;
    pathLen = 0;

    /* Extract the path component, terminated by end of string or '?' for queries
     or '#' for fragment. */

    while(('\0' != *pURL) && (urlLength > 0U) && ('?' != *pURL) && ('#' != *pURL))
    {
        pURL++;
        pathLen++;
        urlLength--;
    }

    WDRV_DBG_INFORM_PRINT("Scheme: %.*s\r\n", schemeLen, pScheme);

    /* Verify supported schemes, also set port if not already specified. */

    if (0 == strncmp(pScheme, "https", 5))
    {
        if (0U == authorityPort)
        {
            authorityPort = 443;
        }
    }
    else if (0 == strncmp(pScheme, "http", 4))
    {
        if (0U == authorityPort)
        {
            authorityPort = 80;
        }
    }
    else
    {
        WDRV_DBG_ERROR_PRINT("URL: Scheme not supported\r\n");
        WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((userinfoLen > 0U) || (hostLen > 0U))
    {
        /* Authority is being updated. */

        if (userinfoLen > 0U)
        {
            (void)pUserinfo; /* Suppress unused warning if debugging turned off. */
            WDRV_DBG_INFORM_PRINT("Auth  : %.*s\r\n", userinfoLen, pUserinfo);
        }

        if (hostLen > 0U)
        {
            WDRV_DBG_INFORM_PRINT("Host  : %.*s : %d\r\n", hostLen, pHost, authorityPort);

            (void)WINC_CmdOTAC(cmdReqHandle, WINC_CFG_PARAM_ID_OTA_HOST, WINC_TYPE_STRING, (uintptr_t)pHost, hostLen);

            if (WDRV_WINC_IP_ADDRESS_TYPE_ANY != ipAddrType)
            {
                WDRV_DBG_INFORM_PRINT("IP    : %d\r\n", ipAddrType);
            }
        }
    }

    (void)WINC_CmdOTAC(cmdReqHandle, WINC_CFG_PARAM_ID_OTA_PORT, WINC_TYPE_INTEGER_UNSIGNED, authorityPort, 0);

    if (pathLen > 0U)
    {
        /* Path and/or file is being updated. */

        const char *pFinalPath;
        size_t finalPathLen;

        /* Extract final element of path so we have path/file. */

        pFinalPath = strrchr(pPath, (int)'/');

        if (NULL == pFinalPath)
        {
            pFinalPath   = pPath;
            finalPathLen = pathLen;
            pathLen      = 0;
        }
        else
        {
            pFinalPath++;
            finalPathLen = pathLen - (pFinalPath - pPath);
            pathLen     -= finalPathLen;
        }

        if (pathLen > 0U)
        {
            WDRV_DBG_INFORM_PRINT("Path  : %.*s\r\n", pathLen, pPath);
            (void)WINC_CmdOTAC(cmdReqHandle, WINC_CFG_PARAM_ID_OTA_PATH, WINC_TYPE_STRING, (uintptr_t)pPath, pathLen);
        }

        if (finalPathLen > 0U)
        {
            WDRV_DBG_INFORM_PRINT("File  : %.*s\r\n", finalPathLen, pFinalPath);
            (void)WINC_CmdOTAC(cmdReqHandle, WINC_CFG_PARAM_ID_OTA_FILE, WINC_TYPE_STRING, (uintptr_t)pFinalPath, finalPathLen);
        }
    }

    (void)WINC_CmdOTAC(cmdReqHandle, WINC_CFG_PARAM_ID_OTA_TLS_CONF, WINC_TYPE_INTEGER, tlsIdx, 0);
    (void)WINC_CmdOTADL(cmdReqHandle, WINC_CONST_OTA_STATE_ENABLE);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Set in progress flag and callback. */
    pDcpt->pCtrl->otaState.operation           = WDRV_WINC_OTA_OPERATION_DOWNLOAD_VERIFY;
    pDcpt->pCtrl->otaState.opId                = 0;
    pDcpt->pCtrl->otaState.pfOperationStatusCB = pfUpdateStatusCB;

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_ota.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAImageVerify
(
    DRV_HANDLE handle,
    const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;

    /* Ensure the driver is open and no OTA operation is in progress. */
    status = otaInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, otaCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdOTAVFY(cmdReqHandle);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Set in progress flag and callback. */
    pDcpt->pCtrl->otaState.operation           = WDRV_WINC_OTA_OPERATION_VERIFY;
    pDcpt->pCtrl->otaState.opId                = 0;
    pDcpt->pCtrl->otaState.pfOperationStatusCB = pfUpdateStatusCB;

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_ota.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAImageActivate
(
    DRV_HANDLE handle,
    const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;

    /* Ensure the driver is open and no OTA operation is in progress. */
    status = otaInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, otaCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdOTAACT(cmdReqHandle);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Set in progress flag and callback. */
    pDcpt->pCtrl->otaState.operation           = WDRV_WINC_OTA_OPERATION_ACTIVATE;
    pDcpt->pCtrl->otaState.opId                = 0;
    pDcpt->pCtrl->otaState.pfOperationStatusCB = pfUpdateStatusCB;

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_ota.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_OTAImageInvalidate
(
    DRV_HANDLE handle,
    const WDRV_WINC_OTA_STATUS_CALLBACK pfUpdateStatusCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;

    /* Ensure the driver is open and no OTA operation is in progress. */
    status = otaInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, otaCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdOTAINV(cmdReqHandle);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Set in progress flag and callback. */
    pDcpt->pCtrl->otaState.operation           = WDRV_WINC_OTA_OPERATION_INVALIDATE;
    pDcpt->pCtrl->otaState.opId                = 0;
    pDcpt->pCtrl->otaState.pfOperationStatusCB = pfUpdateStatusCB;

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_ota.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_OTAOptionsSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_OTA_OPTIONS *const pOTAOptions
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and options pointer are valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pOTAOptions))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, otaCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdOTAC(cmdReqHandle, WINC_CFG_PARAM_ID_OTA_TIMEOUT, WINC_TYPE_INTEGER_UNSIGNED, pOTAOptions->timeout, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_OTA */
