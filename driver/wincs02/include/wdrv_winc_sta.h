/*******************************************************************************
  WINC Driver STA Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_sta.h

  Summary:
    WINC wireless driver STA header file.

  Description:
    WINC wireless driver STA header file.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2024 released Microchip Technology Inc. All rights reserved.

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

#ifndef WDRV_WINC_STA_H
#define WDRV_WINC_STA_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"
#include "wdrv_winc_bssctx.h"
#include "wdrv_winc_authctx.h"
#include "wdrv_winc_wifi.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  BSS Roaming Configuration

  Summary:
    Defines the BSS roaming configuration.

  Description:
    This enumeration defines the BSS roaming configuration.

  Remarks:
    None.
*/
typedef enum
{
    /* BSS Roaming is turned off. */
    WDRV_WINC_BSS_ROAMING_CFG_OFF = 0,

    /* BSS Roaming is turned on, layer 2 (WiFi) only. */
    WDRV_WINC_BSS_ROAMING_CFG_ON_L2 = 1,

    /* BSS Roaming is turned on, layer 3 (WiFi/IP). */
    WDRV_WINC_BSS_ROAMING_CFG_ON_L3 = 2,
    WDRV_WINC_BSS_ROAMING_CFG_DEFAULT = WDRV_WINC_BSS_ROAMING_CFG_ON_L3,

    WDRV_WINC_NUM_BSS_ROAMING_CFGS = 3
} WDRV_WINC_BSS_ROAMING_CFG;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver STA Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_WSTAProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        const WINC_DEV_EVENT_RSP_ELEMS *const pElems
    )

  Summary:
    AEC process callback.

  Description:
    Callback will be called to process any AEC messages received.

  Precondition:
    WINC_DevAECCallbackRegister must be called to register the callback.

  Parameters:
    context   - Pointer to user context supplied when callback was registered.
    devHandle - WINC device handle.
    pElems    - Pointer to element structure.

  Returns:
    None.

  Remarks:
    Callback should call WINC_CmdReadParamElem to extract elements.

*/

void WDRV_WINC_WSTAProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSDefaultWiFiCfg
    (
        WDRV_WINC_CONN_CFG *const pWiFiCfg
    )

  Summary:
    Initialises a WiFi configuration structure to default value.

  Description:
    Create a default WiFi configuration structure suitable for connecting
      to a BSS as a STA.

  Precondition:
    None.

  Parameters:
    pWiFiCfg - WiFi configuration.

  Returns:
    WDRV_WINC_STATUS_OK              - The request has been accepted.
    WDRV_WINC_STATUS_INVALID_ARG     - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSDefaultWiFiCfg
(
    WDRV_WINC_CONN_CFG *const pWiFiCfg
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSConnect
    (
        DRV_HANDLE handle,
        const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        const WDRV_WINC_CONN_CFG *pWiFiCfg,
        const WDRV_WINC_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
    )

  Summary:
    Connects to a BSS in infrastructure station mode.

  Description:
    Using the defined BSS and authentication contexts this function requests
      the WINC connect to the BSS as an infrastructure station.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.
    A BSS context must have been created and initialized.
    An authentication context must have been created and initialized.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    pBSSCtx          - Pointer to BSS context.
    pAuthCtx         - Pointer to authentication context.
    pWiFiCfg         - Optional WiFi configuration.
    pfNotifyCallback - Pointer to notification callback function.

  Returns:
    WDRV_WINC_STATUS_OK              - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN        - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG     - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR   - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_CONTEXT - The BSS context is not valid.
    WDRV_WINC_STATUS_CONNECT_FAIL    - The connection has failed.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSConnect
(
    DRV_HANDLE handle,
    const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    const WDRV_WINC_CONN_CFG *pWiFiCfg,
    const WDRV_WINC_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSDisconnect(DRV_HANDLE handle)

  Summary:
    Disconnects from a BSS.

  Description:
    Disconnects from an existing BSS.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    WDRV_WINC_STATUS_OK              - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN        - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG     - The parameters were incorrect.
    WDRV_WINC_STATUS_DISCONNECT_FAIL - The disconnection has failed.
    WDRV_WINC_STATUS_REQUEST_ERROR   - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSDisconnect(DRV_HANDLE handle);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSRoamingConfigure
    (
        DRV_HANDLE handle,
        WDRV_WINC_BSS_ROAMING_CFG roamingCfg
    )

  Summary:
    Configures BSS roaming support.

  Description:
    Enables or disables BSS roaming support. If enabled, the WINC can perform
      a renew of the current IP address if configured to do so, otherwise
      it will assume the existing IP address is still valid.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    roamingCfg - Roaming configuration, see WDRV_WINC_BSS_ROAMING_CFG.

  Returns:
    WDRV_WINC_STATUS_OK              - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN        - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG     - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR   - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSRoamingConfigure
(
    DRV_HANDLE handle,
    WDRV_WINC_BSS_ROAMING_CFG roamingCfg
);

#endif /* WDRV_WINC_STA_H */
