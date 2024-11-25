/*******************************************************************************
  WINC Driver External Crypto Implementation

  File Name:
    wdrv_winc_extcrypto.c

  Summary:
    WINC wireless driver external crypto implementation.

  Description:
    This interface manages crypto operations which are performed externally,
    from the perspective of the WINC IC.
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
#include "wdrv_winc_extcrypto.h"
#include "wdrv_winc_tls.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver External Crypto Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void extCryptoEXTCRYPTOCmdRspCallbackHandler
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

static void extCryptoEXTCRYPTOCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
)
{
    const WDRV_WINC_DCPT *pDcpt = (const WDRV_WINC_DCPT*)context;

    if (NULL == pDcpt)
    {
        return;
    }

//    WDRV_DBG_INFORM_PRINT("EXTCRYPTO CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("EXTCRYPTO CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }
}

#ifndef WDRV_WINC_MOD_DISABLE_TLS
//*******************************************************************************
/*
  Function:
    static WDRV_WINC_EXTCRYPTO_SIG_ALGO extCryptoEcdsaCurveToSigAlgo
    (
        uint8_t curveId
    )

  Summary:
    Convert from ECDSA curve identifier to signature algorithm.

  Description:
    Converts the ECDSA curve identifier in received AEC messages to the
    corresponding signature algorithm.

  Parameters:
    curveId - ECDSA curve identifier in received AEC message.

  Returns:
    Signature algorithm.

  Remarks:
    None.

*/

static WDRV_WINC_EXTCRYPTO_SIG_ALGO extCryptoEcdsaCurveToSigAlgo(uint8_t curveId)
{
    WDRV_WINC_EXTCRYPTO_SIG_ALGO sigAlg = WDRV_WINC_EXTCRYPTO_SIG_ALGO_INVALID;

    switch (curveId)
    {
        case WINC_CONST_EXTCRYPTO_CURVE_SECP256R1:
        {
            sigAlg = WDRV_WINC_EXTCRYPTO_SIG_ALGO_ECDSA_SECP256R1;
            break;
        }

        case WINC_CONST_EXTCRYPTO_CURVE_SECP384R1:
        {
            sigAlg = WDRV_WINC_EXTCRYPTO_SIG_ALGO_ECDSA_SECP384R1;
            break;
        }

        case WINC_CONST_EXTCRYPTO_CURVE_SECP521R1:
        {
            sigAlg = WDRV_WINC_EXTCRYPTO_SIG_ALGO_ECDSA_SECP521R1;
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("ECDSA curve ID %d not handled\r\n", curveId);
            break;
        }
    }

    return sigAlg;
}
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_EXTCRYPTOProcessAEC
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
    See wdrv_winc_extcrypto.h for usage information.

*/

void WDRV_WINC_EXTCRYPTOProcessAEC
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
        case WINC_AEC_ID_EXTCRYPTO:
        {
            uint16_t extCryptoCxt;
            uint8_t opType;

            if (pElems->numElems < 2U)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_UNSIGNED, &extCryptoCxt, sizeof(extCryptoCxt));
            (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &opType, sizeof(opType));

            switch (opType)
            {
                case WINC_CONST_EXTCRYPTO_OP_TYPE_SIGN:
                {
                    uint8_t opSrcType;
                    uint8_t opSrcId;
                    uint8_t signType;
                    uint8_t curveId;
                    uint8_t *pSignValue;

                    if (7U != pElems->numElems)
                    {
                        break;
                    }

                    pSignValue = OSAL_Malloc(pElems->elems[6].length);

                    if (NULL == pSignValue)
                    {
                        break;
                    }

                    (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER_UNSIGNED, &opSrcType, sizeof(opSrcType));
                    (void)WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_INTEGER_UNSIGNED, &opSrcId, sizeof(opSrcId));
                    (void)WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_INTEGER_UNSIGNED, &signType, sizeof(signType));
                    (void)WINC_CmdReadParamElem(&pElems->elems[5], WINC_TYPE_INTEGER_UNSIGNED, &curveId, sizeof(curveId));
                    (void)WINC_CmdReadParamElem(&pElems->elems[6], WINC_TYPE_BYTE_ARRAY, pSignValue, pElems->elems[6].length);

#ifndef WDRV_WINC_MOD_DISABLE_TLS
                    if (WINC_CONST_EXTCRYPTO_OP_SOURCE_TYPE_TLSC == opSrcType)
                    {
                        if ((WDRV_WINC_TLS_INVALID_HANDLE != opSrcId) && (opSrcId <= WDRV_WINC_TLS_CTX_NUM))
                        {
                            if (NULL != pDcpt->pCtrl->tlscInfo[opSrcId-1U].pfSignCB)
                            {
                                if (WINC_CONST_EXTCRYPTO_SIGN_TYPE_ECDSA == signType)
                                {
                                    WDRV_WINC_EXTCRYPTO_SIG_ALGO signAlgo = extCryptoEcdsaCurveToSigAlgo(curveId);

                                    if (WDRV_WINC_EXTCRYPTO_SIG_ALGO_INVALID != signAlgo)
                                    {
                                        pDcpt->pCtrl->tlscInfo[opSrcId-1U].pfSignCB(
                                                context,
                                                pDcpt->pCtrl->tlscInfo[opSrcId-1U].signCbCtx,
                                                signAlgo,
                                                pSignValue, pElems->elems[6].length,
                                                extCryptoCxt);
                                    }
                                }
                            }
                        }
                    }
#endif

                    OSAL_Free(pSignValue);
                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("EXTCRYPTO AECCB Op type &d not handled\r\n", opType);
                    break;
                }
            }

            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("EXTCRYPTO AECCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_EXTCRYPTOSignResult
    (
        DRV_HANDLE handle,
        uintptr_t extCryptoCxt,
        bool status,
        const uint8_t *const pSignature,
        size_t lenSignature
    );

  Summary:
    Provide the result of an external signing operation

  Description:
    Provide the signature for a signing operation which has been performed
    externally, from the perspective of the WINC IC.

  Remarks:
    See wdrv_winc_extcrypto.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_EXTCRYPTOSignResult
(
        DRV_HANDLE handle,
        uintptr_t extCryptoCxt,
        bool status,
        const uint8_t *const pSignature,
        size_t lenSignature
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((true == status) && (NULL == pSignature))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (0xffffU < extCryptoCxt)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, lenSignature, extCryptoEXTCRYPTOCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdEXTCRYPTO(cmdReqHandle, (uint16_t)extCryptoCxt, status ? WINC_CONST_EXTCRYPTO_STATUS_SUCCESS : WINC_CONST_EXTCRYPTO_STATUS_FAILURE, pSignature, lenSignature);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
