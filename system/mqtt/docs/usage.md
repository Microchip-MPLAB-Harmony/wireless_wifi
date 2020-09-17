---
grand_parent: Services
parent: MQTT Service
title: MQTT System Service Usage
has_toc: true
nav_order: 1
---

# MQTT System Service Usage
## Description
The MQTT System Service provides simple APIs to enable MQTT Client Connectivity to a configured MQTT Broker. The User need not take care of intermediate states of a MQTT Connection, as the Service internally takes care of that. User is not required to have Security domain knowledge to establish a secured connection via the application using MQTT System Service library.

### Command Line:
User can follow below commands for MQTT System Service: 

1. sysmqtthelp 
    
    MQTT System Service help command which displays the supported CLI commands
    ![](./images/sysmqtthelp_cli.png)

2. sysmqtt open 
    
     Command for Reconfiguring an already open instance of MQTT System Service 
    ![](./images/sysmqttopen_cli.png)
    Note: Once the User has configured all the params, the last command for opening the new connection should 'sysmqtt open <instance> apply'

3. sysmqtt close 

    Command to close the instance of MQTT System Service 
    ![](./images/sysmqttclose_cli.png)

4. sysmqtt send 

    Command to send message on a topic for the instance of MQTT System Service 
    ![](./images/sysmqttsend_cli.png)

5. sysmqtt sunbscribe 

    Command to subscribe to a topic to receive message coming on that topic 
    ![](./images/sysmqttsubscribe_cli.png)

6. sysmqtt unsunbscribe 

    Command to unsubscribe from a topic 
    ![](./images/sysmqttunsubscribe_cli.png)

7. sysmqtt get info 
    
    Command for knowing the Current Information for all the Instances of Net System Service 
    ![](./images/sysmqttgetinfo_cli.png)
    

## Abstraction Model

The MQTT System Service library provides an abstraction to the MQTT APIs to provide following functionalities.

- Connectivity for MQTT Client 
- Secured Connectivity using TLS 
- Self Healing 
- Reduce code user has to write 
- Reduce time to develop and maintain 
 
The following diagram depicts the MQTT System Service abstraction model. 

![](./images/MqttService_abstract.png)

## How The Library Works

By default MHC generated code provides all the functionalities to enable MQTT Client applicatation, with secured or unsecured connectivity. User needs to configure the required MQTT Brokerconfiguration using MHC. User needs to call the SYS_MQTT_Connect() API with a valid callback to open an instance of the MQTT Client configured in the MHC. 

![](./images/MqttConnect.png)

The User Application is expected to call SYS_MQTT_Task() API periodically as this API ensures that the MQTT System service is able to execute its state machine to process any messages and invoke the user callback for any events. 

The User Application can call SYS_MQTT_Publish()/ SYS_MQTT_Subscribe() API in case it wants to publish message to a topic or receive messages on a topic.

![](./images/MqttPublish.png)
![](./images/MqttSubscribe.png)

The User Application when enables Auto-Reconnect, it enables the self healing feature of the MQTT Service. When this feature is enabled, the service will automatically try to establish connection with the MQTT Broker whenever a connection breaks.
![](./images/MqttSelfHealing.png)


