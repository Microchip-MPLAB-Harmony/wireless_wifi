/*******************************************************************************
  WINC Wireless Driver GPIO Interface

  File Name:
    wdrv_winc_gpio.c

  Summary:
    GPIO interface for WINC wireless driver.

  Description:
    GPIO interface for WINC wireless driver.
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

#include <stdlib.h>
#include <stdint.h>
#include "configuration.h"
#include "definitions.h"

/****************************************************************************
 * Function:        WDRV_WINC_GPIOResetAssert
 * Summary: Reset the WINC by asserting the reset line.
 *****************************************************************************/
void WDRV_WINC_GPIOResetAssert(void)
{
    SYS_PORT_PinClear(PORT_PIN_PB06);
    //ramya
    //SYS_PORTS_PinClear(PORTS_ID_0, WDRV_WINC_RESETN_PORT, WDRV_WINC_RESETN_PIN);
}

/****************************************************************************
 * Function:        WDRV_WINC_GPIOResetDeassert
 * Summary: Deassert the reset line.
 *****************************************************************************/
void WDRV_WINC_GPIOResetDeassert(void)
{
    SYS_PORT_PinSet(PORT_PIN_PB06);
    //ramya
    //SYS_PORTS_PinSet(PORTS_ID_0, WDRV_WINC_RESETN_PORT, WDRV_WINC_RESETN_PIN);
}

/****************************************************************************
 * Function:        WDRV_WINC_GPIOChipEnableAssert
 * Summary: Asserts the chip enable list.
 *****************************************************************************/
void WDRV_WINC_GPIOChipEnableAssert(void)
{
    SYS_PORT_PinSet(PORT_PIN_PB05);
    //ramya
    //SYS_PORTS_PinSet(PORTS_ID_0, WDRV_WINC_CHIP_EN_PORT, WDRV_WINC_CHIP_EN_PIN);
}

/****************************************************************************
 * Function:        WDRV_WINC_GPIOChipEnableDeassert
 * Summary: Deassert the chip enable line.
 *****************************************************************************/
void WDRV_WINC_GPIOChipEnableDeassert(void)
{
    SYS_PORT_PinClear(PORT_PIN_PB05);
    //ramya
    //SYS_PORTS_PinClear(PORTS_ID_0, WDRV_WINC_CHIP_EN_PORT, WDRV_WINC_CHIP_EN_PIN);
}

//DOM-IGNORE-END
