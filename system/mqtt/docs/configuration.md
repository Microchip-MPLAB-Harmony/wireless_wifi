---
grand_parent: Services
parent: MQTT Service
title: MQTT System Service Configuration
has_toc: true
nav_order: 1
---

# MQTT System Service Configuration
The MQTT System Service library should be configured through the MHC. When user selects the MQTT System Service library, all the required dependencies components are added automatically in the MHC configuration.

The following figure shows the MHC configuration window for configuring the MQTT System Service and a brief description of various configuration options.
- Open the MHC 3
- Drag the MQTT Service Module into the Active components from the Available components
{:refdef: style="text-align: center;"}
![](./images/Mqtt_service_MHC.png)
{: refdef}

- Configure the various parameters of Basic Configuration
{:refdef: style="text-align: center;"}
![](./images/Mqtt_Basic_Service.png)
{: refdef}

| Parameter Name | Default   Value | Description |
|-|-|-|
| Broker Name |  | Name of the MQTT   Broker |
| Server Port |  | Port   number of the MQTT Broker at which the MQTT Client should connect |
| Enable TLS | FALSE | If   TRUE, the MQTT connection should use TLS while connecting to the broker. If   FALSE, the MQTT connection should not use TLS. |
| Client Id |  | MQTT   Client Id should be unique for the Broker. If left empty, the Id will be   generated randomly |


- Configure the various parameters of Basic Configuration
{:refdef: style="text-align: center;"}
![](./images/Mqtt_Adv_Service.png)
{: refdef}

| Parameter Name | Default   Value | Description |
|-|-|-|
| Enable   Auto Reconnect | TRUE | If TRUE, the MQTT Service will auto reconnect to the Broker if   connection is broken. If FALSE, the customer application needs to take care   of triggering the connection process again. |
| Enable   Clean Session | TRUE | If TRUE, the MQTT Client shall tell the Broker that the session is   clean, else it will let the Broker know that the Session is a continuation of   the previous session |
| KeepAlive Interval | 60 sec | If no   data flows over an open connection for a certain KeepAliveInterval then the   client will generate a PINGREQ and expect to receive a PINGRESP from the   broker. This message exchange confirms that the connection is open and   working |
| Username/   Password | Disabled | In case   the connection to Broker needs a Username and Password |
| Last   Will and testament | Disabled | LWT   Configuration has the following parameters – <br>         1. Topic, <br>         2. QoS, <br>         3. Retain, and <br>         4. Message <br>This   ‘Message’ will be sent on the ‘Topic’ whenever the Broker finds that there is   an ungraceful disconnection with the Client. |

- Configure the remaining parameters
{:refdef: style="text-align: center;"}
![](./images/Mqtt_Remaining_Service.png)
{: refdef}

| Parameter Name | Default   Value | Description |
|-|-|-|
| Subscription Topic | Disabled | Subscription configuration has 2 Parameters – <br>     1.	Topic and <br>     2.	Qos. (0 (Atmost Once), <br>                1 (Atleast Once),<br>                2 (Exactly Once))<br>     The User can configure these parameters to subscribe to a Topic to receive   messages. |
| Publish to Topic | Disabled | Publishing a message to Topic has 3 Parameters –   <br>     1. Topic and <br>     2. Qos ( 0 (Atmost Once), 1 (Atleast Once), 2 (Exactly Once))<br>     3. Retain: If the Broker should retain the message<br>     The User can configure these parameters to and use them along with the   message to send it on a particular Topic. |
| Enable Debug Logs | Disabled | Enabling this flag compiles in debug logs and user can enable them   at runtime. The user can use the following CLI commands to enable/ disable   levels and flows for the MQTT service:<br>     1.	sysmqtt debug level <value> <br>     2.	sysmqtt debug flow <value> |

All of the required files are automatically added into the MPLAB X IDE project by the MHC when the MQTT Service is selected for use.
