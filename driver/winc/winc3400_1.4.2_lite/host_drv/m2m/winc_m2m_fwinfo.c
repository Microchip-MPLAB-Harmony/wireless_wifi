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
#include "driver/winc_ver.h"

static uint32_t winc_firmware_version_read_addr(void)
{
    uint32_t   u32Reg;
    tstrGpRegs strgp;

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_GP_REG_0, &u32Reg))
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

#if defined(CONF_WINC_DEBUG_LEVEL) && (CONF_WINC_DEBUG_LEVEL >= WINC_LOG_LEVEL_INFO)
const char *FWName = "Firmware";
const char *OTAName = "OTA";
#define HIF_TYPE_NAMES_DEFINED
#endif

int_fast8_t m2m_fwinfo_get_firmware_info(bool bMainImage, tstrM2mRev *pstrRev)
{
    uint16_t   u16HifInfo;
    uint32_t   u32Reg;
#ifdef HIF_TYPE_NAMES_DEFINED
    const char *pName;
#endif

    if (NULL == pstrRev)
    {
        return M2M_ERR_INVALID_ARG;
    }

    memset(pstrRev, 0, sizeof(tstrM2mRev));

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_REV_REG, &u32Reg))
    {
        return M2M_ERR_FAIL;
    }

    u16HifInfo = bMainImage ? u32Reg & 0xffff : u32Reg >> 16;

    WINC_LOG_INFO("HIF: %04x", u16HifInfo);

    if (M2M_GET_HIF_BLOCK(u16HifInfo) != M2M_HIF_BLOCK_VALUE)
    {
        return M2M_ERR_FAIL;
    }

    if (bMainImage)
    {
        u32Reg = winc_firmware_version_read_addr() & 0xffff;
#ifdef HIF_TYPE_NAMES_DEFINED
        pName = FWName;
#endif
    }
    else
    {
        u32Reg = winc_firmware_version_read_addr() >> 16;
#ifdef HIF_TYPE_NAMES_DEFINED
        pName = OTAName;
#endif
    }

    if (!u32Reg)
    {
        return M2M_ERR_FAIL;
    }

    if (WINC_BUS_SUCCESS != winc_bus_read_block(u32Reg | 0x30000, (uint8_t *)pstrRev, sizeof(tstrM2mRev)))
    {
        return M2M_ERR_FAIL;
    }

    WINC_LOG_INFO("%s HIF (%u) : %u.%u", pName, M2M_GET_HIF_BLOCK(pstrRev->u16FirmwareHifInfo), M2M_GET_HIF_MAJOR(pstrRev->u16FirmwareHifInfo), M2M_GET_HIF_MINOR(pstrRev->u16FirmwareHifInfo));
    WINC_LOG_INFO("%s ver   : %u.%u.%u", pName, pstrRev->u8FirmwareMajor, pstrRev->u8FirmwareMinor, pstrRev->u8FirmwarePatch);
    WINC_LOG_INFO("%s Build %s Time %s", pName, pstrRev->BuildDate, pstrRev->BuildTime);

    /* Check Hif info is consistent */
    if (u16HifInfo != pstrRev->u16FirmwareHifInfo)
    {
        WINC_LOG_ERROR("Inconsistent %s Version", pName);
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}