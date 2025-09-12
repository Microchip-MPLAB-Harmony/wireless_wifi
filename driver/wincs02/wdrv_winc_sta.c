/*******************************************************************************
  WINC Wireless Driver STA Source File

  File Name:
    wdrv_winc_sta.c

  Summary:
    WINC wireless driver STA implementation.

  Description:
    Provides an interface to connect to a BSS as a station.
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
#include "wdrv_winc_sta.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver STA Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void wstaProcessAEC
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

static void wstaProcessAEC
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t aecId,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    WDRV_WINC_CTRLDCPT *pCtrl;
    uint16_t assocID;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (aecId)
    {
#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
        case WINC_AEC_ID_WSTAAIP:
        {
            if (2U != numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));

            if (assocID == pCtrl->assocInfoSTA.assocID)
            {
                if (NULL != pDcpt->pCtrl->pfNetIfEventCB)
                {
                    WDRV_WINC_NETIF_ADDR_UPDATE_TYPE eventUpdate;

                    if ((WINC_TYPE_IPV4ADDR == pElems[1].type) && (pElems[1].length <= sizeof(WDRV_WINC_IPV4_ADDR)))
                    {
                        eventUpdate.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV4;
                        (void)memcpy((uint8_t*)&eventUpdate.addr.v4, pElems[1].pData, pElems[1].length);
                    }
                    else if ((WINC_TYPE_IPV6ADDR == pElems[1].type) && (pElems[1].length <= sizeof(WDRV_WINC_IPV6_ADDR)))
                    {
                        eventUpdate.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV6;
                        (void)memcpy((uint8_t*)&eventUpdate.addr.v6, pElems[1].pData, pElems[1].length);
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
            if (1U != numElems)
            {
                break;
            }

            if (WDRV_WINC_CONN_STATE_DISCONNECTED != pCtrl->connectedState)
            {
                WDRV_WINC_CONN_STATE ConnState = WDRV_WINC_CONN_STATE_FAILED;

                (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));

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
            if (3U != numElems)
            {
                break;
            }

            if (WDRV_WINC_CONN_STATE_CONNECTED != pCtrl->connectedState)
            {
                pCtrl->connectedState = WDRV_WINC_CONN_STATE_CONNECTED;

                pCtrl->assocInfoSTA.handle = (DRV_HANDLE)pCtrl;
                pCtrl->assocInfoSTA.rssi   = 0;

                (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &pCtrl->assocInfoSTA.assocID, sizeof(pCtrl->assocInfoSTA.assocID));
                (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_MACADDR, pCtrl->assocInfoSTA.peerAddress.addr, WDRV_WINC_MAC_ADDR_LEN);
                (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER, &pCtrl->opChannel, sizeof(pCtrl->opChannel));

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
            if (1U != numElems)
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
            if (1U != numElems)
            {
                break;
            }

            if (WDRV_WINC_CONN_STATE_CONNECTED == pCtrl->connectedState)
            {
                (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));

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
            /* Do nothing. */
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void wstaCmdRspCallbackHandler
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

static void wstaCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("WSTA CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
// Section: WINC Driver STA Implementation
// *****************************************************************************
// *****************************************************************************

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

    if ((NULL == pDcpt) || (NULL == pElems))
    {
        return;
    }

    wstaProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(11, 0, wstaCmdRspCallbackHandler, (uintptr_t)pDcpt);

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

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wstaCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWSTA(cmdReqHandle, (int32_t)WINC_CONST_WSTA_STATE_DISABLE);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wstaCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWSTAC(cmdReqHandle, WINC_CFG_PARAM_ID_WSTA_ROAMING, WINC_TYPE_INTEGER_UNSIGNED, (uintptr_t)roamingCfg, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
