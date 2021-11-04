/**
 *
 * \file
 *
 * \brief This module contains a Harmony 3 BSP APIs implementation.
 *
 * Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
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
