---
grand_parent: WLAN API Guide
parent: WINC WLAN API Guide
title: WINC 1500/3400 Application examples - Getting Started
has_toc: false
nav_order: 2
---

# WINC Application examples - Getting Started
## Introduction

This note describes the steps involved to create Application examples for WINC1500/3400 on H3.

## 1 Common Configuration

Microchip-Direct references/order codes:
• [SAMD21: ATSAMD21-XPRO](https://www.microchip.com/Developmenttools/ProductDetails/ATSAMe54-XPRO)
• [SAME54: ATSAME54-XPRO](https://www.microchip.com/Developmenttools/ProductDetails/ATSAMD21-XPRO)
• [WINC1500: ATWINC1500-XPRO](https://www.microchip.com/Developmenttools/ProductDetails/ATWINC1500-XPRO)
• [WINC3400: ATWINC3400-XPRO](https://www.microchip.com/Developmenttools/ProductDetails/ATWINC3400-XPRO)

### 1.1 IDE Installation

[Download and install MPLAB IDE](https://www.microchip.com/mplab/mplab-x-ide)
[Download and install XC32 compilers](https://www.microchip.com/mplab/compilers)

### 1.2 Configuring Content Manager

Download H3 content manager and configurator Plugins for the IDE

![Microchip-Technology](images/GettingStarted_pic1.png)

• Repo URL: https://github.com/Microchip-MPLAB-Harmony

![Microchip-Technology](images/GettingStarted_pic2.png)

Select and download all required content into your local working folder.

![Microchip-Technology](images/GettingStarted_pic3.png)

## 2 Adding an Application Example

### 2.1 Create a new project

![Microchip-Technology](images/GettingStarted_pic4.png)

### 2.2 Configuring the Framework path

![Microchip-Technology](images/GettingStarted_pic5.png)

### 2.3 Project location

![Microchip-Technology](images/GettingStarted_pic6.png)

### 2.4 Configuration settings

![Microchip-Technology](images/GettingStarted_pic7.png)

### 2.5 MHC Configurator

![Microchip-Technology](images/GettingStarted_pic8.png)

### 2.6 MH3 Launch

![Microchip-Technology](images/GettingStarted_pic9.png)

![Microchip-Technology](images/GettingStarted_pic10.png)

### 2.7 MH3 Configurator UI

Below Components are present by default:

![Microchip-Technology](images/GettingStarted_pic11.png)

![Microchip-Technology](images/GettingStarted_pic12.png)

Required components can be added to the project graph. For example, in case of SAME54 below components can be added to the GUI as shown in the following picture: SAM E54 Xplained Pro BSP, WINC, SPI, SERCOM4, CONSOLE, SERCOM2, DEBUG, TIME, TC3 and EIC

![Microchip-Technology](images/GettingStarted_pic13.png)

### 2.8 Core Component Configuration

Each component can be configured based on user needs.

![Microchip-Technology](images/GettingStarted_pic14.png)

### 2.9 EIC Component Configuration

![Microchip-Technology](images/GettingStarted_pic15.png)

### 2.10 WINC Component Configuration

WINC component configuration will look similar to the below pictures based upon your custom settings.

![Microchip-Technology](images/GettingStarted_pic16.png)

![Microchip-Technology](images/GettingStarted_pic17.png)

### 2.11 SPI Component Configuration

Peripheral components such as the SPI which are added can also be configured as per user needs.

![Microchip-Technology](images/GettingStarted_pic18.png)

![Microchip-Technology](images/GettingStarted_pic19.png)

### 2.12 SERCOM4 Component Configuration

![Microchip-Technology](images/GettingStarted_pic20.png)

### 2.13 SERCOM2 Component Configuration

![Microchip-Technology](images/GettingStarted_pic21.png)

### 2.14 Pin Configuration

Once component configuration is complete, the PIN Configuration has to be completed before generating the application source code. Below examples shows the PIN configuration added for a SAME54 example application.

![Microchip-Technology](images/GettingStarted_pic22.png)

Open the Pin Settings page, configure the PA27, PA06/ PB05, PB07, PB24, PB25, PB26, PB27, PB28, PB29 like below. Notice the custom name of PA27 must be “WDRV_WINC_CHIP_EN” and PA06/ PB05 must be “WDRV_WINC_RESETN”

For WINC1500, PA06 is configured to RESETN pin

![Microchip-Technology](images/GettingStarted_pic23.png)

For WINC3400, PB05 is configured to RESETN pin

![Microchip-Technology](images/GettingStarted_pic24.png)

![Microchip-Technology](images/GettingStarted_pic25.png)

![Microchip-Technology](images/GettingStarted_pic26.png)

You can also refer to the default application examples for a live example of PIN configuration in SAME54 and SAMD21.

### 2.15 Code Generation

![Microchip-Technology](images/GettingStarted_pic27.png)

### 2.16 Code Modification

After the code is generated, app.c and app.h need to be modified for the initialize WINC driver and UART Debug Console.

![Microchip-Technology](images/GettingStarted_pic28.png)

![Microchip-Technology](images/GettingStarted_pic29.png)

![Microchip-Technology](images/GettingStarted_pic30.png)

### 2.17 Build & Test

Use the build and test options to run the example applications on the target device.
