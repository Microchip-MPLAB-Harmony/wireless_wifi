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
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
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
        plf_params.ble_write_cb(data, len);
}

void platform_cmd_cmpl_signal()
{
    OSAL_SEM_Post(&cmdCmplSemaphore);
}

bool platform_cmd_cmpl_wait(void)
{
#if (OSAL_USE_RTOS == 1 || OSAL_USE_RTOS == 9)
    if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&cmdCmplSemaphore, TIMEOUT_VALUE))
    {
        return false;
    }
    else
    {
        return true;
    }
#else
    SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

    if (SYS_TIME_DelayMS(TIMEOUT_VALUE, &timer) != SYS_TIME_SUCCESS)
    {
        // Handle error
    }
    else if(SYS_TIME_DelayIsComplete(timer) != true)
    {
        // Wait till the delay has not expired
        while (SYS_TIME_DelayIsComplete(timer) == false)
        {
            if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&cmdCmplSemaphore, 1))
            {
                return false;
            } 
            if (NULL != plf_params.plf_wait_cb)
                plf_params.plf_wait_cb(plf_params.drvHandle);
        }
    }
    return true;
#endif
}

void platform_event_signal()
{
    OSAL_SEM_Post(&eventSemaphore);
}

uint8_t platform_event_wait(uint32_t timeout)
{
#if (OSAL_USE_RTOS == 1 || OSAL_USE_RTOS == 9)
    if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&eventSemaphore, timeout))
    {
    }
    else
    {
        return AT_BLE_TIMEOUT;
    }

    return AT_BLE_SUCCESS;
#else
    if (0 != timeout)
    {
        SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

        if (SYS_TIME_DelayMS(timeout, &timer) != SYS_TIME_SUCCESS)
        {
        // Handle error
        }
        else if(SYS_TIME_DelayIsComplete(timer) != true)
        {
            // Wait till the delay has not expired
            while (SYS_TIME_DelayIsComplete(timer) == false)
            {
                if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&cmdCmplSemaphore, 1))
                {
                    return AT_BLE_SUCCESS;
                } 
                if (NULL != plf_params.plf_wait_cb)
                plf_params.plf_wait_cb(plf_params.drvHandle);
            }
        }
    }
    else
    {
        if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&cmdCmplSemaphore, 0))
        {
            return AT_BLE_SUCCESS;
        }
    }
#endif
    return AT_BLE_TIMEOUT;
}

//DOM-IGNORE-END
