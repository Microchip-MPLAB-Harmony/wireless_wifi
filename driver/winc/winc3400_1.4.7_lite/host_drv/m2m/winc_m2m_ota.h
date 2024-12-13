/**
 *
 * \file
 *
 * \brief WINC OTA Upgrade API Interface.
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

/**@defgroup OTAAPI OTA
    @brief
        The WINC supports OTA (Over-The-Air) updates. Using the APIs described in this module,
        it is possible to request an ATWINC15x0 to update its firmware image, or safely rollback to
        the previous firmware image.
    @note
        It is NOT possible to update other areas of the WINC flash (e.g. the HTTP file area) using the OTA mechanism.
    @{
        @defgroup   OTACALLBACKS    Callbacks
        @brief
            Lists the different callbacks that can be used during OTA updates.<br>
            Callbacks of type @ref tpfOtaNotifCb and @ref tpfOtaUpdateCb should be passed
            onto @ref m2m_ota_init at system initialization. Other callbacks are provided
            to handle the various steps of Host File Download.

        @defgroup   OTADEFINE       Defines
        @brief
            Specifies the macros and defines used by the OTA APIs.

        @defgroup   OTATYPEDEF      Enumerations and Typedefs
        @brief
            Specifies the enums and Data Structures used by the OTA APIs.

        @defgroup   OTAFUNCTIONS    Functions
        @brief
            Lists the full set of available APIs to manage OTA updates and Host File Downloads.
        @{
            @defgroup OTACOMMON     Common
            @defgroup WINCOTA       WINC
        @}
    @}
*/

#ifndef __M2M_OTA_H__
#define __M2M_OTA_H__

/**@addtogroup  OTATYPEDEF
 * @{
 */
/*!
@enum       tenuOTASSLOption
@brief
            Enumeration for OTA SSL options
            The following SSL options can be set for OTA
*/
typedef enum
{
    WIFI_OTA_SSL_OPT_BYPASS_SERVER_AUTH = 0x1,
    /*!<Server authentication for OTA SSL connections. Values are of type int \n
    1: Bypass server authentication.\n*/
    WIFI_OTA_SSL_OPT_SNI_VALIDATION = 0x2,
    /*!<Server Name Indication. The actual server name to send must be passed using option @ref WIFI_OTA_SSL_OPT_SNI_SERVERNAME \n
    Values are of type int \n
    0: Do not check the received servername against the server provided one\n
    1: Check the received servername against the server provided one\n
    */
    WIFI_OTA_SSL_OPT_SNI_SERVERNAME = 0x4,
    /*!<The server name string to send in the SNI extension. \n
    Value is a null terminated string up to 64 characters in length including the null terminator.\n*/
} tenuOTASSLOption;

/**@}*/     //OTATYPEDEF


/**@addtogroup OTACALLBACKS
 * @{
 */
/*!
@typedef    void (*tpfOtaNotifCb) (tstrOtaUpdateInfo *pstrOtaUpdateInfo);

@brief
            A callback to get notification about a potential OTA update.

@param[in]  pstrOtaUpdateInfo
            A structure to provide notification payload.

@sa
            tstrOtaUpdateInfo

@warning
            The notification is not supported (Not implemented yet)
*/
typedef void (*tpfOtaNotifCb)(tstrOtaUpdateInfo *pstrOtaUpdateInfo);

/*!
@typedef    void (*tpfOtaUpdateCb)(uint8_t u8OtaUpdateStatusType, uint8_t u8OtaUpdateStatus);

@brief
            A callback to get OTA status update, the callback provides the status type and its status.<br>
            The OTA callback provides the download status, the switch to the downloaded firmware status,
            roll-back status and Host File Download status.

@param[in]  u8OtaUpdateStatusType
            Possible values are listed in @ref tenuOtaUpdateStatusType.

@param[in]  u8OtaUpdateStatus
            Possible values are listed as enumerated by @ref tenuOtaUpdateStatus.

@note
            Executes other callbacks passed to the OTA module.

@see
            tenuOtaUpdateStatusType
            tenuOtaUpdateStatus
*/
typedef void (*tpfOtaUpdateCb)(uint8_t u8OtaUpdateStatusType, uint8_t u8OtaUpdateStatus);
/**@}*/     //OTACALLBACKS

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
extern "C" {
#endif

/*!
@ingroup    OTACOMMON
@fn         int8_t m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb, tpfOtaNotifCb pfOtaNotifCb);

@brief
            Synchronous initialization function for the OTA layer by registering the update callback.<br>
            The notification callback is not supported at the current version. Calling this API is a
            MUST for all the OTA API's.

@param[in]  pfOtaUpdateCb
            OTA Update callback function.

@param[in]  pfOtaNotifCb
            OTA Notify callback function.

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb, tpfOtaNotifCb pfOtaNotifCb);

/*!
@ingroup    WINCOTA
@fn         int8_t m2m_ota_notif_set_url(uint8_t *u8Url);

@brief
            Set the OTA notification server URL, the function needs to be called before any check for update.<br>
            This functionality is not supported by WINC firmware.

@param[in]  u8Url
            Set the OTA notification server URL, the function needs to be called before any check for update.

@pre
            Prior calling of @ref m2m_ota_init is required.

@warning
            Notification Server is not supported in the current version (function is not implemented).

@see
            m2m_ota_init

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_ota_notif_set_url(uint8_t *u8Url);

/*!
@ingroup    WINCOTA
@fn         int8_t m2m_ota_notif_check_for_update(void);

@brief
            Synchronous function to check for the OTA update using the Notification Server URL.<br>
            Function is not implemented (not supported at the current version).

@warning
            Function is not implemented (not supported at the current version).

@sa
            m2m_ota_init
            m2m_ota_notif_set_url

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_ota_notif_check_for_update(void);

/*!
@ingroup    WINCOTA
@fn         int8_t m2m_ota_notif_sched(uint32_t u32Period);

@brief
            Schedule OTA notification Server check for update request after specific number of days.<br>
            Function is not implemented (not supported at the current version).

@param[in]  u32Period
            Period in days

@warning
            Function is not implemented (not supported at the current version).

@sa
            m2m_ota_init
            m2m_ota_notif_check_for_update
            m2m_ota_notif_set_url

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_ota_notif_sched(uint32_t u32Period);

/*!
@ingroup    WINCOTA
@fn         int8_t m2m_ota_start_update(unsigned char *pcDownloadUrl);

@brief
            Request OTA start update using the download URL. The OTA module will download the OTA image, ensure integrity of the image
            and update the validity of the image in the control structure. On completion, a callback of type @ref tpfOtaUpdateCb is called
            (callback previously provided via @ref m2m_ota_init). Switching to the updated image additionally requires completion of
            @ref m2m_ota_switch_firmware, followed by a WINC reset.

@param[in]  pcDownloadUrl
            The download firmware URL, according to the application server.

@warning
            Calling this API does not guarantee OTA WINC image update, it depends on the connection with the
            download server and the validity of the image.<br>
            Calling this API invalidates any previous valid rollback image, irrespective of the result, but when
            the OTA succeeds, the current image will become the rollback image after @ref m2m_ota_switch_firmware.

@pre
            @ref m2m_ota_init is a prerequisite and must have been called before using @ref m2m_ota_start_update.<br>
            Switching to the newly downloaded image requires calling @ref m2m_ota_switch_firmware API.

@sa
            @ref m2m_ota_init
            @ref m2m_ota_switch_firmware
            @ref tpfOtaUpdateCb

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
            Note that successful operation in this context means the OTA update request has reached the firmware OTA module.
            It does not indicate whether or not the image update succeeded.

@section    OTAExample Example
            This example shows how an OTA image update and switch is carried out.
            It demonstrates use of the following OTA APIs:
            - @ref m2m_ota_init
            - @ref tpfOtaUpdateCb
            - @ref m2m_ota_start_update
            - @ref m2m_ota_switch_firmware
            - @ref m2m_ota_rollback

@include    m2m_ota_start_update.c
@example    m2m_ota_start_update.c
*/
int8_t m2m_ota_start_update(unsigned char *pcDownloadUrl);

/*!
@ingroup    WINCOTA
@fn         int8_t m2m_ota_rollback(void);

@brief
            Request OTA Roll-back to the old (inactive) WINC image, the WINC firmware will check the validity of the inactive image
            and activate it if valid. On completion, a callback of type @ref tpfOtaUpdateCb is called (application must previously have
            provided the callback via @ref m2m_ota_init). If the callback indicates successful activation, the newly-activated image
            will start running after next system reset.

@warning
            If rollback requires a host driver update in order to maintain HIF compatibility (HIF
            major value change), then it is recommended to update the host driver prior to calling this API.<br>
            In the event of system reset with incompatible driver/firmware, compatibility can be
            recovered by calling @ref m2m_ota_rollback or @ref m2m_ota_switch_firmware. See @ref OTAExample.

@sa
            m2m_ota_init
            m2m_ota_start_update

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_ota_rollback(void);

/*!
@ingroup    OTACOMMON
@fn         int8_t m2m_ota_abort(void);

@brief
            Request the WINC to abort an OTA or Host File download in progress.<br>
            If no download is in progress, the API will respond with failure.

@sa
            m2m_ota_init
            m2m_ota_start_update

@return
            The function returns @ref M2M_SUCCESS for a successful operation and a negative value otherwise.
*/
int8_t m2m_ota_abort(void);

/*!
@ingroup    WINCOTA
@fn         int8_t m2m_ota_switch_firmware(void);

@brief
            Request switch to the updated WINC image. The WINC firmware will check the validity of the
            inactive image and activate it if valid. On completion, a callback of type @ref tpfOtaUpdateCb
            is called (application must previously have provided the callback via @ref m2m_ota_init).
            If the callback indicates successful activation, the newly-activated image will start running
            after next system reset.

@warning
            If switch requires a host driver update in order to maintain HIF compatibility (HIF
            major value change), then it is recommended to update the host driver prior to calling this API.<br>
            In the event of system reset with incompatible driver/firmware, compatibility can be
            recovered by calling @ref m2m_ota_rollback or @ref m2m_ota_switch_firmware. See @ref OTAExample.

@sa
            m2m_ota_init
            m2m_ota_start_update

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_ota_switch_firmware(void);

/*!
@ingroup    WINCOTA
@fn         int8_t m2m_ota_set_ssl_option(tenuOTASSLOption enuOptionName, const void *pOptionValue, size_t OptionLen)

@brief
            Set specific SSL options to be used when the WINC performs an OTA from an https server.

@details
            The following options can be set:
            - @ref WIFI_OTA_SSL_OPT_BYPASS_SERVER_AUTH
            - @ref WIFI_OTA_SSL_OPT_SNI_VALIDATION
            - @ref WIFI_OTA_SSL_OPT_SNI_SERVERNAME

            The setting applies to all subsequent OTA attempts via @ref m2m_ota_start_update <br>

@param[in]  enuOptionName
            The option to set.

@param[in]  pOptionValue
            Pointer to a buffer containing the value to set. The buffer must be at least as long as OptionLen.
            If OptionLen is 0, then pOptionValue may be NULL.

@param[in]  OptionLen
            The length of the option value being set (including the null terminator for strings).

@return
            The function returns @ref M2M_SUCCESS if the parameters are valid and @ref M2M_ERR_INVALID_ARG otherwise.
*/
int8_t m2m_ota_set_ssl_option(tenuOTASSLOption enuOptionName, const void *pOptionValue, size_t OptionLen);

/*!
@ingroup    WINCOTA
@fn         int8_t m2m_ota_get_ssl_option(tenuOTASSLOption enuOptionName, void *pOptionValue, size_t *pOptionLen)

@brief
            Get (read) SSL options relating to OTA

@details
            The following options can be read:
            - @ref WIFI_OTA_SSL_OPT_BYPASS_SERVER_AUTH
            - @ref WIFI_OTA_SSL_OPT_SNI_VALIDATION
            - @ref WIFI_OTA_SSL_OPT_SNI_SERVERNAME

@param[in]  enuOptionName
            The option to get.

@param[out] pOptionValue
            Pointer to a buffer to contain the value to get. The buffer must be at least as long as the value pointed to by pOptionLen.

@param[inout] pOptionLen
            Pointer to a length.
            When calling the function, this length must be the length of the buffer available for reading the option value,
            and must be large enough to hold the returned option value otherwise an @ref M2M_ERR_INVALID_ARG error will be returned.
            When the function returns, this length is the length of the data that has been populated by the function.

@return
            The function returns @ref M2M_SUCCESS if the parameters are valid and @ref M2M_ERR_INVALID_ARG otherwise.
*/
int8_t m2m_ota_get_ssl_option(tenuOTASSLOption enuOptionName, void *pOptionValue, size_t *pOptionLen);

#ifdef __cplusplus
}
#endif
#endif /* __M2M_OTA_H__ */
