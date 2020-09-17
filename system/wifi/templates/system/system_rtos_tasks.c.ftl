<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>void _SYS_WIFI_Task(  void *pvParameters  )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>        SYS_WIFI_Tasks(sysObj.syswifi);
             <#if SYS_WIFI_RTOS_USE_DELAY == true>
    <#lt>        vTaskDelay(${SYS_WIFI_RTOS_DELAY} / portTICK_PERIOD_MS);
             </#if>
    <#lt>    }
    <#lt>}
</#if>