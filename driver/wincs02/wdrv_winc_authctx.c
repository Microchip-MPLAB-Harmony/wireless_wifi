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
// Section: WINC Driver Authentication Context Global Data
// *****************************************************************************
// *****************************************************************************

/* Table to convert auth types to 11i info for DRV_WIFI_WID_11I_INFO. */
static const WDRV_WINC_11I_MASK mapAuthTypeTo11i[] =
{
    /* WDRV_WINC_AUTH_TYPE_OPEN */
    0,
    /* WDRV_WINC_AUTH_TYPE_WEP */
    WDRV_WINC_PRIVACY,
    /* WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL */
    WDRV_WINC_PRIVACY
        | WDRV_WINC_11I_WPAIE | WDRV_WINC_11I_TKIP
        | WDRV_WINC_11I_RSNE | WDRV_WINC_11I_CCMP128
        | WDRV_WINC_11I_BIPCMAC128
        | WDRV_WINC_11I_PSK,
    /* WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL */
    WDRV_WINC_PRIVACY
        | WDRV_WINC_11I_RSNE | WDRV_WINC_11I_CCMP128
        | WDRV_WINC_11I_BIPCMAC128
        | WDRV_WINC_11I_PSK,
#ifdef WDRV_WINC_WPA3_PERSONAL_SUPPORT
    /* WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL */
    WDRV_WINC_PRIVACY
        | WDRV_WINC_11I_RSNE | WDRV_WINC_11I_CCMP128
        | WDRV_WINC_11I_BIPCMAC128
        | WDRV_WINC_11I_PSK | WDRV_WINC_11I_SAE,
    /* WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL */
    WDRV_WINC_PRIVACY
        | WDRV_WINC_11I_RSNE | WDRV_WINC_11I_CCMP128
        | WDRV_WINC_11I_BIPCMAC128 | WDRV_WINC_11I_MFP_REQUIRED
        | WDRV_WINC_11I_SAE,
#else
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
    /* WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL */
    0,
    /* WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL */
    0,
#endif
#endif
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
    /* WDRV_WINC_AUTH_TYPE_WPAWPA2_ENTERPRISE */
    WDRV_WINC_PRIVACY
        | WDRV_WINC_11I_WPAIE
        | WDRV_WINC_11I_RSNE
        | WDRV_WINC_11I_TKIP
        | WDRV_WINC_11I_CCMP128
        | WDRV_WINC_11I_BIPCMAC128
        | WDRV_WINC_11I_1X,
    /* WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE */
    WDRV_WINC_PRIVACY
        | WDRV_WINC_11I_RSNE
        | WDRV_WINC_11I_CCMP128
        | WDRV_WINC_11I_BIPCMAC128
        | WDRV_WINC_11I_1X,
    /* WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE */
    WDRV_WINC_PRIVACY
        | WDRV_WINC_11I_RSNE
        | WDRV_WINC_11I_CCMP128
        | WDRV_WINC_11I_BIPCMAC128
        | WDRV_WINC_11I_1X,
    /* WDRV_WINC_AUTH_TYPE_WPA3_ENTERPRISE */
    WDRV_WINC_PRIVACY
        | WDRV_WINC_11I_RSNE
        | WDRV_WINC_11I_CCMP128
        | WDRV_WINC_11I_BIPCMAC128
        | WDRV_WINC_11I_MFP_REQUIRED
        | WDRV_WINC_11I_1X
        | WDRV_WINC_11I_TD,
#endif
};

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    bool authctxWEPKeyIsValid
    (
        uint8_t idx,
        uint8_t *const pKey,
        uint8_t size
    )

  Summary:
    Checks if WEP key is valid.

  Description:
    Determines if the WEP key, index and size are valid.

  Precondition:
    None.

  Parameters:
    idx  - Key index.
    pKey - Pointer to key.
    size - Size of key.

  Returns:
    true or false indicating if WEP key information is valid.

*/

static bool authctxWEPKeyIsValid
(
    uint8_t idx,
    uint8_t *const pKey,
    uint8_t size
)
{
    /* Check index. Index values 1-4 is only allowed*/
    if ((idx < 1) || (idx > 4))
    {
        return false;
    }
    /* Check key. */
    if (NULL == pKey)
    {
        return false;
    }
    /* Check size. */
    if (
            (WDRV_WINC_WEP_40_KEY_STRING_SIZE != size)
        &&  (WDRV_WINC_WEP_104_KEY_STRING_SIZE != size)
    )
    {
        return false;
    }
    return true;
}

//*******************************************************************************
/*
  Function:
    bool authctxPersonalKeyIsValid
    (
        uint8_t *const pPassword,
        uint8_t size,
        WDRV_WINC_11I_MASK dot11iInfo
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
    uint8_t *const pPassword,
    uint8_t size,
    WDRV_WINC_11I_MASK dot11iInfo
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
    if (dot11iInfo & WDRV_WINC_11I_SAE)
    {
        if (WDRV_WINC_MAX_PSK_PASSWORD_LEN < size)
        {
            return false;
        }
    }

    if (dot11iInfo & WDRV_WINC_11I_PSK)
    {
        /* Determine if this is a password or direct PSK. */
        if (WDRV_WINC_PSK_LEN == size)
        {
            /* PSK. */
            while (size--)
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
            while (size--)
            {
                char character = (char)(pPassword[size]);

                if ((0x20 > character) || (0x7e < character))
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
    WDRV_WINC_11I_MASK WDRV_WINC_AuthGet11iMask
    (
        WDRV_WINC_AUTH_TYPE authType,
        WDRV_WINC_AUTH_MOD_MASK authMod
    )

  Summary:
    Convert authentication type and modifiers to 11i info.

  Description:

  Remarks:
    See wdrv_winc_authctx.h for usage information.

*/

WDRV_WINC_11I_MASK WDRV_WINC_AuthGet11iMask
(
    WDRV_WINC_AUTH_TYPE authType,
    WDRV_WINC_AUTH_MOD_MASK authMod
)
{
    WDRV_WINC_11I_MASK dot11iInfo;

    if (authType >= WDRV_WINC_AUTH_TYPE_MAX)
    {
        return 0;
    }

    /* Convert auth type to 11i info. */
    dot11iInfo = mapAuthTypeTo11i[authType];

    /* Apply any relevant modifiers. */
    if (authMod & WDRV_WINC_AUTH_MOD_MFP_REQ)
    {
        if (
                (WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL == authType)
#ifdef WDRV_WINC_WPA3_PERSONAL_SUPPORT
            ||  (WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL == authType)
#endif
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
            ||  (WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE == authType)
            ||  (WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE == authType)
#endif
        )
        {
            dot11iInfo |= WDRV_WINC_11I_BIPCMAC128 | WDRV_WINC_11I_MFP_REQUIRED;
        }
    }
    else if (authMod & WDRV_WINC_AUTH_MOD_MFP_OFF)
    {
        if (
                (WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL == authType)
            ||  (WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL == authType)
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
            ||  (WDRV_WINC_AUTH_TYPE_WPAWPA2_ENTERPRISE == authType)
            ||  (WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE == authType)
#endif
        )
        {
            dot11iInfo &= ~WDRV_WINC_11I_BIPCMAC128;
        }
    }

    if (authMod & WDRV_WINC_AUTH_MOD_SHARED_KEY)
    {
        if (WDRV_WINC_AUTH_TYPE_WEP == authType)
        {
            dot11iInfo |= WDRV_WINC_SKEY;
        }
    }

    if (authMod & (WDRV_WINC_AUTH_MOD_AP_TD))
    {
#ifdef WDRV_WINC_WPA3_PERSONAL_SUPPORT
        if (
                (WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL == authType)
            ||  (WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL == authType)
        )
        {
            dot11iInfo |= WDRV_WINC_11I_TD;
        }
#endif
    }

    if (authMod & (WDRV_WINC_AUTH_MOD_STA_TD))
    {
#ifdef WDRV_WINC_WPA3_PERSONAL_SUPPORT
        if (dot11iInfo & WDRV_WINC_11I_SAE)
        {
            dot11iInfo |= WDRV_WINC_11I_MFP_REQUIRED;
            dot11iInfo |= WDRV_WINC_11I_TD;
            dot11iInfo &= ~WDRV_WINC_11I_PSK;
        }
#endif
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
        if (
                (dot11iInfo & WDRV_WINC_11I_1X)
            &&  (dot11iInfo & WDRV_WINC_11I_BIPCMAC128)
        )
        {
            dot11iInfo |= WDRV_WINC_11I_MFP_REQUIRED;
            dot11iInfo |= WDRV_WINC_11I_TD;
            dot11iInfo &= ~DRV_WINC_11I_TKIP;
        }
#endif
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

        /* Check WEP authentication. */
        case WDRV_WINC_AUTH_TYPE_WEP:
        {
            if (false == authctxWEPKeyIsValid(
                    pAuthCtx->authInfo.WEP.idx,
                    (uint8_t *const)(pAuthCtx->authInfo.WEP.key),
                    pAuthCtx->authInfo.WEP.size
            ))
            {
                return false;
            }
            break;
        }

        /* Check Personal authentication. */
        case WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL:
        case WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL:
#ifdef WDRV_WINC_WPA3_PERSONAL_SUPPORT
        case WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL:
        case WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL:
#endif
        {
            if (false == authctxPersonalKeyIsValid(
                    (uint8_t *const)(pAuthCtx->authInfo.personal.password),
                    pAuthCtx->authInfo.personal.size,
                    WDRV_WINC_AuthGet11iMask(pAuthCtx->authType, pAuthCtx->authMod)
            ))
            {
                return false;
            }
            break;
        }

        default:
        {
            /* Unknown authentication scheme. */
            return false;
        }
    }

    return true;
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

    memset(pAuthCtx, 0, sizeof(WDRV_WINC_AUTH_CONTEXT));

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
    WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetWEP
    (
        WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        uint8_t idx,
        uint8_t *pKey,
        uint8_t size
    )

  Summary:
    Configure an authentication context for WEP authentication.

  Description:
    The auth type and information are configured appropriately for WEP
      authentication.

  Remarks:
    See wdrv_winc_authctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetWEP
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    uint8_t idx,
    uint8_t *const pKey,
    uint8_t size
)
{
    /* Ensure authentication context is valid. */
    if ((NULL == pAuthCtx) || (NULL == pKey))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the index and key are valid. */
    if (false == authctxWEPKeyIsValid(idx, pKey, size))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set authentication type to WEP. */
    pAuthCtx->authType = WDRV_WINC_AUTH_TYPE_WEP;

    /* Initialise Shared Key authentication to disabled.                     */
    /* The Application may enable Shared Key later if desired via            */
    /* WDRV_WINC_AuthCtxSharedKey.                                       */
    pAuthCtx->authMod &= ~WDRV_WINC_AUTH_MOD_SHARED_KEY;

    /* Set key index and size. */
    pAuthCtx->authInfo.WEP.idx  = idx;
    pAuthCtx->authInfo.WEP.size = size;

    /* Copy WEP key and ensure zero terminated. */
    memcpy(&pAuthCtx->authInfo.WEP.key, pKey, size);
    pAuthCtx->authInfo.WEP.key[size] = '\0';

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
    WDRV_WINC_11I_MASK dot11iInfo;

    /* Ensure authentication context is valid. */
    if ((NULL == pAuthCtx) || (NULL == pPassword))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (WDRV_WINC_AUTH_TYPE_DEFAULT == authType)
    {
#ifdef WDRV_WINC_WPA3_PERSONAL_SUPPORT
        /* Set authentication type to WPA2/WPA3 transition mode. */
        authType = WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL;
#else
        /* Set authentication type to WPA2. */
        authType = WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL;
#endif
    }

    dot11iInfo = WDRV_WINC_AuthGet11iMask(authType, WDRV_WINC_AUTH_MOD_NONE);

    /* Ensure the requested auth type is valid for Personal authentication. */
    if (!(dot11iInfo & (WDRV_WINC_11I_PSK | WDRV_WINC_11I_SAE)))
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
    pAuthCtx->authMod &= ~WDRV_WINC_AUTH_MOD_MFP_REQ;
    pAuthCtx->authMod &= ~WDRV_WINC_AUTH_MOD_MFP_OFF;

    /* Initialise the TD configuration to not enabled.                       */
    /* The Application may change the configuration later if desired via     */
    /* WDRV_WINC_AuthCtxApTransitionDisable or                           */
    /* WDRV_WINC_AuthCtxStaTransitionDisable.                            */
    pAuthCtx->authMod &= ~WDRV_WINC_AUTH_MOD_AP_TD;
    pAuthCtx->authMod &= ~WDRV_WINC_AUTH_MOD_STA_TD;

    /* Copy the key and zero out unused parts of the buffer. */
    pAuthCtx->authInfo.personal.size = size;
    memset( pAuthCtx->authInfo.personal.password,
            0,
            sizeof(pAuthCtx->authInfo.personal.password));
    memcpy(pAuthCtx->authInfo.personal.password, pPassword, size);

    return WDRV_WINC_STATUS_OK;
}
