/*******************************************************************************
  WINC Wireless Driver Network Interface Source File

  File Name:
    wdrv_winc_netif.c

  Summary:
    WINC wireless driver network interface implementation.

  Description:
    This interface provides functionality required for network interface
    operations.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_netif.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Network Interface Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void netifProcessNETIFC
    (
        WDRV_WINC_DCPT *pDcpt,
        WINC_DEV_PARAM_ELEM *pCmdElems,
        const WINC_DEV_PARAM_ELEM *const pRspElems,
        uint16_t status
    )

  Summary:
    Processes responses to NETIFC commands.

  Description:
    Processes both command responses (only received when status is OK) and
    command status responses to response to the application through the information
    callback.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    pDcpt     - Pointer to WINC driver descriptor.
    pCmdElems - Pointer to command parameters.
    pRspElems - Pointer to response elements.
    status    - Status of operation.

  Returns:
    None.

  Remarks:
    For command responses, the ultimate status is OK, so the returned information
    is passed to the application via the callback as successful.

    For command status responses, this is only called when the status is not OK,
    then the application can be sent an error status via the callback.
    pRspElems will be NULL in this case.
    A status response of OK will have previously been sent a command response which
    would have been passed to the application via the command response path above.

*/

static void netifProcessNETIFC
(
    WDRV_WINC_DCPT *pDcpt,
    WINC_DEV_PARAM_ELEM *pCmdElems,
    const WINC_DEV_PARAM_ELEM *const pRspElems,
    uint16_t status
)
{
    WINC_DEV_FRACT_INT_TYPE id;
    WDRV_WINC_NETIF_IDX ifIdx;
    WDRV_WINC_NETIF_INFO_TYPE infoType = WDRV_WINC_NETIF_INFO_INVALID;
    void *pInfoPtr = NULL;
    union
    {
        uint8_t macAddr[WDRV_WINC_MAC_ADDR_LEN];
    } info;

    if ((NULL == pDcpt) || (NULL == pCmdElems))
    {
        return;
    }

    if (NULL == pDcpt->pCtrl->pfNetIfInfoCB)
    {
        return;
    }

    (void)WINC_CmdReadParamElem(&pCmdElems[0], WINC_TYPE_INTEGER, &ifIdx, sizeof(ifIdx));
    (void)WINC_CmdReadParamElem(&pCmdElems[1], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

    switch (id.i)
    {
        case WINC_CFG_PARAM_ID_NETIF_ETHER:
        {
            if (NULL != pRspElems)
            {
                (void)WINC_CmdReadParamElem(&pRspElems[1], WINC_TYPE_MACADDR, &info.macAddr, sizeof(info.macAddr));
                pInfoPtr = &info.macAddr;
            }

            infoType = WDRV_WINC_NETIF_INFO_MAC_ADDR;
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("NETIFC ID %d not handled\r\n", id.i);
            break;
        }
    }

    if (WDRV_WINC_NETIF_INFO_INVALID != infoType)
    {
        pDcpt->pCtrl->pfNetIfInfoCB((DRV_HANDLE)pDcpt, ifIdx, infoType, pInfoPtr, (WINC_STATUS_OK == status) ? WDRV_WINC_STATUS_OK : WDRV_WINC_STATUS_REQUEST_ERROR);
        pDcpt->pCtrl->pfNetIfInfoCB = NULL;
    }
}

//*******************************************************************************
/*
  Function:
    static void netifProcessStatus
    (
        WDRV_WINC_DCPT *pDcpt,
        uint16_t cmdID,
        WINC_CMD_REQ_HANDLE cmdReqHandle,
        const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
        uint16_t statusCode
    )

  Summary:
    Process command status responses.

  Description:
    Processes command status responses received via WINC_DEV_CMDREQ_EVENT_CMD_STATUS events.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    pDcpt        - Pointer to device descriptor.
    cmdID        - Command ID.
    cmdReqHandle - Command request handle.
    pSrcCmd      - Pointer to source command.
    statusCode   - Status code.

  Returns:
    None.

  Remarks:
    None.

*/

static void netifProcessStatus
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t cmdID,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    uint16_t statusCode
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pSrcCmd))
    {
        return;
    }

    if (WINC_STATUS_OK == statusCode)
    {
        return;
    }

    switch (cmdID)
    {
        case WINC_CMD_ID_NETIFC:
        {
            WINC_DEV_PARAM_ELEM elems[10];

            if (false == WINC_DevUnpackElements(pSrcCmd->numParams, pSrcCmd->pParams, elems))
            {
                break;
            }

            netifProcessNETIFC(pDcpt, elems, NULL, statusCode);
            break;
        }

        default:
        {
            /* Do nothing. */
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void netifProcessCmdRsp
    (
        WDRV_WINC_DCPT *pDcpt,
        uint16_t rspId,
        WINC_CMD_REQ_HANDLE cmdReqHandle,
        const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
        int numElems,
        const WINC_DEV_PARAM_ELEM *const pElems
    )

  Summary:
    Process command responses.

  Description:
    Processes command responses received via WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED events.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    pDcpt        - Pointer to device descriptor.
    rspId        - Response command ID.
    cmdReqHandle - Command request handle.
    pSrcCmd      - Pointer to source command.
    numElems     - Number of elements in response.
    pElems       - Pointer to response elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void netifProcessCmdRsp
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t rspId,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems) || (NULL == pSrcCmd))
    {
        return;
    }

    switch (rspId)
    {
        case WINC_CMD_ID_NETIFC:
        {
            WINC_DEV_PARAM_ELEM elems[10];

            if (2U != numElems)
            {
                break;
            }

            if (false == WINC_DevUnpackElements(pSrcCmd->numParams, pSrcCmd->pParams, elems))
            {
                break;
            }

            netifProcessNETIFC(pDcpt, elems, pElems, WINC_STATUS_OK);
            break;
        }

        default:
        {
            /* Do nothing. */
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void netifProcessAEC
    (
        WDRV_WINC_DCPT *pDcpt,
        uint16_t aecId,
        int numElems,
        const WINC_DEV_PARAM_ELEM *const pElems
    )

  Summary:
    Process AECs.

  Description:
    Processes AECs for this module.

  Precondition:
    None.

  Parameters:
    pDcpt    - Pointer to device descriptor.
    aecId    - AEC ID.
    numElems - Number of elements.
    pElems   - Pointer to elements.

  Returns:
    None.

  Remarks:
    None.

*/

static void netifProcessAEC
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t aecId,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    WDRV_WINC_CTRLDCPT *pCtrl;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (aecId)
    {
        case WINC_AEC_ID_NETIFRX:
        {
            if (3U != numElems)
            {
                break;
            }

            if (NULL != pCtrl->pfL2DataMonitorCB)
            {
                WDRV_WINC_NETIF_IDX ifIdx;

                (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &ifIdx, sizeof(ifIdx));

                pCtrl->pfL2DataMonitorCB((DRV_HANDLE)pDcpt, ifIdx, pElems[2].pData, pElems[2].length);
            }

            break;
        }

        default:
        {
            /* Do nothing. */
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void netifCmdRspCallbackHandler
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

static void netifCmdRspCallbackHandler
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    WINC_DEV_CMDREQ_EVENT_TYPE event,
    uintptr_t eventArg
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT*)context;

    if (NULL == pDcpt)
    {
        return;
    }

//    WDRV_DBG_INFORM_PRINT("NETIF CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
                netifProcessStatus(pDcpt, pStatusInfo->rspCmdId, cmdReqHandle, &pStatusInfo->srcCmd, pStatusInfo->status);
            }

            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                netifProcessCmdRsp(pDcpt, pRspElems->rspId, cmdReqHandle, &pRspElems->srcCmd, pRspElems->numElems, pRspElems->elems);
            }

            break;
        }

        default:
        {
            /* Do nothing. */
            break;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Network Interface Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_NETIFProcessAEC
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
    See wdrv_winc_netif.h for usage information.

*/

void WDRV_WINC_NETIFProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)context;

    if ((NULL == pDcpt) || (NULL == pElems))
    {
        return;
    }

    netifProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfRegisterEventCallback
    (
        DRV_HANDLE handle,
        const WDRV_WINC_NETIF_EVENT_HANDLER pfNetIfEventCallback
    )

  Summary:
    Register event callback.

  Description:
    Function to register an event callback function.

  Remarks:
    See wdrv_winc_netif.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfRegisterEventCallback
(
    DRV_HANDLE handle,
    const WDRV_WINC_NETIF_EVENT_HANDLER pfNetIfEventCallback
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

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

    pDcpt->pCtrl->pfNetIfEventCB = pfNetIfEventCallback;

    return WDRV_WINC_STATUS_OK;
}

#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPAutoConfModeSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_NETIF_AAC_TYPE aacMode
    )

  Summary:
    Set IP address auto-configuration mode.

  Description:
    Sets how the IP address of the network interface can be assigned.

  Remarks:
    See wdrv_winc_netif.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfIPAutoConfModeSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_NETIF_AAC_TYPE aacMode
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    uintptr_t netIfCDHCPEnable = 0;

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

    if (WDRV_WINC_NETIF_AAC_NONE != (aacMode & WDRV_WINC_NETIF_AAC_V4_DHCP))
    {
        netIfCDHCPEnable = 1;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, netifCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_DHCPC_EN, WINC_TYPE_INTEGER, netIfCDHCPEnable, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPAddrSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_IP_ADDRESS_TYPE type,
        WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
        unsigned int scope
    )

  Summary:
    Set the IP address of a network interface.

  Description:
    Defines the network IP address and scope of an interface.

  Remarks:
    See wdrv_winc_netif.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfIPAddrSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_IP_ADDRESS_TYPE type,
    WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
    unsigned int scope
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    uint8_t addrScope[16+1];

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
    {
        if (scope > 31U)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }
    else if (WDRV_WINC_IP_ADDRESS_TYPE_IPV6 == type)
    {
        if (scope > 127U)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }
    else
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, netifCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (NULL != pAddr)
    {
        if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
        {
            uint32_t netMask = 0xffffffffU;

            netMask <<= (32U-scope);
            netMask = WDRV_WINC_UtilsHToNL(netMask);

            (void)memcpy(&addrScope[0], (uint8_t*)pAddr, 4);
            (void)memcpy(&addrScope[4], (uint8_t*)&netMask, 4);

            (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_IP_MASK, WINC_TYPE_IPV4ADDR, (uintptr_t)addrScope, 8);
        }
        else
        {
            (void)memcpy(&addrScope[0], (uint8_t*)pAddr, 16);
            addrScope[16] = (uint8_t)scope;

            (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_IPV6_GLO_ADDR, WINC_TYPE_IPV6ADDR, (uintptr_t)addrScope, 17);
        }
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPRouteSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_IP_ADDRESS_TYPE type,
        WDRV_WINC_IP_MULTI_ADDRESS *pNetwork,
        int scope,
        WDRV_WINC_IP_MULTI_ADDRESS *pDest
    )

  Summary:
    Set a route on a network interface.

  Description:
    Defines a route to a network interface.

  Remarks:
    See wdrv_winc_netif.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfIPRouteSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_IP_ADDRESS_TYPE type,
    WDRV_WINC_IP_MULTI_ADDRESS *pNetwork,
    int scope,
    WDRV_WINC_IP_MULTI_ADDRESS *pDest
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
    {
    }
    else if (WDRV_WINC_IP_ADDRESS_TYPE_IPV6 == type)
    {
    }
    else
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, netifCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
    {
        (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_GATEWAY, WINC_TYPE_IPV4ADDR, (uintptr_t)pDest, sizeof(WDRV_WINC_IPV4_ADDR));
    }
    else
    {
        (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_IPV6_GATEWAY, WINC_TYPE_IPV6ADDR, (uintptr_t)pDest, sizeof(WDRV_WINC_IPV6_ADDR));
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
#endif /* WDRV_WINC_DISABLE_L3_SUPPORT */

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfMACAddrGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB
    )

  Summary:
    Get the MAC address of a network interface.

  Description:
    Requests the MAC address of a network interface, to be return via a callback.

  Remarks:
    See wdrv_winc_netif.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfMACAddrGet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const )handle;
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

    if (NULL != pDcpt->pCtrl->pfNetIfInfoCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, netifCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_ETHER, WINC_TYPE_INVALID, 0, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfNetIfInfoCB = pfNetIfInfoCB;

    return WDRV_WINC_STATUS_OK;
}
