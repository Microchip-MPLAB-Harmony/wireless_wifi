/*******************************************************************************
  File Name:
    flexible_flash.c

  Summary:
    This module contains WINC1500 flexible flash map implementation.

  Description:
    This module contains WINC1500 flexible flash map implementation.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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

#include "spi_flash.h"
#include "m2m_types.h"
#include "flexible_flash.h"

#define FLASH_MAP_TABLE_ADDR        (FLASH_SECTOR_SZ+sizeof(tstrOtaControlSec)+8)
#define N_ENTRIES_MAX               32

int8_t spi_flexible_flash_find_section(uint16_t u16EntryIDToLookFor, uint32_t *pu32StartOffset, uint32_t *pu32Size)
{
    int8_t s8Ret = M2M_ERR_INVALID_ARG;
    if((NULL == pu32StartOffset) || (NULL == pu32Size)) goto EXIT;

    uint8_t au8buff[8];
    uint8_t u8CurrEntry = 0;
    s8Ret = spi_flash_read(&au8buff[0], FLASH_MAP_TABLE_ADDR, 4);
    if(M2M_SUCCESS != s8Ret) goto EXIT;

    uint8_t u8nEntries = au8buff[0];     // Max number is 32, reading one byte will suffice
    if(u8nEntries > N_ENTRIES_MAX)
    {
        s8Ret = M2M_ERR_FAIL;
        goto EXIT;
    }

    while(u8nEntries > u8CurrEntry)
    {
        s8Ret = spi_flash_read(&au8buff[0], FLASH_MAP_TABLE_ADDR + 4 + (u8CurrEntry*8), 8);
        u8CurrEntry++;
        if(M2M_SUCCESS != s8Ret) break;
        uint16_t u16EntryID = (au8buff[1] << 8) | au8buff[0];
        if(u16EntryID != u16EntryIDToLookFor) continue;
        *pu32StartOffset = au8buff[2] * FLASH_SECTOR_SZ;
        *pu32Size        = au8buff[3] * FLASH_SECTOR_SZ;
        break;
    }
EXIT:
    return s8Ret;
}