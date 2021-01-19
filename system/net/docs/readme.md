---
parent: Services
title: Net Service
has_children: true
has_toc: false
nav_order: 1

family: PIC32MZW1
function: Net System Service
---

# Net System Service

Net System Service Library provides an application programming interface (API) to manage TCPIP Networking functionalities. The Net System Service uses the NetPres APIs for achieving these functionalities. 

Key Features:
- Supports Client/ Server Mode for IP Network Connectivity
- Supports TCP and UDP Protocols of IP
- Supports TLS for TCP Connection
- Supports Self Healing, that is if the connection for some reason breaks, the service shall take care of reconnecting the same internally.  

The Net System Service provides simple API's to enable network stack functionalities. Multiple clients can request the Net system service functionalities like tcp/udp connection request, tcp/udp disconnect request, sending and receiving data, etc. 

* [Using the library](usage.md/#using-the-library)

* [Configuring the library](configuration.md/#configuring-the-library)

* [Library interface](interface.md)
