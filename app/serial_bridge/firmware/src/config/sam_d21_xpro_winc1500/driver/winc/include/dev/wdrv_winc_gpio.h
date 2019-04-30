/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_gpio.h

  Summary:
    WINC wireless driver GPIO APIs.

  Description:
    Provides an interface to control GPIOs used to control the WINC.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef _WDRV_WINC_GPIO_H
#define _WDRV_WINC_GPIO_H

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_INTInitialize(void)

  Summary:
    Initializes interrupts for the WiFi driver.

  Description:
    This function initializes interrupts for the WiFi driver.

  Precondition:
    WiFi initialization must be complete.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_INTInitialize(void);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_INTDeinitialize(void)

  Summary:
    Deinitializes interrupts for WiFi driver.

  Description:
    This function deinitializes interrupts for the WiFi driver.

  Precondition:
    WiFi initialization must be complete.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_INTDeinitialize(void);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_GPIOChipEnableAssert(void)

  Summary:
    Asserts the chip enable list.

  Description:
    Enables the WINC by asserting the chip enable line.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_GPIOChipEnableAssert(void);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_GPIOChipEnableDeassert(void)

  Summary:
    Deassert the chip enable line.

  Description:
    Disables the WINC by deasserting the chip enable line.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_GPIOChipEnableDeassert(void);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_GPIOResetAssert(void)

  Summary:
    Assert the reset line.

  Description:
    Reset the WINC by asserting the reset line.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_GPIOResetAssert(void);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_GPIOResetDeassert(void)

  Summary:
    Deassert the reset line.

  Description:
    Deassert the reset line to take the WINC out of reset.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_GPIOResetDeassert(void);

#endif /* _WDRV_WINC_GPIO_H */
