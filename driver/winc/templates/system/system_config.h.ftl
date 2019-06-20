/*** WiFi WINC Driver Configuration ***/
<#if DRV_WIFI_WINC_INT_SRC == "EIC">
    <#lt>#define WDRV_WINC_EIC_SOURCE                EIC_PIN_${DRV_WIFI_WINC_EIC_SRC_SELECT}
<#elseif DRV_WIFI_WINC_INT_SRC == "External_Interrupt">
<#elseif DRV_WIFI_WINC_INT_SRC == "Change_Notification">
    <#lt>#define WDRV_WINC_INT_SOURCE INT_SOURCE_CHANGE_NOTICE
    <#lt>#define WDRV_WINC_INT_SOURCE_CN_PORT WDRV_WINC_INT_PORT
</#if>
<#if -1 < DRV_WIFI_WINC_SPI_INST_IDX>
    <#lt>#define WDRV_WINC_SPI_INDEX                 DRV_SPI_INDEX_${DRV_WIFI_WINC_SPI_INST_IDX}
</#if>
<#if DRV_WIFI_WINC_DRIVER_MODE == "Ethernet Mode">
    <#lt>#define WDRV_WINC_NETWORK_MODE_ETHERNET
<#else>
    <#lt>#define WDRV_WINC_NETWORK_MODE_SOCKET
</#if>
<#if DRV_WIFI_WINC_DEVICE == "WINC1500">
    <#lt>#define WDRV_WINC_DEVICE_WINC1500
    <#if DRV_WIFI_WINC1500_VERSION == "19.5.4">
        <#lt>#define WDRV_WINC_DEVICE_DYNAMIC_BYPASS_MODE
    <#elseif DRV_WIFI_WINC1500_VERSION == "19.6.1">
        <#lt>#define WDRV_WINC_DEVICE_SPLIT_INIT
        <#lt>#define WDRV_WINC_DEVICE_ENTERPRISE_CONNECT
        <#lt>#define WDRV_WINC_DEVICE_EXT_CONNECT_PARAMS
        <#lt>#define WDRV_WINC_DEVICE_BSS_ROAMING
        <#lt>#define WDRV_WINC_DEVICE_FLEXIBLE_FLASH_MAP
        <#lt>#define WDRV_WINC_DEVICE_DYNAMIC_BYPASS_MODE
        <#lt>#define WDRV_WINC_DEVICE_WPA_SOFT_AP
        <#if DRV_WIFI_WINC_DRIVER_MODE == "Socket Mode">
            <#lt>#define WDRV_WINC_DEVICE_CONF_NTP_SERVER
            <#lt>#define WDRV_WINC_DEVICE_HOST_FILE_DOWNLOAD
            <#lt>#define WDRV_WINC_DEVICE_SOFT_AP_EXT
        </#if>
        <#lt>#define WDRV_WINC_DEVICE_MULTI_GAIN_TABLE
    </#if>
<#elseif DRV_WIFI_WINC_DEVICE == "WINC3400">
    <#lt>#define WDRV_WINC_DEVICE_WINC3400
    <#lt>#define WDRV_WINC_DEVICE_SPLIT_INIT
    <#lt>#define WDRV_WINC_DEVICE_MULTI_GAIN_TABLE
    <#if DRV_WIFI_WINC3400_VERSION == "1.2.2">
        <#lt>#define WDRV_WINC_DEVICE_USE_FLASH_INIT
    <#elseif DRV_WIFI_WINC3400_VERSION == "1.3.0">
        <#lt>#define WDRV_WINC_DEVICE_ENTERPRISE_CONNECT
        <#lt>#define WDRV_WINC_DEVICE_EXT_CONNECT_PARAMS
        <#lt>#define WDRV_WINC_DEVICE_BSS_ROAMING
        <#lt>#define WDRV_WINC_DEVICE_DYNAMIC_BYPASS_MODE
        <#if DRV_WIFI_WINC_DRIVER_MODE == "Socket Mode">
            <#lt>#define WDRV_WINC_DEVICE_CONF_NTP_SERVER
            <#lt>#define WDRV_WINC_DEVICE_SOFT_AP_EXT
        </#if>
    </#if>
    <#if DRV_WIFI_WINC_USE_BLUETOOTH_WINC3400>
        <#lt>#define WDRV_WINC_ENABLE_BLE
    </#if>
</#if>
<#if DRV_WIFI_WINC_LOG_LEVEL == "None">
    <#lt>#define WDRV_WINC_DEBUG_LEVEL               WDRV_WINC_DEBUG_TYPE_NONE
<#elseif DRV_WIFI_WINC_LOG_LEVEL == "Error">
    <#lt>#define WDRV_WINC_DEBUG_LEVEL               WDRV_WINC_DEBUG_TYPE_ERROR
<#elseif DRV_WIFI_WINC_LOG_LEVEL == "Inform">
    <#lt>#define WDRV_WINC_DEBUG_LEVEL               WDRV_WINC_DEBUG_TYPE_INFORM
<#elseif DRV_WIFI_WINC_LOG_LEVEL == "Trace">
    <#lt>#define WDRV_WINC_DEBUG_LEVEL               WDRV_WINC_DEBUG_TYPE_TRACE
<#elseif DRV_WIFI_WINC_LOG_LEVEL == "Verbose">
    <#lt>#define WDRV_WINC_DEBUG_LEVEL               WDRV_WINC_DEBUG_TYPE_VERBOSE
</#if>
<#if HarmonyCore.SELECT_RTOS != "BareMetal">
    <#lt>/*** WiFi WINC Driver RTOS Configuration ***/
    <#lt>#define DRV_WIFI_WINC_RTOS_STACK_SIZE           ${DRV_WIFI_WINC_RTOS_STACK_SIZE}
    <#lt>#define DRV_WIFI_WINC_RTOS_TASK_PRIORITY        ${DRV_WIFI_WINC_RTOS_TASK_PRIORITY}
</#if>