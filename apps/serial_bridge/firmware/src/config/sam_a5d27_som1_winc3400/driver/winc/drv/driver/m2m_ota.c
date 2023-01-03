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
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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

static uint8_t    gu8OTASSLOpts        = 0;
static uint8_t    gu8SNIServerName[64] = {0};

/* Map OTA SSL flags to SSL socket options */
#define WIFI_OTA_SSL_FLAG_BYPASS_SERVER_AUTH	NBIT1
#define WIFI_OTA_SSL_FLAG_SNI_VALIDATION		NBIT6

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
int8_t m2m_ota_start_update(unsigned char *pcDownloadUrl)
{
	int8_t result;
    tstrOtaStart strOtaStart;
    uint16_t u16UrlLen = strlen((char*)pcDownloadUrl);
    if (u16UrlLen >= 255)
    {
        return M2M_ERR_INVALID_ARG;	
    }

    memset(&strOtaStart, 0, sizeof(strOtaStart));
    memcpy(&strOtaStart.acUrl, pcDownloadUrl, u16UrlLen);

    /* Convert SSL options to flags */
    if (gu8OTASSLOpts & WIFI_OTA_SSL_OPT_BYPASS_SERVER_AUTH)
        strOtaStart.u8SSLFlags |= WIFI_OTA_SSL_FLAG_BYPASS_SERVER_AUTH;

    if (gu8OTASSLOpts & WIFI_OTA_SSL_OPT_SNI_VALIDATION)
        strOtaStart.u8SSLFlags |= WIFI_OTA_SSL_FLAG_SNI_VALIDATION;

    memcpy(&strOtaStart.acSNI, gu8SNIServerName, strlen((char*)gu8SNIServerName));

    strOtaStart.u32TotalLen = sizeof(strOtaStart);
    
    result = hif_send(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_START_UPDATE_V2 | M2M_REQ_DATA_PKT, (uint8_t*)&strOtaStart, strOtaStart.u32TotalLen, NULL, 0, 0);

    if ((result == M2M_ERR_SEND) && (strOtaStart.u8SSLFlags == 0))
    {
    	/* Failure may be due to new host with old firmware, try legacy OTA  */
    	/* as no server auth bypass nor server validation were required      */
    	result = hif_send(M2M_REQ_GROUP_OTA, M2M_OTA_REQ_START_UPDATE, pcDownloadUrl, u16UrlLen, NULL, 0, 0);
    }

    return result;
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

/*!
@fn         int8_t m2m_ota_set_ssl_option(tenuOTASSLOption enuOptionName, const void *pOptionValue, size_t OptionLen)
@brief      Sets SSL related options for OTA via https connections
@param[in]  enuOptionName
The SSL option to set, from the set defined in tenuOTASSLOption
@param[in]  pOptionValue
Pointer to the option value to set. Either a pointer to a uint32 with the value of 0 or 1, or a pointer to a string for the
WIFI_OTA_SSL_OPT_SNI_SERVERNAME option.
@param[in]  OptionLen
The size of the option referred to in pOptionValue
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ota_set_ssl_option(tenuOTASSLOption enuOptionName, const void *pOptionValue, size_t OptionLen)
{
    if((pOptionValue == NULL) && (OptionLen > 0))
        return M2M_ERR_INVALID_ARG;

    switch(enuOptionName)
    {
        case WIFI_OTA_SSL_OPT_SNI_SERVERNAME:
            if(OptionLen > 64)
                return M2M_ERR_INVALID_ARG;
            if (strlen(pOptionValue)+1 != OptionLen)
                return M2M_ERR_INVALID_ARG;

            memcpy(gu8SNIServerName, pOptionValue, OptionLen);
            break;

        case WIFI_OTA_SSL_OPT_SNI_VALIDATION:
        case WIFI_OTA_SSL_OPT_BYPASS_SERVER_AUTH:
            if(OptionLen != sizeof(int))
                return M2M_ERR_INVALID_ARG;
            switch(*(int*)pOptionValue)
            {
                case 1:
                    gu8OTASSLOpts |= enuOptionName;
                break;
                case 0:
                    gu8OTASSLOpts &= ~enuOptionName;
                break;
                default:
                    return M2M_ERR_INVALID_ARG;
            }
        break;

        default:
            return M2M_ERR_INVALID_ARG;
    }
    return M2M_SUCCESS;
}

/*!
@fn         int8_t m2m_ota_get_ssl_option(tenuOTASSLOption enuOptionName, void *pOptionValue, size_t *OptionLen)
@brief      Gets the status of SSL related options for OTA via https connections
@param[in]  enuOptionName
The SSL option to obtain the status of, from the set defined in tenuOTASSLOption
@param[in]  pOptionValue
Pointer to the option value to be updated by the function. Either a pointer to a uint32, or a pointer to a buffer for the
WIFI_OTA_SSL_OPT_SNI_SERVERNAME option.
@param[in]  OptionLen
A pointer to a size_t type variable which will be updated to contain the size of the returned option.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ota_get_ssl_option(tenuOTASSLOption enuOptionName, void *pOptionValue, size_t *pOptionLen)
{
    if((pOptionValue == NULL) || (pOptionLen == NULL))
        return M2M_ERR_INVALID_ARG;

    switch(enuOptionName)
    {
    case WIFI_OTA_SSL_OPT_SNI_VALIDATION:
    case WIFI_OTA_SSL_OPT_BYPASS_SERVER_AUTH:
        if(*pOptionLen < sizeof(int))
            return M2M_ERR_INVALID_ARG;
        *pOptionLen = sizeof(int);
        *(int*)pOptionValue = (gu8OTASSLOpts & enuOptionName) ? 1 : 0;
        break;
    case WIFI_OTA_SSL_OPT_SNI_SERVERNAME:
    {
        size_t sni_len = strlen((char*)gu8SNIServerName)+1;
        if(*pOptionLen < sni_len)
            return M2M_ERR_INVALID_ARG;
        *pOptionLen = sni_len;
        memcpy(pOptionValue, gu8SNIServerName, sni_len);
    }
        break;
    default:
        return M2M_ERR_INVALID_ARG;
    }
    return M2M_SUCCESS;
}
