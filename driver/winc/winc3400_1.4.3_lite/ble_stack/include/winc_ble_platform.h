/*******************************************************************************
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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "winc_ble_api.h"

/**
@defgroup platform Platform API

@{
*/

/** @brief implements platform-specific initialization
 *
 * @return AT_BLE_SUCCESS operation completed successfully
 * @return AT_BLE_FW_INCOMPAT if firmware is incompatible is BLE API
 * @return AT_BLE_FAILURE Generic error.
 */
at_ble_status_t platform_init(uintptr_t handle);

/** @brief sends a message over the platform-specific bus
 *
 * Sends a message over the platform-specific bus that might be UART, SPI or other
 * if the BTLC1000 external wakeup signal is used, it is up to this function implementation
 * to assert/deassert it appropriately
 *
 * @param[in] pu8Data data to send over the interface
 * @param[in] u16DataLen length of data
 * @return Zero if operation completed successfully
 * @return Non zero for Generic error.
 *
 */
at_ble_status_t platform_interface_send(uint8_t *pu8Data, uint16_t u16DataLen);

/** @brief the callback to upper layers to process received packets
  *
  * This function is implemented by the upper layers (the event loop) and it is up
  * to the platform implementation to call it whenever data are received from the interface
  *
  * @param[in] pu8Data data received from the interface
  * @param[in] u16DataLen length of data
  *
  */
void platform_interface_callback(uint8_t *pu8Data, uint16_t u16DataLen);

/** @brief fires the command-complete signal
 *  @note more details at the platform porting guide
 *
 */
void platform_cmd_cmpl_signal(void);

/** @brief blocks until the command-complete signal is fired
 *  @note more details at the platform porting guide
 *
 * @return non-zero if Timeout
 * @return zero if Operation completed successfully
 */
at_ble_status_t platform_cmd_cmpl_wait(void);

/** @brief Acquires a lock on sending a BLE message
 */
void platform_send_lock_aquire(void);

/** @brief Releases a lock on sending a BLE message
 */
void platform_send_lock_release(void);

/** @}*/

#endif // __PLATFORM_H__
