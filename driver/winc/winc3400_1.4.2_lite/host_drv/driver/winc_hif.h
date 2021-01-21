/*!
 *
 * \file
 *
 * \brief This module contains host interface APIs implementation.
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

#ifndef _WINC_HIF_H_
#define _WINC_HIF_H_

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define M2M_HIF_MAX_PACKET_SIZE      (1600 - 4)
/*!< Maximum size of the buffer could be transferred between Host and Firmware. */

#define M2M_HIF_HDR_OFFSET (sizeof(tstrHifHdr) + 4)

/*!
@struct     tstrHifHdr
@brief      Structure to hold HIF header
*/
typedef struct
{
    uint8_t     u8Gid;      /*!< Group ID */
    uint8_t     u8Opcode;   /*!< OP code */
    uint16_t    u16Length;  /*!< Payload length */
} tstrHifHdr;

#ifdef __cplusplus
extern "C" {
#endif

/*!
@typedef    typedef void (*tpfHifCallBack)(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
@brief      Used to point to WiFi call back function.
@param[in]  u8OpCode
            HIF Opcode type.
@param[in]  u16DataSize
            HIF data length.
@param[in]  u32Addr
            HIF address.
@param[in]  grp
            HIF group type.
*/
typedef void (*tpfHifCallBack)(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr);
/*!
@fn         int8_t winc_hif_init(void);
@brief
            To initialize HIF layer.
@return
            The function returns 0 for successful operation and a negative value otherwise.
*/
int8_t winc_hif_init(void);
/*!
@fn         int8_t winc_hif_deinit(void);
@brief
            To Deinitialize HIF layer.
@return
            The function returns 0 for successful operation and a negative value otherwise.
*/
int8_t winc_hif_deinit(void);
/*!
@fn         int8_t winc_hif_send(uint8_t u8Gid, uint8_t u8Opcode, const void *pvCtrlBuf, uint16_t u16CtrlBufSize,
                       const void *pvDataBuf, uint16_t u16DataSize, uint16_t u16DataOffset)
@brief      Send packet using host interface.

@param[in]  u8Gid
            Group ID.
@param[in]  u8Opcode
            Operation ID.
@param[in]  pvCtrlBuf
            Pointer to the Control buffer.
@param[in]  u16CtrlBufSize
            Control buffer size.
@param[in]  pvDataBuf
            Packet buffer Allocated by the caller.
@param[in]  u16DataSize
            Packet buffer size (including the HIF header).
@param[in]  u16DataOffset
            Packet Data offset.
@return     The function returns 0 for successful operation and a negative value otherwise.
*/
int8_t winc_hif_send(uint8_t u8Gid, uint8_t u8Opcode, const void *pvCtrlBuf, uint16_t u16CtrlBufSize,
                     const void *pvDataBuf, uint16_t u16DataSize, uint16_t u16DataOffset);
/*!
@fn         int8_t winc_hif_send_no_data(uint8_t u8Gid, uint8_t u8Opcode, const void *pvCtrlBuf, uint16_t u16CtrlBufSize)
@brief      Send packet using host interface.

@param[in]  u8Gid
            Group ID.
@param[in]  u8Opcode
            Operation ID.
@param[in]  pvCtrlBuf
            Pointer to the Control buffer.
@param[in]  u16CtrlBufSize
            Control buffer size.
@return
            The function returns 0 for successful operation and a negative value otherwise.
*/
int8_t winc_hif_send_no_data(uint8_t u8Gid, uint8_t u8Opcode, const void *pvCtrlBuf, uint16_t u16CtrlBufSize);
/*
@fn         winc_hif_receive
@brief      Host interface interrupt service routine
@param[in]  u32Addr
            Receive start address
@param[out] pvBuf
            Pointer to receive buffer. Allocated by the caller
@param[in]  u16Sz
            Receive buffer size
@return
            The function returns 0 for successful operation and a negative value otherwise.
*/
int8_t winc_hif_receive(uint32_t u32Addr, const void *pvBuf, uint_fast16_t u16Sz);
/*!
@fn         int8_t winc_hif_chip_sleep(void);
@brief
            To make the chip sleep.
@return
            The function returns 0 for successful operation and a negative value otherwise.
*/
int8_t winc_hif_chip_sleep(void);
/*!
@fn         int8_t winc_hif_chip_wake(void);
@brief
            To Wakeup the chip.
@return
            The function returns 0 for successful operation and a negative value otherwise.
*/
int8_t winc_hif_chip_wake(void);
/*!
@fn         void winc_hif_set_power_save(bool bPwrSave);

@brief
            Set the sleep mode of the HIF layer.

@param[in]  bPwrSave
            Is power save active.
*/
void winc_hif_set_power_save(bool bPwrSave);
/*!
@fn         winc_hif_handle_isr(void)
@brief
            Handle interrupt received from WINC firmware.
@return
            The function returns 0 for success and a negative value otherwise.
*/
int8_t winc_hif_handle_isr(void);

#ifdef __cplusplus
}
#endif
#endif
