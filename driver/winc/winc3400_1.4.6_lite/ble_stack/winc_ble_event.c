/*
Copyright (c) RivieraWaves 2009-2014
Copyright (C) 2017, Microchip Technology Inc., and its subsidiaries. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

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

#include "winc_ble_platform.h"
#include "winc_ble_api.h"

#include "winc_ble_gapm_task.h"
#include "winc_ble_api.h"
#include "winc_ble_gapc_task.h"
#include "winc_ble_gattc_task.h"
#include "winc_ble_gattm_task.h"
#include "winc_ble_wifiprov_task.h"
#include "winc_ble_device.h"

#include "winc_ble_interface.h"
#include "winc_ble_event.h"

#define EVENT_POOL_DEPTH 10

struct event
{
    struct event *next;
    uint16_t msg_id;
    uint16_t src_id;
    void *data;
};

static struct event event_pool[EVENT_POOL_DEPTH];

static struct event *event_free_list;
static struct event *event_pending_list;

struct str_watched_event watched_event;


static void event_free(struct event *event)
{
    event->next = event_free_list;
    event_free_list = event;
}

void event_post(uint16_t msg_id, uint16_t src_id, void *data)
{

    // get a free event object
    struct event *evt = event_free_list;

    if (evt != NULL)
    {
        event_free_list = evt->next;

        evt->next = NULL;
        evt->data = data;
        evt->msg_id = msg_id;
        evt->src_id = src_id;

        if (event_pending_list == NULL)
        {
            event_pending_list = evt;
        }
        else
        {
            struct event *cur = event_pending_list;

            while (cur->next != NULL)
            {
                cur = cur->next;
            }

            cur->next = evt;
        }
    }
}

static at_ble_status_t event_get(uint16_t *msg_id, uint16_t *src_id, uint8_t **data)
{
    struct event *event;

    if (event_pending_list == NULL)
    {
        return AT_BLE_FAILURE;
    }

    event = event_pending_list;
    event_pending_list = event_pending_list->next;
    *src_id = event->src_id;
    *msg_id = event->msg_id;
    *data = event->data;
    event_free(event);

    return AT_BLE_SUCCESS;
}

void event_init(void)
{
    uint32_t i;

    event_free_list = NULL;
    event_pending_list = NULL;

    for (i = 0; i < EVENT_POOL_DEPTH; i++)
    {
        event_free(&event_pool[i]);
    }
}

static at_ble_events_t handle_ble_event(uint16_t msg_id, uint16_t src_id, uint8_t *data, void *params)
{
    at_ble_events_t evt_num = AT_BLE_UNDEFINED_EVENT;

    //printf("RW evt : 0x%x\n", msg_id);

    switch (msg_id)
    {
        case GAPM_CMP_EVT:
        {
            evt_num = gapm_cmp_evt(data, params);
            break;
        }

        case GAPM_ADV_REPORT_IND:
        {
            evt_num = AT_BLE_SCAN_INFO;
            gapm_adv_report_evt_handler(data, (at_ble_scan_info_t *)params);
            break;
        }

        case GAPC_PARAM_UPDATED_IND:
        {
            evt_num = AT_BLE_CONN_PARAM_UPDATE_DONE;
            gapc_param_updated_ind(src_id, data, (at_ble_conn_param_update_done_t *)params);
            break;
        }

        case GAPC_PARAM_UPDATE_REQ_IND:
        {
            evt_num = AT_BLE_CONN_PARAM_UPDATE_REQUEST;
            gapc_param_update_req_ind(src_id, data, (at_ble_conn_param_update_request_t *)params);
            break;
        }

        case GATTC_SVC_CHANGED_CFG_IND:
        {
            uint8_t *pParam;
            evt_num = AT_BLE_SERVICE_CHANGED_NOTIFICATION_CONFIRMED;
            pParam = (uint8_t *) params;
            *pParam++ = data[0];
            *pParam = data[1];
            break;
        }

        case GATTC_WRITE_CMD_IND:
        {
            evt_num = AT_BLE_CHARACTERISTIC_CHANGED;
            gattc_write_cmd_ind(src_id, data, (at_ble_characteristic_changed_t *) params);
            break;
        }

        case GAPC_DISCONNECT_IND:
        {
            evt_num = AT_BLE_DISCONNECTED;
            gapc_disconnect_ind(data, (at_ble_disconnected_t *)params);
            at_ble_reset_indi_noti_record(); 
            break;
        }

        case GAPC_BOND_IND:
        {
            evt_num = gapc_bond_ind(src_id, data, (at_ble_pair_done_t *)params);
            break;
        }

        case GAPC_BOND_REQ_IND:
        {
            evt_num = gapc_bond_req_ind(src_id, data, params);
            break;
        }

        case GAPC_CMP_EVT:
        {
            evt_num = gapc_cmp_evt(src_id, data, (at_ble_encryption_status_changed_t *)params);
            break;
        }

        case GAPC_ENCRYPT_IND:
        {
            if (device.role == ROLE_SLAVE)
            {
                evt_num = AT_BLE_ENCRYPTION_STATUS_CHANGED;
                gapc_encrypt_ind(src_id, data, (at_ble_encryption_status_changed_t *)params);
            }

            break;
        }

        case GAPC_ENCRYPT_REQ_IND:
        {
            evt_num = AT_BLE_ENCRYPTION_REQUEST;
            gapc_encrypt_req_ind(src_id, data, (at_ble_encryption_request_t *)params);
            break;
        }

        case GAPC_CON_RSSI_IND:
        {
            evt_num = AT_BLE_RX_POWER_VALUE;
            gapc_con_rssi_ind_parser(src_id, data, (gapc_con_rssi_ind * )params);
            break;
        }

        case GAPC_CON_MAX_PA_GAIN_GET_IND:
        {
            evt_num = AT_BLE_MAX_PA_GAIN_VALUE;
            gapc_con_max_PA_gain_get_ind_parser(src_id, data, (gapc_con_max_PA_gain_get_ind * )params);
            break;
        }

        case GAPC_CON_TX_POW_GET_IND:
        {
            evt_num = AT_BLE_TX_POWER_VALUE;
            gapc_con_tx_pow_get_ind_parser(src_id, data, (gapc_con_tx_pow_get_ind * )params);
            break;
        }

        case GAPC_CON_TX_POW_SET_IND:
        {
            evt_num = AT_BLE_TX_POWER_SET;
            gapc_con_tx_pow_set_ind_parser(src_id, data, (gapc_con_tx_pow_set_ind * )params);
            break;
        }

        case GATTC_CMP_EVT:
        {
            evt_num = gattc_complete_evt_handler(src_id, data, (at_ble_discovery_complete_t *)params);
            break;
        }

        case GATTC_DISC_SVC_IND:
        {
            evt_num = AT_BLE_PRIMARY_SERVICE_FOUND;
            gattc_disc_svc_ind_parser(src_id, data, (at_ble_primary_service_found_t *)params);
            break;
        }

        case GATTC_DISC_SVC_INCL_IND:
        {
            evt_num = AT_BLE_INCLUDED_SERVICE_FOUND;
            gattc_disc_svc_incl_ind_parser(src_id, data, (at_ble_included_service_found_t *)params);
            break;
        }

        case GATTC_DISC_CHAR_IND:
        {
            evt_num = AT_BLE_CHARACTERISTIC_FOUND;
            gattc_disc_char_ind_parser(src_id, data, (at_ble_characteristic_found_t *)params);
            break;
        }

        case GATTC_DISC_CHAR_DESC_IND:
        {
            evt_num = AT_BLE_DESCRIPTOR_FOUND;
            gattc_disc_char_desc_ind_parser(src_id, data, (at_ble_descriptor_found_t *)params);
            break;
        }

        case GATTC_READ_IND:
        {
            evt_num = AT_BLE_CHARACTERISTIC_READ_RESPONSE;
            gattc_read_ind_parser(src_id, data, (at_ble_characteristic_read_response_t *)params);
            break;
        }

        case GATTC_EVENT_IND:
        {
            evt_num = gattc_event_ind_parser(src_id, data, params);
            break;
        }

        case GAPC_SECURITY_IND:
        {
            evt_num = AT_BLE_SLAVE_SEC_REQUEST;
            gapc_sec_req_ind(src_id, data, (at_ble_slave_sec_request_t *)params);
            break;
        }

        case GAPM_DEV_BDADDR_IND:
        {
            evt_num = AT_BLE_RAND_ADDR_CHANGED;
            gapm_dev_bdaddr_ind_handler(data, (at_ble_rand_addr_changed_t *)params);
            break;
        }

        case GAPM_ADDR_SOLVED_IND:
        {
            evt_num = AT_BLE_RESOLV_RAND_ADDR_STATUS;
            gapm_addr_solved_ind_handler(data, (at_ble_resolv_rand_addr_status_t *)params);
            break;
        }

        case WIFIPROV_SCAN_MODE_CHANGE_IND:
        {
            evt_num = AT_BLE_WIFIPROV_SCAN_MODE_CHANGE_IND;
            wifiprov_scan_mode_change_ind_handler(data, (at_ble_wifiprov_scan_mode_change_ind_t *)params);
            break;
        }

        case WIFIPROV_COMPLETE_IND:
        {
            evt_num = AT_BLE_WIFIPROV_COMPLETE_IND;
            wifiprov_complete_ind_handler(data, (at_ble_wifiprov_complete_ind *)params);
            break;
        }

        default:
        {
            //printf("unhandled:\n\tsrc_id:0x%04x\n\tmsg_id:0x%04x\n\tdata  :0x%02X,0x%02X\n", src_id, msg_id, data[0], data[1]);
            break;
        }
    }

    return evt_num;
}

uint32_t special_events_handler(uint16_t msg_id, uint16_t src_id, uint8_t *data)
{
    uint32_t consumed = 0;

    if (msg_id == watched_event.msg_id
            && src_id == watched_event.src_id)
    {
        watched_event.msg_id = 0xFFFF;
        watched_event.params = data;
        platform_cmd_cmpl_signal();

        consumed = 1;
    }
    else if (msg_id == GAPC_CONNECTION_REQ_IND)
    {
        gapc_con_req_ind(data);
        consumed = 1;
    }

    return consumed;
}

at_ble_status_t at_ble_event_get(at_ble_events_t *event, void *params)
{
    uint16_t msg_id = 0;
    uint16_t src_id = 0;
    uint8_t *data = NULL;
    at_ble_status_t status;

    status = event_get(&msg_id, &src_id, &data);

    if (status == AT_BLE_SUCCESS)
    {
        if (msg_id == 0xFFFF && src_id == 0xFFFF)
        {
            *event = AT_BLE_CUSTOM_EVENT;
        }
        else
        {
            *event = handle_ble_event(msg_id, src_id, data, params);
        }
    }

    return status;
}

at_ble_status_t at_ble_event_user_defined_post(void *params)
{
    event_post(0xFFFF, 0xFFFF, params);
    return AT_BLE_SUCCESS;
}