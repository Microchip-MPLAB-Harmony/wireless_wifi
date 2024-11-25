/*******************************************************************************
  WINC Driver STA Implementation

  File Name:
    wdrv_winc_sta.c

  Summary:
    WINC wireless driver STA implementation.

  Description:
    WINC wireless driver STA implementation.
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
#include "wdrv_winc_sta.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver STA Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void staWSTAProcessRsp(DRV_HANDLE handle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)

  Summary:
    Process command responses.

  Description:
    Processes command responses received via WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED events.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    handle - WINC device handle.
    pElems - Pointer to command response elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void staWSTAProcessRsp(DRV_HANDLE handle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)
{
    const WDRV_WINC_DCPT *pDcpt = (const WDRV_WINC_DCPT *)handle;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }
}

//*******************************************************************************
/*
  Function:
    static void staWSTACmdRspCallbackHandler
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

static void staWSTACmdRspCallbackHandler
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

    //WDRV_DBG_INFORM_PRINT("WSTA CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
//            WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (WINC_DEV_EVENT_STATUS_ARGS*)eventArg;
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                staWSTAProcessRsp((DRV_HANDLE)pDcpt, pRspElems);
            }
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("WSTA CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_WSTAProcessAEC
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
    See wdrv_winc_sta.h for usage information.

*/

void WDRV_WINC_WSTAProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)context;
    WDRV_WINC_CTRLDCPT *pCtrl;
    uint16_t assocID;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (pElems->rspId)
    {
#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
        case WINC_AEC_ID_WSTAAIP:
        {
            if (2U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));

            if (assocID == pCtrl->assocInfoSTA.assocID)
            {
                if (NULL != pDcpt->pCtrl->pfNetIfEventCB)
                {
                    WDRV_WINC_NETIF_ADDR_UPDATE_TYPE eventUpdate;

                    if ((WINC_TYPE_IPV4ADDR == pElems->elems[1].type) && (pElems->elems[1].length <= sizeof(WDRV_WINC_IPV4_ADDR)))
                    {
                        eventUpdate.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV4;
                        (void)memcpy((uint8_t*)&eventUpdate.addr.v4, pElems->elems[1].pData, pElems->elems[1].length);
                    }
                    else if ((WINC_TYPE_IPV6ADDR == pElems->elems[1].type) && (pElems->elems[1].length <= sizeof(WDRV_WINC_IPV6_ADDR)))
                    {
                        eventUpdate.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV6;
                        (void)memcpy((uint8_t*)&eventUpdate.addr.v6, pElems->elems[1].pData, pElems->elems[1].length);
                    }
                    else
                    {
                        break;
                    }

                    pDcpt->pCtrl->pfNetIfEventCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->netIfSTA, WDRV_WINC_NETIF_EVENT_ADDR_UPDATE, &eventUpdate);
                }
            }

            break;
        }
#endif

        case WINC_AEC_ID_WSTALD:
        {
            if (1U != pElems->numElems)
            {
                break;
            }

            if (WDRV_WINC_CONN_STATE_DISCONNECTED != pCtrl->connectedState)
            {
                WDRV_WINC_CONN_STATE ConnState = WDRV_WINC_CONN_STATE_FAILED;

                (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));

                if (assocID == pCtrl->assocInfoSTA.assocID)
                {
                    pCtrl->opChannel = WDRV_WINC_CID_ANY;

                    if (WDRV_WINC_CONN_STATE_CONNECTED == pCtrl->connectedState)
                    {
                        ConnState = WDRV_WINC_CONN_STATE_DISCONNECTED;
                    }

                    pCtrl->connectedState = WDRV_WINC_CONN_STATE_DISCONNECTED;

                    if (NULL != pCtrl->pfConnectNotifyCB)
                    {
                        /* Update user application via callback if set. */

                        pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)&pCtrl->assocInfoSTA, ConnState);
                    }
                }
            }

            break;
        }

        case WINC_AEC_ID_WSTALU:
        {
            if (3U != pElems->numElems)
            {
                break;
            }

            if (WDRV_WINC_CONN_STATE_CONNECTED != pCtrl->connectedState)
            {
                pCtrl->connectedState = WDRV_WINC_CONN_STATE_CONNECTED;

                pCtrl->assocInfoSTA.handle = (DRV_HANDLE)pCtrl;
                pCtrl->assocInfoSTA.rssi   = 0;

                (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &pCtrl->assocInfoSTA.assocID, sizeof(pCtrl->assocInfoSTA.assocID));
                (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_MACADDR, pCtrl->assocInfoSTA.peerAddress.addr, WDRV_WINC_MAC_ADDR_LEN);
                (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pCtrl->opChannel, sizeof(pCtrl->opChannel));

                pCtrl->assocInfoSTA.peerAddress.valid = true;

                if (NULL != pCtrl->pfConnectNotifyCB)
                {
                    /* Update user application via callback if set. */

                    pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)&pCtrl->assocInfoSTA, WDRV_WINC_CONN_STATE_CONNECTED);
                }
            }

            break;
        }

        case WINC_AEC_ID_WSTAERR:
        {
            if (1U != pElems->numElems)
            {
                break;
            }

            if (WDRV_WINC_CONN_STATE_CONNECTING == pCtrl->connectedState)
            {
                pCtrl->connectedState = WDRV_WINC_CONN_STATE_DISCONNECTED;

                if (NULL != pCtrl->pfConnectNotifyCB)
                {
                    /* Update user application via callback if set. */
                    pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)&pCtrl->assocInfoSTA, WDRV_WINC_CONN_STATE_FAILED);
                }
            }
            break;
        }

        case WINC_AEC_ID_WSTAROAM:
        {
            if (1U != pElems->numElems)
            {
                break;
            }

            if (WDRV_WINC_CONN_STATE_CONNECTED == pCtrl->connectedState)
            {
                (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));

                if ((pCtrl->assocInfoSTA.assocID == assocID) && (NULL != pCtrl->pfConnectNotifyCB))
                {
                    /* Update user application via callback if set. */

                    pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)&pCtrl->assocInfoSTA, WDRV_WINC_CONN_STATE_ROAMED);
                }
            }
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("WSTA AECCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSDefaultWiFiCfg
    (
        WDRV_WINC_CONN_CFG *const pWiFiCfg
    )

  Summary:
    Initialises a WiFi configuration structure to default value.

  Description:
    Create a default WiFi configuration structure suitable for connecting
      to a BSS as a STA.

  Remarks:
    See wdrv_winc_sta.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSDefaultWiFiCfg
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
    pWiFiCfg->sta.connTimeoutMs = 10000;
    pWiFiCfg->sta.roaming       = (uint8_t)WDRV_WINC_BSS_ROAMING_CFG_DEFAULT;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSConnect
    (
        DRV_HANDLE handle,
        const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        const WDRV_WINC_CONN_CFG *pWiFiCfg,
        const WDRV_WINC_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
    )

  Summary:
    Connects to a BSS in infrastructure station mode.

  Description:
    Using the defined BSS and authentication contexts this function requests
      the WINC connect to the BSS as an infrastructure station.

  Remarks:
    See wdrv_winc_sta.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSConnect
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

    if (WDRV_WINC_CID_ANY == (WDRV_WINC_CHANNEL_ID)channel)
    {
    }
    else
    {
        if (0U == (((uint16_t)1U << (channel-1U)) & (uint16_t)pDcpt->pCtrl->regulatoryChannelMask24))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }

    /* Ensure WINC is not configured for Soft-AP. */
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

        if (pWiFiCfg->sta.roaming >= (uint8_t)WDRV_WINC_NUM_BSS_ROAMING_CFGS)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }
    else
    {
        if (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSDefaultWiFiCfg(&defaultWifiCfg))
        {
            pWiFiCfg = &defaultWifiCfg;
        }
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(11, 0, staWSTACmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_SSID, WINC_TYPE_STRING, (uintptr_t)pBSSCtx->ssid.name, pBSSCtx->ssid.length);
    (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_CHANNEL, WINC_TYPE_INTEGER, channel, 0);

    /* Set BSSID if provided. */
    if (true == pBSSCtx->bssid.valid)
    {
        (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_BSSID, WINC_TYPE_MACADDR, (uintptr_t)pBSSCtx->bssid.addr, WDRV_WINC_MAC_ADDR_LEN);
    }
    else
    {
        (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_BSSID, WINC_TYPE_MACADDR, 0, 0);
    }

    if (NULL != pAuthCtx)
    {
        uintptr_t mfp;

        /* Set <SEC_TYPE> */
        (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_SEC_TYPE, WINC_TYPE_INTEGER, (uintptr_t)pAuthCtx->authType, 0);

        /* Set <CREDENTIALS> if applicable */
        if (pAuthCtx->authInfo.personal.size > 0U)
        {
            (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_CREDENTIALS, WINC_TYPE_STRING, (uintptr_t)pAuthCtx->authInfo.personal.password, pAuthCtx->authInfo.personal.size);
        }

        /* Set <MFP_TYPE> */
        if (WDRV_WINC_AUTH_MOD_NONE != (pAuthCtx->authMod & WDRV_WINC_AUTH_MOD_MFP_REQ))
        {
            mfp = WINC_CFG_PARAM_OPT_WSTA_MFP_TYPE_REQUIRED;
        }
        else if (WDRV_WINC_AUTH_MOD_NONE != (pAuthCtx->authMod & WDRV_WINC_AUTH_MOD_MFP_OFF))
        {
            mfp = WINC_CFG_PARAM_OPT_WSTA_MFP_TYPE_DISABLED;
        }
        else
        {
            mfp = WINC_CFG_PARAM_OPT_WSTA_MFP_TYPE_ENABLED;
        }
        (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_MFP_TYPE, WINC_TYPE_INTEGER_UNSIGNED, mfp, 0);
    }
    else
    {
        (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_SEC_TYPE, WINC_TYPE_INTEGER, (uintptr_t)WDRV_WINC_AUTH_TYPE_OPEN, 0);
    }

    if (NULL != pWiFiCfg)
    {
        (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_CONN_TIMEOUT, WINC_TYPE_INTEGER_UNSIGNED, pWiFiCfg->sta.connTimeoutMs, 0);
        (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_NETIF_IDX, WINC_TYPE_INTEGER_UNSIGNED, pWiFiCfg->ifIdx, 0);
        (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_ROAMING, WINC_TYPE_INTEGER_UNSIGNED, pWiFiCfg->sta.roaming, 0);

        (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)pWiFiCfg->ifIdx, WINC_CFG_PARAM_ID_NETIF_L2_ONLY, WINC_TYPE_BOOL, (uintptr_t)pWiFiCfg->l2Only, 0);
    }

    (void)WINC_CmdWSTA(cmdReqHandle, (int32_t)WINC_CONST_WSTA_STATE_ENABLE);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfConnectNotifyCB = pfNotifyCallback;
    pDcpt->pCtrl->connectedState    = WDRV_WINC_CONN_STATE_CONNECTING;

    pDcpt->pCtrl->assocInfoSTA.handle            = DRV_HANDLE_INVALID;
    pDcpt->pCtrl->assocInfoSTA.rssi              = 0;
    pDcpt->pCtrl->assocInfoSTA.authType          = (pAuthCtx != NULL) ? pAuthCtx->authType : WDRV_WINC_AUTH_TYPE_OPEN;
    pDcpt->pCtrl->assocInfoSTA.peerAddress.valid = false;
    pDcpt->pCtrl->assocInfoSTA.assocID           = 1;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSDisconnect(DRV_HANDLE handle)

  Summary:
    Disconnects from a BSS.

  Description:
    Disconnects from an existing BSS.

  Remarks:
    See wdrv_winc_sta.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSDisconnect(DRV_HANDLE handle)
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

    /* Ensure WINC is connected or attempting to connect. */
    if (WDRV_WINC_CONN_STATE_DISCONNECTED == pDcpt->pCtrl->connectedState)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, staWSTACmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWSTA(cmdReqHandle, (int32_t)WINC_CONST_WSTA_STATE_DISABLE);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSRoamingConfigure
    (
        DRV_HANDLE handle,
        WDRV_WINC_BSS_ROAMING_CFG roamingCfg
    )

  Summary:
    Configures BSS roaming support.

  Description:
    Enables or disables BSS roaming support. If enabled, the WINC can perform
      a renew of the current IP address if configured to do so, otherwise
      it will assume the existing IP address is still valid.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSRoamingConfigure
(
    DRV_HANDLE handle,
    WDRV_WINC_BSS_ROAMING_CFG roamingCfg
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and roaming parameter are valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (roamingCfg > WDRV_WINC_BSS_ROAMING_CFG_ON_L3))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, staWSTACmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_ROAMING, WINC_TYPE_INTEGER_UNSIGNED, (uintptr_t)roamingCfg, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
