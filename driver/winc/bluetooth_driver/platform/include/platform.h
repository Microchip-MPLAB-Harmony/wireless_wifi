// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "at_ble_api.h"
typedef struct {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
}frmw_ver;
typedef struct {
    uintptr_t drvHandle;
    int8_t (*ble_write_cb)(uint8_t*, uint32_t);
    void (*plf_wait_cb)(uintptr_t);
    frmw_ver fw_ver;
}plf_params_t;

/**
@defgroup platform Platform API

@{
*/

 /** @brief implements platform-specific initialization
  *
  * @param[in] platform_params platform specific params, this pointer is passed from the
  * at_ble_init function and interpreted by the platform
  *
  * @return AT_BLE_SUCCESS operation completed successfully
  * @return AT_BLE_FAILURE Generic error.
  */
at_ble_status_t platform_init(plf_params_t* platform_params);

 /** @brief sends a message over the platform-specific bus
  *
  * Sends a message over the platform-specific bus that might be UART, SPI or other
  * if the BTLC1000 external wakeup signal is used, it is up to this function implementation
  * to assert/deassert it appropriately
  *
  * @param[in] data data to send over the interface
  * @param[in] len length of data
  *
  */
void platform_interface_send(uint8_t* data, uint32_t len);

 /** @brief the callback to upper layers to process received packets
  *
  * This function is implemented by the upper layers (the event loop) and it is up
  * to the platform implementation to call it whenever data are received from the interface
  *
  * @param[in] data data received from the interface
  * @param[in] len length of data
  *
  */
void platform_interface_callback(uint8_t* data, uint32_t len);

 /** @brief fires the command-complete signal
  *  @note more details at the platform porting guide
  *
  */
void platform_cmd_cmpl_signal(void);

 /** @brief blocks until the command-complete signal is fired
  *  @note more details at the platform porting guide
  *
  * @return true Timeout
  * @return false Operation completed successfully
  */
bool platform_cmd_cmpl_wait(void);

 /** @brief fires the event signal
  *  @note more details at the platform porting guide
  *
  */
void platform_event_signal(void);

 /** @brief blocks until the event signal is fired
  *  @note more details at the platform porting guide
  *
  * @param[in] timeout timeout in ms passed by user
  *
  */
uint8_t platform_event_wait(uint32_t timeout);
 /** @}*/

#endif // __PLATFORM_H__
