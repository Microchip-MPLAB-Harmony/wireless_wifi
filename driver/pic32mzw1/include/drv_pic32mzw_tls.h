/*
Copyright (C) 2020-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _DRV_PIC32MZW_TLS_H
#define _DRV_PIC32MZW_TLS_H

/* Session handle type */
typedef void* DRV_PIC32MZW_TLS_SESSION_HANDLE;

/* Invalid session handle */
#define DRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE  (((DRV_PIC32MZW_TLS_SESSION_HANDLE) NULL))

/* TLS event type */
typedef enum
{
    /* Delivers the session establishment status with the remote peer. */
    DRV_PIC32MZW_TLS_EVENT_CONNECTION_CREATED,
    /* TLS packet is ready for transmission. */
    DRV_PIC32MZW_TLS_EVENT_TX_PKT_READY,
    /* Application packet is received. */
    DRV_PIC32MZW_TLS_EVENT_RX_BUFFER_READY,
} DRV_PIC32MZW_TLS_EVENT;

/* Callback function type through which TLS events will be notified to the Wifi */
typedef void (*DRV_PIC32MZW_TLS_WIFI_CB)
(
    DRV_PIC32MZW_TLS_EVENT tlsEvent,
    void  *pvEventData,
    uint32_t userData
);

typedef struct
{
    uint8_t *pData;
    uint16_t size;
} DRV_PIC32MZW_TLS_BUFFER;

typedef struct{
    /* flag indicating if the session is resumed */
    bool isResumed;
} DRV_PIC32MZW_TLS_CONN_INFO;

/* WLAN API */
DRV_PIC32MZW_TLS_SESSION_HANDLE DRV_PIC32MZW_TLS_CreateSession(uint16_t txBufferOffset, uint16_t txBufferExtraSpace, bool allowSessionCaching, DRV_PIC32MZW_TLS_WIFI_CB fpWifiCallback, uint32_t wifiCallbackCtx);
bool DRV_PIC32MZW_TLS_StartSession(DRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle);
bool DRV_PIC32MZW_TLS_TerminateSession(DRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, bool authSuccess);
bool DRV_PIC32MZW_TLS_RecvBuffer(DRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, uint8_t *pDataBuff, uint16_t bufferSize);
bool DRV_PIC32MZW_TLS_WriteBuffer(DRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, uint8_t *pDataBuff, uint16_t bufferSize);
bool DRV_PIC32MZW_TLS_GenerateKey(DRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, uint8_t *pMskEmsk, uint16_t keyLen, const char *pLabel);

#endif /* _DRV_PIC32MZW_TLS_H */
