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

#include "winc_ble_api.h"

#include "winc_ble_platform.h"
#include "winc_ble_gattc_task.h"
#include "winc_ble_gattm_task.h"
#include "winc_ble_gapm_task.h"
#include "winc_ble_gapc_task.h"
#include "winc_ble_dbg_task.h"

#include "winc_ble_device.h"

at_ble_status_t at_ble_authenticate(at_ble_handle_t conn_handle, at_ble_pair_features_t *features,
                                    at_ble_LTK_t *ltk, at_ble_CSRK_t *csrk)
{
    at_ble_status_t status = AT_BLE_FAILURE;
    uint8_t auth_req;

    if (((features->initiator_keys == AT_BLE_KEY_DIST_SIGN) && (csrk == NULL)) ||
            ((features->initiator_keys == AT_BLE_KEY_DIST_ENC) && (ltk == NULL)) ||
            ((features->bond) && (ltk == NULL)) ||
            ((device.role != ROLE_MASTER) && (device.role != ROLE_SLAVE)))
    {
        return AT_BLE_INVALID_PARAM;
    }

    if (features->bond)
    {
        memcpy(&device.ltk, ltk, sizeof(at_ble_LTK_t));
    }

    if (csrk != NULL)
    {
        memcpy(&device.csrk, csrk, sizeof(at_ble_CSRK_t));
    }

    // To do save local device keys
    if (!features->mitm_protection)
    {
        auth_req = (features->bond) ? GAP_AUTH_REQ_NO_MITM_BOND :
                   GAP_AUTH_REQ_NO_MITM_NO_BOND;
    }
    else
    {
        auth_req = (features->bond) ? GAP_AUTH_REQ_MITM_BOND :
                   GAP_AUTH_REQ_MITM_NO_BOND;
    }

    if (device.role == ROLE_MASTER)
    {
        status = gapc_bond_cmd_handler(
                     conn_handle, features->io_capabilities, features->oob_available,
                     auth_req, features->max_key_size, features->initiator_keys, features->responder_keys,
                     features->desired_auth);
    }
    else if (device.role == ROLE_SLAVE)
    {
        status = gapc_bond_cfm_handler_pair_resp(
                     1, features->io_capabilities,
                     features->oob_available, auth_req, features->max_key_size,
                     features->initiator_keys, features->responder_keys,
                     features->desired_auth, conn_handle);
    }

    return status;
}

at_ble_status_t at_ble_pair_key_reply(at_ble_handle_t conn_handle,
                                      at_ble_pair_key_type_t type, uint8_t *key)
{
    //Initialize TK with zeros
    uint8_t tk[KEY_LEN] = {0};
    int8_t i;
    uint32_t pin_code = 0;

    if (key == NULL)
    {
        return AT_BLE_FAILURE;
    }

    // To do add out of band pairing
    if (type == AT_BLE_PAIR_PASSKEY)
    {
        uint32_t power_base = 1;

        for (i = AT_BLE_PASSKEY_LEN - 1; i >= 0; i--)
        {
            pin_code += (key[i] - 48 ) * power_base;
            power_base *= 10;
        }

        tk[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
        tk[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
        tk[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
        tk[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);
    }
    else if (type == AT_BLE_PAIR_OOB)
    {
        for (i = 0; i < 16; i++)
        {
            tk[i] = key[i];
        }
    }

    return gapc_bond_cfm_handler_key_exch(GAPC_TK_EXCH, 1, tk, conn_handle);
}


at_ble_status_t at_ble_encryption_start(at_ble_handle_t conn_handle, at_ble_LTK_t *key, at_ble_auth_t auth)
{
    return gapc_encrypt_cmd_handler(conn_handle, key->key, key->ediv, key->nb, key->key_size, auth);
}

at_ble_status_t at_ble_encryption_request_reply(at_ble_handle_t conn_handle, at_ble_auth_t auth,
        bool key_found, at_ble_LTK_t *key)
{
    if (NULL == key)
    {
        return AT_BLE_FAILURE;
    }

    return gapc_encrypt_cfm_handler(conn_handle, auth, key_found, key->key, key->key_size);
}

at_ble_status_t at_ble_send_slave_sec_request(at_ble_handle_t conn_handle, bool mitm_protection, bool bond)
{
    uint8_t auth_req;

    if (!mitm_protection)
    {
        auth_req = bond ? GAP_AUTH_REQ_NO_MITM_BOND :
                   GAP_AUTH_REQ_NO_MITM_NO_BOND;
    }
    else
    {
        auth_req = bond ? GAP_AUTH_REQ_MITM_BOND :
                   GAP_AUTH_REQ_MITM_NO_BOND;
    }

    return gapc_security_cmd_handler(conn_handle, auth_req);
}