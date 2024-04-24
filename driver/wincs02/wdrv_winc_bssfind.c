/*******************************************************************************
  WINC Wireless Driver

  File Name:
    wdrv_winc_bssfind.c

  Summary:
    WINC wireless driver.

  Description:
    WINC wireless driver.
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

#include <stdint.h>
#include <string.h>

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_bssfind.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Find Defines
// *****************************************************************************
// *****************************************************************************

/* Number of scan results to store in cache. */
#define WDRV_WINC_SCAN_RESULT_CACHE_NUM_ENTRIES     50

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Find Data Types
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
    int                         numDescrs;

    /* Scan result entry, one per AP. */
    DRV_WINC_SCAN_RESULTS       bssDescr[WDRV_WINC_SCAN_RESULT_CACHE_NUM_ENTRIES];
} WDRV_WINC_SCAN_RESULT_CACHE;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Find Global Data
// *****************************************************************************
// *****************************************************************************

/* WiFi scan result cache. */
static WDRV_WINC_SCAN_RESULT_CACHE scanResultCache;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BSS Find Implementations
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void bssfindWSCNProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)

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

static void bssfindWSCNProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    WDRV_WINC_CTRLDCPT *pCtrl;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    pCtrl = pDcpt->pCtrl;

    switch (pElems->rspId)
    {
        case WINC_CMD_ID_WSCNC:
        {
            uint8_t id;

            if (pElems->numElems != 2)
            {
                return;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &id, sizeof(id));

            switch (id)
            {
                case WINC_CFG_PARAM_ID_WSCN_ACT_SLOT_TIME:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->scanActiveScanTime, sizeof(pCtrl->scanActiveScanTime));
                    break;
                }

                case WINC_CFG_PARAM_ID_WSCN_PASV_SLOT_TIME:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->scanPassiveListenTime, sizeof(pCtrl->scanPassiveListenTime));
                    break;
                }

                case WINC_CFG_PARAM_ID_WSCN_NUM_SLOTS:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->scanNumSlots, sizeof(pCtrl->scanNumSlots));
                    break;
                }

                case WINC_CFG_PARAM_ID_WSCN_PROBES_PER_SLOT:
                {
                    WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pCtrl->scanNumProbes, sizeof(pCtrl->scanNumProbes));
                    break;
                }

                default:
                {
                    break;
                }
            }

            WDRV_DBG_VERBOSE_PRINT("Scan Params: %d %d %d %d\n", pCtrl->scanActiveScanTime, pCtrl->scanPassiveListenTime, pCtrl->scanNumSlots, pCtrl->scanNumProbes);
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
    static void bssfindWSCNCmdRspCallbackHandler
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

static void bssfindWSCNCmdRspCallbackHandler
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

    //WDRV_DBG_INFORM_PRINT("WSCN CmdRspCB %08x Event %d\n", cmdReqHandle, event);

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
//            WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (WINC_DEV_EVENT_STATUS_ARGS*)eventArg;
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                bssfindWSCNProcessCmdRsp((DRV_HANDLE)pDcpt, pRspElems);
            }
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_WSCNProcessAEC
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
    See wdrv_winc_bssfind.h for usage information.

*/

void WDRV_WINC_WSCNProcessAEC(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_DEV_EVENT_RSP_ELEMS *pElems)
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
        case WINC_AEC_ID_WSCNIND:
        {
            if (pElems->numElems != 5)
            {
                return;
            }

            if (scanResultCache.numDescrs < WDRV_WINC_SCAN_RESULT_CACHE_NUM_ENTRIES)
            {
                DRV_WINC_SCAN_RESULTS *pScanRes = &scanResultCache.bssDescr[scanResultCache.numDescrs];

                WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &pScanRes->rssi, sizeof(pScanRes->rssi));
                WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pScanRes->authTypeRecommended, sizeof(pScanRes->authTypeRecommended));
                WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pScanRes->channel, sizeof(pScanRes->channel));
                WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_MACADDR, pScanRes->bssid.addr, WDRV_WINC_MAC_ADDR_LEN);
                WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_STRING,  pScanRes->ssid.name, WDRV_WINC_MAX_SSID_LEN);

                pScanRes->bssid.valid = true;
                pScanRes->ssid.length = pElems->elems[4].length;

                scanResultCache.numDescrs++;
            }

            break;
        }

        case WINC_AEC_ID_WSCNDONE:
        {
            uint8_t nResults = 0;

            if (pElems->numElems != 1)
            {
                return;
            }

            pCtrl->scanInProgress = false;

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &nResults, sizeof(nResults));

            if (NULL != pCtrl->pfBSSFindNotifyCB)
            {
                if (0 == nResults)
                {
                    pCtrl->pfBSSFindNotifyCB(pCtrl->handle, 0, 0, NULL);
                    pCtrl->pfBSSFindNotifyCB = NULL;
                    break;
                }

                /* Reuse find next function by pre-decrementing scan index. */
                pCtrl->scanIndex--;

                WDRV_WINC_BSSFindNext(pCtrl->handle, pCtrl->pfBSSFindNotifyCB);
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
    void *pCmdReqBuffer;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure request channel is valid. */
    if (channel > WDRV_WINC_CID_2_4G_CH13)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure SSID list is only provided for active scans. */
    if ((false == active) && (NULL != pSSIDList))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if ((false == pDcpt->isOpen) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Convert public API representation of all channels to
       internal representation. */
    if (WDRV_WINC_CID_ANY == channel)
    {
    }
    else
    {
        if(!((1<<(channel-1)) & pDcpt->pCtrl->scanChannelMask24))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }
    }

    if (NULL != pSSIDList)
    {
    }

    scanResultCache.numDescrs = 0;

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 2, bssfindWSCNCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_CHANNEL, WINC_TYPE_INTEGER, channel, 0);
    WINC_CmdWSCN(cmdReqHandle, active ? WINC_CONST_WSCN_ACT_PASV_ACTIVE : WINC_CONST_WSCN_ACT_PASV_PASSIVE);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
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
    if ((false == pDcpt->isOpen) || (NULL == pDcpt->pCtrl) || (DRV_HANDLE_INVALID == pDcpt->pCtrl->handle))
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
        bool findResult;

        while (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSFindGetInfo(handle, &bssInfo))
        {
            /* Notify the user application of the scan results. */
            findResult = pDcpt->pCtrl->pfBSSFindNotifyCB(handle, pDcpt->pCtrl->scanIndex+1, scanResultCache.numDescrs, &bssInfo);

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
    pDcpt->pCtrl->scanIndex = -1;

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

    if (0 == scanResultCache.numDescrs)
    {
        return WDRV_WINC_STATUS_NO_BSS_INFO;
    }

    /* Copy BSS scan cache to user supplied buffer. */
    pBSSInfo->ctx.channel         = pLastBSSScanInfo->channel;
    pBSSInfo->rssi                = pLastBSSScanInfo->rssi;
    pBSSInfo->ctx.cloaked         = false;
    pBSSInfo->authTypeRecommended = pLastBSSScanInfo->authTypeRecommended;

    memcpy(&pBSSInfo->ctx.bssid, &pLastBSSScanInfo->bssid, sizeof(WDRV_WINC_MAC_ADDR));
    memcpy(&pBSSInfo->ctx.ssid,  &pLastBSSScanInfo->ssid,  sizeof(WDRV_WINC_SSID));

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
    void *pCmdReqBuffer;

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
    if ((0 != activeSlotTime) && (pCtrl->scanActiveScanTime != activeSlotTime))
    {
        if ((activeSlotTime > DRV_WINC_MAX_SCAN_TIME) || (activeSlotTime < DRV_WINC_MIN_SCAN_TIME))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        pCtrl->scanActiveScanTime = activeSlotTime;
    }

    /* Check for update to Passive Scan Time. */
    if ((0 != passiveSlotTime) && (pCtrl->scanPassiveListenTime != passiveSlotTime))
    {
        if ((passiveSlotTime > DRV_WINC_MAX_SCAN_TIME) || (passiveSlotTime < DRV_WINC_MIN_SCAN_TIME))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        pCtrl->scanPassiveListenTime = passiveSlotTime;
    }

    /* Check for update to Number of Slots. */
    if ((0 != numSlots) && (pCtrl->scanNumSlots != numSlots))
    {
        pCtrl->scanNumSlots = numSlots;
    }

    /* Check for update to Number of Probes. */
    if ((0 != numProbes) && (pCtrl->scanNumProbes != numProbes))
    {
        if ((numProbes > DRV_WINC_SCAN_MAX_NUM_PROBE) || (numProbes < DRV_WINC_SCAN_MIN_NUM_PROBE))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        pCtrl->scanNumProbes = numProbes;
    }

    pCmdReqBuffer = OSAL_Malloc(512);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 512, 4, bssfindWSCNCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (0 != pCtrl->scanActiveScanTime)
    {
        WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_ACT_SLOT_TIME, WINC_TYPE_INTEGER, pCtrl->scanActiveScanTime, 0);
    }

    if (0 != pCtrl->scanPassiveListenTime)
    {
        WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_PASV_SLOT_TIME, WINC_TYPE_INTEGER, pCtrl->scanPassiveListenTime, 0);
    }

    if (0 != pCtrl->scanNumSlots)
    {
        WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_NUM_SLOTS, WINC_TYPE_INTEGER, pCtrl->scanNumSlots, 0);
    }

    if (0 != pCtrl->scanNumProbes)
    {
        WINC_CmdWSCNC(cmdReqHandle, WINC_CFG_PARAM_ID_WSCN_PROBES_PER_SLOT, WINC_TYPE_INTEGER, pCtrl->scanNumProbes, 0);
    }

    if (false == WINC_DevTransmitCmdReq(pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
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

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BSSFindSetEnabledChannels24
    (
        DRV_HANDLE handle,
        WDRV_WINC_CHANNEL_MASK channelMask24
    )

  Summary:
    Set the enabled channels list for 2.4GHz.

  Description:
    To comply with regulatory domains certain channels must not be scanned.
      This function configures which channels are enabled to be used.

  Remarks:
    See wdrv_winc_bssfind.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BSSFindSetEnabledChannels24
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

    if (0 != (channelMask24 & ~WDRV_WINC_CM_2_4G_ALL))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pDcpt->pCtrl->scanChannelMask24 = channelMask24;

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
