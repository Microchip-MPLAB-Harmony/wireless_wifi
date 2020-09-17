---
grand_parent: Services
parent: MQTT Service
title: MQTT System Service Interface
has_toc: true
nav_order: 2
---

# MQTT System Service Interface
{: .no_toc }

### Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---


## Data Types and Constants Summary

| Name | Description |
|-|-|
| [SYS_MQTT_STATUS](#SYS_MQTT_STATUS) | Identifies the current status of the Sys Mqtt Instance. |
| [SYS_MQTT_RESULT](#SYS_MQTT_RESULT) | Identifies the return values for the Sys Mqtt APIs. |
| [SYS_MQTT_BrokerConfig](#SYS_MQTT_BrokerConfig) | Used for passing on the configuration related to the MQTT Broker |
| [SYS_MQTT_SubscribeConfig](#SYS_MQTT_SubscribeConfig) | Used for passing on the configuration related to the MQTT Subtopics the user |
| [SYS_MQTT_PublishConfig](#SYS_MQTT_PublishConfig) | Used for Reading the message that has been received on a topic subscribed to. |
| [SYS_MQTT_PublishTopicCfg](#SYS_MQTT_PublishTopicCfg) | Used for publishing a message on a topic. It contains the config related to the Topic |
| [SYS_MQTT_EVENT_TYPE](#SYS_MQTT_EVENT_TYPE) | Event Message Type which comes with the Callback SYS_MQTT_CALLBACK() |
| [SYS_MQTT_Config](#SYS_MQTT_Config) | Used for passing on the configuration related to the either MQTT Broker, |

## Initialization functions Summary

| Name | Description |
|-|-|
| [SYS_MQTT_Initialize](#SYS_MQTT_Initialize) | Returns success/ failure for initialization of data structures of the MQTT service |
| [SYS_MQTT_Deinitialize](#SYS_MQTT_Deinitialize) | Deinitialization of data structures of the MQTT service |

## Status functions Summary

| Name | Description |
|-|-|
| [SYS_MQTT_GetStatus](#SYS_MQTT_GetStatus) | Returns System MQTT instance status. |

## Setup functions Summary

| Name | Description |
|-|-|
| [SYS_MQTT_Connect](#SYS_MQTT_Connect) | Connects to the configured MQTT Broker. |
| [SYS_MQTT_Disconnect](#SYS_MQTT_Disconnect) | Disconnects from the MQTT Server |
| [SYS_MQTT_Task](#SYS_MQTT_Task) | Executes the MQTT Service State Machine |
| [SYS_MQTT_Subscribe](#SYS_MQTT_Subscribe) | Returns success/ failure for the subscribing to a Topic by the user. |
| [SYS_MQTT_Unsubscribe](#SYS_MQTT_Unsubscribe) | Returns success/ failure for the unsubscribing to a Topic by the user. |

## Data Exchange functions Summary

| Name | Description |
|-|-|
| [SYS_MQTT_Publish](#SYS_MQTT_Publish) | Returns success/ failure for the publishing of message asked by the user. |
| [SYS_MQTT_CALLBACK](#SYS_MQTT_CALLBACK) | Pointer to a MQTT system service callback function. |

## Data Types and Constants


### SYS_MQTT_STATUS

**Summary**

Identifies the current status of the Sys Mqtt Instance.  

**Remarks**

None. 

```c
typedef enum {
// Idle
SYS_MQTT_STATUS_IDLE = 0,

// Lower Layer is DOWN
SYS_MQTT_STATUS_LOWER_LAYER_DOWN,

// Net Client connecting to Net Server
SYS_MQTT_STATUS_SOCK_CLIENT_CONNECTING,

// Net Instance connected to the peer
SYS_MQTT_STATUS_SOCK_CONNECTED,

// Net Instance Failed to open socket
SYS_MQTT_STATUS_SOCK_OPEN_FAILED,

// Lower Layer is DOWN
SYS_MQTT_STATUS_MQTT_CONNECTED,

// Net Instance in disconnected state
SYS_MQTT_STATUS_MQTT_DISCONNECTING,

// Net Instance in disconnected state
SYS_MQTT_STATUS_MQTT_DISCONNECTED,

// Lower Layer is DOWN
SYS_MQTT_STATUS_MQTT_CONN_FAILED,

// Wait for Connect Ack from Broker
SYS_MQTT_STATUS_WAIT_FOR_MQTT_CONACK,

// Send Mqtt Connect to Broker
SYS_MQTT_STATUS_SEND_MQTT_CONN,

// Wait for Subscribe Ack from Broker
SYS_MQTT_STATUS_WAIT_FOR_MQTT_SUBACK,

// Wait for Publish Ack from Broker
SYS_MQTT_STATUS_WAIT_FOR_MQTT_PUBACK,

// Wait for Unsibscribe Ack from Broker
SYS_MQTT_STATUS_WAIT_FOR_MQTT_UNSUBACK,
} SYS_MQTT_STATUS;
```

### SYS_MQTT_RESULT


**Summary**

Identifies the return values for the Sys Mqtt APIs.  

**Remarks**

None. 

```c
typedef enum {
// Success
SYS_MQTT_SUCCESS = 0,

// Failure
SYS_MQTT_FAILURE = -1,

// Sys NET Service Down
SYS_MQTT_SERVICE_DOWN = -2,

// Sys NET Available Put Buffer not enough for xmitting the Data
SYS_MQTT_SEM_OPERATION_FAILURE = -5,

// Sys NET Invalid Handle
SYS_MQTT_INVALID_HANDLE = -6,
} SYS_MQTT_RESULT;
```

### SYS_MQTT_BrokerConfig


**Summary**

Used for passing on the configuration related to the MQTT Broker  

**Remarks**

None. 

```c
typedef struct {
//to know which of the Configurations are valid
SYS_MQTT_Vendor_Type eVendorType;

// MQTT Broker/ Server Name
char 		brokerName[SYS_MQTT_MAX_BROKER_NAME_LEN];

// MQTT Server Port
uint16_t	serverPort;

// Keep Alive Interval for the Mqtt Session
uint16_t keepAliveInterval;

// MQTT Client ID
char		clientId[SYS_MQTT_CLIENT_ID_MAX_LEN];

// MQTT Username
char		username[SYS_MQTT_USER_NAME_MAX_LEN];

// MQTT password
char		password[SYS_MQTT_PASSWORD_MAX_LEN];

// TLS is Enabled
bool		tlsEnabled;

// AutoConnect is Enabled
bool		autoConnect;
} SYS_MQTT_BrokerConfig;
```

### SYS_MQTT_SubscribeConfig


**Summary**

Used for passing on the configuration related to the MQTT Subtopics the user wants to subscribe to.  

**Remarks**

This Configuration is passed via the SYS_MQTT_Connect() function or the SYS_MQTT_CtrlMsg() function 

```c
typedef struct {
uint8_t entryValid;

//Qos (0/ 1/ 2)
uint8_t	qos;

//Name of the Topic Subscribing to
char	topicName[SYS_MQTT_TOPIC_NAME_MAX_LEN];
} SYS_MQTT_SubscribeConfig;
```

### SYS_MQTT_PublishConfig


**Summary**

Used for Reading the message that has been received on a topic subscribed to. The structure is also used for passing on the LWT config when connecting to MQTT Broker.  

**Remarks**

This Message is passed to the Application via the SYS_MQTT_CALLBACK() function 

```c
typedef struct {
//Qos (0/ 1/ 2)
uint8_t	qos;

//Retain (0/1) - Message needs to be retained by the Broker till every subscriber receives it
uint8_t	retain;

//Message to be Published
uint8_t	message[SYS_MQTT_MSG_MAX_LEN];

//Message Length
uint16_t	messageLength;

//Topic on which to Publish the message
char	*topicName;

//Topic Length
uint16_t	topicLength;
} SYS_MQTT_PublishConfig;
```

### SYS_MQTT_PublishTopicCfg


**Summary**

Used for publishing a message on a topic. It contains the config related to the Topic  

**Remarks**

This Message is passed from the Application to the MQTT servuce via the SYS_MQTT_Publish() function 

```c
typedef struct {
//Qos (0/ 1/ 2)
uint8_t	qos;

//Retain (0/1) - Message needs to be retained by the Broker till every subscriber receives it
uint8_t	retain;

//Topic on which to Publish the message
char	topicName[SYS_MQTT_TOPIC_NAME_MAX_LEN];

//Topic Length
uint16_t	topicLength;
} SYS_MQTT_PublishTopicCfg;
```

### SYS_MQTT_EVENT_TYPE


**Summary**

Event Message Type which comes with the Callback SYS_MQTT_CALLBACK() informing the user of the event that has occured.  

**Remarks**

None. 

```c
typedef enum {
//Message received on a topic subscribed to
SYS_MQTT_EVENT_MSG_RCVD = 0,

//MQTT Client for Disconnected
SYS_MQTT_EVENT_MSG_DISCONNECTED,

//MQTT Client Connected
SYS_MQTT_EVENT_MSG_CONNECTED,

//MQTT Client Subscribed to a Grp
SYS_MQTT_EVENT_MSG_SUBSCRIBED,

//MQTT Client UnSubscribed from a Grp
SYS_MQTT_EVENT_MSG_UNSUBSCRIBED,

//MQTT Client Published to a Grp
SYS_MQTT_EVENT_MSG_PUBLISHED,

//MQTT Client ConnAck TimeOut
SYS_MQTT_EVENT_MSG_CONNACK_TO,

//MQTT Client SubAck TimeOut
SYS_MQTT_EVENT_MSG_SUBACK_TO,

//MQTT Client PubAck TimeOut
SYS_MQTT_EVENT_MSG_PUBACK_TO,

//MQTT Client PubAck TimeOut
SYS_MQTT_EVENT_MSG_UNSUBACK_TO,
} SYS_MQTT_EVENT_TYPE;
```

### SYS_MQTT_Config


**Summary**

Used for passing on the configuration related to the either MQTT Broker, or the Cloud Vendors AWS/ Azure, etc.  

**Remarks**

None. 

```c
typedef struct {
//MQTT Broker Configuration
SYS_MQTT_BrokerConfig sBrokerConfig;

//Number of Topis Subscribed to (0-SYS_MQTT_MAX_TOPICS)
uint8_t							subscribeCount;

//Config for all the Topics Subscribed to
SYS_MQTT_SubscribeConfig sSubscribeConfig[SYS_MQTT_SUB_MAX_TOPICS];

//If last will and testament(LWT) is enabled or not
bool bLwtEnabled;

// LWT Configuration
SYS_MQTT_PublishConfig sLwtConfig;
} SYS_MQTT_Config;
```

## Initialization functions


### SYS_MQTT_Initialize

**Function**

```c
int32_t SYS_MQTT_Initialize()
```

**Summary**

Returns success/ failure for initialization of data structures of the MQTT service  

**Description**

This function is used for initializing the data structures of the MQTT service and is called from within the System Task.  

**Returns**

*SYS_NET_SUCCESS* - Indicates the data structures were initialized successfully 

*SYS_NET_FAILURE* - Indicates that it failed to initialize the data structures.  

**Example**

```c
if( SYS_MQTT_Initialize() == SYS_MQTT_SUCCESS)
{
}
```

**Remarks**

If the MQTT system service is enabled using MHC, then auto generated code will take care of its initialization. 

### SYS_MQTT_Deinitialize

**Function**

```c
void SYS_MQTT_Deinitialize()
```

**Summary**

Deinitialization of data structures of the MQTT service  

**Description**

This function is used for freeing the allocated data structures for the MQTT service.  

**Example**

```c
SYS_MQTT_Deinitialize()
```

**Remarks**

None 
## Status functions




### SYS_MQTT_GetStatus

**Function**

```c
SYS_MQTT_STATUS SYS_MQTT_GetStatus ( SYS_MODULE_OBJ object )
```

**Summary**

Returns System MQTT instance status.  

**Description**

This function returns the current status of the System MQTT instance.  

**Precondition**

SYS_MQTT_Connect should have been called before calling this function  

**Parameters**

*object* - SYS MQTT object handle, returned from SYS_MQTT_Connect  

**Returns**

SYS_MQTT_STATUS  

**Example**

```c
// Handle "objSysMqtt" value must have been returned from SYS_MQTT_Connect.
if (SYS_MQTT_GetStatus (objSysMqtt) == SYS_MQTT_STATUS_WAIT_FOR_MQTT_CONACK)
{
// MQTT system service is initialized, and Waiting for the Connect Ack
// from the Broker for the Connect Packet sent by DUT to it.
}
```

**Remarks**

None. 
## Setup functions




### SYS_MQTT_Connect

**Function**

```c
SYS_MODULE_OBJSYS_MODULE_OBJ* SYS_MQTT_Connect(SYS_MQTT_Config *cfg,
SYS_MQTT_CALLBACK fn MqttFn,
void *cookie);
```

**Summary**

Connects to the configured MQTT Broker.  

**Description**

This function opens a new instance and connects to the configured MQTT Broker.  

**Parameters**

cfg 		- Configuration based on which the Cloud Service needs to Open<br>  MqttFn 	- Function pointer to the Callback to be called in case of an event<br>  cookie		- Cookie passed as one of the params in the Callback for the user to identify the service instance<br>  

**Returns**

If successful, returns a valid handle to an object. Otherwise, it returns SYS_MODULE_OBJ_INVALID.  

**Example**

```c
SYS_MQTT_Config 	g_sMqttSrvcCfg;
SYS_MODULE_OBJ 			g_MqttSrvcHandle;

memset(&g_sMqttSrvcCfg, 0, sizeof(g_sMqttSrvcCfg));

g_sMattSrvcCfg.configBitmask |= SYS_MQTT_CONFIG_MASK_MQTT;

strcpy(g_sMqttSrvcCfg.mqttConfig.brokerConfig.brokerName,
"test.mosquitto.org", strlen("test.mosquitto.org"));

g_sMqttSrvcCfg.mqttConfig.brokerConfig.serverPort = 1883;

strcpy(g_sMqttSrvcCfg.mqttConfig.brokerConfig.clientId,
"pic32mzw1", strlen("pic32maw1"));

g_sMqttSrvcCfg.mqttConfig.brokerConfig.autoConnect = 1;

g_sMqttSrvcCfg.mqttConfig.brokerConfig.tlsEnabled = 0;

g_sMqttSrvcCfg.mqttConfig.subscribeCount = 1;

strcpy(g_sMqttSrvcCfg.mqttConfig.subscribeConfig[0].topicName,
"house/temperature/first_floor/kitchen",
strlen("house/temperature/first_floor/kitchen"));

g_sMqttSrvcCfg.mqttConfig.subscribeConfig[0].qos = 1;

g_MqttSrvcHandle = SYS_MQTT_Connect(&g_sMqttSrvcCfg, MqttSrvcCallback, 0);
if (g_MqttSrvcHandle == SYS_MODULE_OBJ_INVALID)
{
// Handle error
}
```

**Remarks**

This routine should be called only once when the user is configuring the Mqtt service 

### SYS_MQTT_Disconnect

**Function**

```c
void SYS_MQTT_Disconnect(SYS_MODULE_OBJ obj)
```

**Summary**

Disconnects from the MQTT Server  

**Description**

This function is used for disconnecting from the MQTT Server.  

**Precondition**

SYS_MQTT_Connect should have been called.  

**Parameters**

obj 		- SYS_MQTT object handle, returned from SYS_MQTT_Connect  

**Returns**

None  

**Example**

```c
// Handle "objSysMqtt" value must have been returned from SYS_MQTT_Connect.
SYS_MQTT_Disconnect(objSysMqtt);
```

**Remarks**

None. 

### SYS_MQTT_Task

**Function**

```c
void SYS_MQTT_Task(SYS_MODULE_OBJ obj)
```

**Summary**

Executes the MQTT Service State Machine  

**Description**

This function ensures that the MQTT service is able to execute its state machine to process any messages and invoke the user callback for any events.  

**Precondition**

SYS_MQTT_Connect should have been called before calling this function  

**Parameters**

*obj* - SYS MQTT object handle, returned from SYS_MQTT_Connect  

**Returns**

None  

**Example**

```c
// Handle "objSysMqtt" value must have been returned from SYS_MQTT_Connect.
while(1)
{
...
SYS_MQTT_Task(objSysMqtt);
...
}
```


### SYS_MQTT_Subscribe

**Function**

```c
int32_t SYS_MQTT_Subscribe(SYS_MODULE_OBJ obj,
SYS_MQTT_SubscribeConfig *subConfig);
```

**Summary**

Returns success/ failure for the subscribing to a Topic by the user.  

**Description**

This function is used for subscribing to a Topic.  

**Precondition**

SYS_MQTT_Connect should have been called before calling this function  

**Parameters**

*obj* - SYS MQTT object handle, returned from SYS_MQTT_Connect <br> subConfig		- valid pointer to the Topic details on which to Subscribe <br>  

**Returns**

*SYS_MQTT_SUCCESS* - Indicates that the Request was catered to successfully 

*SYS_MQTT_FAILURE* - Indicates that the Request failed  

**Example**

```c
SYS_MQTT_SubscribeConfig		sSubscribeCfg;

memset(&sSubscribeCfg, 0, sizeof(sSubscribeCfg));
sSubscribeCfg.qos = 1;
strcpy(sSubscribeCfg.topicName, "house/temperature/first_floor/kitchen");

// Handle "objSysMqtt" value must have been returned from SYS_MQTT_Connect.
if( SYS_MQTT_Subscribe(objSysMqtt, &sSubscribeCfg) == SYS_MQTT_SUCCESS)
{
}
```


### SYS_MQTT_Unsubscribe

**Function**

```c
int32_t SYS_MQTT_Unsubscribe(SYS_MODULE_OBJ obj, char *subTopic);
```

**Summary**

Returns success/ failure for the unsubscribing to a Topic by the user.  

**Description**

This function is used for Unsubscribing from a Topic.  

**Precondition**

SYS_MQTT_Connect should have been called before calling this function  

**Parameters**

*obj* - SYS MQTT object handle, returned from SYS_MQTT_Connect <br> subtopic		- Topic from which to unsubscribe <br>  

**Returns**

*SYS_MQTT_SUCCESS* - Indicates that the Request was catered to successfully 

*SYS_MQTT_FAILURE* - Indicates that the Request failed  

**Example**

```c
// Handle "objSysMqtt" value must have been returned from SYS_MQTT_Connect.
if( SYS_MQTT_Unsubscribe(objSysMqtt, "house/temperature/first_floor/kitchen") == SYS_MQTT_SUCCESS)
{
}
```

## Data Exchange functions


### SYS_MQTT_Publish

**Function**

```c
int32_t SYS_MQTT_Publish(SYS_MODULE_OBJ obj,
SYS_MQTT_PublishTopicCfg *psPubCfg, char *message, uint16_t message_len);
```

**Summary**

Returns success/ failure for the publishing of message on a topic by the user.  

**Description**

This function is used for Publishing a message on a Topic.  

**Precondition**

SYS_MQTT_Connect should have been called before calling this function  

**Parameters**

*obj* - SYS MQTT object handle, returned from SYS_MQTT_Connect <br> psPubCfg		- valid pointer to the Topic details on which to Publish <br> message		- Message to be published <br> 

*message_len* - Message length <br>  

**Returns**

*SYS_MQTT_SUCCESS* - Indicates that the Request was catered to successfully 

*SYS_MQTT_FAILURE* - Indicates that the Request failed  

**Example**

```c
SYS_MQTT_PublishTopicCfg		sTopicCfg;

memset(&sTopicCfg, 0, sizeof(sTopicCfg));
sTopicCfg.qos = 1;
sTopicCfg.retain = 1;
strcpy(sTopicCfg.topicName, "house/temperature/first_floor/kitchen");
sTopicCfg.topicLength = strlen("house/temperature/first_floor/kitchen");

// Handle "objSysMqtt" value must have been returned from SYS_MQTT_Connect.
if( SYS_MQTT_Publish(objSysMqtt, &sPublishCfg, "80.17", strlen("80.17")) == SYS_MQTT_SUCCESS)
{
}
```


### SYS_MQTT_CALLBACK

**Function**

```c
int32_t SYS_MQTT_CALLBACK(SYS_MQTT_EVENT_TYPE eEventType, void *data, uint16_t len, void* cookie);
```

**Summary**

Pointer to a MQTT system service callback function.  

**Description**

This data type defines a pointer to a Mqtt service callback function, thus defining the function signature. Callback functions may be registered by mqtt clients of the Mqtt service via the SYS_MQTT_Connect call.  

**Precondition**

Is a part of the Mqtt service Setup using the SYS_MQTT_Connect function  

**Parameters**

*eEventType	- event (SYS_MQTT_EVENT_TYPE)* - Message Received/ Got Disconnected <br> data	- Data (if any) related to the Event <br> len		- Length of the Data received <br> cookie 	- A context value, returned untouched to the client when the callback occurs. It can be used to identify the instance of the client who registered the callback.  

**Returns**

None.  

**Example**

```c
void MqttSrvcCallback(SYS_MQTT_EVENT_TYPE event, void *data, uint16_t len, void* cookie, )
{
switch(event)
{
case SYS_MQTT_EVENT_MSG_RCVD:
{
SYS_MQTT_PublishConfig	*psMsg = (SYS_MQTT_PublishConfig	*)data;
psMsg->message[psMsg->messageLength] = 0;
psMsg->topicName[psMsg->topicLength] = 0;
SYS_CONSOLE_PRINT("\nMqttCallback(): Msg received on Topic: %s ; Msg: %s\r\n",
psMsg->topicName, psMsg->message);
break;
}

case SYS_MQTT_EVENT_MSG_DISCONNECT:
{
SYS_CONSOLE_PRINT("CloudSrvcCallback(): MQTT DOWN");
break;
}
}
}
```
