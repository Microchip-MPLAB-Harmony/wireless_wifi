/*******************************************************************************
  WINC Wireless Driver External Interrupt Handler

  File Name:
    wdrv_winc_eint.c

  Summary:
    External interrupt handler for WINC wireless driver.

  Description:
    External interrupt handler for WINC wireless driver.
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
#include <stdbool.h>
#include "configuration.h"
#include "definitions.h"

#if defined(INT_SOURCE_CHANGE_NOTICE) && (WDRV_WINC_INT_SOURCE == INT_SOURCE_CHANGE_NOTICE)
#ifdef PLIB_PORTS_ExistsPinChangeNoticePerPort
#define WDRV_INT_SOURCE WDRV_WINC_INT_SOURCE_CN_PORT
#else
#define WDRV_INT_SOURCE INT_SOURCE_CHANGE_NOTICE
#endif
#elif defined(WDRV_WINC_EIC_SOURCE)
#else
#define WDRV_INT_SOURCE WDRV_WINC_INT_SOURCE
#endif

/****************************************************************************
 * Function:        WDRV_WINC_INTInitialize
 * Summary: Initializes interrupts for the WiFi driver.
 *****************************************************************************/
void WDRV_WINC_INTInitialize(void)
{
#ifdef WDRV_WINC_EIC_SOURCE
    EIC_CallbackRegister(WDRV_WINC_EIC_SOURCE, (EIC_CALLBACK)WDRV_WINC_ISR, 0);
    EIC_InterruptEnable(WDRV_WINC_EIC_SOURCE);
#else
    /* disable the external interrupt */
    SYS_INT_SourceDisable(WDRV_INT_SOURCE);

    /* clear and enable the interrupt */
    SYS_INT_SourceStatusClear(WDRV_INT_SOURCE);

    /* enable the external interrupt */
    SYS_INT_SourceEnable(WDRV_INT_SOURCE);
#endif
}

/****************************************************************************
 * Function:        WDRV_WINC_INTDeinitialize
 * Summary: Deinitializes interrupts for WiFi driver.
 *****************************************************************************/
void WDRV_WINC_INTDeinitialize(void)
{
#ifdef WDRV_WINC_EIC_SOURCE
    EIC_InterruptEnable(WDRV_WINC_EIC_SOURCE);
#else
    SYS_INT_SourceDisable(WDRV_INT_SOURCE);
#endif
}

//DOM-IGNORE-END
