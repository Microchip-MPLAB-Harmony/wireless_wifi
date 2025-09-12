/*******************************************************************************
  WINC Wireless Driver Soft-AP Source File

  File Name:
    wdrv_winc_softap.c

  Summary:
    WINC wireless driver Soft-AP implementation.

  Description:
    Provides an interface to create and manage a Soft-AP.
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
#include "wdrv_winc_softap.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Soft-AP Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void wapProcessStatus
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

static void wapProcessStatus
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
        case WINC_CMD_ID_WAP:
        {
            WINC_DEV_PARAM_ELEM elems[10];
            int state;

            if (NULL == pDcpt->pCtrl->pfConnectNotifyCB)
            {
                break;
            }

            if (pSrcCmd->numParams < 1U)
            {
                break;
            }

            /* Unpack original command parameters. */
            if (false == WINC_DevUnpackElements(pSrcCmd->numParams, pSrcCmd->pParams, elems))
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &state, sizeof(state));

            /* Update user application via callback if set. */
            if (0 == state)
            {
                pDcpt->pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, WDRV_WINC_ASSOC_HANDLE_INVALID, (WINC_STATUS_OK == statusCode) ? WDRV_WINC_CONN_STATE_DISCONNECTED : WDRV_WINC_CONN_STATE_FAILED);
            }
            else if (1 == state)
            {
                pDcpt->pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, WDRV_WINC_ASSOC_HANDLE_INVALID, (WINC_STATUS_OK == statusCode) ? WDRV_WINC_CONN_STATE_CONNECTED : WDRV_WINC_CONN_STATE_FAILED);
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
    static void wapProcessAEC
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

static void wapProcessAEC
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t aecId,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    WDRV_WINC_CTRLDCPT *pCtrl;
    WDRV_WINC_ASSOC_INFO *pStaAssocInfo;
    WDRV_WINC_MAC_ADDR peerAddress;
    uint16_t assocID;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (aecId)
    {
        case WINC_AEC_ID_WAPSC:
        {
            if (2U != numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));
            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_MACADDR, peerAddress.addr, WDRV_WINC_MAC_ADDR_LEN);
            peerAddress.valid = true;

            pStaAssocInfo = WDRV_WINC_AssocFindSTAInfo((DRV_HANDLE)pDcpt, &peerAddress);

            if (NULL != pStaAssocInfo)
            {
                WDRV_DBG_ERROR_PRINT("JOIN: Association found for new connection\r\n");
            }
            else
            {
                pStaAssocInfo = WDRV_WINC_AssocFindSTAInfo((DRV_HANDLE)pDcpt, NULL);
            }

            if (NULL != pStaAssocInfo)
            {
                pStaAssocInfo->handle            = (DRV_HANDLE)pCtrl;
                pStaAssocInfo->peerAddress.valid = true;
                pStaAssocInfo->authType          = WDRV_WINC_AUTH_TYPE_DEFAULT;
                pStaAssocInfo->rssi              = 0;
                pStaAssocInfo->assocID           = assocID;

                (void)memcpy(&pStaAssocInfo->peerAddress, &peerAddress, sizeof(WDRV_WINC_MAC_ADDR));
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
#ifndef WDRV_WINC_MOD_DISABLE_DHCPS
            WDRV_WINC_DHCPS_EVENT_INFO leaseInfo;
            unsigned int i;
#endif

            if (2U != numElems)
            {
                break;
            }

#ifndef WDRV_WINC_MOD_DISABLE_DHCPS
            if (NULL == pCtrl->pfDHCPSEventCB)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));
            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_IPV4ADDR, &leaseInfo.leaseAssignment.ipAddr, sizeof(leaseInfo.leaseAssignment.ipAddr));

            for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
            {
                if ((DRV_HANDLE_INVALID != pDcpt->pCtrl->assocInfoAP[i].handle) &&
                        (true == pDcpt->pCtrl->assocInfoAP[i].peerAddress.valid) &&
                        (assocID == pDcpt->pCtrl->assocInfoAP[i].assocID))
                {
                    memcpy(&leaseInfo.leaseAssignment.macAddr, &pDcpt->pCtrl->assocInfoAP[i].peerAddress, sizeof(WDRV_WINC_MAC_ADDR));

                    pCtrl->pfDHCPSEventCB((DRV_HANDLE)pDcpt, WDRV_WINC_DHCPS_EVENT_LEASE_ASSIGNED, &leaseInfo);
                }
            }
#endif
            break;
        }

        case WINC_AEC_ID_WAPSD:
        {
            if (2U != numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));
            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_MACADDR, peerAddress.addr, WDRV_WINC_MAC_ADDR_LEN);
            peerAddress.valid = true;

            pStaAssocInfo = WDRV_WINC_AssocFindSTAInfo((DRV_HANDLE)pDcpt, &peerAddress);

            if (NULL != pStaAssocInfo)
            {
                pStaAssocInfo->handle            = DRV_HANDLE_INVALID;
                pStaAssocInfo->peerAddress.valid = false;
                pStaAssocInfo->assocID           = 0xffffU;
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
            /* Do nothing. */
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void wapCmdRspCallbackHandler
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

static void wapCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("WAP CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
            /* Status response received for command. */

            const WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (const WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (NULL != pStatusInfo)
            {
                wapProcessStatus(pDcpt, pStatusInfo->rspCmdId, cmdReqHandle, &pStatusInfo->srcCmd, pStatusInfo->status);
            }

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
// Section: WINC Driver Soft-AP Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_WAPProcessAEC
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
    See wdrv_winc_softap.h for usage information.

*/

void WDRV_WINC_WAPProcessAEC
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

    wapProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_APDefaultWiFiCfg
    (
        WDRV_WINC_CONN_CFG *const pWiFiCfg
    )

  Summary:
    Initialises a WiFi configuration structure to default value.

  Description:
    Create a default WiFi configuration structure suitable for creating
      a BSS as a Soft-AP.

  Remarks:
    See wdrv_winc_softap.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_APDefaultWiFiCfg
(
    WDRV_WINC_CONN_CFG *const pWiFiCfg
)
{
    if (NULL == pWiFiCfg)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    (void)memset(pWiFiCfg, 0, sizeof(WDRV_WINC_CONN_CFG));

    pWiFiCfg->ifIdx             = (uint8_t)WDRV_WINC_NETIF_IDX_DEFAULT;
    pWiFiCfg->l2Only            = false;
    pWiFiCfg->ap.rekeyInterval  = 86400;
    pWiFiCfg->ap.cloaked        = false;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_APStart
    (
        DRV_HANDLE handle,
        const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        const WDRV_WINC_CONN_CFG *pWiFiCfg,
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
    const WDRV_WINC_CONN_CFG *pWiFiCfg,
    const WDRV_WINC_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    uint8_t channel;
    unsigned int i;
    WDRV_WINC_CONN_CFG defaultWifiCfg;

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

    channel = (uint8_t)pBSSCtx->channel;

    if (0U == (((uint16_t)1U<<(channel-1U)) & (uint16_t)pDcpt->pCtrl->regulatoryChannelMask24))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
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

    if (NULL != pWiFiCfg)
    {
        if (pWiFiCfg->ifIdx >= (uint8_t)WDRV_WINC_NUM_NETIFS)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        if ((pWiFiCfg->ap.rekeyInterval < WINC_CFG_PARAM_MIN_WAP_REKEY_INTERVAL) || (pWiFiCfg->ap.rekeyInterval > WINC_CFG_PARAM_MAX_WAP_REKEY_INTERVAL))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }
    else
    {
        if (WDRV_WINC_STATUS_OK == WDRV_WINC_APDefaultWiFiCfg(&defaultWifiCfg))
        {
            pWiFiCfg = &defaultWifiCfg;
        }
    }

    pDcpt->pCtrl->pfConnectNotifyCB = pfNotifyCallback;
    pDcpt->pCtrl->isAP              = true;

    for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
    {
        pDcpt->pCtrl->assocInfoAP[i].handle            = DRV_HANDLE_INVALID;
        pDcpt->pCtrl->assocInfoAP[i].peerAddress.valid = false;
        pDcpt->pCtrl->assocInfoAP[i].assocID           = 0xffffU;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(9, 0, wapCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* SSID and channel */
    (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_SSID, WINC_TYPE_STRING, (uintptr_t)pBSSCtx->ssid.name, (uintptr_t)pBSSCtx->ssid.length);

    (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_CHANNEL, WINC_TYPE_INTEGER, channel, 0);

    if (NULL != pAuthCtx)
    {
        uintptr_t mfp;

        /* Set <SEC_TYPE> */
        (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_SEC_TYPE, WINC_TYPE_INTEGER, (uintptr_t)pAuthCtx->authType, 0);

        /* Set <CREDENTIALS> */
        (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_CREDENTIALS, WINC_TYPE_STRING, (uintptr_t)pAuthCtx->authInfo.personal.password, pAuthCtx->authInfo.personal.size);

        /* Set <MFP_TYPE> */
        if (WDRV_WINC_AUTH_MOD_NONE != (pAuthCtx->authMod & WDRV_WINC_AUTH_MOD_MFP_REQ))
        {
            mfp = WINC_CFG_PARAM_OPT_WAP_MFP_TYPE_REQUIRED;
        }
        else if (WDRV_WINC_AUTH_MOD_NONE != (pAuthCtx->authMod & WDRV_WINC_AUTH_MOD_MFP_OFF))
        {
            mfp = WINC_CFG_PARAM_OPT_WAP_MFP_TYPE_DISABLED;
        }
        else
        {
            mfp = WINC_CFG_PARAM_OPT_WAP_MFP_TYPE_ENABLED;
        }

        (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_MFP_TYPE, WINC_TYPE_INTEGER_UNSIGNED, mfp, 0);
    }
    else
    {
        /* Set <SEC_TYPE> */
        (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_SEC_TYPE, WINC_TYPE_INTEGER, (uintptr_t)WDRV_WINC_AUTH_TYPE_OPEN, 0);
    }

    if (NULL != pWiFiCfg)
    {
        (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_HIDDEN, WINC_TYPE_BOOL, (true == pWiFiCfg->ap.cloaked) ? 1U : 0U, 0);

        (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_NETIF_IDX, WINC_TYPE_INTEGER_UNSIGNED, pWiFiCfg->ifIdx, 0);

        (void)WINC_CmdWAPC(cmdReqHandle, WINC_CFG_PARAM_ID_WAP_REKEY_INTERVAL, WINC_TYPE_INTEGER_UNSIGNED, pWiFiCfg->ap.rekeyInterval, 0);
    }

    (void)WINC_CmdWAP(cmdReqHandle, (int32_t)WINC_CONST_WAP_STATE_ENABLED);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wapCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWAP(cmdReqHandle, (int32_t)WINC_CONST_WAP_STATE_DISABLED);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Clear AP state. */
    pDcpt->pCtrl->isAP = false;

    return WDRV_WINC_STATUS_OK;
}
