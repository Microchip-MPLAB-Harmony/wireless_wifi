/*******************************************************************************
  WINC Driver HTTP Provision Context Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_httpprovctx.h

  Summary:
    WINC wireless driver HTTP provisioning context header file.

  Description:
    This interface manages the HTTP provisioning context which is used to
      define the behaviour of the HTTP provisioning provided by Soft-AP mode.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

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

#ifndef _WDRV_WINC_HTTPPROVCTX_H
#define _WDRV_WINC_HTTPPROVCTX_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"
#include "wdrv_winc_authctx.h"
#include "wdrv_winc_bssctx.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver HTTP Provisioning Context Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  Provisioning Callback

  Summary:
    HTTP provisioning callback for connect information.

  Description:
    This callback is used to signal the desired SSID and authentication
      contexts selected via provisioning.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    pSSID       - Pointer to selected SSID.
    pAuthCtx    - Pointer to selected authentication context.
    provisioned - Flag indicating provision success.

  Returns:
    None.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_HTTPPROV_INFO_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_SSID *pSSID,
    WDRV_WINC_AUTH_CONTEXT *pAuthCtx,
    bool provisioned
);

// *****************************************************************************
/*  HTTP Provisioning Context

  Summary:
    HTTP provisioning context definition.

  Description:
 This structure contains the provisioning information needed when creating
 a Soft-AP via WDRV_WINC_APStart.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if any URL is mapped to the provisioning server. */
    bool wildcardURL;

    /* Domain name of provisioning server. */
    char domainName[64+1];

    /* Pointer to callback to signal provisioning information. */
    WDRV_WINC_HTTPPROV_INFO_CALLBACK pfProvConnectInfoCB;
} WDRV_WINC_HTTP_PROV_CONTEXT;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver HTTP Provisioning Context Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetDefaults
    (
        WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx
    )

  Summary:
    Initialize provisioning store.

  Description:
    Ensure the provisioning store is initialized to appropriate defaults.

  Precondition:
    None.

  Parameters:
    pHTTPProvCtx - Pointer to provisioning store structure.

  Returns:
    WDRV_WINC_STATUS_OK             - Store initialized.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetDefaults
(
    WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetDomainName
    (
        WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
        const char *const pname
    )

  Summary:
    Define the DNS domain name for the provisioning server.

  Description:
    This defines the DNS domain name associated with the Soft-AP instance web server.

  Precondition:
    WDRV_WINC_HTTPProvCtxSetDefaults must have been called.

  Parameters:
    pHTTPProvCtx - Pointer to provisioning store structure.
    pname        - Pointer to domain name string.

  Returns:
    WDRV_WINC_STATUS_OK             - The domain name is set.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetDomainName
(
    WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
    const char *const pname
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetURLWildcard
    (
        WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
        bool wildcardURL
    )

  Summary:
    Define the use of a wildcard URL check.

  Description:
    Indicates if the Soft-AP should map all DNS domain names to the server to
      capture the clients device.

  Precondition:
    WDRV_WINC_HTTPProvCtxSetDefaults must have been called.

  Parameters:
    pHTTPProvCtx - Pointer to provisioning store structure.

  Returns:
    WDRV_WINC_STATUS_OK             - The flag is set.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetURLWildcard
(
    WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
    bool wildcardURL
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetConnectCallback
    (
        WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
        const WDRV_WINC_HTTPPROV_INFO_CALLBACK pfProvConnectInfoCB
    )

  Summary:
    Defines the connection information callback.

  Description:
    Associates the function provided with the provisioning context. The callback
      will be used once the client has provided credentials to be used.

  Precondition:
    WDRV_WINC_HTTPProvCtxSetDefaults must have been called.

  Parameters:
    pHTTPProvCtx        - Pointer to provisioning store structure.
    pfProvConnectInfoCB - Function pointer for callback function.

  Returns:
    WDRV_WINC_STATUS_OK             - The callback is set.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetConnectCallback
(
    WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
    const WDRV_WINC_HTTPPROV_INFO_CALLBACK pfProvConnectInfoCB
);

#endif /* _WDRV_WINC_HTTPPROVCTX_H */
