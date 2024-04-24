/*******************************************************************************
  WINC Wireless Driver

  File Name:
    wdrv_winc_dns.c

  Summary:
    WINC wireless driver DNS client implementation.

  Description:
    WINC wireless interface for DNS client configuration.
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

#include <stdint.h>
#include <string.h>
#include <time.h>

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_dns.h"

//*******************************************************************************
/*
  Function:
    static void timeCmdRspCallbackHandler
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

static void dnsCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("DNS CmdRspCB %08x Event %d\n", cmdReqHandle, event);

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
    void WDRV_WINC_DNSProcessAEC
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
    See wdrv_winc_dns.h for usage information.

*/

void WDRV_WINC_DNSProcessAEC(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_DEV_EVENT_RSP_ELEMS *pElems)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)context;
    uint8_t recordType;
    char domain[WINC_CMD_PARAM_ID_DNS_DOMAIN_NAME_SZ];

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_DNSRESOLV:
        {
            WDRV_WINC_IP_MULTI_TYPE_ADDRESS addr;

            if (pElems->numElems != 3)
            {
                return;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &recordType, sizeof(recordType));
            WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_STRING, &domain, sizeof(domain));

            if (WINC_TYPE_IPV4ADDR == pElems->elems[2].type)
            {
                WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_IPV4ADDR, &addr.addr, sizeof(WDRV_WINC_IPV4_ADDR));
                addr.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV4;
            }
            else if (WINC_TYPE_IPV6ADDR == pElems->elems[2].type)
            {
                WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_IPV6ADDR, &addr.addr, sizeof(WDRV_WINC_IPV6_ADDR));
                addr.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV6;
            }
            else
            {
                break;
            }

            if (NULL != pDcpt->pCtrl->pfDNSResolveResponseCB)
            {
                pDcpt->pCtrl->pfDNSResolveResponseCB((DRV_HANDLE)pDcpt, WDRV_WINC_DNS_STATUS_OK, recordType, domain, &addr);
            }

            break;
        }

        case WINC_AEC_ID_DNSERR:
        {
            uint16_t status;

            if (pElems->numElems != 3)
            {
                return;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_STATUS, &status, sizeof(status));
            WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &recordType, sizeof(recordType));
            WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_STRING, &domain, sizeof(domain));

            if (NULL != pDcpt->pCtrl->pfDNSResolveResponseCB)
            {
                pDcpt->pCtrl->pfDNSResolveResponseCB((DRV_HANDLE)pDcpt, status, recordType, domain, NULL);
                pDcpt->pCtrl->pfDNSResolveResponseCB = NULL;
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
    WDRV_WINC_STATUS WDRV_WINC_DNSAutoSet
    (
        DRV_HANDLE handle,
        bool enabled
    )

  Summary:
    Set the enabled state of the DNS server auto-assignment.

  Description:
    Configures the system to use the automatic DNS assignment, for example via
    DHCP.

  Remarks:
    See wdrv_winc_dns.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSAutoSet(DRV_HANDLE handle, bool enabled)
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

    pCmdReqBuffer = OSAL_Malloc(128);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_AUTO, WINC_TYPE_BOOL, enabled, 1);

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
    WDRV_WINC_STATUS WDRV_WINC_DNSServerAddressSet
    (
        DRV_HANDLE handle,
        uint8_t numServers,
        WDRV_WINC_IP_MULTI_TYPE_ADDRESS *pServerAddr
    )

  Summary:
    Sets a list of DNS servers to be used in manual DNS server configuration.

  Description:
    Configures the system to use the DNS servers specified.

  Remarks:
    See wdrv_winc_dns.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSServerAddressSet
(
    DRV_HANDLE handle,
    uint8_t numServers,
    WDRV_WINC_IP_MULTI_TYPE_ADDRESS *pServerAddr
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    int i;

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

    /* Limit servers */
    if (numServers > WINC_CFG_PARAM_ID_DNS_DNS_SVR_NUM)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pServerAddr)
    {
        for (i=0; i<numServers; i++)
        {
            if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 != pServerAddr[i].type)
            {
                return WDRV_WINC_STATUS_INVALID_ARG;
            }
        }
    }

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 3, dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_SVR, WINC_TYPE_INTEGER, 0, 0);

    if (NULL != pServerAddr)
    {
        for (i=0; i<numServers; i++)
        {
            WINC_TYPE typeVal;
            size_t lenVal;

            if (WDRV_WINC_IP_ADDRESS_TYPE_IPV4 == pServerAddr[i].type)
            {
                typeVal = WINC_TYPE_IPV4ADDR;
                lenVal  = sizeof(WDRV_WINC_IPV4_ADDR);
            }
            else
            {
                typeVal = WINC_TYPE_IPV6ADDR;
                lenVal  = sizeof(WDRV_WINC_IPV6_ADDR);
            }

            WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_SVR, typeVal, (uintptr_t)&pServerAddr[i].addr, lenVal);
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
    WDRV_WINC_STATUS WDRV_WINC_DNSResolveDomain
    (
        DRV_HANDLE handle,
        uint8_t type,
        const char *pDomainName,
        uint16_t timeoutMs,
        const WDRV_WINC_DNS_RESOLVE_CALLBACK pfDNSResolveResponseCB
    )

  Summary:
    Resolves the IP address of a specific domain.

  Description:
    Resolves the IP address of a specific domain.

  Remarks:
    See wdrv_winc_dns.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSResolveDomain
(
    DRV_HANDLE handle,
    uint8_t recordType,
    const char *pDomainName,
    uint16_t timeoutMs,
    const WDRV_WINC_DNS_RESOLVE_CALLBACK pfDNSResolveResponseCB
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    size_t addressLen;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pDomainName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Check type */
    if ((WINC_CONST_DNS_TYPE_A != recordType) && (WINC_CONST_DNS_TYPE_AAAA != recordType))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    addressLen = strnlen(pDomainName, WINC_CMD_PARAM_ID_DNS_DOMAIN_NAME_SZ+1);

    if ((0 == addressLen) || (addressLen > WINC_CMD_PARAM_ID_DNS_DOMAIN_NAME_SZ))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pCmdReqBuffer = OSAL_Malloc(256+addressLen);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256+addressLen, 2, dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_TIMEOUT, WINC_TYPE_INTEGER_UNSIGNED, timeoutMs, sizeof(timeoutMs));
    WINC_CmdDNSRESOLV(cmdReqHandle, recordType, (uint8_t*)pDomainName, addressLen);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfDNSResolveResponseCB = pfDNSResolveResponseCB;

    return WDRV_WINC_STATUS_OK;
}

//DOM-IGNORE-END
