/**
 *
 * \file
 *
 * \brief Primary include file for applications.
 */
/*
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _WINC_H_
#define _WINC_H_

#include <stdint.h>

#include "bsp/winc_bsp.h"

#ifdef CONF_WINC_LEGACY_DEFINITIONS
#include "./winc_legacy.h"
#endif

#include "../common/winc_defines.h"
#include "../common/winc_debug.h"
#include "../m2m/winc_m2m_types.h"
#include "../m2m/winc_m2m_wifi.h"
#include "../m2m/winc_m2m_periph.h"
#ifndef CONF_WINC_DISABLE_SOCKET_API
#include "../m2m/winc_m2m_ota.h"
#include "../common/winc_ecc_types.h"
#include "../m2m/winc_m2m_ssl.h"
#include "../socket/winc_socket.h"
#endif
#include "../spi_flash/winc_spi_flash_map.h"
#include "../spi_flash/winc_spi_flash.h"
#ifdef CONF_WINC_BLE_INCLUDE
#include "winc_ble_api.h"
#endif

#ifdef CONF_WINC_SERIAL_BRIDGE_INCLUDE
#include "../serial_bridge/winc_serial_bridge.h"
#endif

#endif
