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
// DOM-IGNORE-END
#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include "event.h"

#define INTERFACE_HDR_LENGTH        9

#define INTERFACE_API_PKT_ID            0x05

#define INTERFACE_SEND_BUF_MAX  600
#define INTERFACE_RCV_BUFF_LEN 500

extern uint8_t interface_send_msg[INTERFACE_SEND_BUF_MAX];

void platform_send_lock_aquire(void);
void platform_send_lock_release(void);

#define INTERFACE_MSG_INIT(msg_id, dest_id) \
do{\
    uint8_t* __ptr = interface_send_msg;\
    uint16_t __len;\
    platform_send_lock_aquire();\
    *__ptr++ = (INTERFACE_API_PKT_ID);\
    *__ptr++ = ((msg_id) & 0x00FF );\
    *__ptr++ = (((msg_id)>>8) & 0x00FF );\
    *__ptr++ = ((dest_id) & 0x00FF );\
    *__ptr++ = (((dest_id)>>8) & 0x00FF );\
    *__ptr++ = ((TASK_EXTERN) & 0x00FF );\
    *__ptr++ = (((TASK_EXTERN)>>8) & 0x00FF );\
    __ptr += 2

#define INTERFACE_PACK_ARG_UINT8(arg)\
    *__ptr++ = (arg)

#define INTERFACE_PACK_ARG_UINT16(arg)\
    *__ptr++ = ((arg) & 0x00FF);\
    *__ptr++ = (((arg) >> 8) & 0x00FF)

#define INTERFACE_PACK_ARG_UINT32(arg) \
    *__ptr++ = (uint8_t)((arg) & 0x00FF );\
    *__ptr++ = (uint8_t)(( (arg) >> 8) & 0x00FF) ;\
    *__ptr++ = (uint8_t)(( (arg) >> 16) & 0x00FF);\
    *__ptr++ = (uint8_t)(( (arg) >> 24) & 0x00FF)

#define INTERFACE_PACK_ARG_BLOCK(ptr,len)\
    memcpy(__ptr, ptr, len);\
    __ptr += len

#define INTERFACE_PACK_ARG_DUMMY(len)\
    __ptr += len

#define INTERFACE_PACK_LEN()\
    __len = __ptr - &interface_send_msg[INTERFACE_HDR_LENGTH];\
    interface_send_msg[7] = ((__len) & 0x00FF );\
    interface_send_msg[8] = (((__len)>>8) & 0x00FF);\
    __len += INTERFACE_HDR_LENGTH;

#define INTERFACE_SEND_NO_WAIT()\
    INTERFACE_PACK_LEN();\
    interface_send(interface_send_msg, __len)

#define INTERFACE_SEND_WAIT(msg, src, status)\
    watched_event.msg_id = msg;\
    watched_event.src_id = src;\
    INTERFACE_PACK_LEN();\
    interface_send(interface_send_msg, __len);\
    *status = AT_BLE_SUCCESS;\
    if(platform_cmd_cmpl_wait()){*status = AT_BLE_FAILURE;}\
    __ptr = watched_event.params;\

#define INTERFACE_MSG_DONE()\
    platform_send_lock_release();\
}while(0)

#define INTERFACE_UNPACK_INIT(ptr)\
do{\
    uint8_t* __ptr = (uint8_t*)(ptr);\

#define INTERFACE_UNPACK_UINT8(ptr)\
    *ptr = *__ptr++

#define INTERFACE_UNPACK_UINT16(ptr)\
    *ptr = (uint16_t)__ptr[0]\
        | ((uint16_t)__ptr[1] << 8);\
    __ptr += 2

#define INTERFACE_UNPACK_UINT32(ptr)\
    *ptr = (uint32_t)__ptr[0] \
        | ((uint32_t)__ptr[1] << 8) \
        | ((uint32_t)__ptr[2] << 16)\
        | ((uint32_t)__ptr[3] << 24);\
    __ptr += 4

#define INTERFACE_UNPACK_BLOCK(ptr, len)\
    memcpy(ptr, __ptr, len);\
    __ptr += len

#define INTERFACE_UNPACK_SKIP(len)\
    __ptr += (len)

#define INTERFACE_UNPACK_DONE()\
}while(0)

void interface_send(uint8_t* msg, uint16_t u16TxLen);

#endif /* HCI_H_ */
