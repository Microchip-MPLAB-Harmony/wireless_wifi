/*******************************************************************************
  WINC Driver MQTT Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_mqtt.h

  Summary:
    WINC wireless driver MQTT header file.

  Description:
    This interface manages MQTT.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef WDRV_WINC_MQTT_H
#define WDRV_WINC_MQTT_H

#ifndef WDRV_WINC_MOD_DISABLE_MQTT

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver MQTT Defines
// *****************************************************************************
// *****************************************************************************

/* This defines the value of an invalid publish handle. */
#define WDRV_WINC_MQTT_PUB_INVALID_HANDLE               0

/* The maximum length of a content type string within the publish message. */
#define WDRV_WINC_MQTT_PUB_MAX_CONTENT_TYPE_LEN         32

/* Defines the maximum length of the assigned client ID. */
#define WDRV_WINC_MQTT_CONN_MAX_ASSIGNED_CLIENT_ID      48

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver MQTT Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MQTT Connection Status

  Summary:
    MQTT connection status type.

  Description:
    Type describing possible MQTT connection states.

  Remarks:
    None.
*/

typedef enum
{
    /* Connection status unknown. */
    WDRV_WINC_MQTT_CONN_STATUS_UNKNOWN,

    /* MQTT is disconnected. */
    WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED,

    /* MQTT is in the process of connecting. */
    WDRV_WINC_MQTT_CONN_STATUS_CONNECTING,

    /* MQTT is connected. */
    WDRV_WINC_MQTT_CONN_STATUS_CONNECTED,

    /* MQTT is in the process of disconnecting. */
    WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTING
} WDRV_WINC_MQTT_CONN_STATUS_TYPE;

// *****************************************************************************
/* MQTT Protocol Version

  Summary:
    MQTT protocol versions.

  Description:
    Possible MQTT protocol versions supported.

  Remarks:
    None.
*/

typedef enum
{
    /* MQTT protocol version 3.1.1. */
    WDRV_WINC_MQTT_PROTO_VER_3 = 3,

    /* MQTT protocol version 5. */
    WDRV_WINC_MQTT_PROTO_VER_5 = 5
} WDRV_WINC_MQTT_PROTO_VER;

// *****************************************************************************
/* MQTT Disconnect Reason Codes

  Summary:
    MQTT reason codes for disconnecting.

  Description:
    Possible MQTT reason codes presented when disconnecting.

  Remarks:
    None.
*/

typedef enum
{
    /* Normal disconnection.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_NORMAL            = WINC_CONST_MQTT_REASON_CODE_NORMAL,

    /* Disconnect with Will Message.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_SEND_LWT          = WINC_CONST_MQTT_REASON_CODE_SEND_LWT,

    /* Unspecified error.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_ERROR             = WINC_CONST_MQTT_REASON_CODE_ERROR,

    /* Malformed packet.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_MALFORM_PKT       = WINC_CONST_MQTT_REASON_CODE_MALFORM_PKT,

    /* Protocol error.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_PROT_ERR          = WINC_CONST_MQTT_REASON_CODE_PROT_ERR,

    /* Implementation specific error.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_IMP_SPEC_ERR      = WINC_CONST_MQTT_REASON_CODE_IMP_SPEC_ERR,

    /* Topic name invalid.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_TOP_INVALID       = WINC_CONST_MQTT_REASON_CODE_TOP_INVALID,

    /* Receive maximum exceeded.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_RX_MAX_EXCEED     = WINC_CONST_MQTT_REASON_CODE_RX_MAX_EXCEED,

    /* Topic alias invalid.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_TOP_ALIAS_INVALID = WINC_CONST_MQTT_REASON_CODE_TOP_ALIAS_INVALID,

    /* Packet too large.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_PKT_TOO_LARGE     = WINC_CONST_MQTT_REASON_CODE_PKT_TOO_LARGE,

    /* Message rate too high.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_MSG_RATE_TOO_HIGH = WINC_CONST_MQTT_REASON_CODE_MSG_RATE_TOO_HIGH,

    /* Quota exceeded.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_QUOTA_EXCEED      = WINC_CONST_MQTT_REASON_CODE_QUOTA_EXCEED,

    /* Administrative action.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_ADMIN_ACT         = WINC_CONST_MQTT_REASON_CODE_ADMIN_ACT,

    /* Payload format invalid.*/
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_PAYLOAD_INVALID   = WINC_CONST_MQTT_REASON_CODE_PAYLOAD_INVALID,
} WDRV_WINC_MQTT_DISCONN_REASON_CODE_TYPE;

// *****************************************************************************
/* MQTT QoS Levels

  Summary:
    MQTT quality of service levels.

  Description:
    Definition of possible quality of service levels.

  Remarks:
    None.
*/

typedef enum
{
    /* QoS 0. */
    WDRV_WINC_MQTT_QOS_0 = WINC_CONST_MQTT_QOS_QOS0,

    /* QoS 1. */
    WDRV_WINC_MQTT_QOS_1 = WINC_CONST_MQTT_QOS_QOS1,

    /* QoS 2. */
    WDRV_WINC_MQTT_QOS_2 = WINC_CONST_MQTT_QOS_QOS2
} WDRV_WINC_MQTT_QOS_TYPE;

// *****************************************************************************
/* MQTT Publish Status

  Summary:
    MQTT publish status values.

  Description:
    Possible values for MQTT publish status.

  Remarks:
    None.
*/

typedef enum
{
    /* Publish has been sent. */
    WDRV_WINC_MQTT_PUB_STATUS_SENT,

    /* Publish acknowledgement and completion received. */
    WDRV_WINC_MQTT_PUB_STATUS_RECV,

    /* An error has occurred. */
    WDRV_WINC_MQTT_PUB_STATUS_ERROR
} WDRV_WINC_MQTT_PUB_STATUS_TYPE;

// *****************************************************************************
/* MQTT Subscribe Status

  Summary:
    MQTT subscribe status values.

  Description:
    Possible values for MQTT subscribe status.

  Remarks:
    None.
*/

typedef enum
{
    /* Subscribe has ben acknowledged. */
    WDRV_WINC_MQTT_SUB_STATUS_ACKED,

    /* New data has been received. */
    WDRV_WINC_MQTT_SUB_STATUS_RXDATA,

    /* An error has occurred. */
    WDRV_WINC_MQTT_SUB_STATUS_ERROR,

    /* A topic has been un-subscribed. */
    WDRV_WINC_MQTT_SUB_STATUS_END
} WDRV_WINC_MQTT_SUB_STATUS_TYPE;

// *****************************************************************************
/* MQTT Payload Formats

  Summary:
    MQTT payload format values.

  Description:
    Possible values for MQTT payload formats.

  Remarks:
    None.
*/

typedef enum
{
    /* Payload format is unspecified. */
    WDRV_WINC_MQTT_PROP_PAYLOAD_FORMAT_UNSPECIFIED = 0,

    /* Payload format is UTF-8. */
    WDRV_WINC_MQTT_PROP_PAYLOAD_FORMAT_UTF8 = 1,

    /* Maximum payload format value. */
    WDRV_WINC_MQTT_PROP_MAX_PAYLOAD_FORMAT = WDRV_WINC_MQTT_PROP_PAYLOAD_FORMAT_UTF8
} WDRV_WINC_MQTT_PROP_PAYLOAD_FORMAT_TYPE;

// *****************************************************************************
/* MQTT Subscribe Properties

  Summary:
    MQTT subscription properties.

  Description:
    Properties to be applied to MQTT subscription requests.

  Remarks:
    None.
*/

typedef struct
{
    /* Subscription Identifier. */
    uint32_t subscriptionIdentifer;
} WDRV_WINC_MQTT_SUB_PROP;

// *****************************************************************************
/* MQTT Publish Properties

  Summary:
    MQTT publish properties.

  Description:
    Properties associated with an MQTT publish operation.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if contents are valid. */
    bool isValid;

    /* Payload format indicator. */
    uint8_t payloadFormatIndicator;

    /* Topic alias. */
    uint16_t topicAlias;

    /* Message expiry interval. */
    uint32_t messageExpiryInterval;

    /* Subscription Identifier. */
    uint32_t subscriptionIdentifer;

    /* Will delay interval. */
    uint32_t willDelayInterval;

    /* Context type. */
    uint8_t contentType[WDRV_WINC_MQTT_PUB_MAX_CONTENT_TYPE_LEN+1];
} WDRV_WINC_MQTT_PUB_PROP;

// *****************************************************************************
/* Message Information Structure

  Summary:
    Structure to contain information about a message.

  Description:
    Message information which can be considered optional has been grouped
    together into a single structure.

  Remarks:
    None.
*/

typedef struct
{
    /* Message is a duplicate of a previous one. */
    bool duplicate;

    /* QoS level of message. */
    WDRV_WINC_MQTT_QOS_TYPE qos;

    /* Flag indicating if message is to be, or has been, retained. */
    bool retain;

    /* Pointer to properties associated with this message. */
    WDRV_WINC_MQTT_PUB_PROP *pProperties;
} WDRV_WINC_MQTT_MSG_INFO;

// *****************************************************************************
/* MQTT Connection Properties

  Summary:
    MQTT connection properties.

  Description:
    Properties associated with an MQTT connection.

  Remarks:
    None.
*/

typedef struct
{
    /* Session expiry interval. */
    uint32_t sessionExpiryInterval;

    /* Maximum packet size. */
    uint32_t maxPacketSize;

    /* Receive maximum. */
    uint16_t receiveMax;

    /* Topic alias max. */
    uint16_t topicAliasMax;
} WDRV_WINC_MQTT_CONN_PROP;

// *****************************************************************************
/* MQTT Connection Acknowledge Properties

  Summary:
    MQTT connection acknowledge properties.

  Description:
    Properties associated with an MQTT connection acknowledgement.

  Remarks:
    None.
*/

typedef struct
{
    /* Session expiry interval. */
    uint32_t sessionExpiryInterval;

    /* Maximum packet size. */
    uint32_t maxPacketSize;

    /* Receive maximum. */
    uint16_t receiveMax;

    /* Topic alias max. */
    uint16_t topicAliasMax;

    /* Flag indicating retain available property. */
    bool retainAvailable;

    /* Flag indicating if wildcard subscriptions are supported. */
    bool wildcardSubsAvailable;

    /* Flag indicating if subscription IDs are available. */
    bool subIDsAvailable;

    /* Flag indicating if shared subscriptions are supported. */
    bool sharedSubsAvailable;

    /* Max QoS settings. */
    uint8_t maxQoS;

    /* Assigned client ID. */
    uint8_t assignedClientId[WDRV_WINC_MQTT_CONN_MAX_ASSIGNED_CLIENT_ID+1];
} WDRV_WINC_MQTT_CONNACK_PROP;

// *****************************************************************************
/* MQTT User Properties

  Summary:
    MQTT user properties.

  Description:
    User properties consisting of a key/value pair.

  Remarks:
    None.
*/

typedef struct
{
    /* Pointer to key name data. */
    const uint8_t *pKey;

    /* Length of data pointed to by pKey. */
    size_t lenKey;

    /* Pointer to value data. */
    const uint8_t *pValue;

    /* Length of data pointed to by pValue. */
    size_t lenValue;
} WDRV_WINC_MQTT_USER_PROP;

// *****************************************************************************
/* MQTT Connection Properties

  Summary:
    MQTT connection properties.

  Description:
    Properties associated with an MQTT connection attempt.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if session was present. */
    bool sessionPresent;

    /* Connect reason code. */
    uint8_t connectReasonCode;

    /* MQTT connection properties. */
    WDRV_WINC_MQTT_CONNACK_PROP properties;
} WDRV_WINC_MQTT_CONN_INFO;

// *****************************************************************************
/* MQTT Publish Handle

  Summary:
    Short term MQTT publish handle.

  Description:
    Publish handles are provided until packet IDs are known.

  Remarks:
    Publish handles are only valid between a publish request and completion
    of the publish command at which point the packet ID replaces it.

    These are only required for QoS levels 1 & 2.
*/

typedef uintptr_t WDRV_WINC_MQTT_PUB_HANDLE;

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_MQTT_CONN_STATUS_CALLBACK)
    (
        DRV_HANDLE handle,
        uintptr_t userCtx,
        WDRV_WINC_MQTT_CONN_STATUS_TYPE state,
        WDRV_WINC_MQTT_CONN_INFO *pConnInfo
    )

  Summary:
    Callback to receive connection status.

  Description:
    Callback is called whenever the connection state changes.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    userCtx   - User context passed to the callback.
    state     - Connection state.
    pConnInfo - Pointer to connection information.

  Returns:
    None.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_MQTT_CONN_STATUS_CALLBACK)
(
    DRV_HANDLE handle,
    uintptr_t userCtx,
    WDRV_WINC_MQTT_CONN_STATUS_TYPE state,
    WDRV_WINC_MQTT_CONN_INFO *pConnInfo
);

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_MQTT_PUB_STATUS_CALLBACK)
    (
        DRV_HANDLE handle,
        uintptr_t userCtx,
        WDRV_WINC_MQTT_PUB_HANDLE pubHandle,
        uint16_t packetId,
        WDRV_WINC_MQTT_PUB_STATUS_TYPE status
    )

  Summary:
    Callback to receive publish status.

  Description:
    Callback is called when the publish message changes state.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    userCtx   - User context passed to the callback.
    pubHandle - Publish handle issued until packet ID is known.
    packetId  - Packet ID when known.
    status    - Status of publish request.

  Returns:
    None.

  Remarks:
    If pubHandle is valid the status refers to the sending of a publish message:

      WDRV_WINC_MQTT_PUB_STATUS_SENT:
        Publish message has been sent, packetId is set to the packet ID of the
        message sent. This packet ID can be used to aligned with further events
        to this callback for QoS acknowledgement/completion.

      WDRV_WINC_MQTT_PUB_STATUS_ERROR:
        There was an error sending the publish message.


    If the pubHandle is not valid the status refers to the receiving of a
    publish message:

      WDRV_WINC_MQTT_PUB_STATUS_RECV:
        A PUBACK (QoS 1) or PUBCOMP (QoS 2) response has been received.
        The packetId will aligned with a previous call to this callback.

      WDRV_WINC_MQTT_PUB_STATUS_ERROR:
        An error occurred during the publish operation.

*/

typedef void (*WDRV_WINC_MQTT_PUB_STATUS_CALLBACK)
(
    DRV_HANDLE handle,
    uintptr_t userCtx,
    WDRV_WINC_MQTT_PUB_HANDLE pubHandle,
    uint16_t packetId,
    WDRV_WINC_MQTT_PUB_STATUS_TYPE status
);

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_MQTT_SUBSCRIBE_CALLBACK)
    (
        DRV_HANDLE handle,
        uintptr_t userCtx,
        const WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
        const char *pTopicName,
        const uint8_t *pTopicData,
        size_t topicDataLen,
        WDRV_WINC_MQTT_SUB_STATUS_TYPE status
    )

  Summary:
    Subscribe callback.

  Description:
    Callback is called when a subscribe request changes state or new data is
    received.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    userCtx      - User context passed to the callback.
    pMsgInfo     - Pointer to message information.
    pTopicName   - Pointer to topic name.
    pTopicData   - Pointer to topic data.
    topicDataLen - Length of topic data.
    status       - Status of subscription.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_MQTT_SUBSCRIBE_CALLBACK)
(
    DRV_HANDLE handle,
    uintptr_t userCtx,
    const WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    const char *pTopicName,
    const uint8_t *pTopicData,
    size_t topicDataLen,
    WDRV_WINC_MQTT_SUB_STATUS_TYPE status
);

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_MQTT_USER_PROP_CALLBACK)
    (
        DRV_HANDLE handle,
        const WDRV_WINC_MQTT_USER_PROP *const pUserProps
    )

  Summary:
    User property callback.

  Description:
    Callback is called when a user property is received.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    pUserProps - Pointer to user property structure.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_MQTT_USER_PROP_CALLBACK)
(
    DRV_HANDLE handle,
    const WDRV_WINC_MQTT_USER_PROP *const pUserProps
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver MQTT Routines
// *****************************************************************************
// *****************************************************************************

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

  Precondition:
    WINC_DevAECCallbackRegister must be called to register the callback.

  Parameters:
    context   - Pointer to user context supplied when callback was registered.
    devHandle - WINC device handle.
    pElems    - Pointer to element structure.

  Returns:
    None.

  Remarks:
    Callback should call WINC_CmdReadParamElem to extract elements.

*/

void WDRV_WINC_MQTTProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    pAddress  - Pointer to broker domain name or IP address.
    port      - TCP port broker is listening on.
    tlsHandle - TLS handle if required.

  Returns:
    WDRV_WINC_STATUS_OK             - A broker configure was sent.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTBrokerSet
(
    DRV_HANDLE handle,
    const char *pAddress,
    uint16_t port,
    WDRV_WINC_TLS_HANDLE tlsHandle
);

//*******************************************************************************
/*
  Function:
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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    pClientId - Pointer to client ID.
    pUsername - Pointer to username.
    pPassword - Pointer to password.

  Returns:
    WDRV_WINC_STATUS_OK             - A client configure was sent.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTClientCfgSet
(
    DRV_HANDLE handle,
    const char *pClientId,
    const char *pUsername,
    const char *pPassword
);

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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    pMsgInfo       - Pointer to message information if required.
    pTopicName     - Pointer to topic name.
    pTopicData     - Pointer to data to publish to topic.
    topicDataLen   - Length of data to publish.

  Returns:
    WDRV_WINC_STATUS_OK             - A LWT update was sent.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    This must be called before WDRV_WINC_MQTTConnect.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTLWTSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    const char *pTopicName,
    const uint8_t *pTopicData,
    size_t topicDataLen
);

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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle        - Client handle obtained by a call to WDRV_WINC_Open.
    cleanSession  - Flag indicating if a clean session should be used.
    keepAliveTime - Keep alive time (in seconds).
    protocolVer   - Protocol version.
    pConnProp     - Pointer to optional connection properties structure.
    pfConnCb      - Pointer to connection status callback.
    connCbCtx     - User context to pass to callback when called.

  Returns:
    WDRV_WINC_STATUS_OK             - A connect request was sent.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

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
);

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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    reasonCode - Reason code to pass on, if supported.

  Returns:
    WDRV_WINC_STATUS_OK             - A disconnect request was sent.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTDisconnect
(
    DRV_HANDLE handle,
    WDRV_WINC_MQTT_DISCONN_REASON_CODE_TYPE reasonCode
);

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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    pMsgInfo       - Pointer to message information if required.
    pTopicName     - Pointer to topic name.
    pTopicData     - Pointer to data to publish to topic.
    topicDataLen   - Length of data to publish.
    pfPubStatusCb  - Pointer to publish status callback.
    pubStatusCbCtx - User context to be passed to callback when called.
    pPubHandle     - Pointer to handle to receive short term publish handle.

  Returns:
    WDRV_WINC_STATUS_OK             - A publish was sent.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    The publish handle returned is only valid until the callback is called
    with both a handle and a packet ID, after which the handle is no longer
    valid and should be discarded. Further updates to the callback will use
    the packet ID. This is only relevant to QoS 1 and 2.

    To use topic aliases in MQTT V5 a publish must be sent first with both
    pTopicName and pMsgInfo->pProperties->topicAlias being valid. Once the
    alias is registered with the broker WDRV_WINC_MQTTPublish can be called
    with pTopicName equal to NULL as long as pMsgInfo->pProperties->topicAlias
    is set to the registered alias ID.

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
);

//*******************************************************************************
/*
  Function:
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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.
    WDRV_WINC_MQTTConnect must have been called to connect to a broker.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    qos            - Quality of service required.
    pTopicName     - Pointer to topic name to subscribe to.
    pSubProp       - Pointer to an optional subscription property structure.
    pfSubscribeCb  - Pointer to subscription callback.
    subscribeCbCtx - User context to pass to callback when called.

  Returns:
    WDRV_WINC_STATUS_OK             - A subscribe was sent.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTSubscribe
(
    DRV_HANDLE handle,
    WDRV_WINC_MQTT_QOS_TYPE qos,
    const char *pTopicName,
    const WDRV_WINC_MQTT_SUB_PROP *const pSubProp,
    WDRV_WINC_MQTT_SUBSCRIBE_CALLBACK pfSubscribeCb,
    uintptr_t subscribeCbCtx
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MQTTUnsubscribe
    (
        DRV_HANDLE handle,
        const char* pTopicName
    )

  Summary:
    Un-subscribe from an MQTT topic.

  Description:
    Sends a request to un-subscribe from an MQTT topic.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.
    WDRV_WINC_MQTTConnect must have been called to connect to a broker.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    pTopicName - Pointer to topic name to un-subscribe from.

  Returns:
    WDRV_WINC_STATUS_OK             - An unsubscribe was sent.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTUnsubscribe
(
    DRV_HANDLE handle,
    const char* pTopicName
);

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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.
    WDRV_WINC_MQTTConnect must have been called to connect to a broker.

  Parameters:
    handle   - Client handle obtained by a call to WDRV_WINC_Open.
    pKey     - Pointer to UTF-8 key name.
    lenKey   - Length of key name.
    pValue   - Pointer to UTF-8 value.
    lenValue - Length of value.

  Returns:
    WDRV_WINC_STATUS_OK             - Request was accepted.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    Setting pValue to NULL or lenValue to zero will cause the user property
    key to be removed.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTUserPropSet
(
    DRV_HANDLE handle,
    const uint8_t *pKey,
    size_t lenKey,
    const uint8_t *pValue,
    size_t lenValue
);

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

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.
    WDRV_WINC_MQTTConnect must have been called to connect to a broker.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    pfUserPropCb - Pointer to callback function to call.

  Returns:
    WDRV_WINC_STATUS_OK             - Request was accepted.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTUserPropCallbackSet
(
    DRV_HANDLE handle,
    WDRV_WINC_MQTT_USER_PROP_CALLBACK pfUserPropCb
);

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
    Ensures all elements of the structure are initialized to safe default values.

  Precondition:
    None.

  Parameters:
    pMsgInfo - Pointer to message info structure.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoDefaultSet
(
    WDRV_WINC_MQTT_MSG_INFO *pMsgInfo
);

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

  Precondition:
    WDRV_WINC_MQTTMsgInfoDefaultSet must have been called.

  Parameters:
    pMsgInfo - Pointer to message info structure.
    qos      - QoS value to assign.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoQoSSet
(
    WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    WDRV_WINC_MQTT_QOS_TYPE qos
);

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

  Precondition:
    WDRV_WINC_MQTTMsgInfoDefaultSet must have been called.

  Parameters:
    pMsgInfo - Pointer to message info structure.
    retain   - Flag indicating if message is to be retained.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoRetainSet
(
    WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    bool retain
);

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

  Precondition:
    WDRV_WINC_MQTTMsgInfoDefaultSet must have been called.
    WDRV_WINC_MQTTPubPropDefaultSet must have been called.

  Parameters:
    pMsgInfo - Pointer to message info structure.
    pPubProp - Pointer to publish properties to assign.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    The publish property structure is not copied into the message info
      structure, therefore the storage for pPubProp must exist while pMsgInfo
      is in use.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTMsgInfoPubPropSet
(
    WDRV_WINC_MQTT_MSG_INFO *pMsgInfo,
    WDRV_WINC_MQTT_PUB_PROP *pPubProp
);

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

  Precondition:
    None.

  Parameters:
    pPubProp - Pointer to publish properties structure.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropDefaultSet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp
);

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

  Precondition:
    WDRV_WINC_MQTTPubPropDefaultSet must have been called.

  Parameters:
    pPubProp      - Pointer to publish properties structure.
    payloadFormat - Payload format.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropPayloadFormatSet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    WDRV_WINC_MQTT_PROP_PAYLOAD_FORMAT_TYPE payloadFormat
);

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

  Precondition:
    WDRV_WINC_MQTTPubPropDefaultSet must have been called.

  Parameters:
    pPubProp   - Pointer to publish properties structure.
    expiryTime - Message expiry time.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropMessageExpirySet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    uint32_t expiryTime
);

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

  Precondition:
    WDRV_WINC_MQTTPubPropDefaultSet must have been called.

  Parameters:
    pPubProp     - Pointer to publish properties structure.
    pContentType - Pointer to content type.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    If pContentType is NULL the content type property field of the structure
      is cleared.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropContentTypeSet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    const uint8_t *pContentType
);

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

  Precondition:
    WDRV_WINC_MQTTPubPropDefaultSet must have been called.

  Parameters:
    pPubProp  - Pointer to publish properties structure.
    delayTime - The will delay time.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    This is only applicable for LWT messages submitted during the MQTT CONNECT
      message stage.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropWillDelaySet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    uint32_t delayTime
);

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

  Description:
    Sets the topic alias property in the publish property structure.

  Precondition:
    WDRV_WINC_MQTTPubPropDefaultSet must have been called.

  Parameters:
    pPubProp   - Pointer to publish properties structure.
    topicAlias - Topic alias.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTPubPropTopicAliasSet
(
    WDRV_WINC_MQTT_PUB_PROP *pPubProp,
    uint16_t topicAlias
);

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

  Precondition:
    None.

  Parameters:
    pSubProp - Pointer to subscribe properties structure.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTSubPropDefaultSet
(
    WDRV_WINC_MQTT_SUB_PROP *pSubProp
);

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

  Precondition:
    WDRV_WINC_MQTTSubPropDefaultSet must have been called.

  Parameters:
    pSubProp - Pointer to subscribe properties structure.
    subId    - Subscription ID.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTSubPropSubscriptIDSet
(
    WDRV_WINC_MQTT_SUB_PROP *pSubProp,
    uint32_t subId
);

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

  Precondition:
    None.

  Parameters:
    pConnProp - Pointer to connection properties structure.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropDefaultSet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp
);

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

  Precondition:
    WDRV_WINC_MQTTConnPropDefaultSet must have been called.

  Parameters:
    pConnProp  - Pointer to connection properties structure.
    expiryTime - Session expiry time.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropSessionExpirySet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp,
    uint32_t expiryTime
);

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

  Precondition:
    WDRV_WINC_MQTTConnPropDefaultSet must have been called.

  Parameters:
    pConnProp     - Pointer to connection properties structure.
    maxPacketSize - Maximum packet size.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropMaxPacketSet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp,
    uint32_t maxPacketSize
);

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

  Precondition:
    WDRV_WINC_MQTTConnPropDefaultSet must have been called.

  Parameters:
    pConnProp  - Pointer to connection properties structure.
    receiveMax - Receive maximum value.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropReceiveMaxSet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp,
    uint16_t receiveMax
);

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

  Precondition:
    WDRV_WINC_MQTTConnPropDefaultSet must have been called.

  Parameters:
    pConnProp     - Pointer to connection properties structure.
    topicAliasMax - Topic alias maximum.

  Returns:
    WDRV_WINC_STATUS_OK             - The property was updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_MQTTConnPropTopicAliasMaxSet
(
    WDRV_WINC_MQTT_CONN_PROP *pConnProp,
    uint16_t topicAliasMax
);

#endif /* WDRV_WINC_MOD_DISABLE_MQTT */
#endif /* WDRV_WINC_MQTT_H */
