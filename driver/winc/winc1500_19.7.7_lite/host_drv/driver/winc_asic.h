/**
 *
 * \file
 *
 * \brief This module contains WINC ASIC specific internal API declarations.
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
