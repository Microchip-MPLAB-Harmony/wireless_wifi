/*!
 *
 * \file
 *
 * \brief This module contains host interface APIs implementation.
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
