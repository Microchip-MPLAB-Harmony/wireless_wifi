---
title: Harmony 3 Wireless Package
nav_order: 1
---

![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# MPLAB® Harmony 3 Wireless

MPLAB® Harmony 3 is an extension of the MPLAB® ecosystem for creating
embedded firmware solutions for Microchip 32-bit SAM and PIC® microcontroller
and microprocessor devices.  Refer to the following links for more information.

- [Microchip 32-bit MCUs](https://www.microchip.com/design-centers/32-bit)
- [Microchip 32-bit MPUs](https://www.microchip.com/design-centers/32-bit-mpus)
- [Microchip MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)
- [Microchip MPLAB® Harmony](https://www.microchip.com/mplab/mplab-harmony)
- [Microchip MPLAB® Harmony Pages](https://microchip-mplab-harmony.github.io/)

This repository contains the MPLAB® Harmony 3 Wireless solutions and example applications.
Quickly incorporate connectivity to your designs with wireless ICs, modules, 
software and development kits that make connecting effortless for your customers.
Our comprehensive wireless portfolio has the technology to meet your range, 
data rate, interoperability, frequency and topology needs. Refer to
the following links for release notes, training materials, and interface
reference information.

- [Release Notes](./release_notes.md)
- [MPLAB® Harmony License](mplab_harmony_license.md)
- [MPLAB® Harmony 3 Wireless Wiki](https://github.com/Microchip-MPLAB-Harmony/wireless/wiki)
- [MPLAB® Harmony 3 Wireless API Help](https://microchip-mplab-harmony.github.io/wireless)
- [MPLAB® Harmony 3 WINC Wireless APIs](doc/WINC%20Driver.pdf)
- [MPLAB® Harmony 3 PIC32MZW1 Wireless APIs](driver/readme.md)

<br />
<br />

# Contents Summary

| Folder     | Description                                               |
| ---        | ---                                                       |
| apps       | Example applications for Wireless library components      |
| config     | Wireless module configuration file                        |
| doc        | Wireless library help documentation (chm)                 |
| docs       | Wireless library help documentation (html)                |
| drivers    | Driver/Firmware files for different products              |
| system     | Application Service Architecture                          |
| utilities  | Contains root certs, firmware programmer/downloader       |

<br />
<br />

## Code Examples

The following applications are provided to demonstrate the typical or interesting usage models of one or more wireless drivers/libraries.

## PIC32MZW1

##### Note: PIC32MZ W1 Software User’s Guide is available at https://www.microchip.com/PIC32MZW1


| Name | Status|
|:---------|:-----------|
| [Wi-Fi STA](apps/wifi_sta/config_driver_readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [Wi-Fi Soft-AP](apps/wifi_ap/config_driver_readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [Wi-Fi STA - Wi-Fi Service Variant](apps/wifi_sta/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [Wi-Fi Soft-AP - Wi-Fi Service Variant](apps/wifi_ap/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [Paho MQTT Client](apps/paho_mqtt_client/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [Paho MQTT TLS Client](apps/paho_mqtt_tls_client/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [TCP Client](apps/tcp_client/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [TCP Server](apps/tcp_server/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [UDP Client](apps/udp_client/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [UDP Server](apps/udp_server/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [Weather Client](apps/weather_client/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [Wi-Fi Easy Config](apps/wifi_easy_config/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [WiFi-Ethernet Dual Interface](apps/wifi_eth_dual_interface/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |

<br />
<br />

## WINC

| Name | Status|
|:---------|:-----------|
| [AP Scan](apps/ap_scan/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic)|
| [Power Save Mode](apps/power_save_mode_example/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic)|
| [Serial Bridge](apps/serial_bridge/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic)|
| [TCP Client](apps/tcp_client/winc_readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic) |
| [Wi-Fi Provisioning via BLE](apps/wifi_provision_via_ble/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic)|
| [Wi-Fi Provisioning via Soft-AP](apps/wifi_provisioning_via_softap/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic)|
| [Wi-Fi Provisioning via WPS](apps/wifi_provisioning_via_wps/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic)|
| [Wi-Fi TCP Server in Soft-AP](apps/wifi_tcp_server_in_softap/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic)|
| [Wi-Fi WINC STA Bypass](apps/wifi_winc_sta_bypass/readme.md) | ![app-production](https://img.shields.io/badge/application-production-brightgreen?style=plastic)|

____

[![License](https://img.shields.io/badge/license-Harmony%20license-orange.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless/blob/master/mplab_harmony_license.md)
[![Latest release](https://img.shields.io/github/release/Microchip-MPLAB-Harmony/wireless.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless/releases/latest)
[![Latest release date](https://img.shields.io/github/release-date/Microchip-MPLAB-Harmony/wireless.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless/releases/latest)
[![Commit activity](https://img.shields.io/github/commit-activity/y/Microchip-MPLAB-Harmony/wireless.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless/graphs/commit-activity)
[![Contributors](https://img.shields.io/github/contributors-anon/Microchip-MPLAB-Harmony/wireless.svg)]()

____

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/user/MicrochipTechnology)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/microchip-technology)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/microchiptechnology/)
[![Follow us on Twitter](https://img.shields.io/twitter/follow/MicrochipTech.svg?style=social)](https://twitter.com/MicrochipTech)

[![](https://img.shields.io/github/stars/Microchip-MPLAB-Harmony/wireless.svg?style=social)]()
[![](https://img.shields.io/github/watchers/Microchip-MPLAB-Harmony/wireless.svg?style=social)]()


