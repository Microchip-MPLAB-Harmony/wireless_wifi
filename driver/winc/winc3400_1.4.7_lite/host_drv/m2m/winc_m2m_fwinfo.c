/**
 *
 * \file
 *
 * \brief This module contains WINC firmware information APIs implementation.
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

#if defined(CONF_WINC_DEBUG_LEVEL) && (CONF_WINC_DEBUG_LEVEL >= WINC_LOG_LEVEL_ERROR)
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