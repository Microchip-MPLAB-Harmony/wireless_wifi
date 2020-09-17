/* WIFI System Service Configuration Options */
<#if SYS_WIFI_ENABLE == true>
<#if SYS_WIFI_MODE == "STA">
#define SYS_WIFI_DEVMODE        			SYS_WIFI_STA
<#elseif SYS_WIFI_MODE == "AP">
#define SYS_WIFI_DEVMODE        			SYS_WIFI_AP
</#if>


#define SYS_WIFI_MAX_CBS					${SYS_WIFI_MAX_CBS}
#define SYS_WIFI_COUNTRYCODE        	   "${SYS_WIFI_COUNTRYCODE}"
<#if SYS_WIFI_STA_ENABLE == true>
#define SYS_WIFI_STA_SSID        			"${SYS_WIFI_STA_SSID_NAME}"
#define SYS_WIFI_STA_PWD        			"${SYS_WIFI_STA_PWD_NAME}"
<#if SYS_WIFI_STA_AUTH == "OPEN">
#define SYS_WIFI_STA_AUTHTYPE				SYS_WIFI_OPEN
<#elseif SYS_WIFI_STA_AUTH == "WPA2">
#define SYS_WIFI_STA_AUTHTYPE				SYS_WIFI_WPAWPA2MIXED 
<#elseif SYS_WIFI_STA_AUTH == "WPA2WPA3">
#define SYS_WIFI_STA_AUTHTYPE				SYS_WIFI_WPA2WPA3MIXED
<#elseif SYS_WIFI_STA_AUTH == "WPA3">
#define SYS_WIFI_STA_AUTHTYPE				SYS_WIFI_WPA3
</#if>
<#if SYS_WIFI_STA_AUTOCONNECT == true>
#define SYS_WIFI_STA_AUTOCONNECT   			true
<#else>
#define SYS_WIFI_STA_AUTOCONNECT   			false
</#if>
</#if>


<#if SYS_WIFI_AP_ENABLE == true>
#define SYS_WIFI_AP_SSID					"${SYS_WIFI_AP_SSID_NAME}"
#define SYS_WIFI_AP_PWD        				"${SYS_WIFI_AP_PWD_NAME}"
<#if SYS_WIFI_AP_AUTH == "OPEN">
#define SYS_WIFI_AP_AUTHTYPE				SYS_WIFI_OPEN
<#elseif SYS_WIFI_AP_AUTH == "WPA2">
#define SYS_WIFI_AP_AUTHTYPE				SYS_WIFI_WPA2
<#elseif SYS_WIFI_AP_AUTH == "WPAWPA2(Mixed)">
#define SYS_WIFI_AP_AUTHTYPE				SYS_WIFI_WPAWPA2MIXED
<#elseif SYS_WIFI_AP_AUTH == "WPA2WPA3">
#define SYS_WIFI_AP_AUTHTYPE				SYS_WIFI_WPA2WPA3MIXED
<#elseif SYS_WIFI_AP_AUTH == "WPA3">
#define SYS_WIFI_AP_AUTHTYPE				SYS_WIFI_WPA3
</#if>
#define SYS_WIFI_AP_CHANNEL					${SYS_WIFI_AP_CHANNEL}
<#if SYS_WIFI_AP_SSIDVISIBILE == true>
#define SYS_WIFI_AP_SSIDVISIBILE   			true
<#else>
#define SYS_WIFI_AP_SSIDVISIBILE   			false
</#if>
</#if>

<#if SYS_WIFI_APPDEBUG_ENABLE == true>
#define SYS_WIFI_DEBUG_PRESTR						"${SYS_WIFI_APPDEBUG_PRESTR}"

<#if SYS_WIFI_APPDEBUG_ERR_LEVEL == true>
#define SYS_WIFI_APPDEBUG_ERR_LEVEL_ENABLE   			0xf
<#else>
#define SYS_WIFI_APPDEBUG_ERR_LEVEL_ENABLE   			0x0
</#if>

<#if SYS_WIFI_APPDEBUG_DBG_LEVEL == true>
#define SYS_WIFI_APPDEBUG_DBG_LEVEL_ENABLE   			0xf
<#else>
#define SYS_WIFI_APPDEBUG_DBG_LEVEL_ENABLE   			0x0
</#if>

<#if SYS_WIFI_APPDEBUG_INFO_LEVEL == true>
#define SYS_WIFI_APPDEBUG_INFO_LEVEL_ENABLE   			0xf
<#else>
#define SYS_WIFI_APPDEBUG_INFO_LEVEL_ENABLE   			0x0
</#if>

<#if SYS_WIFI_APPDEBUG_FUNC_LEVEL == true>
#define SYS_WIFI_APPDEBUG_FUNC_LEVEL_ENABLE   			0xf
<#else>
#define SYS_WIFI_APPDEBUG_FUNC_LEVEL_ENABLE   			0x0
</#if>

<#if SYS_WIFI_APPDEBUG_CFG_FLOW == true>
#define SYS_WIFI_APPDEBUG_CFG_FLOW						0xf
<#else>
#define SYS_WIFI_APPDEBUG_CFG_FLOW						0x0
</#if>
<#if SYS_WIFI_APPDEBUG_CONNECT_FLOW == true>
#define SYS_WIFI_APPDEBUG_CONNECT_FLOW					0xf
<#else>
#define SYS_WIFI_APPDEBUG_CONNECT_FLOW					0x0
</#if>

<#if SYS_WIFI_APPDEBUG_PROVISIONING_FLOW == true>
#define SYS_WIFI_APPDEBUG_PROVISIONING_FLOW				0xf
<#else>
#define SYS_WIFI_APPDEBUG_PROVISIONING_FLOW				0x0
</#if>

<#if SYS_WIFI_APPDEBUG_PROVISIONINGCMD_FLOW == true>
#define SYS_WIFI_APPDEBUG_PROVISIONINGCMD_FLOW			0xf
<#else>
#define SYS_WIFI_APPDEBUG_PROVISIONINGCMD_FLOW			0x0
</#if>

<#if SYS_WIFI_APPDEBUG_PROVISIONINGSOCK_FLOW == true>
#define SYS_WIFI_APPDEBUG_PROVISIONINGSOCK_FLOW			0xf
<#else>
#define SYS_WIFI_APPDEBUG_PROVISIONINGSOCK_FLOW			0x0
</#if>
</#if>




<#if HarmonyCore.SELECT_RTOS != "BareMetal">
    <#lt>/* SYS WIFI RTOS Configurations*/
    <#lt>#define SYS_WIFI_RTOS_SIZE           		${SYS_WIFI_RTOS_TASK_STACK_SIZE}
    <#lt>#define SYS_WIFI_RTOS_PRIORITY             ${SYS_WIFI_RTOS_TASK_PRIORITY}
</#if>
</#if>
<#--
/*******************************************************************************
 End of File
*/
-->
