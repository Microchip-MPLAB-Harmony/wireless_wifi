// DOM-IGNORE-BEGIN
/*
Copyright (c) RivieraWaves 2009-2014
Copyright (C) 2017, Microchip Technology Inc., and its subsidiaries. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

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
#include "at_ble_api.h"
#include "platform.h"
#include "interface.h"
#include "event.h"

uint8_t interface_send_msg[INTERFACE_SEND_BUF_MAX];

#define NMI_BLE_WAITING                 0
#define NMI_BLE_COLLECTING_HDR          1
#define NMI_BLE_COLLECTING_PAYLOAD      2

void platform_interface_callback(uint8_t* data, uint32_t len)
{
    uint32_t i;

    static uint16_t src_id, msg_id, param_len;
    static uint32_t state = NMI_BLE_WAITING;
    static uint8_t header[INTERFACE_HDR_LENGTH - 1]; // we already dropped the first byte (which is always 0x5)
    static uint32_t header_idx;
    static uint8_t buff[INTERFACE_RCV_BUFF_LEN];
    static uint32_t cur_msg_start = 0;
    static uint32_t write_ptr = 0;

    for(i=0; i<len; i++)
    {
        if(state == NMI_BLE_WAITING)
        {
            if(data[i] == INTERFACE_API_PKT_ID)
            {
                state = NMI_BLE_COLLECTING_HDR;
                header_idx = 0;
            }
            continue;
        }

        if(state == NMI_BLE_COLLECTING_HDR)
        {
            header[header_idx] = data[i];
            header_idx++;

            if(header_idx == INTERFACE_HDR_LENGTH - 1)
            {
                state = NMI_BLE_COLLECTING_PAYLOAD;

                INTERFACE_UNPACK_INIT(header);
                INTERFACE_UNPACK_UINT16(&msg_id);
                INTERFACE_UNPACK_SKIP(2); // not interested in destination (always EXT_APP)
                INTERFACE_UNPACK_UINT16(&src_id);
                INTERFACE_UNPACK_UINT16(&param_len);
                INTERFACE_UNPACK_DONE();

                if(param_len == 0)
                {
                    if(!special_events_handler(msg_id, src_id, NULL))
                    {
                        // post it into the event queue
                        event_post(msg_id, src_id, NULL);
                    }
                    state = NMI_BLE_WAITING;
                }

                // make sure that we have room for params
                if(sizeof(buff) - write_ptr < param_len)
                {
                    write_ptr = 0;
                }
                cur_msg_start = write_ptr;
            }
            continue;
        }

        if(state == NMI_BLE_COLLECTING_PAYLOAD)
        {
            buff[write_ptr] = data[i];
            write_ptr++;

            if(write_ptr - cur_msg_start == param_len)
            {
                // message fully received
                state = NMI_BLE_WAITING;

                // handle message
                if(!special_events_handler(msg_id, src_id, &buff[cur_msg_start]))
                {
                    // post it into the event queue
                    event_post(msg_id, src_id, &buff[cur_msg_start]);
                }

            }

        }
    }
}

void interface_send(uint8_t* msg, uint16_t u16TxLen)
{
    platform_interface_send(msg, u16TxLen);
}
// DOM-IGNORE-END
