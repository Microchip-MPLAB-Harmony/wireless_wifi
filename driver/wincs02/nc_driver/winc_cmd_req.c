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

    uint16_t sendReqStateSz = sizeof(WINC_SEND_REQ_STATE) + (sizeof(uint32_t) * numCommands);
    uint16_t sendReqHdrSz   = (sizeof(WINC_SEND_REQ_HDR_ELEM) * WINC_NUM_SEND_REQ_HDRS) * numCommands;

    if (NULL == pBuffer)
    {
        return WINC_CMD_REQ_INVALID_HANDLE;
    }

    /* Ensure buffer is long enough to contain the send request state and header elements */

    if (lenBuffer < (sendReqStateSz + sendReqHdrSz))
    {
        return WINC_CMD_REQ_INVALID_HANDLE;
    }

    memset(pBuffer, 0, sendReqStateSz + sendReqHdrSz);

    pSendReqState = (WINC_SEND_REQ_STATE*)pBuffer;
    pSendReqState->size = lenBuffer;

    pBuffer      += sendReqStateSz;
    lenBuffer    -= sendReqStateSz;

    pSendReqState->pFirstHdrElem = (WINC_SEND_REQ_HDR_ELEM*)pBuffer;
    pSendReqState->pCurHdrElem   = (WINC_SEND_REQ_HDR_ELEM*)pBuffer;

    pBuffer   += sendReqHdrSz;
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
