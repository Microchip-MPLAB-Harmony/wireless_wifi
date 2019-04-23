/*******************************************************************************
  WINC MAC Driver Packet Pool Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_pktpool.h

  Summary:
    General packet pool.

  Description:
    Provides an interface for managing a pool of packets for the WiFi driver.
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
#include "wdrv_winc_pktpool.h"
#include "tcpip/src/tcpip_packet.h"
#include "tcpip/src/link_list.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC MAC Driver Packet Pool Global Data
// *****************************************************************************
// *****************************************************************************

/* This provides a managed list of packets. */
static PROTECTED_SINGLE_LIST packetPoolFreeList;

// *****************************************************************************
// *****************************************************************************
// Section: WINC MAC Driver Packet Pool Implementation
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

  Remarks:
    See wdrv_winc_pktpool.h for usage information.

*/

bool WDRV_PktPoolInitialize(uint16_t numPackets, uint16_t maxPacketSize)
{
    int8_t i;
    TCPIP_MAC_PACKET *ptrPacket;

    TCPIP_Helper_ProtectedSingleListInitialize(&packetPoolFreeList);

    for (i=0; i<numPackets; i++)
    {
        ptrPacket = _TCPIP_PKT_ALLOC_FNC(sizeof(TCPIP_MAC_PACKET), maxPacketSize, 0);

        if (NULL == ptrPacket)
        {
            return false;
        }

        ptrPacket->next = NULL;

        TCPIP_Helper_ProtectedSingleListTailAdd(&packetPoolFreeList, (SGL_LIST_NODE*)ptrPacket);
    }

    return true;
}

//*******************************************************************************
/*
  Function:
    TCPIP_MAC_PACKET* WDRV_PktPoolRemove(void)

  Summary:
    Removes a packet from the pool.

  Description:
    Removes and returns a single packet from the pool if available.

  Remarks:
    See wdrv_winc_pktpool.h for usage information.

*/

TCPIP_MAC_PACKET* WDRV_PktPoolRemove(void)
{
    TCPIP_MAC_PACKET *ptrPacket;

    ptrPacket = (TCPIP_MAC_PACKET*)TCPIP_Helper_ProtectedSingleListHeadRemove(&packetPoolFreeList);

    return ptrPacket;
}

//*******************************************************************************
/*
  Function:
    bool WDRV_PktPoolAdd(TCPIP_MAC_PACKET *ptrPacket)

  Summary:
    Adds a packet to the pool.

  Description:
    Places a packet passed into the function in the packet pool.

  Remarks:
    See wdrv_winc_pktpool.h for usage information.

*/

bool WDRV_PktPoolAdd(TCPIP_MAC_PACKET *ptrPacket)
{
    if (NULL == ptrPacket)
    {
        return false;
    }

    TCPIP_Helper_ProtectedSingleListTailAdd(&packetPoolFreeList, (SGL_LIST_NODE*)ptrPacket);

    return true;
}
