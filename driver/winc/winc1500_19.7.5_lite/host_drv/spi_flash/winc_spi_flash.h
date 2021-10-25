/**
 * \file
 *
 * \brief SPI flash API
 *
 * Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
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
