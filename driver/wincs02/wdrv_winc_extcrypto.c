/*******************************************************************************
  WINC Wireless Driver External Crypto Source File

  File Name:
    wdrv_winc_extcrypto.c

  Summary:
    WINC wireless driver external crypto implementation.

  Description:
    This interface manages crypto operations which are performed externally,
    from the perspective of the WINC IC.
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
#include "wdrv_winc_extcrypto.h"
#include "wdrv_winc_tls.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver External Crypto Internal Implementation
// *****************************************************************************
// *****************************************************************************

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
    static void extcryptoProcessAEC
    (
        WDRV_WINC_DCPT *pDcpt,
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

static void extcryptoProcessAEC
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t aecId,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (aecId)
    {
        case WINC_AEC_ID_EXTCRYPTO:
        {
            uint16_t extCryptoCxt;
            uint8_t opType;

            if (numElems < 2U)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER_UNSIGNED, &extCryptoCxt, sizeof(extCryptoCxt));
            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER_UNSIGNED, &opType, sizeof(opType));

            switch (opType)
            {
                case WINC_CONST_EXTCRYPTO_OP_TYPE_SIGN:
                {
                    uint8_t opSrcType;
                    uint8_t opSrcId;
                    uint8_t signType;
                    uint8_t curveId;
                    uint8_t *pSignValue;

                    if (7U != numElems)
                    {
                        break;
                    }

                    pSignValue = OSAL_Malloc(pElems[6].length);

                    if (NULL == pSignValue)
                    {
                        break;
                    }

                    (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER_UNSIGNED, &opSrcType, sizeof(opSrcType));
                    (void)WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_INTEGER_UNSIGNED, &opSrcId, sizeof(opSrcId));
                    (void)WINC_CmdReadParamElem(&pElems[4], WINC_TYPE_INTEGER_UNSIGNED, &signType, sizeof(signType));
                    (void)WINC_CmdReadParamElem(&pElems[5], WINC_TYPE_INTEGER_UNSIGNED, &curveId, sizeof(curveId));
                    (void)WINC_CmdReadParamElem(&pElems[6], WINC_TYPE_BYTE_ARRAY, pSignValue, pElems[6].length);

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
                                            (uintptr_t)pDcpt,
                                            pDcpt->pCtrl->tlscInfo[opSrcId-1U].signCbCtx,
                                            signAlgo,
                                            pSignValue, pElems[6].length,
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
                    WDRV_DBG_VERBOSE_PRINT("EXTCRYPTO AECCB Op type %d not handled\r\n", opType);
                    break;
                }
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
    static void extcryptoCmdRspCallbackHandler
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

static void extcryptoCmdRspCallbackHandler
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
            /* Do nothing. */
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            /* Do nothing. */
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
// Section: WINC Driver External Crypto Implementation
// *****************************************************************************
// *****************************************************************************

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

    if ((NULL == pDcpt) || (NULL == pElems))
    {
        return;
    }

    extcryptoProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, lenSignature, extcryptoCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdEXTCRYPTO(cmdReqHandle, (uint16_t)extCryptoCxt, status ? WINC_CONST_EXTCRYPTO_STATUS_SUCCESS : WINC_CONST_EXTCRYPTO_STATUS_FAILURE, pSignature, lenSignature);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
