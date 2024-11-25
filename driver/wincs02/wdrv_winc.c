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

/* AEC callback dispatch table. */
static const WINC_DEV_AEC_RSP_CB wincAecCallbackTable[] =
{
    WDRV_WINC_WSCNProcessAEC,
    WDRV_WINC_WSTAProcessAEC,
#ifndef WDRV_WINC_MOD_DISABLE_ICMP
    WDRV_WINC_ICMPProcessAEC,
#endif
    WDRV_WINC_TIMEProcessAEC,
    WDRV_WINC_WAPProcessAEC,
#ifndef WDRV_WINC_MOD_DISABLE_MQTT
    WDRV_WINC_MQTTProcessAEC,
#endif
    WDRV_WINC_EXTCRYPTOProcessAEC,
#ifndef WDRV_WINC_MOD_DISABLE_DNS
    WDRV_WINC_DNSProcessAEC,
#endif
    WDRV_WINC_NETIFProcessAEC,
#ifndef WDRV_WINC_MOD_DISABLE_PROV
    WDRV_WINC_ProvProcessAEC,
#endif
    WDRV_WINC_AssocProcessAEC,
#ifndef WDRV_WINC_MOD_DISABLE_OTA
    WDRV_WINC_OTAProcessAEC,
#endif
#ifndef WDRV_WINC_MOD_DISABLE_NVM
    WDRV_WINC_NVMProcessAEC,
#endif
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
    unsigned int i;

    if (NULL == pCtrl)
    {
        return;
    }

    pCtrl->isAP                 = false;
    pCtrl->connectedState       = WDRV_WINC_CONN_STATE_DISCONNECTED;
    pCtrl->scanInProgress       = false;
    pCtrl->opChannel            = WDRV_WINC_CID_ANY;
#ifndef WDRV_WINC_MOD_DISABLE_OTA
    pCtrl->otaState.operation   = WDRV_WINC_OTA_OPERATION_NONE;
#endif
#ifndef WDRV_WINC_MOD_DISABLE_NVM
    pCtrl->nvmState.operation   = WDRV_WINC_NVM_OPERATION_NONE;
#endif

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
        pCtrl->assocInfoAP[i].assocID           = 0xffffU;
    }

#ifndef WDRV_WINC_MOD_DISABLE_TLS
    for (i=0; i<WDRV_WINC_TLS_CTX_NUM; i++)
    {
        pCtrl->tlscInfo[i].idxInUse = false;
    }
#endif

    for (i=0; i<WDRV_WINC_FILE_CTX_NUM; i++)
    {
        pCtrl->fileCtx[i].inUse = false;
    }

#ifndef WDRV_WINC_MOD_DISABLE_MQTT
    pCtrl->mqtt.connState = WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED;
    pCtrl->mqtt.pfConnCB  = NULL;
#endif

    pCtrl->pfBSSFindNotifyCB        = NULL;
    pCtrl->pfConnectNotifyCB        = NULL;
    pCtrl->pfAssociationRSSICB      = NULL;
#ifndef WDRV_WINC_MOD_DISABLE_ICMP
    pCtrl->pfICMPEchoResponseCB     = NULL;
#endif
    pCtrl->pfSystemTimeGetCurrentCB = NULL;
#ifndef WDRV_WINC_MOD_DISABLE_DNS
    pCtrl->pfDNSResolveResponseCB   = NULL;
#endif
    pCtrl->pfFileFindCB             = NULL;
    pCtrl->pfFileDeleteCB           = NULL;

    (void)memset(&pCtrl->fwVersion, 0, sizeof(WDRV_WINC_FIRMWARE_VERSION_INFO));
}

//*******************************************************************************
/*
  Function:
    static void wincProcessCmdRsp(DRV_HANDLE handle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)

  Summary:
    Process command responses.

  Description:
    Processes command responses received via WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED events.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    handle - WINC device handle.
    pElems - Pointer to command response elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void wincProcessCmdRsp(DRV_HANDLE handle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)
{
    const WDRV_WINC_DCPT *pDcpt = (const WDRV_WINC_DCPT *)handle;
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

            if (2U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &id, sizeof(id));

            switch (id)
            {
                case WINC_CFG_PARAM_ID_CFG_VERSION:
                {
                    uint32_t majMinVer;

                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &majMinVer, sizeof(majMinVer));

                    pCtrl->fwVersion.version.major = (uint16_t)(majMinVer >> 16);
                    pCtrl->fwVersion.version.minor = (uint16_t)(majMinVer & 0xffffU);
                    break;
                }

                case WINC_CFG_PARAM_ID_CFG_PATCH:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BYTE_ARRAY, &pCtrl->fwVersion.version.patch, sizeof(pCtrl->fwVersion.version.patch));
                    break;
                }

                case WINC_CFG_PARAM_ID_CFG_BUILD_HASH:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BYTE_ARRAY, &pCtrl->fwVersion.build.hash, sizeof(pCtrl->fwVersion.build.hash));
                    break;
                }

                case WINC_CFG_PARAM_ID_CFG_BUILD_TIME:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_BYTE_ARRAY, &pCtrl->fwVersion.build.timeUTC, sizeof(pCtrl->fwVersion.build.timeUTC));
                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("CFG ID %d not handled\r\n", id);
                    break;
                }
            }

            break;
        }

        case WINC_CMD_ID_DI:
        {
            WINC_DEV_FRACT_INT_TYPE id;

            if (pElems->numElems < 2U)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

            switch (id.i)
            {
                case WINC_CFG_PARAM_ID_DI_DEVICE_ID:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->devInfo.id, sizeof(pCtrl->devInfo.id));
                    break;
                }

                case WINC_CFG_PARAM_ID_DI_NUM_IMAGES:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->devInfo.numImages, sizeof(pCtrl->devInfo.numImages));
                    break;
                }

                case WINC_CFG_PARAM_ID_DI_IMAGE_INFO:
                {
                    if (pElems->numElems < 4U)
                    {
                        return;
                    }

                    if (id.f < (int16_t)WINC_CFG_PARAM_NUM_DI_IMAGE_INFO)
                    {
                        (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->devInfo.image[id.f].seqNum, sizeof(pCtrl->devInfo.image[id.f].seqNum));
                        (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pCtrl->devInfo.image[id.f].version, sizeof(pCtrl->devInfo.image[id.f].version));
                        (void)WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_INTEGER, &pCtrl->devInfo.image[id.f].srcAddr, sizeof(pCtrl->devInfo.image[id.f].srcAddr));
                    }

                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("DI ID %d not handled\r\n", id.i);
                    break;
                }
            }

            break;
        }

#ifndef WDRV_WINC_MOD_DISABLE_NVM
        case WINC_CMD_ID_NVMC:
        {
            uint8_t id;

            if (2U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &id, sizeof(id));

            switch (id)
            {
                case WINC_CFG_PARAM_ID_NVM_START_OFFSET:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->nvmState.geom.address.start, sizeof(pCtrl->nvmState.geom.address.start));
                    break;
                }

                case WINC_CFG_PARAM_ID_NVM_NUM_SECTORS:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->nvmState.geom.sector.number, sizeof(pCtrl->nvmState.geom.sector.number));
                    break;
                }

                case WINC_CFG_PARAM_ID_NVM_SECTOR_SZ:
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->nvmState.geom.sector.size, sizeof(pCtrl->nvmState.geom.sector.size));
                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("NVMC ID %d not handled\r\n", id);
                    break;
                }
            }

            break;
        }
#endif

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("WINC CmdRspCB ID %04x not handled\r\n", pElems->rspId);
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
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    context      - Context provided to WDRV_WINC_CmdReqInit for callback.
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

    //WDRV_DBG_INFORM_PRINT("WINC CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

    switch (event)
    {
        case WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
        {
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
        {
            OSAL_Free((WINC_COMMAND_REQUEST*)cmdReqHandle);
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
        {
            const WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (const WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (NULL != pStatusInfo)
            {
                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_CFG:
                    {
                        if (WDRV_WINC_STATUS_OK == (WDRV_WINC_STATUS)pStatusInfo->status)
                        {
                            pDcpt->pCtrl->fwVersion.isValid = true;
                        }

                        break;
                    }

                    case WINC_CMD_ID_DI:
                    {
                        if (WDRV_WINC_STATUS_OK == (WDRV_WINC_STATUS)pStatusInfo->status)
                        {
                            pDcpt->pCtrl->devInfo.isValid = true;
                        }
                        break;
                    }

                    default:
                    {
                        WDRV_DBG_VERBOSE_PRINT("WINC CmdRspCB %08x ID %04x status %04x not handled\r\n", cmdReqHandle, pStatusInfo->rspCmdId, pStatusInfo->status);
                        break;
                    }
                }
            }

            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                wincProcessCmdRsp((DRV_HANDLE)pDcpt, pRspElems);
            }
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("WINC CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    void wincProcessAEC
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

static void wincProcessAEC(uintptr_t context, WINC_DEVICE_HANDLE devHandle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)
{
    const WDRV_WINC_DCPT *pDcpt = (const WDRV_WINC_DCPT *)context;
    unsigned int i;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    for (i=0; i<(sizeof(wincAecCallbackTable)/sizeof(WINC_DEV_AEC_RSP_CB)); i++)
    {
        if (NULL == wincAecCallbackTable[i])
        {
            continue;
        }

        wincAecCallbackTable[i](context, devHandle, pElems);
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
#ifdef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
        WINC_SOCKET_INIT_TYPE socketInitData;
#endif
        pDcpt = &wincDescriptor[0];

        if (true == pDcpt->isInit)
        {
            return (SYS_MODULE_OBJ)pDcpt;
        }

        pDcpt->pfEventCallback = NULL;

#ifndef WDRV_WINC_DEVICE_USE_SYS_DEBUG
        pfWINCDebugPrintCb = NULL;
#endif

        (void)OSAL_SEM_Create(&wincCtrlDescriptor.drvEventSemaphore, OSAL_SEM_TYPE_COUNTING, 10, 0);

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

#ifdef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
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
#endif
        (void)WINC_DevAECCallbackRegister(wincCtrlDescriptor.wincDevHandle, wincProcessAEC, (uintptr_t)pDcpt);

        wincCtrlDescriptor.delayTimer = SYS_TIME_HANDLE_INVALID;

        wincCtrlDescriptor.pfL2DataMonitorCB = NULL;
#ifndef WDRV_WINC_MOD_DISABLE_PROV
        wincCtrlDescriptor.pfProvAttachCB = NULL;
#endif
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
        (void)OSAL_SEM_Post(&wincCtrlDescriptor.drvEventSemaphore);
    }

#ifdef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
    (void)WINC_SockRegisterEventCallback(wincCtrlDescriptor.wincDevHandle, NULL, 0);
    (void)WINC_SockDeinit(wincCtrlDescriptor.wincDevHandle);
#endif

    (void)WINC_DevAECCallbackDeregister(wincCtrlDescriptor.wincDevHandle, wincProcessAEC);

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
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)object;

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
    See wdrv_winc.h for usage information.

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
                (void)OSAL_SEM_Delete(&pDcpt->pCtrl->drvEventSemaphore);

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
                    (void)SYS_TIME_DelayMS(100, &pDcpt->pCtrl->delayTimer);

                    wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_HARD_RESET);
                    break;
                }
                else
                {
                    if (0 == WDRV_WINC_RESETN_Get())
                    {
                        WDRV_WINC_RESETN_Set();
                        (void)SYS_TIME_DelayMS(100, &pDcpt->pCtrl->delayTimer);
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

                wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_INIT_COMPLETE);

                cmdReqHandle = WDRV_WINC_CmdReqInit(3, 0, wincCmdRspCallbackHandler, (uintptr_t)pDcpt);

                if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
                {
                    pDcpt->sysStat = SYS_STATUS_ERROR;
                    break;
                }

                (void)WINC_CmdCFG(cmdReqHandle, WINC_CMDCFG_ID_IGNORE_VAL, WINC_TYPE_INVALID, 0, 0);
                (void)WINC_CmdDI(cmdReqHandle, WINC_CMDDI_ID_IGNORE_VAL);
#ifndef WDRV_WINC_MOD_DISABLE_NVM
                (void)WINC_CmdNVMC(cmdReqHandle, WINC_CMDNVMC_ID_IGNORE_VAL, WINC_TYPE_INVALID, 0, 0);
#endif

                if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
                {
                    pDcpt->sysStat = SYS_STATUS_ERROR;
                    break;
                }

                pDcpt->sysStat = SYS_STATUS_READY;

                WDRV_DBG_INFORM_PRINT("complete\r\n");

                if (true == wincEventCheck())
                {
                    (void)OSAL_SEM_Post(&pDcpt->pCtrl->drvEventSemaphore);
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
            else if (WINC_SDIO_STATUS_OP_FAILED == wincSDIOStatus)
            {
                WDRV_DBG_INFORM_PRINT("failed op\r\n");

                pDcpt->pCtrl->wincSDIOState = WINC_SDIO_STATE_UNKNOWN;

                wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_RESET_RETRY);
            }
            else
            {
                if (WINC_SDIO_STATUS_RESET_WAITING == wincSDIOStatus)
                {
                    pDcpt->pCtrl->wincSDIOResetTimeoutCount--;

                    if (0U == pDcpt->pCtrl->wincSDIOResetTimeoutCount)
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
                (void)SYS_TIME_DelayMS(100, &pDcpt->pCtrl->delayTimer);
            }

            break;
        }

        /* Running steady state. */
        case SYS_STATUS_READY:
        {
            if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&pDcpt->pCtrl->drvEventSemaphore, 0))
            {
                (void)WINC_DevHandleEvent(pDcpt->pCtrl->wincDevHandle, wincEventCheck);
            }

            if (false == WINC_DevUpdateEvent(pDcpt->pCtrl->wincDevHandle))
            {
                WDRV_DBG_ERROR_PRINT("WINC event update failed, resetting\r\n");

                wincSystemEvent(pDcpt, WDRV_WINC_SYSTEM_EVENT_DEVICE_COMMS_ERROR);

#ifdef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
                (void)WINC_SockInit(pDcpt->pCtrl->wincDevHandle, NULL);
#endif
#ifndef WDRV_WINC_MOD_DISABLE_MQTT
                if (NULL != pDcpt->pCtrl->mqtt.pfConnCB)
                {
                    if (WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED != pDcpt->pCtrl->mqtt.connState)
                    {
                        pDcpt->pCtrl->mqtt.pfConnCB((DRV_HANDLE)pDcpt, pDcpt->pCtrl->mqtt.connCbCtx, WDRV_WINC_MQTT_CONN_STATUS_DISCONNECTED, &pDcpt->pCtrl->mqtt.connInfo);
                    }
                }
#endif

                if (NULL != pDcpt->pCtrl->pfConnectNotifyCB)
                {
                    if (true == pDcpt->pCtrl->isAP)
                    {
                        unsigned int i;

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
        pDcpt->pCtrl->handle                  = (DRV_HANDLE)pDcpt;
        pDcpt->pCtrl->scanChannelMask24       = WDRV_WINC_CM_2_4G_DEFAULT;
        pDcpt->pCtrl->scanRssiThreshold       = 0;
        pDcpt->pCtrl->regulatoryChannelMask24 = WDRV_WINC_CM_2_4G_DEFAULT;
        pDcpt->pCtrl->pfBSSFindNotifyCB       = NULL;
        pDcpt->pCtrl->pfConnectNotifyCB       = NULL;
        pDcpt->pCtrl->pfAssociationRSSICB     = NULL;
    }

    pDcpt->isOpen = true;

    (void)WDRV_WINC_WifiRegDomainGet((DRV_HANDLE)pDcpt, WDRV_WINC_REGDOMAIN_SELECT_CURRENT, NULL);
    (void)WDRV_WINC_WifiCoexConfGet((DRV_HANDLE)pDcpt, NULL, NULL);

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
// Section: WINC L2 Access Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_L2DataMonitorCallbackSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_L2DATA_MONITOR_CALLBACK pfL2DataMonitorCB
    )

  Summary:
    Set the L2 data frame monitor callback.

  Description:
    Sets the callback used to report L2 data frames.

  Remarks:
    See wdrv_winc_mac.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_L2DataMonitorCallbackSet
(
    DRV_HANDLE handle,
    WDRV_WINC_L2DATA_MONITOR_CALLBACK pfL2DataMonitorCB
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pDcpt->pCtrl->pfL2DataMonitorCB = pfL2DataMonitorCB;

    cmdReqHandle = WDRV_WINC_CmdReqInit(2, 0, wincCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)WDRV_WINC_NETIF_IDX_DEFAULT, WINC_CFG_PARAM_ID_NETIF_L2_MONITOR_MODE, WINC_TYPE_INTEGER_UNSIGNED, (NULL != pfL2DataMonitorCB) ? 7U : 0U, 0);
    (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)WDRV_WINC_NETIF_IDX_DEFAULT, WINC_CFG_PARAM_ID_NETIF_L2_MONITOR_MAX_LEN, WINC_TYPE_INTEGER_UNSIGNED, 0, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_L2DataFrameSend
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        const uint8_t *const pl2Data,
        size_t l2DataLen
    )

  Summary:
    Sends an L2 frame to the WiFi layer.

  Description:
    Queues an L2 frame to the WiFi subsystem for transmission.

  Remarks:
    See wdrv_winc_mac.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_L2DataFrameSend
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    const uint8_t *const pl2Data,
    size_t l2DataLen
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pl2Data))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, l2DataLen, wincCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdNETIFTX(cmdReqHandle, (uint8_t)WDRV_WINC_NETIF_IDX_DEFAULT, pl2Data, l2DataLen);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Information Implementation
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
    (void)memset(pDriverVersion, 0, sizeof(WDRV_WINC_DRIVER_VERSION_INFO));

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
        (void)memcpy(pFirmwareVersion, &pDcpt->pCtrl->fwVersion, sizeof(WDRV_WINC_FIRMWARE_VERSION_INFO));
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

    (void)memcpy(pDeviceInfo, &pDcpt->pCtrl->devInfo, sizeof(WDRV_WINC_DEVICE_INFO));

    return WDRV_WINC_STATUS_OK;
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Configuration Archive Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_CfgArchiveStore
    (
        DRV_HANDLE handle,
        const char *pFilename
    )

  Summary:
    Archives a configuration set.

  Description:
    Archives the currently active configuration set to a flash file.

  Remarks:
    See wdrv_winc.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_CfgArchiveStore
(
    DRV_HANDLE handle,
    const char *pFilename
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t lenFilename;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pFilename))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    lenFilename = strnlen(pFilename, WINC_CMD_PARAM_SZ_FS_FILENAME+1U);

    if (lenFilename > WINC_CMD_PARAM_SZ_FS_FILENAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wincCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdCFGCP(cmdReqHandle, WINC_TYPE_INTEGER_UNSIGNED, 0, 0, WINC_TYPE_STRING, (uintptr_t)pFilename, lenFilename);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_CfgArchiveRecall
    (
        DRV_HANDLE handle,
        const char *pFilename
    )

  Summary:
    Recall a configuration set.

  Description:
    Recall a flash file to the currently active configuration.

  Remarks:
    See wdrv_winc.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_CfgArchiveRecall
(
    DRV_HANDLE handle,
    const char *pFilename
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t lenFilename;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pFilename))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    lenFilename = strnlen(pFilename, WINC_CMD_PARAM_SZ_FS_FILENAME+1U);

    if (lenFilename > WINC_CMD_PARAM_SZ_FS_FILENAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wincCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdCFGCP(cmdReqHandle, WINC_TYPE_STRING, (uintptr_t)pFilename, lenFilename, WINC_TYPE_INTEGER_UNSIGNED, 0, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver General Implementation
// *****************************************************************************
// *****************************************************************************

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

    cmdReqHandle = WDRV_WINC_CmdReqInit(2, 0, wincCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdCFG(cmdReqHandle, WINC_CFG_PARAM_ID_CFG_DEBUG_PORT, WINC_TYPE_INTEGER_UNSIGNED, (uintptr_t)uartPort, 0);

    if (0U != uartBaudRate)
    {
        (void)WINC_CmdCFG(cmdReqHandle, WINC_CFG_PARAM_ID_CFG_DEBUG_BAUD, WINC_TYPE_INTEGER_UNSIGNED, uartBaudRate, 0);
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WINC_CMD_REQ_HANDLE WDRV_WINC_CmdReqInit
    (
        unsigned int numCommands,
        size_t extraDataLen,
        WINC_DEV_CMD_RSP_CB pfCmdRspCallback,
        uintptr_t cmdRspCallbackCtx
    )

  Summary:
    Initialise a command request.

  Description:
    Allocates memory and initialises a command request.

  Remarks:
    See wdrv_winc.h for usage information.

*/

WINC_CMD_REQ_HANDLE WDRV_WINC_CmdReqInit(unsigned int numCommands, size_t extraDataLen, WINC_DEV_CMD_RSP_CB pfCmdRspCallback, uintptr_t cmdRspCallbackCtx)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    size_t bufferSize = (128U*numCommands) + extraDataLen;

    pCmdReqBuffer = OSAL_Malloc(bufferSize);

    if (NULL == pCmdReqBuffer)
    {
        return WINC_CMD_REQ_INVALID_HANDLE;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, bufferSize, (int)numCommands, pfCmdRspCallback, cmdRspCallbackCtx);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WINC_CMD_REQ_INVALID_HANDLE;
    }

    return cmdReqHandle;
}

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_DevTransmitCmdReq
    (
        WINC_DEVICE_HANDLE devHandle,
        WINC_CMD_REQ_HANDLE cmdReqHandle
    )

  Summary:
    Transmits a command request.

  Description:
    Queues a command request for transmission to the WINCS02 device.

  Remarks:
    See wdrv_winc.h for usage information.

*/

bool WDRV_WINC_DevTransmitCmdReq(WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle)
{
    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return false;
    }

    if (false == WINC_DevTransmitCmdReq(devHandle, cmdReqHandle))
    {
        OSAL_Free((WINC_COMMAND_REQUEST*)cmdReqHandle);
        return false;
    }

    return true;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_DevDiscardCmdReq(WINC_CMD_REQ_HANDLE cmdReqHandle)

  Summary:
    Discards a command request.

  Description:
    Deallocates and discards a previously created command request.

  Remarks:
    See wdrv_winc.h for usage information.

*/

void WDRV_WINC_DevDiscardCmdReq(WINC_CMD_REQ_HANDLE cmdReqHandle)
{
    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return;
    }

    OSAL_Free((WINC_COMMAND_REQUEST*)cmdReqHandle);
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
        (void)OSAL_SEM_PostISR(&pDcpt->pCtrl->drvEventSemaphore);
    }
}
