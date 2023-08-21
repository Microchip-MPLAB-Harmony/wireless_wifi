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
#include "bsp/winc_bsp.h"
#include "common/winc_defines.h"
#include "m2m/winc_m2m_types.h"
#include "m2m/winc_m2m_wifi.h"
#include "driver/winc_hif.h"

#define MINIMUM_FW_MAJ      (1)
#define MINIMUM_FW_MIN      (4)
#define MINIMUM_FW_PATCH    (0)

static uintptr_t bleUserHandle = 0;

at_ble_status_t platform_init(uintptr_t handle)
{
    tstrM2mRev strFwVer;

    if (M2M_SUCCESS != m2m_wifi_get_firmware_version(&strFwVer))
    {
        return AT_BLE_FAILURE;
    }

    if (strFwVer.u8FirmwareMajor < MINIMUM_FW_MAJ)
    {
        return AT_BLE_FW_INCOMPAT;
    }
    else if (strFwVer.u8FirmwareMajor == MINIMUM_FW_MAJ)
    {
        if (strFwVer.u8FirmwareMinor < MINIMUM_FW_MIN)
        {
            return AT_BLE_FW_INCOMPAT;
        }

        if ((strFwVer.u8FirmwareMinor == MINIMUM_FW_MIN) &&
                (strFwVer.u8FirmwarePatch < MINIMUM_FW_PATCH))
        {
            return AT_BLE_FW_INCOMPAT;
        }
    }

    WINC_CRIT_SEC_BLE_INIT;

    bleUserHandle = handle;

    return AT_BLE_SUCCESS;
}

at_ble_status_t platform_interface_send(uint8_t *pu8Data, uint16_t u16DataLen)
{
    if (M2M_SUCCESS != winc_hif_send(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_BLE_API_SEND | M2M_REQ_DATA_PKT,
                                     &u16DataLen, sizeof(uint16_t), pu8Data, u16DataLen, sizeof(uint16_t)))
    {
        return AT_BLE_FAILURE;
    }

    return AT_BLE_SUCCESS;
}

void platform_cmd_cmpl_signal(void)
{
    winc_bsp_ble_cmd_wait_signal(bleUserHandle);
}

at_ble_status_t platform_cmd_cmpl_wait(void)
{
    tenuBLECmdWaitStatus enuBLECmdWaitStatus;

    enuBLECmdWaitStatus = winc_bsp_ble_cmd_wait_start(bleUserHandle);

    while (enuBLECmdWaitStatus == WINC_BLE_CMD_WAIT_STATUS_CLEAR)
    {
        winc_hif_handle_isr();

        enuBLECmdWaitStatus = winc_bsp_ble_cmd_wait_check(bleUserHandle);
    }

    if (enuBLECmdWaitStatus == WINC_BLE_CMD_WAIT_STATUS_SET)
    {
        return AT_BLE_SUCCESS;
    }

    return AT_BLE_TIMEOUT;
}

void platform_send_lock_aquire(void)
{
    WINC_CRIT_SEC_BLE_ENTER;
}

void platform_send_lock_release(void)
{
    WINC_CRIT_SEC_BLE_LEAVE;
}
