/**
 *
 * \file
 *
 * \brief Legacy Adaption Layer.
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

#ifndef _WINC_LEGACY_H_
#define _WINC_LEGACY_H_

#define NMI_API

#define CONST const

#define uint8  uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define sint8  int8_t
#define sint16 int16_t
#define sint32 int32_t

#define m2m_strlen strlen
#define m2m_memcpy memcpy
#define m2m_memset memset
#define m2m_memcmp memcmp
#define m2m_strstr strstr
#define m2m_strncmp strncmp

#define m2m_wifi_yield()

#define m2m_wifi_set_sytem_time m2m_wifi_set_system_time
#define m2m_wifi_get_sytem_time m2m_wifi_get_system_time

#endif /* _WINC_LEGACY_H_ */