<#--
/*******************************************************************************
* Copyright (C) 2019-21 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
-->
<#if HarmonyCore.SELECT_RTOS == "BareMetal">
    WDRV_WINC_Tasks(sysObj.drvWifiWinc);
<#elseif HarmonyCore.SELECT_RTOS == "FreeRTOS">
    xTaskCreate( _WDRV_WINC_Tasks,
        "WDRV_WINC_Tasks",
        DRV_WIFI_WINC_RTOS_STACK_SIZE,
        (void*)NULL,
        DRV_WIFI_WINC_RTOS_TASK_PRIORITY,
        (TaskHandle_t*)NULL
    );
<#elseif HarmonyCore.SELECT_RTOS == "ThreadX">
    /* Allocate the stack for _WDRV_WINC_Tasks thread */
    tx_byte_allocate(&byte_pool_0,
        (VOID **) &_WDRV_WINC_Task_Stk_Ptr,
        ${DRV_WIFI_WINC_RTOS_STACK_SIZE},
        TX_NO_WAIT
    );

    /* create the _WDRV_WINC_Tasks thread */
    tx_thread_create(&_WDRV_WINC_Task_TCB,
        (char*)"_WDRV_WINC_Tasks",
        _WDRV_WINC_Tasks,
        0,
        _WDRV_WINC_Task_Stk_Ptr,
        ${DRV_WIFI_WINC_RTOS_STACK_SIZE},
        ${DRV_WIFI_WINC_RTOS_TASK_PRIORITY},
        ${DRV_WIFI_WINC_RTOS_TASK_PRIORITY},
        TX_NO_TIME_SLICE,
        TX_AUTO_START
    );
</#if>

<#--
/*******************************************************************************
 End of File
*/
-->

