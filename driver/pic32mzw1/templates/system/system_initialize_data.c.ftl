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
<#if DRV_WIFI_PIC32MZW1_COEX_READ_BT_PRIO != "">
extern bool ${DRV_WIFI_PIC32MZW1_COEX_READ_BT_PRIO}(void);
</#if>
static CRYPT_RNG_CTX wdrvRngCtx;
static const WDRV_PIC32MZW_SYS_INIT wdrvPIC32MZW1InitData = {
    .pCryptRngCtx  = &wdrvRngCtx,
    .pRegDomName   = "${DRV_WIFI_PIC32MZW1_REG_DOMAIN}",
    .powerSaveMode = WDRV_PIC32MZW_POWERSAVE_RUN_MODE,
    .powerSavePICCorrelation = WDRV_PIC32MZW_POWERSAVE_PIC_ASYNC_MODE,
<#if DRV_WIFI_PIC32MZW1_COEX_ENABLE == true>
    .coexConfigFlags = WDRV_PIC32MZW_COEX_CONFIG_ENABLE
<#if DRV_WIFI_PIC32MZW1_COEX_IF_TYPE == '3-Wire'>
        | WDRV_PIC32MZW_COEX_CONFIG_IF_3WIRE
<#else>
        | WDRV_PIC32MZW_COEX_CONFIG_IF_2WIRE
</#if>
<#if DRV_WIFI_PIC32MZW1_COEX_PRIO_WLAN_TX_GT_BT_LP == true>
        | WDRV_PIC32MZW_COEX_CONFIG_PRIO_WLAN_TX_GT_BTLP
<#else>
        | WDRV_PIC32MZW_COEX_CONFIG_PRIO_WLAN_TX_LT_BTLP
</#if>
<#if DRV_WIFI_PIC32MZW1_COEX_PRIO_WLAN_RX_GT_BT_LP == true>
        | WDRV_PIC32MZW_COEX_CONFIG_PRIO_WLAN_RX_GT_BTLP
<#else>
        | WDRV_PIC32MZW_COEX_CONFIG_PRIO_WLAN_RX_LT_BTLP
</#if>
<#if DRV_WIFI_PIC32MZW1_COEX_SHARED_ANTENNA == true>
        | WDRV_PIC32MZW_COEX_CONFIG_SHARED_ANTENNA,
<#else>
        | WDRV_PIC32MZW_COEX_CONFIG_DEDICATED_ANTENNA,
</#if>
<#else>
    .coexConfigFlags = WDRV_PIC32MZW_COEX_CONFIG_DISABLE,
</#if>
<#if DRV_WIFI_PIC32MZW1_COEX_READ_BT_PRIO != "">
    .pBTPriorityReadFn = ${DRV_WIFI_PIC32MZW1_COEX_READ_BT_PRIO}
<#else>
    .pBTPriorityReadFn = NULL
</#if>
};
