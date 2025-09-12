/*******************************************************************************
  WINC Wireless Driver Association Source File

  File Name:
    wdrv_winc_assoc.c

  Summary:
    WINC wireless driver association implementation.

  Description:
    This interface provides information about the current association with a
    peer device.
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
#include "wdrv_winc_assoc.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Association Internal Implementation
// *****************************************************************************
// *****************************************************************************

static bool assocHandleIsValid
(
    const WDRV_WINC_CTRLDCPT *const pCtrl,
    const WDRV_WINC_ASSOC_INFO *const pAssocInfo
)
{
    unsigned int i;

    if ((NULL == pCtrl) || (NULL == pAssocInfo))
    {
        return false;
    }

    for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
    {
        if (pAssocInfo == &pCtrl->assocInfoAP[i])
        {
            return true;
        }
    }

    if (pAssocInfo == &pCtrl->assocInfoSTA)
    {
        return true;
    }

    return false;
}

//*******************************************************************************
/*
  Function:
    static void assocProcessAEC
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

static void assocProcessAEC
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
        case WINC_AEC_ID_ASSOC:
        {
            int8_t rssi;
            bool peerIsSTA;
            WDRV_WINC_MAC_ADDR peerAddress;
            WDRV_WINC_ASSOC_INFO *pAssocInfo = NULL;
            uint16_t assocID;

            if (4U != numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems[0], WINC_TYPE_INTEGER, &assocID, sizeof(assocID));
            (void)WINC_CmdReadParamElem(&pElems[1], WINC_TYPE_INTEGER, &peerIsSTA, sizeof(peerIsSTA));
            (void)WINC_CmdReadParamElem(&pElems[2], WINC_TYPE_MACADDR, peerAddress.addr, WDRV_WINC_MAC_ADDR_LEN);
            (void)WINC_CmdReadParamElem(&pElems[3], WINC_TYPE_INTEGER, &rssi, sizeof(rssi));

            peerAddress.valid = true;

            if (false == peerIsSTA)
            {
                if (pCtrl->assocInfoSTA.assocID == assocID)
                {
                    pAssocInfo = &pCtrl->assocInfoSTA;
                }
            }
            else
            {
                pAssocInfo = WDRV_WINC_AssocFindSTAInfo((DRV_HANDLE)pDcpt, &peerAddress);
            }

            if (NULL != pAssocInfo)
            {
                pAssocInfo->rssi = rssi;

                if (NULL != pCtrl->pfAssociationRSSICB)
                {
                    pCtrl->pfAssociationRSSICB((DRV_HANDLE)pDcpt, (WDRV_WINC_ASSOC_HANDLE)pAssocInfo, rssi);
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
    static void assocCmdRspCallbackHandler
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

static void assocCmdRspCallbackHandler
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

//    WDRV_DBG_INFORM_PRINT("Assoc CmdRspCB %08x Event %d\r\n", cmdReqHandle, event);

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
// Section: WINC Driver Association Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_AssocProcessAEC
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
    See wdrv_winc_assoc.h for usage information.

*/

void WDRV_WINC_AssocProcessAEC
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

    assocProcessAEC(pDcpt, pElems->rspId, pElems->numElems, pElems->elems);
}

//*******************************************************************************
/*
  Function:
    static WDRV_WINC_ASSOC_INFO* WDRV_WINC_AssocFindSTAInfo
    (
        DRV_HANDLE handle,
        WDRV_WINC_MAC_ADDR *pMacAddr
    )

  Summary:
    Find an association.

  Description:
    Finds an existing association matching the MAC address supplied or
    finds an empty association if no MAC address supplied.

  Remarks:
    See wdrv_winc_assoc.h for usage information.

*/

WDRV_WINC_ASSOC_INFO* WDRV_WINC_AssocFindSTAInfo
(
    DRV_HANDLE handle,
    WDRV_WINC_MAC_ADDR *pMacAddr
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    WDRV_WINC_ASSOC_INFO *pStaAssocInfo = NULL;
    unsigned int i;

    /* Ensure the driver handle is valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return NULL;
    }

    if (NULL == pMacAddr)
    {
        for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
        {
            if ((DRV_HANDLE_INVALID == pDcpt->pCtrl->assocInfoAP[i].handle) &&
                    (false == pDcpt->pCtrl->assocInfoAP[i].peerAddress.valid))
            {
                pStaAssocInfo = &pDcpt->pCtrl->assocInfoAP[i];
                break;
            }
        }
    }
    else if (true == pMacAddr->valid)
    {
        for (i=0; i<WDRV_WINC_NUM_ASSOCS; i++)
        {
            if ((DRV_HANDLE_INVALID != pDcpt->pCtrl->assocInfoAP[i].handle) &&
                    (true == pDcpt->pCtrl->assocInfoAP[i].peerAddress.valid) &&
                    (0 == memcmp(pDcpt->pCtrl->assocInfoAP[i].peerAddress.addr, pMacAddr->addr, WDRV_WINC_MAC_ADDR_LEN)))
            {
                pStaAssocInfo = &pDcpt->pCtrl->assocInfoAP[i];
                break;
            }
        }
    }
    else
    {
    }

    return pStaAssocInfo;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AssocPeerAddressGet
    (
        WDRV_WINC_ASSOC_HANDLE assocHandle,
        WDRV_WINC_NETWORK_ADDRESS *const pPeerAddress
    )

  Summary:
    Retrieve the current association peer device network address.

  Description:
    Attempts to retrieve the network address of the peer device in the
      current association.

  Remarks:
    See wdrv_winc_assoc.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_AssocPeerAddressGet
(
    WDRV_WINC_ASSOC_HANDLE assocHandle,
    WDRV_WINC_MAC_ADDR *const pPeerAddress
)
{
    WDRV_WINC_CTRLDCPT *pCtrl;
    WDRV_WINC_ASSOC_INFO *const pAssocInfo = (WDRV_WINC_ASSOC_INFO *const)assocHandle;

    if ((WDRV_WINC_ASSOC_HANDLE_INVALID == assocHandle) || (NULL == pAssocInfo) || (NULL == pPeerAddress))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pCtrl = (WDRV_WINC_CTRLDCPT*)pAssocInfo->handle;

    if ((DRV_HANDLE_INVALID == pAssocInfo->handle) || (NULL == pCtrl))
    {
        return WDRV_WINC_STATUS_NOT_CONNECTED;
    }

    /* Ensure the association handle is valid. */
    if (false == assocHandleIsValid(pCtrl, pAssocInfo))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    if (true == pAssocInfo->peerAddress.valid)
    {
        /* If association information stored in driver and user application
           supplied a buffer, copy the peer address to the buffer. */

        (void)memcpy(pPeerAddress, &pAssocInfo->peerAddress, sizeof(WDRV_WINC_MAC_ADDR));

        return WDRV_WINC_STATUS_OK;
    }
    else if (WDRV_WINC_CONN_STATE_CONNECTED == pCtrl->connectedState)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }
    else
    {
        /* Do nothing. */
    }

    return WDRV_WINC_STATUS_REQUEST_ERROR;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AssocRSSIGet
    (
        WDRV_WINC_ASSOC_HANDLE assocHandle,
        int8_t *const pRSSI,
        WDRV_WINC_ASSOC_RSSI_CALLBACK const pfAssociationRSSICB
    )

  Summary:
    Retrieve the current association RSSI.

  Description:
    Attempts to retrieve the RSSI of the current association.

  Remarks:
    See wdrv_winc_assoc.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_AssocRSSIGet
(
    WDRV_WINC_ASSOC_HANDLE assocHandle,
    int8_t *const pRSSI,
    WDRV_WINC_ASSOC_RSSI_CALLBACK const pfAssociationRSSICB
)
{
    WDRV_WINC_CTRLDCPT *pCtrl;
    WDRV_WINC_ASSOC_INFO *const pAssocInfo = (WDRV_WINC_ASSOC_INFO *const)assocHandle;

    if ((WDRV_WINC_ASSOC_HANDLE_INVALID == assocHandle) || (NULL == pAssocInfo))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pCtrl = (WDRV_WINC_CTRLDCPT*)pAssocInfo->handle;

    if ((DRV_HANDLE_INVALID == pAssocInfo->handle) || (NULL == pCtrl))
    {
        return WDRV_WINC_STATUS_NOT_CONNECTED;
    }

    /* Ensure the association handle is valid. */
    if (false == assocHandleIsValid(pCtrl, pAssocInfo))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Store the callback for use later. */
    pCtrl->pfAssociationRSSICB = pfAssociationRSSICB;

    if (WDRV_WINC_CONN_STATE_CONNECTED == pCtrl->connectedState)
    {
        /* WINC is currently connected. */

        if (NULL == pfAssociationRSSICB)
        {
            /* No callback has been provided. */

            if ((0 == pAssocInfo->rssi) && (NULL == pRSSI))
            {
                /* No previous RSSI information and no callback or
                   user application buffer to receive the information. */

                return WDRV_WINC_STATUS_REQUEST_ERROR;
            }
            else if (NULL != pRSSI)
            {
                /* A current RSSI value exists and the user application provided
                   a buffer to receive it, copy the information. */

                *pRSSI = pAssocInfo->rssi;

                return WDRV_WINC_STATUS_OK;
            }
            else
            {
                /* No user application buffer and no callback. */
            }
        }
        else
        {
            WINC_CMD_REQ_HANDLE cmdReqHandle;

            /* A callback has been provided, request the current RSSI from the
               WINC device. */

            cmdReqHandle = WDRV_WINC_CmdReqInit(1, 0, assocCmdRspCallbackHandler, (uintptr_t)assocHandle);

            if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
            {
                return WDRV_WINC_STATUS_REQUEST_ERROR;
            }

            (void)WINC_CmdASSOC(cmdReqHandle, pAssocInfo->assocID);

            if (false == WDRV_WINC_DevTransmitCmdReq(pCtrl, cmdReqHandle))
            {
                return WDRV_WINC_STATUS_REQUEST_ERROR;
            }

            /* Request was successful so indicate the user application needs to
               retry request, or rely on callback for information. */

            return WDRV_WINC_STATUS_RETRY_REQUEST;
        }
    }
    else
    {
        /* WINC is currently disconnected. */
    }

    return WDRV_WINC_STATUS_REQUEST_ERROR;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AssocDisconnect(WDRV_WINC_ASSOC_HANDLE assocHandle)

  Summary:
    Disconnects an association.

  Description:
    Disconnects the STA associated with AP referred by the input association handle.
    Association handle is updated to the application whenever a new STA is connected to the softAP
    This API can also be used in STA mode to disconnect the STA from an AP.

  Remarks:
    See wdrv_winc_softap.h for usage information.
*/

WDRV_WINC_STATUS WDRV_WINC_AssocDisconnect(WDRV_WINC_ASSOC_HANDLE assocHandle)
{
    WDRV_WINC_CTRLDCPT *pCtrl;

    WDRV_WINC_ASSOC_INFO *const pAssocInfo = (WDRV_WINC_ASSOC_INFO *const)assocHandle;

    if ((WDRV_WINC_ASSOC_HANDLE_INVALID == assocHandle) || (NULL == pAssocInfo))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pCtrl = (WDRV_WINC_CTRLDCPT*)pAssocInfo->handle;

    if ((DRV_HANDLE_INVALID == pAssocInfo->handle) || (NULL == pCtrl))
    {
        return WDRV_WINC_STATUS_NOT_CONNECTED;
    }

    /* Ensure the association handle is valid. */
    if (false == assocHandleIsValid(pCtrl, pAssocInfo))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Check operation mode is Soft-AP or STA. */
    if (true == pCtrl->isAP)
    {
        return WDRV_WINC_STATUS_OPERATION_NOT_SUPPORTED;
    }
    else
    {
        const WDRV_WINC_DCPT *pDcpt = (const WDRV_WINC_DCPT *)pCtrl->handle;

        /* Ensure the driver handle is valid. */
        if ((DRV_HANDLE_INVALID == pCtrl->handle) || (NULL == pDcpt))
        {
            return WDRV_WINC_STATUS_INVALID_ARG;
        }

        /* Ensure the driver instance has been opened for use. */
        if (false == pDcpt->isOpen)
        {
            return WDRV_WINC_STATUS_NOT_OPEN;
        }

        /* Ensure WINC is not connected or attempting to connect. */
        if (WDRV_WINC_CONN_STATE_DISCONNECTED == pCtrl->connectedState)
        {
            return WDRV_WINC_STATUS_REQUEST_ERROR;
        }
    }

    return WDRV_WINC_STATUS_OK;
}
