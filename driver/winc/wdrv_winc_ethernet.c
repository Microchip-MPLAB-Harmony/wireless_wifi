/*******************************************************************************
  WINC Driver Ethernet Mode Implementation

  File Name:
    wdrv_winc_ethernet.c

  Summary:
    WINC wireless driver (Ethernet mode) implementation.

  Description:
    This interface provides extra functionality required for Ethernet mode operation.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2019, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Ethernet Mode Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_EthernetRecvPacket
    (
        DRV_HANDLE handle,
        uint8_t *const pEthBuf,
        uint16_t lengthEthBuf,
        const WDRV_WINC_ETH_MSG_RECV_CALLBACK pfEthMsgRecvCB
    )

  Summary:
    Provide a buffer for the next received Ethernet packet.

  Description:
    Provides a buffer and callback to the WINC. When the next Ethernet
      packet is received it will placed into this buffer and the callback
      signalled.

  Remarks:
    See wdrv_winc_ethernet.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_EthernetRecvPacket
(
    DRV_HANDLE handle,
    uint8_t *const pEthBuf,
    uint16_t lengthEthBuf,
    const WDRV_WINC_ETH_MSG_RECV_CALLBACK pfEthMsgRecvCB
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle, supplied Ethernet buffer and callback are valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((NULL == pEthBuf) || (0 == lengthEthBuf) || (NULL == pfEthMsgRecvCB))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Associate the supplied buffer with the M2M layer. */
    if (M2M_SUCCESS != m2m_wifi_set_receive_buffer(pEthBuf, lengthEthBuf))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Store callback for later use. */
    pDcpt->pCtrl->pfEthernetMsgRecvCB = pfEthMsgRecvCB;

    /* Indicate the Ethernet buffer has been set. */
    pDcpt->pCtrl->isEthBufSet = true;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_EthernetSendPacket
    (
        DRV_HANDLE handle,
        const uint8_t *const pEthBuf,
        uint16_t lengthEthBuf
    )

  Summary:
    Sends an Ethernet packet.

  Description:
    Sends an Ethernet packet to the WINC for transmission.

  Remarks:
    See wdrv_winc_ethernet.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_EthernetSendPacket
(
    DRV_HANDLE handle,
    const uint8_t *const pEthBuf,
    uint16_t lengthEthBuf
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle and supplied Ethernet frame are valid. */
    if ((DRV_HANDLE_INVALID == handle) || (NULL == pDcpt))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    if ((NULL == pEthBuf) || (0 == lengthEthBuf))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    /* Send the Ethernet frame onto the M2M layer. */
    if (M2M_SUCCESS != m2m_wifi_send_ethernet_pkt((uint8_t*)pEthBuf, lengthEthBuf))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
