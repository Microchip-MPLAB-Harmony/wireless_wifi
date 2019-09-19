// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) RivieraWaves 2009-2014
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#include "at_ble_api.h"

#include "platform.h"
#include "gattc_task.h"
#include "gattm_task.h"
#include "gapm_task.h"
#include "gapc_task.h"
#include "dbg_task.h"
#include "drv/ble_stack/ble_device.h"
#include "event.h"

// Slave preferred Connection interval Min
#define APP_CON_INTV_MIN    8
// Slave preferred Connection interval Max
#define APP_CON_INTV_MAX    10
// Slave preferred Connection latency
#define APP_CON_LATENCY     0
// Slave preferred Link supervision timeout
#define APP_SUPERV_TO       200 // 2s (500*10ms)

#define APP_MAX_MTU         L2C_MIN_LE_MTUSIG
#define APP_FLAGS           0

#define LOCAL_ADDR_TYPE GAPM_PUBLIC_ADDR
#define GAP_device_name "MICROCHIP_BLE"

struct device_info device;

at_ble_status_t at_ble_init(void* args)
{
    at_ble_status_t  status;

    status = platform_init(args);

    if(status == AT_BLE_SUCCESS)
    {
        event_init();
        if(gapm_reset_req_handler() != ATT_ERR_NO_ERROR)
        {
            status = AT_BLE_FAILURE;
        }
    }

    // init device info
    if(status == AT_BLE_SUCCESS)
    {
        uint8_t loopCntr;
        device.srLen = 0;
        device.advLen = 0;
        device.role = ROLE_SLAVE;
        device.at_addr_set = false;
        device.privacy_flags = 0;
        device.addr_auto_gen = false;
        device.conn_handle = 0xFFFF;
        device.appearance = 0;
        device.spcp_param.con_intv_max = APP_CON_INTV_MIN;
        device.spcp_param.con_intv_min = APP_CON_INTV_MAX;
        device.spcp_param.con_latency = APP_CON_LATENCY;
        device.spcp_param.superv_to = APP_SUPERV_TO;
        device.dev_name_write_perm = GAPM_WRITE_DISABLE;
        device.renew_dur = GAP_TMR_PRIV_ADDR_INT;
        at_ble_device_name_set((uint8_t *)GAP_device_name,sizeof(GAP_device_name));
        for(loopCntr=0; loopCntr<AT_BLE_MAX_KEY_LEN; loopCntr++)
        {
            device.irk.key[loopCntr] = loopCntr;
        }
    }
    return status;
}

at_ble_status_t at_ble_set_privacy_key(at_ble_gap_irk_t* irk , uint16_t interval)
{
    if(interval == 0)
    {
        return AT_BLE_INVALID_PARAM;
    }
    else
    {
        device.renew_dur = interval;
    }
    if(irk != NULL)
    {
        memcpy(device.irk.key ,irk->irk , AT_BLE_MAX_KEY_LEN);
    }
    return AT_BLE_SUCCESS;
}

at_ble_status_t at_ble_set_dev_config(at_ble_gap_role role)
{
    gapm_set_dev_config_cmd_handler(role, device.irk.key,
    device.appearance, GAPM_WRITE_DISABLE, device.dev_name_write_perm, APP_MAX_MTU,
    device.spcp_param.con_intv_min, device.spcp_param.con_intv_max, device.spcp_param.con_latency,
    device.spcp_param.superv_to, device.privacy_flags);

    return AT_BLE_SUCCESS;
}

at_ble_status_t at_ble_set_gap_deviceinfo(at_ble_gap_deviceinfo_t*  gap_deviceinfo )
{
    if((gap_deviceinfo == NULL) || ((gap_deviceinfo->dev_name_perm) >AT_BLE_WRITE_AUTH))
    {
        return AT_BLE_INVALID_PARAM;
    }
    else
    {
        memcpy(&(device.spcp_param),&(gap_deviceinfo->spcp_param), sizeof(at_ble_spcp_t));
        device.appearance = gap_deviceinfo->appearance ;

        switch(gap_deviceinfo->dev_name_perm)
        {
        case AT_BLE_WRITE_DISABLE:
                device.dev_name_write_perm = GAPM_WRITE_DISABLE;
            break;
        case AT_BLE_WRITE_ENABLE:
            device.dev_name_write_perm = GAPM_WRITE_ENABLE;
            break;
        case AT_BLE_WRITE_UNAUTH:
            device.dev_name_write_perm = GAPM_WRITE_UNAUTH;
            break;
        case AT_BLE_WRITE_AUTH:
            device.dev_name_write_perm = GAPM_WRITE_AUTH;
            break;
        }
    }
    return AT_BLE_SUCCESS;
}

at_ble_status_t at_ble_device_name_set(uint8_t* dev_name, uint8_t len)
{
    at_ble_status_t status = AT_BLE_SUCCESS;
    do
    {
        if((dev_name == NULL) ||(len == 0))
        {
            status = AT_BLE_INVALID_PARAM;
            break;
        }

        if(gapm_set_dev_name_handler (len,dev_name) != ATT_ERR_NO_ERROR)
        {
            status = AT_BLE_FAILURE;
        }
    }while(0);

    return status;
}

at_ble_status_t at_ble_addr_set(at_ble_addr_t* address)
{
    uint8_t u8Status;
    at_ble_status_t status = AT_BLE_SUCCESS;

    do
    {
        if((address->addr == NULL) ||
            (address->type >AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE))
        {
            status = AT_BLE_INVALID_PARAM;
            break;
        }
        if(address->type == AT_BLE_ADDRESS_PUBLIC)
        {
            u8Status = dbg_le_set_bd_addr_req_handler(address->addr);

            if(u8Status!=AT_BLE_SUCCESS)
            {
                status = AT_BLE_FAILURE;
                break;
            }
            memcpy(device.at_dev_addr.addr, address->addr ,AT_BLE_ADDR_LEN);
        }
        else if (address->type == AT_BLE_ADDRESS_RANDOM_STATIC)
        {
            if(address->addr != NULL)
            {
                memcpy(device.at_dev_addr.addr, address->addr ,AT_BLE_ADDR_LEN);
            }
            else
            {
                device.addr_auto_gen = true;
            }
        }
        else if ((address->type == AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE)
            ||(address->type == AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE))
        {
            device.privacy_flags = 1;
        }

        device.at_addr_set = true;
        device.at_dev_addr.type = address->type;
    }while(0);
    return status;
}

at_ble_status_t at_ble_addr_get(at_ble_addr_t* address)
{
    uint8_t u8Status;
    at_ble_status_t status = AT_BLE_SUCCESS;

    if(address == NULL)
    {
        return AT_BLE_INVALID_PARAM;
    }

    u8Status = dbg_le_get_bd_addr_req_handler(address->addr);

    if(u8Status!=AT_BLE_SUCCESS)
    {
        status = AT_BLE_FAILURE;
    }

    address->type = AT_BLE_ADDRESS_PUBLIC;
    return status;
}

at_ble_status_t at_ble_adv_data_set( uint8_t const *const adv_data,
    uint8_t adv_data_len,uint8_t const *const scan_resp_data,
    uint8_t scan_response_data_len)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    do
    {
        if((adv_data== NULL) && (scan_resp_data == NULL))
        {
            status = AT_BLE_FAILURE;
            break;
        }
        if((adv_data_len > AT_BLE_ADV_MAX_SIZE) ||
            (scan_response_data_len > AT_BLE_ADV_MAX_SIZE))
        {
            status = AT_BLE_INVALID_PARAM;
            break;
        }

        if((adv_data != NULL) && (adv_data_len == 0) )
        {
            // Clear advertising data
            memset(device.ADVData,0, AT_BLE_ADV_MAX_SIZE);
            device.advLen =  0;
        }
        else if((adv_data != NULL) && (adv_data_len > 0))
        {
            // To do add advertising data check
            memcpy(device.ADVData,adv_data, adv_data_len);
            device.advLen =  adv_data_len;
        }

        if(scan_resp_data != NULL && scan_response_data_len == 0 )
        {
            // Clear scan response data
            memset(device.SrData,0, AT_BLE_ADV_MAX_SIZE);
            device.srLen = 0;
        }
        else if((scan_resp_data != NULL) && (scan_response_data_len > 0))
        {
            // To do add scan response data check
            memcpy(device.SrData, scan_resp_data,scan_response_data_len);
            device.srLen = scan_response_data_len;
        }
    }while(0);
    return status;
}

at_ble_status_t at_ble_adv_start(at_ble_adv_type_t type,at_ble_adv_mode_t mode,
                at_ble_addr_t* peer_addr,at_ble_filter_type_t filtered,
                uint16_t interval, uint16_t timeout, bool disable_randomness)
{
    uint8_t adv_type,gatt_dev_addr , peer_addr_type = 0;
    at_ble_addr_t local_addr;
    uint8_t scan_rsp_len = device.srLen;

    at_ble_status_t status = AT_BLE_SUCCESS;

    // To do add timeout value for GAP
    do
    {
        if(((type != AT_BLE_ADV_TYPE_DIRECTED) &&
           (device.at_dev_addr.type == AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE))
           ||(((type == AT_BLE_ADV_TYPE_DIRECTED) &&
           (device.at_dev_addr.type == AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE))))
        {
            status = AT_BLE_PRIVACY_CFG_PB;
            break;
        }

        //If the advertising event type is either a discoverable undirected
        //event type or a non-connectable undirected event type, the advInterval shall
        //not be less than 100 ms. If the advertising event type is a connectable undirected
        //event type, the advInterval can be 20 ms or greater.
        if(((type == AT_BLE_ADV_TYPE_DIRECTED)&&(timeout != 0))
        ||(type > AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED)
        ||(interval > AT_BLE_ADV_INTERVAL_MAX)
        ||((type == AT_BLE_ADV_TYPE_UNDIRECTED)&&(interval < AT_BLE_ADV_INTERVAL_MIN))
        ||((interval < AT_BLE_ADV_NONCON_INTERVAL_MIN) &&
        ((type >= AT_BLE_ADV_TYPE_SCANNABLE_UNDIRECTED))) )
        {
             status = AT_BLE_INVALID_PARAM;
             break;
        };

        if((type == AT_BLE_ADV_TYPE_DIRECTED)&&
        (!(peer_addr->addr[0] || peer_addr->addr[1] || peer_addr->addr[2]
        || peer_addr->addr[3]|| peer_addr->addr[4] || peer_addr->addr[5])))
        {
             status = AT_BLE_INVALID_PARAM;
             break;
        }

        if((AT_BLE_ADV_TYPE_UNDIRECTED == type) && (AT_BLE_ADV_BROADCASTER_MODE == mode))
        {
            status = AT_BLE_INVALID_PARAM;
            break;
        }

        status = at_ble_addr_get(&local_addr);
        if(status!=AT_BLE_SUCCESS)
        {
            status = AT_BLE_FAILURE;
            break;
        }
        switch(type)
        {
            default:
            case  AT_BLE_ADV_TYPE_UNDIRECTED :
                adv_type = GAPM_ADV_UNDIRECT;
                break;
            case  AT_BLE_ADV_TYPE_DIRECTED:
                adv_type = GAPM_ADV_DIRECT;
                break;
            /* differentiate between them by using scan response length*/
            case AT_BLE_ADV_TYPE_NONCONN_UNDIRECTED:
                scan_rsp_len = 0;
            case AT_BLE_ADV_TYPE_SCANNABLE_UNDIRECTED:
                adv_type = GAPM_ADV_NON_CONN;
                break;
        }

        /*Own BD address source of the device */
        switch(local_addr.type)
        {
        case AT_BLE_ADDRESS_PUBLIC:
            gatt_dev_addr = GAPM_PUBLIC_ADDR;
            break;
        case AT_BLE_ADDRESS_RANDOM_STATIC:
            if(device.addr_auto_gen == true)
            {
                gatt_dev_addr = GAPM_PROVIDED_RND_ADDR;
            }
            else
            {
                gatt_dev_addr = GAPM_GEN_STATIC_RND_ADDR;
            }
            break;
        case AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE:
            gatt_dev_addr = GAPM_GEN_RSLV_ADDR;
            break;
        case AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE:
            gatt_dev_addr = GAPM_PROVIDED_RECON_ADDR;
            break;
        case AT_BLE_ADDRESS_INVALID:
        default:
            gatt_dev_addr = GAPM_INVALID_ADDR;
            break;
        }
        if(peer_addr != NULL)
        {
            if(peer_addr->type != AT_BLE_ADDRESS_PUBLIC)
            {
                peer_addr_type = 1;
            }
            else
            {
                peer_addr_type = 0;
            }
        }
        gapm_start_adv_cmd_handler(adv_type, gatt_dev_addr,
            device.renew_dur, local_addr.addr,peer_addr_type,(peer_addr==NULL ? NULL : peer_addr->addr),
            interval, interval, 7, mode, filtered,
            device.advLen, device.ADVData, scan_rsp_len, device.SrData);

    }while(0);
    return status;
}

at_ble_status_t at_ble_adv_stop(void)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    if(gapm_cancel_cmd_handler())
    {
        status = AT_BLE_FAILURE;
    }
    return status;
}

at_ble_status_t at_ble_adv_set_tx_power(int8_t power)
{
    // NOT IMPLEMENTED
    return AT_BLE_FAILURE;
}

at_ble_status_t at_ble_scan_start(uint16_t interval,uint16_t window,uint16_t timeout,
    at_ble_scan_type_t type ,at_ble_scan_mode_t mode, bool filter_whitelist,
    bool filter_duplicates)
{
    uint8_t scan_type, gatt_dev_addr;
    at_ble_addr_t local_addr;
    at_ble_status_t status = AT_BLE_SUCCESS;

    // To do support scan timeout
    do
    {
        // if the scanning window is smaller or equal to the scanning interval
        if ((mode > AT_BLE_SCAN_OBSERVER_MODE)
        ||(window > interval)
        ||(type > AT_BLE_SCAN_ACTIVE)
        ||(window > AT_BLE_SCAN_WINDOW_MAX)
        ||(window < AT_BLE_SCAN_WINDOW_MIN)
        ||(interval > AT_BLE_SCAN_INTERVAL_MAX)
        ||(interval < AT_BLE_SCAN_INTERVAL_MIN))
        {
             status = AT_BLE_INVALID_PARAM;
             break;
        };

        status = at_ble_addr_get(&local_addr);
        if(status!=AT_BLE_SUCCESS)
        {
            status = AT_BLE_FAILURE;
            break;
        }
        if(type == AT_BLE_SCAN_PASSIVE)
        {
            scan_type = GAPM_SCAN_PASSIVE;
        }
        else
        {
            scan_type = GAPM_SCAN_ACTIVE;
        }

        /*Own BD address source of the device */
        switch(local_addr.type)
        {
        case AT_BLE_ADDRESS_PUBLIC:
            gatt_dev_addr = GAPM_PUBLIC_ADDR;
            break;
        case AT_BLE_ADDRESS_RANDOM_STATIC:
            if(device.addr_auto_gen == true)
            {
                gatt_dev_addr = GAPM_PROVIDED_RND_ADDR;
            }
            else
            {
                gatt_dev_addr = GAPM_GEN_STATIC_RND_ADDR;
            }
            break;
        case AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE:
            gatt_dev_addr = GAPM_GEN_RSLV_ADDR;
            break;
        case AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE:
            gatt_dev_addr = GAPM_GEN_NON_RSLV_ADDR;
            break;
        case AT_BLE_ADDRESS_INVALID:
        default:
            gatt_dev_addr = GAPM_INVALID_ADDR;
            break;
        }

        gapm_start_scan_cmd_handler(scan_type, gatt_dev_addr,
            device.renew_dur,
            local_addr.addr, interval, window, mode,
            filter_whitelist, filter_duplicates);

    }while(0);
    return status;
}

at_ble_status_t at_ble_scan_stop(void)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    if(gapm_cancel_cmd_handler())
    {
        status = AT_BLE_FAILURE;
    }
    return status;
}

at_ble_status_t at_ble_connect(at_ble_addr_t peers[], uint8_t peer_count,
    uint16_t scan_interval, uint16_t scan_window, at_ble_connection_params_t* connection_params)
{

    at_ble_status_t status = AT_BLE_SUCCESS;
    at_ble_addr_t local_addr;
    uint8_t gatt_dev_addr;
    do
    {
        /* check for the range validity of the values */
        if ( (connection_params->con_intv_max < AT_CNX_INTERVAL_MIN ||
        connection_params->con_intv_max > AT_CNX_INTERVAL_MAX) ||
        (connection_params->con_intv_min < AT_CNX_INTERVAL_MIN ||
        connection_params->con_intv_min > AT_CNX_INTERVAL_MAX) ||
        (connection_params->superv_to  < AT_CNX_SUP_TO_MIN   ||
        connection_params->superv_to  > AT_CNX_SUP_TO_MAX)   ||
        (connection_params->con_latency  > AT_CNX_LATENCY_MAX) )
        {
            status = AT_BLE_INVALID_PARAM;
            break;
        }

        device.role = ROLE_MASTER;

        status = at_ble_addr_get(&local_addr);
        if(status!=AT_BLE_SUCCESS)
        {
            status = AT_BLE_FAILURE;
            break;
        }

        /*Own BD address source of the device */
        switch(local_addr.type)
        {
        case AT_BLE_ADDRESS_PUBLIC:
            gatt_dev_addr = GAPM_PUBLIC_ADDR;
            break;
        case AT_BLE_ADDRESS_RANDOM_STATIC:
            if(device.addr_auto_gen == true)
            {
                gatt_dev_addr = GAPM_PROVIDED_RND_ADDR;
            }
            else
            {
                gatt_dev_addr = GAPM_GEN_STATIC_RND_ADDR;
            }
            break;
        case AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE:
            gatt_dev_addr = GAPM_GEN_RSLV_ADDR;
            break;
        case AT_BLE_ADDRESS_RANDOM_PRIVATE_NON_RESOLVABLE:
            gatt_dev_addr = GAPM_GEN_NON_RSLV_ADDR;
            break;
        case AT_BLE_ADDRESS_INVALID:
        default:
            gatt_dev_addr = GAPM_INVALID_ADDR;
            break;
        }

        gapm_start_connection_cmd_handler(GAPM_CONNECTION_AUTO,gatt_dev_addr,
            device.renew_dur,local_addr.addr , scan_interval, scan_window,
            connection_params->con_intv_min, connection_params->con_intv_max,
            connection_params->con_latency,
            connection_params->superv_to,
            connection_params->ce_len_min,
            connection_params->ce_len_max,
            peer_count, peers);

    }while(0);
    return status;
}

at_ble_status_t at_ble_connect_cancel(void)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    if(gapm_cancel_cmd_handler())
    {
        status = AT_BLE_FAILURE;
    }
    return status;
}

at_ble_status_t at_ble_sec_info_set(at_ble_handle_t handle, at_ble_signature_info_t signature_info, at_ble_auth_t auth, at_ble_author_t author)
{
    gapc_connection_cfm_handler(signature_info.local_csrk.key, signature_info.lsign_counter,
        signature_info.peer_csrk.key, signature_info.peer_sign_counter, auth,
                    author, handle);
    return AT_BLE_SUCCESS;
}

at_ble_status_t at_ble_whitelist_add(at_ble_addr_t* address)
{
    at_ble_status_t status= AT_BLE_SUCCESS;
    if(address->addr == NULL)
    {
        return AT_BLE_FAILURE;
    }

    status = gapm_white_list_mgm_cmd(GAPM_ADD_DEV_IN_WLIST,
        address->type, address->addr);

    return status;
}

at_ble_status_t at_ble_whitelist_remove(at_ble_addr_t* address)
{
    at_ble_status_t status= AT_BLE_SUCCESS;
    if(address->addr == NULL)
    {
        return AT_BLE_FAILURE;
    }

    status = gapm_white_list_mgm_cmd(GAPM_RMV_DEV_FRM_WLIST,
        address->type, address->addr);

    return status;
}

at_ble_status_t at_ble_whitelist_clear(void)
{
    at_ble_status_t status= AT_BLE_SUCCESS;
    status= gapm_white_list_mgm_cmd(GAPM_CLEAR_WLIST,
        0, NULL);

    return status;
}

at_ble_status_t at_ble_disconnect(at_ble_handle_t handle, at_ble_disconnect_reason_t reason)
{
    uint8_t gapc_reason ;
    switch(reason)
    {
        case AT_BLE_TERMINATED_BY_USER:
            gapc_reason = 0x13;
            break;
        case AT_BLE_UNACCEPTABLE_INTERVAL:
            gapc_reason = 0x3b;
            break;
        default:
            gapc_reason = 0x1F;
    }
    gapc_disconnect_cmd_handler(gapc_reason, handle);
    device.conn_handle = 0xFFFF;

    return AT_BLE_SUCCESS;
}

at_ble_status_t at_ble_connection_param_update(at_ble_handle_t handle,
    at_ble_connection_params_t* connection_params)
{
    /* check for the range validity of the values */
    if ( (connection_params->con_intv_max < AT_CNX_INTERVAL_MIN ||
    connection_params->con_intv_max > AT_CNX_INTERVAL_MAX) ||
    (connection_params->con_intv_min < AT_CNX_INTERVAL_MIN ||
    connection_params->con_intv_min > AT_CNX_INTERVAL_MAX) ||
    (connection_params->superv_to  < AT_CNX_SUP_TO_MIN   ||
    connection_params->superv_to  > AT_CNX_SUP_TO_MAX)   ||
    (connection_params->con_latency  > AT_CNX_LATENCY_MAX) )
    {
        return AT_BLE_INVALID_PARAM;
    }

    gapc_param_update_cmd_handler(handle,
        connection_params->con_intv_min, connection_params->con_intv_max,
        connection_params->con_latency, connection_params->superv_to,
        connection_params->ce_len_min, connection_params->ce_len_max);

    return AT_BLE_SUCCESS;
}

// To do check parameters
void at_ble_conn_update_reply(at_ble_handle_t conn_handle ,
                at_ble_connection_params_t* connection_params)
{
    gapc_param_update_cfm_handler(conn_handle,
        connection_params->ce_len_min, connection_params->ce_len_max);
}

at_ble_status_t at_ble_tx_power_set(at_ble_handle_t conn_handle, int8_t powerdBm)
{
    at_ble_events_t event;
    uint8_t params[1];
    uint8_t escape = 0;
    uint8_t tx_power_set_status = 0;
    gapc_set_tx_pwr_req_handler(conn_handle, (uint8_t) powerdBm);
    do
    {
        if (at_ble_event_get(&event, params, 500) == AT_BLE_SUCCESS)
        {
            if (event == AT_BLE_TX_POWER_SET)
            {
                // BLE Firmware will return the following status:
                //
                //      0. If the requested dB decrement is 0-18 dB, and pa gain of all channels can be fully
                //          decremented by the requested amount
                //      1. If the requested dB decrement is 0-18 dB, however for some of the channels
                //         the decrement requested is greater than the PA gain value at initialisation.
                //         In these cases the PA gain is reduece to the minimum allowed i.e. 0dB
                //      2. If the requested dB decrement is not 0-18 dB. In this case there
                //         is no change in PA gain will be effected.
                gapc_con_tx_pow_set_ind* p = (gapc_con_tx_pow_set_ind* )params;
                tx_power_set_status = p->status;
                break;
            }
        }

        if(++escape>4)
        {
            tx_power_set_status = AT_BLE_FAILURE;
            break;
        }
    } while(event != AT_BLE_TX_POWER_SET);

    return tx_power_set_status;
}

int8_t at_ble_tx_power_get(at_ble_handle_t conn_handle)
{
    at_ble_events_t event;
    uint8_t params[1];
    uint8_t tx_pw = 0;
    uint8_t escape = 0;
    gapc_get_tx_pwr_req_handler(conn_handle,0);// 0 for current PA gain
    do
    {
        if (at_ble_event_get(&event, params, 500) == AT_BLE_SUCCESS)
        {
            if (event == AT_BLE_TX_POWER_VALUE)
            {
                gapc_con_tx_pow_get_ind* p = (gapc_con_tx_pow_get_ind* )params;
                tx_pw = p->tx_pow_lvl;
                break;
            }
        }
        if(++escape>4)
        {
            tx_pw = 0;
            break;
        }
    }while(event != AT_BLE_TX_POWER_VALUE);
    return (int8_t)tx_pw;
}

int8_t at_ble_max_PA_gain_get(at_ble_handle_t conn_handle)
{
    at_ble_events_t event;
    uint8_t params[1];
    uint8_t pa_gain = 0;
    uint8_t escape = 0;
    gapc_get_tx_pwr_req_handler(conn_handle,1); // 1 for asking MAX allowed PA gain
    do
    {
        if (at_ble_event_get(&event, params, 500) == AT_BLE_SUCCESS)
        {
            if (event == AT_BLE_MAX_PA_GAIN_VALUE)
            {
                gapc_con_max_PA_gain_get_ind* p = (gapc_con_max_PA_gain_get_ind* )params;
                pa_gain = p->max_PA_gain;
                break;
            }
        }

        if(++escape>4)
        {
            pa_gain = 0;
            break;
        }
    }while(event != AT_BLE_MAX_PA_GAIN_VALUE);
    return (int8_t)pa_gain;
}

at_ble_status_t at_ble_random_address_resolve(uint8_t nb_key, at_ble_addr_t* rand_addr, uint8_t* irk_key)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    do
    {
        if((rand_addr == NULL) || (irk_key == NULL) ||(nb_key == 0)||
            (rand_addr->type != AT_BLE_ADDRESS_RANDOM_PRIVATE_RESOLVABLE ))
        {
            status = AT_BLE_INVALID_PARAM;
            break;
        }

        gapm_resolv_addr_cmd_handler(nb_key , rand_addr->addr,irk_key);
    }while(0);
    return status;
}

int8_t at_ble_rx_power_get(at_ble_handle_t conn_handle)
{
    at_ble_events_t event;
    uint8_t params[1];
    uint8_t rssi = 0;
    uint8_t escape = 0;
    gapc_get_info_cmd_handler(conn_handle, GAPC_GET_CON_RSSI);
    do
    {
        if (at_ble_event_get(&event, params, 500) == AT_BLE_SUCCESS)
        {
            if (event == AT_BLE_RX_POWER_VALUE)
            {
                gapc_con_rssi_ind* p = (gapc_con_rssi_ind* )params;
                rssi = p->rssi;
                break;
            }
        }

        if(++escape>4)
        {
            rssi = 0;
            break;
        }
    }while(event != AT_BLE_RX_POWER_VALUE);
    return (int8_t)rssi;
}
// DOM-IGNORE-END
