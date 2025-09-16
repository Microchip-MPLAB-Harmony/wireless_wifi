/**
 *
 * \file
 *
 * \brief This module contains WINC driver API implementation.
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
