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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "conf_winc_dev.h"
#include "winc_sdio_drv.h"
#include "winc_dev.h"
#include "winc_debug.h"

typedef struct
{
    uint32_t    regAddr;
    uint8_t     regVal;
} WINC_SDIO_CMD52_REG_ENTRY;

#define SPI_SDIO_BLOCK_SZ       512

static const uint8_t crc7[256] = {
    0x00, 0x12, 0x24, 0x36, 0x48, 0x5a, 0x6c, 0x7e, 0x90, 0x82, 0xb4, 0xa6, 0xd8, 0xca, 0xfc, 0xee,
    0x32, 0x20, 0x16, 0x04, 0x7a, 0x68, 0x5e, 0x4c, 0xa2, 0xb0, 0x86, 0x94, 0xea, 0xf8, 0xce, 0xdc,
    0x64, 0x76, 0x40, 0x52, 0x2c, 0x3e, 0x08, 0x1a, 0xf4, 0xe6, 0xd0, 0xc2, 0xbc, 0xae, 0x98, 0x8a,
    0x56, 0x44, 0x72, 0x60, 0x1e, 0x0c, 0x3a, 0x28, 0xc6, 0xd4, 0xe2, 0xf0, 0x8e, 0x9c, 0xaa, 0xb8,
    0xc8, 0xda, 0xec, 0xfe, 0x80, 0x92, 0xa4, 0xb6, 0x58, 0x4a, 0x7c, 0x6e, 0x10, 0x02, 0x34, 0x26,
    0xfa, 0xe8, 0xde, 0xcc, 0xb2, 0xa0, 0x96, 0x84, 0x6a, 0x78, 0x4e, 0x5c, 0x22, 0x30, 0x06, 0x14,
    0xac, 0xbe, 0x88, 0x9a, 0xe4, 0xf6, 0xc0, 0xd2, 0x3c, 0x2e, 0x18, 0x0a, 0x74, 0x66, 0x50, 0x42,
    0x9e, 0x8c, 0xba, 0xa8, 0xd6, 0xc4, 0xf2, 0xe0, 0x0e, 0x1c, 0x2a, 0x38, 0x46, 0x54, 0x62, 0x70,
    0x82, 0x90, 0xa6, 0xb4, 0xca, 0xd8, 0xee, 0xfc, 0x12, 0x00, 0x36, 0x24, 0x5a, 0x48, 0x7e, 0x6c,
    0xb0, 0xa2, 0x94, 0x86, 0xf8, 0xea, 0xdc, 0xce, 0x20, 0x32, 0x04, 0x16, 0x68, 0x7a, 0x4c, 0x5e,
    0xe6, 0xf4, 0xc2, 0xd0, 0xae, 0xbc, 0x8a, 0x98, 0x76, 0x64, 0x52, 0x40, 0x3e, 0x2c, 0x1a, 0x08,
    0xd4, 0xc6, 0xf0, 0xe2, 0x9c, 0x8e, 0xb8, 0xaa, 0x44, 0x56, 0x60, 0x72, 0x0c, 0x1e, 0x28, 0x3a,
    0x4a, 0x58, 0x6e, 0x7c, 0x02, 0x10, 0x26, 0x34, 0xda, 0xc8, 0xfe, 0xec, 0x92, 0x80, 0xb6, 0xa4,
    0x78, 0x6a, 0x5c, 0x4e, 0x30, 0x22, 0x14, 0x06, 0xe8, 0xfa, 0xcc, 0xde, 0xa0, 0xb2, 0x84, 0x96,
    0x2e, 0x3c, 0x0a, 0x18, 0x66, 0x74, 0x42, 0x50, 0xbe, 0xac, 0x9a, 0x88, 0xf6, 0xe4, 0xd2, 0xc0,
    0x1c, 0x0e, 0x38, 0x2a, 0x54, 0x46, 0x70, 0x62, 0x8c, 0x9e, 0xa8, 0xba, 0xc4, 0xd6, 0xe0, 0xf2
};

static const uint16_t crc16[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static const WINC_SDIO_CMD52_REG_ENTRY cmd52InitSeq1[] =
{
    {WINC_SDIOREG_FN0_FBR_FN1_BLK_SZ_L,     (SPI_SDIO_BLOCK_SZ & 0xff)},
    {WINC_SDIOREG_FN0_FBR_FN1_BLK_SZ_H,     (SPI_SDIO_BLOCK_SZ >> 8)},
    {WINC_SDIOREG_FN0_FBR_FN1_CSA_CFG,      WINC_SDIO_REG_CSA_CFG_EN},
    {WINC_SDIOREG_FN0_CCCR_FN0_BLK_SZ_L,    (SPI_SDIO_BLOCK_SZ & 0xff)},
    {WINC_SDIOREG_FN0_CCCR_FN0_BLK_SZ_H,    (SPI_SDIO_BLOCK_SZ >> 8)},
    {WINC_SDIOREG_FN0_CCCR_IO_EN,           WINC_SDIO_REG_CCCR_FN_IO_1},
    {WINC_SDIOREG_FN0_CCCR_BUS_IF_CTRL,     WINC_SDIO_REG_CCCR_BUS_IF_CTRL_ECSI | WINC_SDIO_REG_CCCR_BUS_IF_CTRL_BUS_1},
    {WINC_SDIOREG_FN0_CCCR_INT_EN,          WINC_SDIO_REG_CCCR_INT_EN_MASTER | WINC_SDIO_REG_CCCR_FN_INT_1},
    {WINC_SDIOREG_FN0_CIS_CLOCK_WAKE_UP,    0x01},
    {WINC_SDIOREG_FN1_INT_EN,               WINC_SDIO_REG_FN1_INT_DATA_RDY},
};

#ifdef WINC_DEV_CACHE_LINE_SIZE
static __attribute__((aligned(WINC_DEV_CACHE_LINE_SIZE))) uint8_t sdioCmd[WINC_DEV_CACHE_GET_SIZE(13)];
static __attribute__((aligned(WINC_DEV_CACHE_LINE_SIZE))) uint8_t sdioCmdRsp[WINC_DEV_CACHE_GET_SIZE(13)];
#else
static uint8_t sdioCmd[13];
static uint8_t sdioCmdRsp[13];
#endif

static bool useCRCs = false;
static WINC_SDIO_SEND_RECEIVE_FP pfSDIOSendReceive = NULL;

/*****************************************************************************
  Description:
    Calculate SDIO CRC7.

  Parameters:
    p - Pointer to data to checksum
    l - Length of data to checksum

  Returns:
    CRC7 of data or zero if disabled

  Remarks:

 *****************************************************************************/

static uint8_t sdioCRC7(const uint8_t *p, size_t l)
{
    uint8_t crc = 0;

    if (false == useCRCs)
    {
        return 0;
    }

    while (l--)
    {
        crc = crc7[crc ^ *p++];
    }

    return crc & 0xfe;
}

/*****************************************************************************
  Description:
    Calculates SDIO CRC16.

  Parameters:
    p - Pointer to data to checksum
    l - Length of data to checksum

  Returns:
    CRC16 of data or zero if disabled

  Remarks:

 *****************************************************************************/

static uint16_t sdioCRC16(const uint8_t *p, size_t l)
{
    uint16_t crc = 0;

    if (false == useCRCs)
    {
        return 0;
    }

    while (l--)
    {
        crc = (crc << 8) ^ crc16[((crc >> 8) ^ *p++)];
    }

    return (crc << 8) | ((crc >> 8) & 0xff);
}

/*****************************************************************************
  Description:
    Writes a sequence of CMD52 messages.

  Parameters:
    pEntry     - Pointer to first entry to send
    numEntries - Number of entries

  Returns:
    true or false indicating success of failure

  Remarks:

 *****************************************************************************/

static bool sdioCmd52SeqWrite(const WINC_SDIO_CMD52_REG_ENTRY *pEntry, int numEntries)
{
    if ((NULL == pEntry) || (0 == numEntries))
    {
        return false;
    }

    while (numEntries--)
    {
        uint8_t rspStatus;

        rspStatus = WINC_SDIOCmd52(pEntry->regAddr, &pEntry->regVal, NULL);

        if (0x00 != rspStatus)
        {
            return false;
        }

        pEntry++;
    }

    return true;
}

/*****************************************************************************
  Description:
    Send SDIO CMD0.

  Parameters:
    None

  Returns:
    R1 response or WINC_SDIO_R1RSP_FAILED on error.

  Remarks:

 *****************************************************************************/

uint8_t WINC_SDIOCmd0(void)
{
    WINC_VERBOSE_PRINT("C0");

    if (NULL == pfSDIOSendReceive)
    {
        return WINC_SDIO_R1RSP_FAILED;
    }

    memset(sdioCmd, 0xff, 9);
    memset(&sdioCmd[2], 0, 4);

    sdioCmd[1] = 0x40;
    sdioCmd[6] = 0x95;

    pfSDIOSendReceive(sdioCmd, sdioCmdRsp, 9);

    WINC_VERBOSE_PRINT("R1[%02x]", sdioCmdRsp[8]);

    return sdioCmdRsp[8];
}

/*****************************************************************************
  Description:
    Send SDIO CMD5.

  Parameters:
    ocr      - OCR value to send in message
    pRspData - Pointer to buffer to receive response

  Returns:
    R1 response or WINC_SDIO_R1RSP_FAILED on error.

  Remarks:

 *****************************************************************************/

uint8_t WINC_SDIOCmd5(uint32_t ocr, uint32_t *pRspData)
{
    WINC_VERBOSE_PRINT("C5");

    if ((NULL == pfSDIOSendReceive) || (NULL == pRspData))
    {
        return WINC_SDIO_R1RSP_FAILED;
    }

    memset(sdioCmd, 0xff, 13);

    sdioCmd[1] = 0x40 | 0x05;
    sdioCmd[2] = 0x00;
    sdioCmd[3] = (ocr >> 16) & 0xff;
    sdioCmd[4] = (ocr >> 8) & 0xff;
    sdioCmd[5] = ocr & 0xff;
    sdioCmd[6] = sdioCRC7(&sdioCmd[1], 5) | 0x01;

    pfSDIOSendReceive(sdioCmd, sdioCmdRsp, 13);

    WINC_VERBOSE_PRINT("R1[%02x]", sdioCmdRsp[8]);

    *pRspData = ((uint32_t)sdioCmdRsp[9]) << 24;
    *pRspData |= sdioCmdRsp[10] << 16;
    *pRspData |= sdioCmdRsp[11] << 8;
    *pRspData |= sdioCmdRsp[12];

    WINC_VERBOSE_PRINT("R5[%08x]", *pRspData);

    return sdioCmdRsp[8];
}

/*****************************************************************************
  Description:
    Send SDIO CMD8.

  Parameters:
    None

  Returns:
    R1 response or WINC_SDIO_R1RSP_FAILED on error.

  Remarks:

 *****************************************************************************/

uint8_t WINC_SDIOCmd8(void)
{
    WINC_VERBOSE_PRINT("C8");

    if (NULL == pfSDIOSendReceive)
    {
        return WINC_SDIO_R1RSP_FAILED;
    }

    memset(sdioCmd, 0xff, 9);

    sdioCmd[1] = 0x40 | 0x08;
    sdioCmd[2] = 0x00;
    sdioCmd[2] = 0x00;
    sdioCmd[4] = 0x01;
    sdioCmd[5] = 0xaa;
    sdioCmd[6] = sdioCRC7(&sdioCmd[1], 5) | 0x01;

    pfSDIOSendReceive(sdioCmd, sdioCmdRsp, 9);

    WINC_VERBOSE_PRINT("R1[%02x]", sdioCmdRsp[8]);

    return sdioCmdRsp[8];
}

/*****************************************************************************
  Description:
    Send/receive SDIO CMD52.

  Parameters:
    fnRegAddr   - Function | register address
    pWriteValue - Pointer to value to send
    pReadValue  - Pointer to location to store received value

  Returns:
    R1 response or WINC_SDIO_R1RSP_FAILED on error.

  Remarks:

 *****************************************************************************/

uint8_t WINC_SDIOCmd52(uint32_t fnRegAddr, const uint8_t* const pWriteValue, uint8_t* const pReadValue)
{
    WINC_VERBOSE_PRINT("C52{%04x}", fnRegAddr);

    if (NULL == pfSDIOSendReceive)
    {
        return WINC_SDIO_R1RSP_FAILED;
    }

    memset(sdioCmd, 0xff, 11);

    sdioCmd[2] = 0x00;

    if (NULL != pWriteValue)
    {
        /* R/W = 1 (W) */
        sdioCmd[2] |= 0x80;

        /* Write Data=value */
        sdioCmd[5] = *pWriteValue;

        if (NULL != pReadValue)
        {
            /* RAW = 1 */
            sdioCmd[2] |= 0x08;
        }
    }
    else
    {
        sdioCmd[5] = 0;
    }

    /* S=0, D=1, Command Index = 52 */
    sdioCmd[1] = 0x40 | 0x34;
    /* R/W=?, Function Number = top 4 bits of fnRegAddr, RAW=?, Stuff=0, Register Address[16:15]=fnRegAddr[16:15] */
    sdioCmd[2] |= ((fnRegAddr >> 24) & 0x70) | 0 | ((fnRegAddr >> 15) & 0x03);
    /* Register Address[14:7]=fnRegAddr[14:7] */
    sdioCmd[3] = (fnRegAddr >> 7) & 0xff;
    /* Register Address[6:0]=fnRegAddr[6:0], Stuff=0 */
    sdioCmd[4] = ((fnRegAddr << 1) & 0xfe) | 0;
    /* CRC=0, E=1 */
    sdioCmd[6] = sdioCRC7(&sdioCmd[1], 5) | 0x01;

    pfSDIOSendReceive(sdioCmd, sdioCmdRsp, 11);

    WINC_VERBOSE_PRINT("R5[%02x%02x]", sdioCmdRsp[8], sdioCmdRsp[9]);

    if (NULL != pReadValue)
    {
        *pReadValue = sdioCmdRsp[9];
    }

    return sdioCmdRsp[8];
}

/*****************************************************************************
  Description:
    Send SDIO CMD59

  Parameters:
    None

  Returns:
    R1 response or WINC_SDIO_R1RSP_FAILED on error.

  Remarks:

 *****************************************************************************/

uint8_t WINC_SDIOCmd59(void)
{
    WINC_VERBOSE_PRINT("C59");

    if (NULL == pfSDIOSendReceive)
    {
        return WINC_SDIO_R1RSP_FAILED;
    }

    memset(sdioCmd, 0xff, 9);

    sdioCmd[1] = 0x40 | 0x3b;
    sdioCmd[2] = 0x00;
    sdioCmd[2] = 0x00;
    sdioCmd[4] = 0x00;
    sdioCmd[5] = useCRCs ? 0x01 : 0x00;
    sdioCmd[6] = sdioCRC7(&sdioCmd[1], 5) | 0x01;

    pfSDIOSendReceive(sdioCmd, sdioCmdRsp, 9);

    WINC_VERBOSE_PRINT("R1[%02x]", sdioCmdRsp[8]);

    return sdioCmdRsp[8];
}

/*****************************************************************************
  Description:
    Send SDIO CMD53.

  Parameters:
    fnRegAddr   - Function | register address
    pWritePtr   - Pointer to buffer to send
    writeLength - Length of data to send
    incAddr     - Flag indicating if address should be incremented

  Returns:
    R1 response or WINC_SDIO_R1RSP_FAILED on error.

  Remarks:

 *****************************************************************************/

uint16_t WINC_SDIOCmd53Write(uint32_t fnRegAddr, uint8_t *pWritePtr, size_t writeLength, bool incAddr)
{
    uint16_t rspStatusData;
    uint16_t count;
    uint16_t transferSize;
    uint16_t crc = 0;
    bool blockMode;

    WINC_TRACE_PRINT("C53W{%08x %d}\n", fnRegAddr, writeLength);

    if (NULL == pfSDIOSendReceive)
    {
        return WINC_SDIO_R1RSP_FAILED;
    }

    memset(sdioCmd, 0xff, 11);

    do
    {
        sdioCmd[2] = 0x00;

        if (writeLength > SPI_SDIO_BLOCK_SZ)
        {
            count        = writeLength / SPI_SDIO_BLOCK_SZ;
            transferSize = SPI_SDIO_BLOCK_SZ;
            blockMode    = true;

            /* Block mode=1 */
            sdioCmd[2] |= 0x08;
        }
        else
        {
            count        = writeLength & 511;
            transferSize = writeLength;
            blockMode    = false;
        }

        /* S=0, D=1, Command Index = 53 */
        sdioCmd[1] = 0x40 | 0x35;
        /* R/W=1 (W), Function Number = top 4 bits of fnRegAddr, Block Mode=?, OP Mode=incAddr, Register Address[16:15]=fnRegAddr[16:15] */
        sdioCmd[2] |= 0x80 | ((fnRegAddr >> 24) & 0x70) | (incAddr?0x04:0x00) | ((fnRegAddr >> 15) & 0x03);
        /* Register Address[14:7]=fnRegAddr[14:7] */
        sdioCmd[3] = (fnRegAddr >> 7) & 0xff;
        /* Register Address[6:0]=fnRegAddr[6:0], Count[8:8] */
        sdioCmd[4] = ((fnRegAddr << 1) & 0xfe) | ((count >> 8) & 0x01);
        /* Count[7:0] */
        sdioCmd[5] = count & 0xff;
        /* CRC=0, E=1 */
        sdioCmd[6] = sdioCRC7(&sdioCmd[1], 5) | 0x01;

        pfSDIOSendReceive(sdioCmd, sdioCmdRsp, 11);

        rspStatusData = ((uint16_t)sdioCmdRsp[8] << 8) | sdioCmdRsp[9];

        WINC_VERBOSE_PRINT("R5[%04x]", rspStatusData);

        if (0 != rspStatusData)
        {
            return rspStatusData;
        }

        do
        {
            /* Send start block token. */
            sdioCmd[1] = blockMode ? 0xfc : 0xfe;

            pfSDIOSendReceive(sdioCmd, NULL, 2);

            /* Send data block and CRC. */
            crc = sdioCRC16(pWritePtr, transferSize);

            pfSDIOSendReceive(pWritePtr, NULL, transferSize);

            sdioCmd[5] = crc >> 8;
            sdioCmd[6] = crc & 0xff;

            pfSDIOSendReceive(&sdioCmd[5], sdioCmdRsp, 5);

            pWritePtr   += transferSize;
            writeLength -= transferSize;

            rspStatusData = sdioCmdRsp[2] & 0x1f;

            /* Wait for data response token. */
            while (0x00 == sdioCmdRsp[4])
            {
                pfSDIOSendReceive(NULL, &sdioCmdRsp[4], 1);
            }

            if (0x05 != rspStatusData)
            {
                return rspStatusData | (WINC_SDIO_R1RSP_FAILED << 8);
            }
        }
        while (writeLength >= SPI_SDIO_BLOCK_SZ);
    }
    while (writeLength > 0);

    return WINC_SDIO_R1RSP_OK;
}

/*****************************************************************************
  Description:
    Receive SDIO CMD53.

  Parameters:
    fnRegAddr   - Function | register address
    pReadPtr    - Pointer to buffer to receive into
    writeLength - Length of data to receive
    incAddr     - Flag indicating if address should be incremented

  Returns:
    R1 response or WINC_SDIO_R1RSP_FAILED on error.

  Remarks:

 *****************************************************************************/

uint16_t WINC_SDIOCmd53Read(uint32_t fnRegAddr, uint8_t *pReadPtr, size_t readLength, bool incAddr)
{
    uint16_t rspStatusData;
    uint16_t count;
    uint16_t transferSize;
    uint8_t regValue;
    uint8_t cmd52Status;

    WINC_TRACE_PRINT("C53R{%08x %d}\n", fnRegAddr, readLength);

    if (NULL == pfSDIOSendReceive)
    {
        return WINC_SDIO_R1RSP_FAILED;
    }

    memset(sdioCmd, 0xff, 11);

    do
    {
        if (WINC_SDIOREG_FN1_DATA == fnRegAddr)
        {
            cmd52Status = WINC_SDIOCmd52(WINC_SDIOREG_FN1_INT_ID_CLR, NULL, &regValue);

            if (0x00 != cmd52Status)
            {
                return cmd52Status | (WINC_SDIO_R1RSP_FAILED << 8);
            }

            if (0 == (regValue & WINC_SDIO_REG_FN1_INT_DATA_RDY))
            {
                continue;
            }

            regValue = WINC_SDIO_REG_FN1_INT_DATA_RDY;

            cmd52Status = WINC_SDIOCmd52(WINC_SDIOREG_FN1_INT_ID_CLR, &regValue, NULL);

            if (0x00 != cmd52Status)
            {
                return cmd52Status | (WINC_SDIO_R1RSP_FAILED << 8);
            }
        }

        sdioCmd[2] = 0x00;

        if (readLength > SPI_SDIO_BLOCK_SZ)
        {
            count        = readLength / SPI_SDIO_BLOCK_SZ;
            transferSize = SPI_SDIO_BLOCK_SZ;

            /* Block mode=1 */
            sdioCmd[2] |= 0x08;
        }
        else
        {
            count        = readLength & 511;
            transferSize = readLength;
        }

        /* S=0, D=1, Command Index = 53 */
        sdioCmd[1] = 0x40 | 0x35;
        /* R/W=0 (R), Function Number = top 4 bits of fnRegAddr, Block Mode=?, OP Mode=incAddr, Register Address[16:15]=fnRegAddr[16:15] */
        sdioCmd[2] |= 0 | ((fnRegAddr >> 24) & 0x70) | (incAddr?0x04:0x00) | ((fnRegAddr >> 15) & 0x03);
        /* Register Address[14:7]=fnRegAddr[14:7] */
        sdioCmd[3] = (fnRegAddr >> 7) & 0xff;
        /* Register Address[6:0]=fnRegAddr[6:0], Count[8:8] */
        sdioCmd[4] = ((fnRegAddr << 1) & 0xfe) | ((count >> 8) & 0x01);
        /* Count[7:0] */
        sdioCmd[5] = count & 0xff;
        /* CRC=0, E=1 */
        sdioCmd[6] = sdioCRC7(&sdioCmd[1], 5) | 0x01;

        pfSDIOSendReceive(sdioCmd, sdioCmdRsp, 11);

        rspStatusData = ((uint16_t)sdioCmdRsp[8] << 8) | sdioCmdRsp[9];

        WINC_VERBOSE_PRINT("R5[%04x]", rspStatusData);

        if (0 != rspStatusData)
        {
            return rspStatusData;
        }

        memset(sdioCmdRsp, 0xff, 3);

        do
        {
            /* Wait for MISO to rise */
            while (0x00 == sdioCmdRsp[2])
            {
                pfSDIOSendReceive(NULL, &sdioCmdRsp[2], 1);
            }

            /* Wait for 0xfe start block token. */
            while (0x00 != (sdioCmdRsp[2] & 0x01))
            {
                pfSDIOSendReceive(NULL, &sdioCmdRsp[2], 1);
            }

            if (0xfe != sdioCmdRsp[2])
            {
                return sdioCmdRsp[2] | (WINC_SDIO_R1RSP_FAILED << 8);
            }

            /* Receive data block and CRC. */
            pfSDIOSendReceive(NULL, pReadPtr, transferSize);

            pfSDIOSendReceive(&sdioCmd[7], sdioCmdRsp, 3);

            if (true == useCRCs)
            {
            }

            pReadPtr   += transferSize;
            readLength -= transferSize;
        }
        while (readLength >= SPI_SDIO_BLOCK_SZ);
    }
    while (readLength > 0);

    return WINC_SDIO_R1RSP_OK;
}

/*****************************************************************************
  Description:
    Initialise the SDIO device.

  Parameters:
    pState        - Pointer to current state, or NULL
    pfSendReceive - Pointer to SPI send/receive function to use

  Returns:
    WINC_SDIO_STATUS_ABORT_FAILED  - Abort operation failed
    WINC_SDIO_STATUS_RESET_FAILED  - Reset operation failed
    WINC_SDIO_STATUS_OP_FAILED     - CMD5 operation failed
    WINC_SDIO_STATUS_CFG_FAILED    - Configuration of device failed
    WINC_SDIO_STATUS_ERROR         - An error occurred
    WINC_SDIO_STATUS_OK            - Initialisation complete
    WINC_SDIO_STATUS_RESET_WAITING - Waiting for reset to complete
    WINC_SDIO_STATUS_OP_WAITING    - Waiting for CMD5 operation to complete

  Remarks:
    If pState is not provided the function will block while initialising the
    device. If it is provided then the function will return on of the waiting
    status codes and should be recalled to check progress.

 *****************************************************************************/

WINC_SDIO_STATUS_TYPE WINC_SDIODeviceInit(WINC_SDIO_STATE_TYPE *pState, WINC_SDIO_SEND_RECEIVE_FP pfSendReceive)
{
    WINC_SDIO_STATE_TYPE localState = WINC_SDIO_STATE_UNKNOWN;
    WINC_SDIO_STATUS_TYPE retStatus = WINC_SDIO_STATUS_ERROR;
    uint8_t rspStatus;
    uint8_t regVal;
    uint32_t cmd5Rsp;
    bool asyncInit = true;

    if (NULL == pState)
    {
        pState    = &localState;
        asyncInit = false;
    }

    pfSDIOSendReceive = pfSendReceive;

    do
    {
        switch (*pState)
        {
            case WINC_SDIO_STATE_UNKNOWN:
            {
                WINC_VERBOSE_PRINT("\n[CCCR Abort]\n");

                rspStatus = WINC_SDIOCmd52(WINC_SDIOREG_FN0_CCCR_IO_ABORT, NULL, &regVal);

                if (WINC_SDIO_R1RSP_IDLE == rspStatus)
                {
                    regVal |= 0x08;

                    rspStatus = WINC_SDIOCmd52(WINC_SDIOREG_FN0_CCCR_IO_ABORT, &regVal, NULL);

                    if (WINC_SDIO_R1RSP_IDLE != rspStatus)
                    {
                        *pState = WINC_SDIO_STATE_ERROR;
                        return WINC_SDIO_STATUS_ABORT_FAILED;
                    }
                }

                *pState = WINC_SDIO_STATE_RESETTING;

                /* Drop through */
            }

            case WINC_SDIO_STATE_RESETTING:
            {
                WINC_VERBOSE_PRINT("\n[Reset]\n");

                rspStatus = WINC_SDIOCmd0();

                //The status can be 0x00 if the WINCS02 internal pull-up on MISO has not yet been activated
                if ((WINC_SDIO_R1RSP_FAILED == rspStatus) || (WINC_SDIO_R1RSP_OK == rspStatus))
                {
                    retStatus = WINC_SDIO_STATUS_RESET_WAITING;
                    break;
                }
                else if (WINC_SDIO_R1RSP_IDLE != rspStatus)
                {
                    *pState = WINC_SDIO_STATE_ERROR;
                    return WINC_SDIO_STATUS_RESET_FAILED;
                }

                *pState = WINC_SDIO_STATE_SEND_OP;

                /* Drop through */
            }

            case WINC_SDIO_STATE_SEND_OP:
            {
                WINC_VERBOSE_PRINT("\n[CMD5]\n");

                rspStatus = WINC_SDIOCmd5(0, &cmd5Rsp);

                if (WINC_SDIO_R1RSP_IDLE != rspStatus)
                {
                    *pState = WINC_SDIO_STATE_ERROR;
                    return WINC_SDIO_STATUS_OP_FAILED;
                }

                *pState = WINC_SDIO_STATE_WAIT_OP;

                /* Drop through */
            }

            case WINC_SDIO_STATE_WAIT_OP:
            {
                cmd5Rsp = 0;

                rspStatus = WINC_SDIOCmd5(0x200000, &cmd5Rsp);

                if (WINC_SDIO_R1RSP_OK != (rspStatus & 0xfe))
                {
                    *pState = WINC_SDIO_STATE_ERROR;
                    return WINC_SDIO_STATUS_OP_FAILED;
                }

                if (0 == (cmd5Rsp & 0x80000000))
                {
                    retStatus = WINC_SDIO_STATUS_OP_WAITING;
                    break;
                }

                *pState = WINC_SDIO_STATE_CONFIG;

                /* Drop through */
            }

            case WINC_SDIO_STATE_CONFIG:
            {
                *pState = WINC_SDIO_STATE_ERROR;

                WINC_VERBOSE_PRINT("\n[CMD59]\n");

                rspStatus = WINC_SDIOCmd59();

                if (WINC_SDIO_R1RSP_OK != rspStatus)
                {
                    return WINC_SDIO_STATUS_CFG_FAILED;
                }

                WINC_VERBOSE_PRINT("\n[CMD52 Seq Write]\n");

                if (false == sdioCmd52SeqWrite(cmd52InitSeq1, sizeof(cmd52InitSeq1)/sizeof(WINC_SDIO_CMD52_REG_ENTRY)))
                {
                    return WINC_SDIO_STATUS_CFG_FAILED;
                }

                rspStatus = WINC_SDIOCmd52(WINC_SDIOREG_FN0_FBR_FN1_CSA_CFG, NULL, &regVal);

                if (WINC_SDIO_R1RSP_OK != rspStatus)
                {
                    return WINC_SDIO_STATUS_CFG_FAILED;
                }

                if (0 == (regVal & WINC_SDIO_REG_CSA_CFG_EN))
                {
                    return WINC_SDIO_STATUS_CFG_FAILED;
                }

                rspStatus = WINC_SDIOCmd52(WINC_SDIOREG_FN0_CCCR_IO_RDY, NULL, &regVal);

                if (WINC_SDIO_R1RSP_OK != rspStatus)
                {
                    return WINC_SDIO_STATUS_CFG_FAILED;
                }

                if (WINC_SDIO_REG_CCCR_FN_IO_1 != (regVal & WINC_SDIO_REG_CCCR_FN_IO_1))
                {
                    return WINC_SDIO_STATUS_CFG_FAILED;
                }

                rspStatus = WINC_SDIOCmd52(WINC_SDIOREG_FN0_CCCR_INT_EN, NULL, &regVal);

                if (WINC_SDIO_R1RSP_OK != rspStatus)
                {
                    return WINC_SDIO_STATUS_CFG_FAILED;
                }

                if ((WINC_SDIO_REG_CCCR_INT_EN_MASTER | WINC_SDIO_REG_CCCR_FN_INT_1) != (regVal & (WINC_SDIO_REG_CCCR_INT_EN_MASTER | WINC_SDIO_REG_CCCR_FN_INT_1)))
                {
                    return WINC_SDIO_STATUS_CFG_FAILED;
                }

                *pState = WINC_SDIO_STATE_RUNNING;
                return WINC_SDIO_STATUS_OK;
            }

            case WINC_SDIO_STATE_RUNNING:
            {
                return WINC_SDIO_STATUS_OK;
            }

            case WINC_SDIO_STATE_ERROR:
            default:
            {
                return WINC_SDIO_STATUS_ERROR;
            }
        }
    }
    while (false == asyncInit);

    return retStatus;
}
