<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>void WDRV_WINC_Tasks(  void *pvParameters  )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>        WDRV_WINC_Tasks(sysObj.drvWifiWinc);
             <#if DRV_WIFI_RTOS_USE_DELAY >
    <#lt>        vTaskDelay(${DRV_WIFI_RTOS_DELAY} / portTICK_PERIOD_MS);
             </#if>
    <#lt>    }
    <#lt>}
</#if>