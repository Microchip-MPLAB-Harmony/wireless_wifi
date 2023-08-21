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
#include "gattc_task.h"
#include "gapm_task.h"
#include "error.h"

at_ble_status_t at_ble_att_error(uint8_t att_error_code)
{
    at_ble_status_t status;
    switch(att_error_code)
    {
    case  ATT_ERR_NO_ERROR:
        status = AT_BLE_SUCCESS;
        break;
    case ATT_ERR_INVALID_HANDLE:
        status = AT_BLE_INVALID_HANDLE;
        break;
    case ATT_ERR_INSUFF_RESOURCE:
        status = AT_BLE_INSUFF_RESOURCE;
        break;
    case ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN:
        status = AT_BLE_INVALID_ATTRIBUTE_VAL_LEN;
        break;
    default:
        status = AT_BLE_FAILURE;
        break;
    }
    return status;
}

at_ble_status_t at_ble_gap_error(uint8_t gap_error_code)
{
    at_ble_status_t status;
    switch(gap_error_code)
    {
    case   GAP_ERR_INVALID_PARAM:
        status = AT_BLE_INVALID_PARAM;
        break;
    case  GAP_ERR_COMMAND_DISALLOWED:
        status = AT_BLE_COMMAND_DISALLOWED;
        break;
    case GAP_ERR_NO_ERROR:
        status = AT_BLE_SUCCESS;
        break;
    default:
        status = AT_BLE_FAILURE;
        break;
    }
    return status;
}
// DOM-IGNORE-END
