/**
 *
 * \file
 *
 * \brief WINC Driver Common API Definitions.
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
#define M2M_ERR_INVALID             (-16)

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
