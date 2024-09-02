// DOM-IGNORE-BEGIN
/*
Copyright (C) 2017, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
 * wifiprov_task.c
 *
 */
#include "at_ble_api.h"
#include "platform.h"
#include "interface.h"
#
#include "wifiprov_task.h"
#include "profiles.h"

/*
 * msgs from APP to wifiprov
 *
 *
 */
#ifdef WDRV_WINC_DEVICE_BLE_API_REV_2
at_ble_status_t wifiprov_configure_provisioning(uint8_t* lname, at_ble_auth_t lauthtype)
#else
at_ble_status_t wifiprov_configure_provisioning(uint8_t* lname)
#endif
{
    //TODO: check on platform error
    uint8_t lname_len = 0;
    if (lname)
    {
        lname_len = strlen((const char*)lname);
        if (lname_len > MAX_LOCALNAME_LENGTH)
            lname_len = MAX_LOCALNAME_LENGTH;
    }

    at_ble_status_t status;
    INTERFACE_MSG_INIT(WIFIPROV_CONFIGURE_REQ, TASK_WIFIPROV);
#ifdef WDRV_WINC_DEVICE_BLE_API_REV_2
    INTERFACE_PACK_ARG_UINT8(lauthtype);
#endif    
    INTERFACE_PACK_ARG_UINT8(lname_len);
    INTERFACE_PACK_ARG_BLOCK(lname,lname_len);
    INTERFACE_SEND_WAIT(WIFIPROV_CONFIGURE_CFM, TASK_WIFIPROV, &status);
    if(status == AT_BLE_SUCCESS)
    {
        INTERFACE_UNPACK_UINT8(&status);
    }
    if (status == 0) status = AT_BLE_SUCCESS;
    INTERFACE_MSG_DONE();
    return status;
}

at_ble_status_t wifiprov_create_db(void)
{
    //TODO: check on platform error
    at_ble_status_t status;

    INTERFACE_MSG_INIT(WIFIPROV_CREATE_DB_REQ, TASK_WIFIPROV);
    INTERFACE_PACK_ARG_UINT8(1|2); // We support both scanning and connection
    INTERFACE_SEND_WAIT(WIFIPROV_CREATE_DB_CFM, TASK_WIFIPROV, &status);
    if(status == AT_BLE_SUCCESS)
    {
        INTERFACE_UNPACK_UINT8(&status);
    }
    //GTODO: Export status define from wifiprov to header file
    if (status == 0) status = AT_BLE_SUCCESS;
    INTERFACE_MSG_DONE();
    return status;
}

at_ble_status_t wifiprov_start(uint8_t *pin, uint8_t len)
{
    //TODO: check on platform error
    at_ble_status_t status = AT_BLE_SUCCESS;

    INTERFACE_MSG_INIT(WIFIPROV_START_CMD, TASK_WIFIPROV);
    INTERFACE_PACK_ARG_BLOCK(pin,len);
    INTERFACE_SEND_NO_WAIT();
    INTERFACE_MSG_DONE();
    return status;
}

at_ble_status_t wifiprov_disable(void)
{
    //TODO: check on platform error
    at_ble_status_t status= AT_BLE_SUCCESS;

    INTERFACE_MSG_INIT(WIFIPROV_DISABLE_CMD, TASK_WIFIPROV);
    INTERFACE_SEND_NO_WAIT();
    INTERFACE_MSG_DONE();
    return status;
}

at_ble_status_t wifiprov_scan_mode_change_ind_send(uint8_t scanmode)
{
    //TODO: check on platform error
    at_ble_status_t status = AT_BLE_SUCCESS;

    INTERFACE_MSG_INIT(WIFIPROV_SCAN_MODE_CHANGE_IND, TASK_WIFIPROV);
    INTERFACE_PACK_ARG_UINT8(scanmode);
    INTERFACE_SEND_NO_WAIT();
    INTERFACE_MSG_DONE();
    return status;
}

at_ble_status_t wifiprov_scan_list_ind_send(struct wifiprov_scanlist_ind *param)
{
    //TODO: check on platform error
    at_ble_status_t status = AT_BLE_SUCCESS;

    INTERFACE_MSG_INIT(WIFIPROV_SCANLIST_IND, TASK_WIFIPROV);
    INTERFACE_PACK_ARG_BLOCK(param, sizeof(struct wifiprov_scanlist_ind));  // GTODO: Does this need to be 'repacked' using pack macros?
    INTERFACE_SEND_NO_WAIT();
    INTERFACE_MSG_DONE();
    return status;
}

at_ble_status_t wifiprov_wifi_con_update(uint8_t conn_state)
{
    //TODO: check on platform error
    at_ble_status_t status = AT_BLE_SUCCESS;

    INTERFACE_MSG_INIT(WIFIPROV_CONNECTION_STATE_CHANGE_IND, TASK_WIFIPROV);
    INTERFACE_PACK_ARG_UINT8(conn_state);
    INTERFACE_SEND_NO_WAIT();
    INTERFACE_MSG_DONE();
    return status;
}

/*
 * events from WIFIPROV to APP
 *
 *
 */
void wifiprov_scan_mode_change_ind_handler(uint8_t *data, at_ble_wifiprov_scan_mode_change_ind_t *param)
{
    INTERFACE_UNPACK_INIT(data);
    INTERFACE_UNPACK_UINT8(&param->scanmode);
    INTERFACE_UNPACK_DONE();
}

void wifiprov_complete_ind_handler(uint8_t *data, at_ble_wifiprov_complete_ind *param)
{
    INTERFACE_UNPACK_INIT(data);
    INTERFACE_UNPACK_UINT8(&param->status);
    INTERFACE_UNPACK_UINT8(&param->sec_type);
    INTERFACE_UNPACK_UINT8(&param->ssid_length);
    INTERFACE_UNPACK_BLOCK(&param->ssid, MAX_WIPROVTASK_SSID_LENGTH);
    INTERFACE_UNPACK_UINT8(&param->passphrase_length);
    INTERFACE_UNPACK_BLOCK(&param->passphrase, MAX_WIPROVTASK_PASS_LENGTH);
    INTERFACE_UNPACK_DONE();
}
// DOM-IGNORE-END
