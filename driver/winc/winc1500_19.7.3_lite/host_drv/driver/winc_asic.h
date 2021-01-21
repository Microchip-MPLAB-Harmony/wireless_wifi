/**
 *
 * \file
 *
 * \brief This module contains WINC ASIC specific internal API declarations.
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

#ifndef _WINC_ASIC_H_
#define _WINC_ASIC_H_

#define M2M_ATE_FW_START_VALUE      (0x3C1CD57D)    /*Also, Change this value in boot_firmware if it will be changed here*/
#define M2M_ATE_FW_IS_UP_VALUE      (0xD75DC1C3)    /*Also, Change this value in ATE (Burst) firmware if it will be changed here*/

#define REV_3A0                     (0x3A0)

#define REV(id)                     ( ((id) & 0x00000fff ) )

typedef struct
{
    uint32_t u32Mac_efuse_mib;
    uint32_t u32Firmware_Ota_rev;
} tstrGpRegs;

#ifdef __cplusplus
extern "C" {
#endif

bool winc_chip_interrupts_enable(void);

uint32_t winc_chip_get_id(void);

bool winc_chip_sleep(void);

bool winc_chip_wake(void);

bool winc_chip_halt(void);

void winc_chip_reset(void);

bool winc_chip_init(bool bBootATE, bool bEthMode, uint32_t u32StateRegVal);

bool winc_chip_deinit(void);

bool winc_chip_set_gpio_dir(uint_fast8_t u8GPIO, bool bDirOut);

bool winc_chip_set_gpio_val(uint_fast8_t u8GPIO, bool bBitSet);

bool winc_chip_get_gpio_val(uint_fast8_t u8GPIO, uint8_t *pu8Val);

bool winc_chip_pullup_ctrl(uint32_t u32PinMask, bool bEnable);

bool winc_chip_get_otp_mac_address(uint8_t *pu8MacAddr);

bool winc_chip_get_mac_address(uint8_t *pu8MacAddr);

#ifdef __cplusplus
}
#endif

#endif  /*_WINC_ASIC_H_*/
