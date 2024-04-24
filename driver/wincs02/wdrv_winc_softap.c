/*******************************************************************************
  WINC Driver Soft-AP Implementation

  File Name:
    wdrv_winc_softap.c

  Summary:
    WINC wireless driver Soft-AP implementation.

  Description:
    Provides an interface to create and manage a Soft-AP.
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
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_softap.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Soft-AP Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_ASSOC_INFO* _WDRV_WINC_FindAssocInfoAP
    (
        WDRV_WINC_CTRLDCPT *pCtrl,
        const uint8_t *pMacAddr
    )

  Summary:
    Find an association.

  Description:
    Finds an existing association matching the MAC address supplied or
    finds an empty association if no MAC address supplied.

  Precondition:
    System interface initialization of the WINC driver.

  Parameters:
    pCtrl    - Pointer to driver control structure.
    pMacAddr - Pointer to MAC address to find or NULL.

  Returns:
    Pointer to association info structure matching.

  Remarks:
    None.

*/

static WDRV_WINC_ASSOC_INFO* _WDRV_WINC_FindAssocInfoAP
(
    WDRV_WINC_CTRLDCPT *pCtrl,
    const uint8_t *pMacAddr
)
{
    int i;
    WDRV_WINC_ASSOC_INFO *pStaAssocInfo = NULL;

    if (NULL == pCtrl)
    {
        return NULL;
    }

    if (NULL == pMacAddr)
    {
        for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
        {
            if ((DRV_HANDLE_INVALID == pCtrl->assocInfoAP[i].handle) &&
                (false == pCtrl->assocInfoAP[i].peerAddress.valid))
            {
                pStaAssocInfo = &pCtrl->assocInfoAP[i];
                break;
            }
        }
    }
    else
    {
        for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
        {
            if ((DRV_HANDLE_INVALID != pCtrl->assocInfoAP[i].handle) &&
                (true == pCtrl->assocInfoAP[i].peerAddress.valid) &&
                (0 == memcmp(pCtrl->assocInfoAP[i].peerAddress.addr, pMacAddr, WDRV_WINC_MAC_ADDR_LEN)))
            {
                pStaAssocInfo = &pCtrl->assocInfoAP[i];
                break;
            }
        }
    }

    return pStaAssocInfo;
}

//*******************************************************************************
/*
  Function:
    static void apWAPCmdRspCallbackHandler
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

static void apWAPCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)context;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return;
    }

    //WDRV_DBG_INFORM_PRINT("WSTA CmdRspCB %08x Event %d\n", cmdReqHandle, event);

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
                    case WINC_CMD_ID_WAP:
                    {
                        WINC_DEV_PARAM_ELEM elems[10];
                        int state;

                        if (NULL == pDcpt->pCtrl->pfConnectNotifyCB)
                        {
                            break;
                        }

                        if (pStatusInfo->srcCmd.numParams < 1)
                        {
                            break;
                        }

                        /* Unpack original command parameters. */
                        if (false == WINC_DevUnpackElements(pStatusInfo->srcCmd.numParams, pStatusInfo->srcCmd.pParams, elems))
                        {
                            break;
                        }

                        WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &state, sizeof(state));

                        /* Update user application via callback if set. */
                        if (0 == state)
                        {
                            pDcpt->pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, WDRV_WINC_ASSOC_HANDLE_INVALID, (WINC_STATUS_OK == pStatusInfo->status) ? WDRV_WINC_CONN_STATE_DISCONNECTED : WDRV_WINC_CONN_STATE_FAILED);
                        }
                        else if (1 == state)
                        {
                            pDcpt->pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, WDRV_WINC_ASSOC_HANDLE_INVALID, (WINC_STATUS_OK == pStatusInfo->status) ? WDRV_WINC_CONN_STATE_CONNECTED : WDRV_WINC_CONN_STATE_FAILED);
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
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_WAPProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        WINC_DEV_EVENT_RSP_ELEMS *pElems
    )

  Summary:
    AEC process callback.

  Description:
    Callback will be called to process any AEC messages received.

  Remarks:
    See wdrv_winc_softap.h for usage information.

*/

void WDRV_WINC_WAPProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_DEV_EVENT_RSP_ELEMS *pElems
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)context;
    WDRV_WINC_CTRLDCPT *pCtrl;

    WDRV_WINC_ASSOC_INFO *pStaAssocInfo;

    uint16_t assocID;
    uint8_t peerAddress[WDRV_WINC_MAC_ADDR_LEN];

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_WAPSC:
        {
            if (pElems->numElems != 2)
            {
                break;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));
            WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_MACADDR, peerAddress, WDRV_WINC_MAC_ADDR_LEN);

            pStaAssocInfo = _WDRV_WINC_FindAssocInfoAP(pCtrl, peerAddress);

            if (NULL != pStaAssocInfo)
            {
                WDRV_DBG_ERROR_PRINT("JOIN: Association found for new connection\r\n");
            }
            else
            {
                pStaAssocInfo = _WDRV_WINC_FindAssocInfoAP(pCtrl, NULL);
            }

            if (NULL != pStaAssocInfo)
            {
                pStaAssocInfo->handle            = (DRV_HANDLE)pCtrl;
                pStaAssocInfo->peerAddress.valid = true;
                pStaAssocInfo->authType          = WDRV_WINC_AUTH_TYPE_DEFAULT;
                pStaAssocInfo->rssi              = 0;
                pStaAssocInfo->assocID           = assocID;

                memcpy(&pStaAssocInfo->peerAddress.addr, peerAddress, WDRV_WINC_MAC_ADDR_LEN);
            }
            else
            {
                WDRV_DBG_ERROR_PRINT("JOIN: New association failed\r\n");
            }


            if (NULL != pStaAssocInfo)
            {
                if (NULL != pCtrl->pfConnectNotifyCB)
                {
                    /* Update user application via callback if set. */
                    pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)pStaAssocInfo, WDRV_WINC_CONN_STATE_CONNECTED);
                }
            }

            break;
        }

        case WINC_AEC_ID_WAPAIP:
        {
            uint32_t ipv4Addr = 0;

            if (pElems->numElems != 2)
            {
                break;
            }

            if (NULL == pCtrl->pfDHCPSEventCB)
            {
                break;
            }

            WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_IPV4ADDR, &ipv4Addr, sizeof(ipv4Addr));

            pCtrl->pfDHCPSEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_DHCPS_EVENT_LEASE_ASSIGNED, &ipv4Addr);

            break;
        }

        case WINC_AEC_ID_WAPSD:
        {
            if (pElems->numElems != 2)
            {
                break;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));
            WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_MACADDR, peerAddress, WDRV_WINC_MAC_ADDR_LEN);

            pStaAssocInfo = _WDRV_WINC_FindAssocInfoAP(pCtrl, peerAddress);

            if (NULL != pStaAssocInfo)
            {
                pStaAssocInfo->handle = DRV_HANDLE_INVALID;
                pStaAssocInfo->peerAddress.valid = false;
                pStaAssocInfo->assocID = -1;
            }
            else
            {
                WDRV_DBG_ERROR_PRINT("JOIN: No association found\r\n");
            }

            if (NULL != pCtrl->pfConnectNotifyCB)
            {
                /* Update user application via callback if set. */
                pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)pStaAssocInfo, WDRV_WINC_CONN_STATE_DISCONNECTED);
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
    WDRV_WINC_STATUS WDRV_WINC_APStart
    (
        DRV_HANDLE handle,
        const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        const WDRV_WINC_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
    )

  Summary:
    Starts an instance of Soft-AP.

  Description:
    Using the defined BSS and authentication contexts with an optional HTTP
      provisioning context (socket mode only) this function creates and starts
      a Soft-AP instance.

  Remarks:
    See wdrv_winc_softap.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_APStart
(
    DRV_HANDLE handle,
    const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    const WDRV_WINC_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    uint8_t channel;
    int i;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if ((NULL == pBSSCtx) && (NULL == pAuthCtx))
    {
        /* Allow callback to be set/changed, but only if not trying to change
         BSS/Auth settings. */
        pDcpt->pCtrl->pfConnectNotifyCB = pfNotifyCallback;
        return WDRV_WINC_STATUS_OK;
    }

    /* Ensure the BSS context is valid. */
    if (false == WDRV_WINC_BSSCtxIsValid(pBSSCtx, false))
    {
        return WDRV_WINC_STATUS_INVALID_CONTEXT;
    }

    /* NULL authentication context is OK - no encryption. */
    if (NULL != pAuthCtx)
    {
        /* Ensure the authentication context is valid. */
        if (false == WDRV_WINC_AuthCtxIsValid(pAuthCtx))
        {
            return WDRV_WINC_STATUS_INVALID_CONTEXT;
        }

    }

    channel = pBSSCtx->channel;

    if (WDRV_WINC_CID_ANY == channel)
    {
    }
    else
    {
        if(!((1<<(channel-1)) & WDRV_WINC_CM_2_4G_DEFAULT))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }

    /* Ensure WINC is not already configured for Soft-AP. */
    if (false != pDcpt->pCtrl->isAP)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Ensure WINC is not connected or attempting to connect. */
    if (WDRV_WINC_CONN_STATE_DISCONNECTED != pDcpt->pCtrl->connectedState)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfConnectNotifyCB = pfNotifyCallback;
    pDcpt->pCtrl->isAP              = true;

    for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
    {
        pDcpt->pCtrl->assocInfoAP[i].handle            = DRV_HANDLE_INVALID;
        pDcpt->pCtrl->assocInfoAP[i].peerAddress.valid = false;
        pDcpt->pCtrl->assocInfoAP[i].assocID           = -1;
    }

    pCmdReqBuffer = OSAL_Malloc(1024);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 1024, 5, apWAPCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* SSID and channel */
    WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_SSID, WINC_TYPE_STRING, (uintptr_t)pBSSCtx->ssid.name, (uintptr_t)pBSSCtx->ssid.length);
    WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_CHANNEL, WINC_TYPE_INTEGER, channel, 0);

    /* Set <SEC_TYPE> */
    WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_SEC_TYPE, WINC_TYPE_INTEGER, pAuthCtx->authType, 0);

    /* Set <CREDENTIALS> */
    WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_CREDENTIALS, WINC_TYPE_STRING, (uintptr_t)pAuthCtx->authInfo.personal.password, pAuthCtx->authInfo.personal.size);

    WINC_CmdWAP(cmdReqHandle, WINC_CONST_WAP_STATE_ENABLED);

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
    WDRV_WINC_STATUS WDRV_WINC_APStop(DRV_HANDLE handle)

  Summary:
    Stops an instance of Soft-AP.

  Description:
    Stops an instance of Soft-AP.

  Remarks:
    See wdrv_winc_softap.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_APStop(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Ensure operation mode is really Soft-AP. */
    if (false == pDcpt->pCtrl->isAP)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, apWAPCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdWAP(cmdReqHandle, WINC_CONST_WAP_STATE_DISABLED);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Clear AP state. */
    pDcpt->pCtrl->isAP = false;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_APRekeyIntervalSet
    (
        DRV_HANDLE handle,
        const uint32_t interval
    )

  Summary:
    Configures the group re-key interval used when operating in Soft-AP mode

  Description:
    The re-key interval specifies how much time must elapse before a group re-key
    is initiated with connected stations.

  Remarks:
    See wdrv_pic32mzw_softap.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_APRekeyIntervalSet(
    DRV_HANDLE handle,
    const uint32_t interval
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Ensure driver handle is valid */
    if (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /*Sanity check the interval */
    if ((interval < WINC_CFG_PARAM_ID_WAP_REKEY_INTERVAL_MIN) || (interval > WINC_CFG_PARAM_ID_WAP_REKEY_INTERVAL_MAX))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, apWAPCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_REKEY_INTERVAL, WINC_TYPE_INTEGER_UNSIGNED, interval, 0);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
