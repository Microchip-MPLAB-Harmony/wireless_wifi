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
Copyright (C) 2024-26 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Network Interface Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void netifProcessStatus
    (
        const WDRV_WINC_DCPT *const pDcpt,
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
    const WDRV_WINC_DCPT *const pDcpt,
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
        case WINC_CMD_ID_NETIFC:
        {
            WDRV_WINC_NETIF_IDX ifIdx;
            WINC_DEV_PARAM_ELEM elems[10];
            void *pInfoPtr = NULL;
            WDRV_WINC_STATUS status;
            int i;

            if (NULL == pDcpt->pCtrl->netIfInfo.pfCB)
            {
                break;
            }

            status = (WINC_STATUS_OK == statusCode) ? WDRV_WINC_STATUS_OK : WDRV_WINC_STATUS_REQUEST_ERROR;

            if (false == WINC_DevUnpackElements(pSrcCmd->numParams, pSrcCmd->pParams, elems))
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER, &ifIdx, sizeof(ifIdx));

            for (i=0; i<WDRV_WINC_NUM_NETIF_INFO_TYPE; i++)
            {
                if (0 != (pDcpt->pCtrl->netIfInfo.reqCompMask & (1<<i)))
                {
                    switch (i)
                    {
                        case WDRV_WINC_NETIF_INFO_MAC_ADDR:
                        {
                            pInfoPtr = &pDcpt->pCtrl->netIfInfo.pStore->intf.macAddr;
                            break;
                        }

                        case WDRV_WINC_NETIF_INFO_INTF:
                        {
                            pInfoPtr = &pDcpt->pCtrl->netIfInfo.pStore->intf;
                            break;
                        }
#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
                        case WDRV_WINC_NETIF_INFO_DHCP_LEASE:
                        {
                            pInfoPtr = &pDcpt->pCtrl->netIfInfo.pStore->dhcp;
                            break;
                        }

                        case WDRV_WINC_NETIF_INFO_IPV4:
                        {
                            pInfoPtr = &pDcpt->pCtrl->netIfInfo.pStore->ipv4;
                            break;
                        }

                        case WDRV_WINC_NETIF_INFO_IPV6:
                        {
                            pInfoPtr = &pDcpt->pCtrl->netIfInfo.pStore->ipv6;
                            break;
                        }
#endif
                        default:
                        {
                            pInfoPtr = NULL;
                            status   = WDRV_WINC_STATUS_INVALID_CONTEXT;
                            break;
                        }
                    }

                    if (WDRV_WINC_STATUS_INVALID_CONTEXT != status)
                    {
                        if (WDRV_WINC_STATUS_OK != status)
                        {
                            pInfoPtr = NULL;
                        }

                        pDcpt->pCtrl->netIfInfo.pfCB((DRV_HANDLE)pDcpt, ifIdx, i, pInfoPtr, status);
                    }
                }
            }

            if (NULL != pDcpt->pCtrl->netIfInfo.pStore)
            {
                OSAL_Free(pDcpt->pCtrl->netIfInfo.pStore);
                pDcpt->pCtrl->netIfInfo.pStore = NULL;
            }

            pDcpt->pCtrl->netIfInfo.pfCB = NULL;

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
        const WDRV_WINC_DCPT *const pDcpt,
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
    const WDRV_WINC_DCPT *const pDcpt,
    uint16_t rspId,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (rspId)
    {
        case WINC_CMD_ID_NETIFC:
        {
            WINC_DEV_FRACT_INT_TYPE id;

            if ((numElems < 2U) || (NULL == pDcpt->pCtrl->netIfInfo.pfCB))
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

            switch (id.i)
            {
                case WINC_CFG_PARAM_ID_NETIF_NAME:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_INTF))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_STRING, &pDcpt->pCtrl->netIfInfo.pStore->intf.name, WINC_CFG_PARAM_SZ_NETIF_NAME);
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_INTF;
                        pDcpt->pCtrl->netIfInfo.pStore->intf.valid.name = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_ETHER:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & (WDRV_WINC_NETIF_REQ_INFO_MAC_ADDR | WDRV_WINC_NETIF_REQ_INFO_INTF)))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_MACADDR, &pDcpt->pCtrl->netIfInfo.pStore->intf.macAddr, WDRV_WINC_MAC_ADDR_LEN);
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= (pDcpt->pCtrl->netIfInfo.reqMask & (WDRV_WINC_NETIF_REQ_INFO_MAC_ADDR | WDRV_WINC_NETIF_REQ_INFO_INTF));
                        pDcpt->pCtrl->netIfInfo.pStore->intf.valid.macAddr = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_HOSTNAME:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_INTF))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_STRING, &pDcpt->pCtrl->netIfInfo.pStore->intf.hostname, WINC_CFG_PARAM_SZ_NETIF_HOSTNAME);
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_INTF;
                        pDcpt->pCtrl->netIfInfo.pStore->intf.valid.hostname = true;
                    }
                    break;
                }

#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
                case WINC_CFG_PARAM_ID_NETIF_DHCPC_EN:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_BOOL, &pDcpt->pCtrl->netIfInfo.pStore->dhcp.enabled, sizeof(pDcpt->pCtrl->netIfInfo.pStore->dhcp.enabled));
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE;
                        pDcpt->pCtrl->netIfInfo.pStore->dhcp.valid.enabled = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_DHCPC_LEASE_START:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_UTC_TIME, &pDcpt->pCtrl->netIfInfo.pStore->dhcp.leaseObtainedUTC, sizeof(pDcpt->pCtrl->netIfInfo.pStore->dhcp.leaseObtainedUTC));
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE;
                        pDcpt->pCtrl->netIfInfo.pStore->dhcp.valid.leaseObtainedUTC = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_DHCPC_LEASE_ENDS:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_UTC_TIME, &pDcpt->pCtrl->netIfInfo.pStore->dhcp.leaseExpiresUTC, sizeof(pDcpt->pCtrl->netIfInfo.pStore->dhcp.leaseExpiresUTC));
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE;
                        pDcpt->pCtrl->netIfInfo.pStore->dhcp.valid.leaseExpiresUTC = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_DHCPC_SRV_ADDR:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_IPV4ADDR, &pDcpt->pCtrl->netIfInfo.pStore->dhcp.serverAddr, sizeof(pDcpt->pCtrl->netIfInfo.pStore->dhcp.serverAddr));
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_DHCP_LEASE;
                        pDcpt->pCtrl->netIfInfo.pStore->dhcp.valid.serverAddr = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_IP_MASK:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_IPV4))
                    {
                        WDRV_WINC_IPV4_ADDR ipMask[2];

                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_IPV4ADDR, &ipMask, sizeof(ipMask));
                        pDcpt->pCtrl->netIfInfo.pStore->ipv4.addr.Val = ipMask[0].Val;
                        pDcpt->pCtrl->netIfInfo.pStore->ipv4.mask.Val = ipMask[1].Val;
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_IPV4;
                        pDcpt->pCtrl->netIfInfo.pStore->ipv4.valid.addr = true;
                        pDcpt->pCtrl->netIfInfo.pStore->ipv4.valid.mask = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_GATEWAY:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_IPV4))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_IPV4ADDR, &pDcpt->pCtrl->netIfInfo.pStore->ipv4.gateway, sizeof(WDRV_WINC_IPV4_ADDR));
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_IPV4;
                        pDcpt->pCtrl->netIfInfo.pStore->ipv4.valid.gateway = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_IPV6_GLO_ADDR:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_IPV6))
                    {
                        uint8_t prefixAddr[17];

                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_IPV6ADDR, prefixAddr, sizeof(prefixAddr));

                        (void)memcpy(&pDcpt->pCtrl->netIfInfo.pStore->ipv6.global[id.f].addr, prefixAddr, sizeof(WDRV_WINC_IPV6_ADDR));
                        pDcpt->pCtrl->netIfInfo.pStore->ipv6.global[id.f].prefixLen = prefixAddr[16];

                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_IPV6;
                        pDcpt->pCtrl->netIfInfo.pStore->ipv6.valid.global = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_IPV6_LL_ADDR:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_IPV6))
                    {
                        uint8_t prefixAddr[17];

                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_IPV6ADDR, prefixAddr, sizeof(prefixAddr));

                        (void)memcpy(&pDcpt->pCtrl->netIfInfo.pStore->ipv6.linkLocal.addr, prefixAddr, sizeof(WDRV_WINC_IPV6_ADDR));
                        pDcpt->pCtrl->netIfInfo.pStore->ipv6.linkLocal.prefixLen = prefixAddr[16];

                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_IPV6;
                        pDcpt->pCtrl->netIfInfo.pStore->ipv6.valid.linkLocal = true;
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_NETIF_IPV6_GATEWAY:
                {
                    if (0 != (pDcpt->pCtrl->netIfInfo.reqMask & WDRV_WINC_NETIF_REQ_INFO_IPV6))
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_IPV6ADDR, &pDcpt->pCtrl->netIfInfo.pStore->ipv6.gateway, sizeof(WDRV_WINC_IPV6_ADDR));
                        pDcpt->pCtrl->netIfInfo.reqCompMask |= WDRV_WINC_NETIF_REQ_INFO_IPV6;
                        pDcpt->pCtrl->netIfInfo.pStore->ipv6.valid.gateway = true;
                    }
                    break;
                }
#endif
                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("NETIFC ID %d not handled\r\n", id.i);
                    break;
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
    static void netifProcessAEC
    (
        const WDRV_WINC_DCPT *const pDcpt,
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
    const WDRV_WINC_DCPT *const pDcpt,
    uint16_t aecId,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    const WDRV_WINC_CTRLDCPT *pCtrl;

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
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)context;

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
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)context;

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
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;

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
    WDRV_WINC_STATUS WDRV_WINC_NetIfInfoSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_NETIF_INFO_TYPE infoType,
        void *pInfo
    )

  Summary:
    Set the information about a network interface.

  Description:
    Configures the information about a network interface.

  Remarks:
    See wdrv_winc_netif.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfInfoSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    WDRV_WINC_NETIF_INFO_TYPE infoType,
    void *pInfo
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const )handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    bool sendReq = false;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pInfo))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (NULL != pDcpt->pCtrl->netIfInfo.pfCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(4, 128, &netifCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    switch (infoType)
    {
        case WDRV_WINC_NETIF_INFO_INTF:
        {
            WDRV_WINC_NETIF_INFO_STORE_INTF_TYPE *pIntfInfo = pInfo;
            size_t length;

            if (true == pIntfInfo->valid.hostname)
            {
                length = strnlen(pIntfInfo->hostname, WINC_CFG_PARAM_SZ_NETIF_HOSTNAME+1);

                if (length > WINC_CFG_PARAM_SZ_NETIF_HOSTNAME)
                {
                    break;
                }

                (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_HOSTNAME, WINC_TYPE_STRING, (uintptr_t)&pIntfInfo->hostname, length);
                sendReq = true;
            }

            break;
        }

#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
        case WDRV_WINC_NETIF_INFO_DHCP_LEASE:
        {
            const WDRV_WINC_NETIF_INFO_STORE_DHCP_TYPE *pDHCPLeaseInfo = pInfo;

            if (true == pDHCPLeaseInfo->valid.enabled)
            {
                (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_DHCPC_EN, WINC_TYPE_INTEGER, pDHCPLeaseInfo->enabled, 0);
                sendReq = true;
            }

            break;
        }

        case WDRV_WINC_NETIF_INFO_IPV4:
        {
            WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE *pIPv4Info = pInfo;

            if ((true == pIPv4Info->valid.addr) && (true == pIPv4Info->valid.mask))
            {
                WDRV_WINC_IPV4_ADDR ipMask[2];

                ipMask[0].Val = pIPv4Info->addr.Val;
                ipMask[1].Val = pIPv4Info->mask.Val;

                (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_IP_MASK, WINC_TYPE_IPV4ADDR, (uintptr_t)&ipMask, sizeof(ipMask));
                sendReq = true;
            }

            if (true == pIPv4Info->valid.gateway)
            {
                (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_GATEWAY, WINC_TYPE_IPV4ADDR, (uintptr_t)&pIPv4Info->gateway, sizeof(WDRV_WINC_IPV4_ADDR));
                sendReq = true;
            }

            break;
        }

        case WDRV_WINC_NETIF_INFO_IPV6:
        {
            WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE *pIPv6Info = pInfo;

            if (true == pIPv6Info->valid.global)
            {
                int i;

                for (i=0; i<WDRV_WINC_NETIF_INFO_NUM_GLO_IPV6; i++)
                {
                    uint8_t prefixAddr[17];

                    (void)memcpy(prefixAddr, &pIPv6Info->global[i].addr, sizeof(WDRV_WINC_IPV6_ADDR));
                    prefixAddr[16] = pIPv6Info->global[i].prefixLen;

                    (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CmdFormFractInt(WINC_CFG_PARAM_ID_NETIF_IPV6_GLO_ADDR, i), WINC_TYPE_IPV6ADDR, (uintptr_t)prefixAddr, sizeof(prefixAddr));
                    sendReq = true;
                }
            }

            if (true == pIPv6Info->valid.gateway)
            {
                (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_IPV6_GATEWAY, WINC_TYPE_IPV6ADDR, (uintptr_t)&pIPv6Info->gateway, sizeof(WDRV_WINC_IPV6_ADDR));
                sendReq = true;
            }

            break;
        }
#endif
        default:
        {
            WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }

    if (false == sendReq)
    {
        WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
        return WDRV_WINC_STATUS_INVALID_ARG;
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
    WDRV_WINC_STATUS WDRV_WINC_NetIfInfoGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        uint32_t reqInfoMask,
        WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB
    )

  Summary:
    Get information about a network interface.

  Description:
    Requests the information of a network interface, to be return via a callback.

  Remarks:
    See wdrv_winc_netif.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfInfoGet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    uint32_t reqInfoMask,
    WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const )handle;
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

    if ((NULL != pDcpt->pCtrl->netIfInfo.pfCB) || (NULL != pDcpt->pCtrl->netIfInfo.pStore))
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, &netifCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->netIfInfo.pStore = OSAL_Malloc(sizeof(WDRV_WINC_NETIF_INFO_STORE_TYPE));

    if (NULL == pDcpt->pCtrl->netIfInfo.pStore)
    {
        WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (WDRV_WINC_NETIF_REQ_INFO_MAC_ADDR == reqInfoMask)
    {
        (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CFG_PARAM_ID_NETIF_ETHER, WINC_TYPE_INVALID, 0, 0);
    }
    else
    {
        (void)WINC_CmdNETIFC(cmdReqHandle, (int32_t)ifIdx, WINC_CMDNETIFC_ID_IGNORE_VAL, WINC_TYPE_INVALID, 0, 0);
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        OSAL_Free(pDcpt->pCtrl->netIfInfo.pStore);
        pDcpt->pCtrl->netIfInfo.pStore = NULL;
        pDcpt->pCtrl->netIfInfo.pfCB   = NULL;
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->netIfInfo.pfCB         = pfNetIfInfoCB;
    pDcpt->pCtrl->netIfInfo.reqMask      = reqInfoMask;
    pDcpt->pCtrl->netIfInfo.reqCompMask  = 0;

    (void)memset(pDcpt->pCtrl->netIfInfo.pStore, 0, sizeof(WDRV_WINC_NETIF_INFO_STORE_TYPE));

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
    WDRV_WINC_NETIF_INFO_STORE_DHCP_TYPE dhcpLeaseInfo;

    (void)memset(&dhcpLeaseInfo, 0, sizeof(WDRV_WINC_NETIF_INFO_STORE_DHCP_TYPE));

    if (0 != (aacMode & WDRV_WINC_NETIF_AAC_V4_DHCP))
    {
        dhcpLeaseInfo.enabled = true;
    }

    dhcpLeaseInfo.valid.enabled = true;

    return WDRV_WINC_NetIfInfoSet(handle, ifIdx, WDRV_WINC_NETIF_INFO_DHCP_LEASE, &dhcpLeaseInfo);
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPAddrSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_IP_ADDRESS_TYPE type,
        const WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
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
    const WDRV_WINC_IP_MULTI_ADDRESS *pAddr,
    unsigned int scope
)
{
    if (NULL == pAddr)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
    {
        WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE ipv4Info;

        if (scope > 31U)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        (void)memset(&ipv4Info, 0, sizeof(WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE));

        ipv4Info.addr.Val = pAddr->v4.Val;
        ipv4Info.valid.addr = true;

        ipv4Info.mask.Val = 0xffffffffU;
        ipv4Info.mask.Val <<= (32U-scope);
        ipv4Info.mask.Val = WDRV_WINC_UtilsHToNL(ipv4Info.mask.Val);
        ipv4Info.valid.mask = true;

        return WDRV_WINC_NetIfInfoSet(handle, ifIdx, WDRV_WINC_NETIF_INFO_IPV4, &ipv4Info);
    }
    else if (WDRV_WINC_IP_ADDRESS_TYPE_IPV6 == type)
    {
        WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE ipv6Info;

        (void)memset(&ipv6Info, 0, sizeof(WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE));

        if (scope > 127U)
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        (void)memcpy(&ipv6Info.global[0].addr, (uint8_t*)pAddr, 16);
        ipv6Info.global[0].prefixLen = (uint8_t)scope;
        ipv6Info.valid.global = true;

        return WDRV_WINC_NetIfInfoSet(handle, ifIdx, WDRV_WINC_NETIF_INFO_IPV6, &ipv6Info);
    }
    else
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfIPRouteSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        WDRV_WINC_IP_ADDRESS_TYPE type,
        const WDRV_WINC_IP_MULTI_ADDRESS *pNetwork,
        int scope,
        const WDRV_WINC_IP_MULTI_ADDRESS *pDest
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
    const WDRV_WINC_IP_MULTI_ADDRESS *pNetwork,
    int scope,
    const WDRV_WINC_IP_MULTI_ADDRESS *pDest
)
{
    if (NULL == pDest)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == type)
    {
        WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE ipv4Info;

        (void)memset(&ipv4Info, 0, sizeof(WDRV_WINC_NETIF_INFO_STORE_IPV4_TYPE));

        (void)memcpy(&ipv4Info.gateway, pDest, sizeof(WDRV_WINC_IPV4_ADDR));
        ipv4Info.valid.gateway = true;

        return WDRV_WINC_NetIfInfoSet(handle, ifIdx, WDRV_WINC_NETIF_INFO_IPV4, &ipv4Info);
    }
    else if (WDRV_WINC_IP_ADDRESS_TYPE_IPV6 == type)
    {
        WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE ipv6Info;

        (void)memset(&ipv6Info, 0, sizeof(WDRV_WINC_NETIF_INFO_STORE_IPV6_TYPE));

        (void)memcpy(&ipv6Info.gateway, pDest, sizeof(WDRV_WINC_IPV6_ADDR));
        ipv6Info.valid.gateway = true;

        return WDRV_WINC_NetIfInfoSet(handle, ifIdx, WDRV_WINC_NETIF_INFO_IPV6, &ipv6Info);
    }
    else
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }
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
    return WDRV_WINC_NetIfInfoGet(handle, ifIdx, WDRV_WINC_NETIF_REQ_INFO_MAC_ADDR, pfNetIfInfoCB);
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NetIfHostnameSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        const char* pHostname
    )

  Summary:
    Set the hostname of a network interface.

  Description:
    Configures the hostname of a network interface.

  Remarks:
    See wdrv_winc_netif.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_NetIfHostnameSet
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    const char* pHostname
)
{
    WDRV_WINC_NETIF_INFO_STORE_INTF_TYPE intf;
    size_t length;

    length = strnlen(pHostname, WINC_CFG_PARAM_SZ_NETIF_HOSTNAME+1);

    if (length > WINC_CFG_PARAM_SZ_NETIF_HOSTNAME)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    (void)strncpy(intf.hostname, pHostname, WINC_CFG_PARAM_SZ_NETIF_HOSTNAME);
    intf.valid.hostname = true;

    return WDRV_WINC_NetIfInfoSet(handle, ifIdx, WDRV_WINC_NETIF_INFO_INTF, &intf);
}
