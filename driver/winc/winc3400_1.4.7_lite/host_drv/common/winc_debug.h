/**
 *
 * \file
 *
 * \brief This module contains debug APIs declarations.
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
