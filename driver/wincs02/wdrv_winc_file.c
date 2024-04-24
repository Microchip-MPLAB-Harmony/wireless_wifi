/*******************************************************************************
  WINC Driver file operation Implementation

  File Name:
    wdrv_winc_file.c

  Summary:
    WINC wireless driver file operation implementation.

  Description:
    This interface manages the file operations.
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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_file.h"

static bool fileLoadBuffer(WDRV_WINC_DCPT *pDcpt, WDRV_WINC_FILE_CTX *pFileCtx, bool forceFlushClose);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver File Operation Implementation
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
    WDRV_WINC_Initialize should have been called.

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
    int i;

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
    static WDRV_WINC_FILE_CTX *fileFindTsfrCmdReq
    (
        WDRV_WINC_FILE_CTX *pFileCtxList,
        WINC_CMD_REQ_HANDLE cmdReqHandle
    )

  Summary:
    Find a file transfer context using command request handle.

  Description:
    Search all file transfer contexts for a matching handle.

  Precondition:
    WDRV_WINC_Initialize should have been called.

  Parameters:
    pFileCtxList - Pointer to first file transfer context.
    cmdReqHandle - Command request handle to find.

  Returns:
    Pointer to file transfer context or NULL for error/not found.

  Remarks:
    None.

*/

static WDRV_WINC_FILE_CTX *fileFindTsfrCmdReq
(
    WDRV_WINC_FILE_CTX *pFileCtxList,
    WINC_CMD_REQ_HANDLE cmdReqHandle
)
{
    int i;

    if (NULL == pFileCtxList)
    {
        return NULL;
    }

    for (i=0; i<WDRV_WINC_FILE_CTX_NUM; i++)
    {
        if (true == pFileCtxList->inUse)
        {
            if (cmdReqHandle == pFileCtxList->cmdReqHandle)
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
    WDRV_WINC_Initialize should have been called.
    WINC_DevTransmitCmdReq must of been called to submit command request.

  Parameters:
    context      - Context provided to WINC_CmdReqInit for callback.
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
    WDRV_WINC_FILE_CTX *pFileCtx;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return;
    }

    //WDRV_DBG_INFORM_PRINT("File CmdRspCB %08x Event %d\n", cmdReqHandle, event);

    switch (event)
    {
        case WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
        {
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
        {
            OSAL_Free((void*)cmdReqHandle);
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
        {
            /* Status response received for command. */

            WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (NULL != pStatusInfo)
            {
                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_FS:
                    {
                        WINC_DEV_PARAM_ELEM elems[10];
                        uint16_t op;
                        uint16_t fileType = WDRV_WINC_FILE_TYPE_INVALID;
                        WDRV_WINC_FILE_STATUS_TYPE status = WDRV_WINC_FILE_STATUS_OK;

                        /* Unpack original command parameters. */
                        if (false == WINC_DevUnpackElements(pStatusInfo->srcCmd.numParams, pStatusInfo->srcCmd.pParams, elems))
                        {
                            break;
                        }

                        /* Extract the command operation and, if present, the file type. */
                        WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &op, sizeof(op));

                        if (pStatusInfo->srcCmd.numParams > 1)
                        {
                            WINC_CmdReadParamElem(&elems[1], WINC_TYPE_INTEGER, &fileType, sizeof(fileType));
                        }

                        if (WINC_STATUS_OK != pStatusInfo->status)
                        {
                            status = WDRV_WINC_FILE_STATUS_ERROR;
                        }

                        switch (op)
                        {
                            case WINC_CONST_FS_OP_LOAD:
                            {
                                /* Find the transfer context associated with the command request, the
                                 the transfer handle isn't set yet so use the command request handle. */
                                pFileCtx = fileFindTsfrCmdReq(pDcpt->pCtrl->fileCtx, cmdReqHandle);

                                if (NULL != pFileCtx)
                                {
                                    if (WINC_STATUS_OK != pStatusInfo->status)
                                    {
                                        /* Call user callback to indicate an error has occurred. */
                                        if (NULL != pFileCtx->pfFileStatusCb)
                                        {
                                            pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, status);
                                        }

                                        /* Reset the transfer context to terminate state. */
                                        memset(pFileCtx, 0, sizeof(WDRV_WINC_FILE_CTX));
                                    }
                                    else
                                    {
                                        /* After this the command request will no longer exist,
                                         clear the command request handle to avoid using it later. */
                                        pFileCtx->cmdReqHandle = WINC_CMD_REQ_INVALID_HANDLE;
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
                                    /* Call the user callback to indicate the result of the list
                                     operation, clear the callback after as no more results expected. */
                                    pDcpt->pCtrl->pfFileFindCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->fileOpUserCtx, fileType, NULL, status);
                                    pDcpt->pCtrl->pfFileFindCB = NULL;
                                }
                                break;
                            }

                            case WINC_CONST_FS_OP_DEL:
                            {
                                if (NULL != pDcpt->pCtrl->pfFileDeleteCB)
                                {
                                    /* Call the user callback to indicate the result of the delete
                                     operation, clear the callback after as no longer required. */
                                    pDcpt->pCtrl->pfFileDeleteCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->fileOpUserCtx, fileType, status);
                                    pDcpt->pCtrl->pfFileDeleteCB = NULL;
                                }
                                break;
                            }

                            default:
                            {
                                break;
                            }
                        }

                        break;
                    }

                    case WINC_CMD_ID_FSTSFR:
                    {
                        WINC_DEV_PARAM_ELEM elems[10];
                        uint16_t tsfrHandle;
                        WDRV_WINC_FILE_STATUS_TYPE status = WDRV_WINC_FILE_STATUS_OK;

                        /* Unpack original command parameters. */
                        if (false == WINC_DevUnpackElements(pStatusInfo->srcCmd.numParams, pStatusInfo->srcCmd.pParams, elems))
                        {
                            break;
                        }

                        /* Extract the transfer handle associated with the original command. */
                        WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &tsfrHandle, sizeof(tsfrHandle));

                        /* Find transfer context associated with transfer handle. */
                        pFileCtx = fileFindTsfrHandle(pDcpt->pCtrl->fileCtx, tsfrHandle);

                        if (NULL == pFileCtx)
                        {
                            break;
                        }

                        if (WINC_STATUS_OK != pStatusInfo->status)
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
                        break;
                    }
                }
            }
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                switch (pRspElems->rspId)
                {
                    case WINC_CMD_ID_FS:
                    {
                        uint16_t op;

                        if (pRspElems->numElems < 2)
                        {
                            break;
                        }

                        /* Extract the file operation from the response elements. */
                        WINC_CmdReadParamElem(&pRspElems->elems[0], WINC_TYPE_INTEGER, &op, sizeof(op));

                        switch (op)
                        {
                            case WINC_CONST_FS_OP_LOAD:
                            {
                                uint16_t tsfrHandle;

                                /* Extract the transfer handle from the response elements. */
                                WINC_CmdReadParamElem(&pRspElems->elems[1], WINC_TYPE_INTEGER, &tsfrHandle, sizeof(tsfrHandle));

                                /* Find the transfer context associated with the transfer handle. */
                                pFileCtx = fileFindTsfrCmdReq(pDcpt->pCtrl->fileCtx, cmdReqHandle);

                                if (NULL == pFileCtx)
                                {
                                    break;
                                }

                                /* Check that no current transfer handle has been set. */
                                if (0 == pFileCtx->tsfrHandle)
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
                                    WDRV_DBG_ERROR_PRINT("FS load response received while transfer in progress\r\n");
                                }
                                break;
                            }

                            case WINC_CONST_FS_OP_LIST:
                            {
                                uint16_t fileType;
                                char filename[32+1];

                                if (pRspElems->numElems < 3)
                                {
                                    break;
                                }

                                if (NULL != pDcpt->pCtrl->pfFileFindCB)
                                {
                                    /* Extract the file type and filename from response elements. */
                                    WINC_CmdReadParamElem(&pRspElems->elems[1], WINC_TYPE_INTEGER, &fileType, sizeof(fileType));
                                    WINC_CmdReadParamElem(&pRspElems->elems[2], WINC_TYPE_STRING,  filename, sizeof(filename));

                                    /* Call user callback indicating a new file entry. */
                                    pDcpt->pCtrl->pfFileFindCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->fileOpUserCtx, fileType, filename, WDRV_WINC_FILE_STATUS_OK);
                                }
                            }

                            default:
                            {
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

                        if (3 != pRspElems->numElems)
                        {
                            break;
                        }

                        /* Extract the transfer handle from the response elements. */
                        WINC_CmdReadParamElem(&pRspElems->elems[0], WINC_TYPE_INTEGER, &tsfrHandle, sizeof(tsfrHandle));

                        /* Find the transfer context associated with the transfer handle. */
                        pFileCtx = fileFindTsfrHandle(pDcpt->pCtrl->fileCtx, tsfrHandle);

                        if (NULL == pFileCtx)
                        {
                            break;
                        }

                        /* Extract the block number and bytes remaining from the response elements. */
                        WINC_CmdReadParamElem(&pRspElems->elems[1], WINC_TYPE_INTEGER, &blockNum, sizeof(blockNum));
                        WINC_CmdReadParamElem(&pRspElems->elems[2], WINC_TYPE_INTEGER, &remaining, sizeof(remaining));

                        /* Check that the received block number is the one expected. */
                        if (blockNum == pFileCtx->op.load.blockNum)
                        {
                            /* Block has been ACK'd, remove the buffered data for refill. */
                            pFileCtx->op.load.bufLen = 0;

                            /* Refill internal buffer if pending data exists. */
                            fileLoadBuffer(pDcpt, pFileCtx, false);

                            if (NULL == pFileCtx->op.load.pData)
                            {
                                /* The supplied user buffer has been consumed. */
                                if (NULL != pFileCtx->pfFileStatusCb)
                                {
                                    /* Call the user callback to indicate the supplied
                                     buffer is complete and new data can be written if
                                     required. */
                                    pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_WRITE_COMPLETE);
                                }
                            }

                            if (0 == remaining)
                            {
                                /* End of the request file transfer. */
                                if (NULL != pFileCtx->pfFileStatusCb)
                                {
                                    /* Call the user callback to indicate the transfer
                                     has been closed. */
                                    pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_CLOSE);
                                }
                            }
                        }

                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }

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
    WDRV_WINC_Initialize should have been called.

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
                bufFreeSpace = pFileCtx->op.load.lenData;
            }

            /* Copy data into the internal buffer. */
            memcpy(&pFileCtx->op.load.buffer[pFileCtx->op.load.bufLen], pFileCtx->op.load.pData, bufFreeSpace);

            pFileCtx->op.load.pData   += bufFreeSpace;
            pFileCtx->op.load.lenData -= bufFreeSpace;
            pFileCtx->op.load.bufLen  += bufFreeSpace;

            /* If all the user input data has been consumed, release the user buffer. */
            if (0 == pFileCtx->op.load.lenData)
            {
                pFileCtx->op.load.pData = NULL;
            }
        }
    }

    /* Check if the internal buffer is full, or partially full with an override to flush. */
    if ((WDRV_WINC_FILE_LOAD_BUF_SZ == pFileCtx->op.load.bufLen) || ((true == forceFlushClose) && (pFileCtx->op.load.bufLen > 0)))
    {
        WINC_CMD_REQ_HANDLE cmdReqHandle;
        void *pCmdReqBuffer;

        if ((NULL == pDcpt) || (NULL == pFileCtx) || (NULL == pDcpt->pCtrl))
        {
            return false;
        }

        pCmdReqBuffer = OSAL_Malloc(256);

        if (NULL == pCmdReqBuffer)
        {
            return false;
        }

        cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 1, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

        if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
        {
            OSAL_Free(pCmdReqBuffer);
            return false;
        }

        WINC_CmdFSTSFR(cmdReqHandle, pFileCtx->tsfrHandle, ++pFileCtx->op.load.blockNum, pFileCtx->op.load.buffer, pFileCtx->op.load.bufLen, -1);

        if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
        {
            OSAL_Free(pCmdReqBuffer);
            return false;
        }
    }

    return true;
}

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
    int i;
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

    if (WDRV_WINC_FILE_MODE_WRITE != mode)
    {
        return WDRV_WINC_FILE_INVALID_HANDLE;
    }

    lenFilename = strnlen(pFilename, 32+1);

    if (lenFilename > 32)
    {
        return WDRV_WINC_FILE_INVALID_HANDLE;
    }

    for (i=0; i<WDRV_WINC_FILE_CTX_NUM; i++)
    {
        if (false == pDcpt->pCtrl->fileCtx[i].inUse)
        {
            WINC_CMD_REQ_HANDLE cmdReqHandle;
            void *pCmdReqBuffer;

            pCmdReqBuffer = OSAL_Malloc(256);

            if (NULL == pCmdReqBuffer)
            {
                return WDRV_WINC_FILE_INVALID_HANDLE;
            }

            cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 1, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

            if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
            {
                OSAL_Free(pCmdReqBuffer);
                return WDRV_WINC_FILE_INVALID_HANDLE;
            }

            WINC_CmdFSLOAD(cmdReqHandle, type, WINC_CONST_FS_TSFRPROT_TSFR, (uint8_t*)pFilename, lenFilename, lenFile);

            if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
            {
                OSAL_Free(pCmdReqBuffer);
                return WDRV_WINC_FILE_INVALID_HANDLE;
            }

            pDcpt->pCtrl->fileCtx[i].inUse           = true;
            pDcpt->pCtrl->fileCtx[i].close           = false;
            pDcpt->pCtrl->fileCtx[i].type            = type;
            pDcpt->pCtrl->fileCtx[i].tsfrHandle      = 0;
            pDcpt->pCtrl->fileCtx[i].cmdReqHandle    = cmdReqHandle;
            pDcpt->pCtrl->fileCtx[i].pfFileStatusCb  = pfStatusCb;
            pDcpt->pCtrl->fileCtx[i].fileStatusCbCtx = statusCbCtx;
            pDcpt->pCtrl->fileCtx[i].fileSize        = lenFile;

            memset(&pDcpt->pCtrl->fileCtx[i].op, 0, sizeof(pDcpt->pCtrl->fileCtx[i].op));

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

    if ((NULL != pFileCtx->op.load.pData) || (WDRV_WINC_FILE_LOAD_BUF_SZ == pFileCtx->op.load.bufLen))
    {
        return WDRV_WINC_STATUS_BUSY;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (false == fileLoadBuffer(pDcpt, pFileCtx, true))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if ((0 == pFileCtx->op.load.bufLen) && (0 == pFileCtx->op.load.lenData))
    {
        if (NULL != pFileCtx->pfFileStatusCb)
        {
            pFileCtx->pfFileStatusCb((DRV_HANDLE)pDcpt, (WDRV_WINC_FILE_HANDLE)pFileCtx, pFileCtx->fileStatusCbCtx, WDRV_WINC_FILE_STATUS_CLOSE);
        }

        memset(pFileCtx, 0, sizeof(WDRV_WINC_FILE_CTX));
    }
    else
    {
        pFileCtx->close = true;
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

    if ((NULL == pData) || (0 == lenData))
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
    void *pCmdReqBuffer;

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

    pDcpt->pCtrl->pfFileFindCB  = pfFindCb;
    pDcpt->pCtrl->fileOpUserCtx = findCbCtx;

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdFSOP(cmdReqHandle, WINC_CONST_FS_OP_LIST, type, NULL, 0);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_FILE_HANDLE WDRV_WINC_FileDelete
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

WDRV_WINC_FILE_HANDLE WDRV_WINC_FileDelete
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
    void *pCmdReqBuffer;

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

    pDcpt->pCtrl->pfFileDeleteCB = pfDeleteCb;
    pDcpt->pCtrl->fileOpUserCtx  = deleteCbCtx;

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, fileCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdFSOP(cmdReqHandle, WINC_CONST_FS_OP_DEL, type, (uint8_t*)pFilename, strlen(pFilename));

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
