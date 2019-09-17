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
#include "htpt_task.h"
#include "cmn_defs.h"

// A set of variables to hold the state of htpt
// Used to conform with BLE 4.1 while running BLE 4.0 underneath , this code is ugly but should be fixed in Release 4.1
typedef struct
{
    at_ble_htpt_sec_level sec_lvl;
    at_ble_handle_t conn_handle;
    uint16_t meas_intv;
}htpt_env;

htpt_env htpt;

at_ble_status_t at_ble_htpt_create_db(at_ble_htpt_db_config_flag features,
                                      at_ble_htpt_temp_type  temp_type,
                                      uint16_t valid_range_min,
                                      uint16_t valid_range_max,
                                      uint16_t meas_intv,
                                      at_ble_htpt_sec_level sec_lvl
                                      )
{
    if (valid_range_min < 1)
        return AT_BLE_FAILURE;

    //Conform with API 4.0 ///
    htpt.sec_lvl = sec_lvl;
    htpt.meas_intv = meas_intv;
    //////////////////

    return htpt_create_db_req_handler( temp_type,
                                    valid_range_min,
                                    valid_range_max,
                                    features);
}

at_ble_status_t at_ble_htpt_enable(     at_ble_handle_t conn_handle,
                                        at_ble_htpt_ntf_ind_cfg ntf_ind_cfg
                                        )
{
    //Conform with API 4.0 ///
    uint8_t sec_lvl = 0;
    uint8_t conn_type = PRF_CON_NORMAL; //Always a normal connection to allow setting characterstics properties
    uint8_t temp_meas_ind_en = (ntf_ind_cfg & 0x0001);
    uint8_t interm_temp_ntf_en = (ntf_ind_cfg & 0x0002);
    uint8_t meas_intv_ind_en = (ntf_ind_cfg & 0x0004);
    uint16_t meas_intv = htpt.meas_intv;
    htpt.conn_handle = conn_handle;
    switch(htpt.sec_lvl){
        case HTPT_DISABLE:
            sec_lvl = (((PERM_RIGHT_DISABLE) << (PERM_ACCESS_SVC)) & (PERM_ACCESS_MASK_SVC));
            break;
        case HTPT_ENABLE:
            sec_lvl =  (((PERM_RIGHT_ENABLE) << (PERM_ACCESS_SVC)) & (PERM_ACCESS_MASK_SVC));
            break;
        case HTPT_UNAUTH:
            sec_lvl = (((PERM_RIGHT_UNAUTH) << (PERM_ACCESS_SVC)) & (PERM_ACCESS_MASK_SVC));
            break;
        case HTPT_AUTH:
            sec_lvl = (((PERM_RIGHT_AUTH) << (PERM_ACCESS_SVC)) & (PERM_ACCESS_MASK_SVC));
            break;
    }

    /////////////////////////////////////////
    return htpt_enable_req_handler(conn_handle,
                                         sec_lvl,
                                         conn_type,
                                         temp_meas_ind_en,
                                         interm_temp_ntf_en,
                                         meas_intv_ind_en,
                                         meas_intv);
}

at_ble_status_t at_ble_htpt_temp_send(  uint32_t temp,
                                        at_ble_prf_date_time_t* time_stamp,
                                        at_ble_htpt_temp_flags  flags,
                                        at_ble_htpt_temp_type  type,
                                        bool flag_stable_meas)
{
    return htpt_temp_send_req_handler(          temp,
                                                time_stamp,
                                                flags,
                                                type,
                                                htpt.conn_handle,
                                                flag_stable_meas);
}

at_ble_status_t at_ble_htpt_meas_intv_update(uint16_t meas_intv)
{
    return htpt_meas_intv_upd_req_handler(   htpt.conn_handle,
                                         meas_intv);
}
// DOM-IGNORE-END
