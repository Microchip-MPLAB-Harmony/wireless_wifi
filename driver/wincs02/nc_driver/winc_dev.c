/*
Copyright (C) 2023-24, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "winc_dev.h"
#include "winc_cmd_req.h"
#include "winc_debug.h"

#define WINC_DEV_NUM_AEC_CB_ENTRIES             15
#define WINC_DEV_NUM_MOD_REQS_COUNTERS          15

typedef enum
{
    WINC_DEV_EVENT_NONE         = 0x00,
    WINC_DEV_EVENT_TX_REQ       = 0x01,
    WINC_DEV_EVENT_RX_REQ       = 0x02
} WINC_DEV_EVENT_TYPE;

typedef struct
{
    uint8_t                         type;
    uint8_t                         number;
    uint16_t                        length;

    union
    {
        struct
        {
            WINC_CMD_REQ_HANDLE     cmdReqHandle;
            WINC_SEND_REQ_HDR_ELEM  *pSendReqHdr;
        } txReq;

        struct
        {
            uint32_t                *pMsgLengths;
            size_t                  receiveBufferOffset;
        } rxReq;
    };
} WINC_DEV_EVENT_CTX;

typedef struct
{
    WINC_DEV_AEC_RSP_CB             pfAecRspCallback;
    uintptr_t                       aecRspCallbackCtx;
} WINC_DEV_AEC_CB_ENTRY;

typedef struct
{
    bool                            isInit;
    bool                            busError;
    uint16_t                        nextSeqNum;
    uint8_t                         *pReceiveBuffer;
    size_t                          receiveBufferSize;
    WINC_CMD_REQ_HANDLE             cmdReqQueue;
    WINC_SEND_REQ_STATE             *pSendReqState;
    WINC_DEV_EVENT_CTX              eventCtx;
    WINC_DEV_AEC_CB_ENTRY           aecCallbackTable[WINC_DEV_NUM_AEC_CB_ENTRIES];
    struct
    {
        uint8_t                     modId;
        uint8_t                     count;
    } modReqCount[WINC_DEV_NUM_MOD_REQS_COUNTERS];
} WINC_DEV_CTRL_CTX;

static WINC_DEV_CTRL_CTX wincDevCtrlCtx = {.isInit = false};

WINC_DEBUG_PRINTF_FP pfWINCDevDebugPrintf = NULL;

static uint8_t devModReqCountUpdate(WINC_DEV_CTRL_CTX *pCtrlCtx, uint8_t modId, bool insert)
{
    int modIdx = 0;
    int firstEmptyIdx = -1;

    if ((NULL == pCtrlCtx) || (0 == modId))
    {
        return 0;
    }

    for (modIdx=0; modIdx<WINC_DEV_NUM_MOD_REQS_COUNTERS; modIdx++)
    {
        if (pCtrlCtx->modReqCount[modIdx].modId == modId)
        {
            break;
        }
        else if ((-1 == firstEmptyIdx) && (0 == pCtrlCtx->modReqCount[modIdx].modId))
        {
            firstEmptyIdx = modIdx;
        }
    }

    if (true == insert)
    {
        if (WINC_DEV_NUM_MOD_REQS_COUNTERS == modIdx)
        {
            if (-1 == firstEmptyIdx)
            {
                return 0;
            }

            modIdx = firstEmptyIdx;

            pCtrlCtx->modReqCount[modIdx].modId = modId;
            pCtrlCtx->modReqCount[modIdx].count = 0;
        }

        if (pCtrlCtx->modReqCount[modIdx].count < 255)
        {
            pCtrlCtx->modReqCount[modIdx].count++;
        }
    }
    else
    {
        if (WINC_DEV_NUM_MOD_REQS_COUNTERS == modIdx)
        {
            return 0;
        }

        if (pCtrlCtx->modReqCount[modIdx].count > 0)
        {
            pCtrlCtx->modReqCount[modIdx].count--;
        }

        if (0 == pCtrlCtx->modReqCount[modIdx].count)
        {
            pCtrlCtx->modReqCount[modIdx].modId = 0;
        }
    }
#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_VERBOSE)
    int i;

    WINC_VERBOSE_PRINT("MRC");
    for (i=0; i<WINC_DEV_NUM_MOD_REQS_COUNTERS; i++)
    {
        WINC_VERBOSE_PRINT("[%d:%d]", pCtrlCtx->modReqCount[i].modId, pCtrlCtx->modReqCount[i].count);
    }
    WINC_VERBOSE_PRINT("\n");
#endif
    return pCtrlCtx->modReqCount[modIdx].count;
}

/*****************************************************************************
  Description:
    Unpack elements from command responses and AECs.

  Parameters:
    pCmdRsp - Pointer to command response structure
    pElems  - Pointer to response elements structure to populate

  Returns:
    true or false indicating success or failure

  Remarks:
    Unpack information from the command response or AEC header and then
    unpack the element TLV header into a more convenient format.

 *****************************************************************************/

static bool devUnpackResponseElements(WINC_COMMAND_RESPONSE *pCmdRsp, WINC_DEV_EVENT_RSP_ELEMS *pElems)
{
    WINC_RSP_ELEMENTS *pRspElems;
    int i;

    if ((NULL == pCmdRsp) || (NULL == pElems))
    {
        return false;
    }

    pElems->rspId = WINC_FIELD_UNPACK_16(pCmdRsp->payload.rsp.rspId);

    pRspElems = (WINC_RSP_ELEMENTS*)pCmdRsp->payload.rsp.elements;

    pElems->numElems = pRspElems->numTlvs;

    if (WINC_COMMAND_MSG_TYPE_AEC == pCmdRsp->msgType)
    {
        WINC_TRACE_PRINT("AEC %04x Num=%d\n", pElems->rspId, pElems->numElems);
    }
    else
    {
        WINC_TRACE_PRINT("Rsp %04x (CMD(%d) %04x) Num=%d\n", pElems->rspId, pElems->srcCmd.idx, pElems->rspCmdId, pElems->numElems);
    }

    if (false == WINC_DevUnpackElements(pRspElems->numTlvs, pRspElems->tlvs, pElems->elems))
    {
        return false;
    }

    for (i=0; i<pRspElems->numTlvs; i++)
    {
        pElems->elems[i].pData[pElems->elems[i].length] = 0;
    }

    return true;
}

/*****************************************************************************
  Description:
    Prepare a pending command request transmission.

  Parameters:
    pCtrlCtx      - Pointer to the device control context
    pSendReqState - Pointer to the current command request

  Returns:
    true or false indicating success or failure

  Remarks:
    Begin the process of sending a command request, sequence numbers are assigned
    to each individual command request within the current burst.

 *****************************************************************************/

static bool devPreparePendingCmdReq(WINC_DEV_CTRL_CTX *pCtrlCtx, WINC_SEND_REQ_STATE *pSendReqState)
{
    WINC_SEND_REQ_HDR_ELEM *pReqHdr;
    WINC_COMMAND_REQUEST *pCmdReq;
    int i;

    if ((NULL == pCtrlCtx) || (NULL == pSendReqState))
    {
        return false;
    }

    WINC_VERBOSE_PRINT("free space was %d\n", pSendReqState->space);

    /* Close command request, zero the free space and reset the work pointer to the
     first request header. */

    pSendReqState->space     = 0;
    pSendReqState->pPtr      = pSendReqState->pFirstHdrElem->pPtr;
    pSendReqState->numErrors = 0;

    if (0 == pSendReqState->numCmds)
    {
        return false;
    }

    pReqHdr = pSendReqState->pFirstHdrElem;

    for (i=0; i<pSendReqState->numCmds; i++)
    {
        /* Advance to the next request header related to the command header. */

        while (0 == (pReqHdr->flags & WINC_FLAG_FIRST_IN_BURST))
        {
            pReqHdr++;
        }

        /* Assign the next sequence number to the command header. */

        pCmdReq = (WINC_COMMAND_REQUEST*)pReqHdr->pPtr;

        pCmdReq->seqNum_h  = pCtrlCtx->nextSeqNum >> 8;
        pCmdReq->seqNum_l  = pCtrlCtx->nextSeqNum & 0xff;

        WINC_VERBOSE_PRINT("Assign SN=%04x to %08x\n", pCtrlCtx->nextSeqNum, pCmdReq);

        pCtrlCtx->nextSeqNum++;

        devModReqCountUpdate(pCtrlCtx, pCmdReq->id_h, true);

        pReqHdr++;
    }

    return true;
}

/*****************************************************************************
  Description:
    Flush the pending command requests.

  Parameters:
    pCtrlCtx - Pointer to the device control context

  Returns:
    None.

  Remarks:
    None.

 *****************************************************************************/

static void devFlushPendingCmdReqQueue(WINC_DEV_CTRL_CTX *pCtrlCtx)
{
    WINC_CMD_REQ_HANDLE cmdReq;
    WINC_SEND_REQ_STATE *pSendReqState;
    WINC_CMD_REQ_HANDLE nextCmdReq;

    if (NULL == pCtrlCtx)
    {
        return;
    }

    cmdReq = pCtrlCtx->cmdReqQueue;

    while (WINC_CMD_REQ_INVALID_HANDLE != cmdReq)
    {
        pSendReqState = (WINC_SEND_REQ_STATE*)cmdReq;

        nextCmdReq = pSendReqState->nextCmdReq;

        if (NULL != pSendReqState->pfCmdRspCallback)
        {
            WINC_DEV_EVENT_COMPLETE_ARGS eventCompleteArgs;

            eventCompleteArgs.numCmds   = pSendReqState->numCmds;
            eventCompleteArgs.numErrors = pSendReqState->numCmds;

            pSendReqState->pfCmdRspCallback(pSendReqState->cmdRspCallbackCtx, (WINC_DEVICE_HANDLE)pCtrlCtx, (WINC_CMD_REQ_HANDLE)pSendReqState, WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE, (uintptr_t)&eventCompleteArgs);
        }

        WINC_VERBOSE_PRINT("CmdReq %08x flushed, moving to %08x\n", pSendReqState, nextCmdReq);

        cmdReq = nextCmdReq;
    }

    pCtrlCtx->cmdReqQueue   = WINC_CMD_REQ_INVALID_HANDLE;
    pCtrlCtx->pSendReqState = NULL;
}

/*****************************************************************************
  Description:
    Process a pending command request transmission.

  Parameters:
    pCtrlCtx      - Pointer to the device control context
    pSendReqState - Pointer to the current command request

  Returns:
    true or false indicating success or failure

  Remarks:
    The number and sizes of commands are sent to the device.

 *****************************************************************************/

static bool devProcessPendingCmdReqQueue(WINC_DEV_CTRL_CTX *pCtrlCtx, WINC_SEND_REQ_STATE *pSendReqState)
{
    uint16_t cmd53Status;
    uint32_t message;

    if (NULL == pCtrlCtx)
    {
        return false;
    }

    if (NULL == pSendReqState)
    {
        pCtrlCtx->pSendReqState = NULL;
        return true;
    }

    /* Construct message to device: | TX_REQ | No. of cmds | Size of first msg | */

    message = (WINC_DEV_EVENT_TX_REQ << 24) | (pSendReqState->numCmds << 16);

    if (pSendReqState->numCmds > 1)
    {
        uint32_t csaPtr = 0x00000000;

        /* When sending more than a single command request, send the list of
         command sizes through to the device using the CSA. */

        cmd53Status = WINC_SDIOCmd53Write(WINC_SDIOREG_FN0_FBR_FN1_CSA_PTR, (uint8_t*)&csaPtr, 3, true);
        if (WINC_SDIO_R1RSP_OK != cmd53Status)
        {
            WINC_ERROR_PRINT("error, csa CMD53 write failed, status=0x%04x\n", cmd53Status);
            pCtrlCtx->busError = true;
            devFlushPendingCmdReqQueue(pCtrlCtx);
            return false;
        }

        cmd53Status = WINC_SDIOCmd53Write(WINC_SDIOREG_FN0_FBR_FN1_CSA_DATA, (uint8_t*)pSendReqState->cmds, ((uint32_t)pSendReqState->numCmds) * sizeof(uint32_t), false);
        if (WINC_SDIO_R1RSP_OK != cmd53Status)
        {
            WINC_ERROR_PRINT("error, length CMD53 write failed, status=0x%04x\n", cmd53Status);
            pCtrlCtx->busError = true;
            devFlushPendingCmdReqQueue(pCtrlCtx);
            return false;
        }

        /* Indicate the size of the list being sent via the CSA in the message. */

        message |= pSendReqState->numCmds;
    }
    else
    {
        /* Indicate the size of the single command request in the message. */

        message |= pSendReqState->cmds[0].size;
    }

    cmd53Status = WINC_SDIOCmd53Write(WINC_SDIOREG_FN1_SD_HOST_GP, (uint8_t*)&message, 4, true);
    if (WINC_SDIO_R1RSP_OK != cmd53Status)
    {
        WINC_ERROR_PRINT("error, message CMD53 write failed, status=0x%04x\n", cmd53Status);
        pCtrlCtx->busError = true;
        devFlushPendingCmdReqQueue(pCtrlCtx);
        return false;
    }

    pSendReqState->pCurHdrElem = pSendReqState->pFirstHdrElem;

    pCtrlCtx->pSendReqState = pSendReqState;

    return true;
}

/*****************************************************************************
  Description:
    Decode a command response or AEC message.

  Parameters:
    pCtrlCtx  - Pointer to the device control context
    pMsg      - Pointer to response message
    msgLength - Length of response message

  Returns:
    None

  Remarks:
    Both command response and AEC are decoded. Command response are matched to
    their corresponding command request burst.

 *****************************************************************************/

static void devDecodeResponseMsg(WINC_DEV_CTRL_CTX *pCtrlCtx, uint8_t *pMsg, size_t msgLength)
{
    WINC_COMMAND_REQUEST *pCmdReq;
    WINC_COMMAND_RESPONSE *pCmdRsp;
    uint16_t rspCmdId;
    uint16_t seqNum;
    int i;
    WINC_SEND_REQ_STATE *pSendReqState;

    if ((NULL == pCtrlCtx) || (NULL == pMsg) || (msgLength < WINC_CMD_RSP_HDR_SIZE))
    {
        return;
    }

    /* Setup pointer to current head command request burst. */

    pSendReqState = (WINC_SEND_REQ_STATE*)pCtrlCtx->cmdReqQueue;

    /* Setup pointer to decode the response header. */

    pCmdRsp = (WINC_COMMAND_RESPONSE*)pMsg;

    /* Extract command ID and sequence number from the response header for later use. */

    rspCmdId = WINC_FIELD_UNPACK_16(pCmdRsp->id);
    seqNum   = WINC_FIELD_UNPACK_16(pCmdRsp->seqNum);

    WINC_TRACE_PRINT("Message type %d received, length=%d, ID=%04x, SN=%04x\n", pCmdRsp->msgType, msgLength, rspCmdId, seqNum);

    if ((WINC_COMMAND_MSG_TYPE_STATUS == pCmdRsp->msgType) || (WINC_COMMAND_MSG_TYPE_RSP == pCmdRsp->msgType))
    {
        WINC_SEND_REQ_HDR_ELEM *pReqHdr;
        int numStatus = 0;

        /* Ensure a minimum length has been passed in to be able to process the header. */

        if (WINC_COMMAND_MSG_TYPE_STATUS == pCmdRsp->msgType)
        {
            if (msgLength < WINC_CMD_RSP_STATUS_LEN)
            {
                return;
            }
        }
        else
        {
            if (msgLength < WINC_CMD_RSP_DATA_MIN_LEN)
            {
                return;
            }
        }

        /* Ensure there is a command request burst to check against. */

        if (NULL == pSendReqState)
        {
            return;
        }

        /* Setup pointers to first command request within the burst. */

        pReqHdr = pSendReqState->pFirstHdrElem;

        for (i=0; i<pSendReqState->numCmds; i++)
        {
            /* Advance to the next request header related to the command header. */

            while (0 == (pReqHdr->flags & WINC_FLAG_FIRST_IN_BURST))
            {
                pReqHdr++;
            }

            /* Ensure the request header pointer is valid. */

            if (NULL == pReqHdr->pPtr)
            {
                continue;
            }

            /* Status responses replace their corresponding command requests within
             the burst, so ignore them when looking for a matching request. */

            if (WINC_COMMAND_MSG_TYPE_STATUS == pReqHdr->pPtr[0])
            {
                /* Count the number of command requests with previously received
                 responses, to determine when all requests have been acknowledged. */

                numStatus++;
            }
            else if (WINC_COMMAND_MSG_TYPE_REQ == pReqHdr->pPtr[0])
            {
                pCmdReq = (WINC_COMMAND_REQUEST*)pReqHdr->pPtr;

                /* Command requests and responses share the same header format for
                 ID and sequence number, so a simple compare can be used to match them. */

                if (0 == memcmp(&pReqHdr->pPtr[1], &pMsg[1], WINC_CMD_RSP_HDR_SIZE-1))
                {
                    WINC_VERBOSE_PRINT("Match found on command %d\n", i);

                    if (WINC_COMMAND_MSG_TYPE_STATUS == pCmdRsp->msgType)
                    {
                        /* The message is a status response, this completes a command request. */

                        uint8_t numParams;

                        /* Save the number of parameters in the original command request to be
                         used later in the callback. */

                        numParams = pCmdReq->numParams;

                        /* Copy the command response over the command request, this wipes out
                         the request header but leaves the parameters intact incase they are
                         needed in the callback. */

                        memcpy(pReqHdr->pPtr, pMsg, WINC_CMD_RSP_STATUS_LEN);

                        /* Update the request header to reflect the new length and flag
                         that the status has been received. */

                        pReqHdr->length = WINC_CMD_RSP_STATUS_LEN;
                        pReqHdr->flags |= WINC_FLAG_STATUS_RCVD;

                        /* Unpack the response status into the request header. */

                        pSendReqState->cmds[i].rsp.status = WINC_FIELD_UNPACK_16(pCmdRsp->payload.status.status);

                        if (WINC_STATUS_OK != pSendReqState->cmds[i].rsp.status)
                        {
                            pSendReqState->numErrors++;
                        }

                        devModReqCountUpdate(pCtrlCtx, rspCmdId>>8, false);

                        WINC_TRACE_PRINT("Status %04x %04x in CMD(%d), SN=%04x\n", pSendReqState->cmds[i].rsp.status, rspCmdId, i, seqNum);

                        if (NULL != pSendReqState->pfCmdRspCallback)
                        {
                            WINC_DEV_EVENT_STATUS_ARGS eventStatusArgs;

                            /* Callback to the application layer that sent the command request
                             indicating the request is complete. */

                            eventStatusArgs.rspCmdId         = rspCmdId;
                            eventStatusArgs.srcCmd.idx       = i;
                            eventStatusArgs.seqNum           = seqNum;
                            eventStatusArgs.status           = pSendReqState->cmds[i].rsp.status;
                            eventStatusArgs.srcCmd.numParams = numParams;
                            eventStatusArgs.srcCmd.pParams   = pCmdReq->params;

                            pSendReqState->pfCmdRspCallback(pSendReqState->cmdRspCallbackCtx, (WINC_DEVICE_HANDLE)pCtrlCtx, (WINC_CMD_REQ_HANDLE)pSendReqState, WINC_DEV_CMDREQ_EVENT_CMD_STATUS, (uintptr_t)&eventStatusArgs);
                        }

                        /* Count the new status response in the burst total. */

                        numStatus++;

                        if (numStatus == pSendReqState->numCmds)
                        {
                            /* All command requests in this burst have been acknowledged
                             and are now complete. Callback to the application layer. Control
                             of the memory of the command request burst is now handed back
                             to the application layer. */

                            WINC_CMD_REQ_HANDLE nextCmdReq;

                            nextCmdReq = pSendReqState->nextCmdReq;

                            if (NULL != pSendReqState->pfCmdRspCallback)
                            {
                                WINC_DEV_EVENT_COMPLETE_ARGS eventCompleteArgs;

                                eventCompleteArgs.numCmds   = pSendReqState->numCmds;
                                eventCompleteArgs.numErrors = pSendReqState->numErrors;

                                pSendReqState->pfCmdRspCallback(pSendReqState->cmdRspCallbackCtx, (WINC_DEVICE_HANDLE)pCtrlCtx, (WINC_CMD_REQ_HANDLE)pSendReqState, WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE, (uintptr_t)&eventCompleteArgs);
                            }

                            WINC_VERBOSE_PRINT("CmdReq %08x retired, moving to %08x\n", pSendReqState, nextCmdReq);

                            /* Detach the memory of the current complete burst, move on
                             to the next one in the queue. */

                            pSendReqState = NULL;

                            pCtrlCtx->cmdReqQueue = nextCmdReq;
                        }
                    }
                    else
                    {
                        /* Command responses are simply passed to the application layer
                         for processing. */

                        WINC_DEV_EVENT_RSP_ELEMS eventRspElems;

                        eventRspElems.rspCmdId          = rspCmdId;
                        eventRspElems.srcCmd.idx        = i;
                        eventRspElems.srcCmd.numParams  = pCmdReq->numParams;
                        eventRspElems.srcCmd.pParams    = pCmdReq->params;

                        devUnpackResponseElements(pCmdRsp, &eventRspElems);

                        if (NULL != pSendReqState->pfCmdRspCallback)
                        {
                            pSendReqState->pfCmdRspCallback(pSendReqState->cmdRspCallbackCtx, (WINC_DEVICE_HANDLE)pCtrlCtx, (WINC_CMD_REQ_HANDLE)pSendReqState, WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED, (uintptr_t)&eventRspElems);
                        }
                    }

                    break;
                }
            }

            pReqHdr++;
        }
    }
    else if (WINC_COMMAND_MSG_TYPE_AEC == pCmdRsp->msgType)
    {
        /* AECs are passed to the application layer, the response elements are
         passed to all registered AEC callback handlers. */

        WINC_DEV_EVENT_RSP_ELEMS eventRspElems;

        memset(&eventRspElems, 0, sizeof(eventRspElems));

        devUnpackResponseElements(pCmdRsp, &eventRspElems);

        for (i=0; i<WINC_DEV_NUM_AEC_CB_ENTRIES; i++)
        {
            if (NULL != pCtrlCtx->aecCallbackTable[i].pfAecRspCallback)
            {
                pCtrlCtx->aecCallbackTable[i].pfAecRspCallback(pCtrlCtx->aecCallbackTable[i].aecRspCallbackCtx, (WINC_DEVICE_HANDLE)pCtrlCtx, &eventRspElems);
            }
        }
    }
    else
    {
        WINC_ERROR_PRINT("error, invalid message type received %d\n", pCmdRsp->msgType);
    }
}

/*****************************************************************************
  Description:
    Process a transmit request event from the device.

  Parameters:
    pCtrlCtx - Pointer to the device control context
    pEvent   - Pointer to the event context

  Returns:
    true or false indicating success or failure

  Remarks:
    The device has indicated it is ready to receive a transmission. The next
    request header of the current command request is transmitted to the device.

 *****************************************************************************/

static bool devProcessTransmitReqEvent(WINC_DEV_CTRL_CTX *pCtrlCtx, WINC_DEV_EVENT_CTX *pEvent)
{
    uint16_t cmd53Status;

    if ((NULL == pCtrlCtx) || (NULL == pEvent))
    {
        return false;
    }

    if ((NULL == pEvent->txReq.pSendReqHdr) || (NULL == pEvent->txReq.pSendReqHdr->pPtr))
    {
        return false;
    }

    if ((0 == pEvent->number) || (0 == pEvent->length))
    {
        return false;
    }

    /* Send the next request header data block to the device, lengths are aligned to
     32-bits. */

    pEvent->txReq.pSendReqHdr->length = (pEvent->txReq.pSendReqHdr->length + 3) & ~3;

    WINC_VERBOSE_PRINT("R: %08x %08x %d %d\n", pEvent->txReq.pSendReqHdr, pEvent->txReq.pSendReqHdr->pPtr, pEvent->txReq.pSendReqHdr->length, pEvent->length);
    cmd53Status = WINC_SDIOCmd53Write(WINC_SDIOREG_FN1_DATA, pEvent->txReq.pSendReqHdr->pPtr, pEvent->txReq.pSendReqHdr->length, false);
    if (WINC_SDIO_R1RSP_OK != cmd53Status)
    {
        WINC_ERROR_PRINT("error, msg(%d) CMD53 write failed, status=0x%04x\n", (pEvent->rxReq.pMsgLengths-(uint32_t*)pCtrlCtx->pReceiveBuffer), cmd53Status);
        memset(pEvent, 0, sizeof(WINC_DEV_EVENT_CTX));
        pCtrlCtx->busError = true;
        devFlushPendingCmdReqQueue(pCtrlCtx);
        return false;
    }
    WINC_VERBOSE_PRINT("status = 0x%04x\n", cmd53Status);

    pEvent->length -= (pEvent->txReq.pSendReqHdr->length >> 2);
    pEvent->number--;

    /* Move on to the next request header within the current burst. */

    pEvent->txReq.pSendReqHdr++;

    if (NULL == pEvent->txReq.pSendReqHdr->pPtr)
    {
        /* This is the last request header within the burst, signal the application
         layer that transmission has completed and move to the next burst in the
         queue. */

        WINC_SEND_REQ_STATE *pSendReqState = (WINC_SEND_REQ_STATE*)pEvent->txReq.cmdReqHandle;

        if (NULL != pSendReqState)
        {
            if (NULL != pSendReqState->pfCmdRspCallback)
            {
                pSendReqState->pfCmdRspCallback(pSendReqState->cmdRspCallbackCtx, (WINC_DEVICE_HANDLE)pCtrlCtx, pEvent->txReq.cmdReqHandle, WINC_DEV_CMDREQ_EVENT_TX_COMPLETE, 0);
            }

            memset(pEvent, 0, sizeof(WINC_DEV_EVENT_CTX));

            WINC_VERBOSE_PRINT("CmdReq %08x complete, moving to %08x\n", pSendReqState, pSendReqState->nextCmdReq);

            devProcessPendingCmdReqQueue(pCtrlCtx, (WINC_SEND_REQ_STATE*)pSendReqState->nextCmdReq);
        }
    }
    else if (0 == pEvent->number)
    {
        WINC_SEND_REQ_STATE *pSendReqState = (WINC_SEND_REQ_STATE*)pEvent->txReq.cmdReqHandle;

        if (NULL != pSendReqState)
        {
            pSendReqState->pCurHdrElem = pEvent->txReq.pSendReqHdr;
        }

        memset(pEvent, 0, sizeof(WINC_DEV_EVENT_CTX));
    }

    return true;
}

/*****************************************************************************
  Description:
    Process a receive request event from the device.

  Parameters:
    pCtrlCtx - Pointer to the device control context
    pEvent   - Pointer to the event context

  Returns:
    true or false indicating success or failure

  Remarks:

 *****************************************************************************/

static bool devProcessReceiveReqEvent(WINC_DEV_CTRL_CTX *pCtrlCtx, WINC_DEV_EVENT_CTX *pEvent)
{
    uint16_t cmd53Status;
    size_t msgLength;

    (void)cmd53Status;

    if ((NULL == pCtrlCtx) || (NULL == pEvent))
    {
        return false;
    }

    if ((0 == pEvent->number) || (0 == pEvent->length))
    {
        return false;
    }

    if (NULL == pEvent->rxReq.pMsgLengths)
    {
        pEvent->rxReq.pMsgLengths = (uint32_t*)pCtrlCtx->pReceiveBuffer;

        if (pEvent->number > 1)
        {
            pEvent->number = pEvent->length;

            msgLength = pEvent->length << 2;

            cmd53Status = WINC_SDIOCmd53Read(WINC_SDIOREG_FN1_DATA, pCtrlCtx->pReceiveBuffer, msgLength, false);
            if (WINC_SDIO_R1RSP_OK != cmd53Status)
            {
                WINC_ERROR_PRINT("error, length CMD53 read failed, status=0x%04x\n", cmd53Status);
                memset(pEvent, 0, sizeof(WINC_DEV_EVENT_CTX));
                return false;
            }
        }
        else
        {
            *pEvent->rxReq.pMsgLengths = pEvent->length;
        }

        pEvent->rxReq.receiveBufferOffset = WINC_DEV_CACHE_GET_SIZE(pEvent->number * sizeof(uint32_t));
    }

    msgLength = *pEvent->rxReq.pMsgLengths << 2;

    if (msgLength > (pCtrlCtx->receiveBufferSize - pEvent->rxReq.receiveBufferOffset))
    {
        WINC_ERROR_PRINT("error, receive message exceeds buffer. %d %d %d\n", *pEvent->rxReq.pMsgLengths, pCtrlCtx->receiveBufferSize, pEvent->number);
        memset(pEvent, 0, sizeof(WINC_DEV_EVENT_CTX));
        return false;
    }

    cmd53Status = WINC_SDIOCmd53Read(WINC_SDIOREG_FN1_DATA, &pCtrlCtx->pReceiveBuffer[pEvent->rxReq.receiveBufferOffset], msgLength, false);
    if (WINC_SDIO_R1RSP_OK != cmd53Status)
    {
        WINC_ERROR_PRINT("error, msg(%d) CMD53 read failed, status=0x%04x\n", (pEvent->rxReq.pMsgLengths-(uint32_t*)pCtrlCtx->pReceiveBuffer), cmd53Status);
        memset(pEvent, 0, sizeof(WINC_DEV_EVENT_CTX));
        return false;
    }

    devDecodeResponseMsg(pCtrlCtx, &pCtrlCtx->pReceiveBuffer[pEvent->rxReq.receiveBufferOffset], msgLength);

    pEvent->rxReq.pMsgLengths++;

    pEvent->number--;

    if (0 == pEvent->number)
    {
        memset(pEvent, 0, sizeof(WINC_DEV_EVENT_CTX));
    }

    return true;
 }

/*****************************************************************************
  Description:
    Initialise the device.

  Parameters:
    pInitData - Pointer to initialisation data

  Returns:
    A WINC_DEVICE_HANDLE device handle or WINC_DEVICE_INVALID_HANDLE for error

  Remarks:

 *****************************************************************************/

WINC_DEVICE_HANDLE WINC_DevInit(WINC_DEV_INIT *pInitData)
{
    if (true == wincDevCtrlCtx.isInit)
    {
        wincDevCtrlCtx.busError = false;

        return (WINC_DEVICE_HANDLE)&wincDevCtrlCtx;
    }

    if (NULL == pInitData)
    {
        return WINC_DEVICE_INVALID_HANDLE;
    }

    /* Clear device control context and store initialisation data into it. */

    memset(&wincDevCtrlCtx, 0, sizeof(WINC_DEV_CTRL_CTX));

#ifdef WINC_DEV_CACHE_LINE_SIZE
    wincDevCtrlCtx.pReceiveBuffer    = (uint8_t*)(((uintptr_t)pInitData->pReceiveBuffer + (WINC_DEV_CACHE_LINE_SIZE-1)) & ~(WINC_DEV_CACHE_LINE_SIZE-1));
    wincDevCtrlCtx.receiveBufferSize = (pInitData->receiveBufferSize - (wincDevCtrlCtx.pReceiveBuffer-pInitData->pReceiveBuffer)) & ~(WINC_DEV_CACHE_LINE_SIZE-1);
#else
    wincDevCtrlCtx.pReceiveBuffer    = pInitData->pReceiveBuffer;
    wincDevCtrlCtx.receiveBufferSize = pInitData->receiveBufferSize;
#endif

    wincDevCtrlCtx.cmdReqQueue = WINC_CMD_REQ_INVALID_HANDLE;

    wincDevCtrlCtx.isInit = true;

    return (WINC_DEVICE_HANDLE)&wincDevCtrlCtx;
}

/*****************************************************************************
  Description:
    De-initialise the device.

  Parameters:
    devHandle - Device handle obtained from WINC_DevInit

  Returns:
    None.

  Remarks:

 *****************************************************************************/

void WINC_DevDeinit(WINC_DEVICE_HANDLE devHandle)
{
    WINC_DEV_CTRL_CTX *pCtrlCtx = (WINC_DEV_CTRL_CTX*)devHandle;
    WINC_SEND_REQ_STATE *pPendingSendReqState;

    if ((NULL == pCtrlCtx) || (false == pCtrlCtx->isInit))
    {
        return;
    }

    pCtrlCtx->isInit = false;

    pPendingSendReqState = (WINC_SEND_REQ_STATE*)pCtrlCtx->cmdReqQueue;

    while (NULL != pPendingSendReqState)
    {
        if (NULL != pPendingSendReqState->pfCmdRspCallback)
        {
            pPendingSendReqState->pfCmdRspCallback(pPendingSendReqState->cmdRspCallbackCtx, (WINC_DEVICE_HANDLE)pCtrlCtx, (WINC_CMD_REQ_HANDLE)pPendingSendReqState, WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE, 0);
        }

        pPendingSendReqState = (WINC_SEND_REQ_STATE*)pPendingSendReqState->nextCmdReq;
    }
}

/*****************************************************************************
  Description:
    Set the debug printf function pointer.

  Parameters:
    pfPrintf - Pointer to a printf style function pointer

  Returns:
    None.

  Remarks:

 *****************************************************************************/

void WINC_DevSetDebugPrintf(WINC_DEBUG_PRINTF_FP pfPrintf)
{
    pfWINCDevDebugPrintf = pfPrintf;
}

/*****************************************************************************
  Description:
    Transmit command request burst.

  Parameters:
    devHandle    - Device handle obtained from WINC_DevInit
    cmdReqHandle - Command request handle obtained from WINC_CmdReqInit

  Returns:
    true or false indicating success or failure

  Remarks:

 *****************************************************************************/

bool WINC_DevTransmitCmdReq(WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle)
{
    WINC_DEV_CTRL_CTX *pCtrlCtx = (WINC_DEV_CTRL_CTX*)devHandle;
    WINC_SEND_REQ_STATE *pSendReqState = (WINC_SEND_REQ_STATE*)cmdReqHandle;

    if ((NULL == pCtrlCtx) || (false == pCtrlCtx->isInit) || (NULL == pSendReqState) || (NULL == pSendReqState->pPtr) || (true == pCtrlCtx->busError))
    {
        return false;
    }

    if (false == devPreparePendingCmdReq(pCtrlCtx, pSendReqState))
    {
        return false;
    }

    if (WINC_CMD_REQ_INVALID_HANDLE != pCtrlCtx->cmdReqQueue)
    {
        /* If there is currently a request in the queue, walk the queue and
         append the request to the end. */

        WINC_SEND_REQ_STATE *pPendingSendReqState = (WINC_SEND_REQ_STATE*)pCtrlCtx->cmdReqQueue;

        while (WINC_CMD_REQ_INVALID_HANDLE != pPendingSendReqState->nextCmdReq)
        {
            pPendingSendReqState = (WINC_SEND_REQ_STATE*)pPendingSendReqState->nextCmdReq;
        }

        WINC_VERBOSE_PRINT("CmdReq add %08x to %08x\n", pSendReqState, pPendingSendReqState);

        pPendingSendReqState->nextCmdReq = cmdReqHandle;

        if (NULL == pCtrlCtx->pSendReqState)
        {
            return devProcessPendingCmdReqQueue(pCtrlCtx, pSendReqState);
        }
    }
    else
    {
        /* No outstanding request, place on the queue and kick off processing. */

        pCtrlCtx->cmdReqQueue = cmdReqHandle;

        WINC_VERBOSE_PRINT("CmdReq add %08x\n", pSendReqState);

        return devProcessPendingCmdReqQueue(pCtrlCtx, pSendReqState);
    }

    return true;
}

/*****************************************************************************
  Description:
    Update a current event.

  Parameters:
    devHandle - Device handle obtained from WINC_DevInit

  Returns:
    true or false indicating success or failure

  Remarks:
    Performs the next iteration of a current event.

 *****************************************************************************/

bool WINC_DevUpdateEvent(WINC_DEVICE_HANDLE devHandle)
{
    WINC_DEV_CTRL_CTX *pCtrlCtx = (WINC_DEV_CTRL_CTX*)devHandle;

    if ((NULL == pCtrlCtx) || (false == pCtrlCtx->isInit) || (true == pCtrlCtx->busError))
    {
        return false;
    }

    switch (pCtrlCtx->eventCtx.type)
    {
        case WINC_DEV_EVENT_NONE:
        {
            break;
        }

        case WINC_DEV_EVENT_TX_REQ:
        {
            return devProcessTransmitReqEvent(pCtrlCtx, &pCtrlCtx->eventCtx);
        }

        case WINC_DEV_EVENT_RX_REQ:
        {
            return devProcessReceiveReqEvent(pCtrlCtx, &pCtrlCtx->eventCtx);
        }

        default:
        {
            WINC_ERROR_PRINT("error, unknown event type %d\n", pCtrlCtx->eventCtx.type);
            break;
        }
    }

    return true;
}

/*****************************************************************************
  Description:
    Handle a new event request from the device.

  Parameters:
    devHandle       - Device handle obtained from WINC_DevInit
    pfEventIntCheck - Pointer to event check function

  Returns:
    true or false indicating success or failure

  Remarks:
    When the device posts a message indicating a new event is ready it is
    read and added as an event to process.
    The pfEventIntCheck function, if supplied, should return true if the WINC
    event interrupt is still asserted.

 *****************************************************************************/

bool WINC_DevHandleEvent(WINC_DEVICE_HANDLE devHandle, WINC_DEV_EVENT_CHECK_FP pfEventIntCheck)
{
    WINC_DEV_CTRL_CTX *pCtrlCtx = (WINC_DEV_CTRL_CTX*)devHandle;
    uint8_t regValue;
    uint8_t regSetValue;
    uint8_t cmd52Status;
    uint16_t cmd53Status;

    if ((NULL == pCtrlCtx) || (false == pCtrlCtx->isInit) || (true == pCtrlCtx->busError))
    {
        return false;
    }

    /* Check for ongoing event. */

    if (WINC_DEV_EVENT_NONE != pCtrlCtx->eventCtx.type)
    {
        return false;
    }

    /* Read the interrupt ID register, check for message event indication. */

    cmd52Status = WINC_SDIOCmd52(WINC_SDIOREG_FN1_INT_ID_CLR, NULL, &regValue);
    if (WINC_SDIO_R1RSP_OK != (cmd52Status & 0xfe))
    {
        WINC_ERROR_PRINT("error, interrupt check failed, status=%02x\n", cmd52Status);
        pCtrlCtx->busError = true;
        return false;
    }

    if (regValue & WINC_SDIO_REG_FN1_INT_MSG_FROM_ARM)
    {
        /* New event message from device. */

        uint32_t *pMessage = (uint32_t*)pCtrlCtx->pReceiveBuffer;

        /* Receive message and decode. */

        cmd53Status = WINC_SDIOCmd53Read(WINC_SDIOREG_FN1_ARM_GP, (uint8_t*)pMessage, sizeof(uint32_t), true);
        if (WINC_SDIO_R1RSP_OK != cmd53Status)
        {
            WINC_ERROR_PRINT("error, message CMD53 read failed, status=0x%04x\n", cmd53Status);
            pCtrlCtx->busError = true;
            return false;
        }

        pCtrlCtx->eventCtx.type   = (*pMessage >> 24) & 0xff;
        pCtrlCtx->eventCtx.number = (*pMessage >> 16) & 0xff;
        pCtrlCtx->eventCtx.length = *pMessage & 0xffff;

        regSetValue = WINC_SDIO_REG_FN1_INT_MSG_FROM_ARM;

        switch (pCtrlCtx->eventCtx.type)
        {
            case WINC_DEV_EVENT_TX_REQ:
            {
                if (NULL == pCtrlCtx->pSendReqState)
                {
                    break;
                }

                if (regValue & WINC_SDIO_REG_FN1_INT_ACK_TO_HOST)
                {
                    regSetValue |= WINC_SDIO_REG_FN1_INT_ACK_TO_HOST;
                }
                else
                {
                    WINC_ERROR_PRINT("error, no ack to message\n");
                }

                pCtrlCtx->eventCtx.txReq.pSendReqHdr  = pCtrlCtx->pSendReqState->pCurHdrElem;
                pCtrlCtx->eventCtx.txReq.cmdReqHandle = (WINC_CMD_REQ_HANDLE)pCtrlCtx->pSendReqState;
                break;
            }

            case WINC_DEV_EVENT_RX_REQ:
            {
                pCtrlCtx->eventCtx.rxReq.pMsgLengths         = NULL;
                pCtrlCtx->eventCtx.rxReq.receiveBufferOffset = 0;
                break;
            }

            default:
            {
                WINC_ERROR_PRINT("error, unknown event type\n");
                return false;
            }
        }

        cmd52Status = WINC_SDIOCmd52(WINC_SDIOREG_FN1_INT_ID_CLR, &regSetValue, NULL);

        if (0x00 != cmd52Status)
        {
            WINC_ERROR_PRINT("error, interrupt clear (0x%02x) failed, status=%02x\n", regSetValue, cmd52Status);
            pCtrlCtx->busError = true;
            return false;
        }

        if (NULL != pfEventIntCheck)
        {
            /* Wait for device interrupt line to clear to indicate the device has
             received an event acknowledgement and is ready to proceed. */

            while (true == pfEventIntCheck())
            {
            }
        }
    }
    else
    {
        WINC_ERROR_PRINT("error, unhandled event 0x%02x\n", regValue);
    }

    return true;
}

/*****************************************************************************
  Description:
    Register a callback to receive AEC notifications.

  Parameters:
    devHandle         - Device handle obtained from WINC_DevInit
    pfAecRspCallback  - Pointer to callback function
    aecRspCallbackCtx - Callback context, provided to callback when called

  Returns:
    true or false indicating success or failure

  Remarks:

 *****************************************************************************/

bool WINC_DevAECCallbackRegister(WINC_DEVICE_HANDLE devHandle, WINC_DEV_AEC_RSP_CB pfAecRspCallback, uintptr_t aecRspCallbackCtx)
{
    WINC_DEV_CTRL_CTX *pCtrlCtx = (WINC_DEV_CTRL_CTX*)devHandle;
    int i;

    if ((NULL == pCtrlCtx) || (false == pCtrlCtx->isInit))
    {
        return false;
    }

    for (i=0; i<WINC_DEV_NUM_AEC_CB_ENTRIES; i++)
    {
        if (NULL == pCtrlCtx->aecCallbackTable[i].pfAecRspCallback)
        {
            pCtrlCtx->aecCallbackTable[i].pfAecRspCallback  = pfAecRspCallback;
            pCtrlCtx->aecCallbackTable[i].aecRspCallbackCtx = aecRspCallbackCtx;

            return true;
        }
    }

    return false;
}

/*****************************************************************************
  Description:
    De-register an AEC notification callback.

  Parameters:
    devHandle        - Device handle obtained from WINC_DevInit
    pfAecRspCallback - Pointer to callback function

  Returns:
    true or false indicating success or failure

  Remarks:

 *****************************************************************************/

bool WINC_DevAECCallbackDeregister(WINC_DEVICE_HANDLE devHandle, WINC_DEV_AEC_RSP_CB pfAecRspCallback)
{
    WINC_DEV_CTRL_CTX *pCtrlCtx = (WINC_DEV_CTRL_CTX*)devHandle;
    int i;

    if ((NULL == pCtrlCtx) || (false == pCtrlCtx->isInit))
    {
        return false;
    }

    for (i=0; i<WINC_DEV_NUM_AEC_CB_ENTRIES; i++)
    {
        if (pfAecRspCallback == pCtrlCtx->aecCallbackTable[i].pfAecRspCallback)
        {
            pCtrlCtx->aecCallbackTable[i].pfAecRspCallback  = NULL;
            pCtrlCtx->aecCallbackTable[i].aecRspCallbackCtx = 0;

            return true;
        }
    }

    return false;
}

/*****************************************************************************
  Description:
    Unpack TLV parameters or elements from messages.

  Parameters:
    numTlvs   - Number of TLVs to unpack
    pTLVBytes - Pointer to TLVs in message
    pElems    - Pointer to param/element structure to receive information

  Returns:
    true or false indicating success or failure

  Remarks:

 *****************************************************************************/

bool WINC_DevUnpackElements(uint8_t numTlvs, uint8_t *pTLVBytes, WINC_DEV_PARAM_ELEM *pElems)
{
    int i;

    if ((NULL == pTLVBytes) || (NULL == pElems))
    {
        return false;
    }

    for (i=0; i<numTlvs; i++, pElems++)
    {
        WINC_TLV_ELEMENT *pTLV;

        pTLV = (WINC_TLV_ELEMENT*)pTLVBytes;

        pElems->type   = pTLV->type;
        pElems->length = WINC_FIELD_UNPACK_16(pTLV->length);
        pElems->pData  = pTLV->data;

        WINC_VERBOSE_PRINT(" %d: T=%d, L=%d\n", i, pElems->type, pElems->length);

        pTLVBytes += sizeof(WINC_TLV_ELEMENT) + pElems->length + (pTLV->flags & 0x03);
    }

    return true;
}
