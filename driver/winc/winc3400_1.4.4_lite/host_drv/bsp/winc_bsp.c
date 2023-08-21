/**
 *
 * \file
 *
 * \brief This module contains a Harmony 3 BSP APIs implementation.
 */
/*
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <stdio.h>

#include "osal/osal.h"

#include "winc_bsp.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc.h"
#include "wdrv_winc_gpio.h"
#include "wdrv_winc_spi.h"

#ifdef WINC_CRIT_SEC_HIF
static OSAL_SEM_HANDLE_TYPE hifSemaphore;
#endif
#ifdef WINC_CRIT_SEC_BUS
static OSAL_MUTEX_HANDLE_TYPE spiLock;
#endif
#ifdef WINC_CRIT_SEC_BLE
static OSAL_MUTEX_HANDLE_TYPE bleLock;
#endif

int_fast8_t winc_bsp_interrupt_request_pending(void)
{
    // Indicate if an interrupt request is pending from the WINC

    return 1;
}

int_fast8_t winc_bsp_interrupt_serviced(bool bEdgeInt)
{
    // An interrupt has been serviced, bEdgeInt indicate if the interrupt to be cleared
    // is an edge interrupt or level interrupt

    if (bEdgeInt)
    {
    }
    else
    {
    }

    return 1;
}

int_fast8_t winc_bsp_init(void)
{
    // Initialise the BSP, ensure IO are configured and interrupts are setup

    return 1;
}

int_fast8_t winc_bsp_deinit(void)
{
    return 1;
}

void winc_bsp_sleep(uint32_t u32TimeMsec)
{
    if (!u32TimeMsec)
        return;

    WDRV_MSDelay(u32TimeMsec);
}

void winc_bsp_chip_enable_assert(void)
{
    WDRV_WINC_GPIOChipEnableAssert();
}

void winc_bsp_chip_enable_deassert(void)
{
    WDRV_WINC_GPIOChipEnableDeassert();
}

void winc_bsp_reset_assert(void)
{
    WDRV_WINC_GPIOResetAssert();
}

void winc_bsp_reset_deassert(void)
{
    WDRV_WINC_GPIOResetDeassert();
}

int_fast8_t winc_bsp_spi_open(void)
{
    return 1;
}

int_fast8_t winc_bsp_spi_write(const uint8_t *puBuf, size_t size)
{
    if ((puBuf == NULL) || (!size))
        return 0;

    if (true == WDRV_WINC_SPISend((unsigned char *const) puBuf, size))
        return 1;

    return 0;
}

int_fast8_t winc_bsp_spi_read(uint8_t *puBuf, size_t size)
{
    if ((puBuf == NULL) || (!size))
        return 0;

    if (true == WDRV_WINC_SPIReceive((unsigned char *const) puBuf, size))
        return 1;

    return 0;
}

int_fast8_t winc_bsp_spi_close(void)
{
    return 1;
}

#ifdef CONF_WINC_SERIAL_BRIDGE_INCLUDE
int_fast8_t __attribute__((weak)) winc_bsp_uart_open(uint32_t u32BaudRate)
{
    return 1;
}

int_fast8_t __attribute__((weak)) winc_bsp_uart_close(void)
{
    return 1;
}

int_fast8_t __attribute__((weak)) winc_bsp_uart_set_baudrate(uint32_t u32BaudRate)
{
    winc_bsp_uart_close();
    winc_bsp_uart_open(u32BaudRate);

    return 1;
}

size_t __attribute__((weak)) winc_bsp_uart_write(const uint8_t *pu8Buf, size_t szLength)
{
    return 0;
}

size_t __attribute__((weak)) winc_bsp_uart_read(uint8_t *pu8Buf, size_t szLength)
{
    return 0;
}
#endif

#ifdef CONF_WINC_BLE_INCLUDE
static bool bBLECmdWait;

tenuBLECmdWaitStatus winc_bsp_ble_cmd_wait_start(uintptr_t handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    bBLECmdWait = false;

    /* Ensure the driver handle is valid. */
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WINC_BLE_CMD_WAIT_STATUS_TIMEOUT;
    }

    while (OSAL_RESULT_TRUE != OSAL_SEM_Pend(&pDcpt->pCtrl->drvEventSemaphore, OSAL_WAIT_FOREVER))
    {
    }
    
    return WINC_BLE_CMD_WAIT_STATUS_CLEAR;
}

tenuBLECmdWaitStatus winc_bsp_ble_cmd_wait_check(uintptr_t handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if ((NULL == pDcpt) || (NULL == pDcpt->pCtrl))
    {
        return WINC_BLE_CMD_WAIT_STATUS_TIMEOUT;
    }

    if (bBLECmdWait)
    {
        return WINC_BLE_CMD_WAIT_STATUS_SET;
    }

    while (OSAL_RESULT_TRUE != OSAL_SEM_Pend(&pDcpt->pCtrl->drvEventSemaphore, OSAL_WAIT_FOREVER))
    {
    }

    return WINC_BLE_CMD_WAIT_STATUS_CLEAR;
}

void winc_bsp_ble_cmd_wait_signal(uintptr_t handle)
{
    bBLECmdWait = true;
}
#endif /* CONF_WINC_BLE_INCLUDE */

#ifdef WINC_CRIT_SEC_HIF
bool winc_crit_sec_hif_init(void)
{
    if (OSAL_RESULT_TRUE != OSAL_SEM_Create(&hifSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1))
        return false;

    return true;
}

void winc_crit_sec_hif_deinit(void)
{
    OSAL_SEM_Delete(&hifSemaphore);
}

void winc_crit_sec_hif_enter(void)
{
    while (OSAL_RESULT_FALSE == OSAL_SEM_Pend(&hifSemaphore, OSAL_WAIT_FOREVER))
    {
    }
}

void winc_crit_sec_hif_leave(void)
{
    OSAL_SEM_Post(&hifSemaphore);
}
#endif

#ifdef WINC_CRIT_SEC_BUS
void winc_crit_sec_bus_init(void)
{
    OSAL_MUTEX_Create(&spiLock);
}

void winc_crit_sec_bus_deinit(void)
{
    OSAL_MUTEX_Delete(&spiLock);
}

bool winc_crit_sec_bus_enter(void)
{
    if (OSAL_RESULT_TRUE != OSAL_MUTEX_Lock(&spiLock, OSAL_WAIT_FOREVER))
    {
        return false;
    }

    return true;
}

void winc_crit_sec_bus_leave(void)
{
    OSAL_MUTEX_Unlock(&spiLock);
}
#endif

#ifdef WINC_CRIT_SEC_BLE
void winc_crit_sec_ble_init(void)
{
    OSAL_MUTEX_Create(&bleLock);
}

void winc_crit_sec_ble_deinit(void)
{
    OSAL_MUTEX_Delete(&bleLock);
}

bool winc_crit_sec_ble_enter(void)
{
    if (OSAL_RESULT_TRUE != OSAL_MUTEX_Lock(&bleLock, OSAL_WAIT_FOREVER))
    {
        return false;
    }

    return true;
}

void winc_crit_sec_ble_leave(void)
{
    OSAL_MUTEX_Unlock(&bleLock);
}
#endif