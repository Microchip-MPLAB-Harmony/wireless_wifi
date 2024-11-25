/*******************************************************************************
  WINC Driver Authentication Context Implementation

  File Name:
    wdrv_winc_authctx.c

  Summary:
    WINC wireless driver authentication context implementation.

  Description:
    This interface manages the authentication contexts which 'wrap' the state
      associated with authentication schemes.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_winc_common.h"
#include "wdrv_winc_authctx.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Data Types
// *****************************************************************************
// *****************************************************************************

/* Bits for 11i info. */
#define DRV_WINC_11I_NONE           0x0000U
#define DRV_WINC_PRIVACY            0x0001U     // Not 11i, but included here for convenience
#define DRV_WINC_SKEY               0x0002U     // Not 11i, but included here for convenience
#define DRV_WINC_11I_WEP            0x0004U
#define DRV_WINC_11I_WEP104         0x0008U
#define DRV_WINC_11I_WPAIE          0x0010U
#define DRV_WINC_11I_RSNE           0x0020U
#define DRV_WINC_11I_CCMP128        0x0040U
#define DRV_WINC_11I_TKIP           0x0080U
#define DRV_WINC_11I_BIPCMAC128     0x0100U
#define DRV_WINC_11I_MFP_REQUIRED   0x0200U
#define DRV_WINC_11I_1X             0x0400U
#define DRV_WINC_11I_PSK            0x0800U
#define DRV_WINC_11I_SAE            0x1000U
#define DRV_WINC_11I_TD             0x2000U
#define DRV_WINC_AP                 0x8000U     // Indicates whether the settings are intended for STA or AP mode
#define DRV_WINC_RSNA_MASK          0x3FF0U     // Mask of bits linked to RSNA's

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Global Data
// *****************************************************************************
// *****************************************************************************

/* Table to convert auth types to 11i info. */
static const uint16_t mapAuthTypeTo11i[] =
{
    /* WDRV_WINC_AUTH_TYPE_OPEN */
    DRV_WINC_11I_NONE,
    /* Gap 1 */
    DRV_WINC_11I_NONE,
    /* WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL */
    DRV_WINC_PRIVACY
        | DRV_WINC_11I_WPAIE | DRV_WINC_11I_TKIP
        | DRV_WINC_11I_RSNE | DRV_WINC_11I_CCMP128
        | DRV_WINC_11I_BIPCMAC128
        | DRV_WINC_11I_PSK,
    /* WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL */
    DRV_WINC_PRIVACY
        | DRV_WINC_11I_RSNE | DRV_WINC_11I_CCMP128
        | DRV_WINC_11I_BIPCMAC128
        | DRV_WINC_11I_PSK,
    /* WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL */
    DRV_WINC_PRIVACY
        | DRV_WINC_11I_RSNE | DRV_WINC_11I_CCMP128
        | DRV_WINC_11I_BIPCMAC128
        | DRV_WINC_11I_PSK | DRV_WINC_11I_SAE,
    /* WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL */
    DRV_WINC_PRIVACY
        | DRV_WINC_11I_RSNE | DRV_WINC_11I_CCMP128
        | DRV_WINC_11I_BIPCMAC128 | DRV_WINC_11I_MFP_REQUIRED
        | DRV_WINC_11I_SAE,
};

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    bool authctxPersonalKeyIsValid
    (
        const uint8_t *const pPassword,
        uint8_t size,
        uint16_t dot11iInfo
    )

  Summary:
    Checks if personal key is valid.

  Description:
    Determines if the personal key and size are valid.

  Precondition:
    None.

  Parameters:
    pPassword  - Pointer to personal key.
    size       - Size of personal key.
    dot11iInfo - .11i information.

  Returns:
    true or false indicating if personal key information is valid.

*/

static bool authctxPersonalKeyIsValid
(
    const uint8_t *const pPassword,
    uint8_t size,
    uint16_t dot11iInfo
)
{
    /* Check password is present. */
    if (NULL == pPassword)
    {
        return false;
    }

    /* If password is to be used for SAE, we place the same upper limit on
     * length as for PSK passphrases. Note this is an implementation-specific
     * restriction, not an 802.11 (2016) restriction. */
    if (DRV_WINC_11I_NONE != (dot11iInfo & DRV_WINC_11I_SAE))
    {
        if (WDRV_WINC_MAX_PSK_PASSWORD_LEN < size)
        {
            return false;
        }
    }

    if (DRV_WINC_11I_NONE != (dot11iInfo & DRV_WINC_11I_PSK))
    {
        /* Determine if this is a password or direct PSK. */
        if (WDRV_WINC_PSK_LEN == size)
        {
            /* PSK. */
            while (0U != (size--))
            {
                char character = (char)(pPassword[size]);

                /* Each character must be in the range '0-9', 'A-F' or 'a-f'. */
                if (
                        (('0' > character) || ('9' < character))
                    &&  (('A' > character) || ('F' < character))
                    &&  (('a' > character) || ('f' < character))
                )
                {
                    return false;
                }
            }
        }
        else
        {
            /* Password. */
            /* Check password size. */
            if (
                    (WDRV_WINC_MAX_PSK_PASSWORD_LEN < size)
                ||  (WDRV_WINC_MIN_PSK_PASSWORD_LEN > size)
            )
            {
                return false;
            }

            /* Each character must be in the range 0x20 to 0x7e. */
            while (0U != (size--))
            {
                if ((0x20U > pPassword[size]) || (0x7eU < pPassword[size]))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

//*******************************************************************************
/*
  Function:
    uint16_t authctxGet11iMask
    (
        WDRV_WINC_AUTH_TYPE authType,
        uint8_t authMod
    )

  Summary:
    Convert authentication type and modifiers to 11i info.

  Description:

  Precondition:
    None.

  Parameters:
    authType - Auth type to convert.
    authMod  - Modifiers to the authentication type.

  Returns:
    11i info mapped from auth type and modifiers.
*/

static uint16_t authctxGet11iMask
(
    WDRV_WINC_AUTH_TYPE authType,
    uint8_t authMod
)
{
    uint16_t dot11iInfo;

    if ((authType >= WDRV_WINC_AUTH_TYPE_MAX) || (authType <= WDRV_WINC_AUTH_TYPE_DEFAULT))
    {
        return DRV_WINC_11I_NONE;
    }

    /* Convert auth type to 11i info. */
    dot11iInfo = mapAuthTypeTo11i[authType];

    /* Apply any relevant modifiers. */
    if (WDRV_WINC_AUTH_MOD_NONE != (authMod & WDRV_WINC_AUTH_MOD_MFP_REQ))
    {
        if (
                (WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL == authType)
            ||  (WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL == authType)
        )
        {
            dot11iInfo |= DRV_WINC_11I_BIPCMAC128 | DRV_WINC_11I_MFP_REQUIRED;
        }
    }
    else if (WDRV_WINC_AUTH_MOD_NONE != (authMod & WDRV_WINC_AUTH_MOD_MFP_OFF))
    {
        if (
                (WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL == authType)
            ||  (WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL == authType)
        )
        {
            dot11iInfo &= (0xffffU ^ DRV_WINC_11I_BIPCMAC128);
        }
    }
    else
    {
        /* Do Nothing. */
    }

    return dot11iInfo;
}

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_AuthCtxIsValid(const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx)

  Summary:
    Tests if an authentication context is valid.

  Description:
    Tests the elements of the authentication context to judge if their values are legal.

  Remarks:
    See wdrv_winc_authctx.h for usage information.

*/

bool WDRV_WINC_AuthCtxIsValid(const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx)
{
    bool retVal = true;

    /* Ensure authentication context is valid. */
    if (NULL == pAuthCtx)
    {
        return false;
    }

    switch (pAuthCtx->authType)
    {
        /* Nothing to check for Open authentication. */
        case WDRV_WINC_AUTH_TYPE_OPEN:
        {
            break;
        }

        /* Check Personal authentication. */
        case WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL:
        case WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL:
        case WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL:
        case WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL:
        {
            if (false == authctxPersonalKeyIsValid(
                    pAuthCtx->authInfo.personal.password,
                    pAuthCtx->authInfo.personal.size,
                    authctxGet11iMask(pAuthCtx->authType, pAuthCtx->authMod)
            ))
            {
                retVal = false;
            }
            break;
        }

        default:
        {
            /* Unknown authentication scheme. */
            retVal = false;
            break;
        }
    }

    return retVal;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetDefaults
    (
        WDRV_WINC_AUTH_CONTEXT *const pAuthCtx
    )

  Summary:
    Configures an authentication context into a default state.

  Description:
    Ensures that each element of the structure is configured into a default state.

  Remarks:
    See wdrv_winc_authctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetDefaults
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx
)
{
    /* Ensure authentication context is valid. */
    if (NULL == pAuthCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    (void)memset(pAuthCtx, 0, sizeof(WDRV_WINC_AUTH_CONTEXT));

    /* Ensure authentication type is a known invalid type. */
    pAuthCtx->authType = WDRV_WINC_AUTH_TYPE_DEFAULT;

    /* Initialise auth modifiers to 0. */
    pAuthCtx->authMod = WDRV_WINC_AUTH_MOD_NONE;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetOpen
    (
        WDRV_WINC_AUTH_CONTEXT *const pAuthCtx
    )

  Summary:
    Configure an authentication context for Open authentication.

  Description:
    The auth type and information are configured appropriately for Open
      authentication.

  Remarks:
    See wdrv_winc_authctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetOpen
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx
)
{
    /* Ensure authentication context is valid. */
    if (NULL == pAuthCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set authentication type to Open. */
    pAuthCtx->authType = WDRV_WINC_AUTH_TYPE_OPEN;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetPersonal
    (
        WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        uint8_t *pPassword,
        uint8_t size,
        WDRV_WINC_AUTH_TYPE authType
    )

  Summary:
    Configure an authentication context for WPA personal authentication.

  Description:
    The auth type and information are configured appropriately for personal
      authentication with the password or PSK provided. The Management Frame
      Protection configuration is initialised to WDRV_WINC_AUTH_MFP_ENABLED.

  Remarks:
    See wdrv_winc_authctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetPersonal
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    uint8_t *const pPassword,
    uint8_t size,
    WDRV_WINC_AUTH_TYPE authType
)
{
    uint16_t dot11iInfo;

    /* Ensure authentication context is valid. */
    if ((NULL == pAuthCtx) || (NULL == pPassword))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (WDRV_WINC_AUTH_TYPE_DEFAULT == authType)
    {
        /* Set authentication type to WPA2/WPA3 transition mode. */
        authType = WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL;
    }

    dot11iInfo = authctxGet11iMask(authType, WDRV_WINC_AUTH_MOD_NONE);

    /* Ensure the requested auth type is valid for Personal authentication. */
    if (!(dot11iInfo & (DRV_WINC_11I_PSK | DRV_WINC_11I_SAE)))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the password is valid. */
    if (false == authctxPersonalKeyIsValid(pPassword, size, dot11iInfo))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set authentication type. */
    pAuthCtx->authType = authType;

    /* Initialise the MFP configuration to WDRV_WINC_AUTH_MFP_ENABLED.   */
    /* The Application may change the configuration later if desired via     */
    /* WDRV_WINC_AuthCtxConfigureMfp.                                    */
    pAuthCtx->authMod &= (0xffu^WDRV_WINC_AUTH_MOD_MFP_REQ);
    pAuthCtx->authMod &= (0xffu^WDRV_WINC_AUTH_MOD_MFP_OFF);

    /* Copy the key and zero out unused parts of the buffer. */
    pAuthCtx->authInfo.personal.size = size;
    (void)memset( pAuthCtx->authInfo.personal.password,
            0,
            sizeof(pAuthCtx->authInfo.personal.password));
    (void)memcpy(pAuthCtx->authInfo.personal.password, pPassword, size);

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AuthCtxConfigureMfp
    (
        WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        WDRV_WINC_AUTH_MFP_CONFIG config
    )

  Summary:
    Set the Management Frame Protection configuration of an authentication
      context.

  Description:
    The authentication context is updated with the Management Frame Protection
      configuration specified in the config parameter.

  Remarks:
    See wdrv_winc_authctx.h for usage information.
*/
WDRV_WINC_STATUS WDRV_WINC_AuthCtxConfigureMfp
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    WDRV_WINC_AUTH_MFP_CONFIG config
)
{
    WDRV_WINC_STATUS status = WDRV_WINC_STATUS_OK;

    /* Ensure authentication context is valid. */
    if (NULL == pAuthCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Update the authentication context. */
    switch (config)
    {
        case WDRV_WINC_AUTH_MFP_ENABLED:
        {
            pAuthCtx->authMod &= (0xffu^WDRV_WINC_AUTH_MOD_MFP_REQ);
            pAuthCtx->authMod &= (0xffu^WDRV_WINC_AUTH_MOD_MFP_OFF);
            break;
        }

        case WDRV_WINC_AUTH_MFP_REQUIRED:
        {
            pAuthCtx->authMod |= WDRV_WINC_AUTH_MOD_MFP_REQ;
            pAuthCtx->authMod &= (0xffu^WDRV_WINC_AUTH_MOD_MFP_OFF);
            break;
        }

        case WDRV_WINC_AUTH_MFP_DISABLED:
        {
            pAuthCtx->authMod &= (0xffu^WDRV_WINC_AUTH_MOD_MFP_REQ);
            pAuthCtx->authMod |= WDRV_WINC_AUTH_MOD_MFP_OFF;
            break;
        }

        default:
        {
            status = WDRV_WINC_STATUS_INVALID_ARG;
            break;
        }
    }

    return status;
}

