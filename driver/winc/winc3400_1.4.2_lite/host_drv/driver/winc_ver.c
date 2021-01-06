/**
 *
 * \file
 *
 * \brief This module contains WINC Version APIs implementation.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
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
#include "m2m/winc_m2m_types.h"
#include "winc_spi.h"
#include "winc_ver.h"

/*
    Special codes for managing HIF restriction to OTA rollback/switch only
*/
#define HIF_OTA_RB_ONLY             0xFFFF
#define HIFCODE_OTA_RB              ((M2M_REQ_GROUP_OTA << 8) | M2M_OTA_REQ_ROLLBACK)
#define HIFCODE_OTA_SW              ((M2M_REQ_GROUP_OTA << 8) | M2M_OTA_REQ_SWITCH_FIRMWARE)
/*
    Codes for new HIF messages (since last HIF major increase).
    Only need ones which are host->winc.
    Each entry is formed of ((GroupId << 8) | OpCode)
*/
#define HIFCODE_SSL_WRITECERT               ((M2M_REQ_GROUP_SSL << 8) | M2M_SSL_REQ_WRITE_OWN_CERTS)
#define HIFCODE_WIFI_PASSIVESCAN            ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQ_PASSIVE_SCAN)
#define HIFCODE_WIFI_CONN                   ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQ_CONN)
#define HIFCODE_WIFI_CONN_PARAM             ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_IND_CONN_PARAM)
#define HIFCODE_WIFI_DELETE_CRED            ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQRSP_DELETE_APID)
#define HIFCODE_WIFI_START_PROV_MODE        ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQ_START_PROVISION_MODE)
#define HIFCODE_WIFI_ENABLE_AP              ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQ_ENABLE_AP)
#define HIFCODE_IP_RAW_SOCK_OPT             ((M2M_REQ_GROUP_IP << 8)   | SOCKET_CMD_RAW_SET_SOCK_OPT)
#define HIFCODE_WIFI_ROAMING                ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQ_ROAMING)
#define HIFCODE_IP_SECURE                   ((M2M_REQ_GROUP_IP << 8)   | SOCKET_CMD_SECURE)
#define HIFCODE_WIFI_SCAN_SSID_LIST         ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQ_SCAN_SSID_LIST)
#define HIFCODE_WIFI_SET_STOP_SCAN_OPTION   ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQ_SET_STOP_SCAN_OPTION)

/*
    List of new HIF messages (since last HIF major increase).
    Only need to list ones which are host->winc.
    Additionally, entry 0 used to indicate OTA RB/SW only.
*/
#define NEW_HIF_LIST \
    HIF_OTA_RB_ONLY, \
    HIFCODE_SSL_WRITECERT, \
    HIFCODE_WIFI_PASSIVESCAN, \
    HIFCODE_WIFI_CONN, \
    HIFCODE_WIFI_CONN_PARAM, \
    HIFCODE_WIFI_DELETE_CRED, \
    HIFCODE_WIFI_START_PROV_MODE, \
    HIFCODE_WIFI_ENABLE_AP, \
    HIFCODE_IP_RAW_SOCK_OPT, \
    HIFCODE_WIFI_ROAMING, \
    HIFCODE_IP_SECURE, \
    HIFCODE_WIFI_SCAN_SSID_LIST, \
    HIFCODE_WIFI_SET_STOP_SCAN_OPTION
/*
    Array of HIF messages which are not supported by Firmware.
    During winc_hif_init() this array is rebased using an offset determined by Firmware HIF level.
*/
static uint16_t gau16HifBlacklist[] = {NEW_HIF_LIST};
#define HIF_BLACKLIST_SZ (sizeof(gau16HifBlacklist)/sizeof(gau16HifBlacklist[0]))
static uint8_t gu8HifBlOffset = 0;

/**
*   @fn     winc_get_hif_info(uint16_t *pu16FwHifInfo, uint16_t *pu16OtaHifInfo);
*   @brief  Get Hif info of images in both partitions (Firmware and OTA).
*   @param [out]    pu16FwHifInfo
*                   Pointer holding Hif info of image in the active partition.
*   @param [out]    pu16OtaHifInfo
*                   Pointer holding Hif info of image in the inactive partition.
*   @return ZERO in case of success and Negative error code in case of failure
*/
int_fast8_t winc_get_hif_info(uint16_t *pu16FwHifInfo, uint16_t *pu16OtaHifInfo)
{
    uint32_t u32Reg;

    if (WINC_BUS_SUCCESS != winc_bus_read_reg_with_ret(NMI_REV_REG, &u32Reg))
    {
        return M2M_ERR_FAIL;
    }

    if (pu16FwHifInfo)
    {
        *pu16FwHifInfo = (uint16_t)u32Reg;
    }

    if (pu16OtaHifInfo)
    {
        *pu16OtaHifInfo = (uint16_t)(u32Reg >> 16);
    }

    return M2M_SUCCESS;
}

/**
*   @fn         int8_t winc_hif_check_compatibility(uint16_t u16HifInfo);
*   @brief
*               To check the compatibility of an image with the current driver.
*   @param [in] u16HifInfo
*               HIF info of image to be checked.
*   @return     The function shall return ZERO for compatible image and a negative value otherwise.
*/
int8_t winc_hif_check_compatibility(uint16_t u16HifInfo)
{
    if ((M2M_GET_HIF_BLOCK(u16HifInfo) == M2M_HIF_BLOCK_VALUE) && (M2M_GET_HIF_MAJOR(u16HifInfo) == M2M_HIF_MAJOR_VALUE))
    {
        return M2M_SUCCESS;
    }

    return M2M_ERR_FW_VER_MISMATCH;
}
/**
*   @fn         int8_t winc_hif_enable_access(void);
*   @brief
*               To enable access to HIF layer, based on HIF level of Firmware.
*               This function reads HIF level directly from a register written by Firmware.
*   @return     The function shall return ZERO for full match operation and a negative value if operation is restricted.
*/
int8_t winc_hif_enable_access(void)
{
    uint16_t fw_hif_info;

    if (M2M_SUCCESS != winc_get_hif_info(&fw_hif_info, NULL))
    {
        return M2M_ERR_FAIL;
    }

    if (M2M_SUCCESS != winc_hif_check_compatibility(fw_hif_info))
    {
        gu8HifBlOffset = 0;
        WINC_LOG_ERROR("HIF access limited to OTA Switch/Rollback only");
        return M2M_ERR_FAIL;
    }

    switch (M2M_GET_HIF_MINOR(fw_hif_info))
    {
        case 0:
            gu8HifBlOffset = 1;
            break;

        case 1:
            gu8HifBlOffset = 2;
            break;

        case 2:
            gu8HifBlOffset = 2;
            break;

        case 3:
            gu8HifBlOffset = 3;
            break;

        case 4:
            gu8HifBlOffset = 10;
            break;

        case 5:
            gu8HifBlOffset = 13;
            break;

        // Additional case to be added each time hif minor increments.
        // All additional cases to be removed in the event of a hif major increment.
        // Default catches all cases in which hif minor is greater in Firmware than in Driver.
        default:
            gu8HifBlOffset = HIF_BLACKLIST_SZ;
            break;
    }

    return M2M_SUCCESS;
}
/**
*   @fn         int8_t winc_hif_check_code(uint8_t u8Gid, uint8_t u8OpCode);
*   @brief
*               To check that a particular hif message is supported with the current driver/firmware pair.
*   @param [in] u8Gid
*               Group ID.
*   @param [in] u8Opcode
*               Operation ID.
*   @return     The function shall return ZERO for support and a negative value otherwise.
*/
int8_t winc_hif_check_code(uint8_t u8Gid, uint8_t u8OpCode)
{
    uint8_t u8BlId;
    uint16_t u16HifCode = ((uint16_t)u8Gid << 8) | u8OpCode;

    if ((u16HifCode == HIFCODE_OTA_RB) || (u16HifCode == HIFCODE_OTA_SW))
    {
        return M2M_SUCCESS;
    }

    if (gu8HifBlOffset == 0)
    {
        WINC_LOG_ERROR("HIF OTA rb/sw only");
        return M2M_ERR_SEND;
    }

    for (u8BlId = gu8HifBlOffset; u8BlId < HIF_BLACKLIST_SZ; u8BlId++)
    {
        if (u16HifCode == gau16HifBlacklist[u8BlId])
        {
            WINC_LOG_ERROR("HIF message unsupported");
            return M2M_ERR_SEND;
        }
    }

    return M2M_SUCCESS;
}
