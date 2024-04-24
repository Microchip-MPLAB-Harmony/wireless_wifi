/*******************************************************************************
  WINC Driver Socket Mode Implementation

  File Name:
    wdrv_winc_socket.c

  Summary:
    WINC wireless driver (Socket mode) implementation.

  Description:
    This interface provides extra functionality required for socket mode operation.
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
#include "wdrv_winc_socket.h"

//*******************************************************************************
/*
  Function:
    static void pingCmdRspCallbackHandler
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

static void pingCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("PING CmdRspCB %08x Event %d\n", cmdReqHandle, event);

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

            if (NULL != pStatusInfo)
            {
                if (WINC_STATUS_OK != pStatusInfo->status)
                {
                    if (NULL != pDcpt->pCtrl->pfICMPEchoResponseCB)
                    {
                        pDcpt->pCtrl->pfICMPEchoResponseCB((DRV_HANDLE)pDcpt, NULL, WDRV_WINC_IP_ADDRESS_TYPE_ANY, 0);
                        pDcpt->pCtrl->pfICMPEchoResponseCB = NULL;
                    }
                }
            }
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_ICMPProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        WINC_DEV_EVENT_RSP_ELEMS *pElems
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
    WINC_DEV_EVENT_RSP_ELEMS *pElems
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)context;
    uint16_t rtt;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_PING:
        {
            if (pElems->numElems != 2)
            {
                return;
            }

            if (NULL != pDcpt->pCtrl->pfICMPEchoResponseCB)
            {
                WDRV_WINC_IP_MULTI_ADDRESS *pIPAddr;
                WDRV_WINC_IP_ADDRESS_TYPE ipAddrType;

                pIPAddr = (WDRV_WINC_IP_MULTI_ADDRESS*)pElems->elems[0].pData;

                if ((WINC_TYPE_IPV4ADDR == pElems->elems[0].type) && (pElems->elems[0].length <= sizeof(WDRV_WINC_IPV4_ADDR)))
                {
                    ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_IPV4;
                }
                else if ((WINC_TYPE_IPV6ADDR == pElems->elems[0].type) && (pElems->elems[0].length <= sizeof(WDRV_WINC_IPV6_ADDR)))
                {
                    ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_IPV6;
                }
                else
                {
                    pIPAddr = NULL;
                    ipAddrType = WDRV_WINC_IP_ADDRESS_TYPE_ANY;
                }

                WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &rtt, sizeof(rtt));

                pDcpt->pCtrl->pfICMPEchoResponseCB((DRV_HANDLE)pDcpt, pIPAddr, ipAddrType, rtt);
                pDcpt->pCtrl->pfICMPEchoResponseCB = NULL;
            }

            break;
        }

        case WINC_AEC_ID_PINGERR:
        {
            if (NULL != pDcpt->pCtrl->pfICMPEchoResponseCB)
            {
                pDcpt->pCtrl->pfICMPEchoResponseCB((DRV_HANDLE)pDcpt, NULL, WDRV_WINC_IP_ADDRESS_TYPE_ANY, 0);
                pDcpt->pCtrl->pfICMPEchoResponseCB = NULL;
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

    WINC_SockRegisterEventCallback(pDcpt->pCtrl->wincDevHandle, pfSocketEventCb, (uintptr_t)pDcpt);

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_ICMPEchoRequestAddr
    (
        DRV_HANDLE handle,
        WDRV_WINC_IP_MULTI_ADDRESS *pIPAddr,
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
    WDRV_WINC_IP_MULTI_ADDRESS *pIPAddr,
    WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
    const WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
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

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 1, pingCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdPING(cmdReqHandle, typeTargetAddr, (uintptr_t)pIPAddr, lenTargetAddr, 0);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfICMPEchoResponseCB = pfICMPEchoResponseCB;

    return WDRV_WINC_STATUS_OK;
}
