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

#if defined(WDRV_WINC_DEVICE_WINC1500)
#define WLAN_SSID           "WINC1500_PROVISION_AP"
#elif defined(WDRV_WINC_DEVICE_WINC3400)
#define WLAN_SSID           "WINC3400_PROVISION_AP"
#endif 
#define WLAN_CHANNEL        1
#define WLAN_AUTH_OPEN
#define WLAN_WEB_KEY        "1234567890"
#define WLAN_WEB_KEY_INDEX  1

#define TCP_LISTEN_PORT     80
#define TCP_BUFFER_SIZE     1460

#define WLAN_DHCP_SRV_ADDR      "192.168.1.1"
#define WLAN_DHCP_SRV_NETMASK   "255.255.255.0"

#endif /* _EXAMPLE_CONF_H */

/*******************************************************************************
 End of File
 */

