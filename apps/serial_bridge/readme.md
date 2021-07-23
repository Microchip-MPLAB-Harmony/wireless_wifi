---
parent: Harmony 3 Wireless_wifi Package
title: Serial Bridge
has_toc: true
has_children: false
has_toc: false
nav_order: 1

family: SAMD21
family: SAME54
function: Serial Bridge
---

# Serial Bridge

This example compiles the serial bridge application which is used to communicate to WINC devices from PC.

## Description

Serial bridge application acts as a bridge between PC and WINC devices. For example firmware update project use serial bridge application to load the firmware to WINC device via serial bridge application located in host MCU.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_wifi)


Path of the application within the repository is **apps/serial_bridge/firmware** .

To build the application, refer to the following table and open the project using its IDE.

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| sam_d21_xpro_winc1500.X | MPLABX project for SAMD21 and WINC3400 Xplained pro |
| sam_d21_xpro_winc3400.X | MPLABX project for SAMD21 and WINC3400 Xplained pro |
| sam_e54_xpro_winc1500.X | MPLABX project for SAME54 and WINC3400 Xplained pro |
| sam_e54_xpro_winc3400.X | MPLABX project for SAME54 and WINC3400 Xplained pro |
|||

## Setting up SAMXXX Xplained Pro board

- Connect the Debug USB port on the SAMXXX Xplained Pro board to the computer using a micro USB cable

## Running the Application

1. Open the project.
2. Build and program the generated code into the hardware using its IDE.
3. Please refer to "WINC Firmware Update Guide" for more information.
4. Please note this example will not output any console messages.
