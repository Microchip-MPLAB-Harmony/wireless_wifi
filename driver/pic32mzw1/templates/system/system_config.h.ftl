<#--
Copyright (C) 2020-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
-->

/*** WiFi PIC32MZW1 Driver Configuration ***/
<#if sys_debug?? && sys_debug.SYS_DEBUG_USE_CONSOLE?? && DRV_WIFI_PIC32MZW1_USE_SYS_DEBUG && sys_debug.SYS_DEBUG_USE_CONSOLE>
    <#lt>#define WDRV_PIC32MZW1_DEVICE_USE_SYS_DEBUG
<#elseif DRV_WIFI_PIC32MZW1_LOG_LEVEL == "None">
    <#lt>#define WDRV_PIC32MZW_DEBUG_LEVEL               WDRV_PIC32MZW_DEBUG_TYPE_NONE
<#elseif DRV_WIFI_PIC32MZW1_LOG_LEVEL == "Error">
    <#lt>#define WDRV_PIC32MZW_DEBUG_LEVEL               WDRV_PIC32MZW_DEBUG_TYPE_ERROR
<#elseif DRV_WIFI_PIC32MZW1_LOG_LEVEL == "Inform">
    <#lt>#define WDRV_PIC32MZW_DEBUG_LEVEL               WDRV_PIC32MZW_DEBUG_TYPE_INFORM
<#elseif DRV_WIFI_PIC32MZW1_LOG_LEVEL == "Trace">
    <#lt>#define WDRV_PIC32MZW_DEBUG_LEVEL               WDRV_PIC32MZW_DEBUG_TYPE_TRACE
<#elseif DRV_WIFI_PIC32MZW1_LOG_LEVEL == "Verbose">
    <#lt>#define WDRV_PIC32MZW_DEBUG_LEVEL               WDRV_PIC32MZW_DEBUG_TYPE_VERBOSE
</#if>
<#if DRV_WIFI_PIC32MZW1_SUPPORT_SAE == true>
#define WDRV_PIC32MZW_WPA3_PERSONAL_SUPPORT
</#if>
<#if DRV_WIFI_PIC32MZW1_REQUIRE_BA414E == true>
#define WDRV_PIC32MZW_BA414E_SUPPORT
</#if>
<#if DRV_WIFI_PIC32MZW1_REQUIRE_BIGINTSW == true>
#define WDRV_PIC32MZW_BIGINTSW_SUPPORT
</#if>
<#if DRV_WIFI_PIC32MZW1_SUPPORT_ENTERPRISE == true>
#define WDRV_PIC32MZW_ENTERPRISE_SUPPORT
/* Wolfssl : Support openssl based server certs having 'basic constraint' extension value as non-boolean. */
#define WOLFSSL_X509_BASICCONS_INT
/* Wolfssl : Don't validate key-usage extension of the certificate */
#define IGNORE_KEY_EXTENSIONS
</#if>
<#if DRV_WIFI_PIC32MZW1_REQUIRE_WOLFSSL == true>
#define WDRV_PIC32MZW_WOLFSSL_SUPPORT
</#if>
#define WDRV_PIC32MZW_ALARM_PERIOD_1MS          ${DRV_WIFI_PIC32MZW1_ALARM_PERIOD_1MS}
#define WDRV_PIC32MZW_ALARM_PERIOD_MAX          ${DRV_WIFI_PIC32MZW1_ALARM_PERIOD_MAX}
