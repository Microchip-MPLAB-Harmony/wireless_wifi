/**
 *
 * \file
 *
 * \brief WINC3400 Peripherals Application Interface.
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

#ifndef _M2M_PERIPH_H_
#define _M2M_PERIPH_H_


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@enum       tenuGpioNum

@brief
            A list of GPIO numbers configurable through the m2m_periph module.
*/
typedef enum
{
    M2M_PERIPH_GPIO15, /*!< GPIO15 pad  */
    M2M_PERIPH_GPIO16, /*!< GPIO16 pad  */
    M2M_PERIPH_GPIO18, /*!< GPIO18 pad  */
    M2M_PERIPH_GPIO_MAX
} tenuGpioNum;

#define M2M_PERIPH_PULLUP_DIS_HOST_WAKEUP       (1ul << 0)
#define M2M_PERIPH_PULLUP_DIS_RTC_CLK           (1ul << 1)
#define M2M_PERIPH_PULLUP_DIS_IRQN              (1ul << 2)
#define M2M_PERIPH_PULLUP_DIS_GPIO_3            (1ul << 3)
#define M2M_PERIPH_PULLUP_DIS_GPIO_4            (1ul << 4)
#define M2M_PERIPH_PULLUP_DIS_GPIO_5            (1ul << 5)
#define M2M_PERIPH_PULLUP_DIS_GPIO_6            (1ul << 6)
#define M2M_PERIPH_PULLUP_DIS_SD_CLK            (1ul << 7)
#define M2M_PERIPH_PULLUP_DIS_SD_CMD_SPI_SCK    (1ul << 8)
#define M2M_PERIPH_PULLUP_DIS_SD_DAT0_SPI_TXD   (1ul << 9)
#define M2M_PERIPH_PULLUP_DIS_SD_DAT1_SPI_SSN   (1ul << 10)
#define M2M_PERIPH_PULLUP_DIS_SD_DAT1_SPI_RXD   (1ul << 11)
#define M2M_PERIPH_PULLUP_DIS_SD_DAT3           (1ul << 12)
#define M2M_PERIPH_PULLUP_DIS_GPIO_13           (1ul << 0)
#define M2M_PERIPH_PULLUP_DIS_GPIO_14           (1ul << 1)
#define M2M_PERIPH_PULLUP_DIS_GPIO_15           (1ul << 2)
#define M2M_PERIPH_PULLUP_DIS_GPIO_16           (1ul << 3)
#define M2M_PERIPH_PULLUP_DIS_GPIO_17           (1ul << 4)
#define M2M_PERIPH_PULLUP_DIS_GPIO_18           (1ul << 5)
#define M2M_PERIPH_PULLUP_DIS_GPIO_19           (1ul << 6)
#define M2M_PERIPH_PULLUP_DIS_GPIO_20           (1ul << 7)
#define M2M_PERIPH_PULLUP_DIS_GPIO_22           (1ul << 9)
#define M2M_PERIPH_PULLUP_DIS_GPIO_23           (1ul << 10)
#define M2M_PERIPH_PULLUP_DIS_GPIO_38           (1ul << 25)
#define M2M_PERIPH_PULLUP_DIS_GPIO_39           (1ul << 26)
#define M2M_PERIPH_PULLUP_DIS_GPIO_40           (1ul << 27)
#define M2M_PERIPH_PULLUP_DIS_GPIO_42           (1ul << 29)

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
extern "C" {
#endif

/*!
@fn         int8_t m2m_periph_gpio_set_dir(uint8_t u8GpioNum, uint8_t u8GpioDir);

@brief
            Configure a specific WINC3400 pad as a GPIO and sets its direction (input or output).

@param[in]  u8GpioNum
            GPIO number. Allowed values are defined in @ref tenuGpioNum.

@param[in]  u8GpioDir
            GPIO direction: Zero = input. Non-zero = output.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@sa
            tenuGpioNum
*/
int8_t m2m_periph_gpio_set_dir(uint8_t u8GpioNum, uint8_t u8GpioDir);

/*!
@fn         int8_t m2m_periph_gpio_set_val(uint8_t u8GpioNum, uint8_t u8GpioVal);

@brief
            Set an WINC3400 GPIO output level high or low.

@param[in]  u8GpioNum
            GPIO number. Allowed values are defined in @ref tenuGpioNum.

@param[in]  u8GpioVal
            GPIO output value. Zero = low, non-zero = high.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@sa
            tenuGpioNum
*/
int8_t m2m_periph_gpio_set_val(uint8_t u8GpioNum, uint8_t u8GpioVal);

/*!
@fn         int8_t m2m_periph_gpio_get_val(uint8_t u8GpioNum, uint8_t *pu8GpioVal);

@brief
            Read an WINC3400 GPIO input level.

@param[in]  u8GpioNum
            GPIO number. Allowed values are defined in @ref tenuGpioNum.

@param[out] pu8GpioVal
            GPIO input value. Zero = low, non-zero = high.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@sa
            tenuGpioNum
*/
int8_t m2m_periph_gpio_get_val(uint8_t u8GpioNum, uint8_t *pu8GpioVal);

/*!
@fn         int8_t m2m_periph_pullup_ctrl(uint32_t pinmask, uint8_t enable);

@brief
            Control the programmable pull-up resistor on the chip pads.

@param[in]  pinmask
            Write operation bitwise-ORed mask for which pads to control. Allowed values are defined in @ref tenuPullupMask.

@param[in]  enable
            Set to 0 to disable pull-up resistor. Non-zero will enable the pull-up.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@sa
            tenuPullupMask
*/
int8_t m2m_periph_pullup_ctrl(uint32_t pinmask, uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif /* _M2M_PERIPH_H_ */