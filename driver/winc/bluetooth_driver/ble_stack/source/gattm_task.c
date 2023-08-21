// DOM-IGNORE-BEGIN
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
/*
 * gattm_task.c
 *
 *  Created on: Sep 16, 2013
 *      Author: delsissy
 */

#include "at_ble_api.h"
#include "platform.h"
#include "interface.h"

#include "gattm_task.h"

uint8_t gattm_add_svc_req_handler(uint16_t *start_hdl, uint16_t total_size, uint8_t nb_att_uuid_16,
        uint8_t nb_att_uuid_32, uint8_t nb_att_uuid_128)
{
    uint8_t u8Status;

    INTERFACE_MSG_INIT(GATTM_ADD_SVC_REQ, TASK_ATTM);
    INTERFACE_PACK_ARG_UINT16(*start_hdl);
    INTERFACE_PACK_ARG_UINT16(TASK_EXTERN);
    INTERFACE_PACK_ARG_UINT16(total_size);
    INTERFACE_PACK_ARG_UINT8(nb_att_uuid_16);
    INTERFACE_PACK_ARG_UINT8(nb_att_uuid_32);
    INTERFACE_PACK_ARG_UINT8(nb_att_uuid_128);
    INTERFACE_SEND_WAIT(GATTM_ADD_SVC_RSP, TASK_ATTM);
    INTERFACE_UNPACK_UINT16(start_hdl);
    INTERFACE_UNPACK_UINT8(&u8Status);
    INTERFACE_MSG_DONE();

    return u8Status;
}

uint8_t gattm_add_attribute_req_handler (uint16_t start_hdl, uint16_t max_data_size, uint16_t perm,
        uint8_t uuid_len, uint8_t *uuid, uint16_t *handle)
{
    uint8_t u8Status;

    INTERFACE_MSG_INIT(GATTM_ADD_ATTRIBUTE_REQ, TASK_ATTM);
    INTERFACE_PACK_ARG_UINT16(start_hdl);
    INTERFACE_PACK_ARG_UINT16(max_data_size);
    INTERFACE_PACK_ARG_UINT16(perm);
    INTERFACE_PACK_ARG_UINT8(uuid_len);
    INTERFACE_PACK_ARG_BLOCK(uuid, uuid_len);
    INTERFACE_SEND_WAIT(GATTM_ADD_ATTRIBUTE_RSP, TASK_ATTM);
    INTERFACE_UNPACK_UINT16(handle);
    INTERFACE_UNPACK_UINT8(&u8Status);
    INTERFACE_MSG_DONE();

    return u8Status;
}

uint8_t gattm_att_set_value_req_handler(uint16_t handle, uint16_t length, uint8_t *value)
{
    uint8_t u8Status;
    uint16_t u16RcvHandle;

    INTERFACE_MSG_INIT(GATTM_ATT_SET_VALUE_REQ, TASK_ATTM);
    INTERFACE_PACK_ARG_UINT16(handle);
    INTERFACE_PACK_ARG_UINT16(length);
    INTERFACE_PACK_ARG_BLOCK(value, length);
    INTERFACE_SEND_WAIT(GATTM_ATT_SET_VALUE_RSP, TASK_ATTM);
    INTERFACE_UNPACK_UINT16(&u16RcvHandle);
    INTERFACE_UNPACK_UINT8(&u8Status);
    INTERFACE_MSG_DONE();

    return u8Status;
}

uint8_t gattm_att_get_value_req_handler(uint16_t handle, uint16_t* length, uint8_t *value)
{
    uint16_t u16RcvHandle;
    uint8_t u8Status;

    INTERFACE_MSG_INIT(GATTM_ATT_GET_VALUE_REQ, TASK_ATTM);
    INTERFACE_PACK_ARG_UINT16(handle);
    INTERFACE_SEND_WAIT(GATTM_ATT_GET_VALUE_RSP, TASK_ATTM);
    INTERFACE_UNPACK_UINT16(&u16RcvHandle);
    INTERFACE_UNPACK_UINT16(length);
    INTERFACE_UNPACK_UINT8(&u8Status);
    INTERFACE_UNPACK_BLOCK(value, *length);
    INTERFACE_MSG_DONE();
    if(handle != u16RcvHandle)
            return AT_BLE_FAILURE;

    return u8Status;
}
// DOM-IGNORE-END
