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

    //WDRV_DBG_INFORM_PRINT("MQTT CmdRspCB %08x Event %d\n", cmdReqHandle, event);

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

                        if (1 != pRspElems->numElems)
                        {
                            break;
                        }

                        WINC_CmdReadParamElem(&pRspElems->elems[0], WINC_TYPE_INTEGER, &packetId, sizeof(packetId));

                        if (NULL != pDcpt->pCtrl->mqtt.pfPubStatusCb)
                        {
                            pDcpt->pCtrl->mqtt.pfPubStatusCb((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.pubStatusCbCtx, (WDRV_WINC_MQTT_PUB_HANDLE)cmdReqHandle, packetId, pubStatus);
                        }

                        break;
                    }
                }
            }
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
        WINC_DEV_EVENT_RSP_ELEMS *pElems
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
    WINC_DEV_EVENT_RSP_ELEMS *pElems
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

            if (pElems->numElems != 1)
            {
                return;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &connState, sizeof(connState));

            prevConnState = pDcpt->pCtrl->mqtt.connState;

            if (WDRV_WINC_MQTT_CONN_STATUS_CONNECTING == pDcpt->pCtrl->mqtt.connState)
            {
                if (0 == connState)
                {
                    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED;
                }
                else
                {
                    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_CONNECTED;
                }
            }
            else if (WDRV_WINC_MQTT_CONN_STATUS_CONNECTED == pDcpt->pCtrl->mqtt.connState)
            {
                if (0 == connState)
                {
                    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED;
                }
            }
            else if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTING == pDcpt->pCtrl->mqtt.connState)
            {
                if (0 == connState)
                {
                    pDcpt->pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED;
                }
            }

            if (prevConnState != pDcpt->pCtrl->mqtt.connState)
            {
                if (NULL != pDcpt->pCtrl->mqtt.pfConnCB)
                {
                    pDcpt->pCtrl->mqtt.pfConnCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.connCbCtx, pDcpt->pCtrl->mqtt.connState);
                }
            }

            break;
        }

        case WINC_AEC_ID_MQTTCONNACK:
        {
            if (pElems->numElems != 2)
            {
                return;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &pDcpt->pCtrl->mqtt.connAckFlags, sizeof(pDcpt->pCtrl->mqtt.connAckFlags));
            WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pDcpt->pCtrl->mqtt.connAckResult, sizeof(pDcpt->pCtrl->mqtt.connAckResult));
            break;
        }

        case WINC_AEC_ID_MQTTPUBACK:
        case WINC_AEC_ID_MQTTPUBCOMP:
        case WINC_AEC_ID_MQTTPUBERR:
        {
            uint16_t packetId;
            WDRV_WINC_MQTT_PUB_STATUS_TYPE pubStatus = WDRV_WINC_MQTT_PUB_STATUS_RECV;

            if (1 != pElems->numElems)
            {
                break;
            }

            if (WINC_AEC_ID_MQTTPUBERR == pElems->rspId)
            {
                pubStatus = WDRV_WINC_MQTT_PUB_STATUS_ERROR;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &packetId, sizeof(packetId));

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

            if (1 != pElems->numElems)
            {
                break;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &reasonCode, sizeof(reasonCode));

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

            if (1 != pElems->numElems)
            {
                break;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &reasonCode, sizeof(reasonCode));

            if (0 != reasonCode)
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

            if ((5 != pElems->numElems) && (6 != pElems->numElems))
            {
                break;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &msgInfo.duplicate, sizeof(msgInfo.duplicate));
            WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &msgInfo.qos, sizeof(msgInfo.qos));
            WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &msgInfo.retain, sizeof(msgInfo.retain));

            if (5 == pElems->numElems)
            {
                if (NULL != pDcpt->pCtrl->mqtt.pfSubscribeCb)
                {
                    pDcpt->pCtrl->mqtt.pfSubscribeCb((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.subscribeCbCtx, &msgInfo, (char*)pElems->elems[3].pData, pElems->elems[4].pData, pElems->elems[4].length, WDRV_WINC_MQTT_SUB_STATUS_RXDATA);
                }
            }
            else
            {
                uint16_t msgId;
                uint16_t msgLength;

                WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_INTEGER, &msgId, sizeof(msgId));
                WINC_CmdReadParamElem(&pElems->elems[5], WINC_TYPE_INTEGER, &msgLength, sizeof(msgLength));
            }

            break;
        }

        case WINC_AEC_ID_MQTTPROPRX:
        {
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
    void *pCmdReqBuffer;
    size_t addressLen;
    uint8_t tlsIdx;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pAddress))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    tlsIdx = WDRV_WINC_TLSCtxHandleToCfgIdx(tlsHandle);

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE != tlsHandle) && ((tlsIdx >= WDRV_WINC_TLS_CTX_NUM) || (false == pDcpt->pCtrl->tlscInfo[tlsIdx-1].idxInUse)))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    addressLen = strnlen(pAddress, WINC_CFG_PARAM_ID_MQTT_BROKER_ADDR_SZ+1);

    if ((0 == addressLen) || (addressLen > WINC_CFG_PARAM_ID_MQTT_BROKER_ADDR_SZ))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (0 == port)
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

    pCmdReqBuffer = OSAL_Malloc(256+addressLen);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256+addressLen, 3, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_BROKER_ADDR, WINC_TYPE_STRING, (uintptr_t)pAddress, addressLen);
    WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_BROKER_PORT, WINC_TYPE_INTEGER, port, 0);
    WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_TLS_CONF, WINC_TYPE_INTEGER, tlsIdx, 0);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
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
    void *pCmdReqBuffer;
    size_t clientIdLen;
    size_t usernameLen;
    size_t passwordLen;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    clientIdLen = strnlen(pClientId, WINC_CFG_PARAM_ID_MQTT_CLIENT_ID_SZ+1);

    if (clientIdLen > WINC_CFG_PARAM_ID_MQTT_CLIENT_ID_SZ)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    usernameLen = strnlen(pUsername, WINC_CFG_PARAM_ID_MQTT_USERNAME_SZ+1);

    if (usernameLen > WINC_CFG_PARAM_ID_MQTT_USERNAME_SZ)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    passwordLen = strnlen(pPassword, WINC_CFG_PARAM_ID_MQTT_PASSWORD_SZ+1);

    if (passwordLen > WINC_CFG_PARAM_ID_MQTT_PASSWORD_SZ)
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

    pCmdReqBuffer = OSAL_Malloc(256+clientIdLen+usernameLen+passwordLen);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256+clientIdLen+usernameLen+passwordLen, 3, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_CLIENT_ID, WINC_TYPE_STRING, (uintptr_t)pClientId, clientIdLen);
    WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_USERNAME, WINC_TYPE_STRING, (uintptr_t)pUsername, usernameLen);
    WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_PASSWORD, WINC_TYPE_STRING, (uintptr_t)pPassword, passwordLen);

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
    void *pCmdReqBuffer;
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

    pCmdReqBuffer = OSAL_Malloc(128+topicLen+topicDataLen);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128+topicLen+topicDataLen, 1, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (NULL == pMsgInfo)
    {
        pMsgInfo = &defaultQoS0MsgInfo;
    }

    WINC_CmdMQTTLWT(cmdReqHandle, pMsgInfo->qos, pMsgInfo->retain, (uint8_t*)pTopicName, topicLen, pTopicData, topicDataLen);

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
    WDRV_WINC_STATUS WDRV_WINC_MQTTConnect
    (
        DRV_HANDLE handle,
        bool cleanSession,
        uint16_t keepAliveTime,
        WDRV_WINC_MQTT_PROTO_VER protocolVer,
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
    WDRV_WINC_MQTT_CONN_STATUS_CALLBACK pfConnCb,
    uintptr_t connCbCtx
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;

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

    pDcpt->pCtrl->mqtt.pfConnCB      = pfConnCb;
    pDcpt->pCtrl->mqtt.connCbCtx     = connCbCtx;
    pDcpt->pCtrl->mqtt.connAckFlags  = 0;
    pDcpt->pCtrl->mqtt.connAckResult = 0;

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 3, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_KEEP_ALIVE, WINC_TYPE_INTEGER, keepAliveTime, 0);
    WINC_CmdMQTTC(cmdReqHandle, WINC_CFG_PARAM_ID_MQTT_PROTO_VER, WINC_TYPE_INTEGER, protocolVer, 0);
    WINC_CmdMQTTCONN(cmdReqHandle, (true == cleanSession) ? 1 : 0);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
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
    void *pCmdReqBuffer;

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

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdMQTTDISCONN(cmdReqHandle, reasonCode);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
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
    void *pCmdReqBuffer;
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

    topicLen = strlen(pTopicName);

    pCmdReqBuffer = OSAL_Malloc(128+topicLen+topicDataLen);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128+topicLen+topicDataLen, 1, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (NULL == pMsgInfo)
    {
        pMsgInfo = &defaultQoS0MsgInfo;
    }

    WINC_CmdMQTTPUB(cmdReqHandle, pMsgInfo->duplicate, pMsgInfo->qos, pMsgInfo->retain, (uint8_t*)pTopicName, topicLen, pTopicData, topicDataLen);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
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
    WDRV_WINC_MQTT_SUBSCRIBE_CALLBACK pfSubscribeCb,
    uintptr_t subscribeCbCtx
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
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

    pCmdReqBuffer = OSAL_Malloc(128+topicLen);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128+topicLen, 1, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdMQTTSUB(cmdReqHandle, (uint8_t*)pTopicName, topicLen, qos);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
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
    void *pCmdReqBuffer;
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

    pCmdReqBuffer = OSAL_Malloc(128+topicLen);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128+topicLen, 1, mqttCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdMQTTUNSUB(cmdReqHandle, (uint8_t*)pTopicName, topicLen);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
