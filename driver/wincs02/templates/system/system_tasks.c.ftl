<#--
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
*/
-->
<#if HarmonyCore.SELECT_RTOS == "BareMetal">
    WDRV_WINC_Tasks(sysObj.drvWifiWinc);
<#elseif HarmonyCore.SELECT_RTOS == "FreeRTOS">
    xTaskCreate( lWDRV_WINC_Tasks,
        "WDRV_WINC_Tasks",
        WDRV_WINC_RTOS_STACK_SIZE,
        (void*)NULL,
        WDRV_WINC_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );
<#elseif HarmonyCore.SELECT_RTOS == "ThreadX">
    /* Allocate the stack for lWDRV_WINC_Tasks thread */
    tx_byte_allocate(&byte_pool_0,
        (VOID **) &lWDRV_WINC_Task_Stk_Ptr,
        WDRV_WINC_RTOS_STACK_SIZE,
        TX_NO_WAIT
    );

    /* create the lWDRV_WINC_Tasks thread */
    tx_thread_create(&lWDRV_WINC_Task_TCB,
        (char*)"lWDRV_WINC_Tasks",
        lWDRV_WINC_Tasks,
        0,
        lWDRV_WINC_Task_Stk_Ptr,
        WDRV_WINC_RTOS_STACK_SIZE,
        WDRV_WINC_RTOS_TASK_PRIORITY,
        WDRV_WINC_RTOS_TASK_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START
    );
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->

