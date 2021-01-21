/**
 *
 * \file
 *
 * \brief This module contains WINC firmware information APIs implementation.
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
#include "winc_m2m_types.h"
#include "winc_m2m_fwinfo.h"
#include "driver/winc_asic.h"
#include "driver/winc_spi.h"
#include "spi_flash/winc_spi_flash.h"

static uint32_t winc_firmware_version_read_addr(void)
{
    uint32_t   u32Reg;
    tstrGpRegs strgp;

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_GP_REG_2, &u32Reg))
    {
        return 0;
    }

    if (!u32Reg)
    {
        return 0;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_block(u32Reg | 0x30000, (uint8_t *)&strgp, sizeof(tstrGpRegs)))
    {
        return 0;
    }

    return strgp.u32Firmware_Ota_rev;
}

int_fast8_t m2m_fwinfo_version_check(tstrM2mRev *pstrRev)
{
    uint16_t curr_drv_ver, min_req_drv_ver, curr_firm_ver;

    if (NULL == pstrRev)
    {
        return M2M_ERR_INVALID_ARG;
    }

    curr_firm_ver   = M2M_MAKE_VERSION(pstrRev->u8FirmwareMajor, pstrRev->u8FirmwareMinor, pstrRev->u8FirmwarePatch);
    curr_drv_ver    = M2M_MAKE_VERSION(M2M_RELEASE_VERSION_MAJOR_NO, M2M_RELEASE_VERSION_MINOR_NO, M2M_RELEASE_VERSION_PATCH_NO);
    min_req_drv_ver = M2M_MAKE_VERSION(pstrRev->u8DriverMajor, pstrRev->u8DriverMinor, pstrRev->u8DriverPatch);

    if ((curr_firm_ver == 0) || (min_req_drv_ver == 0))
    {
        return M2M_ERR_INVALID_ARG;
    }

    if (curr_drv_ver < min_req_drv_ver)
    {
        /*The current driver version should be larger or equal
        than the min driver that the current firmware support  */
        return M2M_ERR_FW_VER_MISMATCH;
    }

    if (curr_drv_ver > curr_firm_ver)
    {
        /*The current driver should be equal or less than the firmware version*/
        return M2M_ERR_FW_VER_MISMATCH;
    }

    return M2M_SUCCESS;
}

int_fast8_t m2m_fwinfo_get_firmware_info(bool bMainImage, tstrM2mRev *pstrRev)
{
    uint32_t   u32Reg;

    if (NULL == pstrRev)
    {
        return M2M_ERR_INVALID_ARG;
    }

    memset(pstrRev, 0, sizeof(tstrM2mRev));

    u32Reg = winc_firmware_version_read_addr() & 0xffff;

    if (!u32Reg)
    {
        return M2M_ERR_FAIL;
    }

    if (bMainImage)
    {
        u32Reg &= 0xffff;
    }
    else
    {
        u32Reg >>= 16;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_block(u32Reg | 0x30000, (uint8_t *)pstrRev, sizeof(tstrM2mRev)))
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}
