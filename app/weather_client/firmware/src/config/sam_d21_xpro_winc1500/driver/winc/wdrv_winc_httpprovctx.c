/*******************************************************************************
  WINC Driver HTTP Provision Context Implementation

  File Name:
    wdrv_winc_httpprovctx.c

  Summary:
    WINC wireless driver HTTP provisioning context implementation.

  Description:
    This interface manages the HTTP provisioning context which is used to
      define the behaviour of the HTTP provisioning provided by Soft-AP mode.
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

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_winc_common.h"
#include "wdrv_winc_httpprovctx.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver HTTP Provisioning Context Implementation
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

  Remarks:
    See wdrv_winc_httpprovctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetDefaults
(
    WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx
)
{
    /* Ensure provisioning context is valid. */
    if (NULL == pHTTPProvCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set the context to no domain, with wild card URL mapping and no callback. */
    pHTTPProvCtx->wildcardURL         = true;
    pHTTPProvCtx->domainName[0]       = '\0';
    pHTTPProvCtx->pfProvConnectInfoCB = NULL;

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_httpprovctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetDomainName
(
    WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
    const char *const pname
)
{
    size_t nameLength;

    /* Ensure provisioning context is valid. */
    if (NULL == pHTTPProvCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Check length of domain name. */
    nameLength = strlen(pname);

    if (nameLength > 64)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Copy domain name into context ensuring unused space is zeroed. */
    memset(&pHTTPProvCtx->domainName, 0, 64+1);
    memcpy(pHTTPProvCtx->domainName, pname, nameLength);

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_httpprovctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetURLWildcard
(
    WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
    bool wildcardURL
)
{
    /* Ensure provisioning context is valid. */
    if (NULL == pHTTPProvCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set wild card flag. */
    pHTTPProvCtx->wildcardURL = wildcardURL;

    return WDRV_WINC_STATUS_OK;
}

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

  Remarks:
    See wdrv_winc_httpprovctx.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetConnectCallback
(
    WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
    const WDRV_WINC_HTTPPROV_INFO_CALLBACK pfProvConnectInfoCB
)
{
    /* Ensure provisioning context is valid. */
    if (NULL == pHTTPProvCtx)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Set the provision callback. */
    pHTTPProvCtx->pfProvConnectInfoCB = pfProvConnectInfoCB;

    return WDRV_WINC_STATUS_OK;
}
