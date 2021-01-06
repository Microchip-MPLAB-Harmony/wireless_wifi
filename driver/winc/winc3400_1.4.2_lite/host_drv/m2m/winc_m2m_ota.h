/**
 *
 * \file
 *
 * \brief WINC OTA Upgrade API Interface.
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

/**@defgroup OTAAPI OTA
   @brief
        The WINC supports OTA (Over-The-Air) updates. Using the APIs described in this module,
        it is possible to request an ATWINC15x0 to update its firmware image, or safely rollback to
        the previous firmware image.
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
            The OTA callback provides the download status, the switch to the downloaded firmware status
            and roll-back status.

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
            Request the WINC to abort an OTA in progress.<br>
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

#ifdef __cplusplus
}
#endif
#endif /* __M2M_OTA_H__ */
