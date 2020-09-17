<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
    <#lt>static void _WDRV_PIC32MZW1_Tasks(  void *pvParameters  )
    <#lt>{
    <#lt>    while(1)
    <#lt>    {
    <#lt>        WDRV_PIC32MZW_Tasks(sysObj.drvWifiPIC32MZW1);
    <#lt>    }
    <#lt>}
</#if>