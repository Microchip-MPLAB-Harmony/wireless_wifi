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
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "system_config.h"
#include "system_definitions.h"
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
    if ((NULL == pDcpt) || (NULL == pEthBuf)
                        || (0 == lengthEthBuf) || (NULL == pfEthMsgRecvCB))
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
    pDcpt->pfEthernetMsgRecvCB = pfEthMsgRecvCB;

    /* Indicate the Ethernet buffer has been set. */
    pDcpt->isEthBufSet = true;

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
    if ((NULL == pDcpt) || (NULL == pEthBuf) || (0 == lengthEthBuf))
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
