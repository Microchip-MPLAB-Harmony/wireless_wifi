/*
Copyright (C) 2023-25 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "winc_dev.h"
#include "winc_cmd_req.h"

/* Allocate a number of request headers per command. */
#define WINC_NUM_SEND_REQ_HDRS      3

/*****************************************************************************
  Description:
    Initialise a command request burst.

  Parameters:
    pBuffer           - Pointer to buffer to contain command request burst
    lenBuffer         - Length of buffer provided
    numCommands       - Number of commands expected to be in burst
    pfCmdRspCallback  - Pointer to callback function to receive burst status
    cmdRspCallbackCtx - Context value to pass to callback function

  Returns:
    A WINC_CMD_REQ_HANDLE handle or WINC_CMD_REQ_INVALID_HANDLE

  Remarks:

 *****************************************************************************/

WINC_CMD_REQ_HANDLE WINC_CmdReqInit(uint8_t* pBuffer, size_t lenBuffer, int numCommands, WINC_DEV_CMD_RSP_CB pfCmdRspCallback, uintptr_t cmdRspCallbackCtx)
{
    WINC_SEND_REQ_STATE *pSendReqState;

    size_t sendReqStateSz = sizeof(WINC_SEND_REQ_STATE) + (sizeof(uint32_t) * (size_t)numCommands);
    size_t sendReqHdrSz   = (sizeof(WINC_SEND_REQ_HDR_ELEM) * (size_t)WINC_NUM_SEND_REQ_HDRS) * (size_t)numCommands;

    if (NULL == pBuffer)
    {
        return WINC_CMD_REQ_INVALID_HANDLE;
    }

    /* Ensure buffer is long enough to contain the send request state and header elements */

    if (lenBuffer < (sendReqStateSz + sendReqHdrSz))
    {
        return WINC_CMD_REQ_INVALID_HANDLE;
    }

    (void)memset(pBuffer, 0, sendReqStateSz + sendReqHdrSz);

    pSendReqState = (WINC_SEND_REQ_STATE*)pBuffer;
    pSendReqState->size = lenBuffer;

    pBuffer   = pBuffer + sendReqStateSz;
    lenBuffer -= sendReqStateSz;

    pSendReqState->pFirstHdrElem = (WINC_SEND_REQ_HDR_ELEM*)pBuffer;
    pSendReqState->pCurHdrElem   = (WINC_SEND_REQ_HDR_ELEM*)pBuffer;

    pBuffer   = pBuffer + sendReqHdrSz;
    lenBuffer -= sendReqHdrSz;

    pSendReqState->numCmds    = 0;
    pSendReqState->maxNumCmds = numCommands;
    pSendReqState->pPtr       = pBuffer;
    pSendReqState->space      = lenBuffer;

    /* Reserve single byte for zero termination in unpacking function. */
    pSendReqState->space--;

    pSendReqState->pFirstHdrElem->pPtr = pBuffer;
    pSendReqState->pfCmdRspCallback    = pfCmdRspCallback;
    pSendReqState->cmdRspCallbackCtx   = cmdRspCallbackCtx;

    return (uintptr_t)pSendReqState;
}

/*****************************************************************************
  Description:
    Discard a command request burst.

  Parameters:
    devHandle    - Device handle obtained from WINC_DevInit
    cmdReqHandle - Command request handle obtained from WINC_CmdReqInit

  Returns:
    true or false

  Remarks:
    devHandle may be WINC_DEVICE_INVALID_HANDLE if command request is not yet
    in use by the device.

 *****************************************************************************/

bool WINC_CmdReqDiscard(WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle)
{
    WINC_SEND_REQ_STATE *pSendReqState = (WINC_SEND_REQ_STATE*)cmdReqHandle;
    WINC_SEND_REQ_STATE *pNextCmdReq;

    if (NULL == pSendReqState)
    {
        return false;
    }

    while (NULL != pSendReqState)
    {
        pNextCmdReq = (WINC_SEND_REQ_STATE*)pSendReqState->nextCmdReq;

        if (NULL != pSendReqState->pfCmdRspCallback)
        {
            pSendReqState->pfCmdRspCallback(pSendReqState->cmdRspCallbackCtx, devHandle, (WINC_CMD_REQ_HANDLE)pSendReqState, WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE, 0);
        }

        pSendReqState = pNextCmdReq;
    }

    return true;
}
