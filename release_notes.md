![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip MPLAB® Harmony 3 Release Notes

## Wireless_wifi Release v3.6.1

### Dot Release Update for v3.6.1:
- WINC: Adds support for WINC3400 FW v1.4.3.
    - Add FragAttack countermeasures and drop WEP support.

**Other release notes for v3.6.0 apply here too**

## Wireless_wifi Release v3.6.0
This release includes additions and improvements to
**PIC32MZW1 and WINC**.

## Note: This release does not include the older WINC1500 v19.7.3 driver/firmware.
## Please make use of the Wireless_wifi release v3.5.1 if you have a strong reason to remain on v19.7.3

### New Features

- WINC: Support update for NET v3.7.4, ThreadX, - Harmony TCP/IP stack dependency and OSAL heap use
- WINC: Adds support for WINC1500 FW v19.7.6.
- WINC: Improved Gain Tables and carrier supporession margin.
- WINC & PIC32MZW1: Implement FragAttack Countermeasures.
- PIC32MZW1: Add API for Vendor specific IE in Becaon frame(soft AP).
- PIC32MZW1: Adapt WiFi firmware library alarm clock rate to PBCLK3 rate.


### Bug fixes
- PIC32MZW1: AP SSID leaks as 'wifi' when starting Soft-AP
- PIC32MZW1: Ethernet MAC driver unchecked pointer use
- PIC32MZW1: STA mode interop issue with APs that reverse the byte order of IGTK key id.
- PIC32MZW1: Fix rare Wake-up issue with Wireless powersave.

### Known Issues/Limitations
- PIC32MZW1: RegDomain name used as part of WDRV_PIC32MZW_RegDomainSet() supports limited customization.
- WINC MISRA warnings/false positives(same as previous releases):
    - driver/winc/drv/driver/m2m_wifi.c(1039) 	644 	9.1 	Variable 'strNetworkId' (line 1030) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]
    - driver/winc/wdrv_winc_authctx.c(122) 	530 	9.1 	Symbol 'PKCS1_RSA_PRIVATE_KEY' (line 105) not initialized [MISRA 2012 Rule 9.1, mandatory]
    - driver/winc/drv/driver/nmspi.c(928) 	644 	9.1 	Variable 'tmpBuf' (line 905) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]
    - driver/winc/drv/socket/socket.c(574) 	644 	9.1 	Variable 'pstrSock' (line 535) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]


## Wireless_wifi Release v3.5.1

### Dot Release Update for v3.5.1:
- PIC32MZW1: WLAN library update to support XC32 v3.01 compiler version.
- Minor correction in one of the API descriptions.

No other functional changes in the release.

**Other release notes for v3.5.0 apply here too**

## Wireless_wifi Release v3.5.0
This release includes additions and improvements to
the **PIC32MZW1 device only**.

**Devtools Note: PIC32MZW1 supports MPLABX IDE v5.50 & XC32 v2.50 version of the compiler with this release(not XC32 v3.00 yet).**

### New Features
- PIC32MZW1: Improved Scanning.
- PIC32MZW1: Configurable group rekey timeout API in softAP mode.
- PIC32MZW1: API to disconnect an associated STA associated with softAP based on the input association handle.

##### `Recommendation for UDP Rx usage: Applications which use the Harmony implementation of UDP Rx should update the UDP Rx Queue limit to 16 (up from the default value of 7). In Microchip Harmony Configurator, this setting is in: UDP -> Advanced Settings -> RX Queue Limit. `

### Bug fixes
- PIC32MZW1: Assert in call to WDRV_PIC32MZW_BSSConnect() if configUSE_TIMERS is enabled.
- PIC32MZW1: Data frames are sent as 'Data' not 'QOSData'
- PIC32MZW1: Ethernet driver double de-init causes crash on RTOS.
- PIC32MZW1: Handle scenario in TPC while packets with TSSI 0 are transmitted.

### Known Issues/Limitations
- PIC32MZW1: RegDomain name used as part of WDRV_PIC32MZW_RegDomainSet() supports limited customization.
- PIC32MZW1: AP SSID leaks as 'wifi' when starting Soft-AP.

## Wireless_wifi Release v3.4.1
### New Features
- Note: 'Wireless' repository renamed as 'Wireless_wifi'
- PIC32MZW1: Bifurcation of repo components (Firmware, Host Driver etc) from Example applications.
- WINC: Bifurcation of repo components (Firmware, Host Driver etc) from Example applications.
- Going Forward, example applications and system services for wireless_wifi products wil reside in separate repositories.
| Repository | Product family |
| ---------- | -------------- |
| [wireless_wifi](https://github.com/Microchip-MPLAB-Harmony/wireless_wifi) | Core wireless_wifi products repository |
| [wireless_apps_winc1500](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_winc1500) | Application examples for WINC1500 |
| [wireless_apps_winc3400](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_winc3400) | Application examples for WINC3400 |
| [wireless_apps_pic32mzw1_wfi32e01](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01) | Application examples for PIC32MZW1 |
| [wireless_system_pic32mzw1_wfi32e01](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01) | Wireless system Services for PIC32MZW1 |

### Bug fixes
- Not Applicable for this release
### Known Issues/Limitations
- Not Applicable for this release

## Wireless Release v3.4.0
### New Features
- PIC32MZW1: WPA3 in non-RTOS mode. WPA3 support (regardless of RTOS/non-RTOS mode) is now dependent on H3 crypto release v3.6.2 or later.
- PIC32MZW1: Adds Ethernet support in Net and MQTT services (only Wi-Fi and Wi-Fi + Ethernet modes are supported in this release).
- PIC32MZW1: Adds OTA example application.
- PIC32MZW1: Adds Touch + Wi-Fi example application.
- WINC: Adds support for WINC3400 FW v1.4.2.
- WINC: Adds support for WINC1500 FW v19.7.3.
- WINC: Adds support for WINC3400 FW v1.4.2 lite driver.
- WINC: Adds support for WINC1500 FW v19.7.3 lite driver.
### Bug fixes
- PIC32MZW1: soft-AP mode throughput improvements.
- PIC32MZW1: WEP shared key authentication failure with certain APs.
- PIC32MZW1: Wi-Fi connection behaviour is impacted if AP is on a channel that was not enabled in the last scan request.
- PIC32MZW1: Fixes Wi-Fi provisioning code generation issues seen in MAC and Linux.
- WINC: Updated documentation for WINC driver.
- WINC: Fixed BLE startup issue in WINC3400.
- WINC: Fixed FW update issue in Serial bridge with WINC1500/WINC3400.
### Known Issues/Limitations
- PIC32MZW1: QoS Data unsupported in transmit path.
- PIC32MZW1: RegDomain name used as part of WDRV_PIC32MZW_RegDomainSet() supports limited customization.

## Wireless Release v3.3.1
### Dot Release Updates for v3.3.1:
- PIC32MZW1: Include Paho MQTT dependency. 
- Documentation URL updates for application examples.

**Release notes for v3.3.0 apply**

## Wireless Release v3.3.0
### New Features
- WINC: Adds support for WINC3400 FW v1.3.1.
- WINC: Added Bypass mode application example for SAME54 + WINC3400.
- WINC: Added new application example support for SAME54.
- WINC: Updated cache maintenance option for PIC32MZ.
- PIC32MZW1: Update PIC32MZW1 WLAN driver to v3.1 with new application examples.
- PIC32MZW1: Basic 802.11 Powersave implementation.
- PIC32MZW1: New memory manager for improved stability.
- PIC32MZW1: OTA Support.
- PIC32MZW1: Support added for Wireless Services.

### Bug fixes
- WINC: Updated documentation for WINC driver and application examples.
- WINC: SAMD21 + WINC3400 application startup issues.
- PIC32MZW1: A run time exception occurs with the TCPIP network packet manager code generated by MHC.
- PIC32MZW1: Erroneous free of memory in UDP Rx Path.
- PIC32MZW1: Ethernet MAC address not loaded from OTP.
- PIC32MZW1: Stack overflow in Scan path.
- PIC32MZW1: DUT fails to connect to an AP after first connection attempt fails.
- PIC32MZW1: TCP Tx occasionally stalls when 11n is enabled.
- PIC32MZW1: Scanning is not supported in AP mode.

### Known Issues
- PIC32MZW1: WPA3 is not supported in non-RTOS mode.
- PIC32MZW1: QoS Data unsupported in transmit path.

- WINC MISRA warnings/false positives:
1. driver/winc/drv/driver/m2m_wifi.c(1140) 	644	9.1
Variable 'strNetworkId' (line 1131) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]
			
2. app.c(136) 	530	9.1
Symbol 'args' (line 135) not initialized [MISRA 2012 Rule 9.1, mandatory]
			
3. driver/winc/drv/driver/nmspi.c(904) 	644	9.1
Variable 'tmpBuf' (line 881) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]

- PIC32MZW1 MISRA warnings/false positives:
1. exceptions.c(122) 530 9.1
Symbol '_localStackPointerValue' (line 115) not initialized [MISRA 2012 Rule 9.1, mandatory]

2. library/tcpip/src/arp.c(979) 530 9.1
Symbol 'ARP_HASH_ENTRY' (line 93, file ../project/src/config/pic32mz_w1_curiosity/library/tcpip/src/arp_private.h) not initialized [MISRA 2012 Rule 9.1, mandatory]

3. driver/ethmac/src/dynamic/drv_ethmac.c(455) 	530 	9.1
Symbol 'DRV_ETHMAC_OTP_REC' (line 98) not initialized [MISRA 2012 Rule 9.1, mandatory]

- PIC32MZW1 System Service

1. RSA 4k certificate verification is not working with MQTT/NET Service as the System goes ‘Out of Memory’.

2. WPA3 feature in wireless system service is un-tested.

3. User should increase the UART Tx ring buffer size if output console data size is more than 1K

### Development Tools
- WINC: [MPLAB X v5.4 ](https://www.microchip.com/mplab/mplab-x-ide)
- WINC: [MPLAB® XC32 C/C++ Compiler v2.41](https://www.microchip.com/mplab/compilers)
- PIC32MZW1: [MPLAB X v5.4 ](https://www.microchip.com/mplab/mplab-x-ide)
- PIC32MZW1: [MPLAB® XC32 C/C++ Compiler v2.50](https://www.microchip.com/mplab/compilers)

## Wireless Release v3.2.0
### New Features
- WINC: Adds support/Callback for ECC callback.
- WINC: Adds Support for ARM Cortex M0+ Hosts through GPIO & PIO interrupts. Earlier only EIC Interrupts were allowed.
- WINC: Updated cache maintenance option for PIC32MZ.
- PIC32MZW1: Add WLAN driver support and application examples for PIC32MZW1 device.

### Bug fixes
- WINC: Fixed MHC Component Manager menu for Wireless Bypass mode (Supported only for WINC1500 in this release).
- WINC: Improved documentation for WINC driver and application examples.
- WINC: SYS_Console related changes to reflect updates in core/csp/bsp v3.7.0.

### Known Issues
- WINC: SAMD21 + WINC3400 examples have IDE specific dependencies which may prevent them from functioning correctly.
This is a known issue which will be solved in a future release.
- PIC32MZW1:  - A run time exception occurs with the TCPIP network packet manager code generated by MHC. The workaround for this is:
    - Addition of line ```#define TCPIP_IF_PIC32MZW1``` in the project's configuration.h file.
    - Addition of the check ```|| defined(TCPIP_IF_PIC32MZW1)``` as shown below for defining the MAC frame offset in the file tcpip_packet.c
    ```
    #if defined(TCPIP_IF_PIC32WK) || defined(TCPIP_IF_PIC32MZW1)
    #define TCPIP_MAC_FRAME_OFFSET      (34+4)  
    ```
    - Do NOT overwrite this workaround when regenerating the project.
    - This is a known issue which will be solved in a future release.
- PIC32MZW1: TCP Tx occasionally stalls when 11n is enabled.
- PIC32MZW1: STA mode connection problems when multiple APs are present with the same SSID but different security settings.

- PIC32MZW1: QoS Data unsupported in transmit path.
- MISRA false positives:
1. driver/winc/drv/driver/m2m_wifi.c(1140) 	644	9.1
Variable 'strNetworkId' (line 1131) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]
			
2. driver/winc/wdrv_winc_nvm.c(413) 	644	9.1
Variable 'strControl' (line 398) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]
			
3. app.c(136) 	530	9.1
Symbol 'args' (line 135) not initialized [MISRA 2012 Rule 9.1, mandatory]
			
4. driver/winc/drv/driver/nmspi.c(904) 	644	9.1
Variable 'tmpBuf' (line 881) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]

5. driver/winc/drv/driver/nmflash.c(690) 	530	9.1
Symbol 'write_init_params' (line 684) not initialized [MISRA 2012 Rule 9.1, mandatory]

6. wifi_provisioning_via_softap/example.c(269) 	644	9.1
Variable 'wep_parameters' (line 188) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]

7. exceptions.c(122) 530 9.1
Symbol '_localStackPointerValue' (line 115) not initialized [MISRA 2012 Rule 9.1, mandatory]

8. library/tcpip/src/arp.c(979) 530 9.1
Symbol 'ARP_HASH_ENTRY' (line 93, file ../project/src/config/pic32mz_w1_curiosity/library/tcpip/src/arp_private.h) not initialized [MISRA 2012 Rule 9.1, mandatory]

9. system/console/src/sys_console.c(340) 530 9.1
Symbol 'args' (line 320) not initialized [MISRA 2012 Rule 9.1, mandatory]

### Development Tools
- WINC: Same as v3.1.1
- PIC32MZW1: [MPLAB X v5.35 ](https://www.microchip.com/mplab/mplab-x-ide)
- PIC32MZW1: [MPLAB® XC32 C/C++ Compiler v2.40](https://www.microchip.com/mplab/compilers)

## Wireless Release v3.1.1
### New Features
- Added Bypass (Ethernet) mode support for WINC.
WINC1500 Socket Mode vs Ethernet Mode

The WINC modules are capable of operating in either Socket mode or Ethernet (A.K.A. bypass) mode.

•	Socket mode allows an application running on the host MCU to utilize the TCP/IP stack within the firmware of the WINC device, such that the host MCU does not need to run its own TCP/IP stack. This approach is memory efficient and faster to implement.
•	Ethernet (bypass) mode disables the TCP/IP stack within the firmware of the WINC device, instead the host MCU can only send and receive Ethernet frames. This mode allows the host MCU to operate its own TCP/IP stack treating the WINC device as a simple Ethernet controller. This approach provides flexibility.

### Bug fixes
- Fixed MHC menu for RTOS memory allocation size change (words to bytes).

### Known Issues
- Bypass mode currently works only with the reference example - wifi_winc_sta_bypass.
All other examples are configured to work with socket mode.

- MISRA false positives:
1. driver/winc/drv/driver/m2m_wifi.c(1140) 	644	9.1
Variable 'strNetworkId' (line 1131) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]
			
2. driver/winc/wdrv_winc_nvm.c(413) 	644	9.1
Variable 'strControl' (line 398) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]
			
3. app.c(136) 	530	9.1
Symbol 'args' (line 135) not initialized [MISRA 2012 Rule 9.1, mandatory]
			
4. driver/winc/drv/driver/nmspi.c(904) 	644	9.1
Variable 'tmpBuf' (line 881) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]

5. driver/winc/drv/driver/nmflash.c(690) 	530	9.1
Symbol 'write_init_params' (line 684) not initialized [MISRA 2012 Rule 9.1, mandatory]

6. wifi_provisioning_via_softap/example.c(269) 	644	9.1
Variable 'wep_parameters' (line 188) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]

### Development Tools
- Same as v3.0.0

## Wireless Release v3.1.0
### New Features
- Support for WINC3400 (v1.2.2) on SAMD21.
- Support for WINC3400 (v1.2.2) on SAME54 - Beta release.

### Bug fixes
- Fixed MHC Component Manager menu for Wireless.

### Known Issues
- Version interop issues for provisioning .apk with some Android versions.
- Default password for Bluetooth provisioning is '123456'.

### Development Tools
- Same as v3.0.0

## Wireless Release v3.0.0
### New Features

- **New part support** - This release introduces initial support for SAMD21 and SAME54 families of 32-bit microcontrollers.

- **Development kit and demo application support** - The following table provides number of peripheral library application available for different development kits

    | Development Kits                                                                                                                               | Number of applications |
    | ---                                                                                                                                            | --- |
    | [SAM D21 Xplained Pro Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATSAMD21-XPRO)                     | 7 |
    | [SAM E54 Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/ATSAME54-XPRO)                                 | 5 |


### Known Issues

The current known issues are as follows:

* In the Microchip Harmony Configurator (MHC), EIC module for Interrupts should be selected after selecting the WINC driver component.

* Beta release for SAME54- Only applications fully tested.

* MH3-24109 - non-RTOS WINC1500 project incorrectly includes configuration.h 

* Preliminary support added for WINC3400 using MPLAB X and XC32. This complete tooling support will be added in a future release.

* Please see the help documentation in the doc and docs folder for this Harmony 3 module. Documentation is available across CHM, PDF and HTML formats.

### Development Tools

* [MPLAB® X IDE v5.25](https://www.microchip.com/mplab/mplab-x-ide)
* [MPLAB® XC32 C/C++ Compiler v2.30](https://www.microchip.com/mplab/compilers)
* MPLAB® X IDE plug-ins:
* MPLAB® Harmony Configurator (MHC) v3.3.0.0 and above.
