/*
Copyright (C) 2023-24, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef WINC_SDIO_DRV_H
#define WINC_SDIO_DRV_H

#define WINC_SDIOREG_FN0_CCCR_IO_EN             0x00000002
#define WINC_SDIOREG_FN0_CCCR_IO_RDY            0x00000003
#define WINC_SDIOREG_FN0_CCCR_INT_EN            0x00000004
#define WINC_SDIOREG_FN0_CCCR_INT_PEND          0x00000005
#define WINC_SDIOREG_FN0_CCCR_IO_ABORT          0x00000006
#define WINC_SDIOREG_FN0_CCCR_BUS_IF_CTRL       0x00000007
#define WINC_SDIOREG_FN0_CCCR_FN0_BLK_SZ_L      0x00000010
#define WINC_SDIOREG_FN0_CCCR_FN0_BLK_SZ_H      0x00000011

#define WINC_SDIOREG_FN0_FBR_FN1_CSA_CFG        0x00000100
#define WINC_SDIOREG_FN0_FBR_FN1_CSA_PTR        0x0000010c
#define WINC_SDIOREG_FN0_FBR_FN1_CSA_DATA       0x0000010f
#define WINC_SDIOREG_FN0_FBR_FN1_BLK_SZ_L       0x00000110
#define WINC_SDIOREG_FN0_FBR_FN1_BLK_SZ_H       0x00000111

#define WINC_SDIOREG_FN0_CIS_CLOCK_WAKE_UP      0x00018000

#define WINC_SDIOREG_FN1_DATA                   0x10000000
#define WINC_SDIOREG_FN1_INT_ID_CLR             0x10000008
#define WINC_SDIOREG_FN1_INT_EN                 0x10000009
#define WINC_SDIOREG_FN1_AHB_TRANSCOUNT         0x1000000c
#define WINC_SDIOREG_FN1_SD_HOST_GP             0x10000024
#define WINC_SDIOREG_FN1_ARM_GP                 0x10000028
#define WINC_SDIOREG_FN1_RDDATRDY               0x1000002c

/* WINC_SDIOREG_FN0_CCCR_IO_EN/WINC_SDIOREG_FN0_CCCR_IO_RDY */
#define WINC_SDIO_REG_CCCR_FN_IO_1              0x02
#define WINC_SDIO_REG_CCCR_FN_IO_2              0x04
#define WINC_SDIO_REG_CCCR_FN_IO_3              0x08
#define WINC_SDIO_REG_CCCR_FN_IO_4              0x10
#define WINC_SDIO_REG_CCCR_FN_IO_5              0x20
#define WINC_SDIO_REG_CCCR_FN_IO_6              0x30
#define WINC_SDIO_REG_CCCR_FN_IO_7              0x40

/* WINC_SDIOREG_FN0_CCCR_INT_EN/WINC_SDIOREG_FN0_CCCR_INT_PEND */
#define WINC_SDIO_REG_CCCR_INT_EN_MASTER        0x01
#define WINC_SDIO_REG_CCCR_FN_INT_1             0x02
#define WINC_SDIO_REG_CCCR_FN_INT_2             0x04
#define WINC_SDIO_REG_CCCR_FN_INT_3             0x08
#define WINC_SDIO_REG_CCCR_FN_INT_4             0x10
#define WINC_SDIO_REG_CCCR_FN_INT_5             0x20
#define WINC_SDIO_REG_CCCR_FN_INT_6             0x30
#define WINC_SDIO_REG_CCCR_FN_INT_7             0x40

/* WINC_SDIOREG_FN0_CCCR_BUS_IF_CTRL */
#define WINC_SDIO_REG_CCCR_BUS_IF_CTRL_CD_DIS   0x80
#define WINC_SDIO_REG_CCCR_BUS_IF_CTRL_SCSI     0x40
#define WINC_SDIO_REG_CCCR_BUS_IF_CTRL_ECSI     0x20
#define WINC_SDIO_REG_CCCR_BUS_IF_CTRL_S8B      0x04
#define WINC_SDIO_REG_CCCR_BUS_IF_CTRL_BUS_1    0x00
#define WINC_SDIO_REG_CCCR_BUS_IF_CTRL_BUS_4    0x02
#define WINC_SDIO_REG_CCCR_BUS_IF_CTRL_BUS_8    0x03

/* WINC_SDIOREG_FN0_FBR_FN1_CSA_CFG */
#define WINC_SDIO_REG_CSA_CFG_EN                0x80
#define WINC_SDIO_REG_CSA_CFG_SUP               0x40
#define WINC_SDIO_REG_CSA_CFG_INTF_CODE_MASK    0x0f

/* WINC_SDIOREG_FN1_INT_ID_CLR */
#define WINC_SDIO_REG_FN1_INT_DATA_RDY          0x01
#define WINC_SDIO_REG_FN1_INT_READ_ERR          0x02
#define WINC_SDIO_REG_FN1_INT_MSG_FROM_ARM      0x04
#define WINC_SDIO_REG_FN1_INT_ACK_TO_HOST       0x08

/* R1 */
#define WINC_SDIO_R1RSP_OK                      0x00
#define WINC_SDIO_R1RSP_FAILED                  0xff
#define WINC_SDIO_R1RSP_IDLE                    0x01
#define WINC_SDIO_R1RSP_ERASE_RST               0x02
#define WINC_SDIO_R1RSP_ILLEGAL_CMD             0x04
#define WINC_SDIO_R1RSP_COM_CRC_ERR             0x08
#define WINC_SDIO_R1RSP_ERASE_ERR               0x10
#define WINC_SDIO_R1RSP_FUNC_NUM_ERR            0x10
#define WINC_SDIO_R1RSP_ADDR_ERR                0x20
#define WINC_SDIO_R1RSP_PARAM_ERR               0x40

/*****************************************************************************
  Description:
    SDIO initialisation states.

  Remarks:

 *****************************************************************************/

typedef enum
{
    WINC_SDIO_STATE_UNKNOWN,
    WINC_SDIO_STATE_ERROR,
    WINC_SDIO_STATE_RESETTING,
    WINC_SDIO_STATE_SEND_OP,
    WINC_SDIO_STATE_WAIT_OP,
    WINC_SDIO_STATE_CONFIG,
    WINC_SDIO_STATE_RUNNING,
} WINC_SDIO_STATE_TYPE;

/*****************************************************************************
  Description:
    SDIO initialisation states values.

  Remarks:
    Negative values are errors.

 *****************************************************************************/

typedef enum
{
    WINC_SDIO_STATUS_ABORT_FAILED       = -6,
    WINC_SDIO_STATUS_RESET_FAILED       = -5,
    WINC_SDIO_STATUS_OP_FAILED          = -4,
    WINC_SDIO_STATUS_CFG_FAILED         = -3,
    WINC_SDIO_STATUS_ERROR              = -2,
    WINC_SDIO_STATUS_UNKNOWN            = -1,
    WINC_SDIO_STATUS_OK                 = 0,
    WINC_SDIO_STATUS_RESET_WAITING,
    WINC_SDIO_STATUS_OP_WAITING,
} WINC_SDIO_STATUS_TYPE;

/* SDIO SPI send/receive function type definition. */
typedef bool (*WINC_SDIO_SEND_RECEIVE_FP)(void* pTransmitData, void* pReceiveData, size_t size);

/*****************************************************************************
                          WINC SDIO Module API
 *****************************************************************************/

uint8_t WINC_SDIOCmd0(void);
uint8_t WINC_SDIOCmd5(uint32_t ocr, uint32_t *pRspData);
uint8_t WINC_SDIOCmd8(void);
uint8_t WINC_SDIOCmd52(uint32_t fnRegAddr, const uint8_t* const pWriteValue, uint8_t* const pReadValue);
uint8_t WINC_SDIOCmd59(void);
uint16_t WINC_SDIOCmd53Write(uint32_t fnRegAddr, uint8_t *pWritePtr, size_t writeLength, bool incAddr);
uint16_t WINC_SDIOCmd53Read(uint32_t fnRegAddr, uint8_t *pReadPtr, size_t readLength, bool incAddr);
WINC_SDIO_STATUS_TYPE WINC_SDIODeviceInit(WINC_SDIO_STATE_TYPE *pState, WINC_SDIO_SEND_RECEIVE_FP pfSendReceive);

#endif /* WINC_SDIO_DRV_H */
