/*******************************************************************************
  WINC Driver BSS Context Implementation

  File Name:
    wdrv_winc_bssctx.c

  Summary:
    WINC wireless driver BSS context implementation.

  Description:
    This interface manages the BSS contexts which 'wrap' the state
      associated with BSSs.
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
//DOM-IGNORE-END

#include <stdint.h>
#include <string.h>

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc_common.h"
#include "wdrv_winc_bssctx.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Context Implementation
// *****************************************************************************
// *****************************************************************************

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
    See wdrv_winc_bssctx.h for usage information.

*/

bool WDRV_WINC_BSSCtxIsValid
(
    const WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    bool ssidValid
)
{
    /* Ensure BSS context is valid. */
    if (NULL == pBSSCtx)
    {
        return false;
    }

    /* Ensure the channels are valid. */
    if ((!pBSSCtx->channel) ||
        ((pBSSCtx->channel > 14) && (pBSSCtx->channel != WDRV_WINC_ALL_CHANNELS)))
    {
        return false;
    }

    /* Ensure the SSID length is valid. */
    if (pBSSCtx->ssid.length > 32)
    {
        return false;
    }

    if (true == ssidValid)
    {
        /* If requested, check the SSID is present. */
        if (0 == pBSSCtx->ssid.length)
        {
            return false;
        }
    }

    return true;
}

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

  Remarks:
    See wdrv_winc_bssctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetDefaults
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx
)
{
    /* Ensure BSS context is valid. */
    if (NULL == pBSSCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

#ifdef WDRV_WINC_DEVICE_EXT_CONNECT_PARAMS
    memset(pBSSCtx->bssid, 0, M2M_MAC_ADDRES_LEN);
#endif

    /* Set context to have no SSID, all channels and not cloaked. */
    pBSSCtx->ssid.length = 0;
    pBSSCtx->channel     = WDRV_WINC_ALL_CHANNELS;
    pBSSCtx->cloaked     = false;

    return WDRV_WINC_STATUS_OK;
}

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
    The SSID string and length provided the SSID is copied into the BSS
      context.

  Remarks:
    See wdrv_winc_bssctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetSSID
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    uint8_t *const pSSID,
    uint8_t ssidLength
)
{
    /* Ensure BSS context and SSID buffer and length are valid. */
    if ((NULL == pBSSCtx) || (NULL == pSSID) || (ssidLength > 32))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Copy the SSID ensure unused space is zeroed. */
    memset(&pBSSCtx->ssid.name, 0, 32);
    memcpy(&pBSSCtx->ssid.name, pSSID, ssidLength);
    pBSSCtx->ssid.length = ssidLength;

    /* Validate context. */
    if (false == WDRV_WINC_BSSCtxIsValid(pBSSCtx, false))
    {
        return WDRV_WINC_STATUS_INVALID_CONTEXT;
    }

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_bssctx.h for usage information.

*/

#ifdef WDRV_WINC_DEVICE_EXT_CONNECT_PARAMS
WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetBSSID
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    uint8_t *const pBSSID
)
{
    /* Ensure BSS context and SSID buffer and length are valid. */
    if ((NULL == pBSSCtx) || (NULL == pBSSID))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Copy the BSSID. */
    memcpy(&pBSSCtx->bssid, pBSSID, M2M_MAC_ADDRES_LEN);

    /* Validate context. */
    if (false == WDRV_WINC_BSSCtxIsValid(pBSSCtx, false))
    {
        return WDRV_WINC_STATUS_INVALID_CONTEXT;
    }

    return WDRV_WINC_STATUS_OK;
}
#endif

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetChannel
    (
        WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        uint8_t channel
    )

  Summary:
    Configures the channel of the BSS context.

  Description:
    The supplied channel value is copied into the BSS context.

  Remarks:
    See wdrv_winc_bssctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetChannel(
                        WDRV_WINC_BSS_CONTEXT *const pBSSCtx, uint8_t channel)
{
    /* Ensure BSS context and channels are valid. */
    if ((NULL == pBSSCtx) || (!channel) ||
        ((channel > 14) && (channel != WDRV_WINC_ALL_CHANNELS)))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Copy channel. */
    pBSSCtx->channel = channel;

    /* Validate context. */
    if (false == WDRV_WINC_BSSCtxIsValid(pBSSCtx, false))
    {
        return WDRV_WINC_STATUS_INVALID_CONTEXT;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetSSIDVisibility
    (
        WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        bool visible
    )

  Summary:
    Configures the visibility of the BSS context.

  Description:
    Specific to Soft-AP mode this flag defines if the BSS context will create a
      visible presence on air.

  Remarks:
    See wdrv_winc_bssctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetSSIDVisibility
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    bool visible
)
{
    /* Ensure BSS context is valid. */
    if (NULL == pBSSCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set cloaked state. */
    pBSSCtx->cloaked = visible ? false : true;

    /* Validate context. */
    if (false == WDRV_WINC_BSSCtxIsValid(pBSSCtx, false))
    {
        return WDRV_WINC_STATUS_INVALID_CONTEXT;
    }

    return WDRV_WINC_STATUS_OK;
}
