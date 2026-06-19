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
/* DNS Protocol Transport Type.

  Summary:
    DNS protocol transport types.

  Description:
    Type describing possible DNS transport protocols.

  Remarks:
    None.
*/

typedef enum
{
    /* DNS transport over UDP datagrams (RFC1035). */
    WDRV_WINC_DNS_TRANS_TYPE_UDP,

    /* DNS transport over TCP (RFC7766). */
    WDRV_WINC_DNS_TRANS_TYPE_TCP,

    /* DNS transport over TLS (RFC7858). */
    WDRV_WINC_DNS_TRANS_TYPE_TLS,
} WDRV_WINC_DNS_TRANS_TYPE;

// *****************************************************************************
/* DNSSEC Mode.

  Summary:
    Mode of DNSSEC operations.

  Description:
    Type describing possible DNSSEC modes.

  Remarks:
    None.
*/

typedef enum
{
    /* DNSSEC awareness is turned off. */
    WDRV_WINC_DNSSEC_MODE_OFF = 0,

    /* DNSSEC awareness is turned on. checking disabled. */
    WDRV_WINC_DNSSEC_MODE_CHECK_DISABLED = 1,

    /* DNSSEC awareness is turned on, authenticated answers only. */
    WDRV_WINC_DNSSEC_MODE_AUTH_ANS_ONLY = 2
} WDRV_WINC_DNSSEC_MODE;

// *****************************************************************************
/* Multiast-DNS Preference.

  Summary:
    Preferences for MDNS servers.

  Description:
    Type describing possible server protocols for multicast DNS.

  Remarks:
    None.
*/

typedef enum
{
    /* Multicast-DNS turned off. */
    WDRV_WINC_DNS_MC_PREF_OFF = 0,

    /* IPv4 only multicast DNS servers. */
    WDRV_WINC_DNS_MC_PREF_IPV4_SRVS = 1,

    /* IPv6 only multicast DNS servers. */
    WDRV_WINC_DNS_MC_PREF_IPV6_SRVS = 2,

    /* IPv4 & IPv6 multicast DNS servers. */
    WDRV_WINC_DNS_MC_PREF_ALL_SRVS = 3
} WDRV_WINC_DNS_MC_PREF_TYPE;

// *****************************************************************************
/* DNS Resource Record Type.

  Summary:
    Union of DNS resource record structures.

  Description:
    Union of DNS structures, each specific to a resource type.

  Remarks:
    None.
*/

typedef union
{
    /* A fields, see RFC1035 for details. */
    struct
    {
        /* The address field. */
        WDRV_WINC_IPV4_ADDR address;
    } A;

    /* NS fields, see RFC1035 for details. */
    struct
    {
        /* Pointer to the nsdname field. */
        const char *pName;
    } NS;

    /* CNAME fields, see RFC1035 for details. */
    struct
    {
        /* Pointer to the cname field. */
        const char *pName;
    } CNAME;

    /* SOA fields, see RFC1035 for details. */
    struct
    {
        /* The serial field. */
        uint32_t serial;

        /* The refresh field. */
        uint32_t refresh;

        /* The retry field. */
        uint32_t retry;

        /* The expire field. */
        uint32_t expire;

        /* The minimum field. */
        uint32_t minimum;

        /* Pointer to the rname field. */
        const char *pRName;

        /* Pointer to the mname field. */
        const char *pMName;
    } SOA;

    /* PTR fields, see RFC1035 for details. */
    struct
    {
        /* Pointer to the ptrdname field. */
        const char *pDName;
    } PTR;

    /* MX fields, see RFC1035 for details. */
    struct
    {
        /* The preference field. */
        uint16_t preference;

        /* Pointer to the exchange field. */
        const char *pExchange;
    } MX;

    /* TXT fields, see RFC1035 for details. */
    struct
    {
        /* Pointer to the txt-data field. */
        const char *pData;
    } TXT;

    /* AAAA fields, see RFC3596 for details. */
    struct
    {
        /* The address field. */
        WDRV_WINC_IPV6_ADDR address;
    } AAAA;

    /* SRV fields, see RFC2782 for details. */
    struct
    {
        /* The priority field. */
        uint16_t priority;

        /* The weight field. */
        uint16_t weight;

        /* The port field. */
        uint16_t port;

        /* Pointer to the target field. */
        const char *pTarget;
    } SRV;

    /* DNSSEC - DS fields, see RFC4034 for details. */
    struct
    {
        /* The key tag field. */
        uint16_t keyTag;

        /* The algorithm field. */
        uint8_t algorithm;

        /* The digest type field. */
        uint8_t digestType;

        /* Pointer to the digest field. */
        const uint8_t *pDigest;

        /* Length of the digest field. */
        uint16_t digestLen;
    } DS;

    /* DNSSEC - RRSIG fields, see RFC4034 for details. */
    struct
    {
        /* The types covered field. */
        uint16_t typeCovered;

        /* The algorithm number field. */
        uint8_t algorithm;

        /* The labels field. */
        uint8_t labels;

        /* The original TTL field. */
        uint32_t origTTL;

        /* The signature expiration field. */
        uint32_t sigExpiration;

        /* The signature inception field. */
        uint32_t sigInception;

        /* The key tag field. */
        uint16_t keyTag;

        /* Pointer to the signer's name field. */
        const char *pSignersName;

        /* Pointer to the signature field. */
        const uint8_t *pSignature;

        /* Length of the signature field. */
        uint16_t signatureLen;
    } RRSIG;

    /* DNSSEC - NSEC fields, see RFC4034 for details. */
    struct
    {
        /* Pointer to the next domain name field. */
        const char *pNextDomain;

        /* Pointer to the type bit maps field. */
        const uint8_t  *pTypeBitMap;

        /* Length of the type bit maps field. */
        uint16_t typeBitMapLen;
    } NSEC;

    /* DNSSEC - DNSKEY fields, see RFC4034 for details. */
    struct
    {
        /* The flags field. */
        uint16_t flags;

        /* The protocol field. */
        uint8_t protocol;

        /* The algorithm field. */
        uint8_t algorithm;

        /* Pointer to the public key field. */
        const uint8_t *pPublicKey;

        /* Length of the public key field. */
        uint16_t publicKeyLen;
    } DNSKEY;

    /* Unknown type fields. */
    struct
    {
        /* Pointer to the RR data field. */
        uint8_t *pData;

        /* Length of the RR data field. */
        uint16_t dataLen;
    } RR;
} WDRV_WINC_DNS_RES_RECORD;

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
/*
  Function:
    void (*WDRV_WINC_DNS_QUERY_RESOURCE_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_DNS_STATUS_TYPE status,
        uint8_t recordType,
        const char *pQueryName,
        WDRV_WINC_DNS_RES_RECORD *pQueryRR
    )

  Summary:
    Callback returning the query resource record.

  Description:
    Callback returning the query resource record.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    status      - Operation status.
    recordType  - Record type found.
    pQueryName  - Pointer to name being queries.
    pQueryRR    - Pointer to query resource record.

  Remarks:
    WDRV_WINC_DNS_RES_RECORD is a union, for known record types they
    are returned in a type specific structure. For unknown record types a
    generic structure is used.

*/

typedef void (*WDRV_WINC_DNS_QUERY_RESOURCE_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_DNS_STATUS_TYPE status,
    uint8_t recordType,
    const char *pQueryName,
    WDRV_WINC_DNS_RES_RECORD *pQueryRR
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
    WDRV_WINC_STATUS WDRV_WINC_DNSProtoCfgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DNS_TRANS_TYPE transType,
        uint16_t port,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        uint16_t idleConnTimeout
    )

  Summary:
    Configure DNS protocol.

  Description:
    Configures the type of protocol used for DNS connection and related settings.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle          - Client handle obtained by a call to WDRV_WINC_Open.
    transType       - Transport type, UDP, TCP or TLS.
    port            - Server port to connect to, normally 53 (UDP/TCP) or 853 (TLS)
    tlsHandle       - TLS handle for DNS over TLS operation.
    idleConnTimeout - Connection idle timeout for TCP/TLS.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSProtoCfgSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DNS_TRANS_TYPE transType,
    uint16_t port,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    uint16_t idleConnTimeout
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSCacheCfgSet
    (
        DRV_HANDLE handle,
        uint16_t maxTTL
    )

  Summary:
    Configure DNS cache.

  Description:
    Configures the DNS cache operation.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    maxTTL - Maximum record TTL.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSCacheCfgSet
(
    DRV_HANDLE handle,
    uint16_t maxTTL
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSSECCfgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DNSSEC_MODE mode
    )

  Summary:
    Configure DNSSEC operation.

  Description:
    Configures the DNSSEC mode of operation.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    mode   - DNSSEC mode.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSSECCfgSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DNSSEC_MODE mode
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSMulticastCfgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DNS_MC_PREF_TYPE srvPref
    )

  Summary:
    Configure multicast DNS operation.

  Description:
    Configures the multicast DNS operation parameters.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    srvPref - Server connection preference.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSMulticastCfgSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DNS_MC_PREF_TYPE srvPref
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

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSResourceQuery
    (
        DRV_HANDLE handle,
        uint8_t type,
        const char *pQueryName,
        uint16_t timeoutMs,
        const WDRV_WINC_DNS_QUERY_RESOURCE_CALLBACK pfDNSResQueryResponseCB
    )

  Summary:
    Queries the DNS resolver.

  Description:
    Queries the DNS resolver for the named resource.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle                  - Client handle obtained by a call to WDRV_WINC_Open.
    recordType              - The type of query record.
    pQueryName              - Pointer to query name to resolve.
    timeoutMs               - Timeout (in milliseconds).
    pfDNSResQueryResponseCB - Pointer to DNS query response callback.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSResourceQuery
(
    DRV_HANDLE handle,
    uint8_t recordType,
    const char *pQueryName,
    uint16_t timeoutMs,
    const WDRV_WINC_DNS_QUERY_RESOURCE_CALLBACK pfDNSResQueryResponseCB
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_MOD_DISABLE_DNS */
#endif /* WDRV_WINC_DNS_H */
