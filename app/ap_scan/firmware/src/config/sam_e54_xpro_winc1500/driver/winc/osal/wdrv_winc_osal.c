/*******************************************************************************
  WINC Wireless Driver OS Abstraction Layer

  File Name:
    wdrv_winc_osal.c

  Summary:
    OS abstraction layer for WINC wireless driver.

  Description:
    OS abstraction layer for WINC wireless driver.
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

#include "osal/osal.h"
#include "wdrv_winc_common.h"

#if (OSAL_USE_RTOS == 1 || OSAL_USE_RTOS == 9)
void WDRV_MSDelay(uint32_t ms)
{
    if (!ms)
    {
        ms = 1;
    }

    vTaskDelay(ms * portTICK_PERIOD_MS);
}
#else
void WDRV_MSDelay(uint32_t ms)
{
    SYS_TIME_HANDLE tmrHandle = SYS_TIME_HANDLE_INVALID;

    if (SYS_TIME_SUCCESS != SYS_TIME_DelayMS(ms, &tmrHandle))
    {
        return;
    }

    while (true != SYS_TIME_DelayIsComplete(tmrHandle))
    {
    }
}
#endif /* (OSAL_USE_RTOS == 1 || OSAL_USE_RTOS == 9) */

//DOM-IGNORE-END
