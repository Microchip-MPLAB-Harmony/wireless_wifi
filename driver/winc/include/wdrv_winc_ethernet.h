/*******************************************************************************
  WINC Driver Ethernet Mode Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_ethernet.h

  Summary:
    WINC wireless driver (Ethernet mode) header file.

  Description:
    This interface provides extra functionality required for Ethernet mode operation.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef _WDRV_WINC_ETHERNET_H
#define _WDRV_WINC_ETHERNET_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "wdrv_winc.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Ethernet Mode Routines
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

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    pEthBuf        - Pointer to buffer to store next packet in.
    lengthEthBuf   - Length of buffer provided.
    pfEthMsgRecvCB - Pointer to function to use as callback.

  Returns:
    WDRV_WINC_STATUS_OK             - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_EthernetRecvPacket
(
    DRV_HANDLE handle,
    uint8_t *const pEthBuf,
    uint16_t lengthEthBuf,
    const WDRV_WINC_ETH_MSG_RECV_CALLBACK pfEthMsgRecvCB
);

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

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    pEthBuf      - Pointer to buffer containing Ethernet packet.
    lengthEthBuf - Length of packet data in buffer provided.

  Returns:
    WDRV_WINC_STATUS_OK             - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_EthernetSendPacket
(
    DRV_HANDLE handle,
    const uint8_t *const pEthBuf,
    uint16_t lengthEthBuf
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* _WDRV_WINC_ETHERNET_H */
