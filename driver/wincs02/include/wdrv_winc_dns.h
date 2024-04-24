/*******************************************************************************
  WINC Driver DNS Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_dns.h

  Summary:
    WINC wireless driver DNS header file.

  Description:
    Provides an interface to configure and use the DNS client on the WINC.
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

#ifndef WDRV_WINC_DNS_H
#define WDRV_WINC_DNS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver DNS Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* DNS Status Type.

  Summary:
    Status type for DNS operations.

  Description:
    Type describing possible status results for DNS operations.

  Remarks:
    None.
*/

typedef enum
{
    WDRV_WINC_DNS_STATUS_OK         = WINC_STATUS_OK,
    WDRV_WINC_DNS_STATUS_ERROR      = WINC_STATUS_DNS_ERROR,
    WDRV_WINC_DNS_STATUS_TIME_OUT   = WINC_STATUS_DNS_TIMEOUT
} WDRV_WINC_DNS_STATUS_TYPE;

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_DNS_RESOLVE_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_DNS_STATUS_TYPE status,
        uint8_t recordType,
        const char *pDomainName,
        WDRV_WINC_IP_MULTI_TYPE_ADDRESS *pIPAddr
    )

  Summary:
    Callback returning the DNS resolve.

  Description:
    Callback returning the DNS resolve.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_DNSResolveDomain must be called to register callback.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    status      - Operation status.
    recordType  - Record type found.
    pDomainName - Pointer to domain name being resolved.
    pIPAddr     - Pointer to IP address found.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_DNS_RESOLVE_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_DNS_STATUS_TYPE status,
    uint8_t recordType,
    const char *pDomainName,
    WDRV_WINC_IP_MULTI_TYPE_ADDRESS *pIPAddr
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver DNS Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_DNSProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        WINC_DEV_EVENT_RSP_ELEMS *pElems
    )

  Summary:
    AEC process callback.

  Description:
    Callback will be called to process any AEC messages received.

  Precondition:
    WINC_DevAECCallbackRegister must be called to register the callback.

  Parameters:
    context   - Pointer to user context supplied when callback was registered.
    devHandle - WINC device handle.
    pElems    - Pointer to element structure.

  Returns:
    None.

  Remarks:
    Callback should call WINC_CmdReadParamElem to extract elements.

*/

void WDRV_WINC_DNSProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_DEV_EVENT_RSP_ELEMS *pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSAutoSet(DRV_HANDLE handle, bool enabled)

  Summary:
    Set the enabled state of the DNS server auto-assignment.

  Description:
    Configures the system to use the automatic DNS assignment, for example via
    DHCP.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    enabled - Flag to enable DNS server auto-assignment.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSAutoSet(DRV_HANDLE handle, bool enabled);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSServerAddressSet
    (
        DRV_HANDLE handle,
        uint8_t numServers,
        WDRV_WINC_IP_MULTI_TYPE_ADDRESS *pServerAddr
    )

  Summary:
    Sets a list of DNS servers to be used in manual DNS server configuration.

  Description:
    Configures the system to use the DNS servers specified.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    numServers  - Number of servers to set.
    pServerAddr - Pointer to list of server addresses and types.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSServerAddressSet
(
    DRV_HANDLE handle,
    uint8_t numServers,
    WDRV_WINC_IP_MULTI_TYPE_ADDRESS *pServerAddr
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSResolveDomain
    (
        DRV_HANDLE handle,
        uint8_t type,
        const char *pDomainName,
        uint16_t timeoutMs,
        const WDRV_WINC_DNS_RESOLVE_CALLBACK pfDNSResolveResponseCB
    )

  Summary:
    Resolves the IP address of a specific domain.

  Description:
    Resolves the IP address of a specific domain.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle                 - Client handle obtained by a call to WDRV_WINC_Open.
    type                   - The type of IP address.
    pDomainName            - Pointer to domain name to resolve.
    timeoutMs              - Timeout (in milliseconds).
    pfDNSResolveResponseCB - Pointer to DNS resolve response callback.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSResolveDomain
(
    DRV_HANDLE handle,
    uint8_t type,
    const char *pDomainName,
    uint16_t timeoutMs,
    const WDRV_WINC_DNS_RESOLVE_CALLBACK pfDNSResolveResponseCB
);

#endif /* WDRV_WINC_DNS_H */
