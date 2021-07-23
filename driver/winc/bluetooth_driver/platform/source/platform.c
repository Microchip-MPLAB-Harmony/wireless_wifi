/*******************************************************************************
  WINC3400 BLE Platform Interface

  File Name:
    platform.c

  Summary:
    WINC3400 BLE Platform Interface.

  Description:
    WINC3400 BLE Platform Interface.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2017-21 Microchip Technology Inc. and its subsidiaries.
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
#include "configuration.h"
#include "definitions.h"

#include "platform.h"
#include "osal/osal.h"

#define TIMEOUT_VALUE 4000

static OSAL_SEM_HANDLE_TYPE cmdCmplSemaphore;
static OSAL_SEM_HANDLE_TYPE eventSemaphore;
static OSAL_SEM_HANDLE_TYPE sendSemaphore;

static plf_params_t plf_params;

void platform_send_lock_aquire(void)
{
    OSAL_SEM_Pend(&sendSemaphore, OSAL_WAIT_FOREVER);
}

void platform_send_lock_release(void)
{
    OSAL_SEM_Post(&sendSemaphore);
}

at_ble_status_t platform_init(plf_params_t* platform_params)
{
    if (OSAL_RESULT_TRUE != OSAL_SEM_Create(&cmdCmplSemaphore, OSAL_SEM_TYPE_COUNTING, 10, 0))
    {
        return AT_BLE_FAILURE;
    }

    if (OSAL_RESULT_TRUE != OSAL_SEM_Create(&eventSemaphore, OSAL_SEM_TYPE_COUNTING, 10, 0))
    {
        return AT_BLE_FAILURE;
    }

    if (OSAL_RESULT_TRUE != OSAL_SEM_Create(&sendSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1))
    {
        return AT_BLE_FAILURE;
    }

    memcpy(&plf_params, platform_params, sizeof(plf_params_t));

    return AT_BLE_SUCCESS;
}

void platform_interface_send(uint8_t* data, uint32_t len)
{
    if (NULL != plf_params.ble_write_cb)
    {
        plf_params.ble_write_cb(data, len);
    }
}

void platform_cmd_cmpl_signal()
{
    OSAL_SEM_Post(&cmdCmplSemaphore);
}

bool platform_cmd_cmpl_wait(void)
{
#ifndef DRV_WIFI_WINC_RTOS_STACK_SIZE
    SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

    if (SYS_TIME_DelayMS(TIMEOUT_VALUE, &timer) != SYS_TIME_SUCCESS)
    {
        // Handle error
    }
    else if (SYS_TIME_DelayIsComplete(timer) != true)
    {
        // Wait while the delay has not expired
        while (SYS_TIME_DelayIsComplete(timer) == false)
        {
            if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&cmdCmplSemaphore, 1))
            {
                SYS_TIME_TimerDestroy(timer);
                return false;
            }

            if (NULL != plf_params.plf_wait_cb)
            {
                plf_params.plf_wait_cb(plf_params.drvHandle);
            }
        }
    }
#else
    if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&cmdCmplSemaphore, TIMEOUT_VALUE))
    {
        return false;
    }
#endif
    return true;
}

void platform_event_signal()
{
    OSAL_SEM_Post(&eventSemaphore);
}

uint8_t platform_event_wait(uint32_t timeout)
{
#ifndef DRV_WIFI_WINC_RTOS_STACK_SIZE
    if (0 != timeout)
    {
        SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

        if (SYS_TIME_DelayMS(timeout, &timer) != SYS_TIME_SUCCESS)
        {
        // Handle error
        }
        else if (SYS_TIME_DelayIsComplete(timer) != true)
        {
            // Wait while the delay has not expired
            while (SYS_TIME_DelayIsComplete(timer) == false)
            {
                if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&eventSemaphore, 1))
                {
                    SYS_TIME_TimerDestroy(timer);
                    return AT_BLE_SUCCESS;
                }

                if (NULL != plf_params.plf_wait_cb)
                {
                    plf_params.plf_wait_cb(plf_params.drvHandle);
                }
            }
        }
    }
    else
    {
        if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&eventSemaphore, 0))
        {
            return AT_BLE_SUCCESS;
        }
    }
#else
    if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&eventSemaphore, timeout))
    {
        return AT_BLE_SUCCESS;
    }
#endif
    return AT_BLE_TIMEOUT;
}

//DOM-IGNORE-END
