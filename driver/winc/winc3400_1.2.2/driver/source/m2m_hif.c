/*******************************************************************************
  File Name:
    m2m_hif.c

  Summary:
    This module contains M2M host interface API implementations.

  Description:
    This module contains M2M host interface API implementations.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "nm_common.h"
#include "nmbus.h"
#include "nmdrv.h"
#include "nm_bsp.h"
#include "m2m_hif.h"
#include "m2m_types.h"
#include "nmasic.h"
#include "m2m_periph.h"
#include "wdrv_winc_common.h"
#include "osal/osal.h"

#define NMI_AHB_DATA_MEM_BASE  0x30000
#define NMI_AHB_SHARE_MEM_BASE 0xd0000

#define WIFI_HOST_RCV_CTRL_0    (0x1070)
#define WIFI_HOST_RCV_CTRL_1    (0x1084)
#define WIFI_HOST_RCV_CTRL_2    (0x1078)
#define WIFI_HOST_RCV_CTRL_3    (0x106c)
#define WAKE_VALUE              (0x5678)
#define SLEEP_VALUE             (0x4321)
#define WAKE_REG                (0x1074)

#define INTERRUPT_CORTUS_0_3000D0   (0x10a8)
#define INTERRUPT_CORTUS_1_3000D0   (0x10ac)
#define INTERRUPT_CORTUS_2_3000D0   (0x10b0)
#define INTERRUPT_CORTUS_3_3000D0   (0x10b4)

static OSAL_SEM_HANDLE_TYPE hifSemaphore;

static volatile uint8_t gu8ChipMode = 0;
static volatile uint8_t gu8ChipSleep = 0;
static volatile uint8_t gu8HifSizeDone = 0;
static volatile uint8_t gu8Interrupt = 0;
static volatile uint8_t gu8Yield = 0;

/*
    Special codes for managing HIF restriction to OTA rollback/switch only
*/
#define HIF_OTA_RB_ONLY             0xFFFF
#define HIFCODE_OTA_RB              ((M2M_REQ_GROUP_OTA << 8) | M2M_OTA_REQ_ROLLBACK)
#define HIFCODE_OTA_SW              ((M2M_REQ_GROUP_OTA << 8) | M2M_OTA_REQ_SWITCH_FIRMWARE)
#define HIFCODE_SSL_WRITECERT       ((M2M_REQ_GROUP_SSL << 8) | M2M_SSL_REQ_WRITE_OWN_CERTS)
#define HIFCODE_WIFI_PASSIVESCAN    ((M2M_REQ_GROUP_WIFI << 8) | M2M_WIFI_REQ_PASSIVE_SCAN)
/*
    List of new HIF messages (since last HIF major increase)
    Each entry is formed of ((GroupId << 8) | OpCode)
    Additionally, entry 0 used to indicate OTA RB/SW only.
*/
#define NEW_HIF_LIST HIF_OTA_RB_ONLY, HIFCODE_SSL_WRITECERT, HIFCODE_WIFI_PASSIVESCAN
/*
    Array of HIF messages which are not supported by Firmware.
    During hif_init() this array is rebased using an offset determined by Firmware HIF level.
*/
static uint16_t gau16HifBlacklist[] = {NEW_HIF_LIST};
#define HIF_BLACKLIST_SZ (sizeof(gau16HifBlacklist)/sizeof(gau16HifBlacklist[0]))
static uint8_t gu8HifBlOffset = 0;

tpfHifCallBack pfWifiCb = NULL;     /*!< pointer to Wi-Fi call back function */
tpfHifCallBack pfIpCb  = NULL;      /*!< pointer to Socket call back function */
tpfHifCallBack pfOtaCb = NULL;      /*!< pointer to OTA call back function */
tpfHifCallBack pfSigmaCb = NULL;
tpfHifCallBack pfHifCb = NULL;
tpfHifCallBack pfSSLCb = NULL;

static int8_t hif_set_rx_done(void)
{
    uint32_t reg;
    int8_t ret = M2M_SUCCESS;

    if (ISNMC3400(nmi_get_chipid())) {
        ret = nm_write_reg(INTERRUPT_CORTUS_0_3000D0, 1);
        if(ret != M2M_SUCCESS)goto ERR1;
    } else  {
        ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_0, &reg);
        if(ret != M2M_SUCCESS)goto ERR1;

        /* Set RX Done */
        reg |= (1<<1);
        ret = nm_write_reg(WIFI_HOST_RCV_CTRL_0, reg);
        if(ret != M2M_SUCCESS)goto ERR1;
    }

ERR1:
    return ret;
}

/**
*   @fn         static void m2m_hif_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
*   @brief      WiFi call back function
*   @param [in] u8OpCode
*                   HIF Opcode type.
*   @param [in] u16DataSize
*                   HIF data length.
*   @param [in] u32Addr
*                   HIF address.
*/
static void m2m_hif_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
{
}

/**
*   @fn     int8_t hif_chip_wake(void);
*   @brief  To Wakeup the chip.
*   @return The function shall return ZERO for successful operation and a negative value otherwise.
*/
int8_t hif_chip_wake(void)
{
    int8_t ret = M2M_SUCCESS;
    if(gu8ChipSleep == 0)
    {
        if(gu8ChipMode == M2M_PS_DEEP_AUTOMATIC)
        {
            ret = nm_clkless_wake();
            if(ret != M2M_SUCCESS)goto ERR1;
            ret = nm_write_reg(WAKE_REG, WAKE_VALUE);
            if(ret != M2M_SUCCESS)goto ERR1;
        }
    }
    gu8ChipSleep++;
ERR1:
    return ret;
}

/*!
@fn \
    void hif_set_sleep_mode(uint8_t u8Pstype);

@brief
    Set the sleep mode of the HIF layer.

@param [in] u8Pstype
                Sleep mode.

@return
    The function SHALL return 0 for success and a negative value otherwise.
*/

void hif_set_sleep_mode(uint8_t u8Pstype)
{
    gu8ChipMode = u8Pstype;
}
/*!
@fn \
    uint8_t hif_get_sleep_mode(void);

@brief
    Get the sleep mode of the HIF layer.

@return
    The function SHALL return the sleep mode of the HIF layer.
*/

uint8_t hif_get_sleep_mode(void)
{
    return gu8ChipMode;
}

/**
*   @fn     int8_t hif_chip_sleep(void);
*   @brief  To make the chip sleep.
*    @return        The function shall return ZERO for successful operation and a negative value otherwise.
*/
int8_t hif_chip_sleep(void)
{
    int8_t ret = M2M_SUCCESS;

    if(gu8ChipSleep >= 1)
    {
        gu8ChipSleep--;
    }

    if(gu8ChipSleep == 0)
    {
        if(gu8ChipMode == M2M_PS_DEEP_AUTOMATIC)
        {
            uint32_t reg = 0;
            ret = nm_write_reg(WAKE_REG, SLEEP_VALUE);
            if(ret != M2M_SUCCESS)goto ERR1;
            /* Clear bit 1 */
            ret = nm_read_reg_with_ret(0x1, &reg);
            if(ret != M2M_SUCCESS)goto ERR1;
            if(reg&0x2)
            {
                reg &=~(1 << 1);
                ret = nm_write_reg(0x1, reg);
            }
        }
    }
ERR1:
    return ret;
}

/**
*   @fn     int8_t hif_init(void *arg);
*   @brief  To initialize HIF layer.
*   @param [in] arg
*               Pointer to the arguments.
*   @return     The function shall return ZERO for successful operation and a negative value otherwise.
*/
int8_t hif_init(void *arg)
{
    pfWifiCb = NULL;
    pfIpCb = NULL;

    gu8ChipSleep = 0;
    gu8ChipMode = M2M_NO_PS;

    gu8Interrupt = 0;

    if (OSAL_RESULT_TRUE != OSAL_SEM_Create(&hifSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1))
        return M2M_ERR_INIT;

    gu8HifBlOffset = 0;
    hif_register_cb(M2M_REQ_GROUP_HIF, m2m_hif_cb);

    return M2M_SUCCESS;
}

/**
*   @fn     int8_t hif_deinit(void * arg);
*   @brief  To Deinitialize HIF layer.
*   @param [in] arg
*               Pointer to the arguments.
*   @return     The function shall return ZERO for successful operation and a negative value otherwise.
*/
int8_t hif_deinit(void *arg)
{
    int8_t ret = M2M_SUCCESS;
    ret = hif_chip_wake();

    gu8ChipMode = 0;
    gu8ChipSleep = 0;
    gu8HifSizeDone = 0;
    gu8Interrupt = 0;

    pfWifiCb = NULL;
    pfIpCb  = NULL;
    pfOtaCb = NULL;
    pfHifCb = NULL;

    return ret;
}
/**
*   @fn         int8_t hif_check_compatibility(uint16_t u16HifInfo);
*   @brief
*               To check the compatibility of an image with the current driver.
*   @param [in] u16HifInfo
*               HIF info of image to be checked.
*   @return     The function shall return ZERO for compatible image and a negative value otherwise.
*/
int8_t hif_check_compatibility(uint16_t u16HifInfo)
{
    int8_t ret = M2M_ERR_FW_VER_MISMATCH;
    if((M2M_GET_HIF_BLOCK(u16HifInfo) == M2M_HIF_BLOCK_VALUE) && (M2M_GET_HIF_MAJOR(u16HifInfo) == M2M_HIF_MAJOR_VALUE))
    {
        ret = M2M_SUCCESS;
    }
    return ret;
}
/**
*   @fn         int8_t hif_enable_access(void);
*   @brief
*               To enable access to HIF layer, based on HIF level of Firmware.
*               This function reads HIF level directly from a register written by Firmware.
*   @return     The function shall return ZERO for full match operation and a negative value if operation is restricted.
*/
int8_t hif_enable_access(void)
{
    int8_t ret = M2M_SUCCESS;
    uint16_t fw_hif_info = 0;

    ret = nm_get_hif_info(&fw_hif_info, NULL);
    if(ret == M2M_SUCCESS)
    {
        ret = hif_check_compatibility(fw_hif_info);
        if(ret == M2M_SUCCESS)
        {
            switch(M2M_GET_HIF_MINOR(fw_hif_info))
            {
            case 0:
                gu8HifBlOffset = 1;
                break;
            case 1:
                gu8HifBlOffset = 2;
                break;
            case 2:
                gu8HifBlOffset = 2;
                break;
            case 3:
                gu8HifBlOffset = 3;
                break;
            // Additional case to be added each time hif minor increments.
            // All additional cases to be removed in the event of a hif major increment.
            // Default catches all cases in which hif minor is greater in Firmware than in Driver.
            default:
                gu8HifBlOffset = HIF_BLACKLIST_SZ;
                break;
            }
        }
        else
        {
            gu8HifBlOffset = 0;
            M2M_ERR("HIF access limited to OTA Switch/Rollback only\r\n");
        }
    }
    return ret;
}
/**
*   @fn         int8_t hif_check_code(uint8_t u8Gid, uint8_t u8OpCode);
*   @brief
*               To check that a particular hif message is supported with the current driver/firmware pair.
*   @param [in] u8Gid
*               Group ID.
*   @param [in] u8Opcode
*               Operation ID.
*   @return     The function shall return ZERO for support and a negative value otherwise.
*/
int8_t hif_check_code(uint8_t u8Gid, uint8_t u8OpCode)
{
    uint8_t u8BlId;
    uint16_t u16HifCode = ((uint16_t)u8Gid<<8) | u8OpCode;
    if((u16HifCode == HIFCODE_OTA_RB) || (u16HifCode == HIFCODE_OTA_SW))
    {
        return M2M_SUCCESS;
    }
    if(gu8HifBlOffset == 0)
    {
        M2M_ERR("HIF OTA rb/sw only\r\n");
        return M2M_ERR_SEND;
    }
    for(u8BlId = gu8HifBlOffset; u8BlId < HIF_BLACKLIST_SZ; u8BlId++)
    {
        if(u16HifCode == gau16HifBlacklist[u8BlId])
        {
            M2M_ERR("HIF message unsupported\r\n");
            return M2M_ERR_SEND;
        }
    }
    return M2M_SUCCESS;
}
/**
*   @fn     int8_t hif_send(uint8_t u8Gid,uint8_t u8Opcode,uint8_t *pu8CtrlBuf,uint16_t u16CtrlBufSize,
                       uint8_t *pu8DataBuf,uint16_t u16DataSize, uint16_t u16DataOffset)
*   @brief  Send packet using host interface.

*   @param [in] u8Gid
*               Group ID.
*   @param [in] u8Opcode
*               Operation ID.
*   @param [in] pu8CtrlBuf
*               Pointer to the Control buffer.
*   @param [in] u16CtrlBufSize
                Control buffer size.
*   @param [in] u16DataOffset
                Packet Data offset.
*   @param [in] pu8DataBuf
*               Packet buffer Allocated by the caller.
*   @param [in] u16DataSize
                Packet buffer size (including the HIF header).
*    @return        The function shall return ZERO for successful operation and a negative value otherwise.
*/
int8_t hif_send(uint8_t u8Gid, uint8_t u8Opcode, uint8_t *pu8CtrlBuf, uint16_t u16CtrlBufSize,
               uint8_t *pu8DataBuf, uint16_t u16DataSize, uint16_t u16DataOffset)
{
    int8_t      ret = M2M_ERR_SEND;
    volatile tstrHifHdr strHif;

    while (OSAL_RESULT_FALSE == OSAL_SEM_Pend(&hifSemaphore, OSAL_WAIT_FOREVER))
    {
    }

    strHif.u8Opcode     = u8Opcode&(~NBIT7);
    strHif.u8Gid        = u8Gid;
    strHif.u16Length    = M2M_HIF_HDR_OFFSET;
    if(pu8DataBuf != NULL)
    {
        strHif.u16Length += u16DataOffset + u16DataSize;
    }
    else
    {
        strHif.u16Length += u16CtrlBufSize;
    }

    ret = hif_check_code(strHif.u8Gid, strHif.u8Opcode);
    if(ret != M2M_SUCCESS)
    {
        goto ERR1;
    }

    ret = hif_chip_wake();
    if(ret == M2M_SUCCESS)
    {
        volatile uint32_t reg, dma_addr = 0;
        volatile uint16_t cnt = 0;

        reg = 0UL;
        reg |= (uint32_t)u8Gid;
        reg |= ((uint32_t)u8Opcode<<8);
        reg |= ((uint32_t)strHif.u16Length<<16);
        ret = nm_write_reg(NMI_STATE_REG, reg);
        if(M2M_SUCCESS != ret) goto ERR1;

        reg = 0;
        reg |= (1<<1);
        ret = nm_write_reg(WIFI_HOST_RCV_CTRL_2, reg);
        if(M2M_SUCCESS != ret) goto ERR1;


        if (ISNMC3400(nmi_get_chipid())) {

            ret = nm_write_reg(INTERRUPT_CORTUS_1_3000D0, 1);
            if(M2M_SUCCESS != ret) goto ERR1;
        }

        dma_addr = 0;

        for(cnt = 0; cnt < 1000*5; cnt ++)
        {
            ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_2, (uint32_t *)&reg);
            if(ret != M2M_SUCCESS) break;

            if (!(reg & 0x2))
            {
                ret = nm_read_reg_with_ret(0x150400, (uint32_t *)&dma_addr);
                if(ret != M2M_SUCCESS) {
                    /*in case of read error clear the dma address and return error*/
                    dma_addr = 0;
                }
                /*in case of success break */
                break;
            }
            //If we are struggling to get a response, start waiting longer
            if (cnt>=1000)
                nm_sleep(5);
        }

        if (dma_addr != 0)
        {
            volatile uint32_t u32CurrAddr;
            u32CurrAddr = dma_addr;
            strHif.u16Length=NM_BSP_B_L_16(strHif.u16Length);
            M2M_DBG("Writing into %lx %ld\r\n", dma_addr, strHif.u16Length);
            ret = nm_write_block(u32CurrAddr, (uint8_t*)&strHif, M2M_HIF_HDR_OFFSET);
            if(M2M_SUCCESS != ret) goto ERR1;
            u32CurrAddr += M2M_HIF_HDR_OFFSET;
            if(pu8CtrlBuf != NULL)
            {
                ret = nm_write_block(u32CurrAddr, pu8CtrlBuf, u16CtrlBufSize);
                if(M2M_SUCCESS != ret) goto ERR1;
                u32CurrAddr += u16CtrlBufSize;
            }
            if(pu8DataBuf != NULL)
            {
                u32CurrAddr += (u16DataOffset - u16CtrlBufSize);
                ret = nm_write_block(u32CurrAddr, pu8DataBuf, u16DataSize);
                if(M2M_SUCCESS != ret) goto ERR1;
                u32CurrAddr += u16DataSize;
            }

            reg = dma_addr << 2;

            /* Following line of code is to generate the interrupt which is not strictly needed for 3400,
                but has no noticeable side effects
            */
            reg |= (1 << 1);
            ret = nm_write_reg(WIFI_HOST_RCV_CTRL_3, reg);
            if(M2M_SUCCESS != ret) goto ERR1;

            if (ISNMC3400(nmi_get_chipid())) {
                ret = nm_write_reg(INTERRUPT_CORTUS_2_3000D0, 1);
                if(M2M_SUCCESS != ret) goto ERR1;
            }
        }
        else
        {
            M2M_DBG("Failed to alloc rx size\r\n");
            ret =  M2M_ERR_MEM_ALLOC;
            goto ERR1;
        }

    }
    else
    {
        M2M_ERR("(HIF)Failed to wakeup the chip\r\n");
        goto ERR1;
    }
    ret = hif_chip_sleep();

ERR1:
    OSAL_SEM_Post(&hifSemaphore);
    return ret;
}
/**
*   @fn     hif_isr
*   @brief  Host interface interrupt service routine
*   @return 1 in case of interrupt received else 0 will be returned
*/
static int8_t hif_isr(void)
{
    int8_t ret = M2M_SUCCESS;
    uint32_t reg;
    volatile tstrHifHdr strHif;

    while (OSAL_RESULT_FALSE == OSAL_SEM_Pend(&hifSemaphore, OSAL_WAIT_FOREVER))
    {
    }

    ret = nm_read_reg_with_ret(0x1070, &reg);
    if(M2M_SUCCESS == ret)
    {
        if(reg & 0x1)   /* New interrupt has been received */
        {
            uint16_t size;

            /*Clearing RX interrupt*/
            ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_0,&reg);
            if(ret != M2M_SUCCESS)goto ERR1;
            reg &= ~(1<<0);
            ret = nm_write_reg(WIFI_HOST_RCV_CTRL_0,reg);
            if(ret != M2M_SUCCESS)goto ERR1;
            /* read the rx size */
            ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_0, &reg);
            if(M2M_SUCCESS != ret)
            {
                M2M_ERR("(hif) WIFI_HOST_RCV_CTRL_0 bus fail\r\n");
                goto ERR1;
            }
            gu8HifSizeDone = 0;
            size = (uint16_t)((reg >> 2) & 0xfff);
            if (size > 0) {
                uint32_t address = 0;
                /**
                start bus transfer
                **/
                ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_1, &address);
                if(M2M_SUCCESS != ret)
                {
                    M2M_ERR("(hif) WIFI_HOST_RCV_CTRL_1 bus fail\r\n");
                    goto ERR1;
                }
                ret = nm_read_block(address, (uint8_t*)&strHif, sizeof(tstrHifHdr));
                strHif.u16Length = NM_BSP_B_L_16(strHif.u16Length);
                if(M2M_SUCCESS != ret)
                {
                    M2M_ERR("(hif) address bus fail\r\n");
                    goto ERR1;
                }
                if(strHif.u16Length != size)
                {
                    if((size - strHif.u16Length) > 4)
                    {
                        M2M_ERR("(hif) Corrupted packet Size = %u <L = %u, G = %u, OP = %02X>\r\n",
                            size, strHif.u16Length, strHif.u8Gid, strHif.u8Opcode);
                        ret = M2M_ERR_BUS_FAIL;
                        goto ERR1;
                    }
                }

                OSAL_SEM_Post(&hifSemaphore);

                if(M2M_REQ_GROUP_WIFI == strHif.u8Gid)
                {
                    if(pfWifiCb)
                        pfWifiCb(strHif.u8Opcode,strHif.u16Length - M2M_HIF_HDR_OFFSET, address + M2M_HIF_HDR_OFFSET);
                }
                else if(M2M_REQ_GROUP_IP == strHif.u8Gid)
                {
                    if(pfIpCb)
                        pfIpCb(strHif.u8Opcode,strHif.u16Length - M2M_HIF_HDR_OFFSET, address + M2M_HIF_HDR_OFFSET);
                }
                else if(M2M_REQ_GROUP_OTA == strHif.u8Gid)
                {
                    if(pfOtaCb)
                        pfOtaCb(strHif.u8Opcode,strHif.u16Length - M2M_HIF_HDR_OFFSET, address + M2M_HIF_HDR_OFFSET);
                }
                else if(M2M_REQ_GROUP_SIGMA == strHif.u8Gid)
                {
                    if(pfSigmaCb)
                        pfSigmaCb(strHif.u8Opcode,strHif.u16Length - M2M_HIF_HDR_OFFSET, address + M2M_HIF_HDR_OFFSET);
                }
                else if(M2M_REQ_GROUP_SSL == strHif.u8Gid)
                {
                    if(pfSSLCb)
                        pfSSLCb(strHif.u8Opcode,strHif.u16Length - M2M_HIF_HDR_OFFSET, address + M2M_HIF_HDR_OFFSET);
                }
                else
                {
                    M2M_ERR("(hif) invalid group ID\r\n");
                    return M2M_ERR_BUS_FAIL;
                }

                return M2M_SUCCESS;
            }
            else
            {
                ret = M2M_ERR_RCV;
                M2M_ERR("(hif) Wrong Size\r\n");
                goto ERR1;
            }
        }
        else
        {
            M2M_ERR("(hif) False interrupt %lx\r\n",reg);
            ret = M2M_ERR_FAIL;
        }
    }
    else
    {
        M2M_ERR("(hif) Failed to read interrupt reg\r\n");
    }

ERR1:
    OSAL_SEM_Post(&hifSemaphore);
    return ret;
}

/**
*   @fn     hif_handle_isr(void)
*   @brief  Handle interrupt received from WINC3400 firmware.
*   @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t hif_handle_isr(void)
{
    int8_t ret = M2M_SUCCESS;

    ret = hif_chip_wake();
    if (M2M_SUCCESS != ret)
    {
        M2M_ERR("(hif) Failed to wakeup the chip\r\n");
    }
    else
    {
        ret = hif_isr();
        if (M2M_SUCCESS != ret)
        {
            M2M_ERR("(hif) Failed to handle interrupt %d try Again..\r\n",ret);

            hif_chip_sleep();
        }
        else
        {
            ret = hif_chip_sleep();
        }
    }

    return ret;
}

/*
*   @fn     hif_receive
*   @brief  Host interface interrupt service routine
*   @param [in] u32Addr
*               Receive start address
*   @param [out]    pu8Buf
*               Pointer to receive buffer. Allocated by the caller
*   @param [in] u16Sz
*               Receive buffer size
*   @param [in] isDone
*               If you don't need any more packets send True otherwise send false
*   @return     The function shall return ZERO for successful operation and a negative value otherwise.
*/
int8_t hif_receive(uint32_t u32Addr, uint8_t *pu8Buf, uint16_t u16Sz, uint8_t isDone)
{
    uint32_t address, reg;
    uint16_t size;
    int8_t ret = M2M_SUCCESS;

    if(u32Addr == 0 || pu8Buf == NULL || u16Sz == 0)
    {
        if(isDone)
        {
            gu8HifSizeDone = 1;

            /* set RX done */
            ret = hif_set_rx_done();

            if((u32Addr == 0) && (pu8Buf == NULL) && (u16Sz == 0))
                return M2M_SUCCESS;
        }

        ret = M2M_ERR_FAIL;
        M2M_ERR(" hif_receive: Invalid argument\r\n");
        goto ERR1;
    }

    ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_0, &reg);
    if(ret != M2M_SUCCESS)goto ERR1;


    size = (uint16_t)((reg >> 2) & 0xfff);
    ret = nm_read_reg_with_ret(WIFI_HOST_RCV_CTRL_1, &address);
    if(ret != M2M_SUCCESS)goto ERR1;

    /* Receive the payload */
    ret = nm_read_block(u32Addr, pu8Buf, u16Sz);
    if(ret != M2M_SUCCESS)goto ERR1;

    if(u16Sz > size)
    {
        ret = M2M_ERR_FAIL;
        M2M_ERR("APP Requested Size is larger than the received buffer size <%d><%d>\r\n",u16Sz, size);
        goto ERR1;
    }
    if((u32Addr < address)||((u32Addr + u16Sz)>(address+size)))
    {
        ret = M2M_ERR_FAIL;
        M2M_ERR("APP Requested Address beyond the received buffer address and length\r\n");
        goto ERR1;
    }

    /* check if this is the last packet */
    if(isDone || (((address+size) - (u32Addr+u16Sz)) == 0) ||
        ((4 - ((u32Addr+u16Sz) & 3)) == ((address+size) - (u32Addr+u16Sz))))    /* Length in the RCV CTRL 0 register is rounded off to 4 by the firmware,
                                                                                   but length inside the HIF header is not, Hence consider done if number
                                                                                   of rounding bytes equal to length left to read */
    {
        gu8HifSizeDone = 1;

        /* set RX done */
        ret = hif_set_rx_done();
    }

ERR1:
    return ret;
}

/**
*   @fn     hif_register_cb
*   @brief  To set Callback function for every component
*   @param [in] u8Grp
*               Group to which the Callback function should be set.
*   @param [in] fn
*               function to be set
*   @return     The function shall return ZERO for successful operation and a negative value otherwise.
*/
int8_t hif_register_cb(uint8_t u8Grp, tpfHifCallBack fn)
{
    int8_t ret = M2M_SUCCESS;
    switch(u8Grp)
    {
        case M2M_REQ_GROUP_IP:
            pfIpCb = fn;
            break;
        case M2M_REQ_GROUP_WIFI:
            pfWifiCb = fn;
            break;
        case M2M_REQ_GROUP_OTA:
            pfOtaCb = fn;
            break;
        case M2M_REQ_GROUP_HIF:
            pfHifCb = fn;
            break;
        case M2M_REQ_GROUP_SIGMA:
            pfSigmaCb = fn;
            break;
        case M2M_REQ_GROUP_SSL:
            pfSSLCb = fn;
            break;
        default:
            M2M_ERR("GRp ? %d\r\n", u8Grp);
            ret = M2M_ERR_FAIL;
            break;
    }
    return ret;
}

//DOM-IGNORE-END