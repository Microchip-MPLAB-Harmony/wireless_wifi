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
// DOM-IGNORE-END

/**@defgroup OTAAPI OTA
*/

#ifndef __M2M_OTA_H__
#define __M2M_OTA_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "nm_common.h"
#include "m2m_types.h"
#include "nmdrv.h"

/**
* @addtogroup OTATYPEDEF
*/
/**@{*/

/*!
@typedef \
    void (*tpfOtaUpdateCb) (uint8_t u8OtaUpdateStatusType ,uint8_t u8OtaUpdateStatus);

@brief
    A callback to get OTA status update, the callback provides the download status,
    the switch to the downloaded firmware status and roll-back status.

@param[in] u8OtaUpdateStatusType Possible values are listed in \ref tenuOtaUpdateStatusType. Possible types are:
- [DL_STATUS](@ref DL_STATUS)
- [SW_STATUS](@ref SW_STATUS)
- [RB_STATUS](@ref RB_STATUS)

@param[in] u8OtaUpdateStatus Possible values are listed in \ref tenuOtaUpdateStatus.

@see
    tenuOtaUpdateStatusType
    tenuOtaUpdateStatus
 */
typedef void (*tpfOtaUpdateCb) (uint8_t u8OtaUpdateStatusType ,uint8_t u8OtaUpdateStatus);
 /**@}
 */
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/** \defgroup OTAFUNCTIONS Functions
 *  @ingroup OTAAPI
 */
#ifdef __cplusplus
extern "C" {
#endif

 /**@{*/
/*!
@fn \
    int8_t  m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb);
 * Synchronous initialization function for the OTA layer by registering the update callback.
 * Calling this API is a prerequisite for all other OTA API's.
@param [in]     pfOtaUpdateCb
                    OTA Update callback function

@return
    The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
int8_t  m2m_ota_init(tpfOtaUpdateCb pfOtaUpdateCb);
/*!
@fn \
    int8_t m2m_ota_start_update(uint8_t * u8DownloadUrl);
        Request OTA start update using the downloaded URL.
        The firmware OTA module will download the OTA image, ensure integrity of the image, and update the validity of the image in
        the control structure.
        On completion, a callback of type @ref tpfOtaUpdateCb is called (callback previously provided via @ref m2m_ota_init()).
        Switching to the updated image additionally requires completion of @ref m2m_ota_switch_firmware(), and system_reset().

@param [in] u8DownloadUrl
            The download firmware URL, according to the application server.

@warning
        Calling this API does not guarantee OTA WINC image update; it depends on the connection with the download server and the validity
        of the image.
        Calling this API invalidates the previous rollback image. The current image will become the rollback image after
        @ref m2m_ota_switch_firmware().

@pre
        @ref m2m_ota_init() must have been called before using @ref m2m_ota_start_update().
@sa
        @ref m2m_ota_init()
        @ref m2m_ota_switch_firmware()
        @ref tpfOtaUpdateCb

@return
        The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
        Note that successful operation in this context means the OTA update request has reached the firmware OTA module. It does not
        indicate whether or not the image update succeeded.

\section OTAExample Example
        This example shows how an OTA image update and switch is carried out.
        It demonstrates use of the following OTA APIs:
            @ref m2m_ota_init()
            @ref tpfOtaUpdateCb
            @ref m2m_ota_start_update()
            @ref m2m_ota_switch_firmware()
            @ref m2m_ota_rollback()
        It also makes use of @ref m2m_wifi_check_ota_rb() in order to inform OTA decisions.

@code
static void OtaUpdateCb(uint8_t u8OtaUpdateStatusType ,uint8_t u8OtaUpdateStatus)
{
    int8_t s8tmp;
    tstrM2mRev strtmp;

    M2M_INFO("%d %d\n", u8OtaUpdateStatusType, u8OtaUpdateStatus);

    switch(u8OtaUpdateStatusType) {
        case DL_STATUS:
            if(u8OtaUpdateStatus == OTA_STATUS_SUCCESS) {
                M2M_INFO("OTA download succeeded\n");
                s8tmp = m2m_wifi_check_ota_rb();
                if(s8tmp == M2M_ERR_FW_VER_MISMATCH) {
                    //  In this case the application SHOULD update the host driver before calling
                    //  @ref m2m_ota_switch_firmware(). Switching firmware image and resetting without updating host
                    //  driver would lead to severely limited functionality (i.e. OTA rollback only).
                }
                else if(s8tmp == M2M_SUCCESS) {
                    //  In this case the application MAY WANT TO update the host driver before calling
                    //  @ref m2m_ota_switch_firmware(). Switching firmware image and resetting without updating host
                    //  driver may lead to suboptimal functionality.
                }
                else {
                    M2M_INFO("Cannot recognize downloaded image\n");
                    //  In this case the application MUST NOT update the host driver if such an update would change the
                    //  driver HIF Major field. Firmware switch @ref using m2m_ota_switch_firmware() is blocked.
                    break;
                }
                M2M_INFO("Now switching active partition...\n");
                s8tmp = m2m_ota_switch_firmware();
            }
        break;
        case SW_STATUS:
        case RB_STATUS:
            if(u8OtaUpdateStatus == OTA_STATUS_SUCCESS) {
                M2M_INFO("Switch/Rollback succeeded\n");
                M2M_INFO("Now resetting the system...\n");
                system_reset();
            }
        break;
    }
}
static void wifi_event_cb(uint8_t u8WiFiEvent, void * pvMsg)
{
    //  ...
    case M2M_WIFI_REQ_DHCP_CONF:
    {
        //After successful connection, start the OTA upgrade.
        m2m_ota_start_update(OTA_URL);
    }
    break;
    //  ...
}
int main (void)
{
    int8_t              ret = M2M_SUCCESS;
    tstrWifiInitParam   param;
    bool                rollback_required = FALSE;

    //  ... system init etc here ...

    //Initialize the WINC Driver.
    memset((uint8_t*)&param, 0, sizeof(param));
    param.pfAppWifiCb = wifi_event_cb;

    ret = m2m_wifi_init(&param);
    if(ret == M2M_ERR_FW_VER_MISMATCH) {
        ret = m2m_wifi_check_ota_rb();
        if(ret == M2M_SUCCESS) {
            //  In this case the image in the inactive partition has compatible HIF. We will switch/rollback to it
            //  after initializing the OTA module.
            rollback_required = TRUE;
        }
    }
    if(ret != M2M_SUCCESS) {
        M2M_ERR("Driver Init Failed <%d>\n",ret);
        while(1);
    }

    //Initialize the OTA module.
    m2m_ota_init(OtaUpdateCb, NULL);

    if(rollback_required) {
        //  We need to call either @ref m2m_ota_rollback() or @ref m2m_ota_switch_firmware() (functionally equivalent).
        m2m_ota_rollback();
    } else {
        //  Connect to AP that provides connection to the OTA server
        m2m_wifi_default_connect();
    }

    while(1) {
        //  Handle the app state machine plus the WINC event handler.
        while(m2m_wifi_handle_events(NULL) != M2M_SUCCESS) {
        }
    }
}
@endcode

*/
int8_t m2m_ota_start_update(uint8_t * u8DownloadUrl);
/*!
@fn \
    int8_t m2m_ota_rollback(void);
    Request rollback to the old (inactive) WINC image. The WINC firmware will check the validity of
    the inactive image and activate it if it is valid.
    On completion, a callback of type @ref tpfOtaUpdateCb is called (application must previously
    have provided the callback via @ref m2m_ota_init()).
    If the callback indicates successful activation, the newly-activated image will start running
    after next system reset.

@warning
    If rollback will necessitate a host driver update in order to maintain HIF compatibility (HIF
    major value change), then it is recommended to update the host driver prior to calling this
    API.
    In the event of system reset with incompatible driver/firmware, compatibility can be recovered
    by calling @ref m2m_ota_rollback or @ref m2m_ota_switch_firmware. See @ref OTAExample.

@sa
    m2m_ota_init
    m2m_ota_start_update

@return
    The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
int8_t m2m_ota_rollback(void);
/*!
@fn \
    int8_t m2m_ota_abort(void);
    Abort an ota in progress (eg if server has stalled) and clean up
@sa
    m2m_ota_init
    m2m_ota_start_update

@return
    The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
int8_t m2m_ota_abort(void);
/*!
@fn \
    int8_t m2m_ota_switch_firmware(void);
    Request switch to the updated WINC image. The WINC firmware will check the validity of the
    inactive image and activate it if it is valid.
    On completion, a callback of type @ref tpfOtaUpdateCb is called (application must previously
    have provided the callback via @ref m2m_ota_init()).
    If the callback indicates successful activation, the newly-activated image will start running
    after next system reset.

@warning
    If switch will necessitate a host driver update in order to maintain HIF compatibility (HIF
    major value change), then it is recommended to update the host driver prior to calling this
    API.
    In the event of system reset with incompatible driver/firmware, compatibility can be recovered
    by calling @ref m2m_ota_rollback or @ref m2m_ota_switch_firmware. See @ref OTAExample.

@sa
    m2m_ota_init
    m2m_ota_start_update

@return
    The function returns @ref M2M_SUCCESS for successful operations  and a negative value otherwise.
*/
int8_t m2m_ota_switch_firmware(void);

 /**@}*/
#ifdef __cplusplus
}
#endif
#endif /* __M2M_OTA_H__ */
