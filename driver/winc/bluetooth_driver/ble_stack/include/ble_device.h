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
// DOM-IGNORE-END
#ifndef __DEVICE_H__
#define __DEVICE_H__

/// Constant defining the role
typedef enum
{
    ///Master role
    ROLE_MASTER,
    ///Slave role
    ROLE_SLAVE,
    ///Enumeration end value for role value check
    ROLE_END
}at_ble_dev_role;

struct device_info
{
    // local device adv and scan data
    uint8_t ADVData[AT_BLE_ADV_MAX_SIZE];
    uint8_t SrData[AT_BLE_ADV_MAX_SIZE];
    uint8_t advLen;
    uint8_t srLen;

    // handle when the device is in a slave connection
    at_ble_handle_t conn_handle;

    // local device bt address
    bool at_addr_set;
    at_ble_addr_t at_dev_addr;
    bool addr_auto_gen;
    uint8_t privacy_flags;
    // peer device address
    at_ble_addr_t peer_addr;

    // the role in which the device is running in now
    at_ble_dev_role role;

    // device local keys
    at_ble_LTK_t ltk;
    at_ble_CSRK_t csrk;
    at_ble_IRK_t irk;
    uint16_t renew_dur;

    at_ble_spcp_t spcp_param;
    uint16_t appearance;
    uint8_t dev_name_write_perm;

};

extern struct device_info device;

#endif
