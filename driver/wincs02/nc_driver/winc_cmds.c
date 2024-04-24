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

#include "winc_cmds.h"
#include "winc_dev.h"
#include "winc_cmd_req.h"
#include "winc_debug.h"

/*****************************************************************************
  Description:
    Determine if type is an integer type or not.

  Parameters:
    typeVal - Type of value being checked

  Returns:
    true or false indicating is type is an integer.

  Remarks:

 *****************************************************************************/

static bool cmdIsInt(WINC_TYPE typeVal)
{
    switch (typeVal)
    {
        case WINC_TYPE_STATUS:
        case WINC_TYPE_INTEGER:
        case WINC_TYPE_INTEGER_UNSIGNED:
        case WINC_TYPE_INTEGER_FRAC:
        case WINC_TYPE_BOOL:
        case WINC_TYPE_UTC_TIME:
        {
            return true;
        }

        default:
        {
            return false;
        }
    }

    return false;
}

/*****************************************************************************
  Description:
    Encodes a parameter into the command request.

  Parameters:
    handle  - Command request handle
    typeVal - Type of value being encoded
    pVal    - Pointer to value
    lenVal  - Length of value

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

static bool cmdParamBuilder(WINC_CMD_REQ_HANDLE handle, WINC_TYPE typeVal, const void *pVal, size_t lenVal)
{
    WINC_SEND_REQ_STATE *pSendReqState = (WINC_SEND_REQ_STATE*)handle;
    WINC_TLV_ELEMENT *pTlvElem;
    uint8_t valPad;

    if (NULL == pSendReqState)
    {
        WINC_ERROR_PRINT("error: invalid send request state handle\n");
        return false;
    }

    /* Determine the padding required to align value data to 32-bits. */
    valPad = (4 - (lenVal & 3)) & 3;

    if ((NULL == pSendReqState->pCmdReq) || (NULL == pSendReqState->pPtr) || (pSendReqState->space < (sizeof(WINC_TLV_ELEMENT) + lenVal + valPad)))
    {
        WINC_ERROR_PRINT("error: send request state failure, possible no space\n");
        pSendReqState->pPtr = NULL;
        return false;
    }

    /* Encode the TLV header with type, padding and length. */
    pTlvElem = (WINC_TLV_ELEMENT*)pSendReqState->pPtr;
    pTlvElem->type     = typeVal;
    pTlvElem->flags    = valPad;
    pTlvElem->length_h = lenVal >> 8;
    pTlvElem->length_l = lenVal & 0xff;

    if (NULL != pVal)
    {
        /* Copy in value data, for integers ensure they are packed big-endian. */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        if (true == cmdIsInt(typeVal))
        {
            int i;
            const uint8_t *pByte = pVal;

            pByte += lenVal;

            for (i=0; i<lenVal; i++)
            {
                pByte--;
                pTlvElem->data[i] = *pByte;
            }
        }
        else
#endif
        {
            memcpy(pTlvElem->data, pVal, lenVal);
        }
    }

    /* Clear padding area. */
    if (valPad > 0)
    {
        memset(&pTlvElem->data[lenVal], 0, valPad);
    }

    /* Update send request state for next command or parameter. */
    lenVal += valPad;

    pSendReqState->pPtr  += (sizeof(WINC_TLV_ELEMENT) + lenVal);
    pSendReqState->space -= (sizeof(WINC_TLV_ELEMENT) + lenVal);

    pSendReqState->pCmdReq->numParams++;

    pSendReqState->pCurHdrElem->length               += (sizeof(WINC_TLV_ELEMENT) + lenVal);
    pSendReqState->cmds[pSendReqState->numCmds].size += (sizeof(WINC_TLV_ELEMENT) + lenVal);

    return true;
}

/*****************************************************************************
  Description:
    Encode a multi-type parameter value into the command request.

  Parameters:
    handle  - Command request handle
    typeVal - Type of value being encoded
    val     - Value, either a pointer or an integer
    lenVal  - Length of value

  Returns:
    true or false indicating success or failure.

  Remarks:
    Determines if the value is an integer type, and thus the value is passed in or
    another type where the value is passed by pointer.

 *****************************************************************************/

static bool cmdMultiValueBuilder(WINC_CMD_REQ_HANDLE handle, WINC_TYPE typeVal, uintptr_t val, size_t lenVal)
{
    if (WINC_TYPE_INVALID != typeVal)
    {
        if (true == cmdIsInt(typeVal))
        {
            return cmdParamBuilder(handle, typeVal, &val, sizeof(uintptr_t));
        }
        else
        {
            return cmdParamBuilder(handle, typeVal, (void*)val, lenVal);
        }
    }

    return true;
}

/*****************************************************************************
  Description:
    Encode a possible fractional integer parameter value into the command request.

  Parameters:
    handle  - Command request handle
    val     - Value, either an integer or fractional integer

  Returns:
    true or false indicating success or failure.

  Remarks:
    Fractional integers consist of a non-zero 16-bit integer part and a 16-bit
    fractional part. If the top 16-bits are set it is considered a fractional
    integer, otherwise it is a normal unsigned integer.

 *****************************************************************************/

static bool cmdFractValueBuilder(WINC_CMD_REQ_HANDLE handle, uint32_t val)
{
    if ((val >> 16) > 1)
    {
        return cmdParamBuilder(handle, WINC_TYPE_INTEGER_FRAC, &val, 4);
    }
    else
    {
        return cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &val, 4);
    }
}

/*****************************************************************************
  Description:
    Checks a possible fractional integer or integer against a maximum value.

  Parameters:
    val    - Value, either an integer or fractional integer
    maxVal - Maximum value of integer.

  Returns:
    true or false indicating within range or not.

  Remarks:
    Fractional integers consist of a non-zero 16-bit integer part and a 16-bit
    fractional part. If the top 16-bits are set it is considered a fractional
    integer and the maximum value is compared to the top 16-bits, otherwise
    it is a normal unsigned integer and the maximum value is compared to the
    full value.

 *****************************************************************************/

static bool cmdCheckFractType(uint32_t val, uint16_t maxVal)
{
    if (((0 == (val >> 16)) && (val > maxVal)) || ((val >> 16) > maxVal))
    {
        return false;
    }

    return true;
}

/*****************************************************************************
  Description:
    Start to build a command request.

  Parameters:
    handle - Command request handle
    msgId  - Message ID

  Returns:
    Pointer to command request structure or NULL for error.

  Remarks:

 *****************************************************************************/

static WINC_COMMAND_REQUEST* cmdBuildCommandReq(WINC_CMD_REQ_HANDLE handle, uint16_t msgId)
{
    WINC_SEND_REQ_STATE *pSendReqState = (WINC_SEND_REQ_STATE*)handle;

    if (NULL == pSendReqState)
    {
        WINC_ERROR_PRINT("error: invalid send request state handle\n");
        return NULL;
    }

    WINC_VERBOSE_PRINT("Build: %04x in %08x (%d)\n", msgId, pSendReqState->pPtr, pSendReqState->size);

    /* Ensure first send request pointer is valid. */
    if (NULL == pSendReqState->pPtr)
    {
        WINC_ERROR_PRINT("error: send request state failure\n");
        return NULL;
    }

    /* Ensure there is space for the command headers. */
    if (pSendReqState->numCmds >= pSendReqState->maxNumCmds)
    {
        WINC_ERROR_PRINT("error: too many commands\n");
        pSendReqState->pPtr = NULL;
        return false;
    }

    /* Ensure there is enough space at least a message request header. */
    if (pSendReqState->space < sizeof(WINC_COMMAND_REQUEST))
    {
        pSendReqState->pPtr = NULL;
        return NULL;
    }

    pSendReqState->pCurHdrElem->pPtr = pSendReqState->pPtr;

    /* Command request header of message comes next, initialise it. */
    pSendReqState->pCmdReq = (WINC_COMMAND_REQUEST*)pSendReqState->pPtr;
    pSendReqState->pCmdReq->msgType   = WINC_COMMAND_MSG_TYPE_REQ;
    pSendReqState->pCmdReq->id_h      = msgId >> 8;
    pSendReqState->pCmdReq->id_l      = msgId & 0xff;
    pSendReqState->pCmdReq->numParams = 0;

    pSendReqState->pPtr  += sizeof(WINC_COMMAND_REQUEST);
    pSendReqState->space -= sizeof(WINC_COMMAND_REQUEST);

    /* Initialise the length in the send request header list. */
    pSendReqState->pCurHdrElem->length = sizeof(WINC_COMMAND_REQUEST);
    pSendReqState->pCurHdrElem->flags |= WINC_FLAG_FIRST_IN_BURST;

    WINC_VERBOSE_PRINT("Built: %08x %08x\n", pSendReqState->pCurHdrElem, pSendReqState->pCmdReq);

    return pSendReqState->pCmdReq;
}

/*****************************************************************************
  Description:
    Complete a command request.

  Parameters:
    handle - Command request handle

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

static bool cmdCompleteCommandReq(WINC_CMD_REQ_HANDLE handle)
{
    WINC_SEND_REQ_STATE *pSendReqState = (WINC_SEND_REQ_STATE*)handle;

    if (NULL == pSendReqState)
    {
        WINC_ERROR_PRINT("error: invalid send request state handle\n");
        return false;
    }

    if ((NULL == pSendReqState->pCmdReq) || (NULL == pSendReqState->pPtr))
    {
        WINC_ERROR_PRINT("error: send request state failure\n");
        pSendReqState->pPtr = NULL;
        return false;
    }

    WINC_VERBOSE_PRINT("Compl: %08x %d\n", pSendReqState->pCmdReq, pSendReqState->cmds[pSendReqState->numCmds].size);

    pSendReqState->pCmdReq->length_h = pSendReqState->cmds[pSendReqState->numCmds].size >> 8;
    pSendReqState->pCmdReq->length_l = pSendReqState->cmds[pSendReqState->numCmds].size & 0xff;

    pSendReqState->pCurHdrElem->flags |= WINC_FLAG_LAST_IN_BURST;

    pSendReqState->cmds[pSendReqState->numCmds].size += sizeof(WINC_COMMAND_REQUEST);
    pSendReqState->cmds[pSendReqState->numCmds].size >>= 2;

    pSendReqState->numCmds++;

    pSendReqState->pCurHdrElem++;

    if ((uint8_t*)pSendReqState->pCurHdrElem < pSendReqState->pFirstHdrElem->pPtr)
    {
        pSendReqState->pCurHdrElem->pPtr   = NULL;
        pSendReqState->pCurHdrElem->length = 0;
    }
    else
    {
        pSendReqState->pCurHdrElem = NULL;
    }

    return true;
}

/*****************************************************************************
  Description:
    Extract a parameter element.

  Parameters:
    pElem   - Pointer to element structure
    typeVal - Type of value to extract
    pVal    - Pointer to buffer to receive value
    lenVal  - Length of buffer to receive value

  Returns:
    Length of value written to output buffer, or 0 for error.

  Remarks:

 *****************************************************************************/

size_t WINC_CmdReadParamElem(WINC_DEV_PARAM_ELEM *pElem, WINC_TYPE typeVal, void *pVal, size_t lenVal)
{
    size_t retVal = lenVal;
    uint8_t* pDstVal = pVal;

    if ((NULL == pElem) || (NULL == pVal))
    {
        return 0;
    }

    if (true == cmdIsInt(pElem->type))
    {
        uint8_t signByte = 0x00;
        uint8_t *pSrcVal;
        uint16_t lenSrcVal;
        uint32_t localUint;

        /* If type is a signed integer then prepare for sign extension. */
        if (WINC_TYPE_INTEGER == pElem->type)
        {
            if (0 != (pElem->pData[0] & 0x80))
            {
                signByte = 0xff;
            }
        }

        if (WINC_TYPE_INTEGER_FRAC == typeVal)
        {
            pDstVal = (uint8_t*)&localUint;
        }

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        lenSrcVal = pElem->length;
        pSrcVal   = &pElem->pData[pElem->length-1];

        /* Copy integer in reverse, big to little endian. */
        while ((lenVal > 0) && (lenSrcVal > 0))
        {
            *pDstVal++ = *pSrcVal--;
            lenVal--;
            lenSrcVal--;
        }

        /* Pad remaining length of destination with sign byte. */
        while (lenVal > 0)
        {
            *pDstVal++ = signByte;
            lenVal--;
        }
#else
#endif
        if (WINC_TYPE_INTEGER_FRAC == typeVal)
        {
            WINC_DEV_FRACT_INT_TYPE *pFIVal = pVal;

            if (WINC_TYPE_INTEGER_FRAC == pElem->type)
            {
                pFIVal->i = (localUint >> 16) & 0xFFFF;
                pFIVal->f = localUint & 0xFFFF;
            }
            else if (localUint <= 0xFFFF)
            {
                pFIVal->i = localUint;
                pFIVal->f = -1;
            }
            else
            {
                return 0;
            }
        }
    }
    else if (WINC_TYPE_STRING == pElem->type)
    {
        if (lenVal < pElem->length)
        {
            return 0;
        }
        else if (lenVal > pElem->length)
        {
            memset(&pDstVal[pElem->length], 0, lenVal-pElem->length);
        }

        memcpy(pDstVal, pElem->pData, pElem->length);

        retVal = pElem->length;
    }
    else
    {
        if (lenVal != pElem->length)
        {
            return 0;
        }

        memcpy(pDstVal, pElem->pData, lenVal);
    }

    return retVal;
}

/*****************************************************************************
 *****************************************************************************/

/*****************************************************************************
   Description:
     This command requests manufacturer identification

   Parameters:
     handle - Command request session handle

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdGMI(WINC_CMD_REQ_HANDLE handle)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_GMI);

    if (NULL == pCmdReq)
    {
        return false;
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command requests model identification

   Parameters:
     handle - Command request session handle

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdGMM(WINC_CMD_REQ_HANDLE handle)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_GMM);

    if (NULL == pCmdReq)
    {
        return false;
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command requests revision identification

   Parameters:
     handle - Command request session handle

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdGMR(WINC_CMD_REQ_HANDLE handle)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_GMR);

    if (NULL == pCmdReq)
    {
        return false;
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command sets the DTE serial port baud rate

   Parameters:
     handle      - Command request session handle
     optBaudRate - Baud rate

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdIPR(WINC_CMD_REQ_HANDLE handle, uint32_t optBaudRate)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_IPR);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optBaudRate)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optBaudRate, 4);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the system configuration

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdCFG(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_CFG);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the DHCP server configuration

   Parameters:
     handle     - Command request session handle
     optIdx     - Pool index
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdDHCPSC(WINC_CMD_REQ_HANDLE handle, int32_t optIdx, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optIdx < -1) || (optIdx > 0))
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_DHCPSC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (-1 != optIdx)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optIdx, 4);
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the DNS configuration

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdDNSC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_DNSC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to resolve domain names via DNS

   Parameters:
     handle        - Command request session handle
     type          - Type of record
     pDomainName   - Domain name to resolve
     lenDomainName - Length of pDomainName

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdDNSRESOLV(WINC_CMD_REQ_HANDLE handle, uint8_t type, const uint8_t* pDomainName, size_t lenDomainName)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((1 != type) && (28 != type))
    {
        return false;
    }

    if ((NULL == pDomainName) || (lenDomainName > 128))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_DNSRESOLV);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &type, 1);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pDomainName, lenDomainName);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command performs a filesystem operation

   Parameters:
     handle         - Command request session handle
     op             - Operation
     optFiletype    - File type
     pOptFilename   - The name of the file
     lenOptFilename - Length of pOptFilename

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdFSOP(WINC_CMD_REQ_HANDLE handle, uint8_t op, uint8_t optFiletype, const uint8_t* pOptFilename, size_t lenOptFilename)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((op < 1) || (op > 4))
    {
        return false;
    }

    if (optFiletype > 2)
    {
        return false;
    }

    if ((NULL != pOptFilename) && (lenOptFilename > 32))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_FS);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &op, 1);

    if (0 != optFiletype)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optFiletype, 1);
    }

    if (NULL != pOptFilename)
    {
        cmdParamBuilder(handle, WINC_TYPE_STRING, pOptFilename, lenOptFilename);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command performs a filesystem operation

   Parameters:
     handle        - Command request session handle
     filetype      - File type
     tsfrprot      - Transfer protocol
     pFilename     - The name of the file
     lenFilename   - Length of pFilename
     optFilelength - File length

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdFSLOAD(WINC_CMD_REQ_HANDLE handle, uint8_t filetype, uint8_t tsfrprot, const uint8_t* pFilename, size_t lenFilename, uint16_t optFilelength)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    const uint8_t op = 1;
    if (0 == handle)
    {
        return false;
    }

    if ((filetype < 1) || (filetype > 2))
    {
        return false;
    }

    if ((tsfrprot < 1) || (tsfrprot > 5))
    {
        return false;
    }

    if ((NULL == pFilename) || (lenFilename > 32))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_FS);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &op, 1);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &filetype, 1);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &tsfrprot, 1);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pFilename, lenFilename);

    if (0 != optFilelength)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optFilelength, 2);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command performs a filesystem transfer operation

   Parameters:
     handle        - Command request session handle
     optTsfrHandle - Transfer handle
     optBlockNum   - Block number
     pOptData      - Transfer data
     lenOptData    - Length of pOptData
     optCrc        - Transfer CRC-16

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdFSTSFR(WINC_CMD_REQ_HANDLE handle, uint16_t optTsfrHandle, uint8_t optBlockNum, const uint8_t* pOptData, size_t lenOptData, int32_t optCrc)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optCrc < -1) || (optCrc > 65535))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_FSTSFR);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optTsfrHandle)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optTsfrHandle, 2);
    }

    if (0 != optBlockNum)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optBlockNum, 1);
    }

    if (NULL != pOptData)
    {
        cmdParamBuilder(handle, WINC_TYPE_BYTE_ARRAY, pOptData, lenOptData);
    }

    if (-1 != optCrc)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optCrc, 4);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the MQTT configuration

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used to connect to an MQTT broker

   Parameters:
     handle   - Command request session handle
     optClean - Clean Session

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTCONN(WINC_CMD_REQ_HANDLE handle, int32_t optClean)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optClean < -1) || (optClean > 1))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTCONN);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (-1 != optClean)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optClean, 4);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used to subscribe to an MQTT topic

   Parameters:
     handle       - Command request session handle
     pTopicName   - Topic Name
     lenTopicName - Length of pTopicName
     maxQos       - Maximum QoS

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTSUB(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName, uint8_t maxQos)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (NULL == pTopicName)
    {
        return false;
    }

    if (maxQos > 2)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTSUB);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_STRING, pTopicName, lenTopicName);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &maxQos, 1);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used to list MQTT topic subscriptions

   Parameters:
     handle - Command request session handle

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTSUBLST(WINC_CMD_REQ_HANDLE handle)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTSUBLST);

    if (NULL == pCmdReq)
    {
        return false;
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used receive data from subscriptions

   Parameters:
     handle       - Command request session handle
     pTopicName   - Topic Name
     lenTopicName - Length of pTopicName
     msgId        - Message Identifier
     length       - The number of bytes to receive

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTSUBRD(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName, uint16_t msgId, uint16_t length)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (NULL == pTopicName)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTSUBRD);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_STRING, pTopicName, lenTopicName);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &msgId, 2);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &length, 2);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used to unsubscribe from an MQTT topic

   Parameters:
     handle       - Command request session handle
     pTopicName   - Topic Name
     lenTopicName - Length of pTopicName

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTUNSUB(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (NULL == pTopicName)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTUNSUB);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_STRING, pTopicName, lenTopicName);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used to publish a message

   Parameters:
     handle          - Command request session handle
     dup             - Duplicate Message
     qos             - QoS
     retain          - Retain Message
     pTopicName      - Topic Name
     lenTopicName    - Length of pTopicName
     pTopicPayload   - Topic Payload
     lenTopicPayload - Length of pTopicPayload

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTPUB(WINC_CMD_REQ_HANDLE handle, uint8_t dup, uint8_t qos, uint8_t retain, const uint8_t* pTopicName, size_t lenTopicName, const uint8_t* pTopicPayload, size_t lenTopicPayload)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (dup > 1)
    {
        return false;
    }

    if (qos > 2)
    {
        return false;
    }

    if (retain > 1)
    {
        return false;
    }

    if (NULL == pTopicName)
    {
        return false;
    }

    if (NULL == pTopicPayload)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTPUB);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &dup, 1);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &qos, 1);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &retain, 1);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pTopicName, lenTopicName);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pTopicPayload, lenTopicPayload);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used to define a last will message

   Parameters:
     handle          - Command request session handle
     qos             - QoS
     retain          - Retain Message
     pTopicName      - Topic Name
     lenTopicName    - Length of pTopicName
     pTopicPayload   - Topic Payload
     lenTopicPayload - Length of pTopicPayload

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTLWT(WINC_CMD_REQ_HANDLE handle, uint8_t qos, uint8_t retain, const uint8_t* pTopicName, size_t lenTopicName, const uint8_t* pTopicPayload, size_t lenTopicPayload)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (qos > 2)
    {
        return false;
    }

    if (retain > 1)
    {
        return false;
    }

    if (NULL == pTopicName)
    {
        return false;
    }

    if (NULL == pTopicPayload)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTLWT);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &qos, 1);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &retain, 1);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pTopicName, lenTopicName);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pTopicPayload, lenTopicPayload);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used to disconnect from a broker

   Parameters:
     handle        - Command request session handle
     optReasonCode - Reason Code

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTDISCONN(WINC_CMD_REQ_HANDLE handle, int32_t optReasonCode)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (((optReasonCode < -1) || (optReasonCode > 0)) && (4 != optReasonCode) && ((optReasonCode < 128) || (optReasonCode > 131)) && (144 != optReasonCode) && ((optReasonCode < 147) || (optReasonCode > 153)))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTDISCONN);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (-1 != optReasonCode)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optReasonCode, 4);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the MQTT transmit properties

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTPROPTX(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTPROPTX);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the MQTT transmit properties

   Parameters:
     handle  - Command request session handle
     pKey    - Parameter key
     lenKey  - Length of pKey
     pVals   - Parameter value
     lenVals - Length of pVals

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTPROPTXKV(WINC_CMD_REQ_HANDLE handle, const uint8_t* pKey, size_t lenKey, const uint8_t* pVals, size_t lenVals)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    const uint8_t id = 38;
    if (0 == handle)
    {
        return false;
    }

    if (NULL == pKey)
    {
        return false;
    }

    if (NULL == pVals)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTPROPTX);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &id, 1);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pKey, lenKey);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pVals, lenVals);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read the MQTT receive properties

   Parameters:
     handle - Command request session handle
     optId  - Parameter ID number

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTPROPRX(WINC_CMD_REQ_HANDLE handle, uint32_t optId)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTPROPRX);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This is used to define which transmit properties are selected

   Parameters:
     handle     - Command request session handle
     setPropId  - Flag indicating if optPropId is set
     optPropId  - Property Identifier
     optPropSel - Property Selected

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdMQTTPROPTXS(WINC_CMD_REQ_HANDLE handle, bool setPropId, int32_t optPropId, int32_t optPropSel)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optPropSel < -1) || (optPropSel > 1))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_MQTTPROPTXS);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (true == setPropId)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optPropId, 4);
    }

    if (-1 != optPropSel)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optPropSel, 4);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the network interface configuration

   Parameters:
     handle     - Command request session handle
     optIf      - Interface number
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdNETIFC(WINC_CMD_REQ_HANDLE handle, int32_t optIf, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optIf < -1) || (optIf > 1))
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_NETIFC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (-1 != optIf)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optIf, 4);
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command sends a ping (ICMP Echo Request) to the target address

   Parameters:
     handle             - Command request session handle
     typeTargetAddr     - Type of targetAddr
     targetAddr         - IP address or host name of target
     lenTargetAddr      - Length of targetAddr
     optProtocolVersion - IP protocol version

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdPING(WINC_CMD_REQ_HANDLE handle, WINC_TYPE typeTargetAddr, uintptr_t targetAddr, size_t lenTargetAddr, uint8_t optProtocolVersion)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((0 != optProtocolVersion) && (4 != optProtocolVersion) && (6 != optProtocolVersion))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_PING);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdMultiValueBuilder(handle, typeTargetAddr, targetAddr, lenTargetAddr);

    if (0 != optProtocolVersion)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optProtocolVersion, 1);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to reset the DCE

   Parameters:
     handle - Command request session handle

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdRST(WINC_CMD_REQ_HANDLE handle)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_RST);

    if (NULL == pCmdReq)
    {
        return false;
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the SNTP configuration

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSNTPC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SNTPC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to open a new socket

   Parameters:
     handle             - Command request session handle
     protocol           - The protocol to use
     optProtocolVersion - IP protocol version

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKO(WINC_CMD_REQ_HANDLE handle, uint8_t protocol, uint8_t optProtocolVersion)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((protocol < 1) || (protocol > 2))
    {
        return false;
    }

    if ((0 != optProtocolVersion) && (4 != optProtocolVersion) && (6 != optProtocolVersion))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKO);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &protocol, 1);

    if (0 != optProtocolVersion)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optProtocolVersion, 1);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to bind a socket to a local port

   Parameters:
     handle      - Command request session handle
     sockId      - The socket ID
     lclPort     - The local port number to use
     optPendSkts - Number of pending sockets connections

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKBL(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint16_t lclPort, uint8_t optPendSkts)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (optPendSkts > 5)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKBL);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &lclPort, 2);

    if (0 != optPendSkts)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optPendSkts, 1);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to bind a socket to a remote address

   Parameters:
     handle      - Command request session handle
     sockId      - The socket ID
     typeRmtAddr - Type of rmtAddr
     rmtAddr     - The address of the remote device
     lenRmtAddr  - Length of rmtAddr
     rmtPort     - The port number on the remote device

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKBR(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeRmtAddr, uintptr_t rmtAddr, size_t lenRmtAddr, uint16_t rmtPort)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKBR);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);
    cmdMultiValueBuilder(handle, typeRmtAddr, rmtAddr, lenRmtAddr);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &rmtPort, 2);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to bind a socket to a multicast group

   Parameters:
     handle        - Command request session handle
     sockId        - The socket ID
     typeMcastAddr - Type of mcastAddr
     mcastAddr     - The address of the multicast group
     lenMcastAddr  - Length of mcastAddr
     mcastPort     - The port number of the multicast group

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKBM(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeMcastAddr, uintptr_t mcastAddr, size_t lenMcastAddr, uint16_t mcastPort)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKBM);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);
    cmdMultiValueBuilder(handle, typeMcastAddr, mcastAddr, lenMcastAddr);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &mcastPort, 2);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to enable TLS on a socket

   Parameters:
     handle  - Command request session handle
     sockId  - The socket ID
     tlsConf - TLS certificate configuration

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKTLS(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t tlsConf)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (tlsConf > 2)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKTLS);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &tlsConf, 1);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to send data over a socket that is bound to a remote
       address and port number

   Parameters:
     handle     - Command request session handle
     sockId     - The socket ID
     length     - The length of the data to send
     optSeqNum  - Sequence number of first byte
     pOptData   - The data to send in either ASCII or hexadecimal string
                    format. If omitted the DCE will enter raw binary mode and
                    will remain in that mode until the specified length of
                    binary data has been received from the DTE
     lenOptData - Length of pOptData

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKWR(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint16_t length, int32_t optSeqNum, const uint8_t* pOptData, size_t lenOptData)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (length > 1460)
    {
        return false;
    }

    if ((optSeqNum < -1) || (optSeqNum > 65535))
    {
        return false;
    }

    if ((NULL != pOptData) && (lenOptData > 1460))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKWR);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &length, 2);

    if (-1 != optSeqNum)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optSeqNum, 4);
    }

    if (NULL != pOptData)
    {
        cmdParamBuilder(handle, WINC_TYPE_BYTE_ARRAY, pOptData, lenOptData);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to send data to an arbitrary destination using the
       connectionless UDP protocol

   Parameters:
     handle      - Command request session handle
     sockId      - The socket ID
     typeRmtAddr - Type of rmtAddr
     rmtAddr     - The address of the remote device
     lenRmtAddr  - Length of rmtAddr
     rmtPort     - The port number on the remote device
     length      - The length of the data to send
     optSeqNum   - Sequence number of first byte
     pOptData    - The data to send in either ASCII or hexadecimal string
                     format. If omitted the DCE will enter raw binary mode and
                     will remain in that mode until the specified length of
                     binary data has been received from the DTE
     lenOptData  - Length of pOptData

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKWRTO(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeRmtAddr, uintptr_t rmtAddr, size_t lenRmtAddr, uint16_t rmtPort, uint16_t length, int32_t optSeqNum, const uint8_t* pOptData, size_t lenOptData)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (length > 1472)
    {
        return false;
    }

    if ((optSeqNum < -1) || (optSeqNum > 65535))
    {
        return false;
    }

    if ((NULL != pOptData) && (lenOptData > 1472))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKWRTO);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);
    cmdMultiValueBuilder(handle, typeRmtAddr, rmtAddr, lenRmtAddr);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &rmtPort, 2);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &length, 2);

    if (-1 != optSeqNum)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optSeqNum, 4);
    }

    if (NULL != pOptData)
    {
        cmdParamBuilder(handle, WINC_TYPE_BYTE_ARRAY, pOptData, lenOptData);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read data from a socket

   Parameters:
     handle     - Command request session handle
     sockId     - The socket ID
     outputMode - The format the DTE wishes to receive the data
     length     - The number of bytes the DTE wishes to read

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKRD(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t outputMode, int32_t length)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((outputMode < 1) || (outputMode > 3))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKRD);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &outputMode, 1);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER, &length, 4);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read data from a socket buffer

   Parameters:
     handle     - Command request session handle
     sockId     - The socket ID
     outputMode - The format the DTE wishes to receive the data
     length     - The number of bytes the DTE wishes to read

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKRDBUF(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t outputMode, int32_t length)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((outputMode < 1) || (outputMode > 3))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKRDBUF);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &outputMode, 1);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER, &length, 4);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to close a socket

   Parameters:
     handle - Command request session handle
     sockId - The socket ID

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKCL(WINC_CMD_REQ_HANDLE handle, uint16_t sockId)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKCL);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &sockId, 2);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to present a list of the DCE's open
       sockets/connections

   Parameters:
     handle    - Command request session handle
     optSockId - The socket ID

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKLST(WINC_CMD_REQ_HANDLE handle, uint16_t optSockId)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKLST);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optSockId)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optSockId, 2);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the socket configuration

   Parameters:
     handle     - Command request session handle
     optSockId  - The socket ID
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSOCKC(WINC_CMD_REQ_HANDLE handle, uint16_t optSockId, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SOCKC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optSockId)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optSockId, 2);
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to set or query the system time

   Parameters:
     handle    - Command request session handle
     optFormat - Format of time

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdTIME(WINC_CMD_REQ_HANDLE handle, uint8_t optFormat)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (optFormat > 3)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_TIME);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optFormat)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optFormat, 1);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to set or query the system time

   Parameters:
     handle - Command request session handle
     format - Format of time
     utcSec - UTC seconds

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdTIMEUTCSEC(WINC_CMD_REQ_HANDLE handle, uint8_t format, uint32_t utcSec)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((format < 1) || (format > 3))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_TIME);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &format, 1);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &utcSec, 4);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to set or query the system time

   Parameters:
     handle      - Command request session handle
     pDateTime   - Date/time in format YYYY-MM-DDTHH:MM:SS.00Z
     lenDateTime - Length of pDateTime

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdTIMERFC(WINC_CMD_REQ_HANDLE handle, const uint8_t* pDateTime, size_t lenDateTime)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    const uint8_t format = 2;
    if (0 == handle)
    {
        return false;
    }

    if (NULL == pDateTime)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_TIME);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &format, 1);
    cmdParamBuilder(handle, WINC_TYPE_STRING, pDateTime, lenDateTime);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the TLS configuration

   Parameters:
     handle     - Command request session handle
     optConf    - Configuration number
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdTLSC(WINC_CMD_REQ_HANDLE handle, uint8_t optConf, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (optConf > 2)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_TLSC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optConf)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optConf, 1);
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the TLS cipher suite configuration

   Parameters:
     handle     - Command request session handle
     optCslIdx  - Cipher suite list index
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdTLSCSC(WINC_CMD_REQ_HANDLE handle, uint8_t optCslIdx, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (optCslIdx > 2)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_TLSCSC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optCslIdx)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optCslIdx, 1);
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the DCE's hotspot access point
       configuration

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWAPC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WAPC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to enable the DCE's hotspot access point
       functionality

   Parameters:
     handle   - Command request session handle
     optState - State of the hotspot feature

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWAP(WINC_CMD_REQ_HANDLE handle, int32_t optState)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optState < -1) || (optState > 1))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WAP);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (-1 != optState)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optState, 4);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to modify or query the behavior of the active
       scanning function

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWSCNC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WSCNC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to scan for infrastructure networks in range of the
       DCE

   Parameters:
     handle  - Command request session handle
     actPasv - Flag indicating active or passive scanning

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWSCN(WINC_CMD_REQ_HANDLE handle, uint8_t actPasv)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (actPasv > 1)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WSCN);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &actPasv, 1);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the DCE's Wi-Fi station mode
       configuration

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWSTAC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WSTAC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to control or query the DCE's station mode
       functionality

   Parameters:
     handle   - Command request session handle
     optState - State of the Wi-Fi station feature

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWSTA(WINC_CMD_REQ_HANDLE handle, int32_t optState)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optState < -1) || (optState > 1))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WSTA);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (-1 != optState)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optState, 4);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to query current WiFi associations

   Parameters:
     handle     - Command request session handle
     optAssocId - Association ID

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdASSOC(WINC_CMD_REQ_HANDLE handle, uint16_t optAssocId)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_ASSOC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optAssocId)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optAssocId, 2);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to query system information

   Parameters:
     handle    - Command request session handle
     optFilter - System information filter bitmask

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdSI(WINC_CMD_REQ_HANDLE handle, uint8_t optFilter)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optFilter > 2) && (4 != optFilter) && (8 != optFilter))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_SI);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optFilter)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &optFilter, 1);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the provisioning service
       configuration

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWPROVC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WPROVC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to control or query the provisioning service

   Parameters:
     handle   - Command request session handle
     optState - State of the provisioning service

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWPROV(WINC_CMD_REQ_HANDLE handle, int32_t optState)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if ((optState < -1) || (optState > 1))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WPROV);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (-1 != optState)
    {
        cmdParamBuilder(handle, WINC_TYPE_INTEGER, &optState, 4);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     Query the device information

   Parameters:
     handle - Command request session handle
     optId  - Parameter ID number

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdDI(WINC_CMD_REQ_HANDLE handle, uint32_t optId)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_DI);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to provide the result of an external crypto
       operation

   Parameters:
     handle          - Command request session handle
     opId            - Operation identifier, matching <OP_ID> in corresponding
                         AEC
     status          - Operation success or failure
     pOptSignature   - Signature (big endian). For ECDSA signatures: R then S,
                         each the size of the curve.
     lenOptSignature - Length of pOptSignature

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdEXTCRYPTO(WINC_CMD_REQ_HANDLE handle, uint16_t opId, uint8_t status, const uint8_t* pOptSignature, size_t lenOptSignature)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (status > 1)
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_EXTCRYPTO);

    if (NULL == pCmdReq)
    {
        return false;
    }

    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &opId, 2);
    cmdParamBuilder(handle, WINC_TYPE_INTEGER_UNSIGNED, &status, 1);

    if (NULL != pOptSignature)
    {
        cmdParamBuilder(handle, WINC_TYPE_BYTE_ARRAY, pOptSignature, lenOptSignature);
    }

    return cmdCompleteCommandReq(handle);
}

/*****************************************************************************
   Description:
     This command is used to read or set the device's Wi-Fi configuration

   Parameters:
     handle     - Command request session handle
     optId      - Parameter ID number
     typeOptVal - Type of optVal
     optVal     - Parameter value
     lenOptVal  - Length of optVal

   Returns:
     true or false indicating success or failure.

   Remarks:
     None.

 *****************************************************************************/

bool WINC_CmdWIFIC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal)
{
    WINC_COMMAND_REQUEST *pCmdReq;

    if (0 == handle)
    {
        return false;
    }

    if (false == cmdCheckFractType(optId, 255))
    {
        return false;
    }

    pCmdReq = cmdBuildCommandReq(handle, WINC_CMD_ID_WIFIC);

    if (NULL == pCmdReq)
    {
        return false;
    }

    if (0 != optId)
    {
        cmdFractValueBuilder(handle, optId);
    }

    cmdMultiValueBuilder(handle, typeOptVal, optVal, lenOptVal);

    return cmdCompleteCommandReq(handle);
}
