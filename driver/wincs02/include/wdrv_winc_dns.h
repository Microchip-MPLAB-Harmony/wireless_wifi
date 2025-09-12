/*******************************************************************************
  WINC Wireless Driver DNS Client Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_dns.h

  Summary:
    WINC wireless driver DNS client interface.

  Description:
    Provides an interface to configure and use the DNS client on the WINC.
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

#ifndef WDRV_WINC_DNS_H
#define WDRV_WINC_DNS_H

#ifndef WDRV_WINC_MOD_DISABLE_DNS

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver DNS Client Data Types
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
// Section: WINC Driver DNS Client Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_DNSProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        const WINC_DEV_EVENT_RSP_ELEMS *const pElems
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
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
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
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

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
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

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
        uint8_t recordType,
        const char *pDomainName,
        uint16_t timeoutMs,
        const WDRV_WINC_DNS_RESOLVE_CALLBACK pfDNSResolveResponseCB
    )

  Summary:
    Resolves the IP address of a specific domain.

  Description:
    Resolves the IP address of a specific domain.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle                 - Client handle obtained by a call to WDRV_WINC_Open.
    recordType             - The type of IP address.
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
    uint8_t recordType,
    const char *pDomainName,
    uint16_t timeoutMs,
    const WDRV_WINC_DNS_RESOLVE_CALLBACK pfDNSResolveResponseCB
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_MOD_DISABLE_DNS */
#endif /* WDRV_WINC_DNS_H */
