/*******************************************************************************
  WINC Wireless Driver Authentication Context Source File

  File Name:
    wdrv_winc_authctx.c

  Summary:
    WINC wireless driver authentication context implementation.

  Description:
    This interface manages the authentication contexts which 'wrap' the state
    associated with authentication schemes.
 *******************************************************************************/

/*
Copyright (C) 2024-26 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#include "wdrv_winc.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Defines
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
// Section: WINC Driver Authentication Context Local Data
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
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
    /* WDRV_WINC_AUTH_TYPE_WPAWPA2_ENTERPRISE */
    DRV_WINC_PRIVACY
    | DRV_WINC_11I_WPAIE | DRV_WINC_11I_TKIP
    | DRV_WINC_11I_RSNE  | DRV_WINC_11I_CCMP128
    | DRV_WINC_11I_BIPCMAC128
    | DRV_WINC_11I_1X,
    /* WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE */
    DRV_WINC_PRIVACY
    | DRV_WINC_11I_RSNE | DRV_WINC_11I_CCMP128
    | DRV_WINC_11I_BIPCMAC128
    | DRV_WINC_11I_1X,
    /* WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE */
    DRV_WINC_PRIVACY
    | DRV_WINC_11I_RSNE | DRV_WINC_11I_CCMP128
    | DRV_WINC_11I_BIPCMAC128
    | DRV_WINC_11I_1X,
    /* WDRV_WINC_AUTH_TYPE_WPA3_ENTERPRISE */
    DRV_WINC_PRIVACY
    | DRV_WINC_11I_RSNE | DRV_WINC_11I_CCMP128
    | DRV_WINC_11I_BIPCMAC128 | DRV_WINC_11I_MFP_REQUIRED
    | DRV_WINC_11I_1X,
#endif
};

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Internal Implementation
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
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
            ||  (WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE == authType)
            ||  (WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE == authType)
#endif
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
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
            ||  (WDRV_WINC_AUTH_TYPE_WPAWPA2_ENTERPRISE == authType)
            ||  (WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE == authType)
#endif
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

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Implementation
// *****************************************************************************
// *****************************************************************************

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

#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
        /* Check Enterprise authentication. */
        case WDRV_WINC_AUTH_TYPE_WPAWPA2_ENTERPRISE:
        case WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE:
        case WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE:
        case WDRV_WINC_AUTH_TYPE_WPA3_ENTERPRISE:
        {
            int size;

            /* Validate identity. */
            size = strlen(pAuthCtx->authInfo.enterprise.identity);
            if ((0 == size) || (size > WDRV_WINC_ENT_AUTH_IDENTITY_LEN_MAX))
            {
                return false;
            }

            /* Validate tunnel. */
            switch (pAuthCtx->authInfo.enterprise.tunnel.method)
            {
                case WDRV_WINC_AUTH_1X_TUNNEL_METHOD_ANY:
                case WDRV_WINC_AUTH_1X_TUNNEL_METHOD_PEAPV0:
                case WDRV_WINC_AUTH_1X_TUNNEL_METHOD_PEAPV1:
                {
                    /* Validate identity inside tunnel. */
                    size = strlen(pAuthCtx->authInfo.enterprise.tunnel.identity);
                    if ((0 == size) || (size > WDRV_WINC_ENT_AUTH_IDENTITY_LEN_MAX))
                    {
                        return false;
                    }
                    break;
                }

                case WDRV_WINC_AUTH_1X_TUNNEL_METHOD_NONE:
                case WDRV_WINC_AUTH_1X_TUNNEL_METHOD_TTLSV0:
                {
                    break;
                }

                default:
                {
                    /* No other tunnel types supported. */
                    return false;
                }
            }

            /* Validate credentials. */
            switch (pAuthCtx->authInfo.enterprise.credentials.type)
            {
                case WDRV_WINC_AUTH_1X_CREDENTIAL_TYPE_TLS:
                {
                    /* TTLSv0/TLS is not supported. */
                    if (WDRV_WINC_AUTH_1X_TUNNEL_METHOD_TTLSV0 == pAuthCtx->authInfo.enterprise.tunnel.method)
                    {
                        return false;
                    }

                    break;
                }

                case WDRV_WINC_AUTH_1X_CREDENTIAL_TYPE_MSCHAPV2:
                {
                    /* MSCHAPv2 credentials must be tunnelled. */
                    if (WDRV_WINC_AUTH_1X_TUNNEL_METHOD_NONE == pAuthCtx->authInfo.enterprise.tunnel.method)
                    {
                        return false;
                    }

                    /* Validate username. */
                    size = strlen(pAuthCtx->authInfo.enterprise.credentials.mschapv2.username);
                    if ((0 == size) || (size > WDRV_WINC_ENT_AUTH_MSCHAPV2_USERNAME_LEN_MAX))
                    {
                        return false;
                    }

                    /* Validate password. */
                    size = strlen(pAuthCtx->authInfo.enterprise.credentials.mschapv2.password);
                    if ((0 == size) || (size > WDRV_WINC_ENT_AUTH_MSCHAPV2_PASSWORD_LEN_MAX))
                    {
                        return false;
                    }
                    break;
                }

                default:
                {
                    /* No other credential types supported. */
                    return false;
                }
            }
            break;
        }
#endif

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
        const uint8_t *pPassword,
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
    const uint8_t *const pPassword,
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

#ifdef WDRV_WINC_ENTERPRISE_SUPPORT

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetEnterpriseTLS
    (
        WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        WDRV_WINC_TLS_HANDLE tlsHandle
    )

  Summary:
    Configure TLS credentials for a WPA-Enterprise authentication.

  Description:
    The authentication context enterprise.credentials structure is populated
      with TLS credentials.

  Remarks:
    See wdrv_pic32mzw_authctx.h for usage information.
*/
WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetEnterpriseTLS
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    WDRV_WINC_TLS_HANDLE tlsHandle
)
{
    uint8_t tlsIdx;

    /* Ensure authentication context is valid. */
    if (NULL == pAuthCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    tlsIdx = WDRV_WINC_TLSCtxHandleToCfgIdx(tlsHandle);

    /* Ensure the TLS handle is valid and the context is in use. */
    if ((WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle) || (tlsIdx > WDRV_WINC_TLS_CTX_NUM) || (0 == tlsIdx))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set credential type */
    pAuthCtx->authInfo.enterprise.credentials.type = WDRV_WINC_AUTH_1X_CREDENTIAL_TYPE_TLS;

    /* Copy the context handle */
    pAuthCtx->authInfo.enterprise.credentials.tls.tlsIdx = tlsIdx;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetEnterpriseMSCHAPV2
    (
        WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        const char *const pUsername,
        size_t usernameLen,
        const char *const pPassword,
        size_t passwordLen
    )

  Summary:
    Configure MSCHAPv2 credentials for a WPA-Enterprise authentication.

  Description:
    The authentication context enterprise.credentials structure is populated
      with MSCHAPv2 credentials.

  Remarks:
    See wdrv_pic32mzw_authctx.h for usage information.
*/
WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetEnterpriseMSCHAPV2
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    const char *const pUsername,
    size_t usernameLen,
    const char *const pPassword,
    size_t passwordLen
)
{
    /* Ensure authentication context is valid. */
    if (NULL == pAuthCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure username and password are provided. */
    if ((NULL == pUsername) || (NULL == pPassword))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }
    if ((0 == usernameLen) || (usernameLen > WDRV_WINC_ENT_AUTH_MSCHAPV2_USERNAME_LEN_MAX))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }
    if ((0 == passwordLen) || (passwordLen > WDRV_WINC_ENT_AUTH_MSCHAPV2_PASSWORD_LEN_MAX))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set credential type */
    pAuthCtx->authInfo.enterprise.credentials.type = WDRV_WINC_AUTH_1X_CREDENTIAL_TYPE_MSCHAPV2;

    /* Set MSCHAPv2 username */
    memset(&pAuthCtx->authInfo.enterprise.credentials.mschapv2.username, 0, WDRV_WINC_ENT_AUTH_MSCHAPV2_USERNAME_LEN_MAX+1);
    memcpy(&pAuthCtx->authInfo.enterprise.credentials.mschapv2.username, pUsername, usernameLen);

    /* Set MSCHAPv2 password */
    memset(&pAuthCtx->authInfo.enterprise.credentials.mschapv2.password, 0, WDRV_WINC_ENT_AUTH_MSCHAPV2_PASSWORD_LEN_MAX+1);
    memcpy(&pAuthCtx->authInfo.enterprise.credentials.mschapv2.password, pPassword, passwordLen);

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetEnterprise
    (
        WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
        const char *const pIdentity,
        size_t identityLen,
        WDRV_WINC_AUTH_1X_TUNNEL_METHOD tunnelMethod,
        WDRV_WINC_TLS_HANDLE tunnelTlsHandle,
        const char *const pIdentityInsideTunnel,
        size_t identityInsideTunnelLen,
        WDRV_WINC_AUTH_TYPE authType
    )

  Summary:
    Configure an authentication context for WPA Enterprise authentication.

  Description:
    The auth type and information are configured appropriately for WPA
      Enterprise authentication with the identity and tunnel parameters
      provided. The Management Frame Protection configuration is initialised to
      WDRV_WINC_AUTH_MFP_ENABLED.

  Remarks:
    See wdrv_pic32mzw_authctx.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetEnterprise
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    const char *const pIdentity,
    size_t identityLen,
    WDRV_WINC_AUTH_1X_TUNNEL_METHOD tunnelMethod,
    WDRV_WINC_TLS_HANDLE tunnelTlsHandle,
    const char *const pIdentityInsideTunnel,
    size_t identityInsideTunnelLen,
    WDRV_WINC_AUTH_TYPE authType
)
{
    uint16_t dot11iInfo;
    uint8_t tunnelTlsIdx;

    /* Ensure authentication context is valid. */
    if (NULL == pAuthCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure identity is valid. */
    if ((NULL == pIdentity) || (0 == identityLen) || (identityLen > WDRV_WINC_ENT_AUTH_IDENTITY_LEN_MAX))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (WDRV_WINC_AUTH_TYPE_DEFAULT == authType)
    {
        /* Set authentication type to WPA2/WPA3 transition mode. */
        authType = WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE;
    }

    dot11iInfo = authctxGet11iMask(authType, WDRV_WINC_AUTH_MOD_NONE);

    /* Ensure the requested auth type is valid */
    if (!(dot11iInfo & DRV_WINC_11I_1X))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure tunnel parameters are valid. */
    if (WDRV_WINC_AUTH_1X_TUNNEL_METHOD_NONE != tunnelMethod)
    {
        /* Ensure identity inside tunnel is valid. */
        if (
            (WDRV_WINC_AUTH_1X_TUNNEL_METHOD_TTLSV0 != tunnelMethod)
            &&  (
                (NULL == pIdentityInsideTunnel)
                ||  (0 == identityInsideTunnelLen)
                ||  (identityInsideTunnelLen > WDRV_WINC_ENT_AUTH_IDENTITY_LEN_MAX)
            )
        )
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        tunnelTlsIdx = WDRV_WINC_TLSCtxHandleToCfgIdx(tunnelTlsHandle);

        /* Ensure the TLS handle is valid. */
        if ((WDRV_WINC_TLS_INVALID_HANDLE == tunnelTlsHandle) || (tunnelTlsIdx > WDRV_WINC_TLS_CTX_NUM) || (0 == tunnelTlsIdx))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }

    /* Set authentication type */
    pAuthCtx->authType = authType;

    /* Initialize the MFP configuration to WDRV_WINC_AUTH_MFP_ENABLED.   */
    /* The Application may change the configuration later if desired via     */
    /* WDRV_WINC_AuthCtxConfigureMfp.                                    */
    pAuthCtx->authMod &= ~WDRV_WINC_AUTH_MOD_MFP_REQ;
    pAuthCtx->authMod &= ~WDRV_WINC_AUTH_MOD_MFP_OFF;

    /* Set identity. */
    memset(pAuthCtx->authInfo.enterprise.identity, 0, WDRV_WINC_ENT_AUTH_IDENTITY_LEN_MAX+1);
    memcpy(pAuthCtx->authInfo.enterprise.identity, pIdentity, identityLen);

    /* Set tunnel parameters. */
    pAuthCtx->authInfo.enterprise.tunnel.method = tunnelMethod;

    if (WDRV_WINC_AUTH_1X_TUNNEL_METHOD_NONE != tunnelMethod)
    {
        /* Set TLS index. */
        pAuthCtx->authInfo.enterprise.tunnel.tlsIdx = tunnelTlsIdx;

        /* Set identity inside tunnel. */
        if (WDRV_WINC_AUTH_1X_TUNNEL_METHOD_TTLSV0 != tunnelMethod)
        {
            memset(pAuthCtx->authInfo.enterprise.tunnel.identity, 0, WDRV_WINC_ENT_AUTH_IDENTITY_LEN_MAX+1);
            memcpy(pAuthCtx->authInfo.enterprise.tunnel.identity, pIdentityInsideTunnel, identityInsideTunnelLen);
        }
    }

    return WDRV_WINC_STATUS_OK;
}
#endif /* WDRV_WINC_ENTERPRISE_SUPPORT */
