/**
 *
 * \file
 *
 * \brief This module contains WINC Version APIs declarations.
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

#ifndef _WINC_VER_H_
#define _WINC_VER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
*   @fn     winc_get_hif_info(uint16_t *pu16FwHifInfo, uint16_t *pu16OtaHifInfo);
*   @brief  Get Hif info of images in both partitions (Firmware and OTA).
*   @param [out]    pu16FwHifInfo
*                   Pointer holding Hif info of image in the active partition.
*   @param [out]    pu16OtaHifInfo
*                   Pointer holding Hif info of image in the inactive partition.
*   @return ZERO in case of success and Negative error code in case of failure
*/
int_fast8_t winc_get_hif_info(uint16_t *pu16FwHifInfo, uint16_t *pu16OtaHifInfo);
/**
*   @fn         int8_t winc_hif_check_compatibility(uint16_t u16HifInfo);
*   @brief
*               To check the compatibility of an image with the current driver.
*   @param [in] u16HifInfo
*               HIF info of image to be checked.
*   @return     The function shall return ZERO for compatible image and a negative value otherwise.
*/
int8_t winc_hif_check_compatibility(uint16_t u16HifInfo);
/**
*   @fn         int8_t winc_hif_enable_access(void);
*   @brief
*               To enable access to HIF layer, based on HIF level of Firmware.
*               This function reads HIF level directly from a register written by Firmware.
*   @return     The function shall return ZERO for full match operation and a negative value if operation is restricted.
*/
int8_t winc_hif_enable_access(void);
/**
*   @fn         int8_t winc_hif_check_code(uint8_t u8Gid, uint8_t u8OpCode);
*   @brief
*               To check that a particular hif message is supported with the current driver/firmware pair.
*   @param [in] u8Gid
*               Group ID.
*   @param [in] u8Opcode
*               Operation ID.
*   @return     The function shall return ZERO for support and a negative value otherwise.
*/
int8_t winc_hif_check_code(uint8_t u8Gid, uint8_t u8OpCode);

#ifdef __cplusplus
}
#endif

#endif /* _WINC_VER_H_ */