/*******************************************************************************
  WINC Wireless Driver BSS Context Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_bssctx.h

  Summary:
    WINC wireless driver BSS context interface.

  Description:
    This interface manages the BSS contexts which 'wrap' the state associated
    with BSSs.
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

#ifndef WDRV_WINC_BSSCTX_H
#define WDRV_WINC_BSSCTX_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Context Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  BSS Context

  Summary:
    Context structure containing information about BSSs.

  Description:
    The context contains the SSID, channel and visibility of a BSS.

  Remarks:
    None.
*/

typedef struct
{
    /* SSID of BSS. */
    WDRV_WINC_SSID ssid;

    /* BSSID. */
    WDRV_WINC_MAC_ADDR bssid;

    /* Current channel of BSS. */
    WDRV_WINC_CHANNEL_ID channel;
} WDRV_WINC_BSS_CONTEXT;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Context Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_BSSCtxIsValid
    (
        const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        bool ssidValid
    )

  Summary:
    Tests if a BSS context is valid.

  Description:
    Tests the elements of the BSS context to judge if their values are legal.

  Precondition:
    None.

  Parameters:
    pBSSCtx   - Pointer to a BSS context.
    ssidValid - Flag indicating if the SSID within the context must be valid.

  Returns:
    true or false indicating if context is valid.

  Remarks:
    A valid SSID is one which has a non-zero length. The check is optional as
      it is legal for the SSID field to be zero length.

*/

bool WDRV_WINC_BSSCtxIsValid
(
    const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    bool ssidValid
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetDefaults
    (
        WDRV_WINC_BSS_CONTEXT *const pBSSCtx
    )

  Summary:
    Configures a BSS context into a default legal state.

  Description:
    Ensures that each element of the structure is configured into a legal state.

  Precondition:
    None.

  Parameters:
    pBSSCtx - Pointer to a BSS context.

  Returns:
    WDRV_WINC_STATUS_OK             - The context has been configured.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetDefaults
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetSSID
    (
        WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        uint8_t *const pSSID,
        uint8_t ssidLength
    )

  Summary:
    Configures the SSID of the BSS context.

  Description:
    The SSID string and length provided are copied into the BSS context.

  Precondition:
    None.

  Parameters:
    pBSSCtx    - Pointer to a BSS context.
    pSSID      - Pointer to buffer containing the new SSID.
    ssidLength - The length of the SSID held in the pSSID buffer.

  Returns:
    WDRV_WINC_STATUS_OK              - The context has been configured.
    WDRV_WINC_STATUS_INVALID_ARG     - The parameters were incorrect.
    WDRV_WINC_STATUS_INVALID_CONTEXT - The BSS context is not valid.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetSSID
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    uint8_t *const pSSID,
    uint8_t ssidLength
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetBSSID
    (
        WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        uint8_t *const pBSSID
    )

  Summary:
    Configures the BSSID of the BSS context.

  Description:
    The BSSID string is copied into the BSS context.

  Precondition:
    None.

  Parameters:
    pBSSCtx    - Pointer to a BSS context.
    pBSSID     - Pointer to buffer containing the new BSSID.

  Returns:
    WDRV_WINC_STATUS_OK              - The context has been configured.
    WDRV_WINC_STATUS_INVALID_ARG     - The parameters were incorrect.
    WDRV_WINC_STATUS_INVALID_CONTEXT - The BSS context is not valid.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetBSSID
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    uint8_t *const pBSSID
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetChannel
    (
        WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        WDRV_WINC_CHANNEL_ID channel
    )

  Summary:
    Configures the channel of the BSS context.

  Description:
    The supplied channel value is copied into the BSS context.

  Precondition:
    None.

  Parameters:
    pBSSCtx - Pointer to a BSS context.
    channel - Channel number.

  Returns:
    WDRV_WINC_STATUS_OK              - The context has been configured.
    WDRV_WINC_STATUS_INVALID_ARG     - The parameters were incorrect.
    WDRV_WINC_STATUS_INVALID_CONTEXT - The BSS context is not valid.

  Remarks:
    channel may be WDRV_WINC_ALL_CHANNELS or WDRV_WINC_CID_ANY to represent no
      fixed channel.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetChannel
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    WDRV_WINC_CHANNEL_ID channel
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_BSSCTX_H */
