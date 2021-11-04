/**
 *
 * \file
 *
 * \brief WINC BSP API Declarations.
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

/** @defgroup winc_bsp Board Support Package (BSP)
    @brief
        The BSP is an architecture specific package implemented to support the common WINC driver. When
        porting the driver to a new architecture this package will need to be written to provide the
        functionality described here.
    @{
        @defgroup   BSPDefine   Defines
        @defgroup   BSPInit     Initialization and de-initialization functions
        @defgroup   BSPSleep    Sleep/delay functions
        @defgroup   BSPInt      Interrupt handling functions
        @defgroup   BSPSPI      SPI interface functions
        @defgroup   BSPGPIO     GPIO control functions
        @defgroup   BSPUART     UART interface functions
        @note
            These functions are only required for supporting the serial bridge functionality of the
            driver.
    @}
 */

#ifndef _WINC_BSP_H_
#define _WINC_BSP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "conf_winc.h"

/**@addtogroup BSPDefine
 * @{
 */
#if defined(CONF_WINC_MCU_ARCH_LITTLE_ENDIAN)
#define HOST_TO_WINC_U32(U32)      (U32)
#define HOST_TO_WINC_U16(U16)      (U16)
#elif defined(CONF_WINC_MCU_ARCH_BIG_ENDIAN)
#define HOST_TO_WINC_U32(U32)      CONF_WINC_UINT32_SWAP(U32)
#define HOST_TO_WINC_U16(U16)      CONF_WINC_UINT16_SWAP(U16)
#endif

#define WINC_TO_HOST_U32            HOST_TO_WINC_U32
#define WINC_TO_HOST_U16            HOST_TO_WINC_U16
/**@}*/     // BSPDefine

#ifdef __cplusplus
extern "C" {
#endif

/**@addtogroup BSPInit
 * @{
 */
/*!
 @fn \
    int_fast8_t winc_bsp_init(void);

 @brief
    This function is used to initialize the BSP in order to prepare the WINC before any
    WINC API calls.

 @details
    The winc_bsp_init function is the first function that should be called at the beginning of
    every application to initialize the BSP and the WINC board. Otherwise, the rest of the BSP
    function calls will return with failure. This function should also be called after the WINC
    has been switched off with a successful call to @ref winc_bsp_deinit in order to reinitialize
    the BSP before the Application can use any of the WINC APIs again. After the function
    initializes the WINC.

@note
    Implementation of this function is host dependent.

@warning
    Omitting this function will lead to unavailability of host-chip communication.

@see    winc_bsp_deinit

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_init(void);

/*!
@fn \
    int_fast8_t winc_bsp_deinit(void);

@brief
    This function is used to de-initialize the BSP and turn off the WINC board.

@details
    The winc_bsp_deinit is the last function that should be called after the application has
    finished and before the WINC is switched off. Every function call of @ref winc_bsp_init
    should be matched with a call to winc_bsp_deinit. Failure to do so may result in the WINC
    consuming higher power than expected, since it won't be properly de-initialized.

@pre
    The BSP should be initialized through @ref winc_bsp_init first.

@note
    Implementation of this function is host dependent.

@warning
    Omitting this function may lead to unknown behavior in case of soft reset.

@see    winc_bsp_init

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_deinit(void);

/**@}*/     // BSPInit
/**@addtogroup BSPSleep
 * @{
 */
/*!
@fn \
    void winc_bsp_sleep(uint32_t u32TimeMsec);

@brief
    Used to put the host to sleep for the specified duration (in milliseconds).
    Forcing the host to sleep for extended period may lead to host not being able to respond
    to WINC board events. It is important to be considerate while choosing the sleep period.

@param[in]  u32TimeMsec
    Time unit in milliseconds.

@pre
    Initialize @ref winc_bsp_init first.

@note
    Implementation of this function is host dependent.

@see    winc_bsp_init
*/
void winc_bsp_sleep(uint32_t u32TimeMsec);

/**@}*/     // BSPSleep
/**@addtogroup BSPInt
 * @{
 */
/*!
@fn \
    int_fast8_t winc_bsp_interrupt_request_pending(void);

@brief
    Indicates to the WINC driver is an interrupt request has been received. This function will be
    called by the WINC driver during @ref m2m_wifi_handle_events to determine if the WINC should be
    queried for possible messages.

@pre
    Initialize @ref winc_bsp_init first.

@note
    Implementation of this function is host dependent.

@see    winc_bsp_init

@return
    The functions returns a non-zero value to indicate if an interrupt has been received, otherwise zero.
*/
int_fast8_t winc_bsp_interrupt_request_pending(void);

/*!
@fn \
    int_fast8_t winc_bsp_interrupt_serviced(bool bEdgeInt);

@brief
    Indicates to the BSP that a pending interrupt has been processed by the driver. The WINC driver
    will call this function to indicate that a message has been received from the WINC. @p bEdgeInt
    indicates if an edge interrupt should be serviced (true) or a level interrupt (false).

    If the BSP is managing the WINC devices IRQn line as an edge interrupt then when @p bEdgeInt is true
    the BSP should consider a pending interrupt has been serviced. If @p bEdgeInt is false the BSP must
    ignore it. This ensures the interrupt is serviced before the WINC re-activates the IRQn line and thus
    before another interrupt can be received.

    If the BSP is managing the WINC devices IRQn line as a level interrupt then when @p bEdgeInt is false
    the BSP should consider a pending interrupt has been serviced. If @p bEdgeInt is true the BSP must
    ignore it. This ensures the interrupt is serviced after the WINC re-activates the IRQn line and thus
    ensures any previous interrupt level has been de-asserted by the WINC.

@pre
    Initialize @ref winc_bsp_init first.

@note
    Implementation of this function is host dependent.

@see    winc_bsp_init

@return
    The functions returns a non-zero value to indicate if an interrupt service has been acknowledged by the BSP.
*/
int_fast8_t winc_bsp_interrupt_serviced(bool bEdgeInt);

/**@}*/     // BSPInt
/**@addtogroup BSPSPI
 * @{
 */
/*!
@fn \
    int_fast8_t winc_bsp_spi_open(void);

@brief
    Called by the driver to open the SPI bus for use.

@pre
    Initialize @ref winc_bsp_init first.

@note
    Implementation of this function is host dependent.

@see    winc_bsp_init

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_spi_open(void);

/*!
@fn \
    int_fast8_t winc_bsp_spi_close(void);

@brief
    Called by the driver to close the SPI bus. @ref winc_bsp_spi_open should have been called before this.

@pre
    Initialize @ref winc_bsp_init first and @ref winc_bsp_spi_open must have been called.

@note
    Implementation of this function is host dependent.

@see    winc_bsp_init
@see    winc_bsp_spi_open

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_spi_close(void);

/*!
@fn \
    int_fast8_t winc_bsp_spi_write(const uint8_t *puBuf, size_t size);

@brief
    Called by the driver to write data to the SPI bus.

@param[in]  puBuf
    A pointer to the data to be written to the SPI bus.

@param[in]  size
    The number of bytes to be written.

@pre
    Initialize @ref winc_bsp_init first and @ref winc_bsp_spi_open must have been called.

@note
    Implementation of this function is host dependent.

@see    winc_bsp_init
@see    winc_bsp_spi_open

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_spi_write(const uint8_t *puBuf, size_t size);

/*!
@fn \
    int_fast8_t winc_bsp_spi_read(uint8_t *puBuf, size_t size);

@brief
    Called by the driver to read data from the SPI bus.

@param[in]  puBuf
    A pointer to a buffer to which the data should be written.

@param[in]  size
    The number of bytes to be read.

@pre
    Initialize @ref winc_bsp_init first and @ref winc_bsp_spi_open must have been called.

@note
    Implementation of this function is host dependent.

@see    winc_bsp_init
@see    winc_bsp_spi_open

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_spi_read(uint8_t *puBuf, size_t size);

/**@}*/     // BSPSPI
/**@addtogroup BSPGPIO
 * @{
 */
/*!
@fn \
    void winc_bsp_chip_enable_assert(void);

@brief
    Called by the driver when the chip enable line should be asserted.
*/
void winc_bsp_chip_enable_assert(void);

/*!
@fn \
    void winc_bsp_chip_enable_deassert(void);

@brief
    Called by the driver when the chip enable line should be de-asserted.
*/
void winc_bsp_chip_enable_deassert(void);

/*!
@fn \
    void winc_bsp_reset_assert(void);

@brief
    Called by the driver when the WINC reset line should be asserted.
*/
void winc_bsp_reset_assert(void);

/*!
@fn \
    void winc_bsp_reset_deassert(void);

@brief
    Called by the driver when the WINC reset line should be de-asserted.
*/
void winc_bsp_reset_deassert(void);

/**@}*/     // BSPGPIO
/**@addtogroup BSPUART
 * @{
 */
/*!
@fn \
    int_fast8_t winc_bsp_uart_open(uint32_t u32BaudRate);

@brief
    Called by the driver to open the UART.

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_uart_open(uint32_t u32BaudRate);

/*!
@fn \
    int_fast8_t winc_bsp_uart_close(void);

@brief
    Called by the driver to close the UART.

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_uart_close(void);

/*!
@fn \
    int_fast8_t winc_bsp_uart_set_baudrate(uint32_t u32BaudRate);

@brief
    Called by the driver to set the UART baud rate.

@return
    The function returns a value of 1 for successful operations and a 0 value otherwise.
*/
int_fast8_t winc_bsp_uart_set_baudrate(uint32_t u32BaudRate);

/*!
@fn \
    size_t winc_bsp_uart_write(const uint8_t *pu8Buf, size_t szLength);

@brief
    Called by the driver to write data to the UART.

@note
    This function must either indicate all bytes were accepted or an error occurred.

@return
    The number of bytes written to the UART or zero for no data/error.
*/
size_t winc_bsp_uart_write(const uint8_t *pu8Buf, size_t szLength);

/*!
@fn \
    size_t winc_bsp_uart_read(uint8_t *pu8Buf, size_t szLength);

@brief
    Called by the driver to read data from the UART.

@return
    The number of bytes read, this may be less than \p szLength
*/
size_t winc_bsp_uart_read(uint8_t *pu8Buf, size_t szLength);

/**@}*/     // BSPUART

#ifdef __cplusplus
}
#endif

#endif  /*_WINC_BSP_H_*/
