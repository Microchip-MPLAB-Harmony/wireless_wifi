<#if HarmonyCore.SELECT_RTOS == "ThreadX">
TX_THREAD   lWDRV_WINC_Task_TCB;
uint8_t*    lWDRV_WINC_Task_Stk_Ptr;

</#if>
<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
static void lWDRV_WINC_Tasks(void *pvParameters)
<#elseif HarmonyCore.SELECT_RTOS == "ThreadX">
static void lWDRV_WINC_Tasks(ULONG thread_input)
</#if>
{
    while(1)
    {
        SYS_STATUS status;

        WDRV_WINC_Tasks(sysObj.drvWifiWinc);

        status = WDRV_WINC_Status(sysObj.drvWifiWinc);

        if ((SYS_STATUS_ERROR == status) || (SYS_STATUS_UNINITIALIZED == status))
        {
<#if HarmonyCore.SELECT_RTOS == "FreeRTOS">
            vTaskDelay(50 / portTICK_PERIOD_MS);
<#elseif HarmonyCore.SELECT_RTOS == "ThreadX">
            tx_thread_sleep((ULONG)(50 / (TX_TICK_PERIOD_MS)));
</#if>
        }
    }
}
