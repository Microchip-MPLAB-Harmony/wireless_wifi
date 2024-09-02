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
/*
Copyright (C) 2017-21, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include "configuration.h"
#include "definitions.h"

#include "platform.h"
#include "osal/osal.h"

/* When communicating with ATWINC3400, the interface may be unavailable for  */
/* up to 5 seconds in some situations (during WPA2 connection or TLS         */
/* certificate verification). Thus it is recommended that this communication */
/* timeout is set greater than 4000 ms.                                      */
#define TIMEOUT_VALUE 6000

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
