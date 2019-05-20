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
      custie      - Custom IE management for Soft-AP.
      nvm         - Off line access the WINC SPI flash.
      powersave   - Power save control.
      socket      - TCP/IP sockets.
      softap      - Soft-AP mode.
      sta         - Infrastructure stations mode.
      systtime    - System time.
      ssl         - SSL configuration and management.
      ble         - BLE control (WINC3400)

 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

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

#ifndef _WDRV_WINC_CLIENT_API_H
#define _WDRV_WINC_CLIENT_API_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"

#include "wdrv_winc.h"
#include "wdrv_winc_bssfind.h"
#include "wdrv_winc_powersave.h"
#include "wdrv_winc_assoc.h"
#include "wdrv_winc_systime.h"
#include "wdrv_winc_softap.h"
#include "wdrv_winc_sta.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_custie.h"
#ifdef WDRV_WINC_NETWORK_MODE_SOCKET
#include "wdrv_winc_socket.h"
#include "wdrv_winc_ssl.h"
#endif
#ifdef WDRV_WINC_DEVICE_HOST_FILE_DOWNLOAD
#include "wdrv_winc_host_file.h"
#endif
#ifdef WDRV_WINC_ENABLE_BLE
#include "wdrv_winc_ble.h"
#endif
#include "wdrv_winc_nvm.h"

#endif /* _WDRV_WINC_CLIENT_API_H */
