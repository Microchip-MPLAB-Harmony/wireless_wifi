/*******************************************************************************
  WINC Wireless Driver Client API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_client_api.h

  Summary:
    WINC wireless driver client API interface.

  Description:
    This file pulls together the elements which make up the client API:
      assoc       - Current association.
      bssfind     - BSS scan functionality.
      bssfind     - BSS scan functionality.
      dhcps       - DHCP server functionality.
      dns         - DNS client functionality.
      file        - File operation functionality.
      mqtt        - MQTT client functionality.
      netif       - Network interface functionality.
      softap      - Soft-AP mode.
      sta         - Infrastructure stations mode.
      tls         - TLS functionality.
      wifi        - WiFi configuration functionality.
 *******************************************************************************/

/*
Copyright (C) 2024-25 Microchip Technology Inc. and its subsidiaries. All rights reserved.

Subject to your compliance with these terms, you may use this Microchip software and any derivatives
exclusively with Microchip products. You are responsible for complying with third party license terms
applicable to your use of third party software (including open source software) that may accompany this
Microchip software. SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR
STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-
INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL
MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS,
DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER
CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY
TO MICROCHIP FOR THIS SOFTWARE.
*/

#ifndef WDRV_WINC_CLIENT_API_H
#define WDRV_WINC_CLIENT_API_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc.h"
#include "wdrv_winc_bssfind.h"
#include "wdrv_winc_assoc.h"
#ifndef WDRV_WINC_MOD_DISABLE_DHCPS
#include "wdrv_winc_dhcps.h"
#endif
#include "wdrv_winc_file.h"
#include "wdrv_winc_softap.h"
#include "wdrv_winc_sta.h"
#include "wdrv_winc_netif.h"
#ifndef WDRV_WINC_MOD_DISABLE_TLS
#include "wdrv_winc_tls.h"
#endif
#ifndef WDRV_WINC_MOD_DISABLE_MQTT
#include "wdrv_winc_mqtt.h"
#endif
#include "wdrv_winc_common.h"
#ifndef WDRV_WINC_MOD_DISABLE_SNTP
#include "wdrv_winc_sntp.h"
#endif
#include "wdrv_winc_wifi.h"
#ifndef WDRV_WINC_MOD_DISABLE_DNS
#include "wdrv_winc_dns.h"
#endif

#endif /* WDRV_WINC_CLIENT_API_H */
