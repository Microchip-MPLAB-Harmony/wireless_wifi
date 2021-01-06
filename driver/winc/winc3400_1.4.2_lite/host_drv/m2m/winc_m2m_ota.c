/**
 *
 * \file
 *
 * \brief WINC IoT OTA Interface.
 *
 * Copyright (c) 2020 Microchip Technology Inc. and its subsidiaries.
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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "bsp/winc_bsp.h"
#include "common/winc_defines.h"
#include "common/winc_debug.h"
#include "winc_m2m_types.h"
#include "winc_m2m_wifi.h"
#include "winc_m2m_ota.h"
#include "driver/winc_drv.h"
#include "driver/winc_hif.h"
#include "spi_flash/winc_spi_flash.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
static tpfOtaUpdateCb gpfOtaUpdateCb = NULL;
static tpfOtaNotifCb  gpfOtaNotifCb  = NULL;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
void m2m_ota_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
{
    UNUSED_VAR(u16DataSize);

    int8_t s8Ret = M2M_SUCCESS;

    if (u8OpCode == M2M_OTA_RESP_NOTIF_UPDATE_INFO)
    {
        tstrOtaUpdateInfo strOtaUpdateInfo;
        memset(&strOtaUpdateInfo, 0, sizeof(tstrOtaUpdateInfo));
        s8Ret = winc_hif_receive(u32Addr, &strOtaUpdateInfo, sizeof(tstrOtaUpdateInfo));

        if (s8Ret == M2M_SUCCESS)
        {
            if (gpfOtaNotifCb)
            {
                gpfOtaNotifCb(&strOtaUpdateInfo);
            }
        }
    }
    else if (u8OpCode == M2M_OTA_RESP_UPDATE_STATUS)
    {
        tstrOtaUpdateStatusResp strOtaUpdateStatusResp;
        memset(&strOtaUpdateStatusResp, 0, sizeof(tstrOtaUpdateStatusResp));
        s8Ret = winc_hif_receive(u32Addr, &strOtaUpdateStatusResp, sizeof(tstrOtaUpdateStatusResp));

        if (s8Ret == M2M_SUCCESS)
        {
            if (gpfOtaUpdateCb)
            {
                gpfOtaUpdateCb(strOtaUpdateStatusResp.u8OtaUpdateStatusType, strOtaUpdateStatusResp.u8OtaUpdateStatus);
            }
        }
    }
    else
    {
        WINC_LOG_ERROR("Invalid OTA resp %u", u8OpCode);
    }
}

int8_t m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb, tpfOtaNotifCb pfOtaNotifCb)
{
    gpfOtaUpdateCb = pfOtaUpdateCb;
    gpfOtaNotifCb  = pfOtaNotifCb;

    return M2M_SUCCESS;
}

int8_t m2m_ota_notif_set_url(uint8_t *u8Url)
{
    uint16_t u16UrlSize = (uint16_t)strlen((const char *)u8Url) + 1;
    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_NOTIF_SET_URL, u8Url, u16UrlSize);
}

int8_t m2m_ota_notif_check_for_update(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_NOTIF_CHECK_FOR_UPDATE, NULL, 0);
}

int8_t m2m_ota_notif_sched(uint32_t u32Period)
{
    UNUSED_VAR(u32Period);

    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_NOTIF_CHECK_FOR_UPDATE, NULL, 0);
}

int8_t m2m_ota_start_update(unsigned char *pcDownloadUrl)
{
    uint16_t u16DurlSize = (uint16_t)strlen((const char *)pcDownloadUrl) + 1;
    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_START_UPDATE, pcDownloadUrl, u16DurlSize);
}

int8_t m2m_ota_rollback(void)
{
    tstrM2mRev strRev;

    if (M2M_SUCCESS != m2m_ota_get_firmware_version(&strRev))
    {
        return M2M_ERR_FAIL;
    }

    if (M2M_GET_HIF_BLOCK(strRev.u16FirmwareHifInfo) != M2M_HIF_BLOCK_VALUE)
    {
        return M2M_ERR_FAIL;
    }

    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_ROLLBACK, NULL, 0);
}

int8_t m2m_ota_abort(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_ABORT, NULL, 0);
}

int8_t m2m_ota_switch_firmware(void)
{
    tstrM2mRev strRev;

    if (M2M_SUCCESS != m2m_ota_get_firmware_version(&strRev))
    {
        return M2M_ERR_FAIL;
    }

    if (M2M_GET_HIF_BLOCK(strRev.u16FirmwareHifInfo) != M2M_HIF_BLOCK_VALUE)
    {
        return M2M_ERR_FAIL;
    }

    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_SWITCH_FIRMWARE, NULL, 0);
}
