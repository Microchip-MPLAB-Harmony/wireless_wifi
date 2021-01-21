/**
 *
 * \file
 *
 * \brief This module contains SPI protocol bus API implementation.
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
#include "common/winc_debug.h"
#include "common/winc_registers.h"
#include "winc_spi.h"

#define CMD_INTERNAL_WRITE      0xc3
#define CMD_INTERNAL_READ       0xc4
#define CMD_DMA_EXT_WRITE       0xc7
#define CMD_DMA_EXT_READ        0xc8
#define CMD_SINGLE_WRITE        0xc9
#define CMD_SINGLE_READ         0xca
#define CMD_RESET               0xcf

#define SPI_RESP_RETRY_COUNT    (10)
#define DATA_PKT_SZ             (8*1024)

static bool bSPIBusError = true;

static uint8_t spi_rsp(const uint8_t u8Type)
{
    int_fast8_t i = SPI_RESP_RETRY_COUNT;
    uint8_t u8Rsp;

    WINC_ASSERT( ((u8Type & 0xf0) == 0xc0) || (u8Type == 0xf0) );

    do
    {
        // Read single byte from SPI
        if (!winc_bsp_spi_read(&u8Rsp, 1))
        {
            WINC_LOG_ERROR("Failed response type read");
            return 0xff;
        }

        if ((0xf0 == u8Type) && ((u8Rsp & 0xf0) == 0xf0))
        {
            // If type is 0xf0 this is a data start header
            // so just return the packet order in the lower nibble
            return u8Rsp & 0xf0;
        }
        else if (u8Rsp == u8Type)
        {
            // Command response must match command type requested
            // to be valid. Proceed to read the state value
            if (!winc_bsp_spi_read(&u8Rsp, 1))
            {
                WINC_LOG_ERROR("Failed response state read");
                return 0xff;
            }

            return u8Rsp;
        }

        // SPI bus must be idle while not receiving valid response
        // only wait for number of bytes before failing response
    }
    while (!u8Rsp && i--);

    return 0xff;
}

static int_fast8_t spi_cmd_send(const uint8_t *pu8Buf, const uint_fast8_t u8Sz)
{
    WINC_ASSERT(pu8Buf != NULL);
    WINC_ASSERT(u8Sz >= 4);

    // Write command buffer to SPI
    if (!winc_bsp_spi_write(pu8Buf, u8Sz))
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed command write");
        return WINC_BUS_FAIL;
    }

    // Read back command response
    if (spi_rsp(pu8Buf[0]) != 0)
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed command response");
        return WINC_BUS_FAIL;
    }

    // Response was valid and indicated no error condition

    return WINC_BUS_SUCCESS;
}

static int_fast8_t spi_data_read(uint8_t *pu8Buf, uint_fast16_t u16Sz)
{
    uint_fast16_t u16NumBytes;

    WINC_ASSERT(pu8Buf != NULL);
    WINC_ASSERT(u16Sz > 0);

    while (u16Sz)
    {
        // Limit size of a single transfer to DATA_PKT_SZ bytes
        if (u16Sz <= DATA_PKT_SZ)
        {
            u16NumBytes = u16Sz;
        }
        else
        {
            u16NumBytes = DATA_PKT_SZ;
        }

        // Get the data start byte and check for error in lower nibble
        if (spi_rsp(0xf0) & 0x0c)
        {
            WINC_ASSERT(0);
            WINC_LOG_ERROR("Failed data response read");
            return WINC_BUS_FAIL;
        }

        // Read data packet
        if (!winc_bsp_spi_read(pu8Buf, u16NumBytes))
        {
            WINC_ASSERT(0);
            WINC_LOG_ERROR("Failed data block read");
            return WINC_BUS_FAIL;
        }

        pu8Buf += u16NumBytes;
        u16Sz -= u16NumBytes;
    }

    return WINC_BUS_SUCCESS;
}

static int_fast8_t spi_data_write(const uint8_t *pu8Buf, uint_fast16_t u16Sz)
{
    uint_fast16_t u16NumBytes;
    uint8_t u8Cmd;

    WINC_ASSERT(pu8Buf != NULL);
    WINC_ASSERT(u16Sz > 0);

    // Default to data start as first packet in sequence
    u8Cmd = 0xf1;

    while (u16Sz)
    {
        // Limit size of a single transfer to DATA_PKT_SZ bytes
        if (u16Sz <= DATA_PKT_SZ)
        {
            u16NumBytes = u16Sz;
        }
        else
        {
            u16NumBytes = DATA_PKT_SZ;
        }

        // Calculate remaining data size, if there is no more data after
        // this packet then change the packet order to last in sequence

        u16Sz -= u16NumBytes;

        if (!u16Sz)
        {
            u8Cmd = 0xf3;
        }

        // Write data start and packet order values
        if (!winc_bsp_spi_write(&u8Cmd, 1))
        {
            WINC_ASSERT(0);
            WINC_LOG_ERROR("Failed data block command write");
            return WINC_BUS_FAIL;
        }

        // Write data packet to SPI
        if (!winc_bsp_spi_write(pu8Buf, u16NumBytes))
        {
            WINC_ASSERT(0);
            WINC_LOG_ERROR("Failed data block write");
            return WINC_BUS_FAIL;
        }

        // Change default packet order to not first or last in sequence
        u8Cmd = 0xf2;

        pu8Buf += u16NumBytes;
    }

    return WINC_BUS_SUCCESS;
}

static int_fast8_t spi_read_reg(uint32_t u32Addr, uint32_t *pu32RetVal)
{
    uint8_t u8TmpBuf[4];
    uint8_t u8CmdBuf[4];

    WINC_ASSERT(pu32RetVal != NULL);

    if (u32Addr <= 0xff)
    {
        // Read is from clockless registers so setup command for internal read
        u8CmdBuf[0] = CMD_INTERNAL_READ;
        u8CmdBuf[1] = (1 << 7);

        // Shift register address up so following address buffer store puts the
        // address in the right place
        u32Addr <<= 8;
    }
    else
    {
        // Read is from clocked registers so setup command for normal read
        u8CmdBuf[0] = CMD_SINGLE_READ;

        // Store first byte of register address before continuing below
        u8CmdBuf[1] = (uint8_t)(u32Addr >> 16);
    }

    // For clockless register addresses only the first store here is used which
    // undoes the shift done above. For clocked registers the remaining two bytes
    // of the address are stored and used.
    u8CmdBuf[2] = (uint8_t)(u32Addr >> 8);
    u8CmdBuf[3] = (uint8_t)u32Addr;

    // Send command buffer via SPI bus
    if (WINC_BUS_SUCCESS != spi_cmd_send(u8CmdBuf, 4))
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed command, read reg (%08" PRIx32 ")", u32Addr);
        return WINC_BUS_FAIL;
    }

    // Read data, to avoid endianness issue use a temporary buffer to receive the
    // bytes in WINC order.
    if (WINC_BUS_SUCCESS != spi_data_read(&u8TmpBuf[0], 4))
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed data read");
        return WINC_BUS_FAIL;
    }

    // Translate received bytes into 32-bit value
    *pu32RetVal = u8TmpBuf[0] |
                  ((uint32_t)u8TmpBuf[1] << 8) |
                  ((uint32_t)u8TmpBuf[2] << 16) |
                  ((uint32_t)u8TmpBuf[3] << 24);

    return WINC_BUS_SUCCESS;
}

static int_fast8_t spi_write_reg(const uint32_t u32Addr, const uint32_t u32Val, bool bCheckRsp)
{
    uint_fast8_t cmd_sz;
    uint8_t u8CmdBuf[8];
    uint8_t *pu8CmdBuf = u8CmdBuf;

    if (u32Addr <= 0x30)
    {
        // Write is to clockless registers so setup command for internal write
        u8CmdBuf[0] = CMD_INTERNAL_WRITE;

        u8CmdBuf[1] = (1 << 7);

        cmd_sz = 7;
        pu8CmdBuf += 2;
    }
    else
    {
        // Write is to clocked registers so setup command for normal write
        u8CmdBuf[0] = CMD_SINGLE_WRITE;

        // Store first two bytes of address, the last is completed below in
        // common with clockless write.
        u8CmdBuf[1] = (uint8_t)(u32Addr >> 16);
        u8CmdBuf[2] = (uint8_t)(u32Addr >> 8);

        cmd_sz = 8;
        pu8CmdBuf += 3;
    }

    // Store last byte of address (only byte for clockless registers)
    pu8CmdBuf[0] = (uint8_t)(u32Addr);

    // Store new register value in WINC order
    pu8CmdBuf[1] = (uint8_t)(u32Val >> 24);
    pu8CmdBuf[2] = (uint8_t)(u32Val >> 16);
    pu8CmdBuf[3] = (uint8_t)(u32Val >> 8);
    pu8CmdBuf[4] = (uint8_t)(u32Val);

    if (bCheckRsp)
    {
        // Send command and check for valid response
        if (WINC_BUS_SUCCESS != spi_cmd_send(u8CmdBuf, cmd_sz))
        {
            WINC_ASSERT(0);
            WINC_LOG_ERROR("Failed command, write reg (%08" PRIx32 ")", u32Addr);
            return WINC_BUS_FAIL;
        }
    }
    else
    {
        // Send command but don't look for a response
        if (!winc_bsp_spi_write(u8CmdBuf, cmd_sz))
        {
            WINC_ASSERT(0);
            WINC_LOG_ERROR("Failed command write");
            return WINC_BUS_FAIL;
        }
    }

    return WINC_BUS_SUCCESS;
}

int_fast8_t winc_bus_write_reg(const uint32_t u32Addr, const uint32_t u32Val)
{
    // External API: Don't run if bus error is present, preset error so only
    // successful path leads to clearing the error condition
    WINC_CRIT_SEC_BUS_ENTER;

    if (bSPIBusError)
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    bSPIBusError = true;

    if (WINC_BUS_SUCCESS != spi_write_reg(u32Addr, u32Val, 1))
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    // Clear bus error condition as operation was successful
    bSPIBusError = false;
    WINC_CRIT_SEC_BUS_LEAVE;
    return WINC_BUS_SUCCESS;
}

int_fast8_t winc_bus_write_reg_no_rsp(const uint32_t u32Addr, const uint32_t u32Val)
{
    // External API: Don't run if bus error is present, preset error so only
    // successful path leads to clearing the error condition
    WINC_CRIT_SEC_BUS_ENTER;

    if (bSPIBusError)
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    bSPIBusError = true;

    if (WINC_BUS_SUCCESS != spi_write_reg(u32Addr, u32Val, 0))
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    // Clear bus error condition as operation was successful
    bSPIBusError = false;
    WINC_CRIT_SEC_BUS_LEAVE;
    return WINC_BUS_SUCCESS;
}

int_fast8_t winc_bus_write_block(uint32_t u32Addr, const uint8_t *pu8Buf, uint_fast16_t u16Sz)
{
    uint8_t u8CmdBuf[7] = {CMD_DMA_EXT_WRITE};

    WINC_ASSERT(pu8Buf != NULL);
    WINC_ASSERT(u16Sz > 0);

    // External API: Don't run if bus error is present, preset error so only
    // successful path leads to clearing the error condition
    WINC_CRIT_SEC_BUS_ENTER;

    if (bSPIBusError)
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    bSPIBusError = true;

    //Workaround hardware problem with single byte transfers over SPI bus
    if (u16Sz == 1)
    {
        u16Sz = 2;
    }

    // Store destination address and transfer size into buffer
    u8CmdBuf[1] = (uint8_t)(u32Addr >> 16);
    u8CmdBuf[2] = (uint8_t)(u32Addr >> 8);
    u8CmdBuf[3] = (uint8_t)u32Addr;
    u8CmdBuf[4] = 0;
    u8CmdBuf[5] = (uint8_t)(u16Sz >> 8);
    u8CmdBuf[6] = (uint8_t)(u16Sz);

    // Send command buffer
    if (WINC_BUS_SUCCESS != spi_cmd_send(u8CmdBuf, 7))
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed command, write block (%08" PRIx32 ")", u32Addr);
        return WINC_BUS_FAIL;
    }

    // Send data packet
    if (WINC_BUS_SUCCESS != spi_data_write(pu8Buf, u16Sz))
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed block data write");
        return WINC_BUS_FAIL;
    }

    // Check data response
    if (spi_rsp(0xc3) != 0)
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed block data response");
        return WINC_BUS_FAIL;
    }

    // Clear bus error condition as operation was successful
    bSPIBusError = false;
    WINC_CRIT_SEC_BUS_LEAVE;
    return WINC_BUS_SUCCESS;
}

int_fast8_t winc_bus_read_block(const uint32_t u32Addr, uint8_t *pu8Buf, uint_fast16_t u16Sz)
{
    int_fast8_t i8Result;
    uint8_t u8TmpBuf[2];
    uint8_t u8CmdBuf[7] = {CMD_DMA_EXT_READ};

    WINC_ASSERT(pu8Buf != NULL);
    WINC_ASSERT(u16Sz > 0);

    // External API: Don't run if bus error is present, preset error so only
    // successful path leads to clearing the error condition
    WINC_CRIT_SEC_BUS_ENTER;

    if (bSPIBusError)
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    bSPIBusError = true;

    // Store address into buffer
    u8CmdBuf[1] = (uint8_t)(u32Addr >> 16);
    u8CmdBuf[2] = (uint8_t)(u32Addr >> 8);
    u8CmdBuf[3] = (uint8_t)u32Addr;

    // Store first byte of transfer size into buffer
    u8CmdBuf[4] = 0;

    if (u16Sz == 1)
    {
        // Possible issue with single byte transfers, request two bytes instead
        u8CmdBuf[5] = 0;
        u8CmdBuf[6] = 2;
    }
    else
    {
        // Store remaining bytes of transfer size into buffer
        u8CmdBuf[5] = (uint8_t)(u16Sz >> 8);
        u8CmdBuf[6] = (uint8_t)(u16Sz);
    }

    // Send command buffer
    if (WINC_BUS_SUCCESS != spi_cmd_send(u8CmdBuf, 7))
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed command, read block (%08" PRIx32 ")", u32Addr);
        return WINC_BUS_FAIL;
    }

    // Read data packet
    if (u16Sz == 1)
    {
        // For single byte transfer go via a temporary buffer to
        // ensure second byte requested doesn't trash memory if caller
        // only supplied a single byte buffer
        i8Result = spi_data_read(u8TmpBuf, 2);
        pu8Buf[0] = u8TmpBuf[0];
    }
    else
    {
        i8Result = spi_data_read(pu8Buf, u16Sz);
    }

    if (WINC_BUS_SUCCESS != i8Result)
    {
        WINC_ASSERT(0);
        WINC_LOG_ERROR("Failed block data read");
        return WINC_BUS_FAIL;
    }

    // Clear bus error condition as operation was successful
    bSPIBusError = false;
    WINC_CRIT_SEC_BUS_LEAVE;
    return WINC_BUS_SUCCESS;
}

int_fast8_t winc_bus_init(void)
{
    static const uint8_t u8CmdBuf[9] =
    {
        CMD_SINGLE_WRITE,
        0x00,
        (NMI_SPI_PROTOCOL_CONFIG >> 8) & 0xff,
        (NMI_SPI_PROTOCOL_CONFIG) & 0xff,
        0x00,
        0x00,
        0x00,
        0x52,
        0x5c
    };

    WINC_CRIT_SEC_BUS_INIT;

    // Initialize bus error to clear, send SPI protocol configuration
    // as first test of SPI bus.
    bSPIBusError = false;

    return spi_cmd_send(u8CmdBuf, 9);
}

int_fast8_t winc_bus_reset(void)
{
    static const uint8_t u8CmdBuf[5] = {CMD_RESET, 0xff, 0xff, 0xff, 0x00/*, 0xaa*/};

    // Clear bus error condition to allow command send to work
    bSPIBusError = false;

#ifdef WINC_CRIT_SEC_BUS

    if (WINC_BUS_SUCCESS != spi_cmd_send(u8CmdBuf, 5))
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    WINC_CRIT_SEC_BUS_LEAVE;

    return WINC_BUS_SUCCESS;
#else
    // Send command to reset SPI
    return spi_cmd_send(u8CmdBuf, 5);
#endif
}

int_fast8_t winc_bus_deinit(void)
{
    WINC_CRIT_SEC_BUS_DEINIT;

    // Set bus error condition so all external APIs related to actual SPI use
    // effectively become NOPs
    bSPIBusError = true;

    return WINC_BUS_SUCCESS;
}

uint32_t winc_bus_read_reg(const uint32_t u32Addr)
{
    uint32_t u32Val;

    // External API: Don't run if bus error is present, preset error so only
    // successful path leads to clearing the error condition
    WINC_CRIT_SEC_BUS_ENTER;

    if (bSPIBusError)
    {
        WINC_ASSERT(0);
        return 0;
    }

    bSPIBusError = true;

    if (WINC_BUS_SUCCESS != spi_read_reg(u32Addr, &u32Val))
    {
        WINC_ASSERT(0);
        return 0;
    }

    // Clear bus error condition as operation was successful
    bSPIBusError = false;
    WINC_CRIT_SEC_BUS_LEAVE;
    return u32Val;
}

int_fast8_t winc_bus_read_reg_with_ret(const uint32_t u32Addr, uint32_t *pu32RetVal)
{
    WINC_ASSERT(pu32RetVal != NULL);

    // External API: Don't run if bus error is present, preset error so only
    // successful path leads to clearing the error condition
    WINC_CRIT_SEC_BUS_ENTER;

    if (bSPIBusError)
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    bSPIBusError = true;

    if (WINC_BUS_SUCCESS != spi_read_reg(u32Addr, pu32RetVal))
    {
        WINC_ASSERT(0);
        return WINC_BUS_FAIL;
    }

    // Clear bus error condition as operation was successful
    bSPIBusError = false;
    WINC_CRIT_SEC_BUS_LEAVE;
    return WINC_BUS_SUCCESS;
}

int_fast8_t winc_bus_set_reg_bits(uint32_t u32Addr, uint32_t u32Bits)
{
    uint32_t u32Reg;

    // External API: Only calls other external APIs so if bus error is present
    // this function becomes NOP

    // Read the current value of the register
    winc_bus_read_reg_with_ret(u32Addr, &u32Reg);

    if ((u32Reg & u32Bits) != u32Bits)
    {
        // Only set the bits request if they are not already set
        u32Reg |= u32Bits;

        winc_bus_write_reg(u32Addr, u32Reg);
    }

    if (winc_bus_error())
    {
        return WINC_BUS_FAIL;
    }

    return WINC_BUS_SUCCESS;
}

int_fast8_t winc_bus_clear_reg_bits(uint32_t u32Addr, uint32_t u32Bits)
{
    uint32_t u32Reg;

    // External API: Only calls other external APIs so if bus error is present
    // this function becomes NOP

    // Read the current value of the register
    winc_bus_read_reg_with_ret(u32Addr, &u32Reg);

    if (u32Reg & u32Bits)
    {
        // Only clear the bits request if some of them are actually set
        u32Reg &= ~u32Bits;

        winc_bus_write_reg(u32Addr, u32Reg);
    }

    if (winc_bus_error())
    {
        return WINC_BUS_FAIL;
    }

    return WINC_BUS_SUCCESS;
}

bool winc_bus_error(void)
{
    bool bRetErrorStatus;

    WINC_CRIT_SEC_BUS_ENTER;
    WINC_ASSERT(bSPIBusError == false);

    bRetErrorStatus = bSPIBusError;
    WINC_CRIT_SEC_BUS_LEAVE;

    // Return current bus error condition
    return bRetErrorStatus;
}
