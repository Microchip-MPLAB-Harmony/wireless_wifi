/*******************************************************************************
  WINC Wireless Driver Authentication Context Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_authctx.h

  Summary:
    WINC wireless driver authentication context interface.

  Description:
    This interface manages the authentication contexts which 'wrap' the state
    associated with authentication schemes.
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

#ifndef WDRV_WINC_AUTHCTX_H
#define WDRV_WINC_AUTHCTX_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Defines
// *****************************************************************************
// *****************************************************************************

/* No modifiers set; the default behaviour for each auth type applies. */
#define WDRV_WINC_AUTH_MOD_NONE         0x00U
/* If set, this modifier causes management frame protection to be required.
 * It is relevant to the following auth types:
 *      WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL
 *      WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL
 * This modifier can be set/cleared by WDRV_WINC_AuthCtxConfigureMfp. */
#define WDRV_WINC_AUTH_MOD_MFP_REQ      0x01U
/* If set, this modifier causes management frame protection to be disabled.
 * It is relevant to the following auth types:
 *      WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL
 *      WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL
 * This modifier is ignored if WDRV_WINC_AUTH_MOD_MFP_REQ is set.
 * This modifier can be set/cleared by WDRV_WINC_AuthCtxConfigureMfp. */
#define WDRV_WINC_AUTH_MOD_MFP_OFF      0x02U

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Data Types
// *****************************************************************************
// *****************************************************************************

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

    /* WPA2 mixed mode (AP) / compatibility mode (STA) with PSK.
     * (As an AP GTK is TKIP, as a STA GTK is chosen by AP;
     * PTK can be CCMP or TKIP) */
    WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL = 2,

    /* WPA2-only authentication with PSK. (PTK and GTK are both CCMP).       */
    /* Note that a WPA2-only STA will not connect to a WPA2 mixed mode AP.   */
    WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL = 3,

    /* WPA3 SAE transition mode. (CCMP, IGTK can be BIP or none) */
    WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL = 4,

    /* WPA3 SAE authentication. (CCMP, IGTK is BIP) */
    WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL = 5,

    /* Authentication types with this value or above are not recognised. */
    WDRV_WINC_AUTH_TYPE_MAX
} WDRV_WINC_AUTH_TYPE;

// *****************************************************************************
/*  MFP Configurations

  Summary:
    List of possible configurations for Management Frame Protection.

  Description:
    This type defines the possible configurations that can be specified in
    WDRV_WINC_AuthCtxConfigureMfp.

  Remarks:
    Not all MFP configurations are compatible with all auth types. When an auth
    context is applied, the MFP configuration is ignored if it is not
    compatible with the auth type.
*/

typedef enum
{
    /* Management Frame Protection enabled but not required.
     * This is the default configuration for the following auth types:
     *      WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL
     * This configuration is not compatible with other auth types. */
    WDRV_WINC_AUTH_MFP_ENABLED,
    /* Management Frame Protection required.
     * This is an optional configuration for the following auth types:
     *      WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL
     * This configuration is not compatible with other auth types. */
    WDRV_WINC_AUTH_MFP_REQUIRED,
    /* Management Frame Protection disabled.
     * This is an optional configuration for the following auth types:
     *      WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL
     *      WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL
     * This configuration is not compatible with other auth types. */
    WDRV_WINC_AUTH_MFP_DISABLED,
} WDRV_WINC_AUTH_MFP_CONFIG;

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
    uint8_t authMod;

    /* Union of data structures for each authentication type. */
    union
    {
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
    } authInfo;
} WDRV_WINC_AUTH_CONTEXT;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Authentication Context Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

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

  Precondition:
    None.

  Parameters:
    pAuthCtx    - Pointer to an authentication context.
    config      - The required Management Frame Protection configuration.

  Returns:
    WDRV_WINC_STATUS_OK             - The context has been updated.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    Not all MFP configurations are compatible with all auth types. When an auth
      context is applied, the MFP configuration is ignored if it is not
      compatible with the auth type.
    The MFP configuration is initialised to WDRV_WINC_AUTH_MFP_ENABLED by
      WDRV_WINC_AuthCtxSetPersonal.
*/

WDRV_WINC_STATUS WDRV_WINC_AuthCtxConfigureMfp
(
    WDRV_WINC_AUTH_CONTEXT *const pAuthCtx,
    WDRV_WINC_AUTH_MFP_CONFIG config
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_AUTHCTX_H */
