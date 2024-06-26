/*******************************************************************************
  WINC1500 Peripherals Application Interface

  File Name:
    m2m_periph.h

  Summary:
    WINC1500 Peripherals Application Interface

  Description:
    WINC1500 Peripherals Application Interface
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2019, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
    void * arg;
} tstrPerphInitParam;


/*!
@enum   \
    tenuGpioNum

@brief
    A list of GPIO numbers configurable through the m2m_periph module.
*/
typedef enum {
    M2M_PERIPH_GPIO3, /*!< GPIO15 pad   */
    M2M_PERIPH_GPIO4, /*!< GPIO16 pad   */
    M2M_PERIPH_GPIO5, /*!< GPIO18 pad   */
    M2M_PERIPH_GPIO6, /*!< GPIO18 pad   */
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
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_HOST_WAKEUP, /*!< I2C master SCL is avaiable on HOST_WAKEUP. */
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_SD_DAT3,     /*!< I2C master SCL is avaiable on SD_DAT3 (GPIO 7). */
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_GPIO13,      /*!< I2C master SCL is avaiable on GPIO 13. */
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_GPIO4,       /*!< I2C master SCL is avaiable on GPIO 4.*/
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_I2C_SCL,     /*!< I2C master SCL is avaiable on I2C slave SCL. */
    M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_NUM
} tenuI2cMasterSclMuxOpt;

/*!
@enum   \
    tenuI2cMasterSdaMuxOpt

@brief
    Allowed pin multiplexing options for I2C master SDA signal.
*/
typedef enum {
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_RTC_CLK , /*!< I2C master SDA is avaiable on RTC_CLK. */
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_SD_CLK,   /*!< I2C master SDA is avaiable on SD_CLK (GPIO 8). */
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_GPIO14,   /*!< I2C master SDA is avaiable on GPIO 14. */
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_GPIO6,    /*!< I2C master SDA is avaiable on GPIO 6.*/
    M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_I2C_SDA,  /*!< I2C master SDA is avaiable on I2C slave SDA. */
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
    M2M_PERIPH_PULLUP_DIS_HOST_WAKEUP     = (1ul << 0),
    M2M_PERIPH_PULLUP_DIS_RTC_CLK         = (1ul << 1),
    M2M_PERIPH_PULLUP_DIS_IRQN            = (1ul << 2),
    M2M_PERIPH_PULLUP_DIS_GPIO_3          = (1ul << 3),
    M2M_PERIPH_PULLUP_DIS_GPIO_4          = (1ul << 4),
    M2M_PERIPH_PULLUP_DIS_GPIO_5          = (1ul << 5),
    M2M_PERIPH_PULLUP_DIS_SD_DAT3         = (1ul << 6),
    M2M_PERIPH_PULLUP_DIS_SD_DAT2_SPI_RXD = (1ul << 7),
    M2M_PERIPH_PULLUP_DIS_SD_DAT1_SPI_SSN = (1ul << 9),
    M2M_PERIPH_PULLUP_DIS_SD_CMD_SPI_SCK  = (1ul << 10),
    M2M_PERIPH_PULLUP_DIS_SD_DAT0_SPI_TXD = (1ul << 11),
    M2M_PERIPH_PULLUP_DIS_GPIO_6          = (1ul << 12),
    M2M_PERIPH_PULLUP_DIS_SD_CLK          = (1ul << 13),
    M2M_PERIPH_PULLUP_DIS_I2C_SCL         = (1ul << 14),
    M2M_PERIPH_PULLUP_DIS_I2C_SDA         = (1ul << 15),
    M2M_PERIPH_PULLUP_DIS_GPIO_11         = (1ul << 16),
    M2M_PERIPH_PULLUP_DIS_GPIO_12         = (1ul << 17),
    M2M_PERIPH_PULLUP_DIS_GPIO_13         = (1ul << 18),
    M2M_PERIPH_PULLUP_DIS_GPIO_14         = (1ul << 19),
    M2M_PERIPH_PULLUP_DIS_GPIO_15         = (1ul << 20),
    M2M_PERIPH_PULLUP_DIS_GPIO_16         = (1ul << 21),
    M2M_PERIPH_PULLUP_DIS_GPIO_17         = (1ul << 22),
    M2M_PERIPH_PULLUP_DIS_GPIO_18         = (1ul << 23),
    M2M_PERIPH_PULLUP_DIS_GPIO_19         = (1ul << 24),
    M2M_PERIPH_PULLUP_DIS_GPIO_20         = (1ul << 25),
    M2M_PERIPH_PULLUP_DIS_GPIO_21         = (1ul << 26),
    M2M_PERIPH_PULLUP_DIS_GPIO_22         = (1ul << 27),
    M2M_PERIPH_PULLUP_DIS_GPIO_23         = (1ul << 28),
    M2M_PERIPH_PULLUP_DIS_GPIO_24         = (1ul << 29),
} tenuPullupMask;

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
    Initialize the NMC1500 peripheral driver module.

@param [in] param
                Peripheral module initialization structure. See members of tstrPerphInitParam.

@return
    The function SHALL return 0 for success and a negative value otherwise.

@sa
    tstrPerphInitParam
*/
int8_t m2m_periph_init(tstrPerphInitParam * param);

/*!
@fn \
    int8_t m2m_periph_gpio_set_dir(uint8_t u8GpioNum, uint8_t u8GpioDir);

@brief
    Configure a specific NMC1500 pad as a GPIO and sets its direction (input or output).

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
    Set an NMC1500 GPIO output level high or low.

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
    Read an NMC1500 GPIO input level.

@param [in] u8GpioNum
                GPIO number. Allowed values are defined in tenuGpioNum.

@param [out] pu8GpioVal
                GPIO input value. Zero = low, non-zero = high.

@return
    The function SHALL return 0 for success and a negative value otherwise.

@sa
    tenuGpioNum
*/
int8_t m2m_periph_gpio_get_val(uint8_t u8GpioNum, uint8_t * pu8GpioVal);

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