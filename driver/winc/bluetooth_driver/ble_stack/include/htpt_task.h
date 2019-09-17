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
// DOM-IGNORE-END
/*
 * htpt_task.h
 *
 *  Created on: Feb 23, 2015
 *      Author: aabdelfattah
 */
#ifndef HTPT_TASK_H_
#define HTPT_TASK_H_

#include "cmn_defs.h"
#include "profiles.h"
/*
 * msgs from APP to HTPT
 *
 *
 */

at_ble_status_t htpt_create_db_req_handler(uint8_t  temp_type,
                                            uint16_t valid_range_min,
                                            uint16_t valid_range_max,
                                            uint8_t features);

at_ble_status_t htpt_enable_req_handler( at_ble_handle_t conn_handle,
                                        uint8_t sec_lvl,
                                        uint8_t con_type,
                                        uint16_t temp_meas_ind_en,
                                        uint16_t interm_temp_ntf_en,
                                        uint16_t meas_intv_ind_en,
                                        uint16_t meas_intv);


at_ble_status_t htpt_temp_send_req_handler(uint32_t temp,
                                                at_ble_prf_date_time_t* time_stamp,
                                                uint8_t  flags,
                                                uint8_t  type,
                                                at_ble_handle_t conn_handle,
                                                uint8_t flag_stable_meas);



at_ble_status_t htpt_meas_intv_upd_req_handler( at_ble_handle_t conn_handle,
                                        uint16_t meas_intv);
at_ble_status_t htpt_temp_type_upd_req_handler( at_ble_handle_t conn_handle,
                                               uint8_t value);

/*
 * events from HTPT to APP
 *
 *
 */
void htpt_create_db_cfm_handler(uint16_t src,uint8_t *data,at_ble_htpt_create_db_cfm_t *param);
void htpt_disable_ind_handler(uint16_t src,uint8_t *data,at_ble_htpt_disable_ind_t *param);
void htpt_error_ind_handler(uint16_t src,uint8_t *data, at_ble_prf_server_error_ind_t  *param);
void htpt_temp_send_cfm_handler(uint16_t src, uint8_t *data,at_ble_htpt_temp_send_cfm_t *param);
void htpt_meas_intv_chg_ind_handler(uint16_t src,uint8_t *data,at_ble_htpt_meas_intv_chg_ind_t *param);
void htpt_cfg_indntf_ind_handler(uint16_t src,uint8_t *data,at_ble_htpt_cfg_indntf_ind_t *param);

/// Messages for Health Thermometer Profile Thermometer
enum
{
    ///Add a HTS instance into the database
    HTPT_CREATE_DB_REQ = 20480,
    ///inform APP of database creation status
    HTPT_CREATE_DB_CFM,

    ///Start the Health Thermometer Profile Thermometer profile - at connection
    HTPT_ENABLE_REQ,

    ///Automatically sent to the APP after a disconnection with the peer device
    HTPT_DISABLE_IND,

    /// Error indication to Host
    HTPT_ERROR_IND,

    ///Send temperature value from APP
    HTPT_TEMP_SEND_REQ,
    ///Send temperature value confirm to APP so stable values can be erased if correctly sent.
    HTPT_TEMP_SEND_CFM,

    ///Indicate Measurement Interval
    HTPT_MEAS_INTV_UPD_REQ,
    ///Inform APP of new measurement interval value
    HTPT_MEAS_INTV_CHG_IND,

    ///Inform APP of new configuration value
    HTPT_CFG_INDNTF_IND,

    ///Update Temperature Type Value
    HTPT_TEMP_TYPE_UPD_REQ,
};

/// Parameters of the @ref HTPT_CREATE_DB_REQ message
/*struct htpt_create_db_req
{
    ///Measurement Interval Valid Range - Minimal Value
    uint16_t valid_range_min;
    ///Measurement Interval Valid Range - Maximal Value
    uint16_t valid_range_max;

    ///Database configuration
    uint8_t features;
};*/

/// Parameters of the @ref HTPT_CREATE_DB_CFM message
/*struct htpt_create_db_cfm
{
    ///Status
    uint8_t status;
};*/

/// Parameters of the @ref HTPT_ENABLE_REQ message
/*struct htpt_enable_req
{
    ///Connection handle
    uint16_t conhdl;
    /// security level: b0= nothing, b1=unauthenticated, b2=authenticated, b3=authorized; b1 or b2 and b3 can go together
    uint8_t sec_lvl;
    ///Type of connection - will someday depend on button press length; can be CFG or NORMAL
    uint8_t con_type;

    ///Temperature measurement indication configuration
    uint16_t temp_meas_ind_en;
    ///Intermediate temperature notification configuration
    uint16_t interm_temp_ntf_en;
    ///Measurement interval indication configuration
    uint16_t meas_intv_ind_en;
    ///Measurement interval
    uint16_t meas_intv;
};*/

///Parameters of the @ref HTPT_DISABLE_IND message
/*struct htpt_disable_ind
{
    ///Connection handle
    uint16_t conhdl;

    ///Temperature measurement indication configuration
    uint16_t temp_meas_ind_en;
    ///Intermediate temperature notification configuration
    uint16_t interm_temp_ntf_en;
    ///Measurement interval indication configuration
    uint16_t meas_intv_ind_en;
    ///Measurement interval
    uint16_t meas_intv;
};*/

///Temperature Measurement Value Structure
/*struct htp_temp_meas
{
    ///Temp
    uint32_t temp;
    ///Time stamp
    struct prf_date_time time_stamp;
    ///Flag
    uint8_t  flags;
    ///Type
    uint8_t  type;
};*/

///Parameters of the @ref HTPT_TEMP_UPD_REQ message
/*struct htpt_temp_send_req
{
    ///Temperature Measurement
    struct htp_temp_meas temp_meas;
    ///Connection handle
    uint16_t conhdl;
    ///Stable or intermediary type of temperature
    uint8_t flag_stable_meas;
};*/

///Parameters of the @ref HTPT_TEMP_UPD_CFM message
/*struct htpt_temp_send_cfm
{
    ///Connection handle
    uint16_t conhdl;
    ///Status
    uint8_t status;
    ///Confirmation Type
    uint8_t cfm_type;
};*/

///Parameters of the @ref HTPT_MEAS_INTV_UPD_REQ message
/*struct htpt_meas_intv_upd_req
{
    ///Connection handle
    uint16_t conhdl;
    ///Measurement Interval value
    uint16_t meas_intv;
};*/

///Parameters of the @ref HTPT_MEAS_INTV_CHG_IND message
/*struct htpt_meas_intv_chg_ind
{
    uint16_t intv;
};*/

///Parameters of the @ref HTPT_CFG_INDNTF_IND message
/*struct htpt_cfg_indntf_ind
{
    ///Connection handle
    uint16_t conhdl;
    ///Stop/notify/indicate value to configure into the peer characteristic
    uint16_t cfg_val;
    ///Own code for differentiating between Temperature Measurement, Intermediate Temperature and Measurement Interval chars
    uint8_t char_code;
};*/

///Parameters of the @ref HTPT_TEMP_TYPE_UPD_REQ message
/*struct htpt_temp_type_upd_req
{
    ///Temperature Type Value
    uint8_t value;
};*/

#endif // HTPT_TASK_H_
