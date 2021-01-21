/**
 *
 * \file
 *
 * \brief This module contains host interface APIs implementation.
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
#include "winc_spi.h"
#include "winc_hif.h"
#include "winc_asic.h"

static uint_fast8_t         gu8ChipSleep;
static uint32_t             gu32RxAddr;
static uint_fast16_t        gu16RxSize;
static bool                 gbChipPwrSave;

static void hif_null_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
{
    UNUSED_VAR(u8OpCode);
    UNUSED_VAR(u16DataSize);
    UNUSED_VAR(u32Addr);
}

static const tpfHifCallBack gHifCallbacks[] =
{
    CONF_WINC_HIF_CB_WIFI_HANDLER,
    CONF_WINC_HIF_CB_IP_HANDLER,
    CONF_WINC_HIF_CB_HIF_HANDLER,
    CONF_WINC_HIF_CB_OTA_HANDLER,
    CONF_WINC_HIF_CB_SSL_HANDLER,
    CONF_WINC_HIF_CB_CRYPTO_HANDLER,
    CONF_WINC_HIF_CB_SIGMA_HANDLER,
};

#define WINC_HIF_CB_HIGHEST_GID     (sizeof(gHifCallbacks)/sizeof(tpfHifCallBack))

int8_t winc_hif_chip_wake(void)
{
    if (gu8ChipSleep++ == 0)
    {
        if (gbChipPwrSave)
        {
            if (!winc_chip_wake())
            {
                return M2M_ERR_FAIL;
            }
        }
    }

    return M2M_SUCCESS;
}

void winc_hif_set_power_save(bool bPwrSave)
{
    gbChipPwrSave = bPwrSave;
}

int8_t winc_hif_chip_sleep(void)
{
    if (gu8ChipSleep)
    {
        gu8ChipSleep--;
    }

    if (gu8ChipSleep == 0)
    {
        if (gbChipPwrSave)
        {
            if (!winc_chip_sleep())
            {
                return M2M_ERR_FAIL;
            }
        }
    }

    return M2M_SUCCESS;
}

int8_t winc_hif_init(void)
{
    gu8ChipSleep = 0;
    gbChipPwrSave = false;

    WINC_CRIT_SEC_HIF_INIT;

    return M2M_SUCCESS;
}

int8_t winc_hif_deinit(void)
{
    WINC_CRIT_SEC_HIF_DEINIT;

    return winc_hif_chip_wake();
}

int8_t winc_hif_send(uint8_t u8Gid, uint8_t u8Opcode, const void *pvCtrlBuf, uint16_t u16CtrlBufSize,
                     const void *pvDataBuf, uint16_t u16DataSize, uint16_t u16DataOffset)
{
    tstrHifHdr      strHif;
    uint32_t        u32Reg;
    uint32_t        u32DMAAddr;
    uint_fast16_t   u16Count;
    uint32_t        u32CurrAddr;
    uint32_t        u32HifLength = M2M_HIF_HDR_OFFSET;

    if (pvDataBuf != NULL)
    {
        if ((pvCtrlBuf != NULL) && (u16CtrlBufSize > u16DataOffset))
        {
            WINC_LOG_ERROR("HIF send, data will overwrite ctrl");
            return M2M_ERR_SEND;
        }

        u32HifLength += u16DataOffset + u16DataSize;
    }
    else
    {
        u32HifLength += u16CtrlBufSize;
    }

    if (u32HifLength > M2M_HIF_MAX_PACKET_SIZE)
    {
        WINC_LOG_ERROR("HIF message length (%" PRIx32 ") exceeds max length (%d)", u32HifLength, M2M_HIF_MAX_PACKET_SIZE);
        return M2M_ERR_SEND;
    }

    strHif.u8Opcode     = u8Opcode & (~NBIT7);
    strHif.u8Gid        = u8Gid;
    strHif.u16Length    = (uint16_t)u32HifLength;

    WINC_CRIT_SEC_HIF_ENTER;

    if (M2M_SUCCESS != winc_hif_chip_wake())
    {
        WINC_LOG_ERROR("Failed to wakeup the chip");

        WINC_CRIT_SEC_HIF_LEAVE;
        return M2M_ERR_SEND;
    }

    u32Reg = (uint32_t)u8Gid;
    u32Reg |= ((uint32_t)u8Opcode << 8);
    u32Reg |= ((uint32_t)strHif.u16Length << 16);
    winc_bus_write_reg(NMI_STATE_REG, u32Reg);

    winc_bus_write_reg(WIFI_HOST_RCV_CTRL_2, NBIT1);

    u32DMAAddr = 0;

    u16Count = 1000;

    while (u16Count--)
    {
        winc_bus_read_reg_with_ret(WIFI_HOST_RCV_CTRL_2, &u32Reg);

        if (winc_bus_error())
        {
            winc_hif_chip_sleep();

            WINC_CRIT_SEC_HIF_LEAVE;
            return M2M_ERR_SEND;
        }

        if (!(u32Reg & NBIT1))
        {
            winc_bus_read_reg_with_ret(WIFI_HOST_RCV_CTRL_4, &u32DMAAddr);
            break;
        }

        /*
            * If it takes too long to get a response, the slow down to
            * avoid back-to-back register read operations.
            */
        if (u16Count <= 500)
        {
            if (u16Count == 500)
            {
                WINC_LOG_INFO("Slowing down...");
            }

            winc_bsp_sleep(1);
        }
    }

    if (winc_bus_error() || !u32DMAAddr)
    {
        winc_hif_chip_sleep();
        WINC_LOG_DEBUG("Failed to receive DMA address");

        WINC_CRIT_SEC_HIF_LEAVE;
        return M2M_ERR_MEM_ALLOC;
    }

    u32CurrAddr = u32DMAAddr;
    strHif.u16Length = HOST_TO_WINC_U16(strHif.u16Length);

    winc_bus_write_block(u32CurrAddr, (uint8_t *)&strHif, M2M_HIF_HDR_OFFSET);

    u32CurrAddr += M2M_HIF_HDR_OFFSET;

    if (pvCtrlBuf != NULL)
    {
        winc_bus_write_block(u32CurrAddr, pvCtrlBuf, u16CtrlBufSize);
    }

    if (pvDataBuf != NULL)
    {
        u32CurrAddr += u16DataOffset;
        winc_bus_write_block(u32CurrAddr, pvDataBuf, u16DataSize);
    }

    winc_bus_write_reg(WIFI_HOST_RCV_CTRL_3, (u32DMAAddr << 2) | NBIT1);

    winc_hif_chip_sleep();

    WINC_CRIT_SEC_HIF_LEAVE;

    if (winc_bus_error())
    {
        return M2M_ERR_SEND;
    }

    return M2M_SUCCESS;
}

int8_t winc_hif_send_no_data(uint8_t u8Gid, uint8_t u8Opcode, const void *pvCtrlBuf, uint16_t u16CtrlBufSize)
{
    return winc_hif_send(u8Gid, u8Opcode, pvCtrlBuf, u16CtrlBufSize, NULL, 0, 0);
}

int8_t winc_hif_handle_isr(void)
{
    uint32_t u32Reg;
    tstrHifHdr strHif;

    if (!winc_bsp_interrupt_request_pending())
    {
        return M2M_SUCCESS;
    }

    WINC_CRIT_SEC_HIF_ENTER;

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(WIFI_HOST_RCV_CTRL_0, &u32Reg))
    {
        WINC_LOG_ERROR("Failed to read interrupt register");

        WINC_CRIT_SEC_HIF_LEAVE;
        return M2M_ERR_FAIL;
    }

    if (!(u32Reg & NBIT0))
    {
        WINC_LOG_ERROR("False interrupt %" PRIx32, u32Reg);

        WINC_CRIT_SEC_HIF_LEAVE;
        return M2M_ERR_FAIL;
    }

    /* New interrupt has been received */
    /*Clearing RX interrupt*/
    u32Reg &= (uint32_t)(~NBIT0);

    if (WINC_BUS_SUCCESS != winc_bus_write_reg(WIFI_HOST_RCV_CTRL_0, u32Reg))
    {
        WINC_CRIT_SEC_HIF_LEAVE;
        return M2M_ERR_FAIL;
    }

    gu16RxSize = (uint_fast16_t)((u32Reg >> 2) & 0xfff);

    if (!gu16RxSize)
    {
        WINC_LOG_ERROR("Wrong size");
        WINC_CRIT_SEC_HIF_LEAVE;
        return M2M_ERR_RCV;
    }

    /**
    start bus transfer
    **/
    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(WIFI_HOST_RCV_CTRL_1, &gu32RxAddr))
    {
        WINC_LOG_ERROR("WIFI_HOST_RCV_CTRL_1 bus fail");
        WINC_CRIT_SEC_HIF_LEAVE;
        return M2M_ERR_FAIL;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_block(gu32RxAddr, (uint8_t *)&strHif, sizeof(tstrHifHdr)))
    {
        WINC_LOG_ERROR("Read HIF header failed");
        WINC_CRIT_SEC_HIF_LEAVE;
        return M2M_ERR_FAIL;
    }

    WINC_CRIT_SEC_HIF_LEAVE;

    strHif.u16Length = WINC_TO_HOST_U16(strHif.u16Length);

    if ((gu16RxSize - strHif.u16Length) > 4)
    {
        WINC_LOG_ERROR("Invalid packet Size = %u <L = %u, G = %u, OP = %02X>",
                       gu16RxSize, strHif.u16Length, strHif.u8Gid, strHif.u8Opcode);

        return M2M_ERR_BUS_FAIL;
    }

    if ((strHif.u8Gid == 0) || (strHif.u8Gid > WINC_HIF_CB_HIGHEST_GID))
    {
        WINC_LOG_ERROR("Invalid group ID");

        return M2M_ERR_FAIL;
    }

    gHifCallbacks[strHif.u8Gid - 1](strHif.u8Opcode, strHif.u16Length - M2M_HIF_HDR_OFFSET, gu32RxAddr + M2M_HIF_HDR_OFFSET);

    winc_bsp_interrupt_serviced(true);

    /* Set RX Done */
    winc_bus_set_reg_bits(WIFI_HOST_RCV_CTRL_0, NBIT1);

    winc_bsp_interrupt_serviced(false);

    if (winc_bus_error())
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

int8_t winc_hif_receive(uint32_t u32Addr, const void *pvBuf, uint_fast16_t u16Sz)
{
    if (u16Sz > gu16RxSize)
    {
        WINC_LOG_ERROR("APP requested size is larger than the received buffer size <%u><%u>", u16Sz, gu16RxSize);
        return M2M_ERR_FAIL;
    }

    if ((u32Addr < gu32RxAddr) || ((u32Addr + u16Sz) > (gu32RxAddr + gu16RxSize)))
    {
        WINC_LOG_ERROR("APP requested address beyond the received buffer address and length");
        return M2M_ERR_FAIL;
    }

    /* Receive the payload */
    if (WINC_BUS_SUCCESS != winc_bus_read_block(u32Addr, (void *)pvBuf, u16Sz))
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}
