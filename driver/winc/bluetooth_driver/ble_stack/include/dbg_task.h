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
/*
 * dbg_task.h
 *
 *  Created on: Oct 28, 2014
 *      Author: lali
 */
#ifndef DBG_TASK_H_
#define DBG_TASK_H_

#include "cmn_defs.h"

#define RPL_POWER_MIN 1
#define RPL_POWER_MAX 6
#define RPL_PWR_TBL_SIZE 0x0f

/// dbg Task messages
enum dbg_msg_id
{
    /// Debug commands description.
    DBG_MSG_ID_CMD_FIRST = (uint16_t)(3 << 10),
    DBG_RD_MEM_REQ = DBG_MSG_ID_CMD_FIRST,
    DBG_WR_MEM_REQ, //0c01
    DBG_DEL_PARAM_REQ,
    DBG_RD_PARAM_REQ,
    DBG_WR_PARAM_REQ,
    DBG_PLF_RESET_REQ,
    DBG_FLASH_IDENT_REQ,
    DBG_FLASH_ERASE_REQ,
    DBG_FLASH_WRITE_REQ,
    DBG_FLASH_READ_REQ,

    DBG_RD_KE_STATS_REQ, //0c0a
    DBG_RF_REG_RD_REQ,
    DBG_RF_REG_WR_REQ,
    DBG_HW_REG_RD_REQ,
    DBG_HW_REG_WR_REQ,
    DBG_RF_SET_BLE_GAIN_REQ,

    DBG_LE_SET_BD_ADDR_REQ, //0c10
    DBG_LE_SET_TYPE_PUB_REQ,//0c11
    DBG_LE_SET_TYPE_RAND_REQ,//0c12
    DBG_LE_SET_CRC_REQ,
    DBG_LE_LLCP_DISCARD_REQ,//0c14
    DBG_LE_RESET_RX_CNT_REQ,
    DBG_LE_RESET_TX_CNT_REQ,
    DBG_SET_TX_PW_REQ,//0c17
    DBG_LE_GET_BD_ADDR_REQ=0xc1a,
};

enum dbg_evt_id
{
    /// Debug events description.
    DBG_RD_MEM_CMP_EVT = 0x0c2e,
    DBG_WR_MEM_CMP_EVT, //0c20
    DBG_DEL_PARAM_CMP_EVT,
    DBG_RD_PARAM_CMP_EVT,
    DBG_WR_PARAM_CMP_EVT,
    DBG_FLASH_IDENT_CMP_EVT,
    DBG_FLASH_ERASE_CMP_EVT,//0c25
    DBG_FLASH_WRITE_CMP_EVT,
    DBG_FLASH_READ_CMP_EVT, //0c27

    DBG_RD_KE_STATS_CMP_EVT,
    DBG_TRACE_WARNING_EVT,
    DBG_RF_REG_RD_CMP_EVT,
    DBG_RF_REG_WR_CMP_EVT,
    DBG_PLF_RESET_CMP_EVT,
    DBG_HW_REG_RD_CMP_EVT,
    DBG_HW_REG_WR_CMP_EVT,

    DBG_LE_SET_BD_ADDR_CMP_EVT, //0c2f
    DBG_LE_SET_TYPE_PUB_CMP_EVT,
    DBG_LE_SET_TYPE_RAND_CMP_EVT,
    DBG_LE_SET_CRC_CMP_EVT,
    DBG_LE_LLCP_DISCARD_CMP_EVT,
    DBG_LE_RESET_RX_CNT_CMP_EVT,
    DBG_LE_RESET_TX_CNT_CMP_EVT,
    DBG_SET_TX_PW_CMP_EVT,
    DBG_LE_GET_BD_ADDR_CMP_EVT=0xc48
};

uint8_t dbg_le_set_bd_addr_req_handler(uint8_t* addr);
uint8_t dbg_le_get_bd_addr_req_handler(uint8_t* addr);
uint8_t dbg_set_tx_pw_req_handler(uint16_t conn_handle, uint8_t level);
uint8_t dbg_wr_mem_req_handler(uint32_t memAddr,uint8_t * data,uint8_t size);
uint8_t dbg_wr_mem_req_handler32(uint32_t memAddr,uint32_t * data,uint8_t size);
void dbg_wr_mem_req_handler32_reset(uint32_t memAddr,uint32_t * data,uint8_t size);
uint8_t dbg_rd_mem_req_handler(uint32_t memAddr,uint8_t* data ,uint8_t size);
uint8_t dbg_rd_mem_req_handler32(uint32_t memAddr,uint8_t* data ,uint8_t size);

#endif

