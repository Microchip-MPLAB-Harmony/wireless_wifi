/*******************************************************************************
  File Name:
    nm_common.c

  Summary:
    This module contains common APIs implementations.

  Description:
    This module contains common APIs implementations.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

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

#include "nm_common.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_gpio.h"

/*!
 *  @fn         nm_sleep
 *  @brief      Sleep in units of mSec
 *  @param[IN]  u32TimeMsec
 *              Time in milliseconds
 */
void nm_sleep(uint32_t u32TimeMsec)
{
    WDRV_MSDelay(u32TimeMsec);
}

/*!
 *  @fn     nm_reset
 *  @brief  Reset NMC3400 SoC by setting CHIP_EN and RESET_N signals low,
 *           CHIP_EN high then RESET_N high
 */
void nm_reset(void)
{
    WDRV_WINC_GPIOChipEnableDeassert();
    WDRV_WINC_GPIOResetAssert();
    nm_sleep(100);
    WDRV_WINC_GPIOChipEnableAssert();
    nm_sleep(10);
    WDRV_WINC_GPIOResetDeassert();
    nm_sleep(10);
}

//DOM-IGNORE-END
