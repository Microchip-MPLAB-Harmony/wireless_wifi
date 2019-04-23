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
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

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

#include "system_config.h"
#include "system_definitions.h"
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
