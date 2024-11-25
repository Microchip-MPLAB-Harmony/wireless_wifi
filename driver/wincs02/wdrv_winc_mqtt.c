/*******************************************************************************
  WINC Driver MQTT Implementation

  File Name:
    wdrv_winc_mqtt.c

  Summary:
    WINC wireless driver MQTT implementation.

  Description:
    This interface manages MQTT.
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
#include "wdrv_winc_mqtt.h"

#ifndef WDRV_WINC_MOD_DISABLE_MQTT

static const WDRV_WINC_MQTT_MSG_INFO defaultQoS0MsgInfo =
{
    .duplicate = false,
    .qos       = WDRV_WINC_MQTT_QOS_0,
    .retain    = false,
};

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver MQTT Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void mqttCmdRspCallbackHandler
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

static void mqttCmdRspCallbackHandler
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

    //WDRV_DBG_INFORM_PRINT("MQTT CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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

            const WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (NULL != pStatusInfo)
            {
                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_MQTTPUB:
                    {
                        if (WINC_STATUS_OK != pStatusInfo->status)
                        {
                            if (NULL != pDcpt->pCtrl->mqtt.pfPubStatusCb)
                            {
                                pDcpt->pCtrl->mqtt.pfPubStatusCb((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.pubStatusCbCtx, (WDRV_WINC_MQTT_PUB_HANDLE)cmdReqHandle, 0, WDRV_WINC_MQTT_PUB_STATUS_ERROR);
                            }
                        }

                        break;
                    }

                    default:
                    {
                        WDRV_DBG_VERBOSE_PRINT("MQTT CmdRspCB %08x ID %04x status %04x not handled\r\n", cmdReqHandle, pStatusInfo->rspCmdId, pStatusInfo->status);
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
                switch (pRspElems->rspId)
                {
                    case WINC_CMD_ID_MQTTPUB:
                    {
                        uint16_t packetId;
                        WDRV_WINC_MQTT_PUB_STATUS_TYPE pubStatus = WDRV_WINC_MQTT_PUB_STATUS_SENT;

                        if (1U != pRspElems->numElems)
                        {
                            break;
                        }

                        (void)WINC_CmdReadParamElem(&pRspElems->elems[0], WINC_TYPE_INTEGER, &packetId, sizeof(packetId));

                        if (NULL != pDcpt->pCtrl->mqtt.pfPubStatusCb)
                        {
                            pDcpt->pCtrl->mqtt.pfPubStatusCb((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.pubStatusCbCtx, (WDRV_WINC_MQTT_PUB_HANDLE)cmdReqHandle, packetId, pubStatus);
                        }

                        break;
                    }

                    default:
                    {
                        WDRV_DBG_VERBOSE_PRINT("MQTT CmdRspCB ID %04x not handled\r\n", pRspElems->rspId);
                        break;
                    }
                }
            }
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("MQTT CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_MQTTProcessAEC
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
    See wdrv_winc_mqtt.h for usage information.

*/

void WDRV_WINC_MQTTProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)context;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_MQTTCONN:
        {
            uint8_t connState;
            WDRV_WINC_MQTT_CONN_STATUS_TYPE prevConnState;

            if (1U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &connState, sizeof(connState));

            prevConnState = pDcpt->pCtrl->mqtt.connState;

            if (WDRV_WINC_MQTT_CONN_STATUS_CONNECTING == pDcpt->pCtrl->mqtt.connState)
            {
                if (0U == connState)
                {
                    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED;
                }
                else
                {
                    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_CONNECTED;

                    (void)memset(&pDcpt->pCtrl->mqtt.pubProps, 0, sizeof(WDRV_WINC_MQTT_PUB_PROP));
                }
            }
            else if (WDRV_WINC_MQTT_CONN_STATUS_CONNECTED == pDcpt->pCtrl->mqtt.connState)
            {
                if (0U == connState)
                {
                    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED;
                }
            }
            else if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTING == pDcpt->pCtrl->mqtt.connState)
            {
                if (0U == connState)
                {
                    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED;
                }
            }
            else
            {
                /* Do nothing. */
            }

            if (prevConnState != pDcpt->pCtrl->mqtt.connState)
            {
                if (NULL != pDcpt->pCtrl->mqtt.pfConnCB)
                {
                    pDcpt->pCtrl->mqtt.pfConnCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.connCbCtx, pDcpt->pCtrl->mqtt.connState, &pDcpt->pCtrl->mqtt.connInfo);
                }
            }

            break;
        }

        case WINC_AEC_ID_MQTTCONNACK:
        {
            uint8_t connAckFlags;

            if (2U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &connAckFlags, sizeof(connAckFlags));
            (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pDcpt->pCtrl->mqtt.connInfo.connectReasonCode, sizeof(pDcpt->pCtrl->mqtt.connInfo.connectReasonCode));

            pDcpt->pCtrl->mqtt.connInfo.sessionPresent = (0 != (connAckFlags & WINC_CONST_MQTT_CONNACK_FLAGS_SESSION_PRESENT)) ? true : false;
            break;
        }

        case WINC_AEC_ID_MQTTPUBACK:
        case WINC_AEC_ID_MQTTPUBCOMP:
        case WINC_AEC_ID_MQTTPUBERR:
        {
            uint16_t packetId;
            WDRV_WINC_MQTT_PUB_STATUS_TYPE pubStatus = WDRV_WINC_MQTT_PUB_STATUS_RECV;

            if (1U != pElems->numElems)
            {
                break;
            }

            if (WINC_AEC_ID_MQTTPUBERR == pElems->rspId)
            {
                pubStatus = WDRV_WINC_MQTT_PUB_STATUS_ERROR;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &packetId, sizeof(packetId));

            if (NULL != pDcpt->pCtrl->mqtt.pfPubStatusCb)
            {
                pDcpt->pCtrl->mqtt.pfPubStatusCb((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.pubStatusCbCtx, WDRV_WINC_MQTT_PUB_INVALID_HANDLE, packetId, pubStatus);
            }

            break;
        }

        case WINC_AEC_ID_MQTTSUB:
        {
            uint16_t reasonCode;
            WDRV_WINC_MQTT_SUB_STATUS_TYPE subStatus = WDRV_WINC_MQTT_SUB_STATUS_ACKED;

            if (1U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &reasonCode, sizeof(reasonCode));

            if (reasonCode > WINC_CONST_MQTT_QOS_QOS2)
            {
                subStatus = WDRV_WINC_MQTT_SUB_STATUS_ERROR;
            }

            if (NULL != pDcpt->pCtrl->mqtt.pfSubscribeCb)
            {
                pDcpt->pCtrl->mqtt.pfSubscribeCb((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.subscribeCbCtx, NULL, NULL, NULL, 0, subStatus);
            }

            break;
        }

        case WINC_AEC_ID_MQTTUNSUB:
        {
            uint16_t reasonCode;
            WDRV_WINC_MQTT_SUB_STATUS_TYPE subStatus = WDRV_WINC_MQTT_SUB_STATUS_END;

            if (1U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &reasonCode, sizeof(reasonCode));

            if (0U != reasonCode)
            {
                subStatus = WDRV_WINC_MQTT_SUB_STATUS_ERROR;
            }

            if (NULL != pDcpt->pCtrl->mqtt.pfSubscribeCb)
            {
                pDcpt->pCtrl->mqtt.pfSubscribeCb((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.subscribeCbCtx, NULL, NULL, NULL, 0, subStatus);
            }

            break;
        }

        case WINC_AEC_ID_MQTTSUBRX:
        {
            WDRV_WINC_MQTT_MSG_INFO msgInfo;

            if ((5U != pElems->numElems) && (6U != pElems->numElems))
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &msgInfo.duplicate, sizeof(msgInfo.duplicate));
            (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &msgInfo.qos, sizeof(msgInfo.qos));
            (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &msgInfo.retain, sizeof(msgInfo.retain));

            msgInfo.pProperties = NULL;

            if (5U == pElems->numElems)
            {
                if (NULL != pDcpt->pCtrl->mqtt.pfSubscribeCb)
                {
                    if (true == pDcpt->pCtrl->mqtt.pubProps.isValid)
                    {
                        msgInfo.pProperties = &pDcpt->pCtrl->mqtt.pubProps;
                    }

                    pDcpt->pCtrl->mqtt.pfSubscribeCb((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.subscribeCbCtx, &msgInfo, (const char*)pElems->elems[3].pData, pElems->elems[4].pData, pElems->elems[4].length, WDRV_WINC_MQTT_SUB_STATUS_RXDATA);
                }
            }
            else
            {
                uint16_t msgId;
                uint16_t msgLength;

                (void)WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_INTEGER, &msgId, sizeof(msgId));
                (void)WINC_CmdReadParamElem(&pElems->elems[5], WINC_TYPE_INTEGER, &msgLength, sizeof(msgLength));
            }

            (void)memset(&pDcpt->pCtrl->mqtt.pubProps, 0, sizeof(WDRV_WINC_MQTT_PUB_PROP));

            break;
        }

        case WINC_AEC_ID_MQTTPROPRX:
        {
            uint8_t propId;
            bool pubPropUpdated = false;

            if (pElems->numElems < 2U)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &propId, sizeof(propId));

            switch (propId)
            {
                case WINC_CFG_PARAM_ID_MQTTPROP_PAYLOAD_FORMAT_IND:
                {
                    if (0U != WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->mqtt.pubProps.payloadFormatIndicator, sizeof(pDcpt->pCtrl->mqtt.pubProps.payloadFormatIndicator)))
                    {
                        pubPropUpdated = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_MSG_EXPIRY_INTERVAL:
                {
                    if (0U != WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->mqtt.pubProps.messageExpiryInterval, sizeof(pDcpt->pCtrl->mqtt.pubProps.messageExpiryInterval)))
                    {
                        pubPropUpdated = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_CONTENT_TYPE:
                {
                    if (0U != WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_STRING, &pDcpt->pCtrl->mqtt.pubProps.contentType, WDRV_WINC_MQTT_PUB_MAX_CONTENT_TYPE_LEN))
                    {
                        pDcpt->pCtrl->mqtt.pubProps.contentType[WDRV_WINC_MQTT_PUB_MAX_CONTENT_TYPE_LEN] = (uint8_t)'\0';
                        pubPropUpdated = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_SUBSCRIPTION_ID:
                {
                    if (0U != WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->mqtt.pubProps.subscriptionIdentifer, sizeof(pDcpt->pCtrl->mqtt.pubProps.subscriptionIdentifer)))
                    {
                        pubPropUpdated = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_SESSION_EXPIRY_INTERVAL:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->mqtt.connInfo.properties.sessionExpiryInterval, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.sessionExpiryInterval));
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_ASSIGNED_CLIENT_ID:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_STRING, &pDcpt->pCtrl->mqtt.connInfo.properties.assignedClientId, WDRV_WINC_MQTT_CONN_MAX_ASSIGNED_CLIENT_ID);
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_RECEIVE_MAX:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->mqtt.connInfo.properties.receiveMax, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.receiveMax));
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_TOPIC_ALIAS_MAX:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->mqtt.connInfo.properties.topicAliasMax, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.topicAliasMax));
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_MAX_QOS:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->mqtt.connInfo.properties.maxQoS, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.maxQoS));
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_RETAIN_AVAIL:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BOOL, &pDcpt->pCtrl->mqtt.connInfo.properties.retainAvailable, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.retainAvailable));
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_USER_PROP:
                {
                    if ((3U == pElems->numElems) && (NULL != pDcpt->pCtrl->mqtt.pfUserPropCb))
                    {
                        WDRV_WINC_MQTT_USER_PROP userProp;

                        userProp.pKey     = pElems->elems[1].pData;
                        userProp.lenKey   = pElems->elems[1].length;
                        userProp.pValue   = pElems->elems[2].pData;
                        userProp.lenValue = pElems->elems[2].length;

                        pDcpt->pCtrl->mqtt.pfUserPropCb((DRV_HANDLE)pDcpt, &userProp);
                    }

                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_MAX_PACKET_SZ:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &pDcpt->pCtrl->mqtt.connInfo.properties.maxPacketSize, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.maxPacketSize));
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_WILDCARD_SUB_AVAIL:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BOOL, &pDcpt->pCtrl->mqtt.connInfo.properties.wildcardSubsAvailable, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.wildcardSubsAvailable));
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_SUBSCRIPTION_ID_AVAIL:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BOOL, &pDcpt->pCtrl->mqtt.connInfo.properties.subIDsAvailable, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.subIDsAvailable));
                    break;
                }

                case WINC_CFG_PARAM_ID_MQTTPROP_SHARED_SUBSCRIPTION_AVAIL:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BOOL, &pDcpt->pCtrl->mqtt.connInfo.properties.sharedSubsAvailable, sizeof(pDcpt->pCtrl->mqtt.connInfo.properties.sharedSubsAvailable));
                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("MQTT AECCB property %d not handled\r\n", propId);
                    break;
                }
            }

            if (true == pubPropUpdated)
            {
                pDcpt->pCtrl->mqtt.pubProps.isValid = true;
            }

            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("MQTT AECCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTBrokerSet
    (
        DRV_HANDLE handle,
        const char *pAddress,
        uint16_t port,
        WDRV_WINC_TLS_HANDLE tlsHandle
    )

  Summary:
    Configure an MQTT broker.

  Description:
    Configure the MQTT broker address and port as well as TLS handle.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTBrokerSet
(
    DRV_HANDLE handle,
    const char *pAddress,
    uint16_t port,
    WDRV_WINC_TLS_HANDLE tlsHandle
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t addressLen;
    uint8_t tlsIdx;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pAddress))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    tlsIdx = WDRV_WINC_TLSCtxHandleToCfgIdx(tlsHandle);

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE != tlsHandle) && ((tlsIdx > WDRV_WINC_TLS_CTX_NUM) || (0 == tlsIdx) || (false == pDcpt->pCtrl->tlscInfo[tlsIdx-1U].idxInUse)))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    addressLen = strnlen(pAddress, WINC_CFG_PARAM_SZ_MQTT_BROKER_ADDR+1U);

    if ((0U == addressLen) || (addressLen > WINC_CFG_PARAM_SZ_MQTT_BROKER_ADDR))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (0U == port)
    {
        if (WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle)
        {
            port = 8883;
        }
        else
        {
            port = 1883;
        }
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED != pDcpt->pCtrl->mqtt.connState)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(3, addressLen, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_BROKER_ADDR, WINC_TYPE_STRING, (uintptr_t)pAddress, addressLen);
    (void)WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_BROKER_PORT, WINC_TYPE_INTEGER, port, 0);
    (void)WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_TLS_CONF, WINC_TYPE_INTEGER, tlsIdx, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
    WDRV_WINC_STATUS WDRV_WINC_MQTTClientCfgSet
    (
        DRV_HANDLE handle,
        const char *pClientId,
        const char *pUsername,
        const char *pPassword
    )

  Summary:
    Configures the client authentication settings.

  Description:
    Configures the client ID, username and password.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTClientCfgSet
(
    DRV_HANDLE handle,
    const char *pClientId,
    const char *pUsername,
    const char *pPassword
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t clientIdLen;
    size_t usernameLen;
    size_t passwordLen;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    clientIdLen = strnlen(pClientId, WINC_CFG_PARAM_SZ_MQTT_CLIENT_ID+1U);

    if (clientIdLen > WINC_CFG_PARAM_SZ_MQTT_CLIENT_ID)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    usernameLen = strnlen(pUsername, WINC_CFG_PARAM_SZ_MQTT_USERNAME+1U);

    if (usernameLen > WINC_CFG_PARAM_SZ_MQTT_USERNAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    passwordLen = strnlen(pPassword, WINC_CFG_PARAM_SZ_MQTT_PASSWORD+1U);

    if (passwordLen > WINC_CFG_PARAM_SZ_MQTT_PASSWORD)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED != pDcpt->pCtrl->mqtt.connState)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(3, clientIdLen+usernameLen+passwordLen, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_CLIENT_ID, WINC_TYPE_STRING, (uintptr_t)pClientId, clientIdLen);
    (void)WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_USERNAME, WINC_TYPE_STRING, (uintptr_t)pUsername, usernameLen);
    (void)WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_PASSWORD, WINC_TYPE_STRING, (uintptr_t)pPassword, passwordLen);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTLWTSet
    (
        DRV_HANDLE handle,
        const WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
        const char *pTopicName,
        const uint8_t *pTopicData,
        size_t topicDataLen
    )

  Summary:
    Sets the MQTT LWT message.

  Description:
    Configures the Last Will and Testament message to be sent with an
    MQTT connect.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTLWTSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    const char *pTopicName,
    const uint8_t *pTopicData,
    size_t topicDataLen
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t topicLen;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pTopicName) || (NULL == pTopicData))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED != pDcpt->pCtrl->mqtt.connState)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    topicLen = strlen(pTopicName);

    if ((NULL != pMsgInfo) && (NULL != pMsgInfo->pProperties))
    {
        cmdReqHandle = WDRV_WINC_CmdReqInit(4, topicLen+topicDataLen, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);
    }
    else
    {
        cmdReqHandle = WDRV_WINC_CmdReqInit(2, topicLen+topicDataLen, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);
    }

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_ALL, 0);

    if (NULL == pMsgInfo)
    {
        pMsgInfo = &defaultQoS0MsgInfo;
    }
    else if (NULL != pMsgInfo->pProperties)
    {
        if (pMsgInfo->pProperties->willDelayInterval > 0U)
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_WILL_DELAY_INTERVAL, WINC_TYPE_INTEGER_UNSIGNED, pMsgInfo->pProperties->willDelayInterval, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_WILL_DELAY_INTERVAL, 1);
        }
    }
    else
    {
        /* Do nothing. */
    }

    (void)WINC_CmdMQTTLWT(cmdReqHandle, (uint8_t)pMsgInfo->qos, (uint8_t)pMsgInfo->retain, (const uint8_t*)pTopicName, topicLen, pTopicData, topicDataLen);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTConnect
    (
        DRV_HANDLE handle,
        bool cleanSession,
        uint16_t keepAliveTime,
        WDRV_WINC_MQTT_PROTO_VER protocolVer,
        const WDRV_WINC_MQTT_CONN_PROP *const pConnProp,
        WDRV_WINC_MQTT_CONN_STATUS_CALLBACK pfConnCb,
        uintptr_t connCbCtx
    )

  Summary:
    Connect to an MQTT broker.

  Description:
    Starts a connection to an MQTT broker.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnect
(
    DRV_HANDLE handle,
    bool cleanSession,
    uint16_t keepAliveTime,
    WDRV_WINC_MQTT_PROTO_VER protocolVer,
    const WDRV_WINC_MQTT_CONN_PROP *const pConnProp,
    WDRV_WINC_MQTT_CONN_STATUS_CALLBACK pfConnCb,
    uintptr_t connCbCtx
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((WDRV_WINC_MQTT_PROTO_VER_5 != protocolVer) && (WDRV_WINC_MQTT_PROTO_VER_3 != protocolVer))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED != pDcpt->pCtrl->mqtt.connState)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->mqtt.pfConnCB  = pfConnCb;
    pDcpt->pCtrl->mqtt.connCbCtx = connCbCtx;

    (void)memset(&pDcpt->pCtrl->mqtt.connInfo, 0, sizeof(WDRV_WINC_MQTT_CONN_INFO));

    pDcpt->pCtrl->mqtt.protocolVer = protocolVer;

    pDcpt->pCtrl->mqtt.connInfo.properties.retainAvailable       = true;
    pDcpt->pCtrl->mqtt.connInfo.properties.wildcardSubsAvailable = true;
    pDcpt->pCtrl->mqtt.connInfo.properties.subIDsAvailable       = true;
    pDcpt->pCtrl->mqtt.connInfo.properties.sharedSubsAvailable   = true;
    pDcpt->pCtrl->mqtt.connInfo.properties.maxQoS                = 2;

    if (NULL != pConnProp)
    {
        if (WDRV_WINC_MQTT_PROTO_VER_3 == protocolVer)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        cmdReqHandle = WDRV_WINC_CmdReqInit(13, 0, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);
    }
    else
    {
        cmdReqHandle = WDRV_WINC_CmdReqInit(5, 0, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);
    }

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_ALL, 0);

    if (NULL != pConnProp)
    {
        if (pConnProp->sessionExpiryInterval > 0U)
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_SESSION_EXPIRY_INTERVAL, WINC_TYPE_INTEGER_UNSIGNED, pConnProp->sessionExpiryInterval, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_SESSION_EXPIRY_INTERVAL, 1);
        }

        if (pConnProp->receiveMax > 0U)
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_RECEIVE_MAX, WINC_TYPE_INTEGER_UNSIGNED, pConnProp->receiveMax, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_RECEIVE_MAX, 1);
        }

        if (pConnProp->maxPacketSize > 0U)
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_MAX_PACKET_SZ, WINC_TYPE_INTEGER_UNSIGNED, pConnProp->maxPacketSize, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_MAX_PACKET_SZ, 1);
        }

        if (pConnProp->topicAliasMax > 0U)
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_TOPIC_ALIAS_MAX, WINC_TYPE_INTEGER_UNSIGNED, pConnProp->topicAliasMax, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_TOPIC_ALIAS_MAX, 1);
        }
    }

    if (WDRV_WINC_MQTT_PROTO_VER_5 == pDcpt->pCtrl->mqtt.protocolVer)
    {
        if (true == pDcpt->pCtrl->mqtt.includeUserProps)
        {
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_USER_PROP, 1);
        }
    }

    (void)WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_KEEP_ALIVE, WINC_TYPE_INTEGER, keepAliveTime, 0);
    (void)WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_PROTO_VER, WINC_TYPE_INTEGER, (uintptr_t)protocolVer, 0);
    (void)WINC_CmdMQTTCONN(cmdReqHandle, (true == cleanSession) ? 1 : 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_CONNECTING;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTDisconnect
    (
        DRV_HANDLE handle,
        WDRV_WINC_MQTT_DISCONN_REASON_CODE_TYPE reasonCode
    )

  Summary:
    Disconnects from an MQTT broker.

  Description:
    Starts the process of disconnecting from an MQTT broker.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTDisconnect
(
    DRV_HANDLE handle,
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_TYPE reasonCode
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTING == pDcpt->pCtrl->mqtt.connState)
    {
        return WDRV_WINC_STATUS_OK;
    }
    else if (WDRV_WINC_MQTT_CONN_STATUS_CONNECTING == pDcpt->pCtrl->mqtt.connState)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }
    else if (WDRV_WINC_MQTT_CONN_STATUS_CONNECTED != pDcpt->pCtrl->mqtt.connState)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }
    else
    {
        /* Do Nothing. */
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTDISCONN(cmdReqHandle, (int32_t)reasonCode);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTING;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTPublish
    (
        DRV_HANDLE handle,
        const WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
        const char *pTopicName,
        const uint8_t *pTopicData,
        size_t topicDataLen,
        WDRV_WINC_MQTT_PUB_STATUS_CALLBACK pfPubStatusCb,
        uintptr_t pubStatusCbCtx,
        WDRV_WINC_MQTT_PUB_HANDLE *pPubHandle
    )

  Summary:
    Publish a message to an MQTT topic.

  Description:
    Publishes a message to an MQTT topic.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPublish
(
    DRV_HANDLE handle,
    const WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    const char *pTopicName,
    const uint8_t *pTopicData,
    size_t topicDataLen,
    WDRV_WINC_MQTT_PUB_STATUS_CALLBACK pfPubStatusCb,
    uintptr_t pubStatusCbCtx,
    WDRV_WINC_MQTT_PUB_HANDLE *pPubHandle
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t topicLen = 0;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pTopicData))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (NULL != pTopicName)
    {
        topicLen = strlen(pTopicName);
    }

    if ((NULL != pMsgInfo) && (NULL != pMsgInfo->pProperties))
    {
        if (WDRV_WINC_MQTT_PROTO_VER_3 == pDcpt->pCtrl->mqtt.protocolVer)
        {
            /* Properties are only allowed for MQTT V5 */
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        if ((0U == topicLen) && (0U == pMsgInfo->pProperties->topicAlias))
        {
            /* If topic name and topic alias are not specified, error. */
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        cmdReqHandle = WDRV_WINC_CmdReqInit(11, topicLen+topicDataLen+(size_t)WDRV_WINC_MQTT_PUB_MAX_CONTENT_TYPE_LEN, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);
    }
    else
    {
        if (NULL == pTopicName)
        {
            /* No topic name specified. */
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        cmdReqHandle = WDRV_WINC_CmdReqInit(3, topicLen+topicDataLen, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);
    }

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_ALL, 0);

    if (NULL == pMsgInfo)
    {
        pMsgInfo = &defaultQoS0MsgInfo;
    }
    else if (NULL != pMsgInfo->pProperties)
    {
        if (pMsgInfo->pProperties->payloadFormatIndicator > 0U)
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_PAYLOAD_FORMAT_IND, WINC_TYPE_INTEGER_UNSIGNED, pMsgInfo->pProperties->payloadFormatIndicator, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_PAYLOAD_FORMAT_IND, 1);
        }

        if (pMsgInfo->pProperties->messageExpiryInterval > 0U)
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_MSG_EXPIRY_INTERVAL, WINC_TYPE_INTEGER_UNSIGNED, pMsgInfo->pProperties->messageExpiryInterval, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_MSG_EXPIRY_INTERVAL, 1);
        }

        if ((pMsgInfo->pProperties->topicAlias > 0U) && (NULL != pTopicName))
        {
            /* Only include topic alias if the topic name is also being included. */
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_TOPIC_ALIAS, WINC_TYPE_INTEGER_UNSIGNED, pMsgInfo->pProperties->topicAlias, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_TOPIC_ALIAS, 1);
        }

        if (0U != pMsgInfo->pProperties->contentType[0])
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, WINC_CONST_MQTT_PROP_ID_CONTENT_TYPE, WINC_TYPE_STRING, (uintptr_t)pMsgInfo->pProperties->contentType, strnlen((const char*)pMsgInfo->pProperties->contentType, WDRV_WINC_MQTT_PUB_MAX_CONTENT_TYPE_LEN));
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_CONTENT_TYPE, 1);
        }
    }
    else
    {
        /* Do nothing. */
    }

    if (WDRV_WINC_MQTT_PROTO_VER_5 == pDcpt->pCtrl->mqtt.protocolVer)
    {
        if (true == pDcpt->pCtrl->mqtt.includeUserProps)
        {
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_USER_PROP, 1);
        }
    }

    if (NULL != pTopicName)
    {
        (void)WINC_CmdMQTTPUB(cmdReqHandle, (true == pMsgInfo->duplicate) ? 1U : 0U, (uint8_t)pMsgInfo->qos, (true == pMsgInfo->retain) ? 1U : 0U, WINC_TYPE_STRING, (uintptr_t)pTopicName, topicLen, pTopicData, topicDataLen);
    }
    else
    {
        (void)WINC_CmdMQTTPUB(cmdReqHandle, (true == pMsgInfo->duplicate) ? 1U : 0U, (uint8_t)pMsgInfo->qos, (true == pMsgInfo->retain) ? 1U : 0U, WINC_TYPE_INTEGER_UNSIGNED, pMsgInfo->pProperties->topicAlias, 0, pTopicData, topicDataLen);
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (NULL != pPubHandle)
    {
        *pPubHandle = (uintptr_t)cmdReqHandle;
    }

    pDcpt->pCtrl->mqtt.pfPubStatusCb  = pfPubStatusCb;
    pDcpt->pCtrl->mqtt.pubStatusCbCtx = pubStatusCbCtx;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
    WDRV_WINC_STATUS WDRV_WINC_MQTTSubscribe
    (
        DRV_HANDLE handle,
        WDRV_WINC_MQTT_QOS_TYPE qos,
        const char *pTopicName,
        const WDRV_WINC_MQTT_SUB_PROP *const pSubProp,
        WDRV_WINC_MQTT_SUBSCRIBE_CALLBACK pfSubscribeCb,
        uintptr_t subscribeCbCtx
    )

  Summary:
    Subscribe to an MQTT topic.

  Description:
    Sends a subscription request to the MQTT broker.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTSubscribe
(
    DRV_HANDLE handle,
    WDRV_WINC_MQTT_QOS_TYPE qos,
    const char *pTopicName,
    const WDRV_WINC_MQTT_SUB_PROP *const pSubProp,
    WDRV_WINC_MQTT_SUBSCRIBE_CALLBACK pfSubscribeCb,
    uintptr_t subscribeCbCtx
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t topicLen;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pTopicName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    topicLen = strlen(pTopicName);

    if (NULL != pSubProp)
    {
        if (WDRV_WINC_MQTT_PROTO_VER_3 == pDcpt->pCtrl->mqtt.protocolVer)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        cmdReqHandle = WDRV_WINC_CmdReqInit(5, topicLen, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);
    }
    else
    {
        cmdReqHandle = WDRV_WINC_CmdReqInit(3, topicLen, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);
    }

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_ALL, 0);

    if (NULL != pSubProp)
    {
        if (pSubProp->subscriptionIdentifer > 0U)
        {
            (void)WINC_CmdMQTTPROPTX(cmdReqHandle, (uint32_t)WINC_CONST_MQTT_PROP_ID_SUBSCRIPTION_ID, WINC_TYPE_INTEGER_UNSIGNED, pSubProp->subscriptionIdentifer, 0);
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_SUBSCRIPTION_ID, 1);
        }
    }

    if (WDRV_WINC_MQTT_PROTO_VER_5 == pDcpt->pCtrl->mqtt.protocolVer)
    {
        if (true == pDcpt->pCtrl->mqtt.includeUserProps)
        {
            (void)WINC_CmdMQTTPROPTXS(cmdReqHandle, (int32_t)WINC_CONST_MQTT_PROP_ID_USER_PROP, 1);
        }
    }

    (void)WINC_CmdMQTTSUB(cmdReqHandle, (const uint8_t*)pTopicName, topicLen, (uint8_t)qos);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->mqtt.pfSubscribeCb  = pfSubscribeCb;
    pDcpt->pCtrl->mqtt.subscribeCbCtx = subscribeCbCtx;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
    WDRV_WINC_STATUS WDRV_WINC_MQTTUnsubscribe
    (
        DRV_HANDLE handle,
        const char *pTopicName
    )

  Summary:
    Un-subscribe from an MQTT topic.

  Description:
    Sends a request to un-subscribe from an MQTT topic.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTUnsubscribe
(
    DRV_HANDLE handle,
    const char *pTopicName
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t topicLen;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pTopicName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    topicLen = strlen(pTopicName);

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, topicLen, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTUNSUB(cmdReqHandle, (const uint8_t*)pTopicName, topicLen);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}


//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTUserPropSet
    (
        DRV_HANDLE handle,
        const uint8_t *pKey,
        size_t lenKey,
        const uint8_t *pValue,
        size_t lenValue
    )

  Summary:
    Set the value of a user property key.

  Description:
    Used to set or clear the value associated with a user property key.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTUserPropSet
(
    DRV_HANDLE handle,
    const uint8_t *pKey,
    size_t lenKey,
    const uint8_t *pValue,
    size_t lenValue
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pKey))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if ((NULL == pValue) || (0U == lenValue))
    {
        pValue   = NULL;
        lenValue = 0;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, lenKey+lenValue, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdMQTTPROPTXKV(cmdReqHandle, pKey, lenKey, pValue, lenValue);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->mqtt.includeUserProps = true;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTUserPropCallbackSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_MQTT_USER_PROP_CALLBACK pfUserPropCb
    )

  Summary:
    Set the user property callback.

  Description:
    Used to set the user property callback which will be called whenever
      a user property key/value pair are received.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTUserPropCallbackSet
(
    DRV_HANDLE handle,
    WDRV_WINC_MQTT_USER_PROP_CALLBACK pfUserPropCb
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pDcpt->pCtrl->mqtt.pfUserPropCb = pfUserPropCb;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoDefaultSet
    (
        WDRV_WINC_MQTT_MSG_INFO *pMsgInfo
    )

  Summary:
    Initialize the message info structure to default values.

  Description:
    Ensures all elements of the structure are intialized to safe default values.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoDefaultSet
(
    WDRV_WINC_MQTT_MSG_INFO *pMsgInfo
)
{
    if (NULL == pMsgInfo)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    (void)memset(pMsgInfo, 0, sizeof(WDRV_WINC_MQTT_MSG_INFO));

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoQoSSet
    (
        WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
        WDRV_WINC_MQTT_QOS_TYPE qos
    )

  Summary:
    Sets the QoS property.

  Description:
    Sets the QoS property of a message info structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoQoSSet
(
    WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    WDRV_WINC_MQTT_QOS_TYPE qos
)
{
    if ((NULL == pMsgInfo) || (qos > WDRV_WINC_MQTT_QOS_2))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pMsgInfo->qos = qos;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoRetainSet
    (
        WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
        bool retain
    )

  Summary:
    Sets the retain property.

  Description:
    Sets the retain property of a message info structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoRetainSet
(
    WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    bool retain
)
{
    if (NULL == pMsgInfo)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pMsgInfo->retain = retain;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoPubPropSet
    (
        WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
        WDRV_WINC_MQTT_PUB_PROP *pPubProp
    )

  Summary:
    Sets the publish properties.

  Description:
    Sets the publish properties associated with a message info structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoPubPropSet
(
    WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    WDRV_WINC_MQTT_PUB_PROP *pPubProp
)
{
    if (NULL == pMsgInfo)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pMsgInfo->pProperties = pPubProp;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropDefaultSet
    (
        WDRV_WINC_MQTT_PUB_PROP *pPubProp
    )

  Summary:
    Defaults the publish property structure.

  Description:
    Sets the publish property structure to safe default values.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

 */

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropDefaultSet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp
)
{
    if (NULL == pPubProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    (void)memset(pPubProp, 0, sizeof(WDRV_WINC_MQTT_PUB_PROP));

    pPubProp->isValid = true;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropPayloadFormatSet
    (
        WDRV_WINC_MQTT_PUB_PROP *pPubProp,
        WDRV_WINC_MQTT_PROP_PAYLOAD_FORMAT_TYPE payloadFormat
    )

  Summary:
    Sets the payload format property.

  Description:
    Sets the payload format property in the publish property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropPayloadFormatSet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    WDRV_WINC_MQTT_PROP_PAYLOAD_FORMAT_TYPE payloadFormat
)
{
    if ((NULL == pPubProp) || (payloadFormat > WDRV_WINC_MQTT_PROP_MAX_PAYLOAD_FORMAT))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pPubProp->payloadFormatIndicator = (uint8_t)payloadFormat;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropMessageExpirySet
    (
        WDRV_WINC_MQTT_PUB_PROP *pPubProp,
        uint32_t expiryTime
    )

  Summary:
    Sets the message expiry property.

  Description:
    Sets the message expiry property in the publish property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropMessageExpirySet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    uint32_t expiryTime
)
{
    if (NULL == pPubProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pPubProp->messageExpiryInterval = expiryTime;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropContentTypeSet
    (
        WDRV_WINC_MQTT_PUB_PROP *pPubProp,
        const uint8_t *pContentType
    )

  Summary:
    Sets the content type property.

  Description:
    Sets the content type property in the publish property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropContentTypeSet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    const uint8_t *pContentType
)
{
    if (NULL == pPubProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL == pContentType)
    {
        (void)memset(pPubProp->contentType, 0, WDRV_WINC_MQTT_PUB_MAX_CONTENT_TYPE_LEN+1);
    }
    else
    {
        size_t len = strnlen((const char*)pContentType, WDRV_WINC_MQTT_PUB_MAX_CONTENT_TYPE_LEN);

        (void)memcpy(pPubProp->contentType, pContentType, len);
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropWillDelaySet
    (
        WDRV_WINC_MQTT_PUB_PROP *pPubProp,
        uint32_t delayTime
    )

  Summary:
    Sets the will delay property.

  Description:
    Sets the will delay property in the publish property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropWillDelaySet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    uint32_t delayTime
)
{
    if (NULL == pPubProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pPubProp->willDelayInterval = delayTime;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropTopicAliasSet
    (
        WDRV_WINC_MQTT_PUB_PROP *pPubProp,
        uint16_t topicAlias
    )

  Summary:
    Sets the topic alias property.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropTopicAliasSet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    uint16_t topicAlias
)
{
    if (NULL == pPubProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pPubProp->topicAlias = topicAlias;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTSubPropDefaultSet
    (
        WDRV_WINC_MQTT_SUB_PROP *pSubProp
    )

  Summary:
    Defaults the subscribe property structure.

  Description:
    Sets the subscribe property structure to safe default values.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTSubPropDefaultSet
(
    WDRV_WINC_MQTT_SUB_PROP *pSubProp
)
{
    if (NULL == pSubProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    (void)memset(pSubProp, 0, sizeof(WDRV_WINC_MQTT_SUB_PROP));

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTSubPropSubscriptIDSet
    (
        WDRV_WINC_MQTT_SUB_PROP *pSubProp,
        uint32_t subId
    )

  Summary:
    Sets the subscription ID property.

  Description:
    Sets the subscription ID property in the subscribe property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTSubPropSubscriptIDSet
(
    WDRV_WINC_MQTT_SUB_PROP *pSubProp,
    uint32_t subId
)
{
    if ((NULL == pSubProp) || (subId > 268435455U))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pSubProp->subscriptionIdentifer = subId;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropDefaultSet
    (
        WDRV_WINC_MQTT_CONN_PROP *pConnProp
    )

  Summary:
    Defaults the connection property structure.

  Description:
    Sets the subscribe connection structure to safe default values.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropDefaultSet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp
)
{
    if (NULL == pConnProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    (void)memset(pConnProp, 0, sizeof(WDRV_WINC_MQTT_CONN_PROP));

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropSessionExpirySet
    (
        WDRV_WINC_MQTT_CONN_PROP *pConnProp,
        uint32_t expiryTime
    )

  Summary:
    Sets the session expiry time property.

  Description:
    Sets the session expiry time property in the connection property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropSessionExpirySet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp,
    uint32_t expiryTime
)
{
    if (NULL == pConnProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pConnProp->sessionExpiryInterval = expiryTime;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropMaxPacketSet
    (
        WDRV_WINC_MQTT_CONN_PROP *pConnProp,
        uint32_t maxPacketSize
    )

  Summary:
    Sets the maximum packet size property.

  Description:
    Sets the maximum packet size property in the connection property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropMaxPacketSet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp,
    uint32_t maxPacketSize
)
{
    if (NULL == pConnProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pConnProp->maxPacketSize = maxPacketSize;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropReceiveMaxSet
    (
        WDRV_WINC_MQTT_CONN_PROP *pConnProp,
        uint16_t receiveMax
    )

  Summary:
    Sets the receive maximum property.

  Description:
    Sets the receive maximum property in the connection property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropReceiveMaxSet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp,
    uint16_t receiveMax
)
{
    if (NULL == pConnProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pConnProp->receiveMax = receiveMax;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropTopicAliasMaxSet
    (
        WDRV_WINC_MQTT_CONN_PROP *pConnProp,
        uint16_t topicAliasMax
    )

  Summary:
    Sets the topic alias maximum property.

  Description:
    Sets the topic alias maximum property in the connection property structure.

  Remarks:
    See wdrv_winc_mqtt.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropTopicAliasMaxSet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp,
    uint16_t topicAliasMax
)
{
    if (NULL == pConnProp)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pConnProp->topicAliasMax = topicAliasMax;

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_MQTT */
