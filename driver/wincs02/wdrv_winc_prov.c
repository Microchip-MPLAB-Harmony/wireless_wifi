/*******************************************************************************
  WINC Wireless Driver Provisioning Implementation

  File Name:
    wdrv_winc_prov.c

  Summary:
    WINC wireless driver provisioning implementation.

  Description:
    This interface provides functionality required for the provisioning service,
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
#include "wdrv_winc_prov.h"

#ifndef WDRV_WINC_MOD_DISABLE_PROV

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Provisioning Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void provCmdRspCallbackHandler
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

static void provCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)context;

    if (NULL == pDcpt)
    {
        return;
    }

    //WDRV_DBG_INFORM_PRINT("PROV CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("PROV CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_ProvProcessAEC
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
    See wdrv_winc_prov.h for usage information.

*/

void WDRV_WINC_ProvProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)context;
    WDRV_WINC_CTRLDCPT *pCtrl;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_WPROVAT:
        {
            WDRV_WINC_PROV_CLIENT_ID id;
            WDRV_WINC_IP_MULTI_TYPE_ADDRESS addr;

            if (3U != pElems->numElems)
            {
                break;
            }

            if (NULL != pCtrl->pfProvAttachCB)
            {
                (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &id, sizeof(id));

                if (WINC_TYPE_IPV4ADDR == pElems->elems[1].type)
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_IPV4ADDR, &addr.addr, sizeof(WDRV_WINC_IPV4_ADDR));
                    addr.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV4;
                }
                else if (WINC_TYPE_IPV6ADDR == pElems->elems[1].type)
                {
                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_IPV6ADDR, &addr.addr, sizeof(WDRV_WINC_IPV6_ADDR));
                    addr.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV6;
                }
                else
                {
                    break;
                }

                pCtrl->pfProvAttachCB((DRV_HANDLE)pDcpt, id, &addr);
            }

            break;
        }

        case WINC_AEC_ID_WPROVDT:
        {
            WDRV_WINC_PROV_CLIENT_ID id;

            if (1U != pElems->numElems)
            {
                break;
            }

            if (NULL != pCtrl->pfProvAttachCB)
            {
                (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &id, sizeof(id));

                pCtrl->pfProvAttachCB((DRV_HANDLE)pDcpt, id, NULL);
            }

            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("PROV AECCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_ProvServiceStart
    (
        DRV_HANDLE handle,
        WDRV_WINC_PROV_PROTOCOL_TYPE type,
        uint16_t port,
        WDRV_WINC_IP_ADDRESS_TYPE ipType,
        WDRV_WINC_PROV_ATTACH_CALLBACK pfProvAttachCB
    )

  Summary:
    Start the provisioning service.

  Description:
    Starts a provisioning service on the specified port and protocol.

  Remarks:
    See wdrv_winc_prov.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_ProvServiceStart
(
    DRV_HANDLE handle,
    WDRV_WINC_PROV_PROTOCOL_TYPE type,
    uint16_t port,
    WDRV_WINC_IP_ADDRESS_TYPE ipType,
    WDRV_WINC_PROV_ATTACH_CALLBACK pfProvAttachCB
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (NULL != pDcpt->pCtrl->pfProvAttachCB)
    {
        return WDRV_WINC_STATUS_BUSY;
    }

    if ((WDRV_WINC_PROV_PROTOCOL_BINARY != type) && (WDRV_WINC_PROV_PROTOCOL_ASCII != type))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (0U == port)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((WDRV_WINC_IP_ADDRESS_TYPE_IPV4 != ipType) && (WDRV_WINC_IP_ADDRESS_TYPE_IPV6 != ipType))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(4, 0, provCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWPROVC(cmdReqHandle, WINC_CFG_PARAM_ID_WPROV_PORT, WINC_TYPE_INTEGER_UNSIGNED, port, 0);
    (void)WINC_CmdWPROVC(cmdReqHandle, WINC_CFG_PARAM_ID_WPROV_ASCII, WINC_TYPE_BOOL, (WDRV_WINC_PROV_PROTOCOL_ASCII == type) ? 1U : 0U, 0);
    (void)WINC_CmdWPROVC(cmdReqHandle, WINC_CFG_PARAM_ID_WPROV_PROTOCOL_VERSION, WINC_TYPE_INTEGER_UNSIGNED, (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == ipType) ? 4U : 6U, 0);
    (void)WINC_CmdWPROV(cmdReqHandle, 1);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfProvAttachCB = pfProvAttachCB;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_ProvServiceStop
    (
        DRV_HANDLE handle
    )

  Summary:
    Stop the provisioning service.

  Description:
    Stops a provisioning service.

  Remarks:
    See wdrv_winc_prov.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_ProvServiceStop
(
    DRV_HANDLE handle
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, provCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWPROV(cmdReqHandle, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfProvAttachCB = NULL;

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_PROV */