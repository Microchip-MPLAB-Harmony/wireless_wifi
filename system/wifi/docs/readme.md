---
parent: Services
title: Wi-Fi Service
has_children: true
has_toc: false
nav_order: 1

family: PIC32MZW1
function: Wi-Fi System Service
---

# Wi-Fi System Service

Wi-Fi System Service Library provides an application programming interface (API) through which user can request Wi-Fi connectivity functionalities.The Wi-Fi System Service use Wi-Fi driver APIs.

Key Features:
- Configuration of Station mode(STA) 
    - Security Support:
        - Open - No security 
        - WPA2 
        - WPAWPA2(Mixed) mode
        - WPA2WPA3(Mixed) 
        - WPA3 
    - Self Healing<br>
        - if the connection for some reason breaks, the service shall take care of reconnecting the same internally. 
- Configuration of Soft Access point mode(AP) 
    - Security Support: 
        - Open - No security 
        - WPA2
        - WPAWPA2(Mixed) 
        - WPA2WPA3(Mixed) 
        - WPA3
        - Hidden Access Point(AP)  

The Wi-Fi System Service provides simple API's to enable Station(STA) or Access Point(AP) functionalities. Multiple clients can request the Wi-Fi system service functionalities like connection request,disconnect request,scan request,etc. User is not required to have Wi-Fi domain knowledge to developed station(STA) or access point(AP) application using Wi-Fi System Service.

* [Using the library](usage.md/#using-the-library)

* [Configuring the library](configuration.md/#configuring-the-library)

* [Library interface](interface.md)
