/*******************************************************************************
  WINC Wireless Driver External Interrupt Handler Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_eint.h

  Summary:
    WINC wireless driver external interrupt handler interface.

  Description:
    Provides an interface to control the interrupts used by the WINC.
 *******************************************************************************/

/*
Copyright (C) 2024-25 Microchip Technology Inc. and its subsidiaries. All rights reserved.

Subject to your compliance with these terms, you may use this Microchip software and any derivatives
exclusively with Microchip products. You are responsible for complying with third party license terms
applicable to your use of third party software (including open source software) that may accompany this
Microchip software. SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR
STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-
INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL
MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS,
DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER
CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY
TO MICROCHIP FOR THIS SOFTWARE.
*/

#ifndef WDRV_WINC_EINT_H
#define WDRV_WINC_EINT_H

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver External Interrupt Handler Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_INTInitialize(SYS_MODULE_OBJ object, int intSrc)

  Summary:
    Initializes interrupts for the WiFi driver.

  Description:
    This function initializes interrupts for the WiFi driver.

  Precondition:
    WDRV_WINC_Initialize must have been called before calling this function.

  Parameters:
    object  - Driver object handle, returned from WDRV_WINC_Initialize.
    intSrc  - Interrupt source.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_INTInitialize(SYS_MODULE_OBJ object, int intSrc);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_INTDeinitialize(int intSrc)

  Summary:
    Deinitializes interrupts for WiFi driver.

  Description:
    This function de-initializes interrupts for the WiFi driver.

  Precondition:
    WDRV_WINC_Initialize must have been called before calling this function.

  Returns:
    intSrc  - Interrupt source.

  Remarks:
    None.
 */
void WDRV_WINC_INTDeinitialize(int intSrc);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_EINT_H */
