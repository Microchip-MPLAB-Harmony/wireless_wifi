/*******************************************************************************
  WINC Driver WPS Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_wps.h

  Summary:
    WINC wireless driver WPS header file.

  Description:
    Provides an interface use WPS for BSS discovery.
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

#ifndef _WDRV_WINC_WPS_H
#define _WDRV_WINC_WPS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "configuration.h"
#include "definitions.h"
#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver WPS Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  WPS Discovery Callback

  Summary:
    Callback to report WPS discovery results.

  Description:
    Callback to report WPS discovery results.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_WPS_DISC_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_BSS_CONTEXT *pBSSCtx,
    WDRV_WINC_AUTH_CONTEXT *pAuthCtx
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver WPS Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStartPIN
    (
        DRV_HANDLE handle,
        uint32_t pin,
        const WDRV_WINC_WPS_DISC_CALLBACK pfWPSDiscoveryCallback
    )

  Summary:
    Start WPS discovery (PIN).

  Description:
    Starts the WPS discovery as an enrollee using the PIN method.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle                 - Client handle obtained by a call to WDRV_WINC_Open.
    pin                    - WPS PIN number.
    pfWPSDiscoveryCallback - Callback function to return discovery results to.

  Returns:
    WDRV_WINC_STATUS_OK            - Discovery request accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    The pin number is an 8-digit number where the least significant digit is
      a checksum as defined by WPS.

*/

WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStartPIN
(
    DRV_HANDLE handle,
    uint32_t pin,
    const WDRV_WINC_WPS_DISC_CALLBACK pfWPSDiscoveryCallback
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStartPBC
    (
        DRV_HANDLE handle,
        const WDRV_WINC_WPS_DISC_CALLBACK pfWPSDiscoveryCallback
    )

  Summary:
    Start WPS discovery (Push Button).

  Description:
    Starts the WPS discovery as an enrollee using the Push Button method.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle                 - Client handle obtained by a call to WDRV_WINC_Open.
    pfWPSDiscoveryCallback - Callback function to return discovery results to.

  Returns:
    WDRV_WINC_STATUS_OK            - Discovery request accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStartPBC
(
    DRV_HANDLE handle,
    const WDRV_WINC_WPS_DISC_CALLBACK pfWPSDiscoveryCallback
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStop(DRV_HANDLE handle)

  Summary:
    Stop WPS discovery.

  Description:
    Stops the WPS discovery if it was started.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle                 - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    WDRV_WINC_STATUS_OK            - Discovery request accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStop(DRV_HANDLE handle);

#endif /* _WDRV_WINC_WPS_H */
