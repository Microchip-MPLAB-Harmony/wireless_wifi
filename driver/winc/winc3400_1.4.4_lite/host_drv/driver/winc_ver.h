/**
 *
 * \file
 *
 * \brief This module contains WINC Version APIs declarations.
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