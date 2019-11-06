![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip MPLAB® Harmony 3 Release Notes

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