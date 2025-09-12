/*******************************************************************************
  WINC Wireless Driver Socket Mode Source File

  File Name:
    wdrv_winc_socket.c

  Summary:
    WINC wireless driver socket mode implementation.

  Description:
    This interface provides extra functionality required for socket mode
    operation.
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
#include "wdrv_winc_socket.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Socket Mode Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void icmpProcessStatus
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

static void icmpProcessStatus
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

    switch (cmdID)
    {
        case WINC_CMD_ID_PING:
        {
            if (WINC_STATUS_OK != statusCode)
            {
                if (NULL != pDcpt->pCtrl->pfICMPEchoResponseCB)
                {
                    pDcpt->pCtrl->pfICMPEchoResponseCB((DRV_HANDLE)pDcpt, NULL, WDRV_WINC_IP_ADDRESS_TYPE_ANY, 0);
                    pDcpt->pCtrl->pfICMPEchoResponseCB = NULL;
                }
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
    static void icmpProcessAEC
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

static void icmpProcessAEC
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t aecId,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (aecId)
    {
        case WINC_AEC_ID_PING:
        {
            uint16_t rtt;

            if (2U != numElems)
            {
                break;
            }

            if (NULL != pDcpt->pCtrl->pfICMPEchoResponseCB)
            {
                WDRV_WINC_IP_MULTI_ADDRESS ipAddr;
                WDRV_WINC_IP_ADDRESS_TYPE ipAddrType;
                WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB;

                if ((WINC_TYPE_IPV4ADDR == pElems[0].type) && (pElems[0].length <= sizeof(WDRV_WINC_IPV4_ADDR)))
                {
                    (void)memcpy(&ipAddr.v4.v, pElems[0].pData, sizeof(WDRV_WINC_IPV4_ADDR));
                    ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_IPV4;
                }
                else if ((WINC_TYPE_IPV6ADDR == pElems[0].type) && (pElems[0].length <= sizeof(WDRV_WINC_IPV6_ADDR)))
                {
                    (void)memcpy(&ipAddr.v6.v, pElems[0].pData, sizeof(WDRV_WINC_IPV6_ADDR));
                    ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_IPV6;
                }
                else
                {
                    ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_ANY;
                }

                (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &rtt, sizeof(rtt));

                pfICMPEchoResponseCB = pDcpt->pCtrl->pfICMPEchoResponseCB;

                pDcpt->pCtrl->pfICMPEchoResponseCB = NULL;

                if (WDRV_WINC_IP_ADDRESS_TYPE_ANY == ipAddrType)
                {
                    pfICMPEchoResponseCB((DRV_HANDLE)pDcpt, NULL, ipAddrType, rtt);
                }
                else
                {
                    pfICMPEchoResponseCB((DRV_HANDLE)pDcpt, &ipAddr, ipAddrType, rtt);
                }
            }

            break;
        }

        case WINC_AEC_ID_PINGERR:
        {
            if (NULL != pDcpt->pCtrl->pfICMPEchoResponseCB)
            {
                WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB = pDcpt->pCtrl->pfICMPEchoResponseCB;

                pDcpt->pCtrl->pfICMPEchoResponseCB = NULL;

                pfICMPEchoResponseCB((DRV_HANDLE)pDcpt, NULL, WDRV_WINC_IP_ADDRESS_TYPE_ANY, 0);
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
    static void icmpCmdRspCallbackHandler
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

static void icmpCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("ICMP CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
                icmpProcessStatus(pDcpt, pStatusInfo->rspCmdId, cmdReqHandle, &pStatusInfo->srcCmd, pStatusInfo->status);
            }

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
// Section: WINC Driver Socket Mode Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_ICMPProcessAEC
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
    See wdrv_winc_socket.h for usage information.

*/

void WDRV_WINC_ICMPProcessAEC
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

    icmpProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
}

#ifdef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SocketRegisterEventCallback
    (
        DRV_HANDLE handle,
        const WINC_SOCKET_EVENT_CALLBACK pfSocketEventCb
    )

  Summary:
    Register an event callback for socket events.

  Description:
    Socket events are dispatched to the application via this callback.

  Remarks:
    See wdrv_winc_socket.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_SocketRegisterEventCallback
(
    DRV_HANDLE handle,
    const WINC_SOCKET_EVENT_CALLBACK pfSocketEventCb
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (false == WINC_SockRegisterEventCallback(pDcpt->pCtrl->wincDevHandle, pfSocketEventCb, (uintptr_t)pDcpt))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
#endif

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_ICMPEchoRequestAddr
    (
        DRV_HANDLE handle,
        const WDRV_WINC_IP_MULTI_ADDRESS *const pIPAddr,
        WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
        const WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB
    )

  Summary:
    Send an ICMP echo request.

  Description:
    Sends an ICMP echo request to the address specified.

  Remarks:
    See wdrv_winc_socket.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_ICMPEchoRequestAddr
(
    DRV_HANDLE handle,
    const WDRV_WINC_IP_MULTI_ADDRESS *const pIPAddr,
    WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
    const WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    WINC_TYPE typeTargetAddr;
    size_t lenTargetAddr;

    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pIPAddr))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == ipAddrType)
    {
        typeTargetAddr = WINC_TYPE_IPV4ADDR;
        lenTargetAddr  = sizeof(WDRV_WINC_IPV4_ADDR);
    }
    else if (WDRV_WINC_IP_ADDRESS_TYPE_IPV6 == ipAddrType)
    {
        typeTargetAddr = WINC_TYPE_IPV6ADDR;
        lenTargetAddr  = sizeof(WDRV_WINC_IPV6_ADDR);
    }
    else
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, lenTargetAddr, icmpCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdPING(cmdReqHandle, typeTargetAddr, (uintptr_t)pIPAddr, lenTargetAddr, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfICMPEchoResponseCB = pfICMPEchoResponseCB;

    return WDRV_WINC_STATUS_OK;
}
