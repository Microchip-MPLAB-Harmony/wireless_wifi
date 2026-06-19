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
  TLS Callback Source File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_pic32mzw_tls_callback.c

  Summary:
    TLS callback implementation for WPA Enterprise.

  Description:
    This file provides the implementation for the TLS callback used
    for WPA Enterprise security.
*******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"
#include "wdrv_pic32mzw_tls.h"
#include "wdrv_pic32mzw_tls_callback.h"
#include "tcpip/src/link_list.h"
<#if DRV_WIFI_PIC32MZW1_USE_WOLFSSL == true>
#include "wolfssl/ssl.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void _WDRV_PIC32MZW_SingleListRemoveAll(SINGLE_LIST *pL)

  Summary:
    Removes all nodes from a single-linked list.

  Description:
    Iterates through the list, removes each node from the head, and frees
    the memory using OSAL_Free.

  Precondition:
    None.

  Parameters:
    pL - Pointer to the single-linked list.

  Returns:
    None.

  Remarks:
    None.

*/
static void _WDRV_PIC32MZW_SingleListRemoveAll(SINGLE_LIST *pL)
{
    SGL_LIST_NODE *pN;

    while (true)
    {
        pN = TCPIP_Helper_SingleListHeadRemove(pL);

        if (NULL == pN)
        {
            break;
        }

        OSAL_Free(pN);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  TLS Session State

  Summary:
    Internal structure holding wolfSSL session state and packet queues.

  Description:
    This structure maintains the wolfSSL session pointer, session caching flag,
    and Tx/Rx packet queues for managing TLS data flow.

  Remarks:
    None.

*/
typedef struct
{
    /* Pointer to wolfSSL session */
    WOLFSSL *pSession;

#ifndef NO_SESSION_CACHE
    /* Flag indicating if session caching is enabled */
    bool sessionCaching;

    /* Pointer to the cached session pointer in WDRV_PIC32MZW_TLS_CONFIG */
    WOLFSSL_SESSION **ppCachedSession;
#endif

    /* Tx queue packet list */
    SINGLE_LIST txPacketList;

    /* Rx queue packet list */
    SINGLE_LIST rxPacketList;
} _WDRV_PIC32MZW_TLS_SESSION_STATE;

// *****************************************************************************
/*  TLS Packet

  Summary:
    Structure for queuing TLS data between driver and wolfSSL.

  Description:
    This structure holds packet data with buffer size and read offset for
    managing partial reads from the packet queue.

  Remarks:
    Uses flexible array member for data storage.

*/
typedef struct __WDRV_PIC32MZW_TLS_PACKET
{
    /* Next packet in the list (for list linking) */
    struct __WDRV_PIC32MZW_TLS_PACKET  *pNext;

    /* Total size of data in buffer */
    uint16_t bufferSize;

    /* Current read offset within data buffer */
    uint16_t offsetRead;

    /* Flexible array for packet data */
    uint8_t  data[];
} _WDRV_PIC32MZW_TLS_PACKET;

// *****************************************************************************
// *****************************************************************************
// Section: TLS I/O Callbacks
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static int _WDRV_PIC32MZW_TLS_IOCallbackRecv
    (
        WOLFSSL *pSSLCtx,
        char *pBuf,
        int buffSize,
        void *pAppCtx
    )

  Summary:
    wolfSSL receive callback for reading data from the Rx packet queue.

  Description:
    This callback is invoked by wolfSSL when it needs to read incoming TLS
    data. It reads from the internal Rx packet queue populated by the driver.

  Precondition:
    TLS session must be initialized.

  Parameters:
    pSSLCtx  - Pointer to the wolfSSL session context.
    pBuf     - Buffer to store received data.
    buffSize - Size of the receive buffer.
    pAppCtx  - Application context (_WDRV_PIC32MZW_TLS_SESSION_STATE pointer).

  Returns:
    Number of bytes read on success, or WOLFSSL_CBIO_ERR_WANT_READ/GENERAL on error.

  Remarks:
    None.

*/
static int _WDRV_PIC32MZW_TLS_IOCallbackRecv(WOLFSSL *pSSLCtx, char *pBuf, int buffSize, void *pAppCtx)
{
    _WDRV_PIC32MZW_TLS_PACKET *pTlsPacket = NULL;
    _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE *)pAppCtx;
    uint16_t pendReadTlsBuffLen = 0;
    uint16_t readTlsBuffLen = 0;
    uint16_t reqBufferSize = buffSize;

    if ((NULL == pSSLCtx) || (NULL == pBuf) || (NULL == pTlsSessionState) || (0 == buffSize))
    {
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }

    if (TCPIP_Helper_SingleListCount(&pTlsSessionState->rxPacketList) > 0)
    {
        pendReadTlsBuffLen = reqBufferSize;
        while(0 != pendReadTlsBuffLen)
        {
            if (TCPIP_Helper_SingleListCount(&pTlsSessionState->rxPacketList) > 0)
            {
                pTlsPacket = (_WDRV_PIC32MZW_TLS_PACKET*)TCPIP_Helper_SingleListHeadRemove(&pTlsSessionState->rxPacketList);
                if (NULL == pTlsPacket)
                {
                    /* Failed to get data from rxPacketList */
                    return WOLFSSL_CBIO_ERR_GENERAL;
                }
                else
                {
                    uint16_t buffSizeDiff = (reqBufferSize - readTlsBuffLen);
                    if (((pTlsPacket->bufferSize - pTlsPacket->offsetRead)) <= buffSizeDiff)
                    {
                        /* Copy the remaining data from the packet to the buffer */
                        memcpy((pBuf + readTlsBuffLen), (pTlsPacket->data + pTlsPacket->offsetRead), (pTlsPacket->bufferSize - pTlsPacket->offsetRead));
                        readTlsBuffLen += (pTlsPacket->bufferSize - pTlsPacket->offsetRead);
                        pendReadTlsBuffLen -= (pTlsPacket->bufferSize - pTlsPacket->offsetRead);
                        /* Delete the packet after copy */
                        OSAL_Free(pTlsPacket);
                    }
                    else
                    {
                        /* Copy a portion of the packet data to the buffer */
                        memcpy((pBuf + readTlsBuffLen), (pTlsPacket->data + pTlsPacket->offsetRead), buffSizeDiff);
                        pTlsPacket->offsetRead += buffSizeDiff;
                        readTlsBuffLen += buffSizeDiff;
                        pendReadTlsBuffLen -= buffSizeDiff;
                        /* Add the packet back to the queue at head position */
                        TCPIP_Helper_SingleListHeadAdd(&pTlsSessionState->rxPacketList, (SGL_LIST_NODE*)pTlsPacket);
                    }
                }
            }
            else
            {
                /* only partial data read.
                   wolfssl will call this function again to read pending data so now return what is available */
                return readTlsBuffLen;
            }
        }
        /* Data read full */
        return readTlsBuffLen;
    }
    /* No packet in queue */
    return WOLFSSL_CBIO_ERR_WANT_READ;
}

//*******************************************************************************
/*
  Function:
    static int _WDRV_PIC32MZW_TLS_IOCallbackSend
    (
        WOLFSSL *pSSLCtx,
        char *pBuf,
        int buffSize,
        void *pAppCtx
    )

  Summary:
    wolfSSL send callback for queuing data to the Tx packet queue.

  Description:
    This callback is invoked by wolfSSL when it needs to send TLS data.
    It allocates a packet, copies the data, and adds it to the Tx queue.

  Precondition:
    TLS session must be initialized.

  Parameters:
    pSSLCtx  - Pointer to the wolfSSL session context.
    pBuf     - Buffer containing data to send.
    buffSize - Size of the data to send.
    pAppCtx  - Application context (_WDRV_PIC32MZW_TLS_SESSION_STATE pointer).

  Returns:
    Number of bytes queued on success, or WOLFSSL_CBIO_ERR_WANT_WRITE/GENERAL on error.

  Remarks:
    None.

*/
static int _WDRV_PIC32MZW_TLS_IOCallbackSend(WOLFSSL *pSSLCtx, char *pBuf, int buffSize, void *pAppCtx)
{
    _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE*)pAppCtx;
    _WDRV_PIC32MZW_TLS_PACKET *pTlsPacket = NULL;

    if ((NULL == pSSLCtx) || (NULL == pBuf) || (NULL == pTlsSessionState) || (0 == buffSize))
    {
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
    }

    pTlsPacket = (_WDRV_PIC32MZW_TLS_PACKET *)OSAL_Malloc(sizeof(_WDRV_PIC32MZW_TLS_PACKET) + (sizeof(uint8_t) * buffSize));
    if (NULL == pTlsPacket)
    {
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    /* Copy the data to the _WDRV_PIC32MZW_TLS_PACKET structure */
    memcpy(pTlsPacket->data, pBuf, buffSize);
    pTlsPacket->bufferSize = buffSize;
    pTlsPacket->offsetRead = 0;

    /* Add the packet to Tx queue */
    TCPIP_Helper_SingleListTailAdd(&pTlsSessionState->txPacketList, (SGL_LIST_NODE*)pTlsPacket);

    return buffSize;
}

// *****************************************************************************
// *****************************************************************************
// Section: TLS Session Management Functions
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static WOLFSSL* _WDRV_PIC32MZW_TLS_SessionCreate
    (
        const WDRV_PIC32MZW_TLS_CONFIG *pTlsConfig,
        bool allowSessionCaching,
        void *callbackCtx
    )

  Summary:
    Creates a new wolfSSL TLS session configured with certificates and keys.

  Description:
    Initializes a wolfSSL context, loads CA certificate and optionally client
    certificate/key, configures I/O callbacks, and creates a new TLS session.

  Precondition:
    wolfSSL library must be initialized.

  Parameters:
    pTlsConfig          - Pointer to TLS configuration containing certificates.
    allowSessionCaching - If true, enables session resumption.
    callbackCtx         - Context pointer passed to I/O callbacks.

  Returns:
    Pointer to WOLFSSL session on success, NULL on failure.

  Remarks:
    None.

*/
static WOLFSSL* _WDRV_PIC32MZW_TLS_SessionCreate(const WDRV_PIC32MZW_TLS_CONFIG *pTlsConfig, bool allowSessionCaching, void *callbackCtx)
{
    WOLFSSL_CTX *pCtx = NULL;
    WOLFSSL *pSsl = NULL;

    /* Sanitize input pointer */
    if (NULL == pTlsConfig)
    {
        return NULL;
    }

    /* Sanitize CA certificate parameters */
    if (NULL == pTlsConfig->caCert.pData)
    {
        return NULL;
    }
    if (0 == pTlsConfig->caCert.length)
    {
        return NULL;
    }
    if (!((WOLFSSL_FILETYPE_PEM == pTlsConfig->caCert.format) || 
          (WOLFSSL_FILETYPE_ASN1 == pTlsConfig->caCert.format)))
    {
        return NULL;
    }

    /* Sanitize client certificate parameters (required only if clientAuth is true) */
    if (true == pTlsConfig->clientAuth)
    {
        if (NULL == pTlsConfig->clientCert.pData)
        {
            return NULL;
        }
        if (0 == pTlsConfig->clientCert.length)
        {
            return NULL;
        }
        if (!((WOLFSSL_FILETYPE_PEM == pTlsConfig->clientCert.format) || 
              (WOLFSSL_FILETYPE_ASN1 == pTlsConfig->clientCert.format)))
        {
            return NULL;
        }

        /* Sanitize private key parameters */
        if (NULL == pTlsConfig->clientPrivKey.pData)
        {
            return NULL;
        }
        if (0 == pTlsConfig->clientPrivKey.length)
        {
            return NULL;
        }
        if (!((WOLFSSL_FILETYPE_PEM == pTlsConfig->clientPrivKey.format) || 
              (WOLFSSL_FILETYPE_ASN1 == pTlsConfig->clientPrivKey.format)))
        {
            return NULL;
        }
    }

    /* Create wolfSSL context with TLS 1.2 client method */
    pCtx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (NULL == pCtx)
    {
        return NULL;
    }

    /* Load CA certificate for peer verification */
    if (SSL_SUCCESS != wolfSSL_CTX_load_verify_buffer(pCtx, 
                                                       pTlsConfig->caCert.pData, 
                                                       pTlsConfig->caCert.length, 
                                                       pTlsConfig->caCert.format))
    {
        wolfSSL_CTX_free(pCtx);
        return NULL;
    }

    /* Enable peer certificate verification */
    wolfSSL_CTX_set_verify(pCtx, WOLFSSL_VERIFY_PEER | WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

    /* Load client certificate and private key if client authentication is enabled */
    if (true == pTlsConfig->clientAuth)
    {
        /* Load client certificate */
        if (SSL_SUCCESS != wolfSSL_CTX_use_certificate_buffer(pCtx, 
                                                               pTlsConfig->clientCert.pData, 
                                                               pTlsConfig->clientCert.length, 
                                                               pTlsConfig->clientCert.format))
        {
            wolfSSL_CTX_free(pCtx);
            return NULL;
        }

        /* Load client private key */
        if (SSL_SUCCESS != wolfSSL_CTX_use_PrivateKey_buffer(pCtx, 
                                                              pTlsConfig->clientPrivKey.pData, 
                                                              pTlsConfig->clientPrivKey.length, 
                                                              pTlsConfig->clientPrivKey.format))
        {
            wolfSSL_CTX_free(pCtx);
            return NULL;
        }
    }

    /* Create WOLFSSL session from context */
    pSsl = wolfSSL_new(pCtx);
    if (NULL == pSsl)
    {
        wolfSSL_CTX_free(pCtx);
        return NULL;
    }

    wolfSSL_CTX_free(pCtx);
    pCtx = NULL;

    /* Set domain name for host verification if provided */
    if ('\0' != pTlsConfig->domainName[0])
    {
        if (SSL_SUCCESS != wolfSSL_check_domain_name(pSsl, pTlsConfig->domainName))
        {
            wolfSSL_free(pSsl);
            return NULL;
        }
    }

    /* Set IO callbacks */
    wolfSSL_SSLSetIORecv(pSsl, &_WDRV_PIC32MZW_TLS_IOCallbackRecv);
    wolfSSL_SSLSetIOSend(pSsl, &_WDRV_PIC32MZW_TLS_IOCallbackSend);

    /* Set callback context if provided */
    if (NULL != callbackCtx)
    {
        wolfSSL_SetIOWriteCtx(pSsl, callbackCtx);
        wolfSSL_SetIOReadCtx(pSsl, callbackCtx);
    }

#ifndef NO_SESSION_CACHE
    if ((true == pTlsConfig->sessionCaching) && (true == allowSessionCaching) && (NULL != pTlsConfig->pCachedSession))
    {
        /* Set cached session */
        if (SSL_SUCCESS != wolfSSL_set_session(pSsl, pTlsConfig->pCachedSession))
        {
            wolfSSL_free(pSsl);         
            return NULL;
        }
    }
#endif

    wolfSSL_KeepArrays(pSsl);
    wolfSSL_set_group_messages(pSsl);

    return pSsl;
}

#ifndef NO_SESSION_CACHE
//*******************************************************************************
/*
  Function:
    static void _WDRV_PIC32MZW_TLS_SessionCacheUpdate
    (
        WOLFSSL *pSsl,
        WOLFSSL_SESSION **ppCachedSession,
        bool deleteCachedSession
    )

  Summary:
    Updates the TLS session cache.

  Description:
    Saves or deletes the current session from the cache based on the
    deleteCachedSession flag.

  Precondition:
    TLS session must be established.

  Parameters:
    pSsl                - Pointer to the wolfSSL session.
    ppCachedSession     - Pointer to the cached session pointer.
    deleteCachedSession - If true, removes the session from cache.

  Returns:
    None.

  Remarks:
    None.

*/
static void _WDRV_PIC32MZW_TLS_SessionCacheUpdate(WOLFSSL *pSsl, WOLFSSL_SESSION **ppCachedSession, bool deleteCachedSession)
{
    if (false == deleteCachedSession)
    {
        /* Save session for future resumption */
        *ppCachedSession = wolfSSL_get_session(pSsl);
    }
    else
    {
        /* Delete cached session if it matches current session */
        WOLFSSL_SESSION *pSession = wolfSSL_get_session(pSsl);

        if (*ppCachedSession == pSession)
        {
            *ppCachedSession = NULL;
        }
    }
}
#endif

//*******************************************************************************
/*
  Function:
    static bool _WDRV_PIC32MZW_TLS_SessionTerminate
    (
        _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState,
        bool deleteCachedSession
    )

  Summary:
    Terminates a TLS session and cleans up resources.

  Description:
    Shuts down the wolfSSL session, frees resources, and clears the
    Tx/Rx packet queues.

  Precondition:
    None.

  Parameters:
    pTlsSessionState    - Pointer to the TLS session state.
    deleteCachedSession - If true, removes the session from cache.

  Returns:
    true on success, false on failure.

  Remarks:
    None.

*/
static bool _WDRV_PIC32MZW_TLS_SessionTerminate(_WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState, bool deleteCachedSession)
{
    if (NULL == pTlsSessionState)
    {
        return false;
    }

    /* Shutdown and free wolfSSL session */
    if (NULL != pTlsSessionState->pSession)
    {
#ifndef NO_SESSION_CACHE
        if (true == pTlsSessionState->sessionCaching)
        {
            _WDRV_PIC32MZW_TLS_SessionCacheUpdate(pTlsSessionState->pSession, pTlsSessionState->ppCachedSession, deleteCachedSession);
        }
#endif
        wolfSSL_shutdown(pTlsSessionState->pSession);
        wolfSSL_free(pTlsSessionState->pSession);
        pTlsSessionState->pSession = NULL;
    }

    /* Remove all packets from the Rx and Tx lists */
    _WDRV_PIC32MZW_SingleListRemoveAll(&pTlsSessionState->rxPacketList);
    _WDRV_PIC32MZW_SingleListRemoveAll(&pTlsSessionState->txPacketList);

    OSAL_Free(pTlsSessionState);

    return true;
}

//*******************************************************************************
/*
  Function:
    static void _WDRV_PIC32MZW_TLS_PacketsSend
    (
        WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle,
        _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState
    )

  Summary:
    Sends all pending packets from the Tx queue to the WiFi driver.

  Description:
    Iterates through the Tx packet queue and sends each packet to the
    driver using WDRV_PIC32MZW_TLS_NotifyTxPacketReady.

  Precondition:
    TLS session must be active.

  Parameters:
    drvSessionHandle  - Driver session handle.
    pTlsSessionState  - Pointer to the TLS session state.

  Returns:
    None.

  Remarks:
    None.

*/
static void _WDRV_PIC32MZW_TLS_PacketsSend(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState)
{
    while (TCPIP_Helper_SingleListCount(&pTlsSessionState->txPacketList) > 0)
    {
        WDRV_PIC32MZW_TLS_BUFFER tlsBuffer;
        _WDRV_PIC32MZW_TLS_PACKET *pTlsPacket;

        pTlsPacket = (_WDRV_PIC32MZW_TLS_PACKET*)TCPIP_Helper_SingleListHeadRemove(&pTlsSessionState->txPacketList);

        if (NULL == pTlsPacket)
        {
            break;
        }

        tlsBuffer.size  = pTlsPacket->bufferSize;
        tlsBuffer.pData = pTlsPacket->data;

        if (false == WDRV_PIC32MZW_TLS_NotifyTxPacketReady(drvSessionHandle, &tlsBuffer))
        {
            TCPIP_Helper_SingleListHeadAdd(&pTlsSessionState->txPacketList, (SGL_LIST_NODE*)pTlsPacket);
            break;
        }

        OSAL_Free(pTlsPacket);
    }
}

//*******************************************************************************
/*
  Function:
    static uint16_t _WDRV_PIC32MZW_TLS_BytesReadyToRead(WOLFSSL *pWolfSSLSession)

  Summary:
    Returns the number of decrypted bytes available to read.

  Description:
    Checks wolfSSL for pending decrypted data. If none pending, triggers
    processing of the next record using wolfSSL_peek.

  Precondition:
    TLS session must be established.

  Parameters:
    pWolfSSLSession - Pointer to the wolfSSL session.

  Returns:
    Number of bytes available to read.

  Remarks:
    None.

*/
static uint16_t _WDRV_PIC32MZW_TLS_BytesReadyToRead(WOLFSSL *pWolfSSLSession)
{
    uint16_t numBytes;

    if (NULL == pWolfSSLSession)
    {
        return 0;
    }

    numBytes = wolfSSL_pending(pWolfSSLSession);

    if (0 == numBytes)
    {
        char tmpBuffer;

        /* Trigger processing of the next record */
        if (0 != wolfSSL_peek(pWolfSSLSession, &tmpBuffer, 1))
        {
            numBytes = wolfSSL_pending(pWolfSSLSession);
        }
    }

    return numBytes;
}

//*******************************************************************************
/*
  Function:
    static void _WDRV_PIC32MZW_TLS_PacketsProcess
    (
        WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle,
        _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState
    )

  Summary:
    Reads decrypted data from TLS session and notifies the WiFi driver.

  Description:
    Reads available decrypted application data from the TLS session and
    sends it to the driver using WDRV_PIC32MZW_TLS_NotifyRxBufferReady.

  Precondition:
    TLS session must be established.

  Parameters:
    drvSessionHandle  - Driver session handle.
    pTlsSessionState  - Pointer to the TLS session state.

  Returns:
    None.

  Remarks:
    None.

*/
static void _WDRV_PIC32MZW_TLS_PacketsProcess(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState)
{
    uint16_t numBytes;

    numBytes = _WDRV_PIC32MZW_TLS_BytesReadyToRead(pTlsSessionState->pSession);

    if (numBytes > 0)
    {
        WDRV_PIC32MZW_TLS_BUFFER tlsBuffer;
        int rc;

        tlsBuffer.pData = (uint8_t*)OSAL_Malloc(numBytes);

        if (NULL == tlsBuffer.pData)
        {
            return;
        }

        rc = wolfSSL_read(pTlsSessionState->pSession, tlsBuffer.pData, numBytes);

        if (rc > 0)
        {
            tlsBuffer.size = rc;

            (void)WDRV_PIC32MZW_TLS_NotifyRxBufferReady(drvSessionHandle, &tlsBuffer);
        }

        OSAL_Free(tlsBuffer.pData);
    }
}

//*******************************************************************************
/*
  Function:
    static bool _WDRV_PIC32MZW_TLS_SessionHandshake
    (
        WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle,
        _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState
    )

  Summary:
    Performs the TLS handshake.

  Description:
    Executes the TLS handshake using wolfSSL_connect. Notifies the driver
    on successful connection or terminates the session on failure.

  Precondition:
    TLS session must be created.

  Parameters:
    drvSessionHandle  - Driver session handle.
    pTlsSessionState  - Pointer to the TLS session state.

  Returns:
    true if handshake is in progress or complete, false on failure.

  Remarks:
    None.

*/
static bool _WDRV_PIC32MZW_TLS_SessionHandshake(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState)
{
    int result;
    int error;

    if (NULL == pTlsSessionState->pSession)
    {
        return false;
    }

    result = wolfSSL_connect(pTlsSessionState->pSession);

    if (SSL_SUCCESS == result)
    {
        bool isResumed = 0 != wolfSSL_session_reused(pTlsSessionState->pSession);
        WDRV_PIC32MZW_TLS_NotifyConnectionEstablished(drvSessionHandle, isResumed);
        return true;
    }

    error = wolfSSL_get_error(pTlsSessionState->pSession, result);

    if ((SSL_ERROR_WANT_READ == error) || (SSL_ERROR_WANT_WRITE == error))
    {
        return true;
    }

    /* Handshake faillure: send queued packets and terminate session */
    _WDRV_PIC32MZW_TLS_PacketsSend(drvSessionHandle, pTlsSessionState);
    _WDRV_PIC32MZW_TLS_SessionTerminate(pTlsSessionState, true);
    return false;
}

// *****************************************************************************
// *****************************************************************************
// Section: User TLS Callback Implementation
// *****************************************************************************
// *****************************************************************************

bool WDRV_PIC32MZW_TLS_Callback(WDRV_PIC32MZW_TLS_USER_CB_EVENT event, void *pParam)
{
    if (NULL == pParam)
    {
        return false;
    }

    switch (event)
    {
        /* Driver requests creation of a new TLS session */
        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_CREATE_SESSION:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_CREATE_SESSION *pCreateSessionParam =
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_CREATE_SESSION*) pParam;
            WDRV_PIC32MZW_TLS_CONFIG *pTLSConfig = (WDRV_PIC32MZW_TLS_CONFIG*)pCreateSessionParam->userContextHandle;
            bool allowSessionCaching = pCreateSessionParam->allowSessionCaching;
            _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState;

            if (true == pCreateSessionParam->srvDomainRequired)
            {
                if ('\0' == pTLSConfig->domainName[0])
                {
                    return false;
                }
            }

            pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE*)OSAL_Malloc(sizeof(_WDRV_PIC32MZW_TLS_SESSION_STATE));

            if (NULL == pTlsSessionState)
            {
                return false;
            }

            pTlsSessionState->pSession = _WDRV_PIC32MZW_TLS_SessionCreate(pTLSConfig, allowSessionCaching, pTlsSessionState);

            if (NULL == pTlsSessionState->pSession)
            {
                OSAL_Free(pTlsSessionState);
                return false;
            }

#ifndef NO_SESSION_CACHE
            pTlsSessionState->sessionCaching = (true == pTLSConfig->sessionCaching) && (true == allowSessionCaching);
            pTlsSessionState->ppCachedSession = &pTLSConfig->pCachedSession;
#endif

            TCPIP_Helper_SingleListInitialize(&pTlsSessionState->txPacketList);
            TCPIP_Helper_SingleListInitialize(&pTlsSessionState->rxPacketList);

            pCreateSessionParam->userSessionHandle = (uintptr_t)pTlsSessionState;
        }
        break;

        /* Driver requests to start/initiate the TLS handshake */
        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_START_SESSION:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_START_SESSION *pStartSessionParam =
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_START_SESSION*) pParam;
            _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE*)pStartSessionParam->userSessionHandle;
            WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle = pStartSessionParam->drvSessionHandle;

            if (NULL == pTlsSessionState)
            {
                return false;
            }

            if (false == _WDRV_PIC32MZW_TLS_SessionHandshake(drvSessionHandle, pTlsSessionState))
            {
                return false;
            }
        }
        break;

        /* Driver notifies of session state change (negotiating/connected) */
        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_UPDATE_SESSION:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_UPDATE_SESSION *pUpdateSessionParam =
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_UPDATE_SESSION*) pParam;
            _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE*)pUpdateSessionParam->userSessionHandle;
            WDRV_PIC32MZW_TLS_SESSION_STATE sessionState = pUpdateSessionParam->drvSessionState;
            WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle = pUpdateSessionParam->drvSessionHandle;

            if (NULL == pTlsSessionState)
            {
                return false;
            }

            /* Continue handshake if still negotiating */
            if (WDRV_PIC32MZW_TLS_SESSION_STATE_NEGOTIATING == sessionState)
            {
                if (false == _WDRV_PIC32MZW_TLS_SessionHandshake(drvSessionHandle, pTlsSessionState))
                {
                    return false;
                }

                _WDRV_PIC32MZW_TLS_PacketsSend(drvSessionHandle, pTlsSessionState);
            }

            /* Process application data when connected */
            if (WDRV_PIC32MZW_TLS_SESSION_STATE_CONNECTED == sessionState)
            {
                _WDRV_PIC32MZW_TLS_PacketsSend(drvSessionHandle, pTlsSessionState);

                _WDRV_PIC32MZW_TLS_PacketsProcess(drvSessionHandle, pTlsSessionState);
            }
        }
        break;

        /* Driver requests generation of EAP session keys (MSK) */
        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_GENERATE_KEY:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_GENERATE_KEY *pGenerateKeyParam =
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_GENERATE_KEY*) pParam;
            _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE*)pGenerateKeyParam->userSessionHandle;

            if (NULL == pTlsSessionState)
            {
                return false;
            }

            if(0 != wolfSSL_make_eap_keys(pTlsSessionState->pSession,
                                          (void *)pGenerateKeyParam->pMsk,
                                          pGenerateKeyParam->lenKey,
                                          pGenerateKeyParam->pLabel))
            {
                return false;
            }
        }
        break;

        /* Driver provides received encrypted data for decryption */
        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_RECV_BUFF:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF *pBuffParam =
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF*) pParam;
            _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE*)pBuffParam->userSessionHandle;
            uint8_t *pBuffer = pBuffParam->pBuffer;
            uint16_t lenBuffer = pBuffParam->lenBuffer;
            _WDRV_PIC32MZW_TLS_PACKET *pTlsPacket;

            if (NULL == pTlsSessionState)
            {
                return false;
            }

            pTlsPacket = (_WDRV_PIC32MZW_TLS_PACKET *)OSAL_Malloc(sizeof(_WDRV_PIC32MZW_TLS_PACKET) + (sizeof(uint8_t) * lenBuffer));
            if (NULL == pTlsPacket)
            {
                return false;
            }

            memcpy(pTlsPacket->data, pBuffer, lenBuffer);
            pTlsPacket->bufferSize = lenBuffer;
            pTlsPacket->offsetRead = 0;

            /* Queue the received encrypted data for wolfSSL processing */
            TCPIP_Helper_SingleListTailAdd(&pTlsSessionState->rxPacketList, (SGL_LIST_NODE*)pTlsPacket);
        }
        break;

        /* Driver requests encryption and transmission of application data */
        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_WRITE_BUFF:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF *pBuffParam =
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF*) pParam;
            _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE*)pBuffParam->userSessionHandle;
            uint8_t *pBuffer = pBuffParam->pBuffer;
            uint16_t lenBuffer = pBuffParam->lenBuffer;

            if (NULL == pTlsSessionState)
            {
                return false;
            }

            if (wolfSSL_write(pTlsSessionState->pSession, pBuffer, lenBuffer) < lenBuffer)
            {
                return false;
            }
        }
        break;

        /* Driver requests termination and cleanup of the TLS session */
        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_TERMINATE_SESSION:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_TERMINATE_SESSION *pTermSessionParam =
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_TERMINATE_SESSION*) pParam;
            _WDRV_PIC32MZW_TLS_SESSION_STATE *pTlsSessionState = (_WDRV_PIC32MZW_TLS_SESSION_STATE*)pTermSessionParam->userSessionHandle;
            bool delCachedSession = pTermSessionParam->delCachedSession;

            if (NULL == pTlsSessionState)
            {
                return false;
            }

            if (false == _WDRV_PIC32MZW_TLS_SessionTerminate(pTlsSessionState, delCachedSession))
            {
                return false;
            }
        }
        break;
    }

    return true;
}

</#if>
<#if DRV_WIFI_PIC32MZW1_GENERATE_ENC_STUBS == true>
// *****************************************************************************
// *****************************************************************************
// Section: TLS Provider Stub Implementation
// *****************************************************************************
// *****************************************************************************

bool WDRV_PIC32MZW_TLS_Callback(WDRV_PIC32MZW_TLS_USER_CB_EVENT event, void *pParam)
{
    if (NULL == pParam)
    {
        return false;
    }

    switch (event)
    {
        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_CREATE_SESSION:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_CREATE_SESSION *pCreateSession = 
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_CREATE_SESSION *)pParam;
            break;
        }

        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_START_SESSION:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_START_SESSION *pStartSession = 
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_START_SESSION *)pParam;
            break;
        }

        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_UPDATE_SESSION:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_UPDATE_SESSION *pUpdateSession = 
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_UPDATE_SESSION *)pParam;
            break;
        }

        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_TERMINATE_SESSION:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_TERMINATE_SESSION *pTerminateSession = 
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_TERMINATE_SESSION *)pParam;
            break;
        }

        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_RECV_BUFF:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF *pRecvBuff = 
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF *)pParam;
            break;
        }

        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_WRITE_BUFF:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF *pWriteBuff = 
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF *)pParam;
            break;
        }

        case WDRV_PIC32MZW_TLS_USER_CB_EVENT_GENERATE_KEY:
        {
            WDRV_PIC32MZW_TLS_USER_CB_PARAM_GENERATE_KEY *pGenerateKey = 
                (WDRV_PIC32MZW_TLS_USER_CB_PARAM_GENERATE_KEY *)pParam;
            break;
        }

        default:
        {
            return false;
        }
    }

    return true;
}

</#if>
