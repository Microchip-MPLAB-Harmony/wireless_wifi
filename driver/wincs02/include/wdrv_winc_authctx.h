/*******************************************************************************
  WINC Driver Authentication Context Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_authctx.h

  Summary:
    WINC wireless driver authentication context header file.

  Description:
    This interface manages the authentication contexts which 'wrap' the state
      associated with authentication schemes.
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

#ifndef WDRV_WINC_AUTHCTX_H
#define WDRV_WINC_AUTHCTX_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Data Types
// *****************************************************************************
// *****************************************************************************

/* Bits for WID_11I_SETTINGS and dot11iInfo field of scan results. */
typedef enum
{
    WDRV_WINC_PRIVACY            = 0x0001,  // Not 11i, but included here for convenience
    WDRV_WINC_SKEY               = 0x0002,  // Not 11i, but included here for convenience
    WDRV_WINC_11I_WEP            = 0x0004,
    WDRV_WINC_11I_WEP104         = 0x0008,
    WDRV_WINC_11I_WPAIE          = 0x0010,
    WDRV_WINC_11I_RSNE           = 0x0020,
    WDRV_WINC_11I_CCMP128        = 0x0040,
    WDRV_WINC_11I_TKIP           = 0x0080,
    WDRV_WINC_11I_BIPCMAC128     = 0x0100,
    WDRV_WINC_11I_MFP_REQUIRED   = 0x0200,
    WDRV_WINC_11I_1X             = 0x0400,
    WDRV_WINC_11I_PSK            = 0x0800,
    WDRV_WINC_11I_SAE            = 0x1000,
    WDRV_WINC_11I_TD             = 0x2000,
    WDRV_WINC_AP                 = 0x8000,   // Indicates whether the settings are intended for STA or AP mode
    WDRV_WINC_RSNA_MASK          = 0x3FF0,   // Mask of bits linked to RSNA's
} WDRV_WINC_11I_MASK;

// *****************************************************************************
/*  Authentication Types

  Summary:
    List of possible authentication types.

  Description:
    This type defines the possible authentication types that can be requested
    in AP mode or STA mode.

  Remarks:
    None.
*/

typedef enum
{
    /* This type may be used in helper function
     * WDRV_WINC_AuthCtxSetPersonal, in which case it is translated into
     * the best practice auth type. Other uses of this type are invalid. */
    WDRV_WINC_AUTH_TYPE_DEFAULT = -1,

    /* No encryption. */
    WDRV_WINC_AUTH_TYPE_OPEN = 0,

    /* WEP encryption. */
    WDRV_WINC_AUTH_TYPE_WEP = 1,

    /* WPA2 mixed mode (AP) / compatibility mode (STA) with PSK.
     * (As an AP GTK is TKIP, as a STA GTK is chosen by AP;
     * PTK can be CCMP or TKIP) */
    WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL = 2,

    /* WPA2-only authentication with PSK. (PTK and GTK are both CCMP).       */
    /* Note that a WPA2-only STA will not connect to a WPA2 mixed mode AP.   */
    WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL = 3,

#define WDRV_WINC_WPA3_PERSONAL_SUPPORT
#ifdef WDRV_WINC_WPA3_PERSONAL_SUPPORT
    /* WPA3 SAE transition mode. (CCMP, IGTK can be BIP or none) */
    WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL = 4,

    /* WPA3 SAE authentication. (CCMP, IGTK is BIP) */
    WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL = 5,
#endif

#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
    /* WPA2/WPA enterprise mixed mode authentication type */
    WDRV_WINC_AUTH_TYPE_WPAWPA2_ENTERPRISE = 6,

    /* WPA2 enterprise authentication type */
    WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE = 7,

    /* WPA3 enterprise transition mode authentication type */
    WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE = 8,

    /* WPA3 enterprise authentication type */
    WDRV_WINC_AUTH_TYPE_WPA3_ENTERPRISE = 9,
#endif

    /* Authentication types with this value or above are not recognised. */
    WDRV_WINC_AUTH_TYPE_MAX
} WDRV_WINC_AUTH_TYPE;

// *****************************************************************************
/*  Authentication Modifiers

  Summary:
    List of possible authentication modifiers.

  Description:
    This type defines the possible modifications that can be made to the
    authentication types in WDRV_WINC_AUTH_TYPE.

  Remarks:
    Not all modifiers are relevant to all auth types. When an auth context is
    applied, modifiers are ignored if they are not relevant to the auth type.
*/

typedef enum
{
    /* No modifiers set; the default behaviour for each auth type applies. */
    WDRV_WINC_AUTH_MOD_NONE         = 0,
    /* If set, this modifier causes management frame protection to be required.
     * It is relevant to the following auth types:
     *      WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE
     *      WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE
     * This modifier can be set/cleared by WDRV_WINC_AuthCtxConfigureMfp. */
    WDRV_WINC_AUTH_MOD_MFP_REQ      = 0x01,
    /* If set, this modifier causes management frame protection to be disabled.
     * It is relevant to the following auth types:
     *      WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPAWPA2_ENTERPRISE
     *      WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE
     * This modifier is ignored if WDRV_WINC_AUTH_MOD_MFP_REQ is set.
     * This modifier can be set/cleared by WDRV_WINC_AuthCtxConfigureMfp. */
    WDRV_WINC_AUTH_MOD_MFP_OFF      = 0x02,
    /* If set, this modifier allows the device, as supplicant, to attempt
     * Shared Key authentication in the event that Open System authentication
     * is rejected by the authenticator.
     * It is relevant to the following auth types:
     *      WDRV_WINC_AUTH_TYPE_WEP
     * This modifier can be set/cleared by WDRV_WINC_AuthCtxSharedKey. */
    WDRV_WINC_AUTH_MOD_SHARED_KEY   = 0x04,
    /* If set, this modifier causes the device, as authenticator, to include a
     * transition disable element. This instructs peer STAs to disable any
     * transition mode protocols.
     * It is relevant to the following auth types:
     *      WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL
     * This modifier can be set/cleared by
     *      WDRV_WINC_AuthCtxApTransitionDisable. */
    WDRV_WINC_AUTH_MOD_AP_TD        = 0x08,
    /* If set, this modifier causes the device, as supplicant, to disable any
     * transition mode protocols.
     * It is relevant to the following auth types:
     *      WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2WPA3_ENTERPRISE
     *      WDRV_WINC_AUTH_TYPE_WPA2_ENTERPRISE (so long as MFP is enabled)
     *      WDRV_WINC_AUTH_TYPE_WPAWPA2_ENTERPRISE (so long as MFP is enabled)
     * This modifier can be set by WDRV_WINC_AuthCtxStaTransitionDisable. */
    WDRV_WINC_AUTH_MOD_STA_TD       = 0x10,
} WDRV_WINC_AUTH_MOD_MASK;

// *****************************************************************************
/*  Authentication Context

  Summary:
    Context structure containing information about authentication types.

  Description:
    The context contains the type of authentication as well as any state
      information.

  Remarks:
    None.
*/

typedef struct
{
    /* Authentication type of context. */
    WDRV_WINC_AUTH_TYPE authType;

    /* Authentication modifiers. */
    WDRV_WINC_AUTH_MOD_MASK authMod;

    /* Union of data structures for each authentication type. */
    union
    {
        /* WEP authentication state. */
        struct
        {
            /* The WEP key index in the range 1-4. */
            uint8_t idx;
            /* The WEP key size is 10 for WEP_40 and 26 for WEP_104. */
            uint8_t size;
            /* The WEP key. */
            uint8_t key[WDRV_WINC_WEP_104_KEY_STRING_SIZE+1];
        } WEP;

        /* WPA-Personal authentication state. */
        struct
        {
            /* The size of the password or PSK, in characters/bytes          */
            /* Size must be in the range 8 to 63 for a WPA(2) password.      */
            /* Size must be 64 for a WPA(2) PSK.                             */
            uint8_t size;
            /* The password or PSK. */
            uint8_t password[WDRV_WINC_PSK_LEN];
        } personal;
#ifdef WDRV_WINC_ENTERPRISE_SUPPORT
         /* 802.1x authentication state. */
        struct
        {
            /* EAP method configured for 802.1X authentication */
            WDRV_WINC_AUTH_1X_METHOD   auth1xMethod;
            struct
            {
                /* Specifies the EAP identity name - [Domain][UserName] or [Username][Domain] */
                char identity[WDRV_WINC_ENT_AUTH_IDENTITY_LEN_MAX+1];
                /* WOLFSSL_CTX handle */
                WDRV_WINC_TLS_CONTEXT_HANDLE tlsCtxHandle;
                /* Server domain name against which either server certificate's subject alternative
                 * name(SAN) or common name(CN) shall be matched for successful enterprise connection */
                char serverDomainName[WDRV_WINC_ENT_AUTH_SERVER_DOMAIN_LEN_MAX + 1];
            } phase1;
            struct
            {
                union
                {
                    struct
                    {
                        /* username for mschapv2 authentication */
                        char username[WDRV_WINC_ENT_AUTH_USERNAME_LEN_MAX + 1];
                        /* password for mschapv2 authentication */
                        char password[WDRV_WINC_ENT_AUTH_PASSWORD_LEN_MAX + 1];
                    } mschapv2;
                } credentials;
            } phase2;
        } enterprise;
#endif
    } authInfo;
} WDRV_WINC_AUTH_CONTEXT;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Routines
// *****************************************************************************
// *****************************************************************************

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

  Precondition:
    None.

  Parameters:
    authType - Auth type to convert.
    authMod  - Modifiers to the authentication type.

  Returns:
    11i info mapped from auth type and modifiers.

  Remarks:
    None.
*/

WDRV_WINC_11I_MASK WDRV_WINC_AuthGet11iMask
(
    WDRV_WINC_AUTH_TYPE authType,
    WDRV_WINC_AUTH_MOD_MASK authMod
);

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_AuthCtxIsValid(const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx)

  Summary:
    Tests if an authentication context is valid.

  Description:
    Tests the elements of the authentication context to judge if their values are legal.

  Precondition:
    None.

  Parameters:
    pAuthCtx  - Pointer to an authentication context.

  Returns:
    true or false indicating if context is valid.

  Remarks:
    None.

*/

bool WDRV_WINC_AuthCtxIsValid(const WDRV_WINC_AUTH_CONTEXT *const pAuthCtx);

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

  Precondition:
    None.

  Parameters:
    pAuthCtx - Pointer to an authentication context.

  Returns:
    WDRV_WINC_STATUS_OK          - The context has been configured.
    WDRV_WINC_STATUS_INVALID_ARG - The parameters were incorrect.

  Remarks:
    A default context is not valid until it is configured.

*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetDefaults
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx
);

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

  Precondition:
    None.

  Parameters:
    pAuthCtx - Pointer to an authentication context.

  Returns:
    WDRV_WINC_STATUS_OK             - The context has been configured.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetOpen
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx
);

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

  Precondition:
    None.

  Parameters:
    pAuthCtx - Pointer to an authentication context.
    idx      - WEP index.
    pKey     - Pointer to WEP key.
    size     - Size of WEP key.

  Returns:
    WDRV_WINC_STATUS_OK             - The context has been configured.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetWEP
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    uint8_t idx,
    uint8_t *const pKey,
    uint8_t size
);

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

  Precondition:
    None.

  Parameters:
    pAuthCtx    - Pointer to an authentication context.
    pPassword   - Pointer to password (or 64-character PSK).
    size        - Size of password (or 64 for PSK).
    authType    - Authentication type (or WDRV_WINC_AUTH_TYPE_DEFAULT).

  Returns:
    WDRV_WINC_STATUS_OK             - The context has been configured.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxSetPersonal
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    uint8_t *const pPassword,
    uint8_t size,
    WDRV_WINC_AUTH_TYPE authType
);

#endif /* WDRV_WINC_AUTHCTX_H */
