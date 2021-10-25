/**
 *
 * \file
 *
 * \brief Primary include file for applications.
 *
 * Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
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
#include "../spi_flash/winc_flexible_flash.h"
#include "../spi_flash/winc_spi_flash_map.h"
#include "../spi_flash/winc_spi_flash.h"

#ifdef CONF_WINC_SERIAL_BRIDGE_INCLUDE
#include "../serial_bridge/winc_serial_bridge.h"
#endif

#endif
