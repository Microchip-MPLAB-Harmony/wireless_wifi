/**
 *
 * \file
 *
 * \brief WINC Driver WINC1500 3A0 Hardware Registers.
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

#ifndef _WINC_1500_3A0_REGISTERS_H_
#define _WINC_1500_3A0_REGISTERS_H_

#define NMI_CLOCKLESS_REG_BASE          (0x00)
#define NMI_PERIPH_REG_BASE             (0x1000)
#define NMI_INTR_REG_BASE               (0x1A00)
#define NMI_SPI_REG_BASE                (0xE800)
#define SPI_FLASH_BASE                  (0x10200)

#define WAKE_CLK_REG                    (NMI_CLOCKLESS_REG_BASE + 0x01)
#define HOST_CORT_COMM                  (NMI_CLOCKLESS_REG_BASE + 0x0B)
#define CLOCKS_EN_REG                   (NMI_CLOCKLESS_REG_BASE + 0x0F)
#define CORT_HOST_COMM                  (NMI_CLOCKLESS_REG_BASE + 0x10)

#define NMI_CHIPID                      (NMI_PERIPH_REG_BASE + 0x000)
#define NMI_EFUSE_0_CONTROL             (NMI_PERIPH_REG_BASE + 0x014)
#define NMI_REV_REG_ATE                 (NMI_PERIPH_REG_BASE + 0x048)    /*Revision info register in case of ATE firmware*/
#define WIFI_HOST_RCV_CTRL_3            (NMI_PERIPH_REG_BASE + 0x06C)
#define WIFI_HOST_RCV_CTRL_0            (NMI_PERIPH_REG_BASE + 0x070)
#define WIFI_HOST_RCV_CTRL_2            (NMI_PERIPH_REG_BASE + 0x078)
#define WIFI_HOST_RCV_CTRL_1            (NMI_PERIPH_REG_BASE + 0x084)
#define DUMMY_REGISTER                  WIFI_HOST_RCV_CTRL_1
#define WIFI_HOST_RCV_CTRL_5            (NMI_PERIPH_REG_BASE + 0x088)
#define NMI_STATE_REG                   (NMI_PERIPH_REG_BASE + 0x08C)
#define NMI_CORTUS_BOOT_RESET_MUXSEL    (NMI_PERIPH_REG_BASE + 0x118)
#define NMI_RF_REVID                    (NMI_PERIPH_REG_BASE + 0x3F4)
#define NMI_GLB_RESET_0                 (NMI_PERIPH_REG_BASE + 0x400)
#define NMI_PIN_MUX_0                   (NMI_PERIPH_REG_BASE + 0x408)
#define NMI_PIN_MUX_1                   (NMI_PERIPH_REG_BASE + 0x410)
#define NMI_PLL_INTERNAL_3              (NMI_PERIPH_REG_BASE + 0x41C)
#define MMI_MISC_CTRL                   (NMI_PERIPH_REG_BASE + 0x428)
#define NMI_GP_REG_0                    (NMI_PERIPH_REG_BASE + 0x49C)
#define NMI_GP_REG_1                    (NMI_PERIPH_REG_BASE + 0x4A0)

#define NMI_INTR_ENABLE                 (NMI_INTR_REG_BASE + 0x000)

#define NMI_SPI_CTL                     (NMI_SPI_REG_BASE + 0x000)
#define NMI_SPI_MASTER_DMA_ADDR         (NMI_SPI_REG_BASE + 0x004)
#define NMI_SPI_MASTER_DMA_COUNT        (NMI_SPI_REG_BASE + 0x008)
#define NMI_SPI_SLAVE_DMA_ADDR          (NMI_SPI_REG_BASE + 0x00C)
#define NMI_SPI_SLAVE_DMA_COUNT         (NMI_SPI_REG_BASE + 0x010)
#define NMI_SPI_TX_MODE                 (NMI_SPI_REG_BASE + 0x020)
#define NMI_SPI_PROTOCOL_CONFIG         (NMI_SPI_REG_BASE + 0x024)
#define NMI_SPI_INTR_CTL                (NMI_SPI_REG_BASE + 0x02C)
#define NMI_SPI_MISC_CTRL               (NMI_SPI_REG_BASE + 0x048)

#define SPI_FLASH_MODE                  (SPI_FLASH_BASE + 0x00)
#define SPI_FLASH_CMD_CNT               (SPI_FLASH_BASE + 0x04)
#define SPI_FLASH_DATA_CNT              (SPI_FLASH_BASE + 0x08)
#define SPI_FLASH_BUF1                  (SPI_FLASH_BASE + 0x0C)
#define SPI_FLASH_BUF2                  (SPI_FLASH_BASE + 0x10)
#define SPI_FLASH_BUF_DIR               (SPI_FLASH_BASE + 0x14)
#define SPI_FLASH_TR_DONE               (SPI_FLASH_BASE + 0x18)
#define SPI_FLASH_DMA_ADDR              (SPI_FLASH_BASE + 0x1C)
#define SPI_FLASH_MSB_CTL               (SPI_FLASH_BASE + 0x20)
#define SPI_FLASH_TX_CTL                (SPI_FLASH_BASE + 0x24)

#define NMI_REV_REG                     (0x207AC)   /*Also, Used to load ATE firmware from SPI Flash and to ensure that it is running too*/
#define M2M_WAIT_FOR_HOST_REG           (0x207BC)

#define NMI_GP_REG_2                    (0xC0008)
#define BOOTROM_REG                     (0xC000C)

#define WIFI_HOST_RCV_CTRL_4            (0x150400)

#endif /* _WINC_1500_3A0_REGISTERS_H_ */
