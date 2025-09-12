/*******************************************************************************
  WINC Wireless Driver Wifi Configuration Source File

  File Name:
    wdrv_winc_wifi.c

  Summary:
    WINC wireless driver wifi configuration implementation.

  Description:
    This interface provides functionality required for configuring the Wifi,
    this includes powersave mode, regulatory domain and coex settings.
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
#include "wdrv_winc_wifi.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Wifi Configuration Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void wifiProcessCoexElem
    (
        DRV_HANDLE handle,
        uint16_t id,
        const WINC_DEV_PARAM_ELEM *const pElem
    )

  Summary:
    Process command/response element flags for Coex.

  Description:
    Processing both the command to change and the query response for coex
    settings by decoding the boolean value and setting the internal state.

  Precondition:
    WDRV_WINC_DevTransmitCmdReq must have been called to submit command request.

  Parameters:
    handle    - WINC device handle.
    id        - WIFIC parameter ID.
    pElem     - Pointer to command or response element for the WIFIC value.

  Returns:
    None.

  Remarks:
    None.

*/

static void wifiProcessCoexElem
(
    DRV_HANDLE handle,
    uint16_t id,
    const WINC_DEV_PARAM_ELEM *const pElem
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    bool flag;

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElem))
    {
        return;
    }

    (void)WINC_CmdReadParamElem(pElem, WINC_TYPE_BOOL, &flag, sizeof(flag));

    switch (id)
    {
        case WINC_CFG_PARAM_ID_WIFI_COEX_ENABLED:
        {
            pDcpt->pCtrl->coex.enabled       = (true == flag) ? 1U : 0U;
            pDcpt->pCtrl->coex.confValid     = 1U;
            pDcpt->pCtrl->coex.reqInProgress = 0U;
            break;
        }

        case WINC_CFG_PARAM_ID_WIFI_COEX_INTERFACE_TYPE:
        {
            pDcpt->pCtrl->coex.use2Wire = (true == flag) ? 1U : 0U;
            break;
        }

        case WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_RX_VS_BT:
        {
            pDcpt->pCtrl->coex.wlanRxHigherThanBt = (true == flag) ? 1U : 0U;
            break;
        }

        case WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_TX_VS_BT:
        {
            pDcpt->pCtrl->coex.wlanTxHigherThanBt = (true == flag) ? 1U : 0U;
            break;
        }

        case WINC_CFG_PARAM_ID_WIFI_COEX_ANTENNA_MODE:
        {
            pDcpt->pCtrl->coex.sharedAntenna = (true == flag) ? 1U : 0U;
            break;
        }

        default:
        {
            WDRV_DBG_VERBOSE_PRINT("WIFIC Coex ID %d not handled\r\n", id);
            break;
        }
    }
}

//*******************************************************************************
/*
  Function:
    static void wifiProcessStatus
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

static void wifiProcessStatus
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t cmdID,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    uint16_t statusCode
)
{
    WINC_DEV_PARAM_ELEM elems[10];

    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pSrcCmd))
    {
        return;
    }

    switch (cmdID)
    {
        case WINC_CMD_ID_WIFIC:
        {
            WINC_DEV_FRACT_INT_TYPE id;

            if (pSrcCmd->numParams < 2U)
            {
                break;
            }

            if (false == WINC_DevUnpackElements(pSrcCmd->numParams, pSrcCmd->pParams, elems))
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&elems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

            switch (id.i)
            {
                case WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_SELECTED:
                {
                    WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB = pDcpt->pCtrl->pfRegDomainEventCB;

                    if (NULL == pfRegDomainEventCB)
                    {
                        break;
                    }

                    if (WDRV_WINC_STATUS_OK != (WDRV_WINC_STATUS)statusCode)
                    {
                        /* If get/set regulatory domain fails, report through callback. */

                        pDcpt->pCtrl->pfRegDomainEventCB = NULL;

                        pfRegDomainEventCB((DRV_HANDLE)pDcpt, 0, 0, false, NULL);
                    }
                    else if (2U == pSrcCmd->numParams)
                    {
                        pDcpt->pCtrl->pfRegDomainEventCB = NULL;

                        /* Set regulatory domain completed, send a get to update information. */
                        (void)WDRV_WINC_WifiRegDomainGet((DRV_HANDLE)pDcpt, WDRV_WINC_REGDOMAIN_SELECT_CURRENT, pfRegDomainEventCB);
                    }
                    else
                    {
                        /* Do nothing. */
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
    static void wifiProcessCmdRsp
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

static void wifiProcessCmdRsp
(
    WDRV_WINC_DCPT *pDcpt,
    uint16_t rspId,
    WINC_CMD_REQ_HANDLE cmdReqHandle,
    const WINC_DEV_EVENT_SRC_CMD *const pSrcCmd,
    int numElems,
    const WINC_DEV_PARAM_ELEM *const pElems
)
{
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pElems) || (NULL == pSrcCmd))
    {
        return;
    }

    switch (rspId)
    {
        case WINC_CMD_ID_WIFIC:
        {
            WINC_DEV_FRACT_INT_TYPE id;

            if (2U != numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER_FRAC, &id, sizeof(id));

            switch (id.i)
            {
                case WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_SELECTED:
                {
                    WDRV_WINC_REGDOMAIN_INFO *pRegDomain;
                    WDRV_WINC_REGDOMAIN_INFO tmpRegDomain;

                    /* Set operations are brought into a temporary structure until it is verified
                     as being the updated domain information. Get operation go straight to the
                     active domain structure. */

                    if (true == pDcpt->pCtrl->regDomainSetInProgress)
                    {
                        pRegDomain = &tmpRegDomain;
                    }
                    else
                    {
                        pRegDomain = &pDcpt->pCtrl->activeRegDomain;
                    }

                    pRegDomain->regDomainLen = (uint8_t)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_STRING, pRegDomain->regDomain, WDRV_WINC_REGDOMAIN_MAX_NAME_LEN);

                    if (0 == pRegDomain->regDomainLen)
                    {
                        break;
                    }

                    if (true == pDcpt->pCtrl->regDomainSetInProgress)
                    {
                        /* If domain information supplied matched the correct settings retry
                         the request as WINCS02 is still in the process of updating them. */

                        if ((0 == memcmp(pRegDomain->regDomain, pDcpt->pCtrl->activeRegDomain.regDomain, pRegDomain->regDomainLen))
                                && (pRegDomain->regDomainLen == pDcpt->pCtrl->activeRegDomain.regDomainLen))
                        {
                            WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB = pDcpt->pCtrl->pfRegDomainEventCB;

                            pDcpt->pCtrl->pfRegDomainEventCB = NULL;

                            (void)WDRV_WINC_WifiRegDomainGet((DRV_HANDLE)pDcpt, WDRV_WINC_REGDOMAIN_SELECT_CURRENT, pfRegDomainEventCB);
                            break;
                        }

                        /* Copy new domain information into active domain structure. Clear channel mask. */

                        (void)memset(&pDcpt->pCtrl->activeRegDomain, 0, sizeof(WDRV_WINC_REGDOMAIN_INFO));
                        (void)memcpy(&pDcpt->pCtrl->activeRegDomain.regDomain, pRegDomain->regDomain, pRegDomain->regDomainLen);
                        pDcpt->pCtrl->activeRegDomain.regDomainLen = pRegDomain->regDomainLen;
                        pDcpt->pCtrl->regDomainSetInProgress       = false;
                    }

                    break;
                }

                case WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_AVAILABLE:
                {
                    if (NULL == pDcpt->pCtrl->pfRegDomainEventCB)
                    {
                        break;
                    }

                    if (-1 == id.f)
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &pDcpt->pCtrl->availRegDomNum, sizeof(pDcpt->pCtrl->availRegDomNum));
                    }
                    else
                    {
                        WDRV_WINC_REGDOMAIN_INFO regDomInfo;

                        (void)memset(&regDomInfo, 0, sizeof(WDRV_WINC_REGDOMAIN_INFO));

                        regDomInfo.regDomainLen = (uint8_t)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_STRING, regDomInfo.regDomain, WDRV_WINC_REGDOMAIN_MAX_NAME_LEN);

                        if (0 == regDomInfo.regDomainLen)
                        {
                            break;
                        }

                        if (id.f < (int16_t)pDcpt->pCtrl->availRegDomNum)
                        {
                            bool active = false;

                            if (0 == memcmp(regDomInfo.regDomain, pDcpt->pCtrl->activeRegDomain.regDomain, WDRV_WINC_REGDOMAIN_MAX_NAME_LEN))
                            {
                                regDomInfo.channelMask = pDcpt->pCtrl->regulatoryChannelMask24;

                                active = true;
                            }

                            pDcpt->pCtrl->pfRegDomainEventCB((DRV_HANDLE)pDcpt, id.f+1, pDcpt->pCtrl->availRegDomNum, active, &regDomInfo);
                        }
                    }

                    break;
                }

                case WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_CHANMASK24:
                {
                    (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &pDcpt->pCtrl->regulatoryChannelMask24, sizeof(pDcpt->pCtrl->regulatoryChannelMask24));
                    pDcpt->pCtrl->activeRegDomain.channelMask = pDcpt->pCtrl->regulatoryChannelMask24;

                    if (false == pDcpt->pCtrl->regDomainSetInProgress)
                    {
                        if (NULL != pDcpt->pCtrl->pfRegDomainEventCB)
                        {
                            WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB;

                            /* If WDRV_WINC_WifiRegDomainGet was called with WDRV_WINC_REGDOMAIN_SELECT_CURRENT
                                 callback to the application with the currently selected domain. */

                            pfRegDomainEventCB = pDcpt->pCtrl->pfRegDomainEventCB;

                            pDcpt->pCtrl->pfRegDomainEventCB = NULL;

                            pfRegDomainEventCB((DRV_HANDLE)pDcpt, 1, 1, true, &pDcpt->pCtrl->activeRegDomain);
                        }
                    }

                    break;
                }

                case WINC_CFG_PARAM_ID_WIFI_POWERSAVE:
                {
                    WDRV_WINC_POWERSAVE_INFO powersaveInfo;

                    if (NULL != pDcpt->pCtrl->pfPowersaveEventCB)
                    {
                        (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &powersaveInfo.psMode, sizeof(powersaveInfo.psMode));

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
                    wifiProcessCoexElem((DRV_HANDLE)pDcpt, id.i, &pElems[1]);
                    break;
                }

                default:
                {
                    WDRV_DBG_VERBOSE_PRINT("WIFIC ID(Rsp) %d not handled\r\n", id.i);
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

    if (NULL == pDcpt)
    {
        return;
    }

//    WDRV_DBG_INFORM_PRINT("WIFI CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
                wifiProcessStatus(pDcpt, pStatusInfo->rspCmdId, cmdReqHandle, &pStatusInfo->srcCmd, pStatusInfo->status);
            }

            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
                wifiProcessCmdRsp(pDcpt, pRspElems->rspId, cmdReqHandle, &pRspElems->srcCmd, pRspElems->numElems, pRspElems->elems);
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
// Section: WINC Driver Wifi Configuration Implementation
// *****************************************************************************
// *****************************************************************************

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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_POWERSAVE, WINC_TYPE_INTEGER, (uintptr_t)psMode, sizeof(psMode));

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_POWERSAVE, WINC_TYPE_INVALID, 0, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
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

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pfRegDomainEventCB))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* If operation in progress, defer caller. */
    if (NULL != pDcpt->pCtrl->pfRegDomainEventCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    if ((0 == memcmp(pRegDomainName, pDcpt->pCtrl->activeRegDomain.regDomain, regDomainNameLen))
            && (regDomainNameLen == pDcpt->pCtrl->activeRegDomain.regDomainLen))
    {
        /* If new domain information matches the current active domain, return success here
         calling the user callback if supplied. */

        pfRegDomainEventCB(handle, 1, 1, true, &pDcpt->pCtrl->activeRegDomain);

        return WDRV_WINC_STATUS_OK;
    }

    pDcpt->pCtrl->regulatoryChannelMask24 = WDRV_WINC_CM_2_4G_NONE;

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_SELECTED, WINC_TYPE_STRING, (uintptr_t)pRegDomainName, regDomainNameLen);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->pCtrl->pfRegDomainEventCB     = pfRegDomainEventCB;
    pDcpt->pCtrl->regDomainSetInProgress = true;

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

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pfRegDomainEventCB))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* If operation in progress, defer caller. */
    if (NULL != pDcpt->pCtrl->pfRegDomainEventCB)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(2, 0, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (WDRV_WINC_REGDOMAIN_SELECT_ALL == regDomainSel)
    {
        (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_AVAILABLE, WINC_TYPE_INVALID, 0, 0);
    }
    else if (WDRV_WINC_REGDOMAIN_SELECT_CURRENT == regDomainSel)
    {
        (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_SELECTED, WINC_TYPE_INVALID, 0, 0);

        (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_REGDOMAIN_CHANMASK24, WINC_TYPE_INVALID, 0, 0);
    }
    else
    {
        WDRV_WINC_DevDiscardCmdReq(cmdReqHandle);
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_ENABLED, WINC_TYPE_BOOL, (true == enableCoexArbiter) ? 1U : 0U, sizeof(bool));

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
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
        const WDRV_WINC_COEX_CFG *const pCoexCfg
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
    const WDRV_WINC_COEX_CFG *const pCoexCfg
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

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

    if (1U == pDcpt->pCtrl->coex.enabled)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(4, 0, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_INTERFACE_TYPE, WINC_TYPE_BOOL, (true == pCoexCfg->use2Wire) ? 1U : 0U, sizeof(bool));

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_RX_VS_BT, WINC_TYPE_BOOL, (true == pCoexCfg->wlanRxHigherThanBt) ? 1U : 0U, sizeof(bool));

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_TX_VS_BT, WINC_TYPE_BOOL, (true == pCoexCfg->wlanTxHigherThanBt) ? 1U : 0U, sizeof(bool));

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_ANTENNA_MODE, WINC_TYPE_BOOL, (true == pCoexCfg->sharedAntenna) ? 1U : 0U, sizeof(bool));

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
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

    if (1U == pDcpt->pCtrl->coex.reqInProgress)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    if (0U == pDcpt->pCtrl->coex.confValid)
    {
        WINC_CMD_REQ_HANDLE cmdReqHandle;

        cmdReqHandle = WDRV_WINC_CmdReqInit(5, 0, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

        if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
        {
            return WDRV_WINC_STATUS_REQUEST_ERROR;
        }

        (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_INTERFACE_TYPE, WINC_TYPE_INVALID, 0, 0);

        (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_RX_VS_BT, WINC_TYPE_INVALID, 0, 0);

        (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_WLAN_TX_VS_BT, WINC_TYPE_INVALID, 0, 0);

        (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_ANTENNA_MODE, WINC_TYPE_INVALID, 0, 0);

        (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_COEX_ENABLED, WINC_TYPE_INVALID, 0, 0);

        if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
        {
            return WDRV_WINC_STATUS_REQUEST_ERROR;
        }

        pDcpt->pCtrl->coex.reqInProgress = 1U;

        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    if (NULL != pCoexCfg)
    {
        pCoexCfg->use2Wire           = (1U == pDcpt->pCtrl->coex.use2Wire           ) ? true : false;
        pCoexCfg->wlanRxHigherThanBt = (1U == pDcpt->pCtrl->coex.wlanRxHigherThanBt ) ? true : false;
        pCoexCfg->wlanTxHigherThanBt = (1U == pDcpt->pCtrl->coex.wlanTxHigherThanBt ) ? true : false;
        pCoexCfg->sharedAntenna      = (1U == pDcpt->pCtrl->coex.sharedAntenna      ) ? true : false;
    }

    if (NULL != pIsEnabled)
    {
        *pIsEnabled = (1U == pDcpt->pCtrl->coex.enabled) ? true : false;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiMACOptionsSet
    (
        DRV_HANDLE handle,
        const WDRV_WINC_MAC_OPTIONS *const pMACOptions
    )

  Summary:
    Sets the MAC options.

  Description:
    Configures WiFi MAC options.

  Remarks:
    See wdrv_winc_wifi.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_WifiMACOptionsSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_MAC_OPTIONS *const pMACOptions
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Ensure the driver handle and options pointer are valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl) || (NULL == pMACOptions))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, wifiCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdWIFIC(cmdReqHandle, WINC_CFG_PARAM_ID_WIFI_AMPDU_TX_ENABLED, WINC_TYPE_BOOL, (true == pMACOptions->useTxAmpdu) ? 1U : 0U, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
