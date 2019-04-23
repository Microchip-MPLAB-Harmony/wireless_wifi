/*******************************************************************************
  WINC3400 Peripherals Application Interface.

  File Name:
    m2m_periph.h

  Summary:
    WINC3400 Peripherals Application Interface

  Description:
    WINC3400 Peripherals Application Interface
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

#ifndef _M2M_PERIPH_H_
#define _M2M_PERIPH_H_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "nm_common.h"
#include "m2m_types.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@struct \
    tstrPerphInitParam

@brief
    Peripheral module initialization parameters.
*/
typedef struct {
    void *arg;
} tstrPerphInitParam;

/*!
@enum   \
    tenuGpioNum

@brief
    A list of GPIO numbers configurable through the m2m_periph module.
*/
typedef enum {
    M2M_PERIPH_GPIO15, /*!< GPIO15 pad  */
    M2M_PERIPH_GPIO16, /*!< GPIO16 pad  */
    M2M_PERIPH_GPIO18, /*!< GPIO18 pad  */
    M2M_PERIPH_GPIO_MAX
} tenuGpioNum;

/*!
@enum   \
    tenuI2cMasterSclMuxOpt

@brief
    Allowed pin multiplexing options for I2C master SCL signal.
*/
typedef enum {
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_HOST_WAKEUP, /*!< I2C master SCL is available on HOST_WAKEUP. */
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_SD_DAT3,     /*!< I2C master SCL is available on SD_DAT3 (GPIO 7). */
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_GPIO13,      /*!< I2C master SCL is available on GPIO 13. */
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_GPIO4,       /*!< I2C master SCL is available on GPIO 4.*/
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_I2C_SCL,     /*!< I2C master SCL is available on I2C slave SCL. */
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_NUM
} tenuI2cMasterSclMuxOpt;

/*!
@enum   \
    tenuI2cMasterSdaMuxOpt

@brief
    Allowed pin multiplexing options for I2C master SDA signal.
*/
typedef enum {
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_RTC_CLK,  /*!< I2C master SDA is available on RTC_CLK. */
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_SD_CLK,   /*!< I2C master SDA is available on SD_CLK (GPIO 8). */
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_GPIO14,   /*!< I2C master SDA is available on GPIO 14. */
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_GPIO6,    /*!< I2C master SDA is available on GPIO 6.*/
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_I2C_SDA,  /*!< I2C master SDA is available on I2C slave SDA. */
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_NUM
} tenuI2cMasterSdaMuxOpt;

/*!
@struct \
    tstrI2cMasterInitParam

@brief
    I2C master configuration parameters.
@sa
    tenuI2cMasterSclMuxOpt
    tenuI2cMasterSdaMuxOpt
*/
typedef struct {
    uint8_t enuSclMuxOpt; /*!< SCL multiplexing option. Allowed value are defined in tenuI2cMasterSclMuxOpt  */
    uint8_t enuSdaMuxOpt; /*!< SDA multiplexing option. Allowed value are defined in tenuI2cMasterSdaMuxOpt  */
    uint8_t u8ClkSpeedKHz; /*!< I2C master clock speed in KHz. */
} tstrI2cMasterInitParam;

/*!
@enum   \
    tenuI2cMasterFlags

@brief
    Bitwise-ORed flags for use in m2m_periph_i2c_master_write and m2m_periph_i2c_master_read
@sa
    m2m_periph_i2c_master_write
    m2m_periph_i2c_master_read
*/
typedef enum  {
    I2C_MASTER_NO_FLAGS          = 0x00,
    /*!< No flags.  */
    I2C_MASTER_NO_STOP           = 0x01,
    /*!< No stop bit after this transaction. Useful for scattered buffer read/write operations. */
    I2C_MASTER_NO_START          = 0x02,
    /*!< No start bit at the beginning of this transaction. Useful for scattered buffer read/write operations.*/
} tenuI2cMasterFlags;

/*!
@enum   \
    tenuPullupMask

@brief
    Bitwise-ORed flags for use in m2m_perph_pullup_ctrl.
@sa
    m2m_periph_pullup_ctrl

*/
typedef enum {
    M2M_PERIPH_PULLUP_DIS_HOST_WAKEUP       = (1ul << 0),
    M2M_PERIPH_PULLUP_DIS_RTC_CLK           = (1ul << 1),
    M2M_PERIPH_PULLUP_DIS_IRQN              = (1ul << 2),
    M2M_PERIPH_PULLUP_DIS_GPIO_3            = (1ul << 3),
    M2M_PERIPH_PULLUP_DIS_GPIO_4            = (1ul << 4),
    M2M_PERIPH_PULLUP_DIS_GPIO_5            = (1ul << 5),
    M2M_PERIPH_PULLUP_DIS_GPIO_6            = (1ul << 6),
    M2M_PERIPH_PULLUP_DIS_SD_CLK            = (1ul << 7),
    M2M_PERIPH_PULLUP_DIS_SD_CMD_SPI_SCK    = (1ul << 8),
    M2M_PERIPH_PULLUP_DIS_SD_DAT0_SPI_TXD   = (1ul << 9),
    M2M_PERIPH_PULLUP_DIS_SD_DAT1_SPI_SSN   = (1ul << 10),
    M2M_PERIPH_PULLUP_DIS_SD_DAT1_SPI_RXD   = (1ul << 11),
    M2M_PERIPH_PULLUP_DIS_SD_DAT3           = (1ul << 12),
} tenuPullupEnable1Mask;
typedef enum {
    M2M_PERIPH_PULLUP_DIS_GPIO_13           = (1ul << 0),
    M2M_PERIPH_PULLUP_DIS_GPIO_14           = (1ul << 1),
    M2M_PERIPH_PULLUP_DIS_GPIO_15           = (1ul << 2),
    M2M_PERIPH_PULLUP_DIS_GPIO_16           = (1ul << 3),
    M2M_PERIPH_PULLUP_DIS_GPIO_17           = (1ul << 4),
    M2M_PERIPH_PULLUP_DIS_GPIO_18           = (1ul << 5),
    M2M_PERIPH_PULLUP_DIS_GPIO_19           = (1ul << 6),
    M2M_PERIPH_PULLUP_DIS_GPIO_20           = (1ul << 7),
    M2M_PERIPH_PULLUP_DIS_GPIO_22           = (1ul << 9),
    M2M_PERIPH_PULLUP_DIS_GPIO_23           = (1ul << 10),
    M2M_PERIPH_PULLUP_DIS_GPIO_38           = (1ul << 25),
    M2M_PERIPH_PULLUP_DIS_GPIO_39           = (1ul << 26),
    M2M_PERIPH_PULLUP_DIS_GPIO_40           = (1ul << 27),
    M2M_PERIPH_PULLUP_DIS_GPIO_42           = (1ul << 29),
} tenuPullupEnable2Mask;
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
extern "C" {
#endif

/*!
@fn \
    int8_t m2m_periph_init(tstrPerphInitParam * param);

@brief
    Initialize the WINC3400 peripheral driver module.

@param [in] param
                Peripheral module initialization structure. See members of tstrPerphInitParam.

@return
    The function SHALL return 0 for success and a negative value otherwise.

@sa
    tstrPerphInitParam
*/
int8_t m2m_periph_init(tstrPerphInitParam *param);

/*!
@fn \
    int8_t m2m_periph_gpio_set_dir(uint8_t u8GpioNum, uint8_t u8GpioDir);

@brief
    Configure a specific WINC3400 pad as a GPIO and sets its direction (input or output).

@param [in] u8GpioNum
                GPIO number. Allowed values are defined in tenuGpioNum.

@param [in] u8GpioDir
                GPIO direction: Zero = input. Non-zero = output.

@return
    The function SHALL return 0 for success and a negative value otherwise.

@sa
    tenuGpioNum
*/
int8_t m2m_periph_gpio_set_dir(uint8_t u8GpioNum, uint8_t u8GpioDir);

/*!
@fn \
    int8_t m2m_periph_gpio_set_val(uint8_t u8GpioNum, uint8_t u8GpioVal);

@brief
    Set an WINC3400 GPIO output level high or low.

@param [in] u8GpioNum
                GPIO number. Allowed values are defined in tenuGpioNum.

@param [in] u8GpioVal
                GPIO output value. Zero = low, non-zero = high.

@return
    The function SHALL return 0 for success and a negative value otherwise.

@sa
    tenuGpioNum
*/
int8_t m2m_periph_gpio_set_val(uint8_t u8GpioNum, uint8_t u8GpioVal);

/*!
@fn \
    int8_t m2m_periph_gpio_get_val(uint8_t u8GpioNum, uint8_t * pu8GpioVal);

@brief
    Read an WINC3400 GPIO input level.

@param [in] u8GpioNum
                GPIO number. Allowed values are defined in tenuGpioNum.

@param [out] pu8GpioVal
                GPIO input value. Zero = low, non-zero = high.

@return
    The function SHALL return 0 for success and a negative value otherwise.

@sa
    tenuGpioNum
*/
int8_t m2m_periph_gpio_get_val(uint8_t u8GpioNum, uint8_t *pu8GpioVal);

/*!
@fn \
    int8_t m2m_periph_pullup_ctrl(uint32_t pinmask, uint8_t enable);

@brief
    Control the programmable pull-up resistor on the chip pads .


@param [in] pinmask
                Write operation bitwise-ORed mask for which pads to control. Allowed values are defined in tenuPullupMask.

@param [in] enable
                Set to 0 to disable pull-up resistor. Non-zero will enable the pull-up.

@return
    The function SHALL return 0 for success and a negative value otherwise.

@sa
    tenuPullupMask
*/
int8_t m2m_periph_pullup_ctrl(uint32_t pinmask, uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif /* _M2M_PERIPH_H_ */