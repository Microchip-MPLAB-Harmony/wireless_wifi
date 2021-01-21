/**
 *
 * \file
 *
 * \brief WINC IoT OTA Interface.
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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "bsp/winc_bsp.h"
#include "common/winc_defines.h"
#include "common/winc_debug.h"
#include "winc_m2m_types.h"
#include "winc_m2m_wifi.h"
#include "winc_m2m_ota.h"
#include "winc_m2m_fwinfo.h"
#include "driver/winc_hif.h"
#include "spi_flash/winc_spi_flash.h"
#include "spi_flash/winc_spi_flash_map.h"
#include "spi_flash/winc_flexible_flash.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
static tpfOtaUpdateCb gpfOtaUpdateCb = NULL;
static tpfOtaNotifCb  gpfOtaNotifCb  = NULL;
static tpfFileGetCb   gpfHFDGetCb    = NULL;
static tpfFileReadCb  gpfHFDReadCb   = NULL;
static tpfFileEraseCb gpfHFDEraseCb  = NULL;

typedef struct
{
    uint32_t u32Offset;
    uint32_t u32Size;
} tstrFileBlockDescriptor;

static uint8_t gu8CurrFileHandlerID  = HFD_INVALID_HANDLER;

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
    else if (u8OpCode == M2M_OTA_RESP_HOST_FILE_STATUS)
    {
        tstrOtaHostFileGetStatusResp strOtaHostFileGetStatusResp = {0};
        s8Ret = winc_hif_receive(u32Addr, &strOtaHostFileGetStatusResp, sizeof(tstrOtaHostFileGetStatusResp));

        if (M2M_SUCCESS == s8Ret)
        {
            if (strOtaHostFileGetStatusResp.u8OtaFileGetStatus == OTA_STATUS_SUCCESS)
            {
                gu8CurrFileHandlerID = strOtaHostFileGetStatusResp.u8CFHandler;
            }

            if(gpfHFDGetCb)
            {
                gpfHFDGetCb(strOtaHostFileGetStatusResp.u8OtaFileGetStatus, gu8CurrFileHandlerID, strOtaHostFileGetStatusResp.u32OtaFileSize);
                gpfHFDGetCb = NULL;
            }
        }
    }
    else if (u8OpCode == M2M_OTA_RESP_HOST_FILE_DOWNLOAD)
    {
        tstrOtaHostFileGetStatusResp strOtaHostFileGetStatusResp = {0};
        s8Ret = winc_hif_receive(u32Addr, &strOtaHostFileGetStatusResp, sizeof(tstrOtaHostFileGetStatusResp));

        if (M2M_SUCCESS == s8Ret)
        {
            if (strOtaHostFileGetStatusResp.u8OtaFileGetStatus == OTA_STATUS_SUCCESS)
            {
                gu8CurrFileHandlerID = strOtaHostFileGetStatusResp.u8CFHandler;
                WINC_LOG_INFO("Generated HostFileHandlerID is %u", gu8CurrFileHandlerID);
            }

            if (gpfHFDGetCb)
            {
                gpfHFDGetCb(strOtaHostFileGetStatusResp.u8OtaFileGetStatus, gu8CurrFileHandlerID, strOtaHostFileGetStatusResp.u32OtaFileSize);
                gpfHFDGetCb = NULL;
            }
        }
    }
    else if (u8OpCode == M2M_OTA_RESP_HOST_FILE_READ)
    {
        tstrOtaHostFileReadStatusResp strOtaHostFileReadStatusResp;
        memset(&strOtaHostFileReadStatusResp, 0, sizeof(tstrOtaHostFileReadStatusResp));
        s8Ret = winc_hif_receive(u32Addr, &strOtaHostFileReadStatusResp, sizeof(tstrOtaHostFileReadStatusResp));

        if (M2M_SUCCESS == s8Ret)
            if (gpfHFDReadCb)
            {
                gpfHFDReadCb(strOtaHostFileReadStatusResp.u8OtaFileReadStatus, strOtaHostFileReadStatusResp.pFileBuf, strOtaHostFileReadStatusResp.u16FileBlockSz);
            }
    }
    else if (u8OpCode == M2M_OTA_RESP_HOST_FILE_ERASE)
    {
        tstrOtaHostFileEraseStatusResp strOtaHostFileEraseStatusResp = {0};
        s8Ret = winc_hif_receive(u32Addr, &strOtaHostFileEraseStatusResp, sizeof(tstrOtaHostFileEraseStatusResp));

        if (M2M_SUCCESS == s8Ret)
        {
            if (gpfHFDEraseCb)
            {
                gpfHFDEraseCb(strOtaHostFileEraseStatusResp.u8OtaFileEraseStatus);
                gpfHFDEraseCb = NULL;
            }
        }
    }
    else
    {
        WINC_LOG_ERROR("Invalid OTA resp %u", u8OpCode);
    }
}

int8_t m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb, tpfOtaNotifCb pfOtaNotifCb, tpfFileGetCb pfHFDGetCb)
{
    gpfOtaUpdateCb = pfOtaUpdateCb;
    gpfOtaNotifCb  = pfOtaNotifCb;
    gpfHFDGetCb    = pfHFDGetCb;

    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_HOST_FILE_STATUS, NULL, 0);
}

uint8_t m2m_ota_host_file_get_id(void)
{
    return gu8CurrFileHandlerID;
}

int8_t m2m_ota_notif_set_url(uint8_t *u8Url)
{
    uint16_t u16UrlSize = (uint16_t)strlen((const char *)u8Url) + 1;
    /*Todo: we may change it to data pkt but we need to give it higher priority
            but the priority is not implemented yet in data pkt
    */
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
    /*Todo: we may change it to data pkt but we need to give it higher priority
            but the priority is not implemented yet in data pkt
    */
    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_START_FW_UPDATE, pcDownloadUrl, u16DurlSize);
}

int8_t m2m_ota_rollback(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_ROLLBACK_FW, NULL, 0);
}

int8_t m2m_ota_abort(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_ABORT, NULL, 0);
}

int8_t m2m_ota_switch_firmware(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_SWITCH_FIRMWARE, NULL, 0);
}

int8_t m2m_ota_get_firmware_version(tstrM2mRev *pstrRev)
{
    int8_t ret;

    ret = winc_hif_chip_wake();

    if (ret != M2M_SUCCESS)
    {
        return ret;
    }

    ret = m2m_fwinfo_get_firmware_info(false, pstrRev);
    winc_hif_chip_sleep();

    if (ret != M2M_SUCCESS)
    {
        return ret;
    }

    return m2m_fwinfo_version_check(pstrRev);
}

int8_t m2m_ota_host_file_get(char *pcDownloadUrl, tpfFileGetCb pfHFDGetCb)
{
    uint16_t u16DUrlSize = (uint16_t)strlen(pcDownloadUrl);

    if ((NULL == pfHFDGetCb) || (0 == u16DUrlSize))
    {
        WINC_LOG_ERROR("Invalid parameters.");
        return M2M_ERR_INVALID_ARG;
    }

    if ('\0' != pcDownloadUrl[u16DUrlSize])
    {
        pcDownloadUrl[u16DUrlSize] = '\0';
    }
    else
    {
        u16DUrlSize++;
    }

    WINC_LOG_INFO("GetHostFile - URL: %s, urlSize: %u", pcDownloadUrl, u16DUrlSize);

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_HOST_FILE_DOWNLOAD, pcDownloadUrl, u16DUrlSize))
    {
        return M2M_ERR_FAIL;
    }

    gpfHFDGetCb = pfHFDGetCb;
    gu8CurrFileHandlerID = HFD_INVALID_HANDLER;

    return M2M_SUCCESS;
}

int8_t m2m_ota_host_file_read_hif(uint8_t u8Handler, uint32_t u32Offset, uint32_t u32Size, tpfFileReadCb pfHFDReadCb)
{
    tstrFileBlockDescriptor strFileBlock;

    strFileBlock.u32Offset = u32Offset;
    strFileBlock.u32Size   = u32Size;

    if ((u8Handler != gu8CurrFileHandlerID) || (HFD_INVALID_HANDLER == gu8CurrFileHandlerID) || (NULL == pfHFDReadCb))
    {
        return M2M_ERR_INVALID_ARG;
    }

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_HOST_FILE_READ, &strFileBlock, sizeof(tstrFileBlockDescriptor)))
    {
        return M2M_ERR_FAIL;
    }

    gpfHFDReadCb = pfHFDReadCb;

    return M2M_SUCCESS;
}

int8_t m2m_ota_host_file_read_spi(uint8_t u8Handler, uint8_t *pu8Buff, uint32_t u32Offset, uint32_t u32Size)
{
    static uint32_t u32FlashHFDStart = 0;
    static uint32_t u32FlashHFDSize  = 0;

    if ((u8Handler == HFD_INVALID_HANDLER) || (NULL == pu8Buff))
    {
        return M2M_ERR_INVALID_ARG;
    }

    if (WIFI_STATE_INIT != m2m_wifi_get_state())
    {
        WINC_LOG_ERROR("WINC is not in an appropriate state for this operation!");
        return M2M_ERR_FAIL;
    }

    if ((u32FlashHFDStart == 0) || (u32FlashHFDSize == 0))
    {
        if (M2M_SUCCESS != spi_flexible_flash_find_section(ENTRY_ID_HOSTFILE, &u32FlashHFDStart, &u32FlashHFDSize))
        {
            return M2M_ERR_FAIL;
        }
    }

    if (M2M_SUCCESS != spi_flash_read(pu8Buff, u32FlashHFDStart, 4))
    {
        return M2M_ERR_FAIL;
    }

    if (pu8Buff[0] != u8Handler)
    {
        return M2M_ERR_FAIL;
    }

    if ((u32Offset >= u32FlashHFDSize) ||
            (u32Size > u32FlashHFDSize) ||
            ((u32Offset + u32Size) >= u32FlashHFDSize))
    {
        return M2M_ERR_FAIL;
    }

    if (M2M_SUCCESS != spi_flash_read(pu8Buff, u32FlashHFDStart + FLASH_SECTOR_SZ + u32Offset, u32Size))
    {
        WINC_LOG_ERROR("Unable to read SPI Flash");
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

int8_t m2m_ota_host_file_erase(uint8_t u8Handler, tpfFileEraseCb pfHFDEraseCb)
{
    if ((u8Handler != gu8CurrFileHandlerID) || (HFD_INVALID_HANDLER == gu8CurrFileHandlerID))
    {
        return M2M_ERR_INVALID_ARG;
    }

    gu8CurrFileHandlerID = HFD_INVALID_HANDLER;
    gpfHFDReadCb  = NULL;
    gpfHFDEraseCb = pfHFDEraseCb;

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_HOST_FILE_ERASE, NULL, 0))
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}