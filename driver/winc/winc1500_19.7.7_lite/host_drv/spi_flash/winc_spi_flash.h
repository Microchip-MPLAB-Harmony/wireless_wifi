/**
 * \file
 *
 * \brief SPI flash API
 */
/*
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/** @defgroup SPIFLASHAPI SPI Flash
    @{
        @defgroup SPIFLASHFUNCTIONS Functions
        @{
        @}
    @}
 */

/**@addtogroup SPIFLASHAPI
* @{
@include spi_flash_example.c
@example spi_flash_example.c
* @}
*/

#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

/**@addtogroup SPIFLASHFUNCTIONS
 * @{
 */

/*!
@fn         int8_t spi_flash_enable(uint8_t enable);

@brief
            Enable SPI flash operations
*/
int8_t spi_flash_enable(uint8_t enable);

/*!
@fn         uint32_t spi_flash_get_size(void);

@brief
            Returns size of the flash in megabits.

@return
            SPI flash size in case of success and a zero value in case of failure.
*/
uint32_t spi_flash_get_size(void);

/*!
@fn         int8_t spi_flash_read(uint8_t *pu8Buf, uint32_t u32Addr, uint32_t u32Sz);

@brief
            Read a specified block of data from the SPI flash.

@param[out] pu8Buf
            Pointer to a data buffer to be filled with data.

@param[in]  u32Addr
            Address offset within the SPI flash to read the data from.

@param[in]  u32Sz
            Total size of data to be read (in bytes).

@note
            No firmware is required to be loaded on the WINC for the SPI flash to be accessed.

@warning
            The read must not exceed the last address of the flash.

@warning
            If the WINC device has running firmware it must be stopped before interacting with
            the SPI flash using @ref m2m_wifi_download_mode.

@see        m2m_wifi_download_mode

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t spi_flash_read(uint8_t *pu8Buf, uint32_t u32Addr, uint32_t u32Sz);

/*!
@fn         int8_t spi_flash_write(uint8_t* pu8Buf, uint32_t u32Offset, uint32_t u32Sz);

@brief
            Write a specified block of data to the SPI flash.

@param[in]  pu8Buf
            Pointer to a data buffer containing data to be written.

@param[in]  u32Offset
            Address offset within the SPI flash to write the data to.

@param[in]  u32Sz
            Total size of data to be written (in bytes).

@note
            No firmware is required to be loaded on the WINC for the SPI flash to be accessed.

@warning
            The read must not exceed the last address of the flash.

@warning
            If the WINC device has running firmware it must be stopped before interacting with
            the SPI flash using @ref m2m_wifi_download_mode.

@warning
            The flash sector should be erased before writing data using the function @ref spi_flash_erase
            otherwise the correct data will not be written.

@see        m2m_wifi_download_mode
@see        spi_flash_erase

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t spi_flash_write(uint8_t *pu8Buf, uint32_t u32Offset, uint32_t u32Sz);

/*!
@fn         int8_t spi_flash_erase(uint32_t u32Offset, uint32_t u32Sz);

@brief      Erase a specified range of sectors within the SPI flash.

@param[in]  u32Offset
            Address offset within the SPI flash to erase from.

@param[in]  u32Sz
            Total size of data to be erased (in bytes).

@note
            No firmware is required to be loaded on the WINC for the SPI flash to be accessed.

@note
            The SPI flash is erased in whole sectors, therefore if @p u32Offset is not aligned
            with a sector boundary it will rounded down to the nearest sector start address.
            If the erase range ends before a sector boundary it will be rounded up to include
            a complete sector.

@warning
            The read must not exceed the last address of the flash.

@warning
            If the WINC device has running firmware it must be stopped before interacting with
            the SPI flash using @ref m2m_wifi_download_mode.

@see        m2m_wifi_download_mode

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t spi_flash_erase(uint32_t u32Offset, uint32_t u32Sz);
/**@}*/

#endif  //__SPI_FLASH_H__
