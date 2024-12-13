/**
 *
 * \file
 *
 * \brief WINC3400 Peripherals Application Interface.
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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "bsp/winc_bsp.h"
#include "common/winc_defines.h"
#include "winc_m2m_periph.h"
#include "driver/winc_hif.h"
#include "driver/winc_asic.h"

#ifdef CONF_PERIPH
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define GPIO_OP_DIR     0
#define GPIO_OP_SET     1
#define GPIO_OP_GET     2
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
STATIC FUNCTIONS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
static int8_t get_gpio_idx(uint8_t u8GpioNum)
{
    if (u8GpioNum >= M2M_PERIPH_GPIO_MAX)
    {
        return -1;
    }

    if (u8GpioNum == M2M_PERIPH_GPIO15)
    {
        return 15;
    }
    else if (u8GpioNum == M2M_PERIPH_GPIO16)
    {
        return 16;
    }
    else if (u8GpioNum == M2M_PERIPH_GPIO18)
    {
        return 18;
    }
    else
    {
        return -2;
    }
}
/*
 * GPIO read/write skeleton with wakeup/sleep capability.
 */
static int8_t gpio_ioctl(uint8_t op, uint8_t u8GpioNum, uint8_t u8InVal, uint8_t *pu8OutVal)
{
    int8_t i8Ret, gpio;

    i8Ret = winc_hif_chip_wake();

    if (i8Ret != M2M_SUCCESS)
    {
        goto _EXIT;
    }

    gpio = get_gpio_idx(u8GpioNum);

    if (gpio < 0)
    {
        goto _EXIT1;
    }

    if (op == GPIO_OP_DIR)
    {
        if (!winc_chip_set_gpio_dir((uint_fast8_t)gpio, u8InVal))
        {
            i8Ret = M2M_ERR_FAIL;
        }
    }
    else if (op == GPIO_OP_SET)
    {
        if (!winc_chip_set_gpio_val((uint_fast8_t)gpio, u8InVal))
        {
            i8Ret = M2M_ERR_FAIL;
        }
    }
    else if (op == GPIO_OP_GET)
    {
        if (!winc_chip_get_gpio_val((uint_fast8_t)gpio, pu8OutVal))
        {
            i8Ret = M2M_ERR_FAIL;
        }
    }

_EXIT1:
    i8Ret = winc_hif_chip_sleep();
_EXIT:
    return i8Ret;
}

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION IMPLEMENTATION
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

int8_t m2m_periph_gpio_set_dir(uint8_t u8GpioNum, uint8_t u8GpioDir)
{
    return gpio_ioctl(GPIO_OP_DIR, u8GpioNum, u8GpioDir, NULL);
}

int8_t m2m_periph_gpio_set_val(uint8_t u8GpioNum, uint8_t u8GpioVal)
{
    return gpio_ioctl(GPIO_OP_SET, u8GpioNum, u8GpioVal, NULL);
}

int8_t m2m_periph_gpio_get_val(uint8_t u8GpioNum, uint8_t *pu8GpioVal)
{
    return gpio_ioctl(GPIO_OP_GET, u8GpioNum, 0, pu8GpioVal);
}

int8_t m2m_periph_pullup_ctrl(uint32_t pinmask, uint8_t enable)
{
    if (!winc_chip_pullup_ctrl(pinmask, enable))
    {
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

#endif /* CONF_PERIPH */