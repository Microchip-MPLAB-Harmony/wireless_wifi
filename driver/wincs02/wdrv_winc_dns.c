/*******************************************************************************
  WINC Wireless Driver DNS Client Source File

  File Name:
    wdrv_winc_dns.c

  Summary:
    WINC wireless driver DNS client implementation.

  Description:
    WINC wireless interface for DNS client configuration.
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

#include <stdint.h>
#include <string.h>
#include <time.h>

#include "wdrv_winc.h"

#ifndef WDRV_WINC_MOD_DISABLE_DNS

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver DNS Client Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void dnsProcessAEC
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

static void dnsProcessAEC
(
    const WDRV_WINC_DCPT *const pDcpt,
    uint16_t aecId,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    uint8_t recordType;
    char domain[WINC_CMD_PARAM_SZ_DNS_QUERY_NAME];

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (aecId)
    {
        case WINC_AEC_ID_DNSRESOLV:
        {
            size_t domainLen;

            if (numElems < 3U)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &recordType, sizeof(recordType));
            domainLen = WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_STRING, &domain, sizeof(domain));

            if (0 == domainLen)
            {
                break;
            }

            if (NULL != pDcpt->pCtrl->pfDNSResolveResponseCB)
            {
                WDRV_WINC_IP_MULTI_TYPE_ADDRESS addr;

                if (WINC_CONST_DNS_TYPE_A == recordType)
                {
                    (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_IPV4ADDR, &addr.addr, sizeof(WDRV_WINC_IPV4_ADDR));
                    addr.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV4;
                }
                else if (WINC_CONST_DNS_TYPE_AAAA == recordType)
                {
                    (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_IPV6ADDR, &addr.addr, sizeof(WDRV_WINC_IPV6_ADDR));
                    addr.type = WDRV_WINC_IP_ADDRESS_TYPE_IPV6;
                }
                else
                {
                    break;
                }

                pDcpt->pCtrl->pfDNSResolveResponseCB((DRV_HANDLE)pDcpt, WDRV_WINC_DNS_STATUS_OK, recordType, domain, &addr);
            }

            if (NULL != pDcpt->pCtrl->pfDNSResQueryResponseCB)
            {
                WDRV_WINC_DNS_RES_RECORD res;

                if (WINC_CONST_DNS_TYPE_A == recordType)
                {
                    (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_IPV4ADDR, &res.A.address, sizeof(WDRV_WINC_IPV4_ADDR));
                }
                else if (WINC_CONST_DNS_TYPE_NS == recordType)
                {
                    res.NS.pName = (char*)pElems[2].pData;
                }
                else if (WINC_CONST_DNS_TYPE_CNAME == recordType)
                {
                    res.CNAME.pName = (char*)pElems[2].pData;
                }
                else if (WINC_CONST_DNS_TYPE_SOA == recordType)
                {
                    if (9U != numElems)
                    {
                        break;
                    }

                    res.SOA.pMName = (char*)pElems[2].pData;
                    (void)WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_INTEGER_UNSIGNED, &res.SOA.serial, sizeof(res.SOA.serial));
                    (void)WINC_CmdReadParamElem(&pElems[4], WINC_TYPE_INTEGER_UNSIGNED, &res.SOA.refresh, sizeof(res.SOA.refresh));
                    (void)WINC_CmdReadParamElem(&pElems[5], WINC_TYPE_INTEGER_UNSIGNED, &res.SOA.retry, sizeof(res.SOA.retry));
                    (void)WINC_CmdReadParamElem(&pElems[6], WINC_TYPE_INTEGER_UNSIGNED, &res.SOA.expire, sizeof(res.SOA.expire));
                    (void)WINC_CmdReadParamElem(&pElems[7], WINC_TYPE_INTEGER_UNSIGNED, &res.SOA.minimum, sizeof(res.SOA.minimum));
                    res.SOA.pRName = (char*)pElems[8].pData;
                }
                else if (WINC_CONST_DNS_TYPE_PTR == recordType)
                {
                    res.PTR.pDName = (char*)pElems[2].pData;
                }
                else if (WINC_CONST_DNS_TYPE_MX == recordType)
                {
                    res.MX.pExchange = (char*)pElems[2].pData;
                    (void)WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_INTEGER_UNSIGNED, &res.MX.preference, sizeof(res.MX.preference));
                }
                else if (WINC_CONST_DNS_TYPE_TXT == recordType)
                {
                    res.TXT.pData = (char*)pElems[2].pData;
                }
                else if (WINC_CONST_DNS_TYPE_AAAA == recordType)
                {
                    (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_IPV6ADDR, &res.AAAA.address, sizeof(WDRV_WINC_IPV6_ADDR));
                }
                else if (WINC_CONST_DNS_TYPE_SRV == recordType)
                {
                    res.SRV.pTarget = (char*)pElems[2].pData;
                    (void)WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_INTEGER_UNSIGNED, &res.SRV.priority, sizeof(res.SRV.priority));
                    (void)WINC_CmdReadParamElem(&pElems[4], WINC_TYPE_INTEGER_UNSIGNED, &res.SRV.weight, sizeof(res.SRV.weight));
                    (void)WINC_CmdReadParamElem(&pElems[5], WINC_TYPE_INTEGER_UNSIGNED, &res.SRV.port, sizeof(res.SRV.port));
                }
                else if (WINC_CONST_DNS_TYPE_DS == recordType)
                {
                    (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER_UNSIGNED, &res.DS.keyTag, sizeof(res.DS.keyTag));
                    (void)WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_INTEGER_UNSIGNED, &res.DS.algorithm, sizeof(res.DS.algorithm));
                    (void)WINC_CmdReadParamElem(&pElems[4], WINC_TYPE_INTEGER_UNSIGNED, &res.DS.digestType, sizeof(res.DS.digestType));
                    res.DS.pDigest   = pElems[5].pData;
                    res.DS.digestLen = pElems[5].length;
                }
                else if (WINC_CONST_DNS_TYPE_RRSIG == recordType)
                {
                    res.RRSIG.pSignersName = (char*)pElems[2].pData;
                    (void)WINC_CmdReadParamElem(&pElems[3],  WINC_TYPE_INTEGER_UNSIGNED, &res.RRSIG.typeCovered, sizeof(res.RRSIG.typeCovered));
                    (void)WINC_CmdReadParamElem(&pElems[4],  WINC_TYPE_INTEGER_UNSIGNED, &res.RRSIG.algorithm, sizeof(res.RRSIG.algorithm));
                    (void)WINC_CmdReadParamElem(&pElems[5],  WINC_TYPE_INTEGER_UNSIGNED, &res.RRSIG.labels, sizeof(res.RRSIG.labels));
                    (void)WINC_CmdReadParamElem(&pElems[6],  WINC_TYPE_INTEGER_UNSIGNED, &res.RRSIG.origTTL, sizeof(res.RRSIG.origTTL));
                    (void)WINC_CmdReadParamElem(&pElems[7],  WINC_TYPE_INTEGER_UNSIGNED, &res.RRSIG.sigExpiration, sizeof(res.RRSIG.sigExpiration));
                    (void)WINC_CmdReadParamElem(&pElems[8],  WINC_TYPE_INTEGER_UNSIGNED, &res.RRSIG.sigInception, sizeof(res.RRSIG.sigInception));
                    (void)WINC_CmdReadParamElem(&pElems[9],  WINC_TYPE_INTEGER_UNSIGNED, &res.RRSIG.keyTag, sizeof(res.RRSIG.keyTag));
                    res.RRSIG.pSignature   = pElems[10].pData;
                    res.RRSIG.signatureLen = pElems[10].length;
                }
                else if (WINC_CONST_DNS_TYPE_NSEC == recordType)
                {
                    res.NSEC.pNextDomain = (char*)pElems[2].pData;
                    res.NSEC.pTypeBitMap   = pElems[3].pData;
                    res.NSEC.typeBitMapLen = pElems[3].length;
                }
                else if (WINC_CONST_DNS_TYPE_DNSKEY == recordType)
                {
                    (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER_UNSIGNED, &res.DNSKEY.flags, sizeof(res.DNSKEY.flags));
                    (void)WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_INTEGER_UNSIGNED, &res.DNSKEY.protocol, sizeof(res.DNSKEY.protocol));
                    (void)WINC_CmdReadParamElem(&pElems[4], WINC_TYPE_INTEGER_UNSIGNED, &res.DNSKEY.algorithm, sizeof(res.DNSKEY.algorithm));
                    res.DNSKEY.pPublicKey   = pElems[5].pData;
                    res.DNSKEY.publicKeyLen = pElems[5].length;
                }
                else
                {
                    res.RR.pData   = pElems[2].pData;
                    res.RR.dataLen = pElems[2].length;
                }

                pDcpt->pCtrl->pfDNSResQueryResponseCB((DRV_HANDLE)pDcpt, WDRV_WINC_DNS_STATUS_OK, recordType, domain, &res);
            }

            break;
        }

        case WINC_AEC_ID_DNSERR:
        {
            uint16_t status;

            if (3U != numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_STATUS, &status, sizeof(status));
            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &recordType, sizeof(recordType));
            (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_STRING, &domain, sizeof(domain));

            if (NULL != pDcpt->pCtrl->pfDNSResolveResponseCB)
            {
                WDRV_WINC_DNS_RESOLVE_CALLBACK pfDNSResolveResponseCB = pDcpt->pCtrl->pfDNSResolveResponseCB;

                pDcpt->pCtrl->pfDNSResolveResponseCB = NULL;

                pfDNSResolveResponseCB((DRV_HANDLE)pDcpt, status, recordType, domain, NULL);
            }

            if (NULL != pDcpt->pCtrl->pfDNSResQueryResponseCB)
            {
                WDRV_WINC_DNS_QUERY_RESOURCE_CALLBACK pfDNSResQueryResponseCB = pDcpt->pCtrl->pfDNSResQueryResponseCB;

                pDcpt->pCtrl->pfDNSResQueryResponseCB = NULL;

                pfDNSResQueryResponseCB((DRV_HANDLE)pDcpt, status, recordType, domain, NULL);
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
    static void dnsCmdRspCallbackHandler
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

static void dnsCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("DNS CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS dnsQueryName
    (
        const WDRV_WINC_DCPT *const pDcpt,
        uint8_t recordType,
        const char *pName,
        uint16_t timeoutMs
    )

  Summary:
    Queries the DNS resolver.

  Description:
    Queries the DNS resolver for the name given.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    pDcpt      - Pointer to device descriptor.
    recordType - The type of query record.
    pName      - Pointer to name to resolve.
    timeoutMs  - Timeout (in milliseconds).

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

static WDRV_WINC_STATUS dnsQueryName
(
    const WDRV_WINC_DCPT *const pDcpt,
    uint8_t recordType,
    const char *pName,
    uint16_t timeoutMs
)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t nameLen;

    /* Ensure the driver handle is valid. */
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pName))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    nameLen = strnlen(pName, WINC_CMD_PARAM_SZ_DNS_QUERY_NAME+1U);

    if ((0U == nameLen) || (nameLen > WINC_CMD_PARAM_SZ_DNS_QUERY_NAME))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(2, nameLen, &dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_TIMEOUT, WINC_TYPE_INTEGER_UNSIGNED, timeoutMs, sizeof(timeoutMs));

    (void)WINC_CmdDNSRESOLV(cmdReqHandle, recordType, (const uint8_t*)pName, nameLen);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver DNS Client Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_DNSProcessAEC
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
    See wdrv_winc_dns.h for usage information.

*/

void WDRV_WINC_DNSProcessAEC
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

    dnsProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, &dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_AUTO, WINC_TYPE_BOOL, (true == enabled) ? 1U : 0U, 1);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
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
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    uint8_t i;

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
    if (numServers > WINC_CFG_PARAM_NUM_DNS_DNS_SVR)
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

    cmdReqHandle = WDRV_WINC_CmdReqInit((unsigned int)1U+numServers, sizeof(WDRV_WINC_IPV6_ADDR)*numServers, &dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_SVR, WINC_TYPE_INTEGER, 0, 0);

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

            (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_SVR, typeVal, (uintptr_t)&pServerAddr[i].addr, lenVal);
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
    WDRV_WINC_STATUS WDRV_WINC_DNSProtoCfgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DNS_TRANS_TYPE transType,
        uint16_t port,
        WDRV_WINC_TLS_HANDLE tlsHandle,
        uint16_t idleConnTimeout
    )

  Summary:
    Configure DNS protocol.

  Description:
    Configures the type of protocol used for DNS connection and related settings.

  Remarks:
    See wdrv_winc_dns.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSProtoCfgSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DNS_TRANS_TYPE transType,
    uint16_t port,
    WDRV_WINC_TLS_HANDLE tlsHandle,
    uint16_t idleConnTimeout
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
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

    if (transType > WDRV_WINC_DNS_TRANS_TYPE_TLS)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((WDRV_WINC_DNS_TRANS_TYPE_TLS == transType) && (WDRV_WINC_TLS_INVALID_HANDLE == tlsHandle))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(4U, 0, &dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (WDRV_WINC_DNS_TRANS_TYPE_UDP == transType)
    {
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_CONN_PREF, WINC_TYPE_INTEGER_UNSIGNED, 0, 0);
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_TLS_CONF, WINC_TYPE_INTEGER_UNSIGNED, WDRV_WINC_TLS_INVALID_HANDLE, 0);
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_SRV_PORT, WINC_TYPE_INTEGER_UNSIGNED, port, 0);
    }
    else if (WDRV_WINC_DNS_TRANS_TYPE_TCP == transType)
    {
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_CONN_PREF, WINC_TYPE_INTEGER_UNSIGNED, 1, 0);
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_TLS_CONF, WINC_TYPE_INTEGER_UNSIGNED, WDRV_WINC_TLS_INVALID_HANDLE, 0);
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_SRV_PORT, WINC_TYPE_INTEGER_UNSIGNED, port, 0);
    }
    else /* WDRV_WINC_DNS_TRANS_TYPE_TLS == transType */
    {
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_CONN_PREF, WINC_TYPE_INTEGER_UNSIGNED, 1, 0);
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_TLS_CONF, WINC_TYPE_INTEGER_UNSIGNED, tlsHandle, 0);
        (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_SRV_SECURE_PORT, WINC_TYPE_INTEGER_UNSIGNED, port, 0);
    }

    (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_CONN_IDLE_TIMEOUT, WINC_TYPE_INTEGER_UNSIGNED, idleConnTimeout, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSCacheCfgSet
    (
        DRV_HANDLE handle,
        uint16_t maxTTL
    )

  Summary:
    Configure DNS cache.

  Description:
    Configures the DNS cache operation.

  Remarks:
    See wdrv_winc_dns.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSCacheCfgSet
(
    DRV_HANDLE handle,
    uint16_t maxTTL
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1U, 0, &dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_CACHE_TTL, WINC_TYPE_INTEGER_UNSIGNED, maxTTL, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSSECCfgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DNSSEC_MODE mode
    )

  Summary:
    Configure DNSSEC operation.

  Remarks:
    See wdrv_winc_dns.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSSECCfgSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DNSSEC_MODE mode
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
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

    if (mode > WDRV_WINC_DNSSEC_MODE_AUTH_ANS_ONLY)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1U, 0, &dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_DNSSEC_MODE, WINC_TYPE_INTEGER_UNSIGNED, mode, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSMulticastCfgSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DNS_MC_PREF_TYPE srvPref
    )

  Summary:
    Configure multicast DNS operation.

  Description:
    Configures the multicast DNS operation parameters.

  Remarks:
    See wdrv_winc_dns.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSMulticastCfgSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DNS_MC_PREF_TYPE srvPref
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
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

    if (srvPref > WDRV_WINC_DNS_MC_PREF_ALL_SRVS)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1U, 0, &dnsCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdDNSC(cmdReqHandle, WINC_CFG_PARAM_ID_DNS_DNS_MC_SRV_PREF, WINC_TYPE_INTEGER_UNSIGNED, srvPref, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
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
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WDRV_WINC_STATUS status;

    /* Check type */
    if ((WINC_CONST_DNS_TYPE_A != recordType) && (WINC_CONST_DNS_TYPE_AAAA != recordType))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    status = dnsQueryName((const WDRV_WINC_DCPT *const)handle, recordType, pDomainName, timeoutMs);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    pDcpt->pCtrl->pfDNSResolveResponseCB  = pfDNSResolveResponseCB;
    pDcpt->pCtrl->pfDNSResQueryResponseCB = NULL;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DNSResourceQuery
    (
        DRV_HANDLE handle,
        uint8_t type,
        const char *pQueryName,
        uint16_t timeoutMs,
        const WDRV_WINC_DNS_QUERY_RESOURCE_CALLBACK pfDNSResQueryResponseCB
    )

  Summary:
    Queries the DNS resolver.

  Description:
    Queries the DNS resolver for the named resource.

  Remarks:
    See wdrv_winc_dns.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_DNSResourceQuery
(
    DRV_HANDLE handle,
    uint8_t recordType,
    const char *pQueryName,
    uint16_t timeoutMs,
    const WDRV_WINC_DNS_QUERY_RESOURCE_CALLBACK pfDNSResQueryResponseCB
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;
    WDRV_WINC_STATUS status;

    status = dnsQueryName((const WDRV_WINC_DCPT *const)handle, recordType, pQueryName, timeoutMs);

    if (WDRV_WINC_STATUS_OK != status)
    {
        return status;
    }

    pDcpt->pCtrl->pfDNSResQueryResponseCB = pfDNSResQueryResponseCB;
    pDcpt->pCtrl->pfDNSResolveResponseCB  = NULL;

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_DNS */
