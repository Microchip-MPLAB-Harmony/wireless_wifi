/*******************************************************************************
  WINC Driver NVM Access Implementation

  File Name:
    wdrv_winc_httpprovctx.c

  Summary:
    WINC wireless driver NVM access implementation.

  Description:
     This interface manages access to the SPI flash of the WINC device.
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

#include <stdint.h>

#include "configuration.h"
#include "definitions.h"
#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_nvm.h"
#include "spi_flash.h"
#ifdef WDRV_WINC_DEVICE_FLEXIBLE_FLASH_MAP
#include "flexible_flash.h"
#else
#include "spi_flash_map.h"
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

#ifndef WDRV_WINC_DEVICE_FLEXIBLE_FLASH_MAP
typedef struct
{
    uint32_t address;
    uint32_t size;
} WDRV_WINC_FLASH_MAP_ENTRY;
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

#ifndef M2M_TLS_ROOTCER_FLASH_SZ
#define M2M_TLS_ROOTCER_FLASH_SZ M2M_TLS_ROOTCER_FLASH_SIZE
#endif

#ifndef M2M_TLS_SERVER_FLASH_SZ
#define M2M_TLS_SERVER_FLASH_SZ M2M_TLS_SERVER_FLASH_SIZE
#endif

#ifdef WDRV_WINC_DEVICE_FLEXIBLE_FLASH_MAP
static const uint16_t flashLocationIDMap[] = {
    0x0000, // WDRV_WINC_NVM_REGION_RAW,
    0x0011, // WDRV_WINC_NVM_REGION_FIRMWARE,
    0x0021, // WDRV_WINC_NVM_REGION_PLL_TABLE,
    0x0021, // WDRV_WINC_NVM_REGION_GAIN_TABLE,
    0x0031, // WDRV_WINC_NVM_REGION_ROOT_CERTS,
    0x0036, // WDRV_WINC_NVM_REGION_TLS_CERTS,
    0x0034, // WDRV_WINC_NVM_REGION_CONN_PARAM,
    0x0035, // WDRV_WINC_NVM_REGION_HTTP_FILES,
#ifdef WDRV_WINC_DEVICE_HOST_FILE_DOWNLOAD
    0x0041, // WDRV_WINC_NVM_REGION_HOST_FILE,
#endif
};
#else
static const WDRV_WINC_FLASH_MAP_ENTRY flashMap[] =
{
    {0                              , 0                         },  // WDRV_WINC_NVM_REGION_RAW,
    {M2M_FIRMWARE_FLASH_OFFSET      , M2M_FIRMWARE_FLASH_SZ     },  // WDRV_WINC_NVM_REGION_FIRMWARE,
    {M2M_PLL_FLASH_OFFSET           , M2M_PLL_FLASH_SZ          },  // WDRV_WINC_NVM_REGION_PLL_TABLE,
    {M2M_GAIN_FLASH_OFFSET          , M2M_GAIN_FLASH_SZ         },  // WDRV_WINC_NVM_REGION_GAIN_TABLE,
    {M2M_TLS_ROOTCER_FLASH_OFFSET   , M2M_TLS_ROOTCER_FLASH_SZ  },  // WDRV_WINC_NVM_REGION_ROOT_CERTS,
    {M2M_TLS_SERVER_FLASH_OFFSET    , M2M_TLS_SERVER_FLASH_SZ   },  // WDRV_WINC_NVM_REGION_TLS_CERTS,
    {M2M_CACHED_CONNS_FLASH_OFFSET  , M2M_CACHED_CONNS_FLASH_SZ },  // WDRV_WINC_NVM_REGION_CONN_PARAM,
    {M2M_HTTP_MEM_FLASH_OFFSET      , M2M_HTTP_MEM_FLASH_SZ     },  // WDRV_WINC_NVM_REGION_HTTP_FILES,
#ifdef WDRV_WINC_DEVICE_HOST_FILE_DOWNLOAD
    {0x80000                        , 512*1024                  },  // WDRV_WINC_NVM_REGION_HOST_FILE,
#endif
};
#endif

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver NVM Access Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    static bool _WDRV_WINC_NVMFindSection
    (
        WDRV_WINC_NVM_REGION region,
        uint32_t *pStartAddr,
        uint32_t *pSize
    )

  Summary:
    Resolve NVM regions types into address and size of region.

  Description:
    This function looks up the NVM region and determines the regions absolute
    address within the SPI flash and the size of the region.

  Precondition:
    None.

  Parameters:
    region      - Region type.
    pStartAddr  - Pointer to variable to receive region start address.
    pSize       - Pointer to variable to receive region size.

  Returns:
    - true for success.
    - false for failure.

  Remarks:
    None.

*/

static bool _WDRV_WINC_NVMFindSection
(
    WDRV_WINC_NVM_REGION region,
    uint32_t *pStartAddr,
    uint32_t *pSize
)
{
    /* Ensure the pointers and region are valid. */
    if ((NULL == pStartAddr) || (NULL == pSize) || (region >= NUM_WDRV_WINC_NVM_REGIONS))
    {
        return false;
    }

    /* For the raw region resolve the full flash space, otherwise lookup
       the region location and size from the flexible flash map. */
    if (WDRV_WINC_NVM_REGION_RAW == region)
    {
        *pStartAddr = 0;
        *pSize = spi_flash_get_size() << 17;
    }
#ifdef WDRV_WINC_DEVICE_FLEXIBLE_FLASH_MAP
    else if (M2M_SUCCESS != spi_flexible_flash_find_section(flashLocationIDMap[region], pStartAddr, pSize))
    {
        return false;
    }

    switch(region)
    {
        case WDRV_WINC_NVM_REGION_PLL_TABLE:
        {
            *pSize = 0x400;
            break;
        }

        case WDRV_WINC_NVM_REGION_GAIN_TABLE:
        {
            *pStartAddr += 0x400;
            *pSize      -= 0x400;
            break;
        }

        default:
        {
            break;
        }
    }
#else
    else
    {
        *pStartAddr = flashMap[region].address;
        *pSize      = flashMap[region].size;
    }
#endif

    WDRV_DBG_INFORM_PRINT("NVM Lookup %2d: 0x%06x %0d\r\n", region, *pStartAddr, *pSize);

    return true;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMEraseSector
    (
        DRV_HANDLE handle,
        WDRV_WINC_NVM_REGION region,
        uint8_t startSector,
        uint8_t numSectors
    );

  Summary:
    Erase a sector within an NVM region.

  Description:
    Erases a number of sectors which exist within the requested region.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_NVMEraseSector
(
    DRV_HANDLE handle,
    WDRV_WINC_NVM_REGION region,
    uint8_t startSector,
    uint8_t numSectors
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    uint32_t flashAddress;
    uint32_t flashRegionSize;

    /* Ensure the driver handle and region are valid. */
    if ((NULL == pDcpt) || (region >= NUM_WDRV_WINC_NVM_REGIONS))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

#ifdef WDRV_WINC_DEVICE_WINC1500
    /* Ensure flash is out of power save mode. */
    if (M2M_SUCCESS != spi_flash_enable(1))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }
#endif

    /* Find region address and size. */
    if (false == _WDRV_WINC_NVMFindSection(region, &flashAddress, &flashRegionSize))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Check requested size fits within region size. */
    if ((numSectors * FLASH_SECTOR_SZ) > flashRegionSize)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Find start address of area within requested region. */
    flashAddress += (startSector * FLASH_SECTOR_SZ);

    /* Erase the requested sectors. */
    if (M2M_SUCCESS != spi_flash_erase(flashAddress, numSectors * FLASH_SECTOR_SZ))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

#ifdef WDRV_WINC_DEVICE_WINC1500
    /* Return flash to power save mode. */
    if (M2M_SUCCESS != spi_flash_enable(0))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }
#endif

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMWrite
    (
        DRV_HANDLE handle,
        WDRV_WINC_NVM_REGION region,
        void *pBuffer,
        uint32_t offset,
        uint32_t size
    );

  Summary:
    Writes data to an NVM region.

  Description:
    Writes the number of bytes specified to the NVM region.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_NVMWrite
(
    DRV_HANDLE handle,
    WDRV_WINC_NVM_REGION region,
    void *pBuffer,
    uint32_t offset,
    uint32_t size
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    uint32_t flashAddress;
    uint32_t flashRegionSize;

    /* Ensure the driver handle, buffer pointer and region are valid. */
    if ((NULL == pDcpt) || (NULL == pBuffer) || (region >= NUM_WDRV_WINC_NVM_REGIONS))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

#ifdef WDRV_WINC_DEVICE_WINC1500
    /* Ensure flash is out of power save mode. */
    if (M2M_SUCCESS != spi_flash_enable(1))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }
#endif

    /* Find region address and size. */
    if (false == _WDRV_WINC_NVMFindSection(region, &flashAddress, &flashRegionSize))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Check requested size fits within region size. */
    if ((offset + size) > flashRegionSize)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Find start address of area within requested region. */
    flashAddress += offset;

    /* Write data to flash. */
    if (M2M_SUCCESS != spi_flash_write(pBuffer, flashAddress, size))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

#ifdef WDRV_WINC_DEVICE_WINC1500
    /* Return flash to power save mode. */
    if (M2M_SUCCESS != spi_flash_enable(0))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }
#endif
    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMRead
    (
        DRV_HANDLE handle,
        WDRV_WINC_NVM_REGION region,
        void *pBuffer,
        uint32_t offset,
        uint32_t size
    );

  Summary:
    Reads data from an NVM region.

  Description:
    Reads the number of bytes specified from the NVM region.

  Remarks:
    See wdrv_winc_nvm.h for usage information.

*/
WDRV_WINC_STATUS WDRV_WINC_NVMRead
(
    DRV_HANDLE handle,
    WDRV_WINC_NVM_REGION region,
    void *pBuffer,
    uint32_t offset,
    uint32_t size
)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    uint32_t flashAddress;
    uint32_t flashRegionSize;

    /* Ensure the driver handle, buffer pointer and region are valid. */
    if ((NULL == pDcpt) || (NULL == pBuffer) || (region >= NUM_WDRV_WINC_NVM_REGIONS))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

#ifdef WDRV_WINC_DEVICE_WINC1500
    /* Ensure flash is out of power save mode. */
    if (M2M_SUCCESS != spi_flash_enable(1))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }
#endif

    /* Find region address and size. */
    if (false == _WDRV_WINC_NVMFindSection(region, &flashAddress, &flashRegionSize))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Check requested size fits within region size. */
    if ((offset + size) > flashRegionSize)
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Find start address of area within requested region. */
    flashAddress += offset;

    /* Read data from flash. */
    if (M2M_SUCCESS != spi_flash_read(pBuffer, flashAddress, size))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

#ifdef WDRV_WINC_DEVICE_WINC1500
    /* Return flash to power save mode. */
    if (M2M_SUCCESS != spi_flash_enable(0))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }
#endif

    return WDRV_WINC_STATUS_OK;
}
