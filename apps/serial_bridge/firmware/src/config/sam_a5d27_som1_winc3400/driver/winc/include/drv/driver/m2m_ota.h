/*******************************************************************************
  WINC3400 OTA Upgrade API Interface

  File Name:
    m2m_ota.h

  Summary:
    WINC3400 OTA Upgrade API Interface

  Description:
    WINC3400 OTA Upgrade API Interface
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
// DOM-IGNORE-END

/**@defgroup OTAAPI OTA
   @brief
        The WINC supports OTA (Over-The-Air) updates. Using the APIs described in this module,
        it is possible to request an ATWINC15x0 to update its firmware image, or safely rollback to
        the previous firmware image.\n Note that it is NOT possible to update other areas of the WINC
        flash (e.g. the HTTP file area) using the OTA mechanism.\n\n There are also APIs to download 
        files and store them in the WINC's Flash (supported by ATWINC1510 only), which can be used 
        for Host MCU OTA updates or accessing information stored remotely.
    @{
        @defgroup   OTACALLBACKS    Callbacks
        @brief
            Lists the different callbacks that can be used during OTA updates.\n
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
    @}
*/

#ifndef __M2M_OTA_H__
#define __M2M_OTA_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "nm_common.h"
#include "m2m_types.h"
#include "nmdrv.h"

/**@addtogroup  OTATYPEDEF
 * @{
 */
/*!
@enum   \
    tenuOTASSLOption
@brief
    Enumeration for OTA SSL options
    The following SSL options can be set for OTA
*/
typedef enum {
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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@typedef void (*tpfOtaUpdateCb)(uint8_t u8OtaUpdateStatusType, uint8_t u8OtaUpdateStatus);

@brief
   A callback to get OTA status update, the callback provides the status type and its status.\n
   The OTA callback provides the download status, the switch to the downloaded firmware status,
   roll-back status and Host File Download status.

@param[in] u8OtaUpdateStatusType
    Possible values are listed in @ref tenuOtaUpdateStatusType .

@param[in] u8OtaUpdateStatus
    Possible values are listed as enumerated by @ref tenuOtaUpdateStatus .

@note
    Executes other callbacks passed to the OTA module.

@see
    tenuOtaUpdateStatusType
    tenuOtaUpdateStatus
 */
typedef void (*tpfOtaUpdateCb)(uint8_t u8OtaUpdateStatusType, uint8_t u8OtaUpdateStatus);
/**@}*/     // OTACALLBACKS

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
extern "C" {
#endif

/*!
@ingroup OTAFUNCTIONS
@fn \
    int8_t  m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb);

@brief
    Synchronous initialization function for the OTA layer by registering the update callback.\n
    The notification callback is not supported at the current version. Calling this API is a
    MUST for all the OTA API's.

@param[in]  pfOtaUpdateCb
    OTA Update callback function.

@return
    The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
 */
int8_t m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb);

/*!
@ingroup OTAFUNCTIONS
@fn \
    int8_t m2m_ota_start_update(unsigned char * pcDownloadUrl);

@brief
    Request OTA start update using the download URL. The OTA module will download the OTA image, ensure integrity of the image
    and update the validity of the image in the control structure. On completion, a callback of type @ref tpfOtaUpdateCb is called
    (callback previously provided via @ref m2m_ota_init ). Switching to the updated image additionally requires completion of
    @ref m2m_ota_switch_firmware and @ref system_reset .

@param[in]  pcDownloadUrl
    The download firmware URL, according to the application server.

@warning
    Calling this API does not guarantee OTA WINC image update, it depends on the connection with the
    download server and the validity of the image.\n
    Calling this API invalidates any previous valid rollback image, irrespective of the result, but when
    the OTA succeeds, the current image will become the rollback image after @ref m2m_ota_switch_firmware.

@pre
    @ref m2m_ota_init is a prerequisite and must have been called before using @ref m2m_ota_start_update \n
    Switching to the newly downloaded image requires calling @ref m2m_ota_switch_firmware API.

@sa
    @ref m2m_ota_init
    @ref m2m_ota_switch_firmware
    @ref tpfOtaUpdateCb

@return
    The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
    Note that successful operation in this context means the OTA update request has reached the firmware OTA module.
    It does not indicate whether or not the image update succeeded.

@section OTAExample Example
        This example shows how an OTA image update and switch is carried out.
        It demonstrates use of the following OTA APIs:
            - @ref m2m_ota_init
            - @ref tpfOtaUpdateCb
            - @ref m2m_ota_start_update
            - @ref m2m_ota_switch_firmware
            - @ref m2m_ota_rollback

        It also makes use of @ref m2m_wifi_check_ota_rb in order to inform OTA decisions.
@code
static void OtaUpdateCb(uint8_t u8OtaUpdateStatusType, uint8_t u8OtaUpdateStatus)
{
    int8_t s8tmp;
    tstrM2mRev strtmp;
    M2M_INFO("%d %d\n", u8OtaUpdateStatusType, u8OtaUpdateStatus);
    switch(u8OtaUpdateStatusType)
    {
    case DL_STATUS:
        if(u8OtaUpdateStatus == OTA_STATUS_SUCCESS)
        {
            M2M_INFO("OTA download succeeded\n");
            s8tmp = m2m_wifi_check_ota_rb();
            if(s8tmp == M2M_ERR_FW_VER_MISMATCH)
            {
                //  In this case the application SHOULD update the host driver before calling
                //  @ref m2m_ota_switch_firmware(). Switching firmware image and resetting without updating host
                //  driver would lead to severely limited functionality (i.e. OTA rollback only).
            }
            else if(s8tmp == M2M_SUCCESS)
            {
                //  In this case the application MAY WANT TO update the host driver before calling
                // @ref m2m_ota_switch_firmware(). Switching firmware image and resetting without
                // updating host driver may lead to suboptimal functionality.
            }
            else
            {
                M2M_INFO("Cannot recognize downloaded image\n");
                //  In this case the application MUST NOT update the host driver if such an update would change the
                //  driver HIF Major field. Firmware switch @ref using m2m_ota_switch_firmware() is blocked.
                break;
            }
            // Switch to the upgraded firmware
            M2M_INFO("Now switching active partition...\n");
            s8tmp = m2m_ota_switch_firmware();
        }
        break;
    case SW_STATUS:
    case RB_STATUS:
        if(u8OtaUpdateStatus == OTA_STATUS_SUCCESS)
        {
            M2M_INFO("Switch/Rollback succeeded\n");

            // Start the host SW upgrade if required, then system reset is required (Reinitialize the driver)

            M2M_INFO("Now resetting the system...\n");
            system_reset();
        }
        break;
    }
}

static void wifi_event_cb(uint8_t u8WiFiEvent, void *pvMsg)
{
    // ...
    case M2M_WIFI_REQ_DHCP_CONF:
    {
        // After successful connection, start the OTA upgrade
        m2m_ota_start_update(OTA_URL);
    }
    break;
    default:
    break;
    // ...
}

int main(void)
{
    tstrWifiInitParam   param;
    int8_t               s8Ret             = M2M_SUCCESS;
    bool                rollback_required = FALSE;

    // System init, etc should be here...

    memset((uint8_t *)&param, 0, sizeof(param));
    param.pfAppWifiCb = wifi_event_cb;

    // Initialize the WINC Driver
    s8Ret = m2m_wifi_init(&param);
    if(s8Ret == M2M_ERR_FW_VER_MISMATCH)
    {
        M2M_ERR("Firmware version mismatch\n");
        s8Ret = m2m_wifi_check_ota_rb();
        if(s8Ret == M2M_SUCCESS)
        {
            //  In this case the image in the inactive partition has compatible HIF. We will switch/rollback to it
            //  after initializing the OTA module.
            rollback_required = TRUE;
        }
    }
    if(M2M_SUCCESS != s8Ret)
    {
        M2M_ERR("Driver Init Failed <%d>\n", s8Ret);
        while(1);
    }
    // Initialize the OTA module
    m2m_ota_init(OtaUpdateCb, NULL);
    if(rollback_required)
    {
        //  We need to call either @ref m2m_ota_rollback() or @ref m2m_ota_switch_firmware() (functionally equivalent).
        m2m_ota_rollback();
    }
    else
    {
        //  Connect to AP that provides connection to the OTA server
        m2m_wifi_default_connect();
    }
    while(1)
    {
        //  Handle the app state machine plus the WINC event handler
        while(m2m_wifi_handle_events(NULL) != M2M_SUCCESS) {
        }
    }
}
@endcode
 */
int8_t m2m_ota_start_update(unsigned char *pcDownloadUrl);

/*!
@ingroup OTAFUNCTIONS
@fn \
    int8_t m2m_ota_rollback(void);

@brief
    Request OTA Roll-back to the old (inactive) WINC image, the WINC firmware will check the validity of the inactive image
    and activate it if valid. On completion, a callback of type @ref tpfOtaUpdateCb is called (application must previously have
    provided the callback via @ref m2m_ota_init). If the callback indicates successful activation, the newly-activated image
    will start running after next system reset.

@warning
    If rollback requires a host driver update in order to maintain HIF compatibility (HIF
    major value change), then it is recommended to update the host driver prior to calling this API.\n
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
@ingroup OTAFUNCTIONS
@fn \
    int8_t m2m_ota_abort(void);

@brief
    Request the WINC to abort an OTA or Host File download in progress.\n
    If no download is in progress, the API will respond with failure.

@sa
    m2m_ota_init
    m2m_ota_start_update

@return
    The function returns @ref M2M_SUCCESS for a successful operation and a negative value otherwise.
 */
int8_t m2m_ota_abort(void);

/*!
@ingroup OTAFUNCTIONS
@fn \
    int8_t m2m_ota_switch_firmware(void);

@brief
    Request switch to the updated WINC image. The WINC firmware will check the validity of the
    inactive image and activate it if valid. On completion, a callback of type @ref tpfOtaUpdateCb
    is called (application must previously have provided the callback via @ref m2m_ota_init).
    If the callback indicates successful activation, the newly-activated image will start running
    after next system reset.

@warning
    If switch requires a host driver update in order to maintain HIF compatibility (HIF
    major value change), then it is recommended to update the host driver prior to calling this API.\n
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
@ingroup OTAFUNCTIONS
@fn \
    int8_t m2m_ota_set_ssl_option(tenuOTASSLOption enuOptionName, const void *pOptionValue, size_t OptionLen)

@brief
    Set specific SSL options to be used when the WINC performs an OTA from an https server.

@details
	The following options can be set:\n

	@ref WIFI_OTA_SSL_OPT_BYPASS_SERVER_AUTH \n
	@ref WIFI_OTA_SSL_OPT_SNI_VALIDATION \n
	@ref WIFI_OTA_SSL_OPT_SNI_SERVERNAME \n

	The setting applies to all subsequent OTA attempts via @ref m2m_ota_start_update \n

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
@ingroup OTAFUNCTIONS
@fn \
    int8_t m2m_ota_get_ssl_option(tenuOTASSLOption enuOptionName, void *pOptionValue, size_t *pOptionLen)

@brief
    Get (read) SSL options relating to OTA

@details
	The following options can be read:\n

		@ref WIFI_OTA_SSL_OPT_BYPASS_SERVER_AUTH \n
		@ref WIFI_OTA_SSL_OPT_SNI_VALIDATION \n
		@ref WIFI_OTA_SSL_OPT_SNI_SERVERNAME \n

@param[in]      enuOptionName
	The option to get.

@param[out]     pOptionValue
    Pointer to a buffer to contain the value to get. The buffer must be at least as long as the value pointed to by pOptionLen.

@param[inout]  pOptionLen
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
