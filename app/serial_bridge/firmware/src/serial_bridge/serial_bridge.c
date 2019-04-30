/*******************************************************************************
  <Title>

  Company:
    Microchip Technology Inc.

  File Name:

  Summary:

  Description:
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "serial_bridge.h"
#include "nmdrv.h"
#include "nmbus.h"
#if defined(WDRV_WINC_DEVICE_WINC1500)
#include "m2m_wifi.h"
#endif

typedef enum
{
    SB_COMMAND_READ_REG_WITH_RET = 0,
    SB_COMMAND_WRITE_REG = 1,
    SB_COMMAND_READ_BLOCK = 2,
    SB_COMMAND_WRITE_BLOCK = 3,
    SB_COMMAND_RECONFIGURE = 5
} SB_COMMAND;

typedef enum
{
    SB_RESPONSE_NACK = 0x5a,
    SB_RESPONSE_ACK = 0xac
} SB_RESPONSE;

static bool _ProcessHeader(SERIAL_BRIDGE_DECODER_STATE *const pSBDecoderState, uint8_t *pHeader)
{
    uint8_t checksum;
    uint8_t i;

    if ((NULL == pSBDecoderState) || (NULL == pHeader))
    {
        return false;
    }

    checksum = 0;

    for (i=0; i<SB_HEADER_SIZE; i++)
    {
        checksum ^= pHeader[i];
    }

    if (0 != checksum)
    {
        return false;
    }

    pSBDecoderState->cmdType = pHeader[0];
    pSBDecoderState->cmdSize = (((uint16_t)pHeader[3]) << 8) | pHeader[2];
    pSBDecoderState->cmdAddr = (((uint32_t)pHeader[7]) << 24) | (((uint32_t)pHeader[6]) << 16) | (((uint32_t)pHeader[5]) << 8) | pHeader[4];
    pSBDecoderState->cmdVal  = (((uint32_t)pHeader[11]) << 24) | (((uint32_t)pHeader[10]) << 16) | (((uint32_t)pHeader[9]) << 8) | pHeader[8];

    if (pSBDecoderState->cmdType == SB_COMMAND_WRITE_BLOCK)
    {
        pSBDecoderState->payloadLength = pSBDecoderState->cmdSize;
    }
    else
    {
        pSBDecoderState->payloadLength = 0;
    }

    return true;
}

static int _ProcessCommand(SERIAL_BRIDGE_DECODER_STATE *const pSBDecoderState, uint8_t *pOut)
{
    if ((NULL == pSBDecoderState) || (NULL == pOut))
    {
        return -1;
    }

    switch (pSBDecoderState->cmdType)
    {
        case SB_COMMAND_READ_REG_WITH_RET:
        {
            uint32_t val;

            val = nm_read_reg(pSBDecoderState->cmdAddr);

            pOut[0] = (val >> 24) & 0xff;
            pOut[1] = (val >> 16) & 0xff;
            pOut[2] = (val >> 8) & 0xff;
            pOut[3] = (val) & 0xff;

            return 4;
        }

        case SB_COMMAND_WRITE_REG:
        {
            nm_write_reg(pSBDecoderState->cmdAddr, pSBDecoderState->cmdVal);

            return 0;
        }

        case SB_COMMAND_READ_BLOCK:
        {
            nm_read_block(pSBDecoderState->cmdAddr, pOut, pSBDecoderState->cmdSize);

            return pSBDecoderState->cmdSize;
        }

        case SB_COMMAND_WRITE_BLOCK:
        {
            if (M2M_SUCCESS == nm_write_block(pSBDecoderState->cmdAddr, pSBDecoderState->cmdBuffer, pSBDecoderState->cmdSize))
            {
                pOut[0] = SB_RESPONSE_ACK;
            }
            else
            {
                pOut[0] = SB_RESPONSE_NACK;
            }

            return 1;
        }

        case SB_COMMAND_RECONFIGURE:
        {
            pSBDecoderState->baudRate = pSBDecoderState->cmdVal;
            return 0;
        }

        default:
        {
        }
    }

    return -1;
}

void SB_StreamDecoderInit(SERIAL_BRIDGE_DECODER_STATE *const pSBDecoderState, uint32_t baudRate)
{
    if (NULL == pSBDecoderState)
    {
        return;
    }

    pSBDecoderState->state = SERIAL_BRIDGE_STATE_WAIT_OP_CODE;
    pSBDecoderState->baudRate = baudRate;

#if defined(WDRV_WINC_DEVICE_WINC1500)
    m2m_wifi_download_mode();
#elif defined(WDRV_WINC_DEVICE_WINC3400)
    nm_cpu_start();
#endif
}

uint32_t SB_StreamDecoderGetBaudRate(SERIAL_BRIDGE_DECODER_STATE *const pSBDecoderState)
{
    if (NULL == pSBDecoderState)
    {
        return 0;
    }

    return pSBDecoderState->baudRate;
}

int SB_StreamDecoder(SERIAL_BRIDGE_DECODER_STATE *const pSBDecoderState, const uint8_t *pIn, int inLength, uint8_t *pOut)
{
    int outLength = 0;
    int cmdOutLength;

    if ((NULL == pSBDecoderState) || (NULL == pIn) || (NULL == pOut))
    {
        return -1;
    }

    while (inLength)
    {
        switch (pSBDecoderState->state)
        {
            case SERIAL_BRIDGE_STATE_WAIT_OP_CODE:
            {
                uint8_t opCode;

                opCode = *pIn++;
                inLength--;

                switch (opCode)
                {
                    case 0x12:
                    {
                        *pOut++ = 0x5c;     /* Baud rate change not currently support due to USART driver issue */
                        outLength++;
                        break;
                    }

                    case 0x13:
                    {
                        break;
                    }

                    case 0xa5:
                    {
                        pSBDecoderState->state = SERIAL_BRIDGE_STATE_WAIT_HEADER;
                        pSBDecoderState->cmdBufferAccumLength = 0;
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
                int copyLength;

                copyLength = inLength;

                if ((copyLength+pSBDecoderState->cmdBufferAccumLength) > SB_HEADER_SIZE)
                {
                    copyLength = SB_HEADER_SIZE - pSBDecoderState->cmdBufferAccumLength;
                }

                memcpy(&pSBDecoderState->cmdBuffer[pSBDecoderState->cmdBufferAccumLength], pIn, copyLength);

                pIn += copyLength;
                inLength -= copyLength;

                pSBDecoderState->cmdBufferAccumLength += copyLength;

                if (SB_HEADER_SIZE == pSBDecoderState->cmdBufferAccumLength)
                {
                    if (true == _ProcessHeader(pSBDecoderState, pSBDecoderState->cmdBuffer))
                    {
                        *pOut++ = SB_RESPONSE_ACK;
                        outLength++;

                        if (pSBDecoderState->payloadLength > 0)
                        {
                            pSBDecoderState->state = SERIAL_BRIDGE_STATE_WAIT_PAYLOAD;
                        }
                        else
                        {
                            cmdOutLength = _ProcessCommand(pSBDecoderState, pOut);

                            pOut += cmdOutLength;
                            outLength += cmdOutLength;

                            pSBDecoderState->state = SERIAL_BRIDGE_STATE_WAIT_OP_CODE;
                        }
                    }
                    else
                    {
                        pSBDecoderState->state = SERIAL_BRIDGE_STATE_WAIT_OP_CODE;
                        *pOut++ = SB_RESPONSE_NACK;
                        outLength++;
                    }

                    pSBDecoderState->cmdBufferAccumLength = 0;
                }
                break;
            }

            case SERIAL_BRIDGE_STATE_WAIT_PAYLOAD:
            {
                int copyLength;

                copyLength = inLength;

                if ((copyLength+pSBDecoderState->cmdBufferAccumLength) > pSBDecoderState->payloadLength)
                {
                    copyLength = pSBDecoderState->payloadLength - pSBDecoderState->cmdBufferAccumLength;
                }

                memcpy(&pSBDecoderState->cmdBuffer[pSBDecoderState->cmdBufferAccumLength], pIn, copyLength);

                pIn += copyLength;
                inLength -= copyLength;

                pSBDecoderState->cmdBufferAccumLength += copyLength;

                if (pSBDecoderState->payloadLength == pSBDecoderState->cmdBufferAccumLength)
                {
                    cmdOutLength = _ProcessCommand(pSBDecoderState, pOut);

                    pOut += cmdOutLength;
                    outLength += cmdOutLength;

                    pSBDecoderState->state = SERIAL_BRIDGE_STATE_WAIT_OP_CODE;
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }

    return outLength;
}

/*******************************************************************************
 End of File
*/
