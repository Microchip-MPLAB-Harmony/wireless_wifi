/*
Copyright (C) 2017, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
