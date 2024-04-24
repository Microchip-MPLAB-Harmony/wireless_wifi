/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_eint.h

  Summary:
    WINC wireless driver interrupt APIs.

  Description:
    Provides an interface to control interrupts used to control the WINC.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

#ifndef WDRV_WINC_EINT_H
#define WDRV_WINC_EINT_H

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
    object  - Driver object handle, returned from WDRV_WINC_Initialize
    intSrc  - Interrupt source

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
    This function deinitializes interrupts for the WiFi driver.

  Precondition:
    WiFi initialization must be complete.

  Returns:
    intSrc  - Interrupt source

  Remarks:
    None.
 */
void WDRV_WINC_INTDeinitialize(int intSrc);

#endif /* WDRV_WINC_EINT_H */
