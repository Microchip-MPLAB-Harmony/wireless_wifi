/**
 *
 * \file
 *
 * \brief This module contains the WINC serial bridge.
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

#include "bsp/winc_bsp.h"
#include "winc_serial_bridge.h"
#include "driver/winc_spi.h"
#include "driver/winc_drv.h"
#include "driver/winc_asic.h"

#ifdef CONF_WINC_SERIAL_BRIDGE_INCLUDE

#define SB_CMD_BUFFER_SIZE  128
#define SB_HEADER_SIZE      12

typedef enum
{
    SERIAL_BRIDGE_STATE_UNKNOWN,
    SERIAL_BRIDGE_STATE_WAIT_OP_CODE,
    SERIAL_BRIDGE_STATE_WAIT_HEADER,
    SERIAL_BRIDGE_STATE_WAIT_PAYLOAD,
    SERIAL_BRIDGE_STATE_PROCESS_COMMAND
} tenuState;

typedef struct
{
    tenuState       enuState;
    uint8_t         au8DataBuf[SB_CMD_BUFFER_SIZE];
    uint_fast16_t   u16RxDataLen;
    uint8_t         u8CmdType;
    uint16_t        u16CmdSize;
    uint32_t        u32CmdAddr;
    uint32_t        u32CmdVal;
    uint_fast16_t   u16PayloadLen;
} tstrSBState;

typedef enum
{
    SB_COMMAND_READ_REG_WITH_RET = 0,
    SB_COMMAND_WRITE_REG         = 1,
    SB_COMMAND_READ_BLOCK        = 2,
    SB_COMMAND_WRITE_BLOCK       = 3,
    SB_COMMAND_RECONFIGURE       = 5
} tenuSBCommand;

typedef enum
{
    SB_RESPONSE_NACK        = 0x5a,
    SB_RESPONSE_ID_VAR_BR   = 0x5b,
    SB_RESPONSE_ID_FIXED_BR = 0x5c,
    SB_RESPONSE_ACK         = 0xac
} tenuSBResponse;

static tstrSBState strSBState;

static bool sb_process_header(uint8_t *pau8Hdr)
{
    uint8_t u8ChkSum;
    uint8_t i;

    if (NULL == pau8Hdr)
    {
        return false;
    }

    u8ChkSum = 0;

    for (i = 0; i < SB_HEADER_SIZE; i++)
    {
        u8ChkSum ^= pau8Hdr[i];
    }

    if (0 != u8ChkSum)
    {
        return false;
    }

    strSBState.u8CmdType = pau8Hdr[0];
    strSBState.u16CmdSize = (((uint16_t)pau8Hdr[3]) << 8) | pau8Hdr[2];
    strSBState.u32CmdAddr = (((uint32_t)pau8Hdr[7]) << 24) | (((uint32_t)pau8Hdr[6]) << 16) | (((uint32_t)pau8Hdr[5]) << 8) | pau8Hdr[4];
    strSBState.u32CmdVal  = (((uint32_t)pau8Hdr[11]) << 24) | (((uint32_t)pau8Hdr[10]) << 16) | (((uint32_t)pau8Hdr[9]) << 8) | pau8Hdr[8];

    if (strSBState.u8CmdType == SB_COMMAND_WRITE_BLOCK)
    {
        strSBState.u16PayloadLen = strSBState.u16CmdSize;

        if (strSBState.u16PayloadLen > SB_CMD_BUFFER_SIZE)
        {
            return false;
        }
    }
    else
    {
        strSBState.u16PayloadLen = 0;
    }

    return true;
}

static bool sb_process_command(void)
{
    uint_fast16_t u16Cnt;

    switch (strSBState.u8CmdType)
    {
        case SB_COMMAND_READ_REG_WITH_RET:
        {
            uint32_t u32RegVal;

            u32RegVal = winc_bus_read_reg(strSBState.u32CmdAddr);

            strSBState.au8DataBuf[0] = (u32RegVal >> 24) & 0xff;
            strSBState.au8DataBuf[1] = (u32RegVal >> 16) & 0xff;
            strSBState.au8DataBuf[2] = (u32RegVal >> 8) & 0xff;
            strSBState.au8DataBuf[3] = (u32RegVal) & 0xff;

            if (4 != winc_bsp_uart_write(strSBState.au8DataBuf, 4))
            {
                return false;
            }

            return true;
        }

        case SB_COMMAND_WRITE_REG:
        {
            winc_bus_write_reg(strSBState.u32CmdAddr, strSBState.u32CmdVal);

            return true;
        }

        case SB_COMMAND_READ_BLOCK:
        {
            u16Cnt = strSBState.u16CmdSize;

            while (u16Cnt >= SB_CMD_BUFFER_SIZE)
            {
                if (WINC_BUS_SUCCESS != winc_bus_read_block(strSBState.u32CmdAddr, strSBState.au8DataBuf, SB_CMD_BUFFER_SIZE))
                {
                    return false;
                }

                if (SB_CMD_BUFFER_SIZE != winc_bsp_uart_write(strSBState.au8DataBuf, SB_CMD_BUFFER_SIZE))
                {
                    return false;
                }
            }

            if (u16Cnt)
            {
                if (WINC_BUS_SUCCESS != winc_bus_read_block(strSBState.u32CmdAddr, strSBState.au8DataBuf, u16Cnt))
                {
                    return false;
                }

                if (strSBState.u16CmdSize != winc_bsp_uart_write(strSBState.au8DataBuf, strSBState.u16CmdSize))
                {
                    return false;
                }
            }

            return true;
        }

        case SB_COMMAND_WRITE_BLOCK:
        {
            if (WINC_BUS_SUCCESS == winc_bus_write_block(strSBState.u32CmdAddr, strSBState.au8DataBuf, strSBState.u16CmdSize))
            {
                strSBState.au8DataBuf[0] = SB_RESPONSE_ACK;
            }
            else
            {
                strSBState.au8DataBuf[0] = SB_RESPONSE_NACK;
            }

            if (1 != winc_bsp_uart_write(strSBState.au8DataBuf, 1))
            {
                return false;
            }

            return true;
        }

        case SB_COMMAND_RECONFIGURE:
        {
#ifdef CONF_WINC_SERIAL_BRIDGE_VARIABLE_BAUD_RATE
            winc_bsp_uart_set_baudrate(strSBState.u32CmdVal);
            return true;
#else
            return false;
#endif
        }

        default:
        {
        }
    }

    return false;
}

int_fast8_t winc_sb_init(uint32_t u32BaudRate)
{
#ifdef CONF_WINC_SB_SEND_UNSOL_SYNC_ID
    uint8_t u8SyncID;
#endif

    strSBState.enuState = SERIAL_BRIDGE_STATE_WAIT_OP_CODE;

    if (WINC_DRV_SUCCESS != winc_drv_init(true))
    {
        return 0;
    }

    winc_chip_interrupts_enable();

    winc_bsp_uart_open(u32BaudRate);

#ifdef CONF_WINC_SB_SEND_UNSOL_SYNC_ID
#ifdef CONF_WINC_SERIAL_BRIDGE_VARIABLE_BAUD_RATE
    u8SyncID = SB_RESPONSE_ID_VAR_BR;
#else
    u8SyncID = SB_RESPONSE_ID_FIXED_BR;
#endif
    winc_bsp_uart_write(&u8SyncID, 1);
#endif

    return 1;
}

void winc_sb_process(void)
{
    uint8_t u8Rsp;

    switch (strSBState.enuState)
    {
        case SERIAL_BRIDGE_STATE_WAIT_OP_CODE:
        {
            uint8_t u8OpCode;

            if (0 == winc_bsp_uart_read(&u8OpCode, 1))
            {
                break;
            }

            switch (u8OpCode)
            {
                case 0x12:
                {
#ifdef CONF_WINC_SERIAL_BRIDGE_VARIABLE_BAUD_RATE
                    u8Rsp = SB_RESPONSE_ID_VAR_BR;
#else
                    u8Rsp = SB_RESPONSE_ID_FIXED_BR;
#endif
                    winc_bsp_uart_write(&u8Rsp, 1);
                    break;
                }

                case 0x13:
                {
                    break;
                }

                case 0xa5:
                {
                    strSBState.enuState = SERIAL_BRIDGE_STATE_WAIT_HEADER;
                    strSBState.u16RxDataLen = 0;
                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case SERIAL_BRIDGE_STATE_WAIT_HEADER:
        {
            strSBState.u16RxDataLen += winc_bsp_uart_read(&strSBState.au8DataBuf[strSBState.u16RxDataLen], SB_HEADER_SIZE - strSBState.u16RxDataLen);

            if (SB_HEADER_SIZE != strSBState.u16RxDataLen)
            {
                break;
            }

            if (true == sb_process_header(strSBState.au8DataBuf))
            {
                u8Rsp = SB_RESPONSE_ACK;
                winc_bsp_uart_write(&u8Rsp, 1);

                if (strSBState.u16PayloadLen > 0)
                {
                    strSBState.enuState = SERIAL_BRIDGE_STATE_WAIT_PAYLOAD;
                    strSBState.u16RxDataLen = 0;
                }
                else
                {
                    strSBState.enuState = SERIAL_BRIDGE_STATE_PROCESS_COMMAND;
                }
            }
            else
            {
                strSBState.enuState = SERIAL_BRIDGE_STATE_WAIT_OP_CODE;

                u8Rsp = SB_RESPONSE_NACK;
                winc_bsp_uart_write(&u8Rsp, 1);
            }

            break;
        }

        case SERIAL_BRIDGE_STATE_PROCESS_COMMAND:
        {
            sb_process_command();

            strSBState.enuState = SERIAL_BRIDGE_STATE_WAIT_OP_CODE;
            break;
        }

        case SERIAL_BRIDGE_STATE_WAIT_PAYLOAD:
        {
            strSBState.u16RxDataLen += winc_bsp_uart_read(&strSBState.au8DataBuf[strSBState.u16RxDataLen], strSBState.u16PayloadLen - strSBState.u16RxDataLen);

            if (strSBState.u16PayloadLen == strSBState.u16RxDataLen)
            {
                sb_process_command();

                strSBState.enuState = SERIAL_BRIDGE_STATE_WAIT_OP_CODE;
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return;
}

#endif /* CONF_WINC_SERIAL_BRIDGE_INCLUDE */
