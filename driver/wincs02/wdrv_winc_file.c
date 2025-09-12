/*******************************************************************************
  WINC Wireless Driver File Operation Source File

  File Name:
    wdrv_winc_file.c

  Summary:
    WINC wireless driver file operation implementation.

  Description:
    This interface manages the file operations.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_file.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver File Operation Local Data
// *****************************************************************************
// *****************************************************************************

static bool fileLoadBuffer(WDRV_WINC_DCPT *pDcpt, WDRV_WINC_FILE_CTX *pFileCtx, bool forceFlushClose);
static void fileCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver File Operation Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_FILE_CTX *fileFindTsfrHandle
    (
        WDRV_WINC_FILE_CTX *pFileCtxList,
        uint16_t tsrfHandle
    )

  Summary:
    Find a file transfer context using transfer handle.

  Description:
    Search all file transfer contexts for a matching handle.

  Precondition:
    WDRV_WINC_Initialize must have been called.

  Parameters:
    pFileCtxList - Pointer to first file transfer context.
    tsrfHandle   - Transfer handle to find.

  Returns:
    Pointer to file transfer context or NULL for error/not found.

  Remarks:
    None.

*/

static WDRV_WINC_FILE_CTX *fileFindTsfrHandle
(
    WDRV_WINC_FILE_CTX *pFileCtxList,
    uint16_t tsrfHandle
)
{
    unsigned int i;

    if (NULL == pFileCtxList)
    {
        return NULL;
    }

    for (i=0; i<WDRV_WINC_FILE_CTX_NUM; i++)
    {
        if (true == pFileCtxList->inUse)
        {
            if (tsrfHandle == pFileCtxList->tsfrHandle)
            {
                return pFileCtxList;
            }
        }

        pFileCtxList++;
    }

    return NULL;
}

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_FILE_CTX *fileFindTsfrTempLoadID
    (
        WDRV_WINC_FILE_CTX *pFileCtxList,
        uintptr_t tempLoadID
    )

  Summary:
    Find a file transfer context using a temporary load ID.

  Description:
    Search all file transfer contexts for a matching ID.

  Precondition:
    WDRV_WINC_Initialize must have been called.

  Parameters:
    pFileCtxList - Pointer to first file transfer context.
    tempLoadID   - Temporary load ID to find.

  Returns:
    Pointer to file transfer context or NULL for error/not found.

  Remarks:
    None.

*/

static WDRV_WINC_FILE_CTX *fileFindTsfrTempLoadID
(
    WDRV_WINC_FILE_CTX *pFileCtxList,
    uintptr_t tempLoadID
)
{
    unsigned int i;

    if (NULL == pFileCtxList)
    {
        return NULL;
    }

    for (i=0; i<WDRV_WINC_FILE_CTX_NUM; i++)
    {
        if (true == pFileCtxList->inUse)
        {
            if (tempLoadID == pFileCtxList->tempLoadID)
            {
                return pFileCtxList;
            }
        }

        pFileCtxList++;
    }

    return NULL;
}

//*******************************************************************************
/*
  Function:
    static void fileProcessStatus
    (
        WDRV_WINC_DCPT *pDcpt,
        uint16_t cmdID,
        WINC_CMD_REQ_HANDLE cmdReqHandle,
        const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
        uint16_t statusCode
    )

  Summary:
    Process command status responses.

  Description:
    Processes command status responses received via WINC_DEV_CMDREQ_EVENT_CMD_STATUS events.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    pDcpt        - Pointer to device descriptor.
    cmdID        - Command ID.
    cmdReqHandle - Command request handle.
    pSrcCmd      - Pointer to source command.
    statusCode   - Status code.

  Returns:
    None.

  Remarks:
    None.

*/

static void fileProcessStatus
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t cmdID,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    uint16_t statusCode
)
{
    WDRV_WINC_FILE_CTX *pFileCtx;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pSrcCmd))
    {
        return;
    }

    switch (cmdID)
    {
        case WINC_CMD_ID_FS:
        {
            uint16_t op;
            WDRV_WINC_FILE_STATUS_TYPE status = WDRV_WINC_FILE_STATUS_OK;
            WINC_DEV_PARAM_ELEM elems[10];

            /* Unpack original command parameters. */
            if (false == WINC_DevUnpackElements(pSrcCmd->numParams, pSrcCmd->pParams, elems))
            {
                break;
            }

            /* Extract the command operation and, if present, the file type. */
            (void)WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &op, sizeof(op));

            if (WINC_STATUS_OK != statusCode)
            {
                status = WDRV_WINC_FILE_STATUS_ERROR;
            }

            switch (op)
            {
                case WINC_CONST_FS_OP_LOAD:
                {
                    /* Find the transfer context associated with the command request, the
                     the transfer handle is not set yet so use the command request handle. */
                    pFileCtx = fileFindTsfrTempLoadID(pDcpt->pCtrl->fileCtx, cmdReqHandle);

                    if (NULL != pFileCtx)
                    {
                        if (WINC_STATUS_OK != statusCode)
                        {
                            /* Call user callback to indicate an error has occurred. */
                            if (NULL != pFileCtx->pfFileStatusCb)
                            {
                                pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, status);
                            }

                            /* Reset the transfer context to terminate state. */
                            (void)memset(pFileCtx, 0, sizeof(WDRV_WINC_FILE_CTX));
                        }
                        else
                        {
                            /* After this the command request will no longer exist,
                             clear the command request handle to avoid using it later. */
                            pFileCtx->tempLoadID = WINC_CMD_REQ_INVALID_HANDLE;
                        }
                    }
                    else
                    {
                        WDRV_DBG_ERROR_PRINT("File load failed, no context\r\n");
                    }

                    break;
                }

                case WINC_CONST_FS_OP_LIST:
                {
                    if (NULL != pDcpt->pCtrl->pfFileFindCB)
                    {
                        WDRV_WINC_FILE_FIND_CALLBACK pfFileFindCB = pDcpt->pCtrl->pfFileFindCB;

                        /* Call the user callback to indicate the result of the list
                         operation, clear the callback after as no more results expected. */

                        pDcpt->pCtrl->pfFileFindCB = NULL;

                        pfFileFindCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->fileOpUserCtx, WDRV_WINC_FILE_TYPE_INVALID, NULL, status);
                    }
                    break;
                }

                case WINC_CONST_FS_OP_DEL:
                {
                    if (NULL != pDcpt->pCtrl->pfFileDeleteCB)
                    {
                        WDRV_WINC_FILE_DELETE_CALLBACK pfFileDeleteCB = pDcpt->pCtrl->pfFileDeleteCB;

                        /* Call the user callback to indicate the result of the delete
                         operation, clear the callback after as no longer required. */

                        pDcpt->pCtrl->pfFileDeleteCB = NULL;

                        pfFileDeleteCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->fileOpUserCtx, WDRV_WINC_FILE_TYPE_INVALID, status);
                    }
                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("File CmdRspCB(Status) FSOP %d not handled\r\n", op);
                    break;
                }
            }

            break;
        }

        case WINC_CMD_ID_FSTSFR:
        {
            WDRV_WINC_FILE_STATUS_TYPE status = WDRV_WINC_FILE_STATUS_OK;
            WINC_DEV_PARAM_ELEM elems[10];
            uint16_t tsfrHandle;

            /* Unpack original command parameters. */
            if (false == WINC_DevUnpackElements(pSrcCmd->numParams, pSrcCmd->pParams, elems))
            {
                break;
            }

            /* Extract the transfer handle associated with the original command. */
            (void)WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &tsfrHandle, sizeof(tsfrHandle));

            /* Find transfer context associated with transfer handle. */
            pFileCtx = fileFindTsfrHandle(pDcpt->pCtrl->fileCtx, tsfrHandle);

            if (NULL == pFileCtx)
            {
                break;
            }

            if (WINC_STATUS_OK != statusCode)
            {
                status = WDRV_WINC_FILE_STATUS_ERROR;
            }

            if (NULL != pFileCtx->pfFileStatusCb)
            {
                /* Call user callback to indicate the result of the transfer
                 operation. */
                pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, status);
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

//*******************************************************************************
/*
  Function:
    static void fileProcessCmdRsp
    (
        WDRV_WINC_DCPT *pDcpt,
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

static void fileProcessCmdRsp
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t rspId,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    WDRV_WINC_FILE_CTX *pFileCtx;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems) || (NULL == pSrcCmd))
    {
        return;
    }

    switch (rspId)
    {
        case WINC_CMD_ID_FS:
        {
            uint16_t op;

            if (numElems < 2U)
            {
                break;
            }

            /* Extract the file operation from the response elements. */
            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &op, sizeof(op));

            switch (op)
            {
                case WINC_CONST_FS_OP_LOAD:
                case WINC_CONST_FS_OP_STORE:
                {
                    uint16_t tsfrHandle;

                    /* Extract the transfer handle from the response elements. */
                    (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &tsfrHandle, sizeof(tsfrHandle));

                    /* Find the transfer context associated with the transfer handle. */
                    pFileCtx = fileFindTsfrTempLoadID(pDcpt->pCtrl->fileCtx, cmdReqHandle);

                    if (NULL == pFileCtx)
                    {
                        break;
                    }

                    /* Check that no current transfer handle has been set. */
                    if (0U == pFileCtx->tsfrHandle)
                    {
                        /* Initialise the file transfer context. */
                        pFileCtx->inUse      = true;
                        pFileCtx->tsfrHandle = tsfrHandle;

                        if (NULL != pFileCtx->pfFileStatusCb)
                        {
                            /* Call user callback indicating an open operation. */
                            pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_OPEN);
                        }
                    }
                    else
                    {
                        WDRV_DBG_ERROR_PRINT("FS operation response received while transfer in progress\r\n");
                    }
                    break;
                }

                case WINC_CONST_FS_OP_LIST:
                {
                    if (numElems < 3U)
                    {
                        break;
                    }

                    if (NULL != pDcpt->pCtrl->pfFileFindCB)
                    {
                        uint16_t fileType;
                        char filename[WINC_CMD_PARAM_SZ_FS_FILENAME+1U];

                        /* Extract the file type and filename from response elements. */
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &fileType, sizeof(fileType));
                        (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_STRING,  filename, sizeof(filename));

                        /* Call user callback indicating a new file entry. */
                        pDcpt->pCtrl->pfFileFindCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->fileOpUserCtx, fileType, filename, WDRV_WINC_FILE_STATUS_OK);
                    }

                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("File CmdRspCB(Rsp) FSOP %d not handled\r\n", op);
                    break;
                }
            }
            break;
        }

        case WINC_CMD_ID_FSTSFR:
        {
            uint16_t blockNum;
            uint16_t remaining;
            uint16_t tsfrHandle;

            if (numElems < 3U)
            {
                break;
            }

            /* Extract the transfer handle from the response elements. */
            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &tsfrHandle, sizeof(tsfrHandle));

            /* Find the transfer context associated with the transfer handle. */
            pFileCtx = fileFindTsfrHandle(pDcpt->pCtrl->fileCtx, tsfrHandle);

            if ((NULL == pFileCtx) || (NULL == pFileCtx->pfFileStatusCb))
            {
                break;
            }

            /* Extract the block number and bytes remaining from the response elements. */
            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &blockNum, sizeof(blockNum));
            (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER, &remaining, sizeof(remaining));

            if (WDRV_WINC_FILE_MODE_WRITE == pFileCtx->mode)
            {
                pFileCtx->op.load.lenRemaining = remaining;

                /* Check that the received block number is the one expected. */
                if (blockNum == pFileCtx->op.load.blockNum)
                {
                    /* Block has been ACK'd, remove the buffered data for refill. */
                    pFileCtx->op.load.bufLen = 0;

                    /* Refill internal buffer if pending data exists. */
                    (void)fileLoadBuffer(pDcpt, pFileCtx, false);

                    if ((NULL == pFileCtx->op.load.pData) && (blockNum == pFileCtx->op.load.blockNum))
                    {
                        /* The supplied user buffer has been consumed.
                           and no more blocks have been sent.
                         Call the user callback to indicate the supplied
                         buffer is complete and new data can be written if
                         required. */
                        pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_WRITE_COMPLETE);

                        /* In case callback closes file, check inUse flag. */
                        if (false == pFileCtx->inUse)
                        {
                            break;
                        }
                    }

                    if (0U == remaining)
                    {
                        /* End of the request file transfer.
                         Call the user callback to indicate the transfer
                         has been closed. */
                        pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_CLOSE);

                        (void)memset(pFileCtx, 0, sizeof(WDRV_WINC_FILE_CTX));
                    }
                }
            }
            else if (WDRV_WINC_FILE_MODE_READ == pFileCtx->mode)
            {
                WINC_CMD_REQ_HANDLE lCmdReqHandle;
                uint16_t dataLength;

                if (0U == pFileCtx->fileSize)
                {
                    /* Use first FSTSFR report (which has no data) remaining value
                     as length of file being received. */
                    pFileCtx->fileSize = remaining;
                }

                /* Update remaining count. */
                pFileCtx->op.store.lenRemaining = remaining;

                if (4U == numElems)
                {
                    size_t sizeRead;

                    /* If data payload is present, read it and update store
                     buffer pointers and lengths. */

                    sizeRead = WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_BYTE_ARRAY, pFileCtx->op.store.pData, pFileCtx->op.store.bufLen);

                    pFileCtx->op.store.pData  += sizeRead;
                    pFileCtx->op.store.bufLen -= sizeRead;

                    if (NULL != pFileCtx->op.store.pLenData)
                    {
                        /* If user request length updates, update them. */
                        *pFileCtx->op.store.pLenData += sizeRead;
                    }
                }

                if (NULL != pFileCtx->op.store.pData)
                {
                    /* If device signal no more data, or we run out of buffer
                     space, signal user that read has completed. */
                    if ((0U == remaining) || (0U == pFileCtx->op.store.bufLen))
                    {
                        pFileCtx->op.store.pData  = NULL;
                        pFileCtx->op.store.bufLen = 0;

                        pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_READ_COMPLETE);

                        /* If file closed in callback or no new read was started
                         stop here. */
                        if ((false == pFileCtx->inUse) || (NULL != pFileCtx->op.store.pData))
                        {
                            break;
                        }
                    }
                }

                if (0U == remaining)
                {
                    /* End of the request file transfer. Call the user callback
                     to indicate the transfer has been closed. */
                    pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_CLOSE);

                    (void)memset(pFileCtx, 0, sizeof(WDRV_WINC_FILE_CTX));

                    break;
                }

                /* Start with remaining file length, but limit it to the allocated
                 buffer size provided. */
                dataLength = remaining;

                if (dataLength > pFileCtx->op.store.bufLen)
                {
                    dataLength = pFileCtx->op.store.bufLen;
                }

                if (dataLength > 0)
                {
                    /* There is more data to receive, send another TSTSFR (FSRECV)
                     to fetch the next block of data. */

                    lCmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

                    if (WINC_CMD_REQ_INVALID_HANDLE == lCmdReqHandle)
                    {
                        break;
                    }

                    (void)WINC_CmdFSRECV(lCmdReqHandle, pFileCtx->tsfrHandle, ++pFileCtx->op.store.blockNum, dataLength);

                    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, lCmdReqHandle))
                    {
                        break;
                    }
                }
            }
            else
            {
                /* Do nothing. */
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

//*******************************************************************************
/*
  Function:
    static void fileCmdRspCallbackHandler
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

static void fileCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)context;

    if (NULL == pDcpt)
    {
        return;
    }

//    WDRV_DBG_INFORM_PRINT("FILE CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
                fileProcessStatus(pDcpt, pStatusInfo->rspCmdId, cmdReqHandle, &pStatusInfo->srcCmd, pStatusInfo->status);
            }

            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                fileProcessCmdRsp(pDcpt, pRspElems->rspId, cmdReqHandle, &pRspElems->srcCmd, pRspElems->numElems, pRspElems->elems);
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

//*******************************************************************************
/*
  Function:
    static bool fileLoadBuffer
    (
        WDRV_WINC_DCPT *pDcpt,
        WDRV_WINC_FILE_CTX *pFileCtx,
        bool forceFlushClose
    )

  Summary:
    Load internal buffer and flush to WINC device.

  Description:
    Data from a user supplied buffer is copied to a internal buffer, when
    full this is flushed to the WINC via AT+FSTSFR,

  Precondition:
    WDRV_WINC_Initialize must have been called.

  Parameters:
    pDcpt           - Pointer to WINC device descriptor.
    pFileCtx        - Pointer to file transfer context.
    forceFlushClose - Flag indicate if an immediate flush should be performed.

  Returns:
    true or false indicating success or error.

  Remarks:

*/

static bool fileLoadBuffer
(
    WDRV_WINC_DCPT *pDcpt,
    WDRV_WINC_FILE_CTX *pFileCtx,
    bool forceFlushClose
)
{
    if ((NULL == pDcpt) || (NULL == pFileCtx) || (NULL == pDcpt->pCtrl))
    {
        return false;
    }

    if (NULL != pFileCtx->op.load.pData)
    {
        /* Check if there is a user supplied buffer provided by WDRV_WINC_FileWrite
         and there is space available in the internal buffer. */
        if (pFileCtx->op.load.bufLen < WDRV_WINC_FILE_LOAD_BUF_SZ)
        {
            uint8_t bufFreeSpace = WDRV_WINC_FILE_LOAD_BUF_SZ - pFileCtx->op.load.bufLen;

            /* Calculate free space vs length of new data. */
            if (bufFreeSpace > pFileCtx->op.load.lenData)
            {
                bufFreeSpace = (uint8_t)pFileCtx->op.load.lenData;
            }

            /* Copy data into the internal buffer. */
            (void)memcpy(&pFileCtx->op.load.buffer[pFileCtx->op.load.bufLen], pFileCtx->op.load.pData, bufFreeSpace);

            pFileCtx->op.load.pData   += bufFreeSpace;
            pFileCtx->op.load.lenData -= bufFreeSpace;
            pFileCtx->op.load.bufLen  += bufFreeSpace;

            /* If all the user input data has been consumed, release the user buffer. */
            if (0U == pFileCtx->op.load.lenData)
            {
                pFileCtx->op.load.pData = NULL;
            }
        }
    }

    /* If all the data has been loaded into the buffer that can be, force a flush. */
    if (pFileCtx->op.load.bufLen == pFileCtx->op.load.lenRemaining)
    {
        forceFlushClose = true;
    }

    /* Check if the internal buffer is full, or partially full with an override to flush. */
    if ((WDRV_WINC_FILE_LOAD_BUF_SZ == pFileCtx->op.load.bufLen) || ((true == forceFlushClose) && (pFileCtx->op.load.bufLen > 0U)))
    {
        WINC_CMD_REQ_HANDLE cmdReqHandle;

        cmdReqHandle = WDRV_WINC_CmdReqInit(1, pFileCtx->op.load.bufLen, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

        if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
        {
            return false;
        }

        (void)WINC_CmdFSTSFR(cmdReqHandle, pFileCtx->tsfrHandle, ++pFileCtx->op.load.blockNum, pFileCtx->op.load.buffer, pFileCtx->op.load.bufLen, WINC_CMDFSTSFR_CRC_IGNORE_VAL);

        if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
        {
            return false;
        }
    }

    return true;
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver File Operation Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_FILE_HANDLE WDRV_WINC_FileOpen
    (
        DRV_HANDLE handle,
        const char *pFilename,
        WDRV_WINC_FILE_TYPE type,
        WDRV_WINC_FILE_MODE_TYPE mode,
        size_t lenFile,
        WDRV_WINC_FILE_STATUS_CALLBACK pfStatusCb,
        uintptr_t statusCbCtx
    )

  Summary:
    Open a file transfer.

  Description:
    Open a file transfer between user application and WINC device.

  Remarks:
    See wdrv_winc_file.h for usage information.

*/

WDRV_WINC_FILE_HANDLE WDRV_WINC_FileOpen
(
    DRV_HANDLE handle,
    const char *pFilename,
    WDRV_WINC_FILE_TYPE type,
    WDRV_WINC_FILE_MODE_TYPE mode,
    size_t lenFile,
    WDRV_WINC_FILE_STATUS_CALLBACK pfStatusCb,
    uintptr_t statusCbCtx
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    unsigned int i;
    size_t lenFilename;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pFilename))
    {
        return WDRV_WINC_FILE_INVALID_HANDLE;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_FILE_INVALID_HANDLE;
    }

    if ((WDRV_WINC_FILE_MODE_WRITE != mode) && (WDRV_WINC_FILE_MODE_READ != mode))
    {
        return WDRV_WINC_FILE_INVALID_HANDLE;
    }

    lenFilename = strnlen(pFilename, WINC_CMD_PARAM_SZ_FS_FILENAME+1U);

    if (lenFilename > WINC_CMD_PARAM_SZ_FS_FILENAME)
    {
        return WDRV_WINC_FILE_INVALID_HANDLE;
    }

    for (i=0; i<WDRV_WINC_FILE_CTX_NUM; i++)
    {
        if (false == pDcpt->pCtrl->fileCtx[i].inUse)
        {
            WINC_CMD_REQ_HANDLE cmdReqHandle;

            cmdReqHandle = WDRV_WINC_CmdReqInit(1, lenFilename, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

            if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
            {
                return WDRV_WINC_FILE_INVALID_HANDLE;
            }

            if (WDRV_WINC_FILE_MODE_WRITE == mode)
            {
                (void)WINC_CmdFSLOAD(cmdReqHandle, (uint8_t)type, (const uint8_t*)pFilename, lenFilename, (uint16_t)lenFile);
            }
            else
            {
                (void)WINC_CmdFSSTORE(cmdReqHandle, (uint8_t)type, (const uint8_t*)pFilename, lenFilename);
                lenFile = 0;
            }

            if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
            {
                return WDRV_WINC_FILE_INVALID_HANDLE;
            }

            pDcpt->pCtrl->fileCtx[i].inUse           = true;
            pDcpt->pCtrl->fileCtx[i].close           = false;
            pDcpt->pCtrl->fileCtx[i].type            = type;
            pDcpt->pCtrl->fileCtx[i].tsfrHandle      = 0;
            pDcpt->pCtrl->fileCtx[i].tempLoadID      = cmdReqHandle;
            pDcpt->pCtrl->fileCtx[i].pfFileStatusCb  = pfStatusCb;
            pDcpt->pCtrl->fileCtx[i].fileStatusCbCtx = statusCbCtx;
            pDcpt->pCtrl->fileCtx[i].fileSize        = lenFile;

            (void)memset(&pDcpt->pCtrl->fileCtx[i].op, 0, sizeof(pDcpt->pCtrl->fileCtx[i].op));

            return (uintptr_t)&pDcpt->pCtrl->fileCtx[i];
        }
    }

    return WDRV_WINC_FILE_INVALID_HANDLE;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_FileClose
    (
        DRV_HANDLE handle,
        WDRV_WINC_FILE_HANDLE fHandle
    )

  Summary:
    Close a file transfer.

  Description:
    Close a file transfer between user application and WINC device.

  Remarks:
    See wdrv_winc_file.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_FileClose
(
    DRV_HANDLE handle,
    WDRV_WINC_FILE_HANDLE fHandle
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WDRV_WINC_FILE_CTX *pFileCtx = (WDRV_WINC_FILE_CTX*)fHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the file handle is valid. */
    if ((WDRV_WINC_FILE_INVALID_HANDLE == fHandle) || (NULL == pFileCtx) || (false == pFileCtx->inUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (WDRV_WINC_FILE_MODE_WRITE == pFileCtx->mode)
    {
        if (false == fileLoadBuffer(pDcpt, pFileCtx, true))
        {
            return WDRV_WINC_STATUS_REQUEST_ERROR;
        }

        if ((NULL != pFileCtx->op.load.pData) || (WDRV_WINC_FILE_LOAD_BUF_SZ == pFileCtx->op.load.bufLen))
        {
            return WDRV_WINC_STATUS_BUSY;
        }

        if ((0U == pFileCtx->op.load.bufLen) && (0U == pFileCtx->op.load.lenData))
        {
            if (NULL != pFileCtx->pfFileStatusCb)
            {
                pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_CLOSE);
            }

            (void)memset(pFileCtx, 0, sizeof(WDRV_WINC_FILE_CTX));
        }
        else
        {
            pFileCtx->close = true;
        }
    }
    else if (WDRV_WINC_FILE_MODE_READ == pFileCtx->mode)
    {
        if (pFileCtx->op.store.lenRemaining > 0U)
        {
            /* If file transfer is not complete, send a FSTSFR (FSRECV) to close it. */

            WINC_CMD_REQ_HANDLE cmdReqHandle;

            cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

            if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
            {
                return WDRV_WINC_STATUS_REQUEST_ERROR;
            }

            (void)WINC_CmdFSRECV(cmdReqHandle, pFileCtx->tsfrHandle, 0, WINC_CMDFSRECV_DATA_LENGTH_IGNORE_VAL);

            if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
            {
                return WDRV_WINC_STATUS_REQUEST_ERROR;
            }

            return WDRV_WINC_STATUS_BUSY;
        }
        else
        {
            if (NULL != pFileCtx->pfFileStatusCb)
            {
                pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_CLOSE);
            }

            (void)memset(pFileCtx, 0, sizeof(WDRV_WINC_FILE_CTX));
        }
    }
    else
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_FileWrite
    (
        DRV_HANDLE handle,
        WDRV_WINC_FILE_HANDLE fHandle,
        const uint8_t *pData,
        size_t lenData
    )

  Summary:
    Write data to a file.

  Description:
    Write data to a file from user application to WINC device.

  Remarks:
    See wdrv_winc_file.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_FileWrite
(
    DRV_HANDLE handle,
    WDRV_WINC_FILE_HANDLE fHandle,
    const uint8_t *pData,
    size_t lenData
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WDRV_WINC_FILE_CTX *pFileCtx = (WDRV_WINC_FILE_CTX*)fHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the file handle is valid. */
    if ((WDRV_WINC_FILE_INVALID_HANDLE == fHandle) || (NULL == pFileCtx) || (false == pFileCtx->inUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((NULL == pData) || (0U == lenData))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((NULL != pFileCtx->op.load.pData) || (WDRV_WINC_FILE_LOAD_BUF_SZ == pFileCtx->op.load.bufLen))
    {
        return WDRV_WINC_STATUS_BUSY;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pFileCtx->mode            = WDRV_WINC_FILE_MODE_WRITE;
    pFileCtx->op.load.pData   = pData;
    pFileCtx->op.load.lenData = lenData;

    if (false == fileLoadBuffer(pDcpt, pFileCtx, false))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_FileRead
    (
        DRV_HANDLE handle,
        WDRV_WINC_FILE_HANDLE fHandle,
        uint8_t *pBuffer,
        size_t lenBuffer,
        size_t *pLenData
    )

  Summary:
    Read data from a file.

  Description:
    Read data from a file from WINC device to user application.

  Remarks:
    See wdrv_winc_file.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_FileRead
(
    DRV_HANDLE handle,
    WDRV_WINC_FILE_HANDLE fHandle,
    uint8_t *pBuffer,
    size_t lenBuffer,
    size_t *pLenData
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WDRV_WINC_FILE_CTX *pFileCtx = (WDRV_WINC_FILE_CTX*)fHandle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the file handle is valid. */
    if ((WDRV_WINC_FILE_INVALID_HANDLE == fHandle) || (NULL == pFileCtx) || (false == pFileCtx->inUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((NULL == pBuffer) || (0U == lenBuffer))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (NULL != pFileCtx->op.store.pData)
    {
        return WDRV_WINC_STATUS_BUSY;
    }

    if ((0U == pFileCtx->op.store.lenRemaining) && (pFileCtx->op.store.blockNum > 0))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pFileCtx->mode              = WDRV_WINC_FILE_MODE_READ;
    pFileCtx->op.store.pData    = pBuffer;
    pFileCtx->op.store.bufLen   = lenBuffer;
    pFileCtx->op.store.pLenData = pLenData;

    if (NULL != pLenData)
    {
        *pLenData = 0;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return false;
    }

    if (0 == pFileCtx->op.store.blockNum)
    {
        /* First read will cause a FSTSFR request to get a report of the file length. */
        (void)WINC_CmdFSTSFR(cmdReqHandle, pFileCtx->tsfrHandle, WINC_CMDFSTSFR_BLOCK_NUM_IGNORE_VAL, NULL, 0, WINC_CMDFSTSFR_CRC_IGNORE_VAL);
    }
    else
    {
        /* Subsequent reads will cause a FSTSFR (FSRECV). */
        (void)WINC_CmdFSRECV(cmdReqHandle, pFileCtx->tsfrHandle, ++pFileCtx->op.store.blockNum, lenBuffer);
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return false;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
    WDRV_WINC_STATUS WDRV_WINC_FileFind
    (
        DRV_HANDLE handle,
        WDRV_WINC_FILE_TYPE type,
        WDRV_WINC_FILE_FIND_CALLBACK pfFindCb,
        uintptr_t findCbCtx
    )

  Summary:
    Find a type of file.

  Description:
    Requests the WINC device send a list of a particular file type.

  Remarks:
    See wdrv_winc_file.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_FileFind
(
    DRV_HANDLE handle,
    WDRV_WINC_FILE_TYPE type,
    WDRV_WINC_FILE_FIND_CALLBACK pfFindCb,
    uintptr_t findCbCtx
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (NULL != pDcpt->pCtrl->pfFileFindCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdFSOP(cmdReqHandle, WINC_CONST_FS_OP_LIST, (uint8_t)type, NULL, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfFileFindCB  = pfFindCb;
    pDcpt->pCtrl->fileOpUserCtx = findCbCtx;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_FileDelete
    (
        DRV_HANDLE handle,
        const char *pFilename,
        WDRV_WINC_FILE_TYPE type,
        WDRV_WINC_FILE_DELETE_CALLBACK pfDeleteCb,
        uintptr_t deleteCbCtx
    )

  Summary:
    Delete a file.

  Description:
    Requests the WINC device deletes a file of a particular name and type.

  Remarks:
    See wdrv_winc_file.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_FileDelete
(
    DRV_HANDLE handle,
    const char *pFilename,
    WDRV_WINC_FILE_TYPE type,
    WDRV_WINC_FILE_DELETE_CALLBACK pfDeleteCb,
    uintptr_t deleteCbCtx
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pFilename))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (NULL != pDcpt->pCtrl->pfFileDeleteCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdFSOP(cmdReqHandle, WINC_CONST_FS_OP_DEL, (uint8_t)type, (const uint8_t*)pFilename, strlen(pFilename));

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfFileDeleteCB = pfDeleteCb;
    pDcpt->pCtrl->fileOpUserCtx  = deleteCbCtx;

    return WDRV_WINC_STATUS_OK;
}
