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

#include "dbg_task.h"

uint8_t dbg_le_set_bd_addr_req_handler(uint8_t* addr)
{
    uint8_t u8Status;
    do
    {
        INTERFACE_MSG_INIT(DBG_LE_SET_BD_ADDR_REQ, TASK_DBG);
        INTERFACE_PACK_ARG_BLOCK(addr , AT_BLE_ADDR_LEN);
        INTERFACE_SEND_WAIT(DBG_LE_SET_BD_ADDR_CMP_EVT, TASK_DBG, &u8Status);
        if(u8Status == AT_BLE_SUCCESS)
        {
            INTERFACE_UNPACK_UINT8(&u8Status);
        }
        INTERFACE_MSG_DONE();
    }while(0);
    return u8Status;
}

uint8_t dbg_le_get_bd_addr_req_handler(uint8_t* addr)
{
    uint8_t u8Status;

    INTERFACE_MSG_INIT(DBG_LE_GET_BD_ADDR_REQ, TASK_DBG);
    INTERFACE_SEND_WAIT(DBG_LE_GET_BD_ADDR_CMP_EVT, TASK_DBG, &u8Status);
    if(u8Status == AT_BLE_SUCCESS)
    {
        INTERFACE_UNPACK_BLOCK(addr,AT_BLE_ADDR_LEN);
    }
    INTERFACE_MSG_DONE();

    return u8Status;
}

uint8_t dbg_set_tx_pw_req_handler(uint16_t conn_handle, uint8_t level)
{
    uint8_t u8Status;

    INTERFACE_MSG_INIT(DBG_SET_TX_PW_REQ, TASK_DBG);
    INTERFACE_PACK_ARG_UINT16(conn_handle);
    INTERFACE_PACK_ARG_UINT8(level);
    INTERFACE_SEND_WAIT(DBG_SET_TX_PW_CMP_EVT, TASK_DBG, &u8Status);
    if(u8Status == AT_BLE_SUCCESS)
    {
        INTERFACE_UNPACK_UINT8(&u8Status);
    }
    INTERFACE_MSG_DONE();

    return u8Status;
}

uint8_t dbg_wr_mem_req_handler(uint32_t memAddr,uint8_t* data,uint8_t size)
{
    uint8_t u8Status;

    INTERFACE_MSG_INIT(DBG_WR_MEM_REQ, TASK_DBG);
    INTERFACE_PACK_ARG_UINT32(memAddr);
    INTERFACE_PACK_ARG_UINT8(8);
    INTERFACE_PACK_ARG_UINT8(size);
    INTERFACE_PACK_ARG_BLOCK(data,size);
    INTERFACE_SEND_WAIT(DBG_WR_MEM_CMP_EVT, TASK_DBG, &u8Status);
    if(u8Status == AT_BLE_SUCCESS)
    {
        INTERFACE_UNPACK_UINT8(&u8Status);
    }
    INTERFACE_MSG_DONE();

    return u8Status;
}

void dbg_wr_mem_req_handler32_reset(uint32_t memAddr,uint32_t * data,uint8_t size)
{
    INTERFACE_MSG_INIT(DBG_WR_MEM_REQ, TASK_DBG);
    INTERFACE_PACK_ARG_UINT32(memAddr);
    INTERFACE_PACK_ARG_UINT8(32);
    INTERFACE_PACK_ARG_UINT8(size);
    INTERFACE_PACK_ARG_BLOCK(data,size);
    INTERFACE_SEND_NO_WAIT();
    INTERFACE_MSG_DONE();
}

uint8_t dbg_wr_mem_req_handler32(uint32_t memAddr,uint32_t* data,uint8_t size)
{
    uint8_t u8Status;

    INTERFACE_MSG_INIT(DBG_WR_MEM_REQ, TASK_DBG);
    INTERFACE_PACK_ARG_UINT32(memAddr);
    INTERFACE_PACK_ARG_UINT8(32);
    INTERFACE_PACK_ARG_UINT8(size);
    INTERFACE_PACK_ARG_BLOCK(data,size);
    INTERFACE_SEND_WAIT(DBG_WR_MEM_CMP_EVT, TASK_DBG, &u8Status);
    if(u8Status == AT_BLE_SUCCESS)
    {
        INTERFACE_UNPACK_UINT8(&u8Status);
    }
    INTERFACE_MSG_DONE();

    return u8Status;
}

uint8_t dbg_rd_mem_req_handler(uint32_t memAddr,uint8_t* data ,uint8_t size)
{
    uint8_t u8Status;

    INTERFACE_MSG_INIT(DBG_RD_MEM_REQ, TASK_DBG);
    INTERFACE_PACK_ARG_UINT32(memAddr);
    INTERFACE_PACK_ARG_UINT8(8);
    INTERFACE_PACK_ARG_UINT8(size);
    INTERFACE_SEND_WAIT(DBG_RD_MEM_CMP_EVT, TASK_DBG, &u8Status);
    if(u8Status == AT_BLE_SUCCESS)
    {
        INTERFACE_UNPACK_UINT8(&u8Status);
        INTERFACE_UNPACK_BLOCK(data,size);
    }
    INTERFACE_MSG_DONE();

    return u8Status;
}

uint8_t dbg_rd_mem_req_handler32(uint32_t memAddr,uint8_t* data ,uint8_t size)
{
    uint8_t u8Status;

    INTERFACE_MSG_INIT(DBG_RD_MEM_REQ, TASK_DBG);
    INTERFACE_PACK_ARG_UINT32(memAddr);
    INTERFACE_PACK_ARG_UINT8(32);
    INTERFACE_PACK_ARG_UINT8(size);
    INTERFACE_SEND_WAIT(DBG_RD_MEM_CMP_EVT, TASK_DBG, &u8Status);
    if(u8Status == AT_BLE_SUCCESS)
    {
        INTERFACE_UNPACK_UINT8(&u8Status);
        INTERFACE_UNPACK_BLOCK(data,size);
    }
    INTERFACE_MSG_DONE();

    return u8Status;
}
// DOM-IGNORE-END
