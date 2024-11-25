/*******************************************************************************
  WINC Driver Soft-AP Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_softap.h

  Summary:
    WINC wireless driver Soft-AP header file.

  Description:
    Provides an interface to create and manage a Soft-AP.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
// DOM-IGNORE-END

#ifndef WDRV_WINC_SOFTAP_H
#define WDRV_WINC_SOFTAP_H

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
// Section: WINC Driver Soft-AP Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_WAPProcessAEC
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

void WDRV_WINC_WAPProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_APDefaultWiFiCfg
    (
        WDRV_WINC_CONN_CFG *const pWiFiCfg
    )

  Summary:
    Initialises a WiFi configuration structure to default value.

  Description:
    Create a default WiFi configuration structure suitable for creating
      a BSS as a Soft-AP.

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

WDRV_WINC_STATUS WDRV_WINC_APDefaultWiFiCfg
(
    WDRV_WINC_CONN_CFG *const pWiFiCfg
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_APStart
    (
        DRV_HANDLE handle,
        const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        const WDRV_WINC_CONN_CFG *pWiFiCfg,
        const WDRV_WINC_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
    )

  Summary:
    Starts an instance of Soft-AP.

  Description:
    Using the defined BSS and authentication contexts with an optional HTTP
      provisioning context (socket mode only) this function creates and starts
      a Soft-AP instance.

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

  Remarks:
    If pBSSCtx and pAuthCtx are both NULL then no AP will be established, however the
    pfNotifyCallback callback will still be accepted, even if an AP is active.

*/

WDRV_WINC_STATUS WDRV_WINC_APStart
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
    WDRV_WINC_STATUS WDRV_WINC_APStop(DRV_HANDLE handle)

  Summary:
    Stops an instance of Soft-AP.

  Description:
    Stops an instance of Soft-AP.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    The AP stopping will be confirmed via the notification callback registered
    by WDRV_WINC_APStart.

*/

WDRV_WINC_STATUS WDRV_WINC_APStop(DRV_HANDLE handle);

#endif /* WDRV_WINC_SOFTAP_H */
