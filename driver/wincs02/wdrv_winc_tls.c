/*******************************************************************************
  WINC Wireless Driver TLS Context Source File

  File Name:
    wdrv_winc_tls.c

  Summary:
    WINC wireless driver TLS context implementation.

  Description:
    This interface manages the TLS contexts.
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
#include "wdrv_winc_tls.h"

#ifndef WDRV_WINC_MOD_DISABLE_TLS

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver TLS Context Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void tlsProcessCmdReqStatus
    (
        WDRV_WINC_DCPT *pDcpt,
        WINC_CMD_REQ_HANDLE cmdReqHandle,
    )

  Summary:
    Process command request status responses.

  Description:
    Processes command request status responses received via WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE events.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    pDcpt        - Pointer to device descriptor.
    cmdReqHandle - Command request handle.

  Returns:
    None.

  Remarks:
    None.

*/

static void tlsProcessCmdReqStatus
(
    WDRV_WINC_DCPT *pDcpt,
    WINC_CMD_REQ_HANDLE cmdReqHandle
)
{
    unsigned int i;

    if (NULL == pDcpt)
    {
        return;
    }

    for (i=0; i<WDRV_WINC_TLS_CIPHER_SUITE_NUM; i++)
    {
        if (true == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].idxInUse)
        {
            if (cmdReqHandle == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].setCmdReqHandle)
            {
                pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].setCmdReqHandle = WINC_CMD_REQ_INVALID_HANDLE;

                if (NULL != pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB)
                {
                    pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB((DRV_HANDLE)pDcpt, i+1U, NULL, 0, true);
                }

                pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB = NULL;
            }
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void tlsProcessStatus
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

static void tlsProcessStatus
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t cmdID,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    uint16_t statusCode
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pSrcCmd))
    {
        return;
    }

    switch (cmdID)
    {
        case WINC_CMD_ID_TLSCSC:
        {
            WINC_DEV_PARAM_ELEM elems[10];
            uint16_t tlsCsHandle;

            if (NULL == pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB)
            {
                break;
            }

            if (pSrcCmd->numParams < 1U)
            {
                break;
            }

            if (false == WINC_DevUnpackElements(pSrcCmd->numParams, pSrcCmd->pParams, elems))
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &tlsCsHandle, sizeof(tlsCsHandle));

            if (2U == pSrcCmd->numParams)
            {
                if (WDRV_WINC_STATUS_OK == (WDRV_WINC_STATUS)statusCode)
                {
                    pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB((DRV_HANDLE)pDcpt, tlsCsHandle, pDcpt->pCtrl->tlsCipherSuites.algorithms, pDcpt->pCtrl->tlsCipherSuites.numAlgorithms, true);
                }
                else
                {
                    pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB((DRV_HANDLE)pDcpt, tlsCsHandle, NULL, 0, false);
                }

                pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB = NULL;
            }
            else if (3U == pSrcCmd->numParams)
            {
                if (WDRV_WINC_STATUS_OK != (WDRV_WINC_STATUS)statusCode)
                {
                    uint16_t cs;

                    (void)WINC_CmdReadParamElem(&elems[2], WINC_TYPE_INTEGER, &cs, sizeof(cs));

                    pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB((DRV_HANDLE)pDcpt, tlsCsHandle, &cs, 1, false);
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
    static void tlsProcessCmdRsp
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

static void tlsProcessCmdRsp
(
    WDRV_WINC_DCPT *pDcpt,
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
        case WINC_CMD_ID_TLSCSC:
        {
            WINC_DEV_FRACT_INT_TYPE id;

            if (2U != numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

            if (-1 == id.f)
            {
                (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &pDcpt->pCtrl->tlsCipherSuites.numAlgorithms, sizeof(pDcpt->pCtrl->tlsCipherSuites.numAlgorithms));
            }
            else
            {
                uint16_t cs;

                (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER_UNSIGNED, &cs, sizeof(cs));

                pDcpt->pCtrl->tlsCipherSuites.algorithms[id.f] = cs;
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
    static void tlsCmdRspCallbackHandler
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

static void tlsCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("TLS CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

    switch (event)
    {
        case WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
        {
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
        {
            tlsProcessCmdReqStatus(pDcpt, cmdReqHandle);

            OSAL_Free((WINC_COMMAND_REQUEST*)cmdReqHandle);
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
        {
            /* Status response received for command. */

            const WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (const WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (NULL != pStatusInfo)
            {
                tlsProcessStatus(pDcpt, pStatusInfo->rspCmdId, cmdReqHandle, &pStatusInfo->srcCmd, pStatusInfo->status);
            }

            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                tlsProcessCmdRsp(pDcpt, pRspElems->rspId, cmdReqHandle, &pRspElems->srcCmd, pRspElems->numElems, pRspElems->elems);
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
// Section: WINC Driver TLS Context Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_TLS_HANDLE WDRV_WINC_TLSCtxOpen(DRV_HANDLE handle)

  Summary:
    Open a TLS context.

  Description:
    Obtains a handle to a free TLS context.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_TLS_HANDLE WDRV_WINC_TLSCtxOpen(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    unsigned int i;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_TLS_INVALID_HANDLE;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_TLS_INVALID_HANDLE;
    }

    for (i=0; i<WDRV_WINC_TLS_CTX_NUM; i++)
    {
        if (false == pDcpt->pCtrl->tlscInfo[i].idxInUse)
        {
            pDcpt->pCtrl->tlscInfo[i].idxInUse = true;
            return (uint8_t)i+1U;
        }
    }

    return WDRV_WINC_TLS_INVALID_HANDLE;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_TLSCtxClose(DRV_HANDLE handle, WDRV_WINC_TLS_HANDLE tlsHandle)

  Summary:
    Closes a TLS context.

  Description:
    Frees a TLS context handle for other uses.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

void WDRV_WINC_TLSCtxClose(DRV_HANDLE handle, WDRV_WINC_TLS_HANDLE tlsHandle)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return;
    }

    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM))
    {
        return;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return;
    }

    if (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse)
    {
        return;
    }

    pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse = false;

    return;
}

//*******************************************************************************
/*
  Function:
    uint8_t WDRV_WINC_TLSCtxHandleToCfgIdx(WDRV_WINC_TLS_HANDLE tlsHandle)

  Summary:
    Convert TLS handle to index.

  Description:
    Convert TLS context handle to configuration index.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

uint8_t WDRV_WINC_TLSCtxHandleToCfgIdx(WDRV_WINC_TLS_HANDLE tlsHandle)
{
    if (WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle)
    {
        return 0;
    }

    return (uint8_t)tlsHandle;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxCACertFileSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pCACertName,
        bool peerAuth
    )

  Summary:
    Sets the certificate authority field of a TLS context.

  Description:
    Sets the certificate authority field of a TLS context.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxCACertFileSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pCACertName,
    bool peerAuth
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t lenCACertName;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pCACertName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    lenCACertName = strnlen(pCACertName, WINC_CFG_PARAM_SZ_TLS_CTX_CA_CERT_NAME+1U);

    /* Ensure the CA certificate name is valid */
    if (lenCACertName > WINC_CFG_PARAM_SZ_TLS_CTX_CA_CERT_NAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(2, WINC_CFG_PARAM_SZ_TLS_CTX_CA_CERT_NAME, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PEER_AUTH, WINC_TYPE_BOOL, (true == peerAuth) ? 1U : 0U, 0);

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_CA_CERT_NAME, WINC_TYPE_STRING, (uintptr_t)pCACertName, lenCACertName);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxCertFileSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pCertName
    )

  Summary:
    Sets the certificate field of a TLS context.

  Description:
    Sets the certificate field of a TLS context.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxCertFileSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pCertName
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t lenCertName;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pCertName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    lenCertName = strnlen(pCertName, WINC_CFG_PARAM_SZ_TLS_CTX_CERT_NAME+1U);

    /* Ensure the certificate name is valid */
    if (lenCertName > WINC_CFG_PARAM_SZ_TLS_CTX_CERT_NAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, WINC_CFG_PARAM_SZ_TLS_CTX_CERT_NAME, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_CERT_NAME, WINC_TYPE_STRING, (uintptr_t)pCertName, lenCertName);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxPrivKeySet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pPrivKeyName,
        const char *pPrivKeyPassword
    )

  Summary:
    Sets the private key name and password fields of a TLS context.

  Description:
    Sets the private key name and password fields of a TLS context.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxPrivKeySet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pPrivKeyName,
    const char *pPrivKeyPassword
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t lenPrivKeyName;
    size_t lenPrivKeyPassword = 0;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pPrivKeyName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    lenPrivKeyName = strnlen(pPrivKeyName, WINC_CFG_PARAM_SZ_TLS_CTX_PRI_KEY_NAME+1U);

    /* Ensure the private key name is valid */
    if (lenPrivKeyName > WINC_CFG_PARAM_SZ_TLS_CTX_PRI_KEY_NAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pPrivKeyPassword)
    {
        lenPrivKeyPassword = strnlen(pPrivKeyPassword, WINC_CFG_PARAM_SZ_TLS_CTX_PRI_KEY_PASSWORD+1U);

        /* Ensure the private key password is valid */
        if (lenPrivKeyPassword > WINC_CFG_PARAM_SZ_TLS_CTX_PRI_KEY_PASSWORD)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(2, WINC_CFG_PARAM_SZ_TLS_CTX_PRI_KEY_NAME+WINC_CFG_PARAM_SZ_TLS_CTX_PRI_KEY_PASSWORD, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PRI_KEY_NAME, WINC_TYPE_STRING, (uintptr_t)pPrivKeyName, lenPrivKeyName);

    if (NULL != pPrivKeyPassword)
    {
        (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PRI_KEY_PASSWORD, WINC_TYPE_STRING, (uintptr_t)pPrivKeyPassword, lenPrivKeyPassword);
    }
    else
    {
        (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PRI_KEY_PASSWORD, WINC_TYPE_STRING, (uintptr_t)"", 0);
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxDHParamtersFileSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pDHParamId
    )

  Summary:
    Sets the Diffie-Hellman parameters field of a TLS context.

  Description:
    Sets the Diffie-Hellman parameters field of a TLS context.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxDHParamtersFileSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pDHParamId
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t lenDHParamId;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pDHParamId))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    lenDHParamId = strnlen(pDHParamId, WINC_CFG_PARAM_SZ_TLS_CTX_DH_PARAM_NAME+1U);

    if (lenDHParamId > WINC_CFG_PARAM_SZ_TLS_CTX_DH_PARAM_NAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, WINC_CFG_PARAM_SZ_TLS_CTX_DH_PARAM_NAME, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_DH_PARAM_NAME, WINC_TYPE_STRING, (uintptr_t)pDHParamId, lenDHParamId);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxSNISet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pServerName
    )

  Summary:
    Sets the SNI field of a TLS context.

  Description:
    Sets the SNI field of a TLS context.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxSNISet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pServerName
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t serverNameLen;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pServerName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    serverNameLen = strnlen(pServerName, WINC_CFG_PARAM_SZ_TLS_CTX_SERVER_NAME+1U);

    /* Ensure the server name is valid */
    if (serverNameLen > WINC_CFG_PARAM_SZ_TLS_CTX_SERVER_NAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, WINC_CFG_PARAM_SZ_TLS_CTX_SERVER_NAME, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_SERVER_NAME, WINC_TYPE_STRING, (uintptr_t)pServerName, serverNameLen);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxHostnameCheckSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        const char *pDomainName,
        bool verifyDomain
    )

  Summary:
    Sets the hostname check field of a TLS context.

  Description:
    Sets the hostname check field of a TLS context.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxHostnameCheckSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    const char *pDomainName,
    bool verifyDomain
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t domainNameLen = 0;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure domain name is set if verify is true. */
    if ((true == verifyDomain) && (NULL == pDomainName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pDomainName)
    {
        domainNameLen = strnlen(pDomainName, WINC_CFG_PARAM_SZ_TLS_CTX_DOMAIN_NAME+1U);

        if (domainNameLen > WINC_CFG_PARAM_SZ_TLS_CTX_DOMAIN_NAME)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(2, WINC_CFG_PARAM_SZ_TLS_CTX_DOMAIN_NAME, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PEER_DOMAIN_VERIFY, WINC_TYPE_BOOL, (true == verifyDomain) ? 1U : 0U, 0);

    if (NULL != pDomainName)
    {
        (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_DOMAIN_NAME, WINC_TYPE_STRING, (uintptr_t)pDomainName, domainNameLen);
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxSetSignCallback
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        WDRV_WINC_EXTCRYPTO_SIGN_CB pfSignCb,
        uintptr_t signCbCtx
    )

  Summary:
    Set the signing callback.

  Description:
    Sets the signing callback for external ctyptographic functions.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxSetSignCallback
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    WDRV_WINC_EXTCRYPTO_SIGN_CB pfSignCb,
    uintptr_t signCbCtx
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    uint8_t extCryptoOps = 0;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pDcpt->pCtrl->tlscInfo[tlsHandle-1U].pfSignCB  = pfSignCb;
    pDcpt->pCtrl->tlscInfo[tlsHandle-1U].signCbCtx = signCbCtx;

    if (NULL != pfSignCb)
    {
        extCryptoOps = 1;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_EXTCRYPTO_OPS, WINC_TYPE_INTEGER, extCryptoOps, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCtxCipherSuiteSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle
    )

  Summary:
    Set the TLS cipher suite.

  Description:
    Set the TLS cipher suite.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCtxCipherSuiteSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pDcpt->pCtrl->tlscInfo[tlsHandle-1U].tlsCsHandle = tlsCsHandle;

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_CIPHER_SUITES_IDX, WINC_TYPE_INTEGER, tlsCsHandle, 1);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_TLS_HANDLE WDRV_WINC_TLSCipherSuiteOpen(DRV_HANDLE handle)

  Summary:
    Open a TLS cipher suite.

  Description:
    Obtains a handle to a free TLS cipher suite. Used for configuring
    specific cipher algorithms for a TLS connection.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_TLS_CS_HANDLE WDRV_WINC_TLSCipherSuiteOpen(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    unsigned int i;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_TLS_INVALID_HANDLE;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_TLS_INVALID_HANDLE;
    }

    for (i=0; i<WDRV_WINC_TLS_CIPHER_SUITE_NUM; i++)
    {
        if (false == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].idxInUse)
        {
            pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].idxInUse = true;

            pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].setCmdReqHandle = WINC_CMD_REQ_INVALID_HANDLE;

            return (WDRV_WINC_TLS_CS_HANDLE)(i+1U);
        }
    }

    return WDRV_WINC_TLS_INVALID_HANDLE;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_TLSCipherSuiteClose
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle
    )

  Summary:
    Closes a TLS cipher suite.

  Description:
    Frees a TLS cipher suite handle for other uses.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

void WDRV_WINC_TLSCipherSuiteClose
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return;
    }

    if ((WDRV_WINC_TLS_CS_INVALID_HANDLE == tlsCsHandle) || (tlsCsHandle > WDRV_WINC_TLS_CIPHER_SUITE_NUM))
    {
        return;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return;
    }

    if (false == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1U].idxInUse)
    {
        return;
    }

    pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1U].idxInUse = false;

    return;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
        const uint16_t *const pAlgorithms,
        uint8_t numAlgorithms,
        const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB
    )

  Summary:
    Set a list of ciphers suite algorithms requested to the TLS cipher suite.

  Description:
    Set a list of ciphers suite algorithms requested to the TLS cipher suite.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgSet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
    const uint16_t *const pAlgorithms,
    uint8_t numAlgorithms,
    const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    uint8_t i;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pAlgorithms))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS cipher suite handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_CS_INVALID_HANDLE == tlsCsHandle) || (tlsCsHandle > WDRV_WINC_TLS_CIPHER_SUITE_NUM) || (false == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(((unsigned int)numAlgorithms)+1U, 0, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSCSC(cmdReqHandle, tlsCsHandle, 1, WINC_TYPE_INTEGER, 0, 1);

    for (i=0; i<numAlgorithms; i++)
    {
        (void)WINC_CmdTLSCSC(cmdReqHandle, tlsCsHandle, 1, WINC_TYPE_INTEGER, pAlgorithms[i], 1);
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1U].setCmdReqHandle = cmdReqHandle;

    pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB = pfTlsCsResponseCB;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
        const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB,
        bool getDefaults
    )

  Summary:
    Gets the current or default cipher suite algorithms set for a given context.

  Description:
     Gets the current or default cipher suite algorithms set for a given context.

  Remarks:
    See wdrv_winc_tls.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgGet
(
    DRV_HANDLE handle,
    WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
    const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB,
    bool getDefaults
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS cipher suite handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_CS_INVALID_HANDLE == tlsCsHandle) || (tlsCsHandle > WDRV_WINC_TLS_CIPHER_SUITE_NUM) || (false == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1U].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdTLSCSC(cmdReqHandle, tlsCsHandle, (true == getDefaults) ? WINC_CFG_PARAM_ID_TLS_CSL_CIPHER_SUITES_AVAIL : WINC_CFG_PARAM_ID_TLS_CSL_CIPHER_SUITES, WINC_TYPE_INVALID, 0, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB = pfTlsCsResponseCB;

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_TLS */
