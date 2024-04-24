/*******************************************************************************
  WINC Driver Network Interface Implementation

  File Name:
    wdrv_winc_netif.c

  Summary:
    WINC wireless driver network interface implementation.

  Description:
    This interface provides functionality required for network interface operations.
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
#include "wdrv_winc_netif.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Network Interface Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void netifProcessNETIFC
    (
        WDRV_WINC_DCPT *pDcpt,
        WINC_DEV_PARAM_ELEM *pCmdElems,
        WINC_DEV_PARAM_ELEM *pRspElems,
        uint16_t status
    )

  Summary:
    Processes responses to NETIFC commands.

  Description:
    Processes both command responses (only received when status is OK) and
    command status responses to response to the application through the information
    callback.

  Precondition:
    WINC_DevTransmitCmdReq must of been called to submit command request.

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
    WINC_DEV_PARAM_ELEM *pRspElems,
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

    WINC_CmdReadParamElem(&pCmdElems[0], WINC_TYPE_INTEGER, &ifIdx, sizeof(ifIdx));
    WINC_CmdReadParamElem(&pCmdElems[1], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

    switch (id.i)
    {
        case WINC_CFG_PARAM_ID_NETIF_ETHER:
        {
            if (NULL != pRspElems)
            {
                WINC_CmdReadParamElem(&pRspElems[1], WINC_TYPE_MACADDR, &info.macAddr, sizeof(info.macAddr));
                pInfoPtr = &info.macAddr;
            }

            infoType = WDRV_WINC_NETIF_INFO_MAC_ADDR;
            break;
        }

        default:
        {
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
    static void netifProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)

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

static void netifProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_CMD_ID_NETIFC:
        {
            WINC_DEV_PARAM_ELEM elems[10];

            if (pElems->numElems != 2)
            {
                break;
            }

            if (false == WINC_DevUnpackElements(pElems->srcCmd.numParams, pElems->srcCmd.pParams, elems))
            {
                break;
            }

            netifProcessNETIFC(pDcpt, elems, pElems->elems, WINC_STATUS_OK);
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
    static void netIfCmdRspCallbackHandler
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

static void netIfCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("NETIF CmdRspCB %08x Event %d\n", cmdReqHandle, event);

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
            /* Status response received for command. */
            WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (WINC_DEV_EVENT_STATUS_ARGS*)eventArg;
            WINC_DEV_PARAM_ELEM elems[10];

            if (NULL == pStatusInfo)
            {
                break;
            }

            if (WINC_STATUS_OK == pStatusInfo->status)
            {
                break;
            }

            switch (pStatusInfo->rspCmdId)
            {
                case WINC_CMD_ID_NETIFC:
                {
                    if (false == WINC_DevUnpackElements(pStatusInfo->srcCmd.numParams, pStatusInfo->srcCmd.pParams, elems))
                    {
                        break;
                    }

                    netifProcessNETIFC(pDcpt, elems, NULL, pStatusInfo->status);
                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                netifProcessCmdRsp((DRV_HANDLE)pDcpt, pRspElems);
            }
            break;
        }
    }
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
    void *pCmdReqBuffer;
    int netIfCDHCPEnable = 0;

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

    if (0 != (aacMode & WDRV_WINC_NETIF_AAC_V4_DHCP))
    {
        netIfCDHCPEnable = 1;
    }

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, netIfCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdNETIFC(cmdReqHandle, ifIdx, WINC_CFG_PARAM_ID_NETIF_DHCPC_EN, WINC_TYPE_INTEGER, netIfCDHCPEnable, 0);

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
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPAddrSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_IP_ADDRESS_TYPE type,
        WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
        int scope
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
    int scope
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    uint8_t addrScope[16+1];

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
    {
        if (scope > 31)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }
    else if (WDRV_WINC_IP_ADDRESS_TYPE_IPV6 == type)
    {
        if (scope > 127)
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

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, netIfCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (NULL != pAddr)
    {
        if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
        {
            uint32_t netMask = 0xffffffff;

            netMask <<= (32-scope);
            netMask = htonl(netMask);

            memcpy(&addrScope[0], pAddr, 4);
            memcpy(&addrScope[4], &netMask, 4);

            WINC_CmdNETIFC(cmdReqHandle, ifIdx, WINC_CFG_PARAM_ID_NETIF_IP_MASK, WINC_TYPE_IPV4ADDR, (uintptr_t)addrScope, 8);
        }
        else
        {
            memcpy(&addrScope[0], pAddr, 16);
            addrScope[16] = scope;

            WINC_CmdNETIFC(cmdReqHandle, ifIdx, WINC_CFG_PARAM_ID_NETIF_IPV6_GLO_ADDR, WINC_TYPE_IPV6ADDR, (uintptr_t)addrScope, 17);
        }
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
    void *pCmdReqBuffer;

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

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, netIfCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
    {
        WINC_CmdNETIFC(cmdReqHandle, ifIdx, WINC_CFG_PARAM_ID_NETIF_GATEWAY, WINC_TYPE_IPV4ADDR, (uintptr_t)pDest, sizeof(WDRV_WINC_IPV4_ADDR));
    }
    else
    {
        WINC_CmdNETIFC(cmdReqHandle, ifIdx, WINC_CFG_PARAM_ID_NETIF_IPV6_GATEWAY, WINC_TYPE_IPV6ADDR, (uintptr_t)pDest, sizeof(WDRV_WINC_IPV6_ADDR));
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

    if (NULL != pDcpt->pCtrl->pfNetIfInfoCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, netIfCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdNETIFC(cmdReqHandle, ifIdx, WINC_CFG_PARAM_ID_NETIF_ETHER, WINC_TYPE_INVALID, 0, 0);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfNetIfInfoCB = pfNetIfInfoCB;

    return WDRV_WINC_STATUS_OK;
}