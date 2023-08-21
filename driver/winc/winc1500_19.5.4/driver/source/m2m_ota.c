/*******************************************************************************
  WINC1500 IoT OTA Interface

  File Name:
    m2m_ota.c

  Summary:
    WINC1500 IoT OTA Interface

  Description:
    WINC1500 IoT OTA Interface
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2019, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include "nm_common.h"
#include "m2m_types.h"
#include "m2m_ota.h"
#include "m2m_hif.h"
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
static tpfOtaUpdateCb gpfOtaUpdateCb = NULL;
static tpfOtaNotifCb  gpfOtaNotifCb = NULL;


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/**
*   @fn         m2m_wifi_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr, uint8_t grp)
*   @brief      WiFi call back function
*   @param [in] u8OpCode
*                   HIF Opcode type.
*   @param [in] u16DataSize
*                   HIF data length.
*   @param [in] u32Addr
*                   HIF address.
*   @param [in] grp
*                   HIF group type.
*   @author
*   @date
*   @version    1.0
*/
static void m2m_ota_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
{
    int8_t ret = M2M_SUCCESS;
    if(u8OpCode == M2M_OTA_RESP_NOTIF_UPDATE_INFO)
    {
        tstrOtaUpdateInfo strOtaUpdateInfo;
        memset((uint8_t*)&strOtaUpdateInfo,0,sizeof(tstrOtaUpdateInfo));
        ret = hif_receive(u32Addr,(uint8_t*)&strOtaUpdateInfo,sizeof(tstrOtaUpdateInfo),0);
        if(ret == M2M_SUCCESS)
        {
            if(gpfOtaNotifCb)
                gpfOtaNotifCb(&strOtaUpdateInfo);
        }
    }
    else if (u8OpCode == M2M_OTA_RESP_UPDATE_STATUS)
    {
        tstrOtaUpdateStatusResp strOtaUpdateStatusResp;
        memset((uint8_t*)&strOtaUpdateStatusResp,0,sizeof(tstrOtaUpdateStatusResp));
        ret = hif_receive(u32Addr, (uint8_t*) &strOtaUpdateStatusResp,sizeof(tstrOtaUpdateStatusResp), 0);
        if(ret == M2M_SUCCESS)
        {
            if(gpfOtaUpdateCb)
                gpfOtaUpdateCb(strOtaUpdateStatusResp.u8OtaUpdateStatusType,strOtaUpdateStatusResp.u8OtaUpdateStatus);
        }
    }
    else
    {
        M2M_ERR("Invalid OTA resp %d ?\r\n",u8OpCode);
    }

}
/*!
@fn \
    int8_t  m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb, tpfOtaNotifCb pfOtaNotifCb);

@brief
    Initialize the OTA layer.

@param [in] pfOtaUpdateCb
                OTA Update callback function

@param [in] pfOtaNotifCb
                OTA notify callback function

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t  m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb, tpfOtaNotifCb pfOtaNotifCb)
{
    int8_t ret = M2M_SUCCESS;

    if(pfOtaUpdateCb){
        gpfOtaUpdateCb = pfOtaUpdateCb;
    }else{
        M2M_ERR("Invalid OTA update callback\r\n");
    }
    if(pfOtaNotifCb){
        gpfOtaNotifCb = pfOtaNotifCb;
    }else{
        M2M_ERR("Invalid Ota notify callback\r\n");
    }

    hif_register_cb(M2M_REQ_GROUP_OTA,m2m_ota_cb);

    return ret;
}
/*!
@fn \
    int8_t  m2m_ota_notif_set_url(uint8_t * u8Url);

@brief
    Set the OTA url

@param [in] u8Url
             The url server address

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t  m2m_ota_notif_set_url(uint8_t * u8Url)
{
    int8_t ret = M2M_SUCCESS;
    uint16_t u16UrlSize = strlen((const char*)u8Url) + 1;
    /*Todo: we may change it to data pkt but we need to give it higer priority
            but the priorty is not implemnted yet in data pkt
    */
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_NOTIF_SET_URL,u8Url,u16UrlSize,NULL,0,0);
    return ret;

}

/*!
@fn \
    int8_t  m2m_ota_notif_check_for_update(void);

@brief
    check for ota update

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t  m2m_ota_notif_check_for_update(void)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_NOTIF_CHECK_FOR_UPDATE,NULL,0,NULL,0,0);
    return ret;
}

/*!
@fn \
    int8_t m2m_ota_notif_sched(uint32_t u32Period);

@brief
    Schedule OTA update

@param [in] u32Period
    Period in days

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ota_notif_sched(uint32_t u32Period)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_NOTIF_CHECK_FOR_UPDATE,NULL,0,NULL,0,0);
    return ret;
}

/*!
@fn \
    int8_t m2m_ota_start_update(uint8_t * u8DownloadUrl);

@brief
    Request OTA start update using the downloaded url

@param [in] u8DownloadUrl
        The download firmware url, you get it from device info

@return
    The function SHALL return 0 for success and a negative value otherwise.

*/
int8_t m2m_ota_start_update(uint8_t * u8DownloadUrl)
{
    int8_t ret = M2M_SUCCESS;
    uint16_t u16DurlSize = strlen((const char*)u8DownloadUrl) + 1;
    /*Todo: we may change it to data pkt but we need to give it higer priority
            but the priorty is not implemnted yet in data pkt
    */
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_START_FW_UPDATE,u8DownloadUrl,u16DurlSize,NULL,0,0);
    return ret;
}
/*!
@fn \
    int8_t m2m_ota_start_update_crt(uint8_t * u8DownloadUrl);

@brief
    Request OTA start for the Cortus app image.

@param [in] u8DownloadUrl
        The cortus application image url.

@return
    The function SHALL return 0 for success and a negative value otherwise.

*/
int8_t m2m_ota_start_update_crt(uint8_t * u8DownloadUrl)
{
    int8_t ret = M2M_SUCCESS;
    uint16_t u16DurlSize = strlen((const char*)u8DownloadUrl) + 1;
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_START_CRT_UPDATE,u8DownloadUrl,u16DurlSize,NULL,0,0);
    return ret;
}


/*!
@fn \
    int8_t m2m_ota_rollback(void);

@brief
    Request OTA Rollback image

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ota_rollback(void)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_ROLLBACK_FW,NULL,0,NULL,0,0);
    return ret;
}
/*!
@fn \
    int8_t m2m_ota_rollback_crt(void);

@brief
    Request Cortus application OTA Rollback image

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ota_rollback_crt(void)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_ROLLBACK_CRT,NULL,0,NULL,0,0);
    return ret;
}

/*!
@fn \
    int8_t m2m_ota_abort(void);

@brief
    Request OTA Abort

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ota_abort(void)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_ABORT,NULL,0,NULL,0,0);
    return ret;
}


/*!
@fn \
    int8_t m2m_ota_switch_firmware(void);

@brief
    Switch to the upgraded Firmware

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ota_switch_firmware(void)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_SWITCH_FIRMWARE,NULL,0,NULL,0,0);
    return ret;
}
/*!
@fn \
    int8_t m2m_ota_switch_crt(void);

@brief
    Switch to the upgraded cortus application.

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ota_switch_crt(void)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_send(M2M_REQ_GROUP_OTA,M2M_OTA_REQ_SWITCH_CRT_IMG,NULL,0,NULL,0,0);
    return ret;
}

/*!
@fn \
    int8_t m2m_ota_get_firmware_version(tstrM2mRev * pstrRev);

@brief
    Get the OTA Firmware version.

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ota_get_firmware_version(tstrM2mRev * pstrRev)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_chip_wake();
    if(ret == M2M_SUCCESS)
    {
        ret = nm_get_ota_firmware_info(pstrRev);
        hif_chip_sleep();
    }
    return ret;
}

//DOM-IGNORE-END
