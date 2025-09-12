/*******************************************************************************
  WINC Wireless Driver SNTP Source File

  File Name:
    wdrv_winc_sntp.c

  Summary:
    WINC wireless driver SNTP implementation.

  Description:
    WINC wireless driver interface for SNTP client configuration.
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

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_sntp.h"

#ifndef WDRV_WINC_MOD_DISABLE_SNTP

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver SNTP Internal Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static void sntpCmdRspCallbackHandler
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

static void sntpCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("SNTP CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
// Section: WINC Driver SNTP Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPEnableSet
    (
        DRV_HANDLE handle,
        bool enabled
    )

  Summary:
    Set the enable state of the Network Time (NTP) client function.

  Description:
    Enables or disables the Network Time (NTP) client.

  Remarks:
    See wdrv_winc_sntp.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPEnableSet
(
    DRV_HANDLE handle,
    bool enabled
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const )handle;
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, sntpCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdSNTPC(cmdReqHandle, WINC_CFG_PARAM_ID_SNTP_NTP_ENABLED, WINC_TYPE_BOOL, (true == enabled) ? 1U : 0U, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPStaticSet
    (
        DRV_HANDLE handle,
        bool enabled
    )

  Summary:
    Set the enable state of the NTP configuration mode.

  Description:
    Configures the NTP configuration mode to either DHCP or Static.

  Remarks:
    See wdrv_winc_sntp.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPStaticSet
(
    DRV_HANDLE handle,
    bool enabled
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const )handle;
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

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, 128, sntpCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdSNTPC(cmdReqHandle, WINC_CFG_PARAM_ID_SNTP_NTP_STATIC, WINC_TYPE_BOOL, (true == enabled) ? 1U : 0U, 0);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPServerAddressSet
    (
        DRV_HANDLE handle,
        const char *pAddr
    )

  Summary:
    Set the address/name of NTP server.

  Description:
    Configures the address/name to be used as the NTP server.

  Remarks:
    See wdrv_winc_sntp.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPServerAddressSet
(
    DRV_HANDLE handle,
    const char* pAddr
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const )handle;
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    size_t addressLen;

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

    addressLen = strnlen(pAddr, WINC_CFG_PARAM_SZ_SNTP_NTP_SVR+1U);

    if ((0U == addressLen) || (addressLen > WINC_CFG_PARAM_SZ_SNTP_NTP_SVR))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    cmdReqHandle = WDRV_WINC_CmdReqInit(1, addressLen, sntpCmdRspCallbackHandler, (uintptr_t)pDcpt);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    (void)WINC_CmdSNTPC(cmdReqHandle, WINC_CFG_PARAM_ID_SNTP_NTP_SVR, WINC_TYPE_STRING, (uintptr_t)pAddr, addressLen);

    if (false == WDRV_WINC_DevTransmitCmdReq(pDcpt->pCtrl, cmdReqHandle))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

#endif /* WDRV_WINC_MOD_DISABLE_SNTP */
