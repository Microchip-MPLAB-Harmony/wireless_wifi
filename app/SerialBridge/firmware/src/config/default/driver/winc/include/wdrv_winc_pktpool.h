/*******************************************************************************
  WINC MAC Driver Packet Pool Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_pktpool.h

  Summary:
    General packet pool.

  Description:
    Provides an interface for managing a pool of packets for the WiFi driver.
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

#ifndef _WDRV_WINC_PKTPOOL_H
#define _WDRV_WINC_PKTPOOL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "tcpip/src/tcpip_packet.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC MAC Driver Packet Pool Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    bool WDRV_PktPoolInitialize(uint16_t numPackets, uint16_t maxPacketSize)

  Summary:
    Initializes the packet pool.

  Description:
    Creates and initializes the packet pool based on the number and maximum
      size of packets.

  Precondition:
    None.

  Parameters:
    numPackets    - Number of packets in the pool.
    maxPacketSize - Maximum size of each packet in the pool.

  Returns:
    true  - Pool created and initialized.
    false - An error occurred.

  Remarks:
    None.

*/

bool WDRV_PktPoolInitialize(uint16_t numPackets, uint16_t maxPacketSize);

//*******************************************************************************
/*
  Function:
    TCPIP_MAC_PACKET* WDRV_PktPoolRemove(void)

  Summary:
    Removes a packet from the pool.

  Description:
    Removes and returns a single packet from the pool if available.

  Precondition:
    WDRV_PktPoolInitialize must have been called to initialize the pool.

  Parameters:
    None.

  Returns:
    NULL for an error or no packets available, otherwise a pointer to the
      removed packet.

  Remarks:
    None.

*/

TCPIP_MAC_PACKET* WDRV_PktPoolRemove(void);

//*******************************************************************************
/*
  Function:
    bool WDRV_PktPoolAdd(TCPIP_MAC_PACKET *ptrPacket)

  Summary:
    Adds a packet to the pool.

  Description:
    Places a packet passed into the function in the packet pool.

  Precondition:
    WDRV_PktPoolInitialize must have been called to initialize the pool.

  Parameters:
    ptrPacket - Pointer to packet to add to the pool.

  Returns:
    true  - Packet added.
    false - An error occurred.

  Remarks:
    None.

*/

bool WDRV_PktPoolAdd(TCPIP_MAC_PACKET *ptrPacket);

#endif /* _WDRV_WINC_PKTPOOL_H */

// DOM-IGNORE-END

