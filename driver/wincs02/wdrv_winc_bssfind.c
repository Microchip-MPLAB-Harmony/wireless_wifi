/*******************************************************************************
  WINC Wireless Driver BSS Scanning Source File

  File Name:
    wdrv_winc_bssfind.c

  Summary:
    WINC wireless driver BSS scanning implementation.

  Description:
    This interface manages the operation of searching for a BSS.
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

#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_bssfind.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Scanning Defines
// *****************************************************************************
// *****************************************************************************

/* Number of scan results to store in cache. */
#define WDRV_WINC_SCAN_RESULT_CACHE_NUM_ENTRIES     50u

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Scanning Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* WiFi Scan Results.

  Summary:
    Structure to store WiFi scan results.

  Description:
    Contains details of a discovered AP found during scanning.

  Remarks:
    None.
*/

typedef struct
{
    /* RSSI of received packets. */
    int8_t              rssi;

    /* Recommended authentication type. */
    uint8_t             authTypeRecommended;

    /* Channel AP was found on. */
    uint8_t             channel;

    /* BSS ID of AP. */
    WDRV_WINC_MAC_ADDR  bssid;

    /* SSID of AP. */
    WDRV_WINC_SSID      ssid;
} DRV_WINC_SCAN_RESULTS;

// *****************************************************************************
/* WiFi Scan Cache.

  Summary:
    Cache to store WiFi scan results.

  Description:
    WiFi scan results are stored in the cache for retrieval by the applicaiton.

  Remarks:
    None.
*/

typedef struct
{
    /* Number of valid cache entries. */
    uint8_t                     numDescrs;

    /* Scan result entry, one per AP. */
    DRV_WINC_SCAN_RESULTS       bssDescr[WDRV_WINC_SCAN_RESULT_CACHE_NUM_ENTRIES];
} WDRV_WINC_SCAN_RESULT_CACHE;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Scanning Local Data
// *****************************************************************************
// *****************************************************************************

/* WiFi scan result cache. */
static WDRV_WINC_SCAN_RESULT_CACHE scanResultCache;

static WDRV_WINC_AUTH_TYPE secTypeAuthTypeMap[] =
{
    /* SEC_TYPE_OPEN */             WDRV_WINC_AUTH_TYPE_OPEN,
    /* Default auth type */         WDRV_WINC_AUTH_TYPE_DEFAULT,
    /* SEC_TYPE_WPA2_PERS_MIXED */  WDRV_WINC_AUTH_TYPE_WPAWPA2_PERSONAL,
    /* SEC_TYPE_WPA2_PERS */        WDRV_WINC_AUTH_TYPE_WPA2_PERSONAL,
    /* SEC_TYPE_WPA3_PERS_TRANS */  WDRV_WINC_AUTH_TYPE_WPA2WPA3_PERSONAL,
    /* SEC_TYPE_WPA3_PERS */        WDRV_WINC_AUTH_TYPE_WPA3_PERSONAL,
    /* SEC_TYPE_WPA2_ENT_MIXED */   WDRV_WINC_AUTH_TYPE_DEFAULT,
    /* SEC_TYPE_WPA2_ENT */         WDRV_WINC_AUTH_TYPE_DEFAULT,
    /* SEC_TYPE_WPA3_ENT_TRANS */   WDRV_WINC_AUTH_TYPE_DEFAULT,
    /* SEC_TYPE_WPA3_ENT */         WDRV_WINC_AUTH_TYPE_DEFAULT
};

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Scanning Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void wscnProcessCmdRsp
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

static void wscnProcessCmdRsp
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t rspId,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    WDRV_WINC_CTRLDCPT *pCtrl;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems) || (NULL == pSrcCmd))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (rspId)
    {
        case WINC_CMD_ID_WSCNC:
        {
            uint8_t id;

            if (2U != numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &id, sizeof(id));

            switch (id)
            {
                case WINC_CFG_PARAM_ID_WSCN_ACT_SLOT_TIME:
                {
                    (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &pCtrl->scanActiveScanTime, sizeof(pCtrl->scanActiveScanTime));
                    break;
                }

                case WINC_CFG_PARAM_ID_WSCN_PASV_SLOT_TIME:
                {
                    (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &pCtrl->scanPassiveListenTime, sizeof(pCtrl->scanPassiveListenTime));
                    break;
                }

                case WINC_CFG_PARAM_ID_WSCN_NUM_SLOTS:
                {
                    (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &pCtrl->scanNumSlots, sizeof(pCtrl->scanNumSlots));
                    break;
                }

                case WINC_CFG_PARAM_ID_WSCN_PROBES_PER_SLOT:
                {
                    (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &pCtrl->scanNumProbes, sizeof(pCtrl->scanNumProbes));
                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("WSCNC ID %d not handled\r\n", id);
                    break;
                }
            }

            WDRV_DBG_VERBOSE_PRINT("Scan Params: %d %d %d %d\r\n", pCtrl->scanActiveScanTime, pCtrl->scanPassiveListenTime, pCtrl->scanNumSlots, pCtrl->scanNumProbes);
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
    static void wscnProcessAEC
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

static void wscnProcessAEC
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
        case WINC_AEC_ID_WSCNIND:
        {
            if (5U != numElems)
            {
                break;
            }

            if (scanResultCache.numDescrs < WDRV_WINC_SCAN_RESULT_CACHE_NUM_ENTRIES)
            {
                DRV_WINC_SCAN_RESULTS *pScanRes = &scanResultCache.bssDescr[scanResultCache.numDescrs];
                uint8_t secType;
                WDRV_WINC_AUTH_TYPE authTypeRecommended = WDRV_WINC_AUTH_TYPE_DEFAULT;

                if (WDRV_WINC_SCAN_MATCH_MODE_STOP_ON_FIRST == pDcpt->pCtrl->scanMatchMode)
                {
                    if (scanResultCache.numDescrs > 0)
                    {
                        break;
                    }
                }

                (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &secType, sizeof(secType));

                if (secType < sizeof(secTypeAuthTypeMap))
                {
                    authTypeRecommended = secTypeAuthTypeMap[secType];
                }

                if (WDRV_WINC_AUTH_TYPE_DEFAULT != authTypeRecommended)
                {
                    (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &pScanRes->rssi, sizeof(pScanRes->rssi));
                    (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_INTEGER, &pScanRes->channel, sizeof(pScanRes->channel));
                    (void)WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_MACADDR, pScanRes->bssid.addr, WDRV_WINC_MAC_ADDR_LEN);
                    (void)WINC_CmdReadParamElem(&pElems[4], WINC_TYPE_STRING,  pScanRes->ssid.name, WDRV_WINC_MAX_SSID_LEN);

                    pScanRes->authTypeRecommended = (uint8_t)secTypeAuthTypeMap[secType];
                    pScanRes->bssid.valid = true;
                    pScanRes->ssid.length = (uint8_t)pElems[4].length;

                    scanResultCache.numDescrs++;
                }
            }

            break;
        }

        case WINC_AEC_ID_WSCNDONE:
        {
            uint8_t nResults = 0;

            if (1U != numElems)
            {
                break;
            }

            pCtrl->scanInProgress = false;

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &nResults, sizeof(nResults));

            if (NULL != pCtrl->pfBSSFindNotifyCB)
            {
                if (0U == nResults)
                {
                    WDRV_WINC_BSSFIND_NOTIFY_CALLBACK pfBSSFindNotifyCB = pCtrl->pfBSSFindNotifyCB;

                    pCtrl->pfBSSFindNotifyCB = NULL;
                    (void)pfBSSFindNotifyCB(pCtrl->handle, 0, 0, NULL);
                    break;
                }

                /* Reuse find next function by pre-decrementing scan index. */
                pCtrl->scanIndex--;

                (void)WDRV_WINC_BSSFindNext(pCtrl->handle, pCtrl->pfBSSFindNotifyCB);
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
    static void wscnCmdRspCallbackHandler
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

static void wscnCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("WSCN CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                wscnProcessCmdRsp(pDcpt, pRspElems->rspId, cmdReqHandle, &pRspElems->srcCmd, pRspElems->numElems, pRspElems->elems);
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
// Section: WINC Driver BSS Scanning Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_WSCNProcessAEC
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
    See wdrv_winc_bssfind.h for usage information.

*/

void WDRV_WINC_WSCNProcessAEC
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

    wscnProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindFirst
    (
        DRV_HANDLE handle,
        WDRV_WINC_CHANNEL_ID channel,
        bool active,
        const WDRV_WINC_SSID_LIST *const pSSIDList,
        const WDRV_WINC_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
    )

  Summary:
    Request a BSS scan is performed by the WINC.

  Description:
    A scan is requested on the specified channels. An optional callback can
      be provided to receive notification of the first BSS discovered.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindFirst
(
    DRV_HANDLE handle,
    WDRV_WINC_CHANNEL_ID channel,
    bool active,
    const WDRV_WINC_SSID_LIST *const pSSIDList,
    const WDRV_WINC_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    int numSSIDInList = 0;
    size_t ssidListSize = 0;
    const WDRV_WINC_SSID_LIST *pSSIDListIter;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Ensure requested channel is valid. */
    if (channel > WDRV_WINC_CID_2_4G_CH13)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure requested channel is allowed */
    if (WDRV_WINC_CID_ANY != channel)
    {
        if (0U == (((uint16_t)1U<<((unsigned)channel-1U)) & (uint16_t)pDcpt->pCtrl->regulatoryChannelMask24))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }

    /* Ensure SSID list is only provided for active scans. */
    if ((false == active) && (NULL != pSSIDList))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if (NULL != pSSIDList)
    {
        pSSIDListIter = pSSIDList;

        while (NULL != pSSIDListIter)
        {
            numSSIDInList++;

            ssidListSize += (int)pSSIDListIter->ssid.length;

            pSSIDListIter = pSSIDListIter->pNext;
        }
    }

    scanResultCache.numDescrs = 0;

    cmdReqHandle = WDRV_WINC_CmdReqInit((unsigned int)5+numSSIDInList, ssidListSize, wscnCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_CHANMASK24, WINC_TYPE_INTEGER, (uintptr_t)pDcpt->pCtrl->scanChannelMask24, 0);

    (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_CHANNEL, WINC_TYPE_INTEGER, (uintptr_t)channel, 0);

    (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_RSSI_THRESH, WINC_TYPE_INTEGER, (uintptr_t)pDcpt->pCtrl->scanRssiThreshold, 0);

    (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_FILT_LIST, WINC_TYPE_STRING, (uintptr_t)"", 0);

    pSSIDListIter = pSSIDList;
    while ((0 != (numSSIDInList--)) && (NULL != pSSIDListIter))
    {
        (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_FILT_LIST, WINC_TYPE_STRING, (uintptr_t)&pSSIDListIter->ssid.name, pSSIDListIter->ssid.length);

        pSSIDListIter = pSSIDListIter->pNext;
    };

    (void)WINC_CmdWSCN(cmdReqHandle, active ? WINC_CONST_WSCN_ACT_PASV_ACTIVE : WINC_CONST_WSCN_ACT_PASV_PASSIVE);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->scanInProgress    = true;
    pDcpt->pCtrl->scanIndex         = 0;
    pDcpt->pCtrl->pfBSSFindNotifyCB = pfNotifyCallback;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindNext
    (
        DRV_HANDLE handle,
        WDRV_WINC_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
    )

  Summary:
    Request the next scan results be provided.

  Description:
    The information structure of the next BSS is requested from the WINC.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindNext
(
    DRV_HANDLE handle,
    WDRV_WINC_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Cannot request results while a scan is in progress. */
    if (true == pDcpt->pCtrl->scanInProgress)
    {
        return WDRV_WINC_STATUS_SCAN_IN_PROGRESS;
    }

    /* Check if the request would exceed the number of results
       available, signal find operation end if so. */
    pDcpt->pCtrl->scanIndex++;

    if (pDcpt->pCtrl->scanIndex >= scanResultCache.numDescrs)
    {
        pDcpt->pCtrl->scanIndex--;

        return WDRV_WINC_STATUS_BSS_FIND_END;
    }

    /* Store callback supplied. */
    pDcpt->pCtrl->pfBSSFindNotifyCB = pfNotifyCallback;

    if (NULL != pDcpt->pCtrl->pfBSSFindNotifyCB)
    {
        WDRV_WINC_BSS_INFO bssInfo;

        while (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSFindGetInfo(handle, &bssInfo))
        {
            /* Notify the user application of the scan results. */
            bool findResult = pDcpt->pCtrl->pfBSSFindNotifyCB(handle, pDcpt->pCtrl->scanIndex+1U, scanResultCache.numDescrs, &bssInfo);

            /* Check if the callback requested the next set of results. */
            if (true == findResult)
            {
                /* Request the next BSS results, or end operation if no
                   more are available. */
                pDcpt->pCtrl->scanIndex++;

                if (pDcpt->pCtrl->scanIndex >= scanResultCache.numDescrs)
                {
                    pDcpt->pCtrl->pfBSSFindNotifyCB = NULL;
                    break;
                }
            }
            else
            {
                pDcpt->pCtrl->pfBSSFindNotifyCB = NULL;
                break;
            }
        }
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindReset
    (
        DRV_HANDLE handle,
        WDRV_WINC_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
    )

  Summary:
    Request the first scan results again

  Description:
    The information structure of the first BSS is requested from the WINC.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindReset
(
    DRV_HANDLE handle,
    WDRV_WINC_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Cannot reset the find operation while a scan is in progress. */
    if (true == pDcpt->pCtrl->scanInProgress)
    {
        return WDRV_WINC_STATUS_SCAN_IN_PROGRESS;
    }

    /* Reuse find next function by pre-decrementing scan index. */
    pDcpt->pCtrl->scanIndex = UCHAR_MAX;

    return WDRV_WINC_BSSFindNext(handle, pfNotifyCallback);
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindGetInfo
    (
        DRV_HANDLE handle,
        WDRV_WINC_BSS_INFO *const pBSSInfo
    )

  Summary:
    Requests the information structure of the current BSS scan result.

  Description:
    After each call to either WDRV_WINC_BSSFindFirst or WDRV_WINC_BSSFindNext
      the driver receives a single BSS information structure which it stores.
      This function retrieves that structure.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindGetInfo
(
    DRV_HANDLE handle,
    WDRV_WINC_BSS_INFO *const pBSSInfo
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    DRV_WINC_SCAN_RESULTS *pLastBSSScanInfo;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pBSSInfo))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (pDcpt->pCtrl->scanIndex >= scanResultCache.numDescrs)
    {
        return WDRV_WINC_STATUS_NO_BSS_INFO;
    }

    pLastBSSScanInfo = &scanResultCache.bssDescr[pDcpt->pCtrl->scanIndex];

    if (0U == scanResultCache.numDescrs)
    {
        return WDRV_WINC_STATUS_NO_BSS_INFO;
    }

    /* Copy BSS scan cache to user supplied buffer. */
    pBSSInfo->ctx.channel         = (WDRV_WINC_CHANNEL_ID)pLastBSSScanInfo->channel;
    pBSSInfo->rssi                = pLastBSSScanInfo->rssi;
    pBSSInfo->authTypeRecommended = (WDRV_WINC_AUTH_TYPE)pLastBSSScanInfo->authTypeRecommended;

    (void)memcpy(&pBSSInfo->ctx.bssid, &pLastBSSScanInfo->bssid, sizeof(WDRV_WINC_MAC_ADDR));
    (void)memcpy(&pBSSInfo->ctx.ssid,  &pLastBSSScanInfo->ssid,  sizeof(WDRV_WINC_SSID));

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindSetScanParameters
    (
        DRV_HANDLE handle,
        uint8_t numSlots,
        uint8_t activeSlotTime,
        uint8_t passiveSlotTime,
        uint8_t numProbes
    )

  Summary:
    Configures the scan operation.

  Description:
    The scan consists of a number of slots per channel, each slot can has a
      configurable time period and number of probe requests to send.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindSetScanParameters
(
    DRV_HANDLE handle,
    uint8_t numSlots,
    uint16_t activeSlotTime,
    uint16_t passiveSlotTime,
    uint8_t numProbes
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WDRV_WINC_CTRLDCPT *pCtrl;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pCtrl = pDcpt->pCtrl;

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pCtrl->handle))
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Check for update to Active Scan Time. */
    if ((0U != activeSlotTime) && (pCtrl->scanActiveScanTime != activeSlotTime))
    {
        if ((activeSlotTime > DRV_WINC_MAX_SCAN_TIME) || (activeSlotTime < DRV_WINC_MIN_SCAN_TIME))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        pCtrl->scanActiveScanTime = activeSlotTime;
    }

    /* Check for update to Passive Scan Time. */
    if ((0U != passiveSlotTime) && (pCtrl->scanPassiveListenTime != passiveSlotTime))
    {
        if ((passiveSlotTime > DRV_WINC_MAX_SCAN_TIME) || (passiveSlotTime < DRV_WINC_MIN_SCAN_TIME))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        pCtrl->scanPassiveListenTime = passiveSlotTime;
    }

    /* Check for update to Number of Slots. */
    if ((0U != numSlots) && (pCtrl->scanNumSlots != numSlots))
    {
        pCtrl->scanNumSlots = numSlots;
    }

    /* Check for update to Number of Probes. */
    if ((0U != numProbes) && (pCtrl->scanNumProbes != numProbes))
    {
        if ((numProbes > DRV_WINC_SCAN_MAX_NUM_PROBE) || (numProbes < DRV_WINC_SCAN_MIN_NUM_PROBE))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        pCtrl->scanNumProbes = numProbes;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(4, 0, wscnCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (0U != pCtrl->scanActiveScanTime)
    {
        (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_ACT_SLOT_TIME, WINC_TYPE_INTEGER, pCtrl->scanActiveScanTime, 0);
    }

    if (0U != pCtrl->scanPassiveListenTime)
    {
        (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_PASV_SLOT_TIME, WINC_TYPE_INTEGER, pCtrl->scanPassiveListenTime, 0);
    }

    if (0U != pCtrl->scanNumSlots)
    {
        (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_NUM_SLOTS, WINC_TYPE_INTEGER, pCtrl->scanNumSlots, 0);
    }

    if (0U != pCtrl->scanNumProbes)
    {
        (void)WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_PROBES_PER_SLOT, WINC_TYPE_INTEGER, pCtrl->scanNumProbes, 0);
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
    WDRV_WINC_STATUS WDRV_WINC_BSSFindSetRSSIThreshold
    (
        DRV_HANDLE handle,
        int8_t rssiThreshold
    )

  Summary:
    Configures the scan RSSI threshold.

  Description:
    The RSSI threshold required for reconnection to an AP without scanning
    all channels first.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindSetRSSIThreshold
(
    DRV_HANDLE handle,
    int8_t rssiThreshold
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pDcpt->pCtrl->scanRssiThreshold = rssiThreshold;

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindSetScanChannels24
    (
        DRV_HANDLE handle,
        WDRV_WINC_CHANNEL_MASK channelMask24
    )

  Summary:
    Set the multi-channel list for scanning in 2.4GHz.

  Description:
    In some scenarios an application may want to scan on a subset of channels.
      This function configures which channels are set to be scanned when issuing
      a multi-channel scan (when WDRV_WINC_CID_ANY is passed as channel).

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindSetScanChannels24
(
    DRV_HANDLE handle,
    WDRV_WINC_CHANNEL24_MASK channelMask24
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Check the proposed channel mask is within regulatory limits */
    if ((uint16_t)WDRV_WINC_CM_2_4G_NONE != ((uint16_t)channelMask24 & ~(uint16_t)pDcpt->pCtrl->regulatoryChannelMask24))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pDcpt->pCtrl->scanChannelMask24 = channelMask24;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindGetScanChannels24
    (
        DRV_HANDLE handle,
        WDRV_WINC_CHANNEL24_MASK *const pChannelMask
    )

  Summary:
    Retrieves the multi-channel list for scanning in 2.4GHz.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindGetScanChannels24
(
    DRV_HANDLE handle,
    WDRV_WINC_CHANNEL24_MASK *const pChannelMask
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pChannelMask))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure pointer is valid */
    if (NULL == pDcpt->pCtrl)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    *pChannelMask = pDcpt->pCtrl->scanChannelMask24;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    uint8_t WDRV_WINC_BSSFindGetNumBSSResults(DRV_HANDLE handle)

  Summary:
    Returns the number of BSS scan results found.

  Description:
    Returns the number of BSS scan results found.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

uint8_t WDRV_WINC_BSSFindGetNumBSSResults(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid and the instance is open.
       Ensure a scan is not already in progress. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return 0;
    }

    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
    {
        return 0;
    }

    /* Return the number of BSSs found. */
    return scanResultCache.numDescrs;
}

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_BSSFindInProgress(DRV_HANDLE handle)

  Summary:
    Indicates if a BSS scan is in progress.

  Description:
    Returns a flag indicating if a BSS scan operation is currently running.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

bool WDRV_WINC_BSSFindInProgress(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid and the instance is open. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return false;
    }

    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
    {
        return false;
    }

    return pDcpt->pCtrl->scanInProgress;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindSetScanMatchMode
    (
        DRV_HANDLE handle,
        WDRV_WINC_SCAN_MATCH_MODE matchMode
    )

  Summary:
    Configures the scan matching mode.

  Description:
    This function configures the matching mode, either stop on first or
      match all, used when scanning for SSIDs.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindSetScanMatchMode
(
    DRV_HANDLE handle,
    WDRV_WINC_SCAN_MATCH_MODE matchMode
)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure pointer is valid */
    if (NULL == pDcpt->pCtrl)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pDcpt->pCtrl->scanMatchMode = matchMode;

    return WDRV_WINC_STATUS_OK;
}
