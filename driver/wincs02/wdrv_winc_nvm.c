/*******************************************************************************
  WINC Wireless Driver NVM Implementation

  File Name:
    wdrv_winc_nvm.c

  Summary:
    WINC wireless driver NVM implementation.

  Description:
    This interface provides functionality required for the NVM service.
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
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_nvm.h"

#ifndef WDRV_WINC_MOD_DISABLE_NVM

static void nvmCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver NVM Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void nvmReportStatus
    (
        WDRV_WINC_DCPT *pDcpt,
        WDRV_WINC_NVM_STATUS_TYPE status,
        uintptr_t opStatusInfo
    )

  Summary:
    Report NVM status.

  Description:
    Report NVM status and optional information to complete an operation.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.
    NVM operation must have been started.

  Parameters:
    pDcpt        - Pointer to WINC device descriptor.
    status       - Status to be reported via callback.
    opStatusInfo - Optional status information to be passed to callback.

  Returns:
    None.

  Remarks:
    None.

*/

static void nvmReportStatus
(
    WDRV_WINC_DCPT *pDcpt,
    WDRV_WINC_NVM_STATUS_TYPE status,
    uintptr_t opStatusInfo
)
{
    WDRV_WINC_NVM_OPERATION_TYPE operation;
    WDRV_WINC_NVM_STATUS_CALLBACK pfOperationStatusCB;

    if (NULL == pDcpt)
    {
        return;
    }

    operation           = pDcpt->pCtrl->nvmState.operation;
    pfOperationStatusCB = pDcpt->pCtrl->nvmState.pfOperationStatusCB;

    pDcpt->pCtrl->nvmState.pfOperationStatusCB = NULL;
    pDcpt->pCtrl->nvmState.operation           = WDRV_WINC_NVM_OPERATION_NONE;

    if (NULL != pfOperationStatusCB)
    {
        pfOperationStatusCB((DRV_HANDLE)pDcpt, operation, status, opStatusInfo);
    }
}

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_STATUS nvmWriteBuffer
    (
        WDRV_WINC_DCPT *pDcpt,
        const void *const pBuffer,
        uint32_t offset,
        uint32_t length
    )

  Summary:
    Write NVM data.

  Description:
    Write the buffer provided to the offset in the NVM partition.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    pDcpt   - Pointer to WINC device descriptor.
    pBuffer - Pointer to data buffer to write.
    offset  - Offset within NVM partition to write data to.
    length  - Length of data to write.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

static WDRV_WINC_STATUS nvmWriteBuffer
(
    WDRV_WINC_DCPT *pDcpt,
    const void *const pBuffer,
    uint32_t offset,
    uint32_t length
)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    uint32_t sectorMask;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pBuffer))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((offset+length) > WINC_CMD_PARAM_MAX_NVM_OFFSET)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    sectorMask = ~((uint32_t)pDcpt->pCtrl->nvmState.geom.sector.size-1U);

    if (((offset+length) & sectorMask) != (offset & sectorMask))
    {
        length = ((offset+length) & sectorMask) - offset;
    }

    if (length > WINC_CMD_PARAM_MAX_NVM_LENGTH)
    {
        length = WINC_CMD_PARAM_MAX_NVM_LENGTH;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, length, nvmCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdNVMWR(cmdReqHandle, offset, (uint16_t)length, pBuffer, length);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_STATUS nvmReadBuffer
    (
        WDRV_WINC_DCPT *pDcpt,
        uint32_t offset,
        uint32_t length
    )

  Summary:
    Read NVM data.

  Description:
    Read the data from the offset in the NVM partition.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    pDcpt  - Pointer to WINC device descriptor.
    offset - Offset within NVM partition to read data from.
    length - Length of data to read.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

static WDRV_WINC_STATUS nvmReadBuffer
(
    WDRV_WINC_DCPT *pDcpt,
    uint32_t offset,
    uint32_t length
)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (length > WINC_CMD_PARAM_MAX_NVM_LENGTH)
    {
        length = WINC_CMD_PARAM_MAX_NVM_LENGTH;
    }

    if ((offset > WINC_CMD_PARAM_MAX_NVM_OFFSET) || ((offset+length) > WINC_CMD_PARAM_MAX_NVM_OFFSET))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, nvmCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdNVMRD(cmdReqHandle, offset, (uint16_t)length);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    static void nvmProcessCmdRsp(DRV_HANDLE handle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)

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

static void nvmProcessCmdRsp(DRV_HANDLE handle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_CMD_ID_NVMRD:
        {
            WDRV_WINC_NVM_READ_STATUS_INFO opStatusInfo;

            if (3U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_UNSIGNED, &opStatusInfo.offset, sizeof(opStatusInfo.offset));
            (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &opStatusInfo.length, sizeof(opStatusInfo.length));

            opStatusInfo.pData = pElems->elems[2].pData;

            if (NULL == pDcpt->pCtrl->nvmState.pBuffer)
            {
                WDRV_WINC_NVM_STATUS_CALLBACK pfOperationStatusCB = pDcpt->pCtrl->nvmState.pfOperationStatusCB;

                nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_SUCCESS, (uintptr_t)&opStatusInfo);

                pDcpt->pCtrl->nvmState.pfOperationStatusCB = pfOperationStatusCB;

                pDcpt->pCtrl->nvmState.length -= opStatusInfo.length;
                pDcpt->pCtrl->nvmState.offset += opStatusInfo.length;

                if (pDcpt->pCtrl->nvmState.length > 0U)
                {
                    if (WDRV_WINC_STATUS_OK != nvmReadBuffer(pDcpt,
                                                             pDcpt->pCtrl->nvmState.offset,
                                                             pDcpt->pCtrl->nvmState.length))
                    {
                        nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_ERROR, 0);
                    }
                    else
                    {
                        pDcpt->pCtrl->nvmState.operation = WDRV_WINC_NVM_OPERATION_READ;
                    }
                }
            }
            else
            {
                (void)memcpy(&pDcpt->pCtrl->nvmState.pBuffer[opStatusInfo.offset], opStatusInfo.pData, opStatusInfo.length);

                if ((opStatusInfo.offset+opStatusInfo.length) == (pDcpt->pCtrl->nvmState.offset+pDcpt->pCtrl->nvmState.length))
                {
                    opStatusInfo.offset = pDcpt->pCtrl->nvmState.offset;
                    opStatusInfo.length = pDcpt->pCtrl->nvmState.length;
                    opStatusInfo.pData  = pDcpt->pCtrl->nvmState.pBuffer;

                    nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_SUCCESS, (uintptr_t)&opStatusInfo);
                }
                else
                {
                    if (WDRV_WINC_STATUS_OK != nvmReadBuffer(pDcpt,
                                                            opStatusInfo.offset+opStatusInfo.length,
                                                            (pDcpt->pCtrl->nvmState.offset+pDcpt->pCtrl->nvmState.length)-(opStatusInfo.offset+opStatusInfo.length)))
                    {
                        nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_ERROR, 0);
                    }
                }
            }

            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("NVM CmdRspCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void nvmCmdRspCallbackHandler
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

static void nvmCmdRspCallbackHandler
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

    //WDRV_DBG_INFORM_PRINT("NVM CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
            /* Status response received for command. */

            const WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (const WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (NULL != pStatusInfo)
            {
                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_NVMER:
                    {
                        if (WINC_STATUS_OK != pStatusInfo->status)
                        {
                            nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_ERROR, 0);
                        }

                        break;
                    }

                    case WINC_CMD_ID_NVMWR:
                    {
                        if (WINC_STATUS_OK != pStatusInfo->status)
                        {
                            nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_ERROR, 0);
                        }
                        else
                        {
                            WINC_DEV_PARAM_ELEM elems[10];
                            uint32_t length;

                            if (3U != pStatusInfo->srcCmd.numParams)
                            {
                                break;
                            }

                            if (false == WINC_DevUnpackElements(pStatusInfo->srcCmd.numParams, pStatusInfo->srcCmd.pParams, elems))
                            {
                                break;
                            }

                            (void)WINC_CmdReadParamElem(&elems[1], WINC_TYPE_INTEGER_UNSIGNED, &length, sizeof(length));

                            pDcpt->pCtrl->nvmState.length -= length;

                            if (0U == pDcpt->pCtrl->nvmState.length)
                            {
                                nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_SUCCESS, 0);
                            }
                            else
                            {
                                pDcpt->pCtrl->nvmState.offset  += length;
                                pDcpt->pCtrl->nvmState.pBuffer += length;

                                if (WDRV_WINC_STATUS_OK != nvmWriteBuffer(pDcpt,
                                                                          pDcpt->pCtrl->nvmState.pBuffer,
                                                                          pDcpt->pCtrl->nvmState.offset,
                                                                          pDcpt->pCtrl->nvmState.length))
                                {
                                    nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_ERROR, 0);
                                }
                            }
                        }

                        break;
                    }

                    case WINC_CMD_ID_NVMCHK:
                    {
                        if (WINC_STATUS_OK != pStatusInfo->status)
                        {
                            nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_ERROR, 0);
                        }
                        break;
                    }

                    case WINC_CMD_ID_NVMRD:
                    {
                        if (WINC_STATUS_OK != pStatusInfo->status)
                        {
                            nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_ERROR, 0);
                        }

                        break;
                    }

                    default:
                    {
                        WDRV_DBG_VERBOSE_PRINT("NVM CmdRspCB %08x ID %04x status %04x not handled\r\n", cmdReqHandle, pStatusInfo->rspCmdId, pStatusInfo->status);
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
                nvmProcessCmdRsp((DRV_HANDLE)pDcpt, pRspElems);
            }

            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("NVM CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_STATUS nvmInProgress(WDRV_WINC_DCPT *const pDcpt)

  Summary:
    Check if NVM operation is in progress.

  Description:
    Checks if an NVM operation is in progress and other handle/pointer validation.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    pDcpt - Descriptor pointer obtained from WDRV_WINC_Open handle.

  Returns:
    WDRV_WINC_STATUS_OK            - There is no NVM operation in progress.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.

  Remarks:
    None.

*/

static WDRV_WINC_STATUS nvmInProgress(WDRV_WINC_DCPT *const pDcpt)
{
    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == (DRV_HANDLE)pDcpt) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Ensure an update is not in progress. */
    if (WDRV_WINC_NVM_OPERATION_NONE != pDcpt->pCtrl->nvmState.operation)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_NVMProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        const WINC_DEV_EVENT_RSP_ELEMS *const pElems
    )

  Summary:
    AEC process callback.

  Description:
    Callback will be called to process any AEC messages received.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/

void WDRV_WINC_NVMProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)context;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_NVMER:
        {
            nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_SUCCESS, 0);
            break;
        }

        case WINC_AEC_ID_NVMCHK:
        {
            WDRV_WINC_NVM_CHK_STATUS_INFO opStatusInfo;

            if (4U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_UNSIGNED, &opStatusInfo.offset, sizeof(opStatusInfo.offset));
            (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &opStatusInfo.length, sizeof(opStatusInfo.length));
            (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER_UNSIGNED, &opStatusInfo.mode, sizeof(opStatusInfo.mode));

            if (WINC_CONST_NVM_CHECK_MODE_CRC16 == (unsigned)opStatusInfo.mode)
            {
                (void)WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_INTEGER_UNSIGNED, &opStatusInfo.crc16, sizeof(opStatusInfo.crc16));
            }
            else
            {
                opStatusInfo.hash.l = (uint8_t)pElems->elems[3].length;
                opStatusInfo.hash.p = pElems->elems[3].pData;
            }

            nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_SUCCESS, (uintptr_t)&opStatusInfo);
            break;
        }

        case WINC_AEC_ID_NVMERR:
        {
            nvmReportStatus(pDcpt, WDRV_WINC_NVM_STATUS_ERROR, 0);
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("NVM AECCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    const WDRV_WINC_NVM_GEOM_INFO* const WDRV_WINC_NVMGeometryGet(DRV_HANDLE handle)

  Summary:
    Returns the NVM geometry.

  Description:
    Returns information on the geometry (sector size, number of sectors, etc)
      of the NVM region.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/

const WDRV_WINC_NVM_GEOM_INFO* const WDRV_WINC_NVMGeometryGet(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return NULL;
    }

    return &pDcpt->pCtrl->nvmState.geom;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMEraseSector
    (
        DRV_HANDLE handle,
        uint8_t startSector,
        uint8_t numSectors,
        WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Erase a sector.

  Description:
    Erases a number of sectors.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_NVMEraseSector
(
    DRV_HANDLE handle,
    uint8_t startSector,
    uint8_t numSectors,
    WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;

    /* Ensure the driver is open and no NVM operation is in progress. */
    status = nvmInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    if ((startSector > (pDcpt->pCtrl->nvmState.geom.sector.number-1U)) || (((uint16_t)startSector+numSectors) > pDcpt->pCtrl->nvmState.geom.sector.number))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, nvmCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdNVMER(cmdReqHandle, startSector, numSectors);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Set in progress flag and callback. */
    pDcpt->pCtrl->nvmState.operation           = WDRV_WINC_NVM_OPERATION_ERASE;
    pDcpt->pCtrl->nvmState.pfOperationStatusCB = pfUpdateStatusCB;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMWrite
    (
        DRV_HANDLE handle,
        void *pBuffer,
        uint32_t offset,
        uint32_t size,
        WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Writes data to the NVM.

  Description:
    Writes a number of bytes to the NVM.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_NVMWrite
(
    DRV_HANDLE handle,
    void *pBuffer,
    uint32_t offset,
    uint32_t length,
    WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WDRV_WINC_STATUS status;

    /* Ensure the driver is open and no NVM operation is in progress. */
    status = nvmInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    /* Ensure the user pointer is valid. */
    if (NULL == pBuffer)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    status = nvmWriteBuffer(pDcpt, pBuffer, offset, length);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    /* Set in progress flag and callback. */
    pDcpt->pCtrl->nvmState.operation           = WDRV_WINC_NVM_OPERATION_WRITE;
    pDcpt->pCtrl->nvmState.pfOperationStatusCB = pfUpdateStatusCB;
    pDcpt->pCtrl->nvmState.pBuffer             = pBuffer;
    pDcpt->pCtrl->nvmState.offset              = offset;
    pDcpt->pCtrl->nvmState.length              = length;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMRead
    (
        DRV_HANDLE handle,
        void *pBuffer,
        uint32_t offset,
        uint32_t length,
        WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Reads data from the NVM.

  Description:
    Reads a number of bytes from the NVM.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_NVMRead
(
    DRV_HANDLE handle,
    void *pBuffer,
    uint32_t offset,
    uint32_t length,
    WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WDRV_WINC_STATUS status;

    /* Ensure the driver is open and no NVM operation is in progress. */
    status = nvmInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    status = nvmReadBuffer(pDcpt, offset, length);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    /* Set in progress flag and callback. */
    pDcpt->pCtrl->nvmState.operation           = WDRV_WINC_NVM_OPERATION_READ;
    pDcpt->pCtrl->nvmState.pfOperationStatusCB = pfUpdateStatusCB;
    pDcpt->pCtrl->nvmState.pBuffer             = pBuffer;
    pDcpt->pCtrl->nvmState.offset              = offset;
    pDcpt->pCtrl->nvmState.length              = length;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMCheck
    (
        DRV_HANDLE handle,
        uint32_t offset,
        uint32_t size,
        WDRV_WINC_NVM_CHECK_MODE_TYPE mode,
        WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Checks data from the NVM.

  Description:
    Performs a CRC/hash check of the number of bytes from the NVM.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_NVMCheck
(
    DRV_HANDLE handle,
    uint32_t offset,
    uint32_t size,
    WDRV_WINC_NVM_CHECK_MODE_TYPE mode,
    WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WDRV_WINC_STATUS status;

    /* Ensure the driver is open and no NVM operation is in progress. */
    status = nvmInProgress(pDcpt);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(2, 0, nvmCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdNVMC(cmdReqHandle, WINC_CFG_PARAM_ID_NVM_CHECK_MODE, WINC_TYPE_INTEGER_UNSIGNED, (uintptr_t)mode, 0);
    (void)WINC_CmdNVMCHK(cmdReqHandle, offset, size);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Set in progress flag and callback. */
    pDcpt->pCtrl->nvmState.operation           = WDRV_WINC_NVM_OPERATION_CHECK;
    pDcpt->pCtrl->nvmState.pfOperationStatusCB = pfUpdateStatusCB;

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_NVM */
