/**
 *
 * \file
 *
 * \brief Default WINC configuration.
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
