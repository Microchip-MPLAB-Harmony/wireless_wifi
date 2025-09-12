/*******************************************************************************
  WINC Wireless Driver BSS Context Source File

  File Name:
    wdrv_winc_bssctx.c

  Summary:
    WINC wireless driver BSS context implementation.

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

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
    if (pBSSCtx->channel > WDRV_WINC_CID_2_4G_CH13)
    {
        return false;
    }

    /* Ensure the SSID length is valid. */
    if (pBSSCtx->ssid.length > WDRV_WINC_MAX_SSID_LEN)
    {
        return false;
    }

    /* Ensure the BSSID is non-zero, if valid. */
    if (true == pBSSCtx->bssid.valid)
    {
        unsigned int i;
        uint8_t macAddrChk;

        macAddrChk = 0;
        for (i=0; i<WDRV_WINC_MAC_ADDR_LEN; i++)
        {
            macAddrChk |= pBSSCtx->bssid.addr[i];
        }

        if (0U == macAddrChk)
        {
            return false;
        }
    }

    if (true == ssidValid)
    {
        /* If requested, check the SSID is present. */
        if (0U == pBSSCtx->ssid.length)
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

    (void)memset(pBSSCtx->bssid.addr, 0, WDRV_WINC_MAC_ADDR_LEN);
    pBSSCtx->bssid.valid = false;

    /* Set context to have no SSID and all channels. */
    pBSSCtx->ssid.length = 0;
    pBSSCtx->channel     = WDRV_WINC_CID_ANY;

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
    if ((NULL == pBSSCtx) || (NULL == pSSID) || (ssidLength > WDRV_WINC_MAX_SSID_LEN))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Copy the SSID ensure unused space is zeroed. */
    (void)memset(&pBSSCtx->ssid.name, 0, WDRV_WINC_MAX_SSID_LEN);
    (void)memcpy(&pBSSCtx->ssid.name, pSSID, ssidLength);
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

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetBSSID
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    uint8_t *const pBSSID
)
{
    /* Ensure BSS context is valid. */
    if (NULL == pBSSCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pBSSID)
    {
        /* Copy the BSSID. */
        (void)memcpy((uint8_t*)&pBSSCtx->bssid, pBSSID, WDRV_WINC_MAC_ADDR_LEN);
        pBSSCtx->bssid.valid = true;
    }
    else
    {
        (void)memset(&pBSSCtx->bssid, 0, WDRV_WINC_MAC_ADDR_LEN);
        pBSSCtx->bssid.valid = false;
    }

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
    WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetChannel
    (
        WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
        WDRV_WINC_CHANNEL_ID channel
    )

  Summary:
    Configures the channel of the BSS context.

  Description:
    The supplied channel value is copied into the BSS context.

  Remarks:
    See wdrv_winc_bssctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSCtxSetChannel
(
    WDRV_WINC_BSS_CONTEXT *const pBSSCtx,
    WDRV_WINC_CHANNEL_ID channel
)
{
    /* Ensure BSS context and channels are valid. */
    if ((NULL == pBSSCtx) || (channel > WDRV_WINC_CID_2_4G_CH13))
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
