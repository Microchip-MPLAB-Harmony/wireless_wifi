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

#include "at_ble_api.h"

#include "platform.h"
#include "gattc_task.h"
#include "ble_device.h"

static uint8_t at_ble_uuid_type2len(at_ble_uuid_type_t type)
{
    switch(type)
    {
    case AT_BLE_UUID_16 :
        return AT_BLE_UUID_16_LEN;

    case AT_BLE_UUID_32 :
        return AT_BLE_UUID_32_LEN;

    default:
    case AT_BLE_UUID_128 :
        return AT_BLE_UUID_128_LEN;
    }
}

at_ble_status_t at_ble_primary_service_discover_all(at_ble_handle_t conn_handle,
    at_ble_handle_t start_handle, at_ble_handle_t end_handle)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    // check if end handle is not smaller than start handle
    if((end_handle>= start_handle) && start_handle > 0)
    {
        gattc_disc_cmd_handler(GATTC_DISC_ALL_SVC,
            0,
            start_handle, end_handle, NULL, conn_handle);
    }
    else
    {
        status = AT_BLE_INVALID_PARAM;
    }

    return status;
}

at_ble_status_t at_ble_primary_service_discover_by_uuid(at_ble_handle_t conn_handle,
    at_ble_handle_t start_handle, at_ble_handle_t end_handle, at_ble_uuid_t* uuid)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    uint32_t uuid_len = 0, valid_uuid=0;

    // check UUID is non zero
    uuid_len = at_ble_uuid_type2len(uuid->type);

    for(;uuid_len>0;uuid_len--)
    {
        valid_uuid += uuid->uuid[uuid_len - 1];
    }

    // check if end handle is not smaller than start handle
    if((end_handle>= start_handle) && (start_handle > 0) && (valid_uuid))
    {
        gattc_disc_cmd_handler(GATTC_DISC_BY_UUID_SVC,
            at_ble_uuid_type2len(uuid->type),
            start_handle, end_handle, uuid->uuid, conn_handle);
    }
    else
    {
        status = AT_BLE_INVALID_PARAM;
    }

    return status;
}

at_ble_status_t at_ble_included_service_discover_all(at_ble_handle_t conn_handle,
    at_ble_handle_t start_handle, at_ble_handle_t end_handle)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    // check if end handle is not smaller than start handle
    if(end_handle>= start_handle)
    {
        gattc_disc_cmd_handler(GATTC_DISC_INCLUDED_SVC,
            0,
            start_handle, end_handle, NULL, conn_handle);
    }
    else
    {
        status = AT_BLE_INVALID_PARAM;
    }

    return status;
}

at_ble_status_t at_ble_characteristic_discover_all(at_ble_handle_t conn_handle,
    at_ble_handle_t start_handle, at_ble_handle_t end_handle)
{
    at_ble_status_t status = AT_BLE_SUCCESS;


    // check if end handle is not smaller than start handle
    if((end_handle >= start_handle) && (start_handle > 0))
    {
        gattc_disc_cmd_handler(GATTC_DISC_ALL_CHAR, 0,
            start_handle, end_handle, NULL, conn_handle);
    }
    else
    {
        status = AT_BLE_INVALID_PARAM;
    }

    return status;
}

at_ble_status_t at_ble_characteristic_discover_by_uuid(at_ble_handle_t conn_handle,
    at_ble_handle_t start_handle, at_ble_handle_t end_handle, at_ble_uuid_t* uuid)
{
    at_ble_status_t status = AT_BLE_SUCCESS;
    uint32_t uuid_len = 0, valid_uuid = 0;

    // check UUID is non zero
    uuid_len = at_ble_uuid_type2len(uuid->type);

    for(;uuid_len>0;uuid_len--)
    {
        valid_uuid += uuid->uuid[uuid_len - 1];
    }

    // check if end handle is not smaller than start handle
    if((end_handle >= start_handle) && (start_handle > 0) && (valid_uuid))
    {
        gattc_disc_cmd_handler(GATTC_DISC_BY_UUID_CHAR,
            at_ble_uuid_type2len(uuid->type),
            start_handle, end_handle, uuid->uuid, conn_handle);
    }
    else
    {
        status = AT_BLE_INVALID_PARAM;
    }

    return status;
}

at_ble_status_t at_ble_descriptor_discover_all(at_ble_handle_t conn_handle,
    at_ble_handle_t start_handle, at_ble_handle_t end_handle)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    // check if end handle is not smaller than start handle
    if((end_handle >= start_handle) && (start_handle > 0))
    {
        gattc_disc_cmd_handler(GATTC_DISC_DESC_CHAR,
            0,
            start_handle, end_handle, NULL, conn_handle);
    }
    else
    {
        status = AT_BLE_INVALID_PARAM;
    }

    return status;
}

at_ble_status_t at_ble_characteristic_read_by_uuid(at_ble_handle_t conn_handle,
    at_ble_handle_t start_handle, at_ble_handle_t end_handle, at_ble_uuid_t* uuid)
{
    at_ble_status_t  status = AT_BLE_SUCCESS;

    // check if end handle is not smaller than start handle
    if(end_handle>= start_handle)
    {
        gattc_read_cmd_handler_by_uuid(start_handle, end_handle,
            at_ble_uuid_type2len(uuid->type), uuid->uuid, conn_handle);
    }
    else
    {
        status = AT_BLE_INVALID_PARAM;
    }

    return status;
}

at_ble_status_t at_ble_characteristic_read(at_ble_handle_t conn_handle, at_ble_handle_t char_handle, uint16_t offset, uint16_t length)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    gattc_read_cmd_handler_simple_read(char_handle, offset, length, conn_handle);
    return status;
}

at_ble_status_t at_ble_characteristic_read_multible(at_ble_handle_t conn_handle,  at_ble_handle_t* char_handle_list,
    uint16_t* char_val_lenght, uint8_t char_handle_count)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    gattc_read_cmd_handler_multible(char_handle_list, char_handle_count, char_val_lenght, conn_handle);
    return status;
}

at_ble_status_t at_ble_characteristic_write(at_ble_handle_t conn_handle, at_ble_handle_t char_handle,
            uint16_t offset, uint16_t length, uint8_t* data,
            bool signed_write, bool with_response )
{
    uint8_t req;

    at_ble_status_t status = AT_BLE_SUCCESS;

    if(signed_write)
    {
        req = GATTC_WRITE_SIGNED;
    }
    else if(with_response)
    {
        req = GATTC_WRITE;
    }
    else
    {
        req = GATTC_WRITE_NO_RESPONSE;
    }

    gattc_write_cmd_handler(req, 1, char_handle, offset, length, data, conn_handle);

    return status;
}

at_ble_status_t at_ble_characteristic_reliable_write_prepare(at_ble_handle_t conn_handle,
        at_ble_handle_t char_handle,
        uint16_t offset, uint16_t length, uint8_t* data)
{
    at_ble_status_t status = AT_BLE_SUCCESS;

    gattc_write_cmd_handler(GATTC_WRITE, 0, char_handle, offset, length, data, conn_handle);
    return status;
}

at_ble_status_t at_ble_characteristic_reliable_write_execute(at_ble_handle_t conn_handle)
{
    at_ble_status_t status = AT_BLE_SUCCESS;
    gattc_execute_write_cmd_handler(1, conn_handle);
    return status;
}

at_ble_status_t at_ble_characteristic_reliable_write_cancel(at_ble_handle_t conn_handle)
{
    at_ble_status_t status = AT_BLE_SUCCESS;
    gattc_execute_write_cmd_handler(0, conn_handle);
    return status;
}

at_ble_status_t at_ble_exchange_mtu(at_ble_handle_t conn_handle)
{
    at_ble_status_t status = AT_BLE_SUCCESS;
    gattc_mtu_cmd_handler(conn_handle);
    return status;
}
// DOM-IGNORE-END
