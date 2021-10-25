/**
 *
 * \file
 *
 * \brief This module contains WINC ASIC specific internal API implementation.
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
#include "winc_asic.h"

#define M2M_FINISH_INIT_STATE       0x02532636UL
#define M2M_FINISH_BOOT_ROM         0x10add09eUL
#define M2M_START_FIRMWARE          0xef522f61UL

#define TIMEOUT                     (2000)
#define WAKUP_TRAILS_TIMEOUT        (4)

#define rHAVE_SDIO_IRQ_GPIO_BIT     (NBIT0)
#define rHAVE_USE_PMU_BIT           (NBIT1)
#define rHAVE_SLEEP_CLK_SRC_RTC_BIT (NBIT2)
#define rHAVE_SLEEP_CLK_SRC_XO_BIT  (NBIT3)
#define rHAVE_EXT_PA_INV_TX_RX      (NBIT4)
#define rHAVE_LEGACY_RF_SETTINGS    (NBIT5)
#define rHAVE_LOGS_DISABLED_BIT     (NBIT6)
#define rHAVE_ETHERNET_MODE_BIT     (NBIT7)
#define rHAVE_RESERVED1_BIT         (NBIT8)
#define rHAVE_RESERVED2_BIT         (NBIT9)
#define rHAVE_XO_XTALGM2_DIS_BIT    (NBIT10)

static bool chip_apply_conf(const uint32_t u32Conf)
{
    uint32_t u32Reg;

    do
    {
        winc_bus_write_reg(NMI_GP_REG_1, u32Conf);

        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_GP_REG_1, &u32Reg))
        {
            return false;
        }

        if (u32Reg == u32Conf)
        {
            return true;
        }
    }
    while (1);

    return false;
}

bool winc_chip_interrupts_enable(void)
{
    // interrupt pin mux select
    if (WINC_BUS_SUCCESS != winc_bus_set_reg_bits(NMI_PIN_MUX_0, (uint32_t)1 << 8))
    {
        return false;
    }

    // interrupt enable
    if (WINC_BUS_SUCCESS != winc_bus_set_reg_bits(NMI_INTR_ENABLE, (uint32_t)1 << 16))
    {
        return false;
    }

    return true;
}

uint32_t winc_chip_get_id(void)
{
    static uint32_t gu32ChipID = 0;
    uint32_t u32RevID;

    if (gu32ChipID)
    {
        return gu32ChipID;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_RF_REVID, &u32RevID))
    {
        return 0;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_CHIPID, &gu32ChipID))
    {
        return 0;
    }

    if (gu32ChipID == 0x1002a0)
    {
        if (u32RevID == 0x1)
        {
            /* 1002A0 */
        }
        else /* if (u32RevID == 0x2) */
        {
            /* 1002A1 */
            gu32ChipID = 0x1002a1;
        }
    }
    else if (gu32ChipID == 0x1002b0)
    {
        if (u32RevID == 3)
        {
            /* 1002B0 */
        }
        else if (u32RevID == 4)
        {
            /* 1002B1 */
            gu32ChipID = 0x1002b1;
        }
        else /* if (u32RevID == 5) */
        {
            /* 1002B2 */
            gu32ChipID = 0x1002b2;
        }
    }
    else if (gu32ChipID == 0x1000f0)
    {
#ifdef BT_CHIP_ID_REG

        if ((WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(BT_CHIP_ID_REG, &gu32ChipID)))
        {
            gu32ChipID = 0;
            return 0;
        }

#endif
    }

    gu32ChipID &= (uint32_t)(~(0x0f0000));
    gu32ChipID |= 0x050000;

    return gu32ChipID;
}

bool winc_chip_sleep(void)
{
    uint32_t u32Reg;

    if (winc_bus_error())
    {
        return false;
    }

    do
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(CORT_HOST_COMM, &u32Reg))
        {
            return false;
        }
    }
    while (u32Reg & NBIT0);

    /* Clear bit 1 */
    winc_bus_clear_reg_bits(WAKE_CLK_REG, NBIT1);
    winc_bus_clear_reg_bits(HOST_CORT_COMM, NBIT0);

    return !winc_bus_error();
}

bool winc_chip_wake(void)
{
    uint32_t u32ClkStatusReg;
    int_fast8_t i8Trials;

    /* Use bit 0 to indicate host wakeup*/
    winc_bus_set_reg_bits(HOST_CORT_COMM, NBIT0);
    winc_bus_set_reg_bits(WAKE_CLK_REG, NBIT1);

    if (winc_bus_error())
    {
        return false;
    }

    i8Trials = WAKUP_TRAILS_TIMEOUT;

    while (i8Trials--)
    {
        if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(CLOCKS_EN_REG, &u32ClkStatusReg))
        {
            WINC_LOG_ERROR("Reading clock status register");
            return false;
        }

        if (u32ClkStatusReg & NBIT2)
        {
            winc_bus_reset();

            return true;
        }

        winc_bsp_sleep(2);
    }

    WINC_LOG_ERROR("Failed to wake up the chip");

    winc_bus_reset();

    return false;
}

bool winc_chip_halt(void)
{
    if (WINC_BUS_SUCCESS != winc_bus_set_reg_bits(NMI_CORTUS_BOOT_RESET_MUXSEL, (uint32_t)1 << 0))
    {
        return false;
    }

    if (WINC_BUS_SUCCESS != winc_bus_clear_reg_bits(NMI_GLB_RESET_0, (uint32_t)1 << 10))
    {
        return false;
    }

    return true;
}

void winc_chip_reset(void)
{
    winc_bus_write_reg_no_rsp(NMI_GLB_RESET_0, 0);
    /* Don't check return, write will fail as WINC will not respond for 50ms */

    winc_bsp_sleep(50);
}

bool winc_chip_init(bool bBootATE, bool bEthMode, uint32_t u32StateRegVal)
{
    uint32_t u32GpReg1;
    uint_fast16_t u16Timeout;

    winc_bus_write_reg(BOOTROM_REG, 0);
    winc_bus_write_reg(NMI_STATE_REG, 0);
    winc_bus_write_reg(NMI_REV_REG, 0);

    if (WINC_BUS_SUCCESS != winc_bus_set_reg_bits(NMI_CORTUS_BOOT_RESET_MUXSEL, (uint32_t)1 << 0))
    {
        return false;
    }

    /* release cortus CPU from reset */
    if (WINC_BUS_SUCCESS != winc_bus_set_reg_bits(NMI_GLB_RESET_0, (uint32_t)1 << 10))
    {
        return false;
    }

    while (1)
    {
        /* wait for efuse loading done */
        if (winc_bus_read_reg(NMI_EFUSE_0_CONTROL) & 0x80000000)
        {
            break;
        }
    }

    /* check if waiting for the host will be skipped or not */
    if (!(winc_bus_read_reg(M2M_WAIT_FOR_HOST_REG) & 1))
    {
        u16Timeout = TIMEOUT;

        while (u16Timeout--)
        {
            winc_bsp_sleep(1);

            if (M2M_FINISH_BOOT_ROM == winc_bus_read_reg(BOOTROM_REG))
            {
                break;
            }
        }

        if (!u16Timeout)
        {
            WINC_LOG_DEBUG("failed to load firmware from flash");
            return false;
        }
    }

    u32GpReg1 = 0
#if (defined __ENABLE_PMU__) || (defined CONF_WINC_INT_PMU)
                | rHAVE_USE_PMU_BIT
#endif
#ifdef __ENABLE_SLEEP_CLK_SRC_RTC__
                | rHAVE_SLEEP_CLK_SRC_RTC_BIT
#elif defined __ENABLE_SLEEP_CLK_SRC_XO__
                | rHAVE_SLEEP_CLK_SRC_XO_BIT
#endif
#ifdef __ENABLE_EXT_PA_INV_TX_RX__
                | rHAVE_EXT_PA_INV_TX_RX
#endif
#ifdef __ENABLE_LEGACY_RF_SETTINGS__
                | rHAVE_LEGACY_RF_SETTINGS
#endif
#ifdef __DISABLE_FIRMWARE_LOGS__
                | rHAVE_LOGS_DISABLED_BIT
#endif
#if defined CONF_WINC_XO_XTALGM2_DIS
                | rHAVE_XO_XTALGM2_DIS_BIT
#endif
                | rHAVE_RESERVED1_BIT;

    if (bBootATE)
    {
        winc_bus_write_reg(NMI_REV_REG, M2M_ATE_FW_START_VALUE);
    }
    else if (bEthMode)
    {
        u32GpReg1 |= rHAVE_ETHERNET_MODE_BIT;
    }

    winc_bus_write_reg(NMI_STATE_REG, u32StateRegVal);

    if (REV(winc_chip_get_id()) >= REV_3A0)
    {
        u32GpReg1 |= rHAVE_USE_PMU_BIT;
    }

    chip_apply_conf(u32GpReg1);

    winc_bus_write_reg(BOOTROM_REG, M2M_START_FIRMWARE);

    u16Timeout = TIMEOUT;

    while (u16Timeout--)
    {
        winc_bsp_sleep(2);

        if (!bBootATE)
        {
            if (M2M_FINISH_INIT_STATE == winc_bus_read_reg(NMI_STATE_REG))
            {
                winc_bus_write_reg(NMI_STATE_REG, 0);

                if (!winc_chip_interrupts_enable())
                {
                    WINC_LOG_ERROR("failed to enable interrupts..");
                    return false;
                }

                return true;
            }
        }
        else
        {
            if (M2M_ATE_FW_IS_UP_VALUE == winc_bus_read_reg(NMI_REV_REG))
            {
                return true;
            }
        }
    }

    WINC_LOG_DEBUG("Time out for wait firmware Run");

    return false;
}

bool winc_chip_deinit(void)
{
    if (WINC_BUS_SUCCESS != winc_bus_clear_reg_bits(NMI_GLB_RESET_0, (uint32_t)1 << 10))
    {
        WINC_LOG_ERROR("Failed to de-initialize");
        return false;
    }

    return true;
}

#ifdef CONF_PERIPH

bool winc_chip_set_gpio_dir(uint_fast8_t u8GPIO, bool bDirOut)
{
    int_fast8_t i8Ret;

    WINC_ASSERT(u8GPIO < 32);

    if (bDirOut)
    {
        i8Ret = winc_bus_set_reg_bits(0x20108, 1UL << u8GPIO);
    }
    else
    {
        i8Ret = winc_bus_clear_reg_bits(0x20108, 1UL << u8GPIO);
    }

    if (WINC_BUS_SUCCESS != i8Ret)
    {
        return false;
    }

    return true;
}

bool winc_chip_set_gpio_val(uint_fast8_t u8GPIO, bool bBitSet)
{
    int_fast8_t i8Ret;

    WINC_ASSERT(u8GPIO < 32);

    if (bBitSet)
    {
        i8Ret = winc_bus_set_reg_bits(0x20100, 1UL << u8GPIO);
    }
    else
    {
        i8Ret = winc_bus_clear_reg_bits(0x20100, 1UL << u8GPIO);
    }

    if (WINC_BUS_SUCCESS != i8Ret)
    {
        return false;
    }

    return true;
}

bool winc_chip_get_gpio_val(uint_fast8_t u8GPIO, uint8_t *pu8Val)
{
    uint32_t u32Val;

    WINC_ASSERT(u8GPIO < 32);
    WINC_ASSERT(pu8Val != NULL);

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(0x20104, &u32Val))
    {
        return false;
    }

    *pu8Val = (uint8_t)((u32Val >> u8GPIO) & 1);

    return true;
}

bool winc_chip_pullup_ctrl(uint32_t u32PinMask, bool bEnable)
{
    int_fast8_t i8Ret;

    if (bEnable)
    {
        i8Ret = winc_bus_clear_reg_bits(0x142c, u32PinMask);
    }
    else
    {
        i8Ret = winc_bus_set_reg_bits(0x142c, u32PinMask);
    }

    if (WINC_BUS_SUCCESS != i8Ret)
    {
        return false;
    }

    return true;
}

#endif /* CONF_PERIPH */

bool winc_chip_get_otp_mac_address(uint8_t *pu8MacAddr)
{
    uint32_t    u32RegValue;
    tstrGpRegs  strGpRegs;

    WINC_ASSERT(pu8MacAddr != NULL);

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_GP_REG_2, &u32RegValue))
    {
        return false;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_block(u32RegValue | 0x30000, (uint8_t *)&strGpRegs, sizeof(tstrGpRegs)))
    {
        return false;
    }

    if ((strGpRegs.u32Mac_efuse_mib & 0xffff0000) == 0)
    {
        WINC_LOG_DEBUG("Default MAC");
        memset(pu8MacAddr, 0, 6);

        return false;
    }

    WINC_LOG_DEBUG("OTP MAC");
    strGpRegs.u32Mac_efuse_mib >>= 16;

    if (WINC_BUS_SUCCESS != winc_bus_read_block(strGpRegs.u32Mac_efuse_mib | 0x30000, pu8MacAddr, 6))
    {
        return false;
    }

    return true;
}

bool winc_chip_get_mac_address(uint8_t *pu8MacAddr)
{
    uint32_t    u32RegValue;
    tstrGpRegs  strGpRegs;

    WINC_ASSERT(pu8MacAddr != NULL);

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_GP_REG_2, &u32RegValue))
    {
        return false;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_block(u32RegValue | 0x30000, (uint8_t *)&strGpRegs, sizeof(tstrGpRegs)))
    {
        return false;
    }

    strGpRegs.u32Mac_efuse_mib &= 0x0000ffff;

    if (WINC_BUS_SUCCESS != winc_bus_read_block(strGpRegs.u32Mac_efuse_mib | 0x30000, pu8MacAddr, 6))
    {
        return false;
    }

    return true;
}
