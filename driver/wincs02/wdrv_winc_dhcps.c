/*******************************************************************************
  WINC Wireless Driver DHCP Server Source File

  File Name:
    wdrv_winc_dhcps.c

  Summary:
    WINC wireless driver DHCP server implementation.

  Description:
    This interface provides functionality required for DHCP server operations.
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
#include "wdrv_winc_dhcps.h"

#ifndef WDRV_WINC_MOD_DISABLE_DHCPS

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver DHCP Server Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void dhcpsCmdRspCallbackHandler
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

static void dhcpsCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("DHCPS CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
            /* Do nothing. */
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            /* Do nothing. */
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
// Section: WINC Driver DHCP Server Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DHCPSEnableSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_IDX poolIdx,
        bool enabled,
        WDRV_WINC_DHCPS_EVENT_HANDLER pfDHCPSEventCB
    )

  Summary:
    Set the enable state of the DHCP server.

  Description:
    Enables or disables the DHCP server.

  Remarks:
    See wdrv_winc_dhcps.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DHCPSEnableSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_IDX poolIdx,
    bool enabled,
    WDRV_WINC_DHCPS_EVENT_HANDLER pfDHCPSEventCB
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, dhcpsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDHCPSC(cmdReqHandle, (int32_t)poolIdx, WINC_CFG_PARAM_ID_DHCPS_ENABLED, WINC_TYPE_BOOL, (true == enabled) ? 1U : 0U, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfDHCPSEventCB = pfDHCPSEventCB;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DHCPSPoolAddressSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_IDX poolIdx,
        WDRV_WINC_IPV4_ADDR *pStartAddr
    )

  Summary:
    Sets the DHCP server pool start address.

  Description:
    Configures the starting IP address of the selected DHCP server pool.

  Remarks:
    See wdrv_winc_dhcps.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DHCPSPoolAddressSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_IDX poolIdx,
    WDRV_WINC_IPV4_ADDR *pStartAddr
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, dhcpsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDHCPSC(cmdReqHandle, (int32_t)poolIdx, WINC_CFG_PARAM_ID_DHCPS_POOL_START, WINC_TYPE_IPV4ADDR, (uintptr_t)pStartAddr, sizeof(WDRV_WINC_IPV4_ADDR));

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DHCPSGatewaySet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_IDX poolIdx,
        WDRV_WINC_IPV4_ADDR *pGWAddr
    )

  Summary:
    Sets the DHCP server pool default gateway address.

  Description:
    Configures the default gateway IP address of a DHCP server pool.

  Remarks:
    See wdrv_winc_dhcps.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DHCPSGatewaySet
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_IDX poolIdx,
    WDRV_WINC_IPV4_ADDR *pGWAddr
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, dhcpsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDHCPSC(cmdReqHandle, (int32_t)poolIdx, WINC_CFG_PARAM_ID_DHCPS_GATEWAY, WINC_TYPE_IPV4ADDR, (uintptr_t)pGWAddr, sizeof(WDRV_WINC_IPV4_ADDR));

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DHCPSNetIfBind
    (
        DRV_HANDLE handle,
        WDRV_WINC_DHCPS_IDX poolIdx,
        WDRV_WINC_NETIF_IDX ifIdx
    )

  Summary:
    Sets the network interface index of the DHCP server pool.

  Description:
    Assigns a DHCP server pool to the specified network interface.

  Remarks:
    See wdrv_winc_dhcps.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DHCPSNetIfBind
(
    DRV_HANDLE handle,
    WDRV_WINC_DHCPS_IDX poolIdx,
    WDRV_WINC_NETIF_IDX ifIdx
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, dhcpsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDHCPSC(cmdReqHandle, (int32_t)poolIdx, WINC_CFG_PARAM_ID_DHCPS_NETIF_IDX, WINC_TYPE_INTEGER, (uintptr_t)ifIdx, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_DHCPS */
