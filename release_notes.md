![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip MPLAB® Harmony 3 Release Notes

## Wireless Release v3.2.0
### New Features
- WINC: Adds support/Callback for ECC callback.
- WINC: Adds Support for M0/M4/M7 Hosts through GPIO & PIO interrupts. Earlier only EIC Interrupts were allowed.
- WINC: Updated cache maintenance option for PIC32MZ.
- PIC32MZW1: Add WLAN driver support and application examples for PIC32MZW1 device

### Bug fixes
- WINC: Fixed MHC Component Manager menu for Wireless Bypass mode (Supported only for WINC1500 in this release).
- WINC: Improved documentation for WINC driver and application examples.
- WINC: SYS_Console related changes to reflect updates in core/csp/bsp v3.7.0.
- PIC32MZW1: IOP failures (all security modes) due to short preamble
- PIC32MZW1: Fix failures in Management Frame Protection.
- PIC32MZW1: Link Control: Firmware Overwrites MAC address in broadcast packets
- PIC32MZW1: Fix in AP mode to support 8 STA connections.
- PIC32MZW1: [Coverity] Buffer not null terminated in wdrv_pic32mzw_regdomain.c
- PIC32MZW1: Fix issue with triggerring dependencies when the PIC32MZW1 wireless driver in included.

### Known Issues
- WINC: Same as v3.1.1
- PIC32MZW1: TCP Tx occasionally stalls when 11n is enabled.
- PIC32MZW1: STA mode connection problems when multiple APs are present with the same SSID but different security settings.
- PIC32MZW1: Scanning is not supported in AP mode.
- PIC32MZW1: TCP Tx occasionally stalls when 11n is enabled.
- PIC32MZW1: QoS Data unsupported in transmit path.

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