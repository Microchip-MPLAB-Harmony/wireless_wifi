/*******************************************************************************
  This module contains WINC3400 ASIC specific internal APIs.

  File Name:
    nmasic.h

  Summary:
    This module contains WINC3400 ASIC specific internal APIs.

  Description:
    This module contains WINC3400 ASIC specific internal APIs.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _NMASIC_H_
#define _NMASIC_H_

#include "nm_common.h"

#define NMI_PERIPH_REG_BASE         0x1000
#define NMI_CHIPID                  (NMI_PERIPH_REG_BASE)
#define rNMI_GP_REG_0               (0x149c)
#define rNMI_GP_REG_1               (0x14A0)
#define rNMI_GLB_RESET              (0x1400)
#define rNMI_BOOT_RESET_MUX         (0x1118)
#define NMI_STATE_REG               (0x108c)
#define BOOTROM_REG                 (0xc000c)
#define NMI_REV_REG                 (0x207ac)   /*Also, Used to load ATE firmware from SPI Flash and to ensure that it is running too*/
#define M2M_WAIT_FOR_HOST_REG       (0x207bc)
#define M2M_FINISH_INIT_STATE       0x02532636UL
#define M2M_FINISH_BOOT_ROM         0x10add09eUL
#define M2M_START_FIRMWARE          0xef522f61UL
#define M2M_START_PS_FIRMWARE       0x94992610UL

#define M2M_ATE_FW_START_VALUE      (0x3C1CD57D)    /*Also, Change this value in boot_firmware if it will be changed here*/
#define M2M_ATE_FW_IS_UP_VALUE      (0xD75DC1C3)    /*Also, Change this value in ATE (Burst) firmware if it will be changed here*/

#define REV_2B0                     (0x2B0)
#define REV_B0                      (0x2B0)
#define REV_3A0                     (0x3A0)
#define CHIP_ID_3000D               (0x3000D0)

#define GET_CHIPID()                nmi_get_chipid()
#define ISNMC1000(id)               (((id & 0xfffff000) == 0x100000) ? 1 : 0)
#define ISNMC1500(id)               (((id & 0xfffff000) == 0x150000) ? 1 : 0)
#define ISNMC3400(id)               (((id & 0xfff0f000) == 0x300000) ? 1 : 0)
#define REV(id)                     ( ((id) & 0x00000fff ) )
#define EFUSED_MAC(value)           (value & 0xffff0000)

#define rHAVE_SDIO_IRQ_GPIO_BIT     (NBIT0)
#define rHAVE_USE_PMU_BIT           (NBIT1)
#define rHAVE_SLEEP_CLK_SRC_RTC_BIT (NBIT2)
#define rHAVE_SLEEP_CLK_SRC_XO_BIT  (NBIT3)
#define rHAVE_EXT_PA_INV_TX_RX      (NBIT4)
#define rHAVE_LEGACY_RF_SETTINGS    (NBIT5)
#define rHAVE_LOGS_DISABLED_BIT     (NBIT6)
#define rHAVE_ETHERNET_MODE_BIT     (NBIT7)

typedef struct {
    uint32_t u32Mac_efuse_mib;
    uint32_t u32Firmware_Ota_rev;
} tstrGpRegs;

#ifdef __cplusplus
extern "C" {
#endif
/**
 *  @fn     nm_clkless_wake
 *  @brief  Wakeup the chip using clockless registers
 *  @return ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
int8_t nm_clkless_wake(void);

int8_t chip_wake(void);

int8_t chip_sleep(void);

void chip_idle(void);

void enable_rf_blocks(void);

int8_t enable_interrupts(void);

int8_t cpu_start(void);

uint32_t nmi_get_chipid(void);

uint32_t nmi_get_rfrevid(void);

void restore_pmu_settings_after_global_reset(void);

void nmi_update_pll(void);

void nmi_set_sys_clk_src_to_xo(void);

int8_t chip_reset(void);

int8_t wait_for_bootrom(uint8_t);

int8_t wait_for_firmware_start(uint8_t);

int8_t chip_deinit(void);

int8_t chip_reset_and_cpu_halt(void);

int8_t set_gpio_dir(uint8_t gpio, uint8_t dir);

int8_t set_gpio_val(uint8_t gpio, uint8_t val);

int8_t get_gpio_val(uint8_t gpio, uint8_t* val);

int8_t pullup_ctrl(uint32_t pinmask, uint8_t enable);

int8_t nmi_get_otp_mac_address(uint8_t *pu8MacAddr, uint8_t * pu8IsValid);

int8_t nmi_get_mac_address(uint8_t *pu8MacAddr);

int8_t chip_apply_conf(uint32_t u32conf);

#ifdef __cplusplus
}
#endif

#endif  /*_NMASIC_H_*/
