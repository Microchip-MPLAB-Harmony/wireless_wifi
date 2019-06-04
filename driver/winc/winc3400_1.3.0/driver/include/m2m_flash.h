/*******************************************************************************
  WINC Flash Interface.

  File Name:
    m2m_flash.h

  Summary:
    WINC Flash Interface

  Description:
    WINC Flash Interface

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

/**@defgroup FLASHAPI Flash
*/

#ifndef __M2M_FLASH_H__
#define __M2M_FLASH_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include "nm_common.h"
#include "m2m_types.h"

/**@defgroup  FlashEnums Enumeration/Typedefs
 * @ingroup FLASHAPI
 * @{*/

/*!
@enum   tenuWincFlashRegion

@brief  The regions of WINC flash available for accessing.
*/
typedef enum
{
    /*! Access to the entire flash address space. */
    WINC_FLASH_REGION_RAW,
    /*! Access to the active firmware region. */
    WINC_FLASH_REGION_FIRMWARE_ACTIVE,
    /*! Access to the inactive firmware region. */
    WINC_FLASH_REGION_FIRMWARE_INACTIVE,
    /*! Access to PLL table region. Erase is not supported for this region. */
    WINC_FLASH_REGION_PLL_TABLE,
    /*! Access to gain table region. Erase is not supported for this region. */
    WINC_FLASH_REGION_GAIN_TABLE,
    /*! Access to PLL and gain tables region. Erase is supported for this region. */
    WINC_FLASH_REGION_PLL_AND_GAIN_TABLES,
    /*! Access to TLS root certificate region. */
    WINC_FLASH_REGION_ROOT_CERTS,
    /*! Access to TLS local certificate region. */
    WINC_FLASH_REGION_LOCAL_CERTS,
    /*! Access to connection parameters region. */
    WINC_FLASH_REGION_CONN_PARAM,
    /*! Access to HTTP files region. This region is also erased/written by erase/write of firmware regions. */
    WINC_FLASH_REGION_HTTP_FILES,
    /*! Number of regions. */
    WINC_FLASH_NUM_REGIONS
} tenuWincFlashRegion;

/**@}
*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/** \defgroup FLASHFUNCTIONS Functions
*  @ingroup FLASHAPI
*/
/**@{*/

/*!
@fn \
    int8_t m2m_flash_erase_sector(tenuWincFlashRegion enuRegion, uint8_t u8StartSector, uint8_t u8NumSectors);

@brief  Erase one or more sectors within a WINC flash region.

@param [in]     enuRegion
                    Region of flash in which to erase.
@param [in]     u8StartSector
                    Sector offset (within region) of first sector to erase.
@param [in]     u8NumSectors
                    Number of sectors to erase.

@return     M2M_SUCCESS            - The erase completed successfully.
            M2M_ERR_INVALID_ARG    - The parameters were invalid.
            M2M_ERR_FAIL           - The request encountered an error.

@pre        The WINC must have been initialised but must not be running. This can be done via
            @ref m2m_wifi_init_hold, @ref m2m_wifi_reinit_hold or @ref m2m_wifi_download_mode.

*/
int8_t m2m_flash_erase_sector(tenuWincFlashRegion enuRegion, uint8_t u8StartSector, uint8_t u8NumSectors);

/*!
@fn \
    int8_t m2m_flash_write(tenuWincFlashRegion enuRegion, void *pvBuffer, uint32_t u32Offset, uint32_t u32Size);

@brief  Write the number of bytes specified to a WINC flash location.

@param [in]     enuRegion
                    Region of flash in which to write.
@param [in]     pvBuffer
                    Pointer to buffer containing the data to write.
@param [in]     u32Offset
                    Byte offset (within region) of write location.
@param [in]     u32Size
                    Number of bytes to write.

@return     M2M_SUCCESS            - The write completed successfully.
            M2M_ERR_INVALID_ARG    - The parameters were invalid.
            M2M_ERR_FAIL           - The request encountered an error.

@pre        The WINC must have been initialised but must not be running. This can be done via
            @ref m2m_wifi_init_hold, @ref m2m_wifi_reinit_hold or @ref m2m_wifi_download_mode.

@warning    The write operation does not erase the flash device first, therefore the user
            must ensure the flash region has been erased first.
*/
int8_t m2m_flash_write(tenuWincFlashRegion enuRegion, void *pvBuffer, uint32_t u32Offset, uint32_t u32Size);

/*!
@fn \
    int8_t m2m_flash_read(tenuWincFlashRegion enuRegion, void *pvBuffer, uint32_t u32Offset, uint32_t u32Size);

@brief  Read the number of bytes specified from a WINC flash location.

@param [in]     enuRegion
                    Region of flash from which to read.
@param [out]    pvBuffer
                    Pointer to buffer to populate with the data being read.
@param [in]     u32Offset
                    Byte offset (within region) of read location.
@param [in]     u32Size
                    Number of bytes to read.

@return     M2M_SUCCESS            - The read completed successfully.
            M2M_ERR_INVALID_ARG    - The parameters were invalid.
            M2M_ERR_FAIL           - The request encountered an error.

@pre        The WINC must have been initialised but must not be running. This can be done via
            @ref m2m_wifi_init_hold, @ref m2m_wifi_reinit_hold or @ref m2m_wifi_download_mode.
*/
int8_t m2m_flash_read(tenuWincFlashRegion enuRegion, void *pvBuffer, uint32_t u32Offset, uint32_t u32Size);

/*!
@fn \
    int8_t m2m_flash_switch_firmware(void);

@brief  Switch the active and inactive firmware images.

@return     M2M_SUCCESS            - The firmware switch completed successfully.
            M2M_ERR_FAIL           - The request encountered an error.

@pre        The WINC must have been initialised but must not be running. This can be done via
            @ref m2m_wifi_init_hold, @ref m2m_wifi_reinit_hold or @ref m2m_wifi_download_mode.

*/
int8_t m2m_flash_switch_firmware(void);

/**@}*/
#endif /* __M2M_FLASH_H__ */
