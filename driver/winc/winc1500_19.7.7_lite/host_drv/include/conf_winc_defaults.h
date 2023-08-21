/**
 *
 * \file
 *
 * \brief Default WINC configuration.
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

#ifndef CONF_WINC_DEFAULTS_H_INCLUDED
#define CONF_WINC_DEFAULTS_H_INCLUDED

#ifndef CONF_WINC_PRINTF
#define CONF_WINC_PRINTF printf
#endif

#ifndef CONF_WINC_RESET_ASSERT_TIME
#define CONF_WINC_RESET_ASSERT_TIME     50
#endif

#ifdef CONF_WINC_HIF_CB_WIFI_HANDLER
void CONF_WINC_HIF_CB_WIFI_HANDLER(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
#else
#define CONF_WINC_HIF_CB_WIFI_HANDLER hif_null_cb
#endif

#ifdef CONF_WINC_HIF_CB_IP_HANDLER
void CONF_WINC_HIF_CB_IP_HANDLER(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
#else
#define CONF_WINC_HIF_CB_IP_HANDLER hif_null_cb
#endif

#ifdef CONF_WINC_HIF_CB_HIF_HANDLER
void CONF_WINC_HIF_CB_HIF_HANDLER(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
#else
#define CONF_WINC_HIF_CB_HIF_HANDLER hif_null_cb
#endif

#ifdef CONF_WINC_HIF_CB_OTA_HANDLER
void CONF_WINC_HIF_CB_OTA_HANDLER(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
#else
#define CONF_WINC_HIF_CB_OTA_HANDLER hif_null_cb
#endif

#ifdef CONF_WINC_HIF_CB_SSL_HANDLER
void CONF_WINC_HIF_CB_SSL_HANDLER(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
#else
#define CONF_WINC_HIF_CB_SSL_HANDLER hif_null_cb
#endif

#ifdef CONF_WINC_HIF_CB_CRYPTO_HANDLER
void CONF_WINC_HIF_CB_CRYPTO_HANDLER(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
#else
#define CONF_WINC_HIF_CB_CRYPTO_HANDLER hif_null_cb
#endif

#ifdef CONF_WINC_HIF_CB_SIGMA_HANDLER
void CONF_WINC_HIF_CB_SIGMA_HANDLER(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
#else
#define CONF_WINC_HIF_CB_SIGMA_HANDLER hif_null_cb
#endif

#if defined(CONF_WINC_MCU_ARCH_LITTLE_ENDIAN)
#elif defined(CONF_WINC_MCU_ARCH_BIG_ENDIAN)
#else
#error Please define MCU endianness
#endif

#if defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_STACK) || defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_STATIC)
#if defined(CONF_WINC_LARGE_MEMORY_ALLOC_FUNC) || defined(CONF_WINC_LARGE_MEMORY_FREE_FUNC)
#error Do not define large memory alloc/free functions for stack/static allocation style
#endif
#elif defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_DYNAMIC)
#ifndef CONF_WINC_LARGE_MEMORY_ALLOC_FUNC
#define CONF_WINC_LARGE_MEMORY_ALLOC_FUNC   malloc
#endif
#ifndef CONF_WINC_LARGE_MEMORY_FREE_FUNC
#define CONF_WINC_LARGE_MEMORY_FREE_FUNC    free
#endif
#else
#define CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_STACK
#endif

#ifndef CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_DYNAMIC
#ifndef CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE
#define CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE    1500
#endif
#else
#ifdef CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE
#error Enterprise connect memory reservation size not required for dynamic memory style
#endif
#endif

#ifndef CONF_WINC_HIF_STRUCT_SIZE_CHECK
#ifdef __GNUC__
#define CONF_WINC_HIF_STRUCT_SIZE_CHECK(STRUCTNAME) _Static_assert((sizeof(STRUCTNAME)%4)==0, "Structure alignment error");
#elif !defined(_WIN32) && !defined(_STDC_)
#define CONF_WINC_HIF_STRUCT_SIZE_CHECK(STRUCTNAME) static_assert((sizeof(STRUCTNAME)%4)==0, "Structure alignment error");
#else
#define CONF_WINC_HIF_STRUCT_SIZE_CHECK(STRUCTNAME)
#endif
#endif

#ifndef WINC_CRIT_SEC_HIF
#define WINC_CRIT_SEC_HIF_INIT
#define WINC_CRIT_SEC_HIF_DEINIT
#define WINC_CRIT_SEC_HIF_ENTER
#define WINC_CRIT_SEC_HIF_LEAVE
#endif

#ifndef WINC_CRIT_SEC_BUS
#define WINC_CRIT_SEC_BUS_INIT
#define WINC_CRIT_SEC_BUS_DEINIT
#define WINC_CRIT_SEC_BUS_ENTER
#define WINC_CRIT_SEC_BUS_LEAVE
#endif

#endif /* CONF_WINC_DEFAULTS_H_INCLUDED */
