/*** WiFi WINC Driver Configuration ***/
<#if DRV_WIFI_WINC_INT_SRC == "EIC">
    <#lt>#define WDRV_WINC_EIC_SOURCE
<#elseif DRV_WIFI_WINC_INT_SRC == "GPIO">
    <#lt>#define WDRV_WINC_GPIO_SOURCE
<#elseif DRV_WIFI_WINC_INT_SRC == "PIO">
    <#lt>#define WDRV_WINC_PIO_SOURCE
<#elseif DRV_WIFI_WINC_INT_SRC == "External_Interrupt">
<#elseif DRV_WIFI_WINC_INT_SRC == "Change_Notification">
    <#lt>#define WDRV_WINC_INT_SOURCE INT_SOURCE_CHANGE_NOTICE
    <#lt>#define WDRV_WINC_INT_SOURCE_CN_PORT WDRV_WINC_INT_PORT
</#if>
<#if sys_debug?? && sys_debug.SYS_DEBUG_USE_CONSOLE?? && DRV_WIFI_WINC_USE_SYS_DEBUG && sys_debug.SYS_DEBUG_USE_CONSOLE>
    <#lt>#define WDRV_WINC_DEVICE_USE_SYS_DEBUG
<#elseif DRV_WIFI_WINC_LOG_LEVEL == "None">
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
<#if (HarmonyCore.SELECT_RTOS)?? && HarmonyCore.SELECT_RTOS != "BareMetal">
    <#lt>/*** WiFi WINC Driver RTOS Configuration ***/
    <#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
        <#lt>#define WDRV_WINC_RTOS_STACK_SIZE           ${DRV_WIFI_WINC_RTOS_STACK_SIZE / 4}
    <#else>
        <#lt>#define WDRV_WINC_RTOS_STACK_SIZE           ${DRV_WIFI_WINC_RTOS_STACK_SIZE}
    </#if>
    <#lt>#define WDRV_WINC_RTOS_TASK_PRIORITY        ${DRV_WIFI_WINC_RTOS_TASK_PRIORITY}
</#if>
#define WDRV_WINC_DEV_RX_BUFF_SZ            ${DRV_WIFI_WINC_RX_BUFF_SZ}
#define WDRV_WINC_DEV_SOCK_SLAB_NUM         ${DRV_WIFI_WINC_SOCKET_SLAB_NUM}
#define WDRV_WINC_DEV_SOCK_SLAB_SZ          ${DRV_WIFI_WINC_SOCKET_SLAB_SZ}
#define WINC_SOCK_NUM_SOCKETS               ${DRV_WIFI_WINC_SOCKET_NUM}
#define WINC_SOCK_BUF_RX_SZ                 ${DRV_WIFI_WINC_SOCKET_RX_BUF_SZ}
#define WINC_SOCK_BUF_TX_SZ                 ${DRV_WIFI_WINC_SOCKET_TX_BUF_SZ}
#define WINC_SOCK_BUF_RX_PKT_BUF_NUM        ${DRV_WIFI_WINC_SOCKET_RX_PKT_BUF_NUM}
#define WINC_SOCK_BUF_TX_PKT_BUF_NUM        ${DRV_WIFI_WINC_SOCKET_TX_PKT_BUF_NUM}
<#if DRV_WIFI_WINC_MODULE_MQTT_EN == false>
#define WDRV_WINC_MOD_DISABLE_MQTT
</#if>
<#if DRV_WIFI_WINC_MODULE_OTA_EN == false>
#define WDRV_WINC_MOD_DISABLE_OTA
</#if>
<#if DRV_WIFI_WINC_MODULE_NVM_EN == false>
#define WDRV_WINC_MOD_DISABLE_NVM
</#if>
<#if DRV_WIFI_WINC_L3_SUPPORT == false>
#define WDRV_WINC_DISABLE_L3_SUPPORT
</#if>