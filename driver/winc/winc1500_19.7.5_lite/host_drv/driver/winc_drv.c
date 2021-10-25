/**
 *
 * \file
 *
 * \brief This module contains WINC driver API implementation.
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
#include "common/winc_debug.h"
#include "common/winc_registers.h"
#include "winc_drv.h"
#include "winc_asic.h"
#include "winc_spi.h"
#include "spi_flash/winc_spi_flash.h"

int_fast8_t winc_drv_init(bool bHold)
{
    if (!winc_bsp_spi_open())
    {
        WINC_LOG_ERROR("Failed to open bus");
        return WINC_DRV_FAIL;
    }

    winc_bsp_chip_enable_deassert();
    winc_bsp_reset_assert();
    winc_bsp_sleep(CONF_WINC_RESET_ASSERT_TIME);
    winc_bsp_chip_enable_assert();
    winc_bsp_sleep(10);
    winc_bsp_reset_deassert();

    winc_bsp_sleep(10);

    if (WINC_BUS_SUCCESS != winc_bus_init())
    {
        return WINC_DRV_FAIL;
    }

    WINC_LOG_INFO("Chip ID %" PRIx32, winc_chip_get_id());

    if (bHold)
    {
        if (!winc_chip_halt())
        {
            return WINC_DRV_FAIL;
        }

        /*disable all interrupt in ROM (to disable uart) in 2b0 chip*/
        winc_bus_write_reg(0x20300, 0);
    }

    return WINC_DRV_SUCCESS;
}

int_fast8_t winc_drv_start(bool bBootATE, bool bEthMode, uint32_t u32StateRegVal)
{
    if (winc_chip_init(bBootATE, bEthMode, u32StateRegVal))
    {
        return WINC_DRV_SUCCESS;
    }

    if (!winc_bsp_spi_close())
    {
        WINC_LOG_ERROR("Failed to close bus");
    }

    winc_bus_deinit();

    return WINC_DRV_FAIL;
}

int_fast8_t winc_drv_deinit(void)
{
    if (!winc_chip_deinit())
    {
        WINC_LOG_ERROR("winc_chip_deinit failed");
        return WINC_DRV_FAIL;
    }

    /* Disable SPI flash to save power when the chip is off */
    if (M2M_SUCCESS != spi_flash_enable(0))
    {
        WINC_LOG_ERROR("SPI flash disable failed");
        return WINC_DRV_FAIL;
    }

    if (!winc_bsp_spi_close())
    {
        WINC_LOG_ERROR("Failed to close bus");
        return WINC_DRV_FAIL;
    }

    winc_bus_deinit();

    return WINC_DRV_SUCCESS;
}
