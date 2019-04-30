/*******************************************************************************
  Serial Bridge

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
#ifndef _SERIAL_BRIDGE_H
#define _SERIAL_BRIDGE_H

#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

#define SB_CMD_BUFFER_SIZE  2048
#define SB_HEADER_SIZE      12

typedef enum
{
    SERIAL_BRIDGE_STATE_UNKNOWN,
    SERIAL_BRIDGE_STATE_WAIT_OP_CODE,
    SERIAL_BRIDGE_STATE_WAIT_HEADER,
    SERIAL_BRIDGE_STATE_WAIT_PAYLOAD,
} SERIAL_BRIDGE_STATE;

typedef struct
{
    SERIAL_BRIDGE_STATE state;
    uint32_t            baudRate;
    uint8_t             cmdBuffer[SB_CMD_BUFFER_SIZE];
    uint16_t            cmdBufferAccumLength;
    uint8_t             cmdType;
    uint16_t            cmdSize;
    uint32_t            cmdAddr;
    uint32_t            cmdVal;
    uint16_t            payloadLength;
} SERIAL_BRIDGE_DECODER_STATE;

void SB_StreamDecoderInit(SERIAL_BRIDGE_DECODER_STATE *const pSBDecoderState, uint32_t baudRate);
uint32_t SB_StreamDecoderGetBaudRate(SERIAL_BRIDGE_DECODER_STATE *const pSBDecoderState);
int SB_StreamDecoder(SERIAL_BRIDGE_DECODER_STATE *const pSBDecoderState, const uint8_t *pIn, int inLength, uint8_t *pOut);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _SERIAL_BRIDGE_H */

/*******************************************************************************
 End of File
 */

