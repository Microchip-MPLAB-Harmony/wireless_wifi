/**
 *
 * \file
 *
 * \brief WINC Driver Common API Definitions.
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

/** @defgroup COMMON Common
    @{
        @defgroup COMMONDEF Defines
    @}
 */

#ifndef _WINC_DEFINES_H_
#define _WINC_DEFINES_H_

/**@addtogroup COMMONDEF
 * @{
 */
#define M2M_SUCCESS                 (0)
#define M2M_ERR_SEND                (-1)
#define M2M_ERR_RCV                 (-2)
#define M2M_ERR_MEM_ALLOC           (-3)
#define M2M_ERR_TIME_OUT            (-4)
#define M2M_ERR_INIT                (-5)
#define M2M_ERR_BUS_FAIL            (-6)
#define M2M_NOT_YET                 (-7)
#define M2M_ERR_FIRMWARE            (-8)
#define M2M_SPI_FAIL                (-9)
#define M2M_ACK                     (-11)
#define M2M_ERR_FAIL                (-12)
#define M2M_ERR_FW_VER_MISMATCH     (-13)
#define M2M_ERR_SCAN_IN_PROGRESS    (-14)
#define M2M_ERR_INVALID_ARG         (-15)

#define NBIT31                      (0x80000000)
#define NBIT30                      (0x40000000)
#define NBIT29                      (0x20000000)
#define NBIT28                      (0x10000000)
#define NBIT27                      (0x08000000)
#define NBIT26                      (0x04000000)
#define NBIT25                      (0x02000000)
#define NBIT24                      (0x01000000)
#define NBIT23                      (0x00800000)
#define NBIT22                      (0x00400000)
#define NBIT21                      (0x00200000)
#define NBIT20                      (0x00100000)
#define NBIT19                      (0x00080000)
#define NBIT18                      (0x00040000)
#define NBIT17                      (0x00020000)
#define NBIT16                      (0x00010000)
#define NBIT15                      (0x00008000)
#define NBIT14                      (0x00004000)
#define NBIT13                      (0x00002000)
#define NBIT12                      (0x00001000)
#define NBIT11                      (0x00000800)
#define NBIT10                      (0x00000400)
#define NBIT9                       (0x00000200)
#define NBIT8                       (0x00000100)
#define NBIT7                       (0x00000080)
#define NBIT6                       (0x00000040)
#define NBIT5                       (0x00000020)
#define NBIT4                       (0x00000010)
#define NBIT3                       (0x00000008)
#define NBIT2                       (0x00000004)
#define NBIT1                       (0x00000002)
#define NBIT0                       (0x00000001)

#ifdef CONF_WINC_MCU_ARCH_LITTLE_ENDIAN
/*! Most significant byte of 32bit word (LE) */
#define WINC_BYTE_0(word)               ((uint8_t)(((word) >> 0 ) & 0xFF))
/*! Second most significant byte of 32bit word (LE) */
#define WINC_BYTE_1(word)               ((uint8_t)(((word) >> 8 ) & 0xFF))
/*! Third most significant byte of 32bit word (LE) */
#define WINC_BYTE_2(word)               ((uint8_t)(((word) >> 16) & 0xFF))
/*! Least significant byte of 32bit word (LE) */
#define WINC_BYTE_3(word)               ((uint8_t)(((word) >> 24) & 0xFF))
#endif

#ifdef CONF_WINC_MCU_ARCH_BIG_ENDIAN
/*! Most significant byte of 32bit word (BE) */
#define WINC_BYTE_0(word)               ((uint8_t)(((word) >> 24) & 0xFF))
/*! Second most significant byte of 32bit word (BE) */
#define WINC_BYTE_1(word)               ((uint8_t)(((word) >> 16) & 0xFF))
/*! Third most significant byte of 32bit word (BE) */
#define WINC_BYTE_2(word)               ((uint8_t)(((word) >> 8 ) & 0xFF))
/*! Least significant byte of 32bit word (BE) */
#define WINC_BYTE_3(word)               ((uint8_t)(((word) >> 0 ) & 0xFF))
#endif

#define UNUSED_VAR(VAR) (void)(VAR)

/**@}*/

#endif  /*_WINC_DEFINES_H_*/
