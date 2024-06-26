/*******************************************************************************
  This module contains WINC1500 bus APIs implementation.

  File Name:
    nmbus.h

  Summary:
    This module contains WINC1500 bus APIs implementation.

  Description:
    This module contains WINC1500 bus APIs implementation.
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

#ifndef _NMBUS_H_
#define _NMBUS_H_

#include "nm_common.h"

#ifdef __cplusplus
extern "C"{
#endif
/**
*   @fn     nm_bus_iface_init
*   @brief  Initialize bus interface
*   @return M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
int8_t nm_bus_iface_init(void *);


/**
*   @fn     nm_bus_iface_deinit
*   @brief  Deinitialize bus interface
*   @return M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
int8_t nm_bus_iface_deinit(void);

/**
*   @fn     nm_bus_reset
*   @brief  reset bus interface
*   @return M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*   @version    1.0
*/
int8_t nm_bus_reset(void);

/**
*   @fn     nm_bus_iface_reconfigure
*   @brief  reconfigure bus interface
*   @return M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
int8_t nm_bus_iface_reconfigure(void *ptr);

/**
*   @fn     nm_read_reg
*   @brief  Read register
*   @param [in] u32Addr
*               Register address
*   @return Register value
*/
uint32_t nm_read_reg(uint32_t u32Addr);

/**
*   @fn     nm_read_reg_with_ret
*   @brief  Read register with error code return
*   @param [in] u32Addr
*               Register address
*   @param [out]    pu32RetVal
*               Pointer to u32 variable used to return the read value
*   @return ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
int8_t nm_read_reg_with_ret(uint32_t u32Addr, uint32_t* pu32RetVal);

/**
*   @fn     nm_write_reg
*   @brief  write register
*   @param [in] u32Addr
*               Register address
*   @param [in] u32Val
*               Value to be written to the register
*   @return ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
int8_t nm_write_reg(uint32_t u32Addr, uint32_t u32Val);

/**
*   @fn     nm_read_block
*   @brief  Read block of data
*   @param [in] u32Addr
*               Start address
*   @param [out]    puBuf
*               Pointer to a buffer used to return the read data
*   @param [in] u32Sz
*               Number of bytes to read. The buffer size must be >= u32Sz
*   @return ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
int8_t nm_read_block(uint32_t u32Addr, uint8_t *puBuf, uint32_t u32Sz);

/**
*   @fn     nm_write_block
*   @brief  Write block of data
*   @param [in] u32Addr
*               Start address
*   @param [in] puBuf
*               Pointer to the buffer holding the data to be written
*   @param [in] u32Sz
*               Number of bytes to write. The buffer size must be >= u32Sz
*   @return ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
*/
int8_t nm_write_block(uint32_t u32Addr, uint8_t *puBuf, uint32_t u32Sz);




#ifdef __cplusplus
}
#endif

#endif /* _NMBUS_H_ */
