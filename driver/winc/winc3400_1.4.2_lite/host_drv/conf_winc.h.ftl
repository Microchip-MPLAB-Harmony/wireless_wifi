/**
 *
 * \file
 *
 * \brief WINC3400 configuration.
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

#ifndef CONF_WINC_H_INCLUDED
#define CONF_WINC_H_INCLUDED

#include "configuration.h"
#include "definitions.h"

<#if DRV_WIFI_WINC_RESET_ASSERT_TIME gt 0>
#define CONF_WINC_RESET_ASSERT_TIME                 ${DRV_WIFI_WINC_RESET_ASSERT_TIME}

</#if>
#define CONF_WINC_HIF_CB_WIFI_HANDLER               m2m_wifi_cb
<#if DRV_WIFI_WINC_DRIVER_MODE = "Ethernet Mode">
//#define CONF_WINC_HIF_CB_IP_HANDLER                 m2m_ip_cb
//#define CONF_WINC_HIF_CB_HIF_HANDLER
//#define CONF_WINC_HIF_CB_OTA_HANDLER                m2m_ota_cb
//#define CONF_WINC_HIF_CB_SSL_HANDLER                m2m_ssl_cb
<#else>
#define CONF_WINC_HIF_CB_IP_HANDLER                 m2m_ip_cb
//#define CONF_WINC_HIF_CB_HIF_HANDLER
#define CONF_WINC_HIF_CB_OTA_HANDLER                m2m_ota_cb
#define CONF_WINC_HIF_CB_SSL_HANDLER                m2m_ssl_cb
</#if>
//#define CONF_WINC_HIF_CB_SIGMA_HANDLER

/* Debug Options */
<#if sys_debug?? && sys_debug.SYS_DEBUG_USE_CONSOLE?? && DRV_WIFI_WINC_USE_SYS_DEBUG && sys_debug.SYS_DEBUG_USE_CONSOLE>
#define CONF_WINC_LOG_ERROR(...)    do { _SYS_DEBUG_PRINT(SYS_ERROR_ERROR, __VA_ARGS__); } while (0)
#define CONF_WINC_LOG_INFO(...)     do { _SYS_DEBUG_PRINT(SYS_ERROR_INFO, __VA_ARGS__); } while (0)
#define CONF_WINC_LOG_DEBUG(...)    do { _SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, __VA_ARGS__); } while (0)
<#elseif DRV_WIFI_WINC_LOG_LEVEL == "None">
<#else>
  <#if DRV_WIFI_WINC_LOG_LEVEL == "Error">
#define CONF_WINC_DEBUG_LEVEL                       1
  <#elseif DRV_WIFI_WINC_LOG_LEVEL == "Inform">
#define CONF_WINC_DEBUG_LEVEL                       2
  <#elseif DRV_WIFI_WINC_LOG_LEVEL == "Trace">
#define CONF_WINC_DEBUG_LEVEL                       2
  <#elseif DRV_WIFI_WINC_LOG_LEVEL == "Verbose">
#define CONF_WINC_DEBUG_LEVEL                       3
  </#if>
#define CONF_WINC_PRINTF                            if (NULL == pfWINCDebugPrintCb) {break;} pfWINCDebugPrintCb
</#if><#-- DRV_WIFI_WINC_LOG_LEVEL -->
//#define CONF_WINC_ASSERT

/* Define only on MCU big endian architectures */
//#define CONF_WINC_MCU_ARCH_BIG_ENDIAN

/* Define only on MCU little endian architectures */
#define CONF_WINC_MCU_ARCH_LITTLE_ENDIAN

/* Define macro to swap endianness of 32-bit unsigned integer */
#define CONF_WINC_UINT32_SWAP(U32)                  ((((U32) & 0x000000FF) << 24) | (((U32) & 0x0000FF00) << 8) | (((U32) & 0x00FF0000) >> 8) | (((U32) & 0xFF000000) >> 24))

/* Define macro to swap endianness of 16-bit unsigned integer */
#define CONF_WINC_UINT16_SWAP(U16)                  ((((U16) & 0x00FF) << 8) | (((U16)  & 0xFF00) >> 8))

/* Define if _Static_assert/static_assert is not supported on this tool chain */
//#define CONF_WINC_HIF_STRUCT_SIZE_CHECK(STRUCTNAME)

<#if DRV_WIFI_WINC_USE_BLUETOOTH_WINC3400>
/* Define to include support for BLE */
#define CONF_WINC_BLE_INCLUDE

</#if>
<#if DRV_WIFI_WINC_ENABLE_SB>
/* Define to include support for serial bridge */
#define CONF_WINC_SERIAL_BRIDGE_INCLUDE

<#if DRV_WIFI_WINC_ENABLE_SB_UNSOL_SYNC_ID>
/* Define if the serial bridge sends a sync ID before being requested */
#define CONF_WINC_SB_SEND_UNSOL_SYNC_ID

</#if><#-- DRV_WIFI_WINC_ENABLE_SB_UNSOL_SYNC_ID -->
<#if DRV_WIFI_WINC_ENABLE_SB_RATE_CHANGE>
/* Define if serial bridge will support variable baud rates */
#define CONF_WINC_SERIAL_BRIDGE_VARIABLE_BAUD_RATE

</#if><#-- DRV_WIFI_WINC_ENABLE_SB_RATE_CHANGE -->
</#if><#-- DRV_WIFI_WINC_ENABLE_SB -->
<#if DRV_WIFI_WINC_DRIVER_MODE = "Ethernet Mode">
/* Define if the socket API is exposed or not to the application */
#define CONF_WINC_DISABLE_SOCKET_API

</#if>
bool winc_crit_sec_hif_init(void);
void winc_crit_sec_hif_deinit(void);
void winc_crit_sec_hif_enter(void);
void winc_crit_sec_hif_leave(void);

#define WINC_CRIT_SEC_HIF
#define WINC_CRIT_SEC_HIF_INIT      if (false == winc_crit_sec_hif_init()) return M2M_ERR_INIT;
#define WINC_CRIT_SEC_HIF_DEINIT    winc_crit_sec_hif_deinit();
#define WINC_CRIT_SEC_HIF_ENTER     winc_crit_sec_hif_enter();
#define WINC_CRIT_SEC_HIF_LEAVE     winc_crit_sec_hif_leave();

void winc_crit_sec_bus_init(void);
void winc_crit_sec_bus_deinit(void);
bool winc_crit_sec_bus_enter(void);
void winc_crit_sec_bus_leave(void);

#define WINC_CRIT_SEC_BUS
#define WINC_CRIT_SEC_BUS_INIT      winc_crit_sec_bus_init();
#define WINC_CRIT_SEC_BUS_DEINIT    winc_crit_sec_bus_deinit();
#define WINC_CRIT_SEC_BUS_ENTER     if (false == winc_crit_sec_bus_enter()) return WINC_BUS_FAIL;
#define WINC_CRIT_SEC_BUS_LEAVE     winc_crit_sec_bus_leave();

void winc_crit_sec_ble_init(void);
void winc_crit_sec_ble_deinit(void);
bool winc_crit_sec_ble_enter(void);
void winc_crit_sec_ble_leave(void);

#define WINC_CRIT_SEC_BLE
#define WINC_CRIT_SEC_BLE_INIT      winc_crit_sec_ble_init();
#define WINC_CRIT_SEC_BLE_DEINIT    winc_crit_sec_ble_deinit();
#define WINC_CRIT_SEC_BLE_ENTER     if (false == winc_crit_sec_ble_enter()) return;
#define WINC_CRIT_SEC_BLE_LEAVE     winc_crit_sec_ble_leave();

#define ETH_MODE

#define WINC_SOCKET_CLOSE           shutdown

<#if DRV_WIFI_WINC_LOG_LEVEL != "None">
#include "wdrv_winc_debug.h"

</#if>
#include "include/conf_winc_defaults.h"

#endif /* CONF_WINC_H_INCLUDED */
