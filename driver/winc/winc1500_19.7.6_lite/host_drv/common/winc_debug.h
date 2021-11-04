/**
 *
 * \file
 *
 * \brief This module contains debug APIs declarations.
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

#ifndef _WINC_DEBUG_H_
#define _WINC_DEBUG_H_

#define WINC_LOG_LEVEL_NONE                         0
#define WINC_LOG_LEVEL_ERROR                        1
#define WINC_LOG_LEVEL_INFO                         2
#define WINC_LOG_LEVEL_DEBUG                        3

#define WINC_LOG_ERROR(...)
#define WINC_LOG_INFO(...)
#define WINC_LOG_DEBUG(...)

#if defined(CONF_WINC_PRINTF) && defined(CONF_WINC_DEBUG_LEVEL) && (CONF_WINC_DEBUG_LEVEL > 0)

#if (CONF_WINC_DEBUG_LEVEL >= WINC_LOG_LEVEL_ERROR)
#undef WINC_LOG_ERROR
#define WINC_LOG_ERROR(...)                         do { CONF_WINC_PRINTF("[error][%s][%u]", __FUNCTION__, (uint16_t)__LINE__); CONF_WINC_PRINTF(__VA_ARGS__); CONF_WINC_PRINTF("\r\n"); } while(0)
#if (CONF_WINC_DEBUG_LEVEL >= WINC_LOG_LEVEL_INFO)
#undef WINC_LOG_INFO
#define WINC_LOG_INFO(...)                          do { CONF_WINC_PRINTF("[info]" __VA_ARGS__); CONF_WINC_PRINTF("\r\n"); } while(0)
#if (CONF_WINC_DEBUG_LEVEL >= WINC_LOG_LEVEL_DEBUG)
#undef WINC_LOG_DEBUG
#define WINC_LOG_DEBUG(...)                         do { CONF_WINC_PRINTF("[debug]" __VA_ARGS__); CONF_WINC_PRINTF("\r\n"); } while(0)
#endif /* WINC_LOG_LEVEL_DEBUG */
#endif /* WINC_LOG_LEVEL_INFO */
#endif /* WINC_LOG_LEVEL_ERROR */

#else

#ifdef CONF_WINC_LOG_ERROR
#undef WINC_LOG_ERROR
#define WINC_LOG_ERROR(format, ...)              CONF_WINC_LOG_ERROR(format "\r\n", ##__VA_ARGS__)
#endif
#ifdef CONF_WINC_LOG_INFO
#undef WINC_LOG_INFO
#define WINC_LOG_INFO(format, ...)               CONF_WINC_LOG_INFO(format "\r\n" , ##__VA_ARGS__)
#endif
#ifdef CONF_WINC_LOG_DEBUG
#undef WINC_LOG_DEBUG
#define WINC_LOG_DEBUG(format, ...)              CONF_WINC_LOG_DEBUG(format "\r\n", ##__VA_ARGS__)
#endif

#endif

#ifdef CONF_WINC_ASSERT
#define WINC_ASSERT(condition)      CONF_WINC_ASSERT(condition)
#else
#define WINC_ASSERT(condition)
#endif

#endif /* _WINC_DEBUG_H_ */
