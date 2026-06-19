<#--
Copyright (C) 2020-2026, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
-->
/*******************************************************************************
  TLS Callback Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_pic32mzw_tls_callback.h

  Summary:
    TLS callback interface for WPA Enterprise.

  Description:
    This file provides the interface for TLS callbacks used
    for WPA/WPA2/WPA3 Enterprise security.
*******************************************************************************/

#ifndef WDRV_PIC32MZW_TLS_CB_H
#define WDRV_PIC32MZW_TLS_CB_H

#include <stdbool.h>
#include <stdint.h>
#include "wdrv_pic32mzw_tls.h"

<#if DRV_WIFI_PIC32MZW1_USE_WOLFSSL == true>
#include "wolfssl/ssl.h"

// *****************************************************************************
/*  Maximum TLS server domain name length

  Summary:
    Maximum length of the server domain name for TLS verification.

  Description:
    None.

  Remarks:
    None.

*/

#define WDRV_PIC32MZW_TLS_DOMAIN_NAME_MAX_LEN    64

// *****************************************************************************
/*  TLS Credential

  Summary:
    Structure containing a single TLS credential (certificate or private key).

  Description:
    This structure holds a pointer to credential data along with its length
    and format. Used to represent CA certificates, client certificates,
    and private keys.

  Remarks:
    Format constants are WOLFSSL_FILETYPE_PEM or WOLFSSL_FILETYPE_ASN1.

*/
typedef struct
{
    /* Pointer to credential data */
    const uint8_t *pData;

    /* Length of data in bytes */
    uint16_t length;

    /* Format (PEM or ASN1/DER) */
    uint8_t format;
} WDRV_PIC32MZW_TLS_CREDENTIAL;

// *****************************************************************************
/*  TLS Configuration

  Summary:
    Structure containing TLS certificate and key configuration for WPA Enterprise.

  Description:
    This structure holds credentials (CA certificate, client certificate,
    private key) and optional domain name required for TLS enterprise
    authentication (WPA Enterprise). Set clientAuth to true when client
    certificate authentication is required.

  Remarks:
    If clientAuth is true, clientCert and clientPrivKey must be provided.
    CA certificate is always required for server verification.

*/
typedef struct
{
    /* CA certificate for server verification */
    WDRV_PIC32MZW_TLS_CREDENTIAL caCert;

    /* Client authentication enabled. If true, clientCert and clientPrivKey must be provided */
    bool clientAuth;

    /* Client certificate for authentication */
    WDRV_PIC32MZW_TLS_CREDENTIAL clientCert;

    /* Client private key */
    WDRV_PIC32MZW_TLS_CREDENTIAL clientPrivKey;

    /* Domain name for server certificate verification (empty string if not used) */
    char domainName[WDRV_PIC32MZW_TLS_DOMAIN_NAME_MAX_LEN + 1];

#ifndef NO_SESSION_CACHE
    /* Session caching enabled */
    bool sessionCaching;
    /* Pointer to cached wolfSSL session for session resumption.
       Initialize to NULL before first use. After a successful TLS session,
       this will be updated with the cached session data.
       User is responsible for setting this pointer to NULL when
       when the cached session is no longer desired. */
    WOLFSSL_SESSION *pCachedSession;
#endif

} WDRV_PIC32MZW_TLS_CONFIG;

</#if>
//*******************************************************************************
/*
  Function:
    bool WDRV_PIC32MZW_TLS_Callback
    (
        WDRV_PIC32MZW_TLS_USER_CB_EVENT event,
        void *pParam
    )

  Summary:
    TLS enterprise callback handler for WiFi driver TLS events.

  Description:
    This function handles TLS session lifecycle events from the PIC32MZW WiFi
    driver. It manages TLS session creation, handshake, data transfer,
    key generation, and session termination for enterprise authentication.

  Precondition:
<#if DRV_WIFI_PIC32MZW1_USE_WOLFSSL == true>
    WDRV_PIC32MZW_TLS_CONFIG must be initialized with valid certificates and keys.
    wolfSSL library must be initialized.
<#else>
    TLS provider must be initialized.
</#if>

  Parameters:
    event  - The TLS event type being signaled by the driver.
    pParam - Pointer to event-specific parameter structure.

  Returns:
    true  - Event was handled successfully.
    false - Event handling failed.

  Remarks:
    This callback is registered with the WiFi driver for WPA Enterprise authentication.
    The pParam type varies based on the event type.
*/
bool WDRV_PIC32MZW_TLS_Callback(WDRV_PIC32MZW_TLS_USER_CB_EVENT event, void *pParam);

#endif /* WDRV_PIC32MZW_TLS_CB_H */
