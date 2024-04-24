/*******************************************************************************
  WINC Driver TLS Context Implementation

  File Name:
    wdrv_winc_tls.c

  Summary:
    WINC wireless driver TLS context implementation.

  Description:
    This interface manages the TLS contexts.
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
#include "wdrv_winc_tls.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver TLS Context Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void tlsProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)

  Summary:
    Process command responses.

  Description:
    Processes command responses received via WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED events.

  Precondition:
    WINC_DevTransmitCmdReq must of been called to submit command request.

  Parameters:
    handle - WINC device handle.
    pElems - Pointer to command response elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void tlsProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_CMD_ID_TLSCSC:
        {
            WINC_DEV_FRACT_INT_TYPE id;

            if (2 != pElems->numElems)
            {
                break;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

            if (-1 == id.f)
            {
                WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pDcpt->pCtrl->tlsCipherSuites.numAlgorithms, sizeof(pDcpt->pCtrl->tlsCipherSuites.numAlgorithms));
            }
            else
            {
                uint16_t cs;

                WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &cs, sizeof(cs));

                pDcpt->pCtrl->tlsCipherSuites.algorithms[id.f] = cs;
            }

            break;
        }

        default:
        {
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

//    WDRV_DBG_INFORM_PRINT("TLSC CmdRspCB %08x Event %d\n", cmdReqHandle, event);

    switch (event)
    {
        case WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
        {
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
        {
            int i;

            for (i=0; i<WDRV_WINC_TLS_CIPHER_SUITE_NUM; i++)
            {
                if (true == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].idxInUse)
                {
                    if (cmdReqHandle == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].setCmdReqHandle)
                    {
                        pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].setCmdReqHandle = WINC_CMD_REQ_INVALID_HANDLE;

                        if (NULL != pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB)
                        {
                            pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB((DRV_HANDLE)pDcpt, i+i, NULL, 0, true);
                        }

                        pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB = NULL;
                    }
                }
            }

            OSAL_Free((void*)cmdReqHandle);
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
        {
            WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (WINC_DEV_EVENT_STATUS_ARGS*)eventArg;
            WINC_DEV_PARAM_ELEM elems[10];

            if (NULL != pStatusInfo)
            {
                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_TLSCSC:
                    {
                        uint16_t tlsCsHandle;

                        if (NULL == pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB)
                        {
                            break;
                        }

                        if (pStatusInfo->srcCmd.numParams < 1)
                        {
                            break;
                        }

                        if (false == WINC_DevUnpackElements(pStatusInfo->srcCmd.numParams, pStatusInfo->srcCmd.pParams, elems))
                        {
                            break;
                        }

                        WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &tlsCsHandle, sizeof(tlsCsHandle));

                        if (2 == pStatusInfo->srcCmd.numParams)
                        {
                            if (WDRV_WINC_STATUS_OK == pStatusInfo->status)
                            {
                                pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB((DRV_HANDLE)pDcpt, tlsCsHandle, pDcpt->pCtrl->tlsCipherSuites.algorithms, pDcpt->pCtrl->tlsCipherSuites.numAlgorithms, true);
                            }
                            else
                            {
                                pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB((DRV_HANDLE)pDcpt, tlsCsHandle, NULL, 0, false);
                            }

                            pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB = NULL;
                        }
                        else if (3 == pStatusInfo->srcCmd.numParams)
                        {
                            if (WDRV_WINC_STATUS_OK != pStatusInfo->status)
                            {
                                uint16_t cs;

                                WINC_CmdReadParamElem(&elems[2], WINC_TYPE_INTEGER, &cs, sizeof(cs));

                                pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB((DRV_HANDLE)pDcpt, tlsCsHandle, &cs, 1, false);
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

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                tlsProcessCmdRsp((DRV_HANDLE)pDcpt, pRspElems);
            }

            break;
        }
    }
}

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
    int i;

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
            return i+1;
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

    if (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1].idxInUse)
    {
        return;
    }

    pDcpt->pCtrl->tlscInfo[tlsHandle-1].idxInUse = false;

    return;
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
    void *pCmdReqBuffer;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pCACertName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pCmdReqBuffer = OSAL_Malloc(384);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 384, 2, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PEER_AUTH, WINC_TYPE_BOOL, (true == peerAuth) ? 1 : 0, 0);
    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_CA_CERT_NAME, WINC_TYPE_STRING, (uintptr_t)pCACertName, strnlen(pCACertName, WINC_CFG_PARAM_ID_TLS_CTX_CA_CERT_NAME_SZ));

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
    void *pCmdReqBuffer;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pCertName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 1, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_CERT_NAME, WINC_TYPE_STRING, (uintptr_t)pCertName, strnlen(pCertName, WINC_CFG_PARAM_ID_TLS_CTX_CERT_NAME_SZ));

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
    void *pCmdReqBuffer;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pPrivKeyName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 2, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PRI_KEY_NAME, WINC_TYPE_STRING, (uintptr_t)pPrivKeyName, strnlen(pPrivKeyName, WINC_CFG_PARAM_ID_TLS_CTX_PRI_KEY_NAME_SZ));

    if (NULL != pPrivKeyPassword)
    {
        WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PRI_KEY_PASSWORD, WINC_TYPE_STRING, (uintptr_t)pPrivKeyPassword, strnlen(pPrivKeyPassword, WINC_CFG_PARAM_ID_TLS_CTX_PRI_KEY_PASSWORD_SZ));
    }
    else
    {
        WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PRI_KEY_PASSWORD, WINC_TYPE_STRING, 0, 0);
    }

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
    void *pCmdReqBuffer;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pServerName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 1, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_SERVER_NAME, WINC_TYPE_STRING, (uintptr_t)pServerName, strnlen(pServerName, WINC_CFG_PARAM_ID_TLS_CTX_SERVER_NAME_SZ));

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
    void *pCmdReqBuffer;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pDomainName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pCmdReqBuffer = OSAL_Malloc(384);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 384, 2, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_PEER_DOMAIN_VERIFY, WINC_TYPE_BOOL, (true == verifyDomain) ? 1 : 0, 0);
    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_DOMAIN_NAME, WINC_TYPE_STRING, (uintptr_t)pDomainName, strnlen(pDomainName, WINC_CFG_PARAM_ID_TLS_CTX_DOMAIN_NAME_SZ));

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
    void *pCmdReqBuffer;
    uint8_t extCryptoOps = 0;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsHandle > WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsHandle-1].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pDcpt->pCtrl->tlscInfo[tlsHandle-1].pfSignCB  = pfSignCb;
    pDcpt->pCtrl->tlscInfo[tlsHandle-1].signCbCtx = signCbCtx;

    if (NULL != pfSignCb)
    {
        extCryptoOps = 1;
    }

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 1, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_EXTCRYPTO_OPS, WINC_TYPE_INTEGER, extCryptoOps, 0);

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
    void *pCmdReqBuffer;

     /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pDcpt->pCtrl->tlscInfo[tlsHandle-1].tlsCsHandle = tlsCsHandle;

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSC(cmdReqHandle, tlsHandle, WINC_CFG_PARAM_ID_TLS_CTX_CIPHER_SUITES_IDX, WINC_TYPE_INTEGER, tlsCsHandle, 1);

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
    int i;

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
            pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].idxInUse        = true;
            pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[i].setCmdReqHandle = WINC_CMD_REQ_INVALID_HANDLE;
            return i+1;
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

    if (false == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1].idxInUse)
    {
        return;
    }

    pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1].idxInUse = false;

    return;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_TLSCipherSuiteAlgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_TLS_CS_HANDLE tlsCsHandle,
        uint16_t *pAlgorithms,
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
    uint16_t *pAlgorithms,
    uint8_t numAlgorithms,
    const WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;

     /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pAlgorithms))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS cipher suite handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_CS_INVALID_HANDLE == tlsCsHandle) || (tlsCsHandle > WDRV_WINC_TLS_CIPHER_SUITE_NUM) || (false == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    pCmdReqBuffer = OSAL_Malloc(128+(70*numAlgorithms));

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128+(70*numAlgorithms), 1+numAlgorithms, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSCSC(cmdReqHandle, tlsCsHandle, 1, WINC_TYPE_INTEGER, 0, 1);

    for (int i=0; i<numAlgorithms; i++)
    {
        WINC_CmdTLSCSC(cmdReqHandle, tlsCsHandle, 1, WINC_TYPE_INTEGER, pAlgorithms[i], 1);
    }

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1].setCmdReqHandle = cmdReqHandle;

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
    void *pCmdReqBuffer;

     /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the TLS cipher suite handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_CS_INVALID_HANDLE == tlsCsHandle) || (tlsCsHandle > WDRV_WINC_TLS_CIPHER_SUITE_NUM) || (false == pDcpt->pCtrl->tlsCipherSuites.tlsCsInfo[tlsCsHandle-1].idxInUse))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, tlsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdTLSCSC(cmdReqHandle, tlsCsHandle, (true == getDefaults) ? WINC_CFG_PARAM_ID_TLS_CSL_CIPHER_SUITES_AVAIL : WINC_CFG_PARAM_ID_TLS_CSL_CIPHER_SUITES, WINC_TYPE_INVALID, 0, 0);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->tlsCipherSuites.pfTlsCsResponseCB = pfTlsCsResponseCB;

    return WDRV_WINC_STATUS_OK;
}
