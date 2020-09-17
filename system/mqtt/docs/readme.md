---
parent: Services
title: MQTT Service
has_children: true
has_toc: false
nav_order: 1

family: PIC32MZW1
function: MQTT System Service
---

# MQTT System Service

MQTT System Service Library provides an application programming interface (API) to manage MQTT functionalities. The MQTT System Service internally uses the third party Paho MQTT software for MQTT support.

Note that currently MQTT System Service is supported with WiFi interface only.

Key Features:
- Supports MQTT Client 
- Supports TLS for MQTT Connection
- Supports Self Healing, that is if the connection for some reason breaks, the service shall take care of reconnecting the same internally.  

The MQTT System Service provides simple API's to enable MQTT functionalities like publishing, and subscribing to a topic. 

* [Using the library](usage.md/#using-the-library)

* [Configuring the library](configuration.md/#configuring-the-library)

* [Library interface](interface.md)
