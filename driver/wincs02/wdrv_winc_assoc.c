/*******************************************************************************
  WINC Driver Association Implementation

  File Name:
    wdrv_winc_assoc.c

  Summary:
    WINC wireless driver association implementation.

  Description:
    This interface provides information about the current association with a
    peer device.
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
    int i;

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

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Association Implementation
// *****************************************************************************
// *****************************************************************************

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

         memcpy(pPeerAddress, &pAssocInfo->peerAddress, sizeof(WDRV_WINC_MAC_ADDR));

        return WDRV_WINC_STATUS_OK;
    }
    else if (WDRV_WINC_CONN_STATE_CONNECTED == pCtrl->connectedState)
    {
        return WDRV_WINC_STATUS_RETRY_REQUEST;
    }

    return WDRV_WINC_STATUS_REQUEST_ERROR;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_AssocRSSIGet
    (
        DRV_HANDLE handle,
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
            /* A callback has been provided, request the current RSSI from the
               WINC device. */

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
    WDRV_WINC_DCPT *pDcpt;

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
        pDcpt = (WDRV_WINC_DCPT *)pCtrl->handle;

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
