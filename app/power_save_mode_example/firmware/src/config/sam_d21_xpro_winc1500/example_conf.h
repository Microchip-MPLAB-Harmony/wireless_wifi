/*******************************************************************************
  MPLAB Harmony Example Configuration File

  Company:
    Microchip Technology Inc.

  File Name:
    example_conf.h

  Summary:
    This header file provides configuration for the example.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

//DOM-IGNORE-END

#ifndef _EXAMPLE_CONF_H
#define _EXAMPLE_CONF_H


/** Wi-Fi Settings */
#define MAIN_WLAN_SSID              "mchp_demo" /* < Destination SSID */
#define WLAN_AUTH_WPA
#define MAIN_WLAN_PSK               "mchp5678" /* < Password for Destination SSID */

/** PowerSave status */
#define MAIN_PS_WAKE                0
#define MAIN_PS_SLEEP               1
#define MAIN_PS_REQ_SLEEP           3

/** PowerSave mode Settings */
#define MAIN_PS_SLEEP_MODE          WDRV_WINC_PS_MODE_MANUAL /* WDRV_WINC_PS_MODE_MANUAL / WDRV_WINC_PS_MODE_AUTO_LOW_POWER / WDRV_WINC_PS_MODE_OFF */

/** Request sleep time for PowerSave manual mode */
#define MAIN_REQUEST_SLEEP_TIME     1000

#endif /* _EXAMPLE_CONF_H */

/*******************************************************************************
 End of File
 */

