/*******************************************************************************
  WINC Wireless Driver Wifi Configuration Implementation

  File Name:
    wdrv_winc_wifi.c

  Summary:
    WINC wireless driver Wifi Configuration implementation.

  Description:
    This interface provides functionality required for configuring the Wifi,
    this includes powersave mode, regulatory domain and coex settings.
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
#include "wdrv_winc_wifi.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Wifi Configuration Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void wifiProcessCoexElem
    (
        DRV_HANDLE handle,
        uint16_t id,
        WINC_DEV_PARAM_ELEM *pElem
    )

  Summary:
    Process command/response element flags for Coex.

  Description:
    Processing both the command to change and the query response for coex
    settings by decoding the boolean value and setting the internal state.

  Precondition:
    WINC_DevTransmitCmdReq must of been called to submit command request.

  Parameters:
    handle - WINC device handle.
    id     - WIFIC parameter ID.
    pElem  - Pointer to command or response element for the WIFIC value.

  Returns:
    None.

  Remarks:
    None.

*/

static void wifiProcessCoexElem
(
    DRV_HANDLE handle,
    uint16_t id,
    WINC_DEV_PARAM_ELEM *pElem
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    bool flag;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElem))
    {
        return;
    }

    WINC_CmdReadParamElem(pElem, WINC_TYPE_BOOL, &flag, sizeof(flag));

    switch (id)
    {
        case WINC_CFG_PARAM_ID_WIFI_COEX_ENABLED:
        {
            pDcpt->pCtrl->coex.enabled = (true == flag) ? 1 : 0;
            pDcpt->pCtrl->coex.confValid = true;
            break;
        }

        case WINC_CFG_PARAM_ID_WIFI_COEX_INTERFACE_TYPE:
        {
            pDcpt->pCtrl->coex.use2Wire = (true == flag) ? 1 : 0;
            break;
        }

        case WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_RX_VS_BT:
        {
            pDcpt->pCtrl->coex.wlanRxHigherThanBt = (true == flag) ? 1 : 0;
            break;
        }

        case WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_TX_VS_BT:
        {
            pDcpt->pCtrl->coex.wlanTxHigherThanBt = (true == flag) ? 1 : 0;
            break;
        }

        case WINC_CFG_PARAM_ID_WIFI_COEX_ANTENNA_MODE:
        {
            pDcpt->pCtrl->coex.sharedAntenna = (true == flag) ? 1 : 0;
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
    static void wifiProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)

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

static void wifiProcessCmdRsp(DRV_HANDLE handle, WINC_DEV_EVENT_RSP_ELEMS *pElems)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_CMD_ID_WIFIC:
        {
            WINC_DEV_FRACT_INT_TYPE id;

            if (pElems->numElems != 2)
            {
                return;
            }

            WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

            switch (id.i)
            {
                case WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_SELECTED:
                {
                    pDcpt->pCtrl->activeRegDomain.regDomainLen = WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_STRING, pDcpt->pCtrl->activeRegDomain.regDomain, WDRV_WINC_REGDOMAIN_MAX_NAME_LEN);

                    if (NULL != pDcpt->pCtrl->pfRegDomainEventCB)
                    {
                        pDcpt->pCtrl->pfRegDomainEventCB((DRV_HANDLE)pDcpt, 1, 1, true, &pDcpt->pCtrl->activeRegDomain);
                    }
                    break;
                }

                case WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_AVAILABLE:
                {
                    if (NULL == pDcpt->pCtrl->pfRegDomainEventCB)
                    {
                        return;
                    }

                    if (-1 == id.f)
                    {
                        WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pDcpt->pCtrl->availRegDomNum, sizeof(pDcpt->pCtrl->availRegDomNum));
                    }
                    else
                    {
                        WDRV_WINC_REGDOMAIN_INFO regDomInfo;
                        uint8_t index = id.f+1;
                        bool active = false;

                        regDomInfo.regDomainLen = WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_STRING, regDomInfo.regDomain, WDRV_WINC_REGDOMAIN_MAX_NAME_LEN);

                        if (index <= pDcpt->pCtrl->availRegDomNum)
                        {
                            if (0 == memcmp(regDomInfo.regDomain, pDcpt->pCtrl->activeRegDomain.regDomain, WDRV_WINC_REGDOMAIN_MAX_NAME_LEN))
                            {
                                active = true;
                            }

                            pDcpt->pCtrl->pfRegDomainEventCB((DRV_HANDLE)pDcpt, index, pDcpt->pCtrl->availRegDomNum, active, &regDomInfo);
                        }
                    }

                    break;
                }

                case WINC_CFG_PARAM_ID_WIFI_POWERSAVE:
                {
                    WDRV_WINC_POWERSAVE_INFO powersaveInfo;

                    if (NULL != pDcpt->pCtrl->pfPowersaveEventCB)
                    {
                        WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &powersaveInfo.psMode, sizeof(powersaveInfo.psMode));

                        pDcpt->pCtrl->pfPowersaveEventCB((DRV_HANDLE)pDcpt, &powersaveInfo);
                    }

                    break;
                }

                case WINC_CFG_PARAM_ID_WIFI_COEX_ENABLED:
                case WINC_CFG_PARAM_ID_WIFI_COEX_INTERFACE_TYPE:
                case WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_RX_VS_BT:
                case WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_TX_VS_BT:
                case WINC_CFG_PARAM_ID_WIFI_COEX_ANTENNA_MODE:
                {
                    wifiProcessCoexElem((DRV_HANDLE)pDcpt, id.i, &pElems->elems[1]);
                    break;
                }

                default:
                {
                    break;
                }
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
    static void wifiCmdRspCallbackHandler
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

static void wifiCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("WIFI CmdRspCB %08x Event %d\n", cmdReqHandle, event);

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

            if (NULL != pStatusInfo)
            {
                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_WIFIC:
                    {
                        WINC_DEV_FRACT_INT_TYPE id;

                        if (pStatusInfo->srcCmd.numParams < 2)
                        {
                            break;
                        }

                        if (false == WINC_DevUnpackElements(pStatusInfo->srcCmd.numParams, pStatusInfo->srcCmd.pParams, elems))
                        {
                            break;
                        }

                        WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

                        switch (id.i)
                        {
                            case WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_SELECTED:
                            {
                                if (NULL == pDcpt->pCtrl->pfRegDomainEventCB)
                                {
                                    break;
                                }

                                if (WDRV_WINC_STATUS_OK == pStatusInfo->status)
                                {
                                    WDRV_WINC_REGDOMAIN_INFO regDomInfo;

                                    regDomInfo.regDomainLen = elems[1].length;

                                    memcpy(regDomInfo.regDomain, elems[1].pData, regDomInfo.regDomainLen);

                                    pDcpt->pCtrl->pfRegDomainEventCB((DRV_HANDLE)pDcpt, 1, 1, true, &regDomInfo);
                                }
                                else
                                {
                                    pDcpt->pCtrl->pfRegDomainEventCB((DRV_HANDLE)pDcpt, 0, 0, false, NULL);
                                }

                                break;
                            }

                            case WINC_CFG_PARAM_ID_WIFI_COEX_ENABLED:
                            case WINC_CFG_PARAM_ID_WIFI_COEX_INTERFACE_TYPE:
                            case WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_RX_VS_BT:
                            case WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_TX_VS_BT:
                            case WINC_CFG_PARAM_ID_WIFI_COEX_ANTENNA_MODE:
                            {
                                wifiProcessCoexElem((DRV_HANDLE)pDcpt, id.i, &elems[1]);
                                break;
                            }

                            default:
                            {
                                break;
                            }
                        }

                        break;
                    }
                }
            }
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                wifiProcessCmdRsp((DRV_HANDLE)pDcpt, pRspElems);
            }
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiPowerSaveModeSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_POWERSAVE_MODE psMode
    )

  Summary:
    Set the mode of the powersave.

  Description:
    Configures the powersave mode to one of the available powersave modes.

  Remarks:
    See wdrv_winc_wifi.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiPowerSaveModeSet
(
    DRV_HANDLE handle,
    WDRV_WINC_POWERSAVE_MODE psMode
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

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_POWERSAVE, WINC_TYPE_INTEGER, psMode, sizeof(psMode));

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
    WDRV_WINC_STATUS WDRV_WINC_WifiPowerSaveModeGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_POWERSAVE_CALLBACK pfPowersaveEventCB
    )

  Summary:
    Get the current powersave mode.

  Description:
    Retrieves the currently applied powersave mode.

  Remarks:
    See wdrv_winc_wifi.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiPowerSaveModeGet
(
    DRV_HANDLE handle,
    WDRV_WINC_POWERSAVE_CALLBACK pfPowersaveEventCB
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

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_POWERSAVE, WINC_TYPE_INVALID, 0, 0);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfPowersaveEventCB = pfPowersaveEventCB;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiRegDomainSet
    (
        DRV_HANDLE handle,
        const uint8_t* pRegDomainName,
        uint8_t regDomainNameLen,
        WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB
    )

  Summary:
    Set the active regulatory domain.

  Description:
    Configures the regulatory domain to one available in NVM.

  Remarks:
    See wdrv_winc_wifi.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiRegDomainSet
(
    DRV_HANDLE handle,
    const uint8_t* pRegDomainName,
    uint8_t regDomainNameLen,
    WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB
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

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_SELECTED, WINC_TYPE_STRING, (uintptr_t)pRegDomainName, regDomainNameLen);

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfRegDomainEventCB = pfRegDomainEventCB;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiRegDomainGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_REGDOMAIN_SELECT regDomainSel,
        WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB
    )

  Summary:
    Get the active or all of the available regulatory domain(s).

  Description:
    Read the active or all available regulatory domain(s).

  Remarks:
    See wdrv_winc_wifi.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiRegDomainGet
(
    DRV_HANDLE handle,
    WDRV_WINC_REGDOMAIN_SELECT regDomainSel,
    WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB
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

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (WDRV_WINC_REGDOMAIN_SELECT_ALL == regDomainSel)
    {
        WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_AVAILABLE, WINC_TYPE_INVALID, 0, 0);
    }
    else if (WDRV_WINC_REGDOMAIN_SELECT_CURRENT == regDomainSel)
    {
        WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_SELECTED, WINC_TYPE_INVALID, 0, 0);
    }
    else
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfRegDomainEventCB = pfRegDomainEventCB;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiCoexEnableSet
    (
        DRV_HANDLE handle,
        bool enableCoexArbiter
    )

  Summary:
    Set the enable state of the coexistence arbiter.

  Description:
    Enables or disables the coexistence arbiter.

  Remarks:
    See wdrv_winc_wifi.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiCoexEnableSet
(
    DRV_HANDLE handle,
    bool enableCoexArbiter
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

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_ENABLED, WINC_TYPE_BOOL, enableCoexArbiter, sizeof(bool));

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
    WDRV_WINC_STATUS WDRV_WINC_WifiCoexConfSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_COEX_CFG *pCoexCfg
    )

  Summary:
    Set the coexistence arbiter configuration.

  Description:
    Sets the interface mode, priority and antenna mode of the coexistence arbiter.

  Remarks:
    See wdrv_winc_wifi.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiCoexConfSet
(
    DRV_HANDLE handle,
    WDRV_WINC_COEX_CFG *pCoexCfg
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pCoexCfg))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (1 == pDcpt->pCtrl->coex.enabled)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pCmdReqBuffer = OSAL_Malloc(256);

    if (NULL == pCmdReqBuffer)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 4, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        OSAL_Free(pCmdReqBuffer);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_INTERFACE_TYPE, WINC_TYPE_BOOL, pCoexCfg->use2Wire,           sizeof(bool));
    WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_RX_VS_BT,  WINC_TYPE_BOOL, pCoexCfg->wlanRxHigherThanBt, sizeof(bool));
    WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_TX_VS_BT,  WINC_TYPE_BOOL, pCoexCfg->wlanTxHigherThanBt, sizeof(bool));
    WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_ANTENNA_MODE,   WINC_TYPE_BOOL, pCoexCfg->sharedAntenna,      sizeof(bool));

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
    WDRV_WINC_STATUS WDRV_WINC_WifiCoexConfGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_COEX_CFG *pCoexCfg,
        bool *pIsEnabled
    )

  Summary:
    Returns the coexistence arbiter configuration.

  Description:
    Returns the current coexistence arbiter configuration related to interface
    mode, priority and antenna mode as well as if the arbiter is enabled or not.

  Remarks:
    See wdrv_winc_wifi.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiCoexConfGet
(
    DRV_HANDLE handle,
    WDRV_WINC_COEX_CFG *pCoexCfg,
    bool *pIsEnabled
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

    if (0 == pDcpt->pCtrl->coex.confValid)
    {
        WINC_CMD_REQ_HANDLE cmdReqHandle;
        void *pCmdReqBuffer;

        pCmdReqBuffer = OSAL_Malloc(384);

        if (NULL == pCmdReqBuffer)
        {
            return WDRV_WINC_STATUS_REQUEST_ERROR;
        }

        cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 384, 5, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

        if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
        {
            OSAL_Free(pCmdReqBuffer);
            return WDRV_WINC_STATUS_REQUEST_ERROR;
        }

        WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_INTERFACE_TYPE, WINC_TYPE_INVALID, 0, 0);
        WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_RX_VS_BT,  WINC_TYPE_INVALID, 0, 0);
        WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_TX_VS_BT,  WINC_TYPE_INVALID, 0, 0);
        WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_ANTENNA_MODE,   WINC_TYPE_INVALID, 0, 0);
        WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_ENABLED,        WINC_TYPE_INVALID, 0, 0);

        if (false == WINC_DevTransmitCmdReq(pDcpt->pCtrl->wincDevHandle, cmdReqHandle))
        {
            OSAL_Free(pCmdReqBuffer);
            return WDRV_WINC_STATUS_REQUEST_ERROR;
        }

        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    if (NULL != pCoexCfg)
    {
        pCoexCfg->use2Wire           = (pDcpt->pCtrl->coex.use2Wire           == 1) ? true : false;
        pCoexCfg->wlanRxHigherThanBt = (pDcpt->pCtrl->coex.wlanRxHigherThanBt == 1) ? true : false;
        pCoexCfg->wlanTxHigherThanBt = (pDcpt->pCtrl->coex.wlanTxHigherThanBt == 1) ? true : false;
        pCoexCfg->sharedAntenna      = (pDcpt->pCtrl->coex.sharedAntenna      == 1) ? true : false;
    }

    if (NULL != pIsEnabled)
    {
        *pIsEnabled = (pDcpt->pCtrl->coex.enabled == 1) ? true : false;
    }

    return WDRV_WINC_STATUS_OK;
}
