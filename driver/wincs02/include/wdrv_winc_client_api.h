/*******************************************************************************
  WINC Driver Client API Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_client_api.h

  Summary:
    WINC wireless driver client API header file.

  Description:
    This file pulls together the elements which make up the client API
      assoc       - Current association.
      bssfind     - BSS scan functionality.
      softap      - Soft-AP mode.
      sta         - Infrastructure stations mode.

 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2024 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

#ifndef WDRV_WINC_CLIENT_API_H
#define WDRV_WINC_CLIENT_API_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"

#include "wdrv_winc.h"
#include "wdrv_winc_bssfind.h"
#include "wdrv_winc_assoc.h"
#include "wdrv_winc_dhcps.h"
#include "wdrv_winc_file.h"
#include "wdrv_winc_softap.h"
#include "wdrv_winc_sta.h"
#include "wdrv_winc_netif.h"
#include "wdrv_winc_tls.h"
#include "wdrv_winc_mqtt.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_sntp.h"
#include "wdrv_winc_wifi.h"
#include "wdrv_winc_dns.h"

#endif /* WDRV_WINC_CLIENT_API_H */
