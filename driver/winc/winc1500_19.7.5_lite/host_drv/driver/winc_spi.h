/**
 *
 * \file
 *
 * \brief This module contains SPI protocol bus API declarations.
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

#ifndef _WINC_SPI_H_
#define _WINC_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#define WINC_BUS_SUCCESS  0
#define WINC_BUS_FAIL     1

int_fast8_t winc_bus_init(void);
int_fast8_t winc_bus_reset(void);
int_fast8_t winc_bus_deinit(void);
uint32_t winc_bus_read_reg(const uint32_t u32Addr);
int_fast8_t winc_bus_read_reg_with_ret(const uint32_t u32Addr, uint32_t *pu32RetVal);
int_fast8_t winc_bus_write_reg(const uint32_t u32Addr, const uint32_t u32Val);
int_fast8_t winc_bus_write_reg_no_rsp(const uint32_t u32Addr, const uint32_t u32Val);
int_fast8_t winc_bus_read_block(const uint32_t u32Addr, uint8_t *pu8Buf, uint_fast16_t u16Sz);
int_fast8_t winc_bus_write_block(uint32_t u32Addr, const uint8_t *pu8Buf, uint_fast16_t u16Sz);
int_fast8_t winc_bus_set_reg_bits(uint32_t u32Addr, uint32_t u32Bits);
int_fast8_t winc_bus_clear_reg_bits(uint32_t u32Addr, uint32_t u32Bits);
bool winc_bus_error(void);

#ifdef __cplusplus
}
#endif

#endif /* _WINC_SPI_H_ */
