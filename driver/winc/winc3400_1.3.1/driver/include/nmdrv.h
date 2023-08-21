/*******************************************************************************
  This module contains WINC3400 M2M driver APIs declarations.

  File Name:
    nmdrv.h

  Summary:
    This module contains WINC3400 M2M driver APIs declarations.

  Description:
    This module contains WINC3400 M2M driver APIs declarations.
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

#ifndef _NMDRV_H_
#define _NMDRV_H_

#include "nm_common.h"

/*!
@enum   \
    tenuNmState
@brief
    Enumeration for WINC state
    The following is used to track the state of the WINC (not initialized, initialized or started)

@remarks
    This is useful when putting the WINC in "download mode" to access the flash via SPI. By using
    @ref nm_get_state and checking against the desired state, it is possible to validate whether
    it is safe to proceed with SPI Flash access.
*/
typedef enum {
    NM_STATE_DEINIT,
    /*!< WINC is not initialized */
    NM_STATE_INIT,
    /*!< WINC has been initialized. SPI flash access is possible. */
    NM_STATE_START,
    /*!< WINC has started */
} tenuNmState;

/**
*  @struct      tstrM2mRev
*  @brief       Structure holding firmware version parameters and build date/time
*/
typedef struct {
    uint16_t u16FirmwareHifInfo; /* Fw HIF Info */
    uint8_t u8FirmwareMajor; /* Version Major Number */
    uint8_t u8FirmwareRsvd; /* Reserved */
    uint8_t u8FirmwareMinor; /* Version Minor */
    uint8_t u8FirmwarePatch; /* Patch Number */
    uint8_t BuildDate[sizeof(__DATE__)]; // 12 bytes
    uint8_t BuildTime[sizeof(__TIME__)]; // 9 bytes
} tstrM2mRev;

#ifdef __cplusplus
extern "C" {
#endif
/**
 *  @fn         nm_get_hif_info(uint16_t *pu16FwHifInfo, uint16_t *pu16OtaHifInfo);
 *  @brief      Get Hif info of images in both partitions (Firmware and Ota).
 *  @param[out] pu16FwHifInfo
 *                  Pointer holding Hif info of image in the active partition.
 *  @param[out] pu16OtaHifInfo
 *                  Pointer holding Hif info of image in the inactive partition.
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_get_hif_info(uint16_t *pu16FwHifInfo, uint16_t *pu16OtaHifInfo);

/**
 *  @fn         nm_get_firmware_full_info(tstrM2mRev* pstrRev)
 *  @brief      Get Firmware version info
 *  @param[out] pstrRev
 *                  Pointer holds address of structure @ref tstrM2mRev that contains the version parameters
 *                  of image in the active partition.
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_get_firmware_full_info(tstrM2mRev* pstrRev);

/**
 *  @fn         nm_get_ota_firmware_info(tstrM2mRev* pstrRev)
 *  @brief      Get Firmware version info
 *  @param[out] pstrRev
 *                  Pointer holds address of structure @ref tstrM2mRev that contains the version parameters
 *                  of image in the inactive partition.
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_get_ota_firmware_info(tstrM2mRev* pstrRev);

/**
 *  @fn         nm_drv_init_download_mode
 *  @brief      Initialize NMC1000 driver in download mode
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_drv_init_download_mode(void);

/**
 *  @fn         nm_drv_init
 *  @brief      Initialize NMC1000 driver
 *  @param[in]  arg
 *                  Generic argument passed on to nm_drv_init_start
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_drv_init(void* arg);

/**
 *  @fn         nm_drv_init_hold
 *  @brief      First part of nm_drv_init, up to the point of initializing spi for flash access.
 *  @see        nm_drv_init
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_drv_init_hold(void);

/**
 *  @fn         nm_drv_init_start
 *  @brief      Second part of nm_drv_init, continuing from where nm_drv_init_hold left off.
 *  @see        nm_drv_init
 *  @param[in]  arg
 *                  Parameter inherited from nm_drv_init
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_drv_init_start(void* arg);

/**
 *  @fn         nm_drv_deinit
 *  @brief      Deinitialize NMC1000 driver
 *  @param[in]  arg
 *                  Generic argument unused.
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_drv_deinit(void* arg);

/**
 *  @fn         nm_cpu_start(void)
 *  @brief      Start CPU from the WINC module
 *  @return     @ref M2M_SUCCESS in case of success and Negative error code in case of failure
 */
int8_t nm_cpu_start(void);

/**
 *  @fn         nm_get_state(void)
 *  @brief      Get the current state of the WINC module
 *  @return     The current state of the WINC module
 */
tenuNmState nm_get_state(void);

#ifdef __cplusplus
}
#endif

#endif  /*_NMDRV_H_*/


