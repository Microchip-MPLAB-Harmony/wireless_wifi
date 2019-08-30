/*******************************************************************************
  WINC3400 IoT OTA Interface.

  File Name:
    m2m_ota.c

  Summary:
    WINC3400 IoT OTA Interface

  Description:
    WINC3400 IoT OTA Interface
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include "nm_common.h"
#include "m2m_types.h"
#include "m2m_ota.h"
#include "m2m_wifi.h"
#include "m2m_hif.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
static tpfOtaUpdateCb gpfOtaUpdateCb = NULL;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/**
@fn         m2m_ota_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
@brief      Internal OTA call back function.
@param[in]  u8OpCode
                HIF Opcode type.
@param[in]  u16DataSize
                HIF data length.
@param[in]  u32Addr
                HIF address.
*/
static void m2m_ota_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
{
    int8_t s8Ret = M2M_SUCCESS;
    if (u8OpCode == M2M_OTA_RESP_UPDATE_STATUS)
    {
        tstrOtaUpdateStatusResp strOtaUpdateStatusResp;
        memset((uint8_t*)&strOtaUpdateStatusResp, 0, sizeof(tstrOtaUpdateStatusResp));
        s8Ret = hif_receive(u32Addr, (uint8_t*)&strOtaUpdateStatusResp, sizeof(tstrOtaUpdateStatusResp), 0);
        if(s8Ret == M2M_SUCCESS)
        {
            if(gpfOtaUpdateCb)
                gpfOtaUpdateCb(strOtaUpdateStatusResp.u8OtaUpdateStatusType, strOtaUpdateStatusResp.u8OtaUpdateStatus);
        }
    }
    else
    {
        M2M_ERR("Invalid OTA resp %d ?\r\n", u8OpCode);
    }
}

/*!
@fn         int8_t  m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb)
@brief      Initialize the OTA layer.
@param[in]  pfOtaUpdateCb
                OTA Update callback function.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb)
{
    int8_t ret = M2M_SUCCESS;

    if (pfOtaUpdateCb) {
        gpfOtaUpdateCb = pfOtaUpdateCb;
    } else {
        M2M_ERR("Invalid OTA update callback\r\n");
    }

    hif_register_cb(M2M_REQ_GROUP_OTA, m2m_ota_cb);

    return ret;
}

/*!
@fn         int8_t m2m_ota_start_update(unsigned char * pcDownloadUrl)
@brief      Request OTA start update using the downloaded URL.
@param[in]  pcDownloadUrl
                The download firmware URL, you get it from device info.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ota_start_update(uint8_t * u8DownloadUrl)
{
    int8_t ret = M2M_SUCCESS;
    uint16_t u16DurlSize = strlen((const char*)u8DownloadUrl) + 1;
    /*Todo: we may change it to data pkt but we need to give it higher priority
            but the priority is not implemented yet in data pkt
    */
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_START_UPDATE,u8DownloadUrl,u16DurlSize,NULL,0,0);
    return ret;
}

/*!
@fn         int8_t m2m_ota_rollback(void)
@brief      Request OTA Rollback image.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ota_rollback(void)
{
    int8_t ret = M2M_SUCCESS;
    tstrM2mRev strRev;

    ret = m2m_ota_get_firmware_version(&strRev);
    if(ret == M2M_SUCCESS)
    {
        if(M2M_GET_HIF_BLOCK(strRev.u16FirmwareHifInfo) == M2M_HIF_BLOCK_VALUE)
        {
            ret = hif_send(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_ROLLBACK, NULL, 0, NULL, 0, 0);
        }
        else
        {
            ret = M2M_ERR_FAIL;
        }
    }
    return ret;
}

/*!
@fn         int8_t m2m_ota_abort(void)
@brief      Request OTA Abort.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ota_abort(void)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_send(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_ABORT, NULL, 0, NULL, 0, 0);
    return ret;
}

/*!
@fn         int8_t m2m_ota_switch_firmware(void)
@brief      Switch to the upgraded Firmware.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ota_switch_firmware(void)
{
    int8_t ret = M2M_SUCCESS;
    tstrM2mRev strRev;

    ret = m2m_ota_get_firmware_version(&strRev);
    if(ret == M2M_SUCCESS)
    {
        if(M2M_GET_HIF_BLOCK(strRev.u16FirmwareHifInfo) == M2M_HIF_BLOCK_VALUE)
        {
            ret = hif_send(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_SWITCH_FIRMWARE, NULL, 0, NULL, 0, 0);
        }
        else
        {
            ret = M2M_ERR_FAIL;
        }
    }
    return ret;
}

//DOM-IGNORE-END