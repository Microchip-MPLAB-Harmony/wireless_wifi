/*******************************************************************************
  WINC Wireless Driver

  File Name:
    wdrv_winc.c

  Summary:
    WINC wireless driver.

  Description:
    WINC wireless driver.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_eint.h"
#include "wdrv_winc_spi.h"
#include "nc_driver/winc_debug.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Defines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This is user configurable function pointer for printf style output from driver. */
#ifndef WDRV_WINC_DEVICE_USE_SYS_DEBUG
WDRV_WINC_DEBUG_PRINT_CALLBACK pfWINCDebugPrintCb;
#endif

/* This is the driver instance structure. */
static WDRV_WINC_DCPT wincDescriptor[2] =
{
    {
        .isInit  = false,
        .sysStat = SYS_STATUS_UNINITIALIZED,
        .isOpen  = false,
        .pCtrl   = NULL,
    },
    {
        .isInit  = false,
        .sysStat = SYS_STATUS_UNINITIALIZED,
        .isOpen  = false,
        .pCtrl   = NULL,
    }
};

// *****************************************************************************
// *****************************************************************************
// Section: WINC MAC Driver Global Data
// *****************************************************************************
// *****************************************************************************

/* This is the Control driver instance descriptor. */
static WDRV_WINC_CTRLDCPT wincCtrlDescriptor;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Callback Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver System Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static bool wincEventCheck(void)

  Summary:
    Check WINC device event.

  Description:
    Indicates if the WINC device has a pending event.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    True or false indicating if an event is pending.

  Remarks:
    None.

*/

static bool wincEventCheck(void)
{
    return (WDRV_WINC_INT_Get() == 0) ? true : false;
}

//*******************************************************************************
/*
  Function:
    static void wincSystemEvent
    (
        WDRV_WINC_DCPT *pDcpt,
        WDRV_WINC_SYSTEM_EVENT_TYPE event
    )

  Summary:
    Sends a system event.

  Description:
    Sends a system event notification via a callback registered with the driver.

  Precondition:
    WDRV_WINC_Initialize must have been called before calling this function.

  Parameters:
    pDcpt - Pointer to WINC device descriptor.
    event - System event.

  Returns:
    None.

  Remarks:
    None.

*/

static void wincSystemEvent
(
    WDRV_WINC_DCPT *pDcpt,
    WDRV_WINC_SYSTEM_EVENT_TYPE event
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pfEventCallback))
    {
        return;
    }

    pDcpt->pfEventCallback((SYS_MODULE_OBJ)pDcpt, event);
}

//*******************************************************************************
/*
  Function:
    static void wincResetDeviceState(WDRV_WINC_CTRLDCPT *pCtrl)

  Summary:
    Reset state related to WINC device.

  Description:
    Reset all control descriptor state associated with WINC device.

  Precondition:
    WDRV_WINC_Initialize must have been called before calling this function.

  Parameters:
    handle - WINC device handle.

  Returns:
    None.

  Remarks:
    None.

*/

static void wincResetDeviceState(WDRV_WINC_CTRLDCPT *pCtrl)
{
    int i;

    if (NULL == pCtrl)
    {
        return;
    }

    pCtrl->isAP                 = false;
    pCtrl->connectedState       = WDRV_WINC_CONN_STATE_DISCONNECTED;
    pCtrl->scanInProgress       = false;
    pCtrl->opChannel            = WDRV_WINC_CID_ANY;

    pCtrl->netIfSTA             = WDRV_WINC_NETIF_IDX_0;
    pCtrl->netIfAP              = WDRV_WINC_NETIF_IDX_0;

    pCtrl->assocInfoSTA.handle              = DRV_HANDLE_INVALID;
    pCtrl->assocInfoSTA.rssi                = 0;
    pCtrl->assocInfoSTA.peerAddress.valid   = false;
    pCtrl->assocInfoSTA.assocID             = 1;

    for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
    {
        pCtrl->assocInfoAP[i].handle            = DRV_HANDLE_INVALID;
        pCtrl->assocInfoAP[i].peerAddress.valid = false;
        pCtrl->assocInfoAP[i].assocID           = -1;
    }

    for (i=0; i<WDRV_WINC_TLS_CTX_NUM; i++)
    {
        pCtrl->tlscInfo[i].idxInUse = false;
    }

    for (i=0; i<WDRV_WINC_FILE_CTX_NUM; i++)
    {
        pCtrl->fileCtx[i].inUse = false;
    }

    pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED;

    pCtrl->pfBSSFindNotifyCB        = NULL;
    pCtrl->pfConnectNotifyCB        = NULL;
    pCtrl->pfAssociationRSSICB      = NULL;
    pCtrl->pfICMPEchoResponseCB     = NULL;
    pCtrl->pfSystemTimeGetCurrentCB = NULL;
    pCtrl->pfDNSResolveResponseCB   = NULL;
    pCtrl->pfFileFindCB             = NULL;
    pCtrl->pfFileDeleteCB           = NULL;
    pCtrl->mqtt.pfConnCB            = NULL;

    memset(&pCtrl->fwVersion, 0, sizeof(WDRV_WINC_FIRMWARE_VERSION_INFO));
}

//*******************************************************************************
/*
  Function:
    static void wincProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)

  Summary:
    Process command responses.

  Description:
    Processes command responses received via WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED events.

  Precondition:
    WINC_DevTransmitCmdReq must of been called to submit command request.

  Parameters:
    handle - WINC device handle.
    pElems - Pointer to command response elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void wincProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WDRV_WINC_CTRLDCPT *pCtrl;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (pElems->rspId)
    {
        case WINC_CMD_ID_CFG:
        {
            uint8_t id;

            if (pElems->numElems != 2)
            {
                return;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &id, sizeof(id));

            switch (id)
            {
                case WINC_CFG_PARAM_ID_CFG_VERSION:
                {
                    uint32_t majMinVer;

                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &majMinVer, sizeof(majMinVer));

                    pCtrl->fwVersion.version.major = majMinVer >> 16;
                    pCtrl->fwVersion.version.minor = majMinVer & 0xffff;
                    break;
                }

                case WINC_CFG_PARAM_ID_CFG_PATCH:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BYTE_ARRAY, &pCtrl->fwVersion.version.patch, sizeof(pCtrl->fwVersion.version.patch));
                    break;
                }

                case WINC_CFG_PARAM_ID_CFG_BUILD_HASH:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BYTE_ARRAY, &pCtrl->fwVersion.build.hash, sizeof(pCtrl->fwVersion.build.hash));
                    break;
                }

                case WINC_CFG_PARAM_ID_CFG_BUILD_TIME:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BYTE_ARRAY, &pCtrl->fwVersion.build.timeUTC, sizeof(pCtrl->fwVersion.build.timeUTC));
                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case WINC_CMD_ID_DI:
        {
            WINC_DEV_FRACT_INT_TYPE id;

            if (pElems->numElems < 2)
            {
                return;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

            switch (id.i)
            {
                case WINC_CFG_PARAM_ID_DI_DEVICE_ID:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->devInfo.id, sizeof(pCtrl->devInfo.id));
                    break;
                }

                case WINC_CFG_PARAM_ID_DI_NUM_IMAGES:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->devInfo.numImages, sizeof(pCtrl->devInfo.numImages));
                    break;
                }

                case WINC_CFG_PARAM_ID_DI_IMAGE_INFO:
                {
                    if (pElems->numElems < 4)
                    {
                        return;
                    }

                    if (id.f < WINC_CFG_PARAM_ID_DI_IMAGE_INFO_NUM)
                    {
                        WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->devInfo.image[id.f].seqNum, sizeof(pCtrl->devInfo.image[id.f].seqNum));
                        WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pCtrl->devInfo.image[id.f].version, sizeof(pCtrl->devInfo.image[id.f].version));
                        WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_INTEGER, &pCtrl->devInfo.image[id.f].srcAddr, sizeof(pCtrl->devInfo.image[id.f].srcAddr));
                    }

                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        default:
        {
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void wincCmdRspCallbackHandler
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        WINC_CMD_REQ_HANDLE cmdReqHandle,
        WINC_DEV_CMDREQ_EVENT_TYPE event,
        uintptr_t eventArg
    )

  Summary:
    Command response callback handler.

  Description:
    Receives command responses for command requests originating from this module.

  Precondition:
    WINC_DevTransmitCmdReq must of been called to submit command request.

  Parameters:
    context      - Context provided to WINC_CmdReqInit for callback.
    devHandle    - WINC device handle.
    cmdReqHandle - Command request handle.
    event        - Command request event being raised.
    eventArg     - Optional event specific information.

  Returns:
    None.

  Remarks:
    Events:
        WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
            Command request transmission has been completed.

        WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
            Command request has completed, all commands processed.

            The command request has completed and is no longer active.
            Any memory associated with the request can be reclaimed.

        WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
            A command within the request has received a command status response.

            eventArg points to an WINC_DEV_EVENT_STATUS_ARGS structure.
            This structure contains the response ID, status, sequence number
            and an WINC_DEV_EVENT_SRC_CMD detailing the source command, the index
            within the request, number and location of parameters.

            Source command parameters can be retrieved using WINC_DevUnpackElements.
            Parameter content can then be retrieved using WINC_CmdReadParamElem.

        WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
            A command within the request has received a command response.

            eventArg points to an WINC_DEV_EVENT_RSP_ELEMS structure.
            This structure contains the response ID, number and contents of
            response elements and an WINC_DEV_EVENT_SRC_CMD detailing the
            source command, the index within the request, number and location
            of parameters.

            Response elements can be retrieved using WINC_CmdReadParamElem.

            Source command parameters can be retrieved using WINC_DevUnpackElements.
            Parameter content can then be retrieved using WINC_CmdReadParamElem.

*/

static void wincCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)context;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return;
    }

    //WDRV_DBG_INFORM_PRINT("WINC CmdRspCB %08x Event %d\n", cmdReqHandle, event);

    switch (event)
    {
        case WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
        {
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
        {
            OSAL_Free((void*)cmdReqHandle);
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
        {
            WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (NULL != pStatusInfo)
            {
                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_CFG:
                    {
                        if (WDRV_WINC_STATUS_OK == pStatusInfo->status)
                        {
                            pDcpt->pCtrl->fwVersion.isValid = true;
                        }

                        break;
                    }

                    case WINC_CMD_ID_DI:
                    {
                        if (WDRV_WINC_STATUS_OK == pStatusInfo->status)
                        {
                            pDcpt->pCtrl->devInfo.isValid = true;
                        }
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }

            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                wincProcessCmdRsp((DRV_HANDLE)pDcpt, pRspElems);
            }
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    SYS_MODULE_OBJ WDRV_WINC_Initialize
    (
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT *const init
    )

  Summary:
    System interface initialization of the WINC driver.

  Description:
    This is the function that initializes the WINC driver. It is called by
    the system.

  Remarks:
    See wdrv_winc_api.h for usage information.

*/

SYS_MODULE_OBJ WDRV_WINC_Initialize
(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT *const init
)
{
    WDRV_WINC_DCPT *pDcpt;

    if (WDRV_WINC_SYS_IDX_0 == index)
    {
        const WDRV_WINC_SYS_INIT* const pInitData = (const WDRV_WINC_SYS_INIT* const)init;
        WINC_DEV_INIT devInitData;
        WINC_SOCKET_INIT_TYPE socketInitData;

        pDcpt = &wincDescriptor[0];

        if (true == pDcpt->isInit)
        {
            return (SYS_MODULE_OBJ)pDcpt;
        }

        pDcpt->pfEventCallback = NULL;

#ifndef WDRV_WINC_DEVICE_USE_SYS_DEBUG
        pfWINCDebugPrintCb = NULL;
#endif

        OSAL_SEM_Create(&wincCtrlDescriptor.drvEventSemaphore, OSAL_SEM_TYPE_COUNTING, 10, 0);

        wincCtrlDescriptor.extSysStat = (WDRV_WINC_SYS_STATUS)SYS_STATUS_UNINITIALIZED;

        if (NULL == pInitData)
        {
            pDcpt->sysStat = SYS_STATUS_ERROR;
            return SYS_MODULE_OBJ_INVALID;
        }

        WDRV_WINC_SPIInitialize(pInitData->pSPICfg);

        wincCtrlDescriptor.intSrc = pInitData->intSrc;

        /* Initialise the interrupts. */
        WDRV_WINC_INTInitialize((SYS_MODULE_OBJ)pDcpt, wincCtrlDescriptor.intSrc);

        wincResetDeviceState(&wincCtrlDescriptor);

        wincCtrlDescriptor.pfNetIfEventCB = NULL;

        wincCtrlDescriptor.pWINCDevReceiveBuffer = OSAL_Malloc(WDRV_WINC_DEV_RX_BUFF_SZ);

        if (NULL == wincCtrlDescriptor.pWINCDevReceiveBuffer)
        {
            WDRV_DBG_ERROR_PRINT("Receive buffer allocation failed\r\n");
            pDcpt->sysStat = SYS_STATUS_ERROR;
            return SYS_MODULE_OBJ_INVALID;
        }

        devInitData.pReceiveBuffer    = wincCtrlDescriptor.pWINCDevReceiveBuffer;
        devInitData.receiveBufferSize = WDRV_WINC_DEV_RX_BUFF_SZ;

        wincCtrlDescriptor.wincSDIOState = WINC_SDIO_STATE_UNKNOWN;
        wincCtrlDescriptor.wincDevHandle = WINC_DevInit(&devInitData);

        if (WINC_DEVICE_INVALID_HANDLE == wincCtrlDescriptor.wincDevHandle)
        {
            WDRV_DBG_ERROR_PRINT("Failed to initialise WINC device\r\n");
            pDcpt->sysStat = SYS_STATUS_ERROR;
            return SYS_MODULE_OBJ_INVALID;
        }

        socketInitData.pfMemAlloc = OSAL_Malloc;
        socketInitData.pfMemFree  = OSAL_Free;
        socketInitData.slabSize   = WDRV_WINC_DEV_SOCK_SLAB_SZ;
        socketInitData.numSlabs   = WDRV_WINC_DEV_SOCK_SLAB_NUM;

        if (false == WINC_SockInit(wincCtrlDescriptor.wincDevHandle, &socketInitData))
        {
            WDRV_DBG_ERROR_PRINT("Failed to initialise WINC sockets\r\n");
            pDcpt->sysStat = SYS_STATUS_ERROR;
            return SYS_MODULE_OBJ_INVALID;
        }

        WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_WSCNProcessAEC,      (uintptr_t)pDcpt);
        WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_WSTAProcessAEC,      (uintptr_t)pDcpt);
        WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_ICMPProcessAEC,      (uintptr_t)pDcpt);
        WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_TIMEProcessAEC,      (uintptr_t)pDcpt);
        WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_WAPProcessAEC,       (uintptr_t)pDcpt);
        WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_MQTTProcessAEC,      (uintptr_t)pDcpt);
        WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_EXTCRYPTOProcessAEC, (uintptr_t)pDcpt);
        WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_DNSProcessAEC,       (uintptr_t)pDcpt);

        wincCtrlDescriptor.delayTimer = SYS_TIME_HANDLE_INVALID;
    }
    else
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Set initial state. */
    pDcpt->isInit  = true;
    pDcpt->isOpen  = false;
    pDcpt->sysStat = SYS_STATUS_BUSY;
    pDcpt->pCtrl   = &wincCtrlDescriptor;

    return (SYS_MODULE_OBJ)pDcpt;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_Deinitialize(SYS_MODULE_OBJ object)

  Summary:
    WINC driver deinitialization function.

  Description:
    This is the function that deinitializes the WINC.
    It is called by the system.

  Remarks:
    See wdrv_winc_api.h for usage information.

*/

void WDRV_WINC_Deinitialize(SYS_MODULE_OBJ object)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)object;

    if ((SYS_MODULE_OBJ_INVALID == object) || (NULL == pDcpt))
    {
        return;
    }

    if (false == pDcpt->isInit)
    {
        return;
    }

    pDcpt->sysStat = SYS_STATUS_UNINITIALIZED;

    if (pDcpt == &wincDescriptor[0])
    {
        OSAL_SEM_Post(&wincCtrlDescriptor.drvEventSemaphore);
    }

    WINC_SockRegisterEventCallback(wincCtrlDescriptor.wincDevHandle, NULL, 0);
    WINC_SockDeinit(wincCtrlDescriptor.wincDevHandle);

    WINC_DevAECCallbackDeregister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_WSCNProcessAEC);
    WINC_DevAECCallbackDeregister(wincCtrlDescriptor.wincDevHandle, WDRV_WINC_WSTAProcessAEC);

    OSAL_Free(wincCtrlDescriptor.pWINCDevReceiveBuffer);
    wincCtrlDescriptor.pWINCDevReceiveBuffer = NULL;

    WINC_DevDeinit(wincCtrlDescriptor.wincDevHandle);
    wincCtrlDescriptor.wincSDIOState = WINC_SDIO_STATE_UNKNOWN;

    /* Clear internal state. */
    pDcpt->isOpen = false;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_Reinitialize
    (
        SYS_MODULE_OBJ object,
        const SYS_MODULE_INIT *const init
    )

  Summary:
    WINC driver reinitialization function.

  Description:
    This is the function that re-initializes the WINC.
    It is called by the system.

  Remarks:
    See wdrv_winc_api.h for usage information.

*/

void WDRV_WINC_Reinitialize
(
    SYS_MODULE_OBJ object,
    const SYS_MODULE_INIT *const init
)
{
}

//*******************************************************************************
/*
  Function:
    SYS_STATUS WDRV_WINC_Status(SYS_MODULE_OBJ object)

  Summary:
    Provides the current status of the WINC driver module.

  Description:
    This function provides the current status of the WINC driver module.

  Remarks:
    See wdrv_winc_api.h for usage information.

*/

SYS_STATUS WDRV_WINC_Status(SYS_MODULE_OBJ object)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)object;

    if ((SYS_MODULE_OBJ_INVALID == object) || (NULL == pDcpt))
    {
        return SYS_STATUS_ERROR;
    }

    if (true == pDcpt->isInit)
    {
        if (SYS_STATUS_UNINITIALIZED == pDcpt->sysStat)
        {
            return SYS_STATUS_BUSY;
        }
    }

    return pDcpt->sysStat;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_SYS_STATUS WDRV_WINC_StatusExt(SYS_MODULE_OBJ object)

  Summary:
    Provides the extended system status of the WINC driver module.

  Description:
    This function provides the extended system status of the WINC driver
    module.

  Remarks:
    See wdrv_pic32mzw.h for usage information.

*/

WDRV_WINC_SYS_STATUS WDRV_WINC_StatusExt(SYS_MODULE_OBJ object)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)object;

    if ((SYS_MODULE_OBJ_INVALID == object) || (NULL == pDcpt))
    {
        return (WDRV_WINC_SYS_STATUS)SYS_STATUS_ERROR;
    }

    if (NULL == pDcpt->pCtrl)
    {
        return (WDRV_WINC_SYS_STATUS)SYS_STATUS_ERROR;
    }

    if (pDcpt->pCtrl->extSysStat != (WDRV_WINC_SYS_STATUS)SYS_STATUS_UNINITIALIZED)
    {
        return pDcpt->pCtrl->extSysStat;
    }

    /* If not in extended state, just return normal status. */
    return (WDRV_WINC_SYS_STATUS)pDcpt->sysStat;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_RegisterSystemEventCallback
    (
        SYS_MODULE_OBJ object,
        WDRV_WINC_SYSTEM_EVENT_CALLBACK pfEventCallback
    )

  Summary:
    Register system event callback.

  Description:
    This function registers a system event callback to be notified of system
    events.

  Remarks:
    See wdrv_winc_api.h for usage information.

*/

void WDRV_WINC_RegisterSystemEventCallback(SYS_MODULE_OBJ object, WDRV_WINC_SYSTEM_EVENT_CALLBACK pfEventCallback)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)object;

    if ((SYS_MODULE_OBJ_INVALID == object) || (NULL == pDcpt))
    {
        return;
    }

    pDcpt->pfEventCallback = pfEventCallback;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_Tasks(SYS_MODULE_OBJ object)

  Summary:
    Maintains the WINC drivers state machine.

  Description:
    This function is used to maintain the driver's internal state machine.

  Remarks:
    See wdrv_winc_api.h for usage information.

*/

void WDRV_WINC_Tasks(SYS_MODULE_OBJ object)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)object;

    if ((SYS_MODULE_OBJ_INVALID == object) || (NULL == pDcpt))
    {
        return;
    }

    switch (pDcpt->sysStat)
    {
        /* Uninitialised state. */
        case SYS_STATUS_UNINITIALIZED:
        {
            if (true == pDcpt->isInit)
            {
                /* Destroy event semaphore. */
                OSAL_SEM_Delete(&pDcpt->pCtrl->drvEventSemaphore);

                pDcpt->pCtrl->extSysStat = (WDRV_WINC_SYS_STATUS)SYS_STATUS_UNINITIALIZED;

                /* De-initialise SPI handling. */
                WDRV_WINC_SPIDeinitialize();

                /* De-initialise the interrupts. */
                WDRV_WINC_INTDeinitialize(pDcpt->pCtrl->intSrc);

                WDRV_WINC_RESETN_Clear();

                pDcpt->isInit = false;
            }

            break;
        }

        case SYS_STATUS_BUSY:
        {
            WINC_SDIO_STATUS_TYPE wincSDIOStatus;
            bool delayComplete = false;

            if (NULL == pDcpt->pCtrl)
            {
                pDcpt->sysStat = SYS_STATUS_ERROR;
                break;
            }

            if (SYS_TIME_HANDLE_INVALID != pDcpt->pCtrl->delayTimer)
            {
                if (true != SYS_TIME_DelayIsComplete(pDcpt->pCtrl->delayTimer))
                {
                    break;
                }

                pDcpt->pCtrl->delayTimer = SYS_TIME_HANDLE_INVALID;
                delayComplete = true;
            }

            if (WINC_SDIO_STATE_UNKNOWN == pDcpt->pCtrl->wincSDIOState)
            {
                if (false == delayComplete)
                {
                    WDRV_DBG_INFORM_PRINT("WINC: Reset\r\n");
                    WDRV_WINC_RESETN_Clear();
                    SYS_TIME_DelayMS(100, &pDcpt->pCtrl->delayTimer);

                    wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_HARD_RESET);
                    break;
                }
                else
                {
                    if (0 == WDRV_WINC_RESETN_Get())
                    {
                        WDRV_WINC_RESETN_Set();
                        SYS_TIME_DelayMS(100, &pDcpt->pCtrl->delayTimer);
                        break;
                    }

                    WDRV_DBG_INFORM_PRINT("WINC: Initializing...");

                    /* Open SPI handling. */
                    if (false == WDRV_WINC_SPIOpen())
                    {
                        pDcpt->sysStat = SYS_STATUS_ERROR;
                        break;
                    }

                    pDcpt->pCtrl->wincSDIOResetTimeoutCount = 100;
                    pDcpt->pCtrl->wincHardReset             = true;

                    wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_INIT_BEGIN);
                }
            }

            wincSDIOStatus = WINC_SDIODeviceInit(&pDcpt->pCtrl->wincSDIOState, WDRV_WINC_SPISendReceive);

            if (WINC_SDIO_STATUS_OK == wincSDIOStatus)
            {
                WINC_CMD_REQ_HANDLE cmdReqHandle;
                void *pCmdReqBuffer;

                wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_INIT_COMPLETE);

                pCmdReqBuffer = OSAL_Malloc(128);

                if (NULL == pCmdReqBuffer)
                {
                    pDcpt->sysStat = SYS_STATUS_ERROR;
                    break;
                }

                cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 2, wincCmdRspCallbackHandler, (uintptr_t)pDcpt);

                if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
                {
                    OSAL_Free(pCmdReqBuffer);
                    pDcpt->sysStat = SYS_STATUS_ERROR;
                    break;
                }

                WINC_CmdCFG(cmdReqHandle, 0, WINC_TYPE_INVALID, 0, 0);
                WINC_CmdDI(cmdReqHandle, 0);

                if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
                {
                    OSAL_Free(pCmdReqBuffer);
                    pDcpt->sysStat = SYS_STATUS_ERROR;
                    break;
                }

                pDcpt->sysStat = SYS_STATUS_READY;

                WDRV_DBG_INFORM_PRINT("complete\r\n");

                if (true == wincEventCheck())
                {
                    OSAL_SEM_Post(&pDcpt->pCtrl->drvEventSemaphore);
                }

                wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_READY);
            }
            else if (WINC_SDIO_STATUS_RESET_FAILED == wincSDIOStatus)
            {
                WDRV_DBG_INFORM_PRINT("failed reset\r\n");

                pDcpt->pCtrl->extSysStat = WDRV_WINC_SYS_STATUS_ERROR_DEVICE_NOT_FOUND;
                pDcpt->sysStat           = SYS_STATUS_ERROR;

                wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_RESET_FAILED);
                break;
            }
            else
            {
                if (WINC_SDIO_STATUS_RESET_WAITING == wincSDIOStatus)
                {
                    pDcpt->pCtrl->wincSDIOResetTimeoutCount--;

                    if (0 == pDcpt->pCtrl->wincSDIOResetTimeoutCount)
                    {
                        WDRV_DBG_INFORM_PRINT("reset timed out\r\n");

                        if (false == pDcpt->pCtrl->wincHardReset)
                        {
                            pDcpt->pCtrl->wincSDIOState = WINC_SDIO_STATE_UNKNOWN;

                            wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_RESET_RETRY);
                        }
                        else
                        {
                            pDcpt->pCtrl->extSysStat = WDRV_WINC_SYS_STATUS_ERROR_DEVICE_NOT_FOUND;
                            pDcpt->sysStat           = SYS_STATUS_ERROR;

                            wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_RESET_TIMEOUT);
                        }
                        break;
                    }
                }

                WDRV_DBG_INFORM_PRINT(".");
                SYS_TIME_DelayMS(100, &pDcpt->pCtrl->delayTimer);
            }

            break;
        }

        /* Running steady state. */
        case SYS_STATUS_READY:
        {
            if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&pDcpt->pCtrl->drvEventSemaphore, 0))
            {
                WINC_DevHandleEvent(pDcpt->pCtrl->wincDevHandle, wincEventCheck);
            }

            if (false == WINC_DevUpdateEvent(pDcpt->pCtrl->wincDevHandle))
            {
                int i;

                WDRV_DBG_ERROR_PRINT("WINC event update failed, resetting\r\n");

                wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_COMMS_ERROR);

                WINC_SockInit(pDcpt->pCtrl->wincDevHandle, NULL);

                if (NULL != pDcpt->pCtrl->mqtt.pfConnCB)
                {
                    if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED != pDcpt->pCtrl->mqtt.connState)
                    {
                        pDcpt->pCtrl->mqtt.pfConnCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.connCbCtx, WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED);
                    }
                }

                if (NULL != pDcpt->pCtrl->pfConnectNotifyCB)
                {
                    if (true == pDcpt->pCtrl->isAP)
                    {
                        for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
                        {
                            if ((DRV_HANDLE_INVALID != pDcpt->pCtrl->assocInfoAP[i].handle) &&
                                (true == pDcpt->pCtrl->assocInfoAP[i].peerAddress.valid))
                            {
                                pDcpt->pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)&pDcpt->pCtrl->assocInfoAP[i], WDRV_WINC_CONN_STATE_DISCONNECTED);
                            }
                        }
                    }
                    else
                    {
                        if (WDRV_WINC_CONN_STATE_DISCONNECTED != pDcpt->pCtrl->connectedState)
                        {
                            pDcpt->pCtrl->pfConnectNotifyCB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)&pDcpt->pCtrl->assocInfoSTA, WDRV_WINC_CONN_STATE_DISCONNECTED);
                        }
                    }
                }

                wincResetDeviceState(pDcpt->pCtrl);

                WDRV_DBG_INFORM_PRINT("WINC: Initializing...");

                pDcpt->pCtrl->wincSDIOState = WINC_SDIO_STATE_RESETTING;
                pDcpt->pCtrl->wincDevHandle = WINC_DevInit(NULL);

                pDcpt->pCtrl->wincSDIOResetTimeoutCount = 10;
                pDcpt->pCtrl->wincHardReset             = false;

                pDcpt->sysStat = SYS_STATUS_BUSY;

                wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_INIT_BEGIN);
            }

            break;
        }

        /* Error state.*/
        case SYS_STATUS_ERROR:
        {
            break;
        }

        default:
        {
            pDcpt->sysStat = SYS_STATUS_ERROR;
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Client Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_DebugRegisterCallback
    (
        WDRV_WINC_DEBUG_PRINT_CALLBACK const pfDebugPrintCallback
    )

  Summary:
    Register callback for debug serial stream.

  Description:
    The debug serial stream provides a printf-like stream of messages from within
    the WINC driver. The caller can provide a function to be called when
    output is available.

  Remarks:
    See wdrv_winc.h for usage information.

 */

#ifndef WDRV_WINC_DEVICE_USE_SYS_DEBUG
void WDRV_WINC_DebugRegisterCallback
(
    WDRV_WINC_DEBUG_PRINT_CALLBACK const pfDebugPrintCallback
)
{
    pfWINCDebugPrintCb = pfDebugPrintCallback;
}
#endif

//*******************************************************************************
/*
  Function:
    DRV_HANDLE WDRV_WINC_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent)

  Summary:
    Opens an instance of the WINC driver.

  Description:
    Opens an instance of the WINC driver and returns a handle which must be
    used when calling other driver functions.

  Remarks:
    See wdrv_winc.h for usage information.

*/

DRV_HANDLE WDRV_WINC_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent)
{
    WDRV_WINC_DCPT *pDcpt = NULL;

    if (WDRV_WINC_SYS_IDX_0 == index)
    {
        pDcpt = &wincDescriptor[0];

        if (NULL == pDcpt->pCtrl)
        {
            return DRV_HANDLE_INVALID;
        }
    }
    else
    {
        return DRV_HANDLE_INVALID;
    }

    /* Check that the driver has been initialised. */
    if (false == pDcpt->isInit)
    {
        return DRV_HANDLE_INVALID;
    }

    /* Check if the driver has already been opened. */
    if (true == pDcpt->isOpen)
    {
        return DRV_HANDLE_INVALID;
    }

    if (WDRV_WINC_SYS_IDX_0 == index)
    {
        pDcpt->pCtrl->handle                = (DRV_HANDLE)pDcpt;
        pDcpt->pCtrl->scanChannelMask24     = WDRV_WINC_CM_2_4G_EUROPE;
        pDcpt->pCtrl->pfBSSFindNotifyCB     = NULL;
        pDcpt->pCtrl->pfConnectNotifyCB     = NULL;
        pDcpt->pCtrl->pfAssociationRSSICB   = NULL;
    }

    pDcpt->isOpen = true;

    WDRV_WINC_WifiRegDomainGet((DRV_HANDLE)pDcpt, WDRV_WINC_REGDOMAIN_SELECT_CURRENT, NULL);
    WDRV_WINC_WifiCoexConfGet((DRV_HANDLE)pDcpt, NULL, NULL);

    return (DRV_HANDLE)pDcpt;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_Close(DRV_HANDLE handle)

  Summary:
    Closes an instance of the WINC driver.

  Description:
    This is the function that closes an instance of the MAC.
    All per client data is released and the handle can no longer be used
    after this function is called.

  Remarks:
    See wdrv_winc.h for usage information.

*/

void WDRV_WINC_Close(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return;
    }

    if (handle == pDcpt->pCtrl->handle)
    {
        pDcpt->pCtrl->pfBSSFindNotifyCB     = NULL;
        pDcpt->pCtrl->pfConnectNotifyCB     = NULL;
        pDcpt->pCtrl->pfAssociationRSSICB   = NULL;
    }

    pDcpt->isOpen = false;
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver General Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_InfoDriverVersionGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DRIVER_VERSION_INFO *const pDriverVersion
    )

  Summary:
    Returns the drivers version information.

  Description:
    Returns information on the drivers version.

  Remarks:
    See wdrv_winc.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_InfoDriverVersionGet
(
    DRV_HANDLE handle,
    WDRV_WINC_DRIVER_VERSION_INFO *const pDriverVersion
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDriverVersion))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Copy driver version information. */
    memset(pDriverVersion, 0, sizeof(WDRV_WINC_DRIVER_VERSION_INFO));

    pDriverVersion->version.major = WINC_DEV_DRIVER_VERSION_MAJOR_NO;
    pDriverVersion->version.minor = WINC_DEV_DRIVER_VERSION_MINOR_NO;
    pDriverVersion->version.patch = WINC_DEV_DRIVER_VERSION_PATCH_NO;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_InfoDeviceFirmwareVersionGet
    (
        DRV_HANDLE handle,
        bool active,
        WDRV_WINC_FIRMWARE_VERSION_INFO *const pFirmwareVersion
    )

  Summary:
    Returns the WINC firmware version information.

  Description:
    Returns information on the WINC firmware version, HIF protocol support
      and build date/time.

  Remarks:
    See wdrv_winc.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_InfoDeviceFirmwareVersionGet
(
    DRV_HANDLE handle,
    bool active,
    WDRV_WINC_FIRMWARE_VERSION_INFO *const pFirmwareVersion
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt)|| (NULL == pDcpt->pCtrl) || (NULL == pFirmwareVersion))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (false == pDcpt->pCtrl->fwVersion.isValid)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    if (true == active)
    {
        memcpy(pFirmwareVersion, &pDcpt->pCtrl->fwVersion, sizeof(WDRV_WINC_FIRMWARE_VERSION_INFO));
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_InfoDeviceGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DEVICE_INFO *const pDeviceInfo
    )

  Summary:
    Returns the WINC device information.

  Description:
    Returns information on the WINC device.

  Remarks:
    See wdrv_winc.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_InfoDeviceGet
(
    DRV_HANDLE handle,
    WDRV_WINC_DEVICE_INFO *const pDeviceInfo
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt)|| (NULL == pDcpt->pCtrl) || (NULL == pDeviceInfo))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (false == pDcpt->pCtrl->devInfo.isValid)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    memcpy(pDeviceInfo, &pDcpt->pCtrl->devInfo, sizeof(WDRV_WINC_DEVICE_INFO));

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DebugUARTSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DEBUG_UART uartPort,
        uint32_t uartBaudRate
    )

  Summary:
    Configures the debug UART on the WINC device.

  Description:
    Sets the debug UART and baud rate on the WINC device.

  Remarks:
    See wdrv_winc.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DebugUARTSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DEBUG_UART uartPort,
    uint32_t uartBaudRate
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (uartPort > WDRV_WINC_DEBUG_UART_2)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 2, wincCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdCFG(cmdReqHandle, WINC_CFG_PARAM_ID_CFG_DEBUG_PORT, WINC_TYPE_INTEGER_UNSIGNED, uartPort, 0);

    if (0 != uartBaudRate)
    {
        WINC_CmdCFG(cmdReqHandle, WINC_CFG_PARAM_ID_CFG_DEBUG_BAUD, WINC_TYPE_INTEGER_UNSIGNED, uartBaudRate, 0);
    }

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_ISR(SYS_MODULE_OBJ object);

  Summary:
    WINC interrupt handler.

  Description:
    This function is called by the interrupt routines to signal an interrupt
      from the WINC.

  Remarks:
    See wdrv_winc_api.h for usage information.

*/

void WDRV_WINC_ISR(SYS_MODULE_OBJ object)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)object;

    if (NULL == pDcpt->pCtrl)
    {
        return;
    }

    if (false == pDcpt->isInit)
    {
        return;
    }

    if (SYS_STATUS_READY == pDcpt->sysStat)
    {
        OSAL_SEM_PostISR(&pDcpt->pCtrl->drvEventSemaphore);
    }
}
