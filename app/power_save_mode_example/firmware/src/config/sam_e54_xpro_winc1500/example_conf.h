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
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef _EXAMPLE_CONF_H
#define _EXAMPLE_CONF_H


/** Wi-Fi Settings */
#define MAIN_WLAN_SSID              "MicrochipDemoAp" /* < Destination SSID */
#define WLAN_AUTH_WPA
#define MAIN_WLAN_PSK               "12345678" /* < Password for Destination SSID */

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

