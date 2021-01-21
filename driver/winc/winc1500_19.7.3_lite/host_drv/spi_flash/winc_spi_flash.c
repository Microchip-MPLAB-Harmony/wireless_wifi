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

#include "bsp/winc_bsp.h"
#include "common/winc_defines.h"
#include "common/winc_registers.h"
#include "common/winc_debug.h"
#include "driver/winc_spi.h"
#include "driver/winc_asic.h"
#include "winc_spi_flash.h"
#include "winc_spi_flash_map.h"

#define HOST_SHARE_MEM_BASE     (0xd0000UL)
/***********************************************************
SPI Flash DMA
***********************************************************/

static uint32_t gu32InternalFlashSize = 0;

/*********************************************/
/* STATIC FUNCTIONS                          */
/*********************************************/

static int8_t spi_flash_read_status_reg(uint8_t *pu8Val)
{
    uint32_t u32Reg;

    winc_bus_write_reg(SPI_FLASH_DATA_CNT, 4);
    winc_bus_write_reg(SPI_FLASH_BUF1, 0x05);
    winc_bus_write_reg(SPI_FLASH_BUF_DIR, 0x01);
    winc_bus_write_reg(SPI_FLASH_DMA_ADDR, DUMMY_REGISTER);
    winc_bus_write_reg(SPI_FLASH_CMD_CNT, 1 | (1 << 7));

    do
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(SPI_FLASH_TR_DONE, &u32Reg))
        {
            break;
        }
    }
    while (u32Reg != 1);

    u32Reg = winc_bus_read_reg(DUMMY_REGISTER);

    if (winc_bus_error())
    {
        return M2M_ERR_FAIL;
    }

    *pu8Val = (uint8_t)(u32Reg & 0xff);

    return M2M_SUCCESS;
}

static int8_t spi_flash_load_to_cortus_mem(uint32_t u32MemAdr, uint32_t u32FlashAdr, uint32_t u32Sz)
{
    uint32_t u32Reg;

    u32FlashAdr = CONF_WINC_UINT32_SWAP(u32FlashAdr);       // 24bit address swapped and placed in top 24bits
    u32FlashAdr |= 0x0b;

    winc_bus_write_reg(SPI_FLASH_DATA_CNT, u32Sz);
    winc_bus_write_reg(SPI_FLASH_BUF1, u32FlashAdr);
    winc_bus_write_reg(SPI_FLASH_BUF2, 0xa5);
    winc_bus_write_reg(SPI_FLASH_BUF_DIR, 0x1f);
    winc_bus_write_reg(SPI_FLASH_DMA_ADDR, u32MemAdr);
    winc_bus_write_reg(SPI_FLASH_CMD_CNT, 5 | (1 << 7));

    do
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(SPI_FLASH_TR_DONE, &u32Reg))
        {
            break;
        }
    }
    while (u32Reg != 1);

    if (winc_bus_error())
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

static int8_t spi_flash_sector_erase(uint32_t u32FlashAdr)
{
    uint32_t u32Reg;

    u32FlashAdr = CONF_WINC_UINT32_SWAP(u32FlashAdr);       // 24bit address swapped and placed in top 24bits
    u32FlashAdr |= 0x20;

    winc_bus_write_reg(SPI_FLASH_DATA_CNT, 0);
    winc_bus_write_reg(SPI_FLASH_BUF1, u32FlashAdr);
    winc_bus_write_reg(SPI_FLASH_BUF_DIR, 0x0f);
    winc_bus_write_reg(SPI_FLASH_DMA_ADDR, 0);
    winc_bus_write_reg(SPI_FLASH_CMD_CNT, 4 | (1 << 7));

    do
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(SPI_FLASH_TR_DONE, &u32Reg))
        {
            break;
        }
    }
    while (u32Reg != 1);

    if (winc_bus_error())
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

static int8_t spi_flash_write_enable(void)
{
    uint32_t u32Reg;

    winc_bus_write_reg(SPI_FLASH_DATA_CNT, 0);
    winc_bus_write_reg(SPI_FLASH_BUF1, 0x06);
    winc_bus_write_reg(SPI_FLASH_BUF_DIR, 0x01);
    winc_bus_write_reg(SPI_FLASH_DMA_ADDR, 0);
    winc_bus_write_reg(SPI_FLASH_CMD_CNT, 1 | (1 << 7));

    do
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(SPI_FLASH_TR_DONE, &u32Reg))
        {
            break;
        }
    }
    while (u32Reg != 1);

    if (winc_bus_error())
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

static int8_t spi_flash_write_disable(void)
{
    uint32_t u32Reg;

    winc_bus_write_reg(SPI_FLASH_DATA_CNT, 0);
    winc_bus_write_reg(SPI_FLASH_BUF1, 0x04);
    winc_bus_write_reg(SPI_FLASH_BUF_DIR, 0x01);
    winc_bus_write_reg(SPI_FLASH_DMA_ADDR, 0);
    winc_bus_write_reg(SPI_FLASH_CMD_CNT, 1 | (1 << 7));

    do
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(SPI_FLASH_TR_DONE, &u32Reg))
        {
            break;
        }
    }
    while (u32Reg != 1);

    if (winc_bus_error())
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

static int8_t spi_flash_page_program(uint32_t u32MemAdr, uint32_t u32FlashAdr, uint32_t u32Sz)
{
    uint32_t u32Reg;

    u32FlashAdr = CONF_WINC_UINT32_SWAP(u32FlashAdr);       // 24bit address swapped and placed in top 24bits
    u32FlashAdr |= 0x02;

    winc_bus_write_reg(SPI_FLASH_DATA_CNT, 0);
    winc_bus_write_reg(SPI_FLASH_BUF1, u32FlashAdr);
    winc_bus_write_reg(SPI_FLASH_BUF_DIR, 0x0f);
    winc_bus_write_reg(SPI_FLASH_DMA_ADDR, u32MemAdr);
    winc_bus_write_reg(SPI_FLASH_CMD_CNT, 4 | (1 << 7) | ((u32Sz & 0xfffff) << 8));

    do
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(SPI_FLASH_TR_DONE, &u32Reg))
        {
            break;
        }
    }
    while (u32Reg != 1);

    if (winc_bus_error())
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

static int8_t spi_flash_read_internal(uint8_t *pu8Buf, uint32_t u32Addr, uint32_t u32Sz)
{
    /* read size must be < 64KB */
    if (M2M_SUCCESS != spi_flash_load_to_cortus_mem(HOST_SHARE_MEM_BASE, u32Addr, u32Sz))
    {
        return M2M_ERR_FAIL;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_block(HOST_SHARE_MEM_BASE, pu8Buf, (uint_fast16_t)u32Sz))
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

static int8_t spi_flash_pp(uint32_t u32Offset, uint8_t *pu8Buf, uint_fast16_t u16Sz)
{
    uint8_t u8Reg;

    /* use shared packet memory as temp mem */
    if (WINC_BUS_SUCCESS != winc_bus_write_block(HOST_SHARE_MEM_BASE, pu8Buf, u16Sz))
    {
        return M2M_ERR_FAIL;
    }

    spi_flash_write_enable();
    spi_flash_page_program(HOST_SHARE_MEM_BASE, u32Offset, u16Sz);
    spi_flash_read_status_reg(&u8Reg);

    do
    {
        if (M2M_SUCCESS != spi_flash_read_status_reg(&u8Reg))
        {
            break;
        }
    }
    while (u8Reg & 0x01);

    spi_flash_write_disable();

    if (winc_bus_error())
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

static uint32_t spi_flash_rdid(void)
{
    uint32_t u32Reg;
    uint_fast16_t u16Cnt;

    winc_bus_write_reg(SPI_FLASH_DATA_CNT, 4);
    winc_bus_write_reg(SPI_FLASH_BUF1, 0x9f);
    winc_bus_write_reg(SPI_FLASH_BUF_DIR, 0x1);
    winc_bus_write_reg(SPI_FLASH_DMA_ADDR, DUMMY_REGISTER);
    winc_bus_write_reg(SPI_FLASH_CMD_CNT, 1 | (1 << 7));

    u16Cnt = 500;

    do
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(SPI_FLASH_TR_DONE, &u32Reg))
        {
            break;
        }
    }
    while ((u32Reg != 1) && (u16Cnt--));

    u32Reg = winc_bus_read_reg(DUMMY_REGISTER);

    if (winc_bus_error())
    {
        return 0;
    }

    WINC_LOG_INFO("Flash ID %" PRIx32, u32Reg);

    return u32Reg;
}

/*********************************************/
/* GLOBAL FUNCTIONS                          */
/*********************************************/

int8_t spi_flash_enable(uint8_t enable)
{
    uint32_t u32Reg;
    uint32_t u32Val;

    if (REV(winc_chip_get_id()) >= REV_3A0)
    {
        /* Enable pinmux to SPI flash. */
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_PIN_MUX_1, &u32Val))
        {
            return M2M_ERR_FAIL;
        }

        /* GPIO15/16/17/18 */
        u32Val &= ~((0x7777ul) << 12);
        u32Val |= ((0x1111ul) << 12);

        winc_bus_write_reg(NMI_PIN_MUX_1, u32Val);

        winc_bus_write_reg(SPI_FLASH_DATA_CNT, 0);

        if (enable)
        {
            winc_bus_write_reg(SPI_FLASH_BUF1, 0xab);
        }
        else
        {
            winc_bus_write_reg(SPI_FLASH_BUF1, 0xb9);
        }

        winc_bus_write_reg(SPI_FLASH_BUF_DIR, 0x1);
        winc_bus_write_reg(SPI_FLASH_DMA_ADDR, 0);
        winc_bus_write_reg(SPI_FLASH_CMD_CNT,  1 | (1 << 7));

        if (winc_bus_error())
        {
            return M2M_ERR_FAIL;
        }

        do
        {
            if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(SPI_FLASH_TR_DONE, &u32Reg))
            {
                break;
            }
        }
        while (u32Reg != 1);

        /* Disable pinmux to SPI flash to minimize leakage. */
        u32Val &= ~((0x7777ul) << 12);
        u32Val |= ((0x0010ul) << 12);
        winc_bus_write_reg(NMI_PIN_MUX_1, u32Val);
    }

    return M2M_SUCCESS;
}

int8_t spi_flash_read(uint8_t *pu8Buf, uint32_t u32offset, uint32_t u32Sz)
{
    if (u32Sz > FLASH_BLOCK_SIZE)
    {
        do
        {
            if (M2M_SUCCESS != spi_flash_read_internal(pu8Buf, u32offset, FLASH_BLOCK_SIZE))
            {
                return M2M_ERR_FAIL;
            }

            u32Sz     -= FLASH_BLOCK_SIZE;
            u32offset += FLASH_BLOCK_SIZE;
            pu8Buf    += FLASH_BLOCK_SIZE;
        }
        while (u32Sz > FLASH_BLOCK_SIZE);
    }

    return spi_flash_read_internal(pu8Buf, u32offset, u32Sz);
}

int8_t spi_flash_write(uint8_t *pu8Buf, uint32_t u32Offset, uint32_t u32Sz)
{
    uint_fast16_t u16wsz;
    uint32_t u32off;

    if (u32Sz == 0)
    {
        WINC_LOG_ERROR("Data size = %" PRIu32, u32Sz);
        return M2M_ERR_FAIL;
    }

    u32off = u32Offset % FLASH_PAGE_SZ;

    if (u32off)     /*first part of data in the address page*/
    {
        // Calculate remaining space in this sector
        u16wsz = (uint_fast16_t)(FLASH_PAGE_SZ - u32off);

        // Cap write to the requested size if smaller
        if (u16wsz > u32Sz)
        {
            u16wsz = (uint_fast16_t)u32Sz;
        }

        if (spi_flash_pp(u32Offset, pu8Buf, u16wsz) != M2M_SUCCESS)
        {
            return M2M_ERR_FAIL;
        }

        pu8Buf    += u16wsz;
        u32Offset += u16wsz;
        u32Sz     -= u16wsz;
    }

    while (u32Sz > 0)
    {
        if (u32Sz >= FLASH_PAGE_SZ)
        {
            u16wsz = FLASH_PAGE_SZ;
        }
        else
        {
            u16wsz = (uint_fast16_t)u32Sz;
        }

        /*write complete page or the remaining data*/
        if (spi_flash_pp(u32Offset, pu8Buf, u16wsz) != M2M_SUCCESS)
        {
            return M2M_ERR_FAIL;
        }

        pu8Buf    += u16wsz;
        u32Offset += u16wsz;
        u32Sz     -= u16wsz;
    }

    return M2M_SUCCESS;
}

int8_t spi_flash_erase(uint32_t u32Offset, uint32_t u32Sz)
{
    uint32_t i;
    uint8_t u8Reg;

    WINC_LOG_INFO("\r\n>Start erasing...");

    for (i = u32Offset; i < (u32Sz + u32Offset); i += (16 * FLASH_PAGE_SZ))
    {
        if (M2M_SUCCESS != spi_flash_write_enable())
        {
            return M2M_ERR_FAIL;
        }

        if (M2M_SUCCESS != spi_flash_read_status_reg(&u8Reg))
        {
            return M2M_ERR_FAIL;
        }

        if (M2M_SUCCESS != spi_flash_sector_erase(i))
        {
            return M2M_ERR_FAIL;
        }

        if (M2M_SUCCESS != spi_flash_read_status_reg(&u8Reg))
        {
            return M2M_ERR_FAIL;
        }

        do
        {
            if (M2M_SUCCESS != spi_flash_read_status_reg(&u8Reg))
            {
                return M2M_ERR_FAIL;
            }
        }
        while (u8Reg & 0x01);
    }

    WINC_LOG_INFO("Done");

    return M2M_SUCCESS;
}

uint32_t spi_flash_get_size(void)
{
    uint32_t u32FlashId;
    uint_fast8_t u8FlashPwr;

    if (gu32InternalFlashSize)
    {
        return gu32InternalFlashSize;
    }

    u32FlashId = spi_flash_rdid();

    if (u32FlashId != 0xffffffff)
    {
        /*flash size is the third byte from the FLASH RDID*/
        u8FlashPwr = ((u32FlashId >> 16) & 0xff) - 0x11; /*2MBIT is the min*/

        /*That number power 2 to get the flash size*/
        gu32InternalFlashSize = 1 << u8FlashPwr;

        WINC_LOG_INFO("Flash Size %" PRIu32 " Mb", gu32InternalFlashSize);
    }
    else
    {
        WINC_LOG_ERROR("Can't detect Flash size");
    }

    return gu32InternalFlashSize;
}
