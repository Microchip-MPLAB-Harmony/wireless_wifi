/*******************************************************************************
  WINC Wireless Driver

  File Name:
    wdrv_winc_stack_itf.c

  Summary:
    Stack interface to link MAC driver to TCP/IP stack.

  Description:
    Stack interface to link MAC driver to TCP/IP stack.
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

#include "system_config.h"
#include "system_definitions.h"
#include "wdrv_winc_stack_drv.h"

const TCPIP_MAC_OBJECT WDRV_WINC_MACObject =
{
    TCPIP_MODULE_MAC_WINC,
    "WINC",
    WDRV_WINC_MACInitialize,
    WDRV_WINC_MACDeinitialize,
    WDRV_WINC_MACReinitialize,
    WDRV_WINC_MACStatus,
    WDRV_WINC_MACTasks,
    WDRV_WINC_MACOpen,
    WDRV_WINC_MACClose,
    WDRV_WINC_MACLinkCheck,
    WDRV_WINC_MACRxFilterHashTableEntrySet,
    WDRV_WINC_MACPowerMode,
    WDRV_WINC_MACPacketTx,
    WDRV_WINC_MACPacketRx,
    WDRV_WINC_MACProcess,
    WDRV_WINC_MACStatisticsGet,
    WDRV_WINC_MACParametersGet,
    WDRV_WINC_MACRegisterStatisticsGet,
    WDRV_WINC_MACConfigGet,
    WDRV_WINC_MACEventMaskSet,
    WDRV_WINC_MACEventAcknowledge,
    WDRV_WINC_MACEventPendingGet,
};

//DOM-IGNORE-END
