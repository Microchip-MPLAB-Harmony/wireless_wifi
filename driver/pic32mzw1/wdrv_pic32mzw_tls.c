/*******************************************************************************
  PIC32MZW Driver TLS interface.

  File Name:
    drv_pic32mzw1_tls.c

  Summary:
    PIC32MZW Driver TLS interface.

  Description:
    This module implements the interface between the TLS stack and the firmware.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2020-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "configuration.h"
#include "drv_pic32mzw_tls.h"
#include "wdrv_pic32mzw_tls.h"
#include "drv_pic32mzw1.h"

#ifdef WDRV_PIC32MZW_TLS_SUPPORT

// *****************************************************************************
// *****************************************************************************
// Section: Data structures
// *****************************************************************************
// *****************************************************************************

typedef struct
{
    /* User session handle */
    uintptr_t userSessionHandle;

    /* User context handle */
    uintptr_t userContextHandle;

    /* Tx buffer offset */
    uint16_t lowerLayerHdrSize;

    /* Tx buffer extra space */
    uint16_t lowerLayerFtrSize;

    /* Wifi callback */
    DRV_PIC32MZW_TLS_WIFI_CB fpWifiCallback;

    /* Wifi callback context */
    uint32_t wifiCallbackCtx;

    /* Session state */
    WDRV_PIC32MZW_TLS_SESSION_STATE state;

} WDRV_PIC32MZW_TLS_SESSION;

typedef struct
{
    /* User callback */
    WDRV_PIC32MZW_TLS_USER_CB fpUserCallback;

    /* TLS sessions */
    WDRV_PIC32MZW_TLS_SESSION sessions[WDRV_PIC32MZW_TLS_MAX_NUM_SESSIONS];

    /* Number of available TLS sessions */
    uint8_t numSessions;

    /* Flag indicating if the server domain is required  */
    bool srvDomainRequired;

} WDRV_PIC32MZW_TLS_DCPT;

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

static WDRV_PIC32MZW_TLS_DCPT dcpt;

// *****************************************************************************
// *****************************************************************************
// Section: Static functions
// *****************************************************************************
// *****************************************************************************

static void _WDRV_PIC32MZW_TLS_Notify(WDRV_PIC32MZW_TLS_SESSION *pSession, DRV_PIC32MZW_TLS_EVENT event, void *pArg)
{
    if (NULL != pSession->fpWifiCallback)
    {
        pSession->fpWifiCallback(event, pArg, pSession->wifiCallbackCtx);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Driver internal functions
// *****************************************************************************
// *****************************************************************************

bool DRV_PIC32MZW_TLS_Init
(
    WDRV_PIC32MZW_TLS_USER_CB userCallback,
    uintptr_t userContextHandles[WDRV_PIC32MZW_TLS_MAX_NUM_SESSIONS],
    uint8_t numSessions,
    bool srvDomainRequired
)
{
    uint8_t i;

    if (WDRV_PIC32MZW_TLS_MAX_NUM_SESSIONS < numSessions)
    {
        return false;
    }

    dcpt.fpUserCallback = userCallback;
    dcpt.numSessions = numSessions;
    dcpt.srvDomainRequired = srvDomainRequired;

    for (i=0; i<dcpt.numSessions; i++)
    {
        dcpt.sessions[i].userContextHandle = userContextHandles[i];
        dcpt.sessions[i].state = WDRV_PIC32MZW_TLS_SESSION_STATE_INVALID;
    }

    return true;
}

void DRV_PIC32MZW_TLS_DeInit(void)
{
    uint8_t i;

    dcpt.fpUserCallback = NULL;

    for (i=0; i<dcpt.numSessions; i++)
    {
        dcpt.sessions[i].state = WDRV_PIC32MZW_TLS_SESSION_STATE_INVALID;
    }
}

void DRV_PIC32MZW_TLS_Update(void)
{
    uint8_t i;

    if (NULL == dcpt.fpUserCallback)
    {
        return;
    }

    for (i=0; i<dcpt.numSessions; i++)
    {
        WDRV_PIC32MZW_TLS_SESSION *pSession = &dcpt.sessions[i];
        WDRV_PIC32MZW_TLS_USER_CB_PARAM_UPDATE_SESSION cbParam;

        if (WDRV_PIC32MZW_TLS_SESSION_STATE_INVALID == pSession->state)
        {
            continue;
        }

        cbParam.userSessionHandle = pSession->userSessionHandle;
        cbParam.drvSessionState   = pSession->state;
        cbParam.drvSessionHandle  = (WDRV_PIC32MZW_TLS_SESSION_HANDLE)pSession;

        if (false == dcpt.fpUserCallback(WDRV_PIC32MZW_TLS_USER_CB_EVENT_UPDATE_SESSION, (void*)&cbParam))
        {
            pSession->state = WDRV_PIC32MZW_TLS_SESSION_STATE_INVALID;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Public functions
// *****************************************************************************
// *****************************************************************************

bool WDRV_PIC32MZW_TLS_NotifyRxBufferReady
(
    WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, 
    WDRV_PIC32MZW_TLS_BUFFER *pUserBuffer
)
{
    WDRV_PIC32MZW_TLS_SESSION *pSession = (WDRV_PIC32MZW_TLS_SESSION*)drvSessionHandle;
    DRV_PIC32MZW_TLS_BUFFER drvBuffer;

    if (WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE == pSession)
    {
        return false;
    }

    if ((NULL == pUserBuffer) || (NULL == pUserBuffer->pData) || (0 == pUserBuffer->size))
    {
        return false;
    }

    drvBuffer.pData = pUserBuffer->pData;
    drvBuffer.size = pUserBuffer->size;
    
    _WDRV_PIC32MZW_TLS_Notify(pSession, DRV_PIC32MZW_TLS_EVENT_RX_BUFFER_READY, (void*)&drvBuffer);

    return true;
}

bool WDRV_PIC32MZW_TLS_NotifyTxPacketReady
(
    WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, 
    WDRV_PIC32MZW_TLS_BUFFER *pUserBuffer
)
{
    WDRV_PIC32MZW_TLS_SESSION *pSession = (WDRV_PIC32MZW_TLS_SESSION*)drvSessionHandle;
    DRV_PIC32MZW_TLS_BUFFER drvBuffer;
    size_t bufferSize;

    if (WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE == pSession)
    {
        return false;
    }

    if ((NULL == pUserBuffer) || (NULL == pUserBuffer->pData) || (0 == pUserBuffer->size))
    {
        return false;
    }

    bufferSize = pSession->lowerLayerHdrSize + pUserBuffer->size + pSession->lowerLayerFtrSize;

    drvBuffer.pData = DRV_PIC32MZW_PacketMemAlloc(DRV_PIC32MZW_ALLOC_OPT_PARAMS bufferSize, MEM_PRI_TX);

    if (NULL == drvBuffer.pData)
    {
        return false;
    }

    memcpy(drvBuffer.pData+pSession->lowerLayerHdrSize, pUserBuffer->pData, pUserBuffer->size);

    drvBuffer.size = pUserBuffer->size;

    _WDRV_PIC32MZW_TLS_Notify(pSession, DRV_PIC32MZW_TLS_EVENT_TX_PKT_READY, (void*)&drvBuffer);

    return true;
}

bool WDRV_PIC32MZW_TLS_NotifyConnectionEstablished(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, bool isResumed)
{
    WDRV_PIC32MZW_TLS_SESSION *pSession = (WDRV_PIC32MZW_TLS_SESSION*)drvSessionHandle;
    DRV_PIC32MZW_TLS_CONN_INFO connectionInfo;

    if (WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE == pSession)
    {
        return false;
    }

    pSession->state = WDRV_PIC32MZW_TLS_SESSION_STATE_CONNECTED;

    connectionInfo.isResumed = isResumed;

    _WDRV_PIC32MZW_TLS_Notify(pSession, DRV_PIC32MZW_TLS_EVENT_CONNECTION_CREATED, &connectionInfo);

    return true;
}

#endif /* WDRV_PIC32MZW_TLS_SUPPORT */

// *****************************************************************************
// *****************************************************************************
// Section: Driver internal functions
// *****************************************************************************
// *****************************************************************************

WDRV_PIC32MZW_TLS_SESSION_HANDLE DRV_PIC32MZW_TLS_CreateSession
(
    uint16_t lowerLayerHdrSize,
    uint16_t lowerLayerFtrSize,
    bool allowSessionCaching,
    DRV_PIC32MZW_TLS_WIFI_CB fpWifiCallback,
    uint32_t wifiCallbackCtx
)
{
#ifdef WDRV_PIC32MZW_TLS_SUPPORT
    WDRV_PIC32MZW_TLS_USER_CB_PARAM_CREATE_SESSION cbParam;
    WDRV_PIC32MZW_TLS_SESSION *pSession = NULL;
    uint8_t i;

    if (NULL == dcpt.fpUserCallback)
    {
        return WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE;
    }

    /* Look for an unused session */
    for (i=0; i<dcpt.numSessions; i++)
    {
        if (WDRV_PIC32MZW_TLS_SESSION_STATE_INVALID == dcpt.sessions[i].state)
        {
            pSession = &dcpt.sessions[i];
            break;
        }
    }

    /* No unused session found */
    if (NULL == pSession)
    {
        return false;
    }

    pSession->lowerLayerHdrSize = lowerLayerHdrSize;
    pSession->lowerLayerFtrSize = lowerLayerFtrSize;
    pSession->fpWifiCallback = fpWifiCallback;
    pSession->wifiCallbackCtx = wifiCallbackCtx;

    cbParam.userContextHandle   = pSession->userContextHandle;
    cbParam.allowSessionCaching = allowSessionCaching;
    cbParam.srvDomainRequired   = dcpt.srvDomainRequired;

    if (false == dcpt.fpUserCallback(WDRV_PIC32MZW_TLS_USER_CB_EVENT_CREATE_SESSION, (void*)&cbParam))
    {
        return WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE;
    }

    pSession->userSessionHandle = cbParam.userSessionHandle;
    pSession->state             = WDRV_PIC32MZW_TLS_SESSION_STATE_INITIALIZED;

    return (WDRV_PIC32MZW_TLS_SESSION_HANDLE)pSession;
#else
    return WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE;
#endif
}

bool DRV_PIC32MZW_TLS_StartSession(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle)
{
#ifdef WDRV_PIC32MZW_TLS_SUPPORT
    WDRV_PIC32MZW_TLS_SESSION *pSession = (WDRV_PIC32MZW_TLS_SESSION*)drvSessionHandle;
    WDRV_PIC32MZW_TLS_USER_CB_PARAM_START_SESSION cbParam;

    if ((NULL == dcpt.fpUserCallback) || (WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE == pSession))
    {
        return false;
    }

    if (WDRV_PIC32MZW_TLS_SESSION_STATE_INITIALIZED != pSession->state)
    {
        return false;
    }

    cbParam.userSessionHandle = pSession->userSessionHandle;
    cbParam.drvSessionHandle  = drvSessionHandle;

    if (false == dcpt.fpUserCallback(WDRV_PIC32MZW_TLS_USER_CB_EVENT_START_SESSION, (void*)&cbParam))
    {
        return false;
    }

    pSession->state = WDRV_PIC32MZW_TLS_SESSION_STATE_NEGOTIATING;

    return true;
#else
    return false;
#endif
}

bool DRV_PIC32MZW_TLS_TerminateSession(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, bool authSuccess)
{
#ifdef WDRV_PIC32MZW_TLS_SUPPORT
    WDRV_PIC32MZW_TLS_SESSION *pSession = (WDRV_PIC32MZW_TLS_SESSION*)drvSessionHandle;
    WDRV_PIC32MZW_TLS_USER_CB_PARAM_TERMINATE_SESSION cbParam;

    if ((NULL == dcpt.fpUserCallback) || (WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE == pSession))
    {
        return false;
    }

    if (WDRV_PIC32MZW_TLS_SESSION_STATE_NEGOTIATING > pSession->state)
    {
        return false;
    }

    cbParam.userSessionHandle = pSession->userSessionHandle;
    cbParam.delCachedSession  = false == authSuccess;

    if (false == dcpt.fpUserCallback(WDRV_PIC32MZW_TLS_USER_CB_EVENT_TERMINATE_SESSION, (void*)&cbParam))
    {
        return false;
    }

    pSession->userSessionHandle = cbParam.userSessionHandle;
    pSession->state             = WDRV_PIC32MZW_TLS_SESSION_STATE_INVALID;

    return true;
#else
    return false;
#endif
}

bool DRV_PIC32MZW_TLS_RecvBuffer(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, uint8_t *pBuffer, uint16_t bufferSize)
{
#ifdef WDRV_PIC32MZW_TLS_SUPPORT
    WDRV_PIC32MZW_TLS_SESSION *pSession = (WDRV_PIC32MZW_TLS_SESSION*)drvSessionHandle;
    WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF cbParam;

    if ((NULL == dcpt.fpUserCallback) || (WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE == pSession))
    {
        return false;
    }

    if (WDRV_PIC32MZW_TLS_SESSION_STATE_NEGOTIATING > pSession->state)
    {
        return false;
    }

    cbParam.userSessionHandle = pSession->userSessionHandle;
    cbParam.pBuffer           = pBuffer;
    cbParam.lenBuffer         = bufferSize;

    return dcpt.fpUserCallback(WDRV_PIC32MZW_TLS_USER_CB_EVENT_RECV_BUFF, (void*)&cbParam);
#else
    return false;
#endif
}

bool DRV_PIC32MZW_TLS_WriteBuffer(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, uint8_t *pBuffer, uint16_t bufferSize)
{
#ifdef WDRV_PIC32MZW_TLS_SUPPORT
    WDRV_PIC32MZW_TLS_SESSION *pSession = (WDRV_PIC32MZW_TLS_SESSION*)drvSessionHandle;
    WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF cbParam;
    bool result;

    if ((NULL == dcpt.fpUserCallback) || (WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE == pSession))
    {
        return false;
    }

    if (WDRV_PIC32MZW_TLS_SESSION_STATE_CONNECTED != pSession->state)
    {
        return false;
    }

    cbParam.userSessionHandle = pSession->userSessionHandle;
    cbParam.pBuffer           = pBuffer;
    cbParam.lenBuffer         = bufferSize;

    result = dcpt.fpUserCallback(WDRV_PIC32MZW_TLS_USER_CB_EVENT_WRITE_BUFF, (void*)&cbParam);

    DRV_PIC32MZW_PacketMemFree(DRV_PIC32MZW_ALLOC_OPT_ARGS pBuffer);

    return result;
#else
    return false;
#endif
}

bool DRV_PIC32MZW_TLS_GenerateKey
(
    WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle,
    uint8_t *pMsk,
    uint16_t keyLen,
    const char *pLabel
)
{
#ifdef WDRV_PIC32MZW_TLS_SUPPORT
    WDRV_PIC32MZW_TLS_SESSION *pSession = (WDRV_PIC32MZW_TLS_SESSION*)drvSessionHandle;
    WDRV_PIC32MZW_TLS_USER_CB_PARAM_GENERATE_KEY cbParam;

    if ((NULL == dcpt.fpUserCallback) || (WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE == pSession))
    {
        return false;
    }

    if (WDRV_PIC32MZW_TLS_SESSION_STATE_CONNECTED != pSession->state)
    {
        return false;
    }

    cbParam.userSessionHandle = pSession->userSessionHandle;
    cbParam.pMsk              = pMsk;
    cbParam.lenKey            = keyLen;
    cbParam.pLabel            = pLabel;

    return dcpt.fpUserCallback(WDRV_PIC32MZW_TLS_USER_CB_EVENT_GENERATE_KEY, (void*)&cbParam);
#else
    return false;
#endif
}
