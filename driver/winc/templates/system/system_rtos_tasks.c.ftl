<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
static void _WDRV_WINC_Tasks(  void *pvParameters  )
{
    while(1)
    {
        SYS_STATUS status;

        WDRV_WINC_Tasks(sysObj.drvWifiWinc);

        status = WDRV_WINC_Status(sysObj.drvWifiWinc);

        if ((SYS_STATUS_ERROR == status) || (SYS_STATUS_UNINITIALIZED == status))
        {
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }
}
</#if>