/*******************************************************************************
  This module contains WINC1500 flexible flash map implementation.

  File Name:
    flexible_flash.h

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
// DOM-IGNORE-END

#ifndef __FLEXIBLE_FLASH_H__
#define __FLEXIBLE_FLASH_H__

#include <stdint.h>

typedef struct {
    uint32_t magic;
    uint32_t max_size;
}tstrFlashLUTHeader;

// NOTE: Don't use enums for id/status here,
// they need to be 16 bit but the enums end up as
// 32 bit even if the __packed__ attribute is used
typedef struct {
    uint16_t id;
    uint8_t sector;
    uint8_t size;
    uint32_t reserved;
}tstrFlashLUTEntry;

#define FLASHMAP_MAGIC_VALUE			0x1ABCDEF9
#define FLASHMAP_MAX_ENTRIES			32

// + 8 is for the number of entries value (uint32_t) and CRC (uint32_t)
// * 2 is for the new lookup table to apply
// + 48 is for the progress monitor
#define FLASHMAP_MAX_SIZE				(sizeof(tstrFlashLUTHeader) + (((sizeof(tstrFlashLUTEntry) * FLASHMAP_MAX_ENTRIES) + 8) * 2) + 48)


/** @defgroup SPiFlashRead spi_flexible_flash_find_section
 *  @ingroup SPIFLASHAPI
 */
  /**@{*/
/*!
 * @fn          int8_t spi_flexible_flash_find_section(uint16_t u16EntryIDToLookFor, uint32_t *pu32StartOffset, uint32_t *pu32Size);
 * @brief       Read the Flash Map to extract the host file starting offset.\n
 * @param [in]  u16EntryIDToLookFor
 *                  The ID of the location in Flash we are looking for. See @ref tenuFlashLUTEntryID.
 * @param [in]  pu32StartOffset
 *                  Pointer to the variable where the Flash section start address should be stored.
 * @param [in]  pu32Size
 *                  Pointer to the variable where the Flash section size should be stored.
 * @warning
 *              In case there is a running WINC firmware, it is required to pause the firmware
 *              first before any trial to access SPI flash to avoid any racing between host and
 *              running firmware on bus. @ref m2m_wifi_download_mode can be used to pause the firmware.
 * @sa          m2m_wifi_download_mode
 *              m2m_wifi_init_hold
 * @return      The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

 */
int8_t spi_flexible_flash_find_section(uint16_t u16EntryIDToLookFor, uint32_t *pu32StartOffset, uint32_t *pu32Size);
 /**@}*/

#endif /* __FLEXIBLE_FLASH_H__ */
