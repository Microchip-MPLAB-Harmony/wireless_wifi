/**
 *
 * \file
 *
 * \brief WINC Crypto module.
 *
 * Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "bsp/winc_bsp.h"
#include "common/winc_defines.h"
#include "common/winc_debug.h"
#include "winc_m2m_types.h"
#include "winc_m2m_crypto.h"
#include "driver/winc_hif.h"
#include "driver/winc_asic.h"
#include "driver/winc_spi.h"

#ifdef CONF_CRYPTO_HW

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*======*======*======*======*======*=======*
* WINC SHA256 HW Engine Register Definition *
*======*======*======*======*======*========*/

#define SHA_BLOCK_SIZE                                          (64)

#define SHARED_MEM_BASE                                         (0xd0000)

#define SHA256_MEM_BASE                                         (0x180000UL)
#define SHA256_ENGINE_ADDR                                      (0x180000ul)

/* SHA256 Registers */
#define SHA256_CTRL                                             (SHA256_MEM_BASE+0x00)
#define SHA256_CTRL_START_CALC_MASK                             (NBIT0)
#define SHA256_CTRL_START_CALC_SHIFT                            (0)
#define SHA256_CTRL_PREPROCESS_MASK                             (NBIT1)
#define SHA256_CTRL_PREPROCESS_SHIFT                            (1)
#define SHA256_CTRL_HASH_HASH_MASK                              (NBIT2)
#define SHA256_CTRL_HASH_HASH_SHIFT                             (2)
#define SHA256_CTRL_INIT_SHA256_STATE_MASK                      (NBIT3)
#define SHA256_CTRL_INIT_SHA256_STATE_SHIFT                     (3)
#define SHA256_CTRL_WR_BACK_HASH_VALUE_MASK                     (NBIT4)
#define SHA256_CTRL_WR_BACK_HASH_VALUE_SHIFT                    (4)
#define SHA256_CTRL_FORCE_SHA256_QUIT_MASK                      (NBIT5)
#define SHA256_CTRL_FORCE_SHA256_QUIT_SHIFT                     (5)

#define SHA256_REGS_SHA256_CTRL_AHB_BYTE_REV_EN                 (NBIT6)
#define SHA256_REGS_SHA256_CTRL_RESERVED                        (NBIT7)
#define SHA256_REGS_SHA256_CTRL_CORE_TO_AHB_CLK_RATIO           (NBIT8+ NBIT9+ NBIT10)
#define SHA256_REGS_SHA256_CTRL_CORE_TO_AHB_CLK_RATIO_MASK      (NBIT2+ NBIT1+ NBIT0)
#define SHA256_REGS_SHA256_CTRL_CORE_TO_AHB_CLK_RATIO_SHIFT     (8)
#define SHA256_REGS_SHA256_CTRL_RESERVED_11                     (NBIT11)
#define SHA256_REGS_SHA256_CTRL_SHA1_CALC                       (NBIT12)
#define SHA256_REGS_SHA256_CTRL_PBKDF2_SHA1_CALC                (NBIT13)

#define SHA256_START_RD_ADDR                                    (SHA256_MEM_BASE+0x04UL)
#define SHA256_DATA_LENGTH                                      (SHA256_MEM_BASE+0x08UL)
#define SHA256_START_WR_ADDR                                    (SHA256_MEM_BASE+0x0cUL)
#define SHA256_COND_CHK_CTRL                                    (SHA256_MEM_BASE+0x10)
#define SHA256_COND_CHK_CTRL_HASH_VAL_COND_CHK_MASK             (NBIT1 | NBIT0)
#define SHA256_COND_CHK_CTRL_HASH_VAL_COND_CHK_SHIFT            (0)
#define SHA256_COND_CHK_CTRL_STEP_VAL_MASK                      (NBIT6 | NBIT5 | NBIT4 | NBIT3 | NBIT2)
#define SHA256_COND_CHK_CTRL_STEP_VAL_SHIFT                     (2)
#define SHA256_COND_CHK_CTRL_COND_CHK_RESULT_MASK               (NBIT7)
#define SHA256_COND_CHK_CTRL_COND_CHK_RESULT_SHIFT              (7)

#define SHA256_MOD_DATA_RANGE                                   (SHA256_MEM_BASE+0x14)
#define SHA256_MOD_DATA_RANGE_ST_BYTE_2_ADD_STP_MASK            (NBIT24-1)
#define SHA256_MOD_DATA_RANGE_ST_BYTE_2_ADD_STP_SHIFT           (0)
#define SHA256_MOD_DATA_RANGE_MOD_DATA_LEN_MASK                 (NBIT24 | NBIT25| NBIT26)
#define SHA256_MOD_DATA_RANGE_MOD_DATA_LEN_SHIFT                (24)

#define SHA256_COND_CHK_STS_1                                   (SHA256_MEM_BASE+0x18)
#define SHA256_COND_CHK_STS_2                                   (SHA256_MEM_BASE+0x1c)
#define SHA256_DONE_INTR_ENABLE                                 (SHA256_MEM_BASE+0x20)
#define SHA256_DONE_INTR_STS                                    (SHA256_MEM_BASE+0x24)
#define SHA256_TARGET_HASH_H1                                   (SHA256_MEM_BASE+0x28)
#define SHA256_TARGET_HASH_H2                                   (SHA256_MEM_BASE+0x2c)
#define SHA256_TARGET_HASH_H3                                   (SHA256_MEM_BASE+0x30)
#define SHA256_TARGET_HASH_H4                                   (SHA256_MEM_BASE+0x34)
#define SHA256_TARGET_HASH_H5                                   (SHA256_MEM_BASE+0x38)
#define SHA256_TARGET_HASH_H6                                   (SHA256_MEM_BASE+0x3c)
#define SHA256_TARGET_HASH_H7                                   (SHA256_MEM_BASE+0x40)
#define SHA256_TARGET_HASH_H8                                   (SHA256_MEM_BASE+0x44)

/*======*======*======*======*======*=======*
* WINC BIGINT HW Engine Register Definition *
*======*======*======*======*======*========*/

#define BIGINT_ENGINE_ADDR                          (0x180080ul)
#define BIGINT_VERSION                              (BIGINT_ENGINE_ADDR + 0x00)

#define BIGINT_MISC_CTRL                            (BIGINT_ENGINE_ADDR + 0x04)
#define BIGINT_MISC_CTRL_CTL_START                  (NBIT0)
#define BIGINT_MISC_CTRL_CTL_RESET                  (NBIT1)
#define BIGINT_MISC_CTRL_CTL_MSW_FIRST              (NBIT2)
#define BIGINT_MISC_CTRL_CTL_SWAP_BYTE_ORDER        (NBIT3)
#define BIGINT_MISC_CTRL_CTL_FORCE_BARRETT          (NBIT4)
#define BIGINT_MISC_CTRL_CTL_M_PRIME_VALID          (NBIT5)

#define BIGINT_M_PRIME                              (BIGINT_ENGINE_ADDR + 0x08)

#define BIGINT_STATUS                               (BIGINT_ENGINE_ADDR + 0x0C)
#define BIGINT_STATUS_STS_DONE                      (NBIT0)

#define BIGINT_CLK_COUNT                            (BIGINT_ENGINE_ADDR + 0x10)
#define BIGINT_ADDR_X                               (BIGINT_ENGINE_ADDR + 0x14)
#define BIGINT_ADDR_E                               (BIGINT_ENGINE_ADDR + 0x18)
#define BIGINT_ADDR_M                               (BIGINT_ENGINE_ADDR + 0x1C)
#define BIGINT_ADDR_R                               (BIGINT_ENGINE_ADDR + 0x20)
#define BIGINT_LENGTH                               (BIGINT_ENGINE_ADDR + 0x24)

#define BIGINT_IRQ_STS                              (BIGINT_ENGINE_ADDR + 0x28)
#define BIGINT_IRQ_STS_DONE                         (NBIT0)
#define BIGINT_IRQ_STS_CHOOSE_MONT                  (NBIT1)
#define BIGINT_IRQ_STS_M_READ                       (NBIT2)
#define BIGINT_IRQ_STS_X_READ                       (NBIT3)
#define BIGINT_IRQ_STS_START                        (NBIT4)
#define BIGINT_IRQ_STS_PRECOMP_FINISH               (NBIT5)

#define BIGINT_IRQ_MASK                             (BIGINT_ENGINE_ADDR + 0x2C)
#define BIGINT_IRQ_MASK_CTL_IRQ_MASK_START          (NBIT4)

#define ENABLE_FLIPPING         1

#define GET_UINT32(BUF,OFFSET)          ((((uint32_t)(BUF)[OFFSET])) | ((((uint32_t)(BUF)[OFFSET + 1]) << 8))  | \
((((uint32_t)(BUF)[OFFSET + 2]) << 16)) | ((((uint32_t)(BUF)[OFFSET + 3]) << 24)))

#define PUTU32(VAL32,BUF,OFFSET)    \
do  \
{   \
    (BUF)[OFFSET    ] = WINC_BYTE_3((VAL32));    \
    (BUF)[OFFSET +1 ] = WINC_BYTE_2((VAL32));    \
    (BUF)[OFFSET +2 ] = WINC_BYTE_1((VAL32));    \
    (BUF)[OFFSET +3 ] = WINC_BYTE_0((VAL32));    \
} while (0)

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@struct     tstrHashContext

@brief
*/
typedef struct
{
    uint32_t    au32HashState[M2M_SHA256_DIGEST_LEN / 4];
    uint8_t     au8CurrentBlock[64];
    uint32_t    u32TotalLength;
    uint8_t     u8InitHashFlag;
} tstrSHA256HashCtxt;

/*======*======*======*======*======*=======*
*           SHA256 IMPLEMENTATION           *
*======*======*======*======*======*========*/

int8_t m2m_crypto_sha256_hash_init(tstrM2mSha256Ctxt *pstrSha256Ctxt)
{
    tstrSHA256HashCtxt  *pstrSHA256 = (tstrSHA256HashCtxt *)pstrSha256Ctxt;

    if (pstrSHA256 != NULL)
    {
        memset(pstrSha256Ctxt, 0, sizeof(tstrM2mSha256Ctxt));
        pstrSHA256->u8InitHashFlag = 1;
    }

    return 0;
}

int8_t m2m_crypto_sha256_hash_update(tstrM2mSha256Ctxt *pstrSha256Ctxt, uint8_t *pu8Data, uint16_t u16DataLength)
{
    int8_t  s8Ret = M2M_ERR_FAIL;
    tstrSHA256HashCtxt  *pstrSHA256 = (tstrSHA256HashCtxt *)pstrSha256Ctxt;

    if (pstrSHA256 != NULL)
    {
        uint32_t u32ReadAddr;
        uint32_t u32WriteAddr    = SHARED_MEM_BASE;
        uint32_t u32Addr         = u32WriteAddr;
        uint32_t u32ResidualBytes;
        uint32_t u32NBlocks;
        uint32_t u32Offset;
        uint32_t u32CurrentBlock = 0;
        uint8_t  u8IsDone        = 0;

        /* Get the remaining bytes from the previous update (if the length is not block aligned). */
        u32ResidualBytes = pstrSHA256->u32TotalLength % SHA_BLOCK_SIZE;

        /* Update the total data length. */
        pstrSHA256->u32TotalLength += u16DataLength;

        if (u32ResidualBytes != 0)
        {
            if ((u32ResidualBytes + u16DataLength) >= SHA_BLOCK_SIZE)
            {
                u32Offset = SHA_BLOCK_SIZE - u32ResidualBytes;
                memcpy(&pstrSHA256->au8CurrentBlock[u32ResidualBytes], pu8Data, u32Offset);
                pu8Data         += u32Offset;
                u16DataLength   -= u32Offset;

                winc_bus_write_block(u32Addr, pstrSHA256->au8CurrentBlock, SHA_BLOCK_SIZE);
                u32Addr += SHA_BLOCK_SIZE;
                u32CurrentBlock  = 1;
            }
            else
            {
                memcpy(&pstrSHA256->au8CurrentBlock[u32ResidualBytes], pu8Data, u16DataLength);
                u16DataLength = 0;
            }
        }

        /* Get the number of HASH BLOCKs and the residual bytes. */
        u32NBlocks          = u16DataLength / SHA_BLOCK_SIZE;
        u32ResidualBytes    = u16DataLength % SHA_BLOCK_SIZE;

        if (u32NBlocks != 0)
        {
            winc_bus_write_block(u32Addr, pu8Data, (uint16_t)(u32NBlocks * SHA_BLOCK_SIZE));
            pu8Data += (u32NBlocks * SHA_BLOCK_SIZE);
        }

        u32NBlocks += u32CurrentBlock;

        if (u32NBlocks != 0)
        {
            uint32_t u32RegVal = 0;

            winc_bus_write_reg(SHA256_CTRL, u32RegVal);
            u32RegVal |= SHA256_CTRL_FORCE_SHA256_QUIT_MASK;
            winc_bus_write_reg(SHA256_CTRL, u32RegVal);

            if (pstrSHA256->u8InitHashFlag)
            {
                pstrSHA256->u8InitHashFlag = 0;
                u32RegVal |= SHA256_CTRL_INIT_SHA256_STATE_MASK;
            }

            u32ReadAddr = u32WriteAddr + (u32NBlocks * SHA_BLOCK_SIZE);
            winc_bus_write_reg(SHA256_DATA_LENGTH, (u32NBlocks * SHA_BLOCK_SIZE));
            winc_bus_write_reg(SHA256_START_RD_ADDR, u32WriteAddr);
            winc_bus_write_reg(SHA256_START_WR_ADDR, u32ReadAddr);

            u32RegVal |= SHA256_CTRL_START_CALC_MASK;

            u32RegVal &= ~(0x7 << 8);
            u32RegVal |= (2 << 8);

            winc_bus_write_reg(SHA256_CTRL, u32RegVal);

            /* 5.   Wait for done_intr */
            while (!u8IsDone)
            {
                u32RegVal = winc_bus_read_reg(SHA256_DONE_INTR_STS);
                u8IsDone = u32RegVal & NBIT0;
            }
        }

        if (u32ResidualBytes != 0)
        {
            memcpy(pstrSHA256->au8CurrentBlock, pu8Data, u32ResidualBytes);
        }

        s8Ret = M2M_SUCCESS;
    }

    return s8Ret;
}

int8_t m2m_crypto_sha256_hash_finish(tstrM2mSha256Ctxt *pstrSha256Ctxt, uint8_t *pu8Sha256Digest)
{
    int8_t  s8Ret = M2M_ERR_FAIL;
    tstrSHA256HashCtxt  *pstrSHA256 = (tstrSHA256HashCtxt *)pstrSha256Ctxt;

    if (pstrSHA256 != NULL)
    {
        uint32_t u32ReadAddr;
        uint32_t u32WriteAddr    = SHARED_MEM_BASE;
        uint32_t u32Addr         = u32WriteAddr;
        uint16_t u16Offset;
        uint16_t u16PaddingLength;
        uint16_t u16NBlocks      = 1;
        uint32_t u32RegVal       = 0;
        uint32_t u32Idx, u32ByteIdx;
        uint32_t au32Digest[M2M_SHA256_DIGEST_LEN / 4];
        uint8_t  u8IsDone        = 0;

        winc_bus_write_reg(SHA256_CTRL, u32RegVal);
        u32RegVal |= SHA256_CTRL_FORCE_SHA256_QUIT_MASK;
        winc_bus_write_reg(SHA256_CTRL, u32RegVal);

        if (pstrSHA256->u8InitHashFlag)
        {
            pstrSHA256->u8InitHashFlag = 0;
            u32RegVal |= SHA256_CTRL_INIT_SHA256_STATE_MASK;
        }

        /* Calculate the offset of the last data byte in the current block. */
        u16Offset = (uint16_t)(pstrSHA256->u32TotalLength % SHA_BLOCK_SIZE);

        /* Add the padding byte 0x80. */
        pstrSHA256->au8CurrentBlock[u16Offset ++] = 0x80;

        /* Calculate the required padding to complete
        one Hash Block Size.
        */
        u16PaddingLength = SHA_BLOCK_SIZE - u16Offset;
        memset(&pstrSHA256->au8CurrentBlock[u16Offset], 0, u16PaddingLength);

        /* If the padding count is not enough to hold 64-bit representation of
        the total input message length, one padding block is required.
        */
        if (u16PaddingLength < 8)
        {
            winc_bus_write_block(u32Addr, pstrSHA256->au8CurrentBlock, SHA_BLOCK_SIZE);
            u32Addr += SHA_BLOCK_SIZE;
            memset(pstrSHA256->au8CurrentBlock, 0, SHA_BLOCK_SIZE);
            u16NBlocks ++;
        }

        /* pack the length at the end of the padding block */
        PUTU32(pstrSHA256->u32TotalLength << 3, pstrSHA256->au8CurrentBlock, (SHA_BLOCK_SIZE - 4));

        u32ReadAddr = u32WriteAddr + (u16NBlocks * SHA_BLOCK_SIZE);
        winc_bus_write_block(u32Addr, pstrSHA256->au8CurrentBlock, SHA_BLOCK_SIZE);
        winc_bus_write_reg(SHA256_DATA_LENGTH, (u16NBlocks * SHA_BLOCK_SIZE));
        winc_bus_write_reg(SHA256_START_RD_ADDR, u32WriteAddr);
        winc_bus_write_reg(SHA256_START_WR_ADDR, u32ReadAddr);

        u32RegVal |= SHA256_CTRL_START_CALC_MASK;
        u32RegVal |= SHA256_CTRL_WR_BACK_HASH_VALUE_MASK;
        u32RegVal &= ~(0x7UL << 8);
        u32RegVal |= (0x2UL << 8);

        winc_bus_write_reg(SHA256_CTRL, u32RegVal);

        /* 5.   Wait for done_intr */
        while (!u8IsDone)
        {
            u32RegVal = winc_bus_read_reg(SHA256_DONE_INTR_STS);
            u8IsDone = u32RegVal & NBIT0;
        }

        winc_bus_read_block(u32ReadAddr, (uint8_t *)au32Digest, 32);

        /* Convert the output words to an array of bytes.
        */
        u32ByteIdx = 0;

        for (u32Idx = 0; u32Idx < (M2M_SHA256_DIGEST_LEN / 4); u32Idx ++)
        {
            pu8Sha256Digest[u32ByteIdx ++] = WINC_BYTE_3(au32Digest[u32Idx]);
            pu8Sha256Digest[u32ByteIdx ++] = WINC_BYTE_2(au32Digest[u32Idx]);
            pu8Sha256Digest[u32ByteIdx ++] = WINC_BYTE_1(au32Digest[u32Idx]);
            pu8Sha256Digest[u32ByteIdx ++] = WINC_BYTE_0(au32Digest[u32Idx]);
        }

        s8Ret = M2M_SUCCESS;
    }

    return s8Ret;
}

/*======*======*======*======*======*=======*
*             RSA IMPLEMENTATION            *
*======*======*======*======*======*========*/

static void FlipBuffer(uint8_t *pu8InBuffer, uint8_t *pu8OutBuffer, uint16_t u16BufferSize)
{
    uint16_t u16Idx;

    for (u16Idx = 0; u16Idx < u16BufferSize; u16Idx ++)
    {
#if ENABLE_FLIPPING == 1
        pu8OutBuffer[u16Idx] = pu8InBuffer[u16BufferSize - u16Idx - 1];
#else
        pu8OutBuffer[u16Idx] = pu8InBuffer[u16Idx];
#endif
    }
}

void BigInt_ModExp
(
    uint8_t *pu8X, uint16_t u16XSize,
    uint8_t *pu8E, uint16_t u16ESize,
    uint8_t *pu8M, uint16_t u16MSize,
    uint8_t *pu8R, uint16_t u16RSize
)
{
    uint32_t u32Reg;
    uint8_t  au8Tmp[780] = {0};
    uint32_t u32XAddr    = SHARED_MEM_BASE;
    uint32_t u32MAddr;
    uint32_t u32EAddr;
    uint32_t u32RAddr;
    uint8_t  u8EMswBits  = 32;
    uint32_t u32Mprime   = 0x7F;
    uint16_t u16XSizeWords, u16ESizeWords;
    uint32_t u32Exponent;

    u16XSizeWords = (u16XSize + 3) / 4;
    u16ESizeWords = (u16ESize + 3) / 4;

    u32MAddr    = u32XAddr + (u16XSizeWords * 4);
    u32EAddr    = u32MAddr + (u16XSizeWords * 4);
    u32RAddr    = u32EAddr + (u16ESizeWords * 4);

    /* Reset the core.
    */
    u32Reg  = 0;
    u32Reg  |= BIGINT_MISC_CTRL_CTL_RESET;
    u32Reg  = winc_bus_read_reg(BIGINT_MISC_CTRL);
    u32Reg  &= ~BIGINT_MISC_CTRL_CTL_RESET;
    u32Reg = winc_bus_read_reg(BIGINT_MISC_CTRL);

    winc_bus_write_block(u32RAddr, au8Tmp, u16RSize);

    /* Write Input Operands to Chip Memory.
    */
    /*------- X -------*/
    FlipBuffer(pu8X, au8Tmp, u16XSize);
    winc_bus_write_block(u32XAddr, au8Tmp, u16XSizeWords * 4);

    /*------- E -------*/
    memset(au8Tmp, 0, sizeof(au8Tmp));
    FlipBuffer(pu8E, au8Tmp, u16ESize);
    winc_bus_write_block(u32EAddr, au8Tmp, u16ESizeWords * 4);
    u32Exponent = GET_UINT32(au8Tmp, (u16ESizeWords * 4) - 4);

    while ((u32Exponent & NBIT31) == 0)
    {
        u32Exponent <<= 1;
        u8EMswBits --;
    }

    /*------- M -------*/
    memset(au8Tmp, 0, sizeof(au8Tmp));
    FlipBuffer(pu8M, au8Tmp, u16XSize);
    winc_bus_write_block(u32MAddr, au8Tmp, u16XSizeWords * 4);

    /* Program the addresses of the input operands.
    */
    winc_bus_write_reg(BIGINT_ADDR_X, u32XAddr);
    winc_bus_write_reg(BIGINT_ADDR_E, u32EAddr);
    winc_bus_write_reg(BIGINT_ADDR_M, u32MAddr);
    winc_bus_write_reg(BIGINT_ADDR_R, u32RAddr);

    /* Mprime.
    */
    winc_bus_write_reg(BIGINT_M_PRIME, u32Mprime);

    /* Length.
    */
    u32Reg  = (u16XSizeWords & 0xFF);
    u32Reg += ((u16ESizeWords & 0xFF) << 8);
    u32Reg += ((uint32_t)u8EMswBits << 16);
    winc_bus_write_reg(BIGINT_LENGTH, u32Reg);

    /* CTRL Register.
    */
    u32Reg = winc_bus_read_reg(BIGINT_MISC_CTRL);
    u32Reg ^= BIGINT_MISC_CTRL_CTL_START;
    u32Reg |= BIGINT_MISC_CTRL_CTL_FORCE_BARRETT;
    //u32Reg |= BIGINT_MISC_CTRL_CTL_M_PRIME_VALID;
#if ENABLE_FLIPPING == 0
    u32Reg |= BIGINT_MISC_CTRL_CTL_MSW_FIRST;
#endif
    winc_bus_write_reg(BIGINT_MISC_CTRL, u32Reg);

    /* Wait for computation to complete. */
    while (1)
    {
        u32Reg = winc_bus_read_reg(BIGINT_IRQ_STS);

        if (u32Reg & BIGINT_IRQ_STS_DONE)
        {
            break;
        }
    }

    winc_bus_write_reg(BIGINT_IRQ_STS, 0);
    memset(au8Tmp, 0, sizeof(au8Tmp));
    winc_bus_read_block(u32RAddr, au8Tmp, u16RSize);
    FlipBuffer(au8Tmp, pu8R, u16RSize);
}

#define MD5_DIGEST_SIZE         (16)
#define SHA1_DIGEST_SIZE        (20)

static const uint8_t au8TEncodingMD5[] =
{
    0x30, 0x20, 0x30, 0x0C, 0x06, 0x08, 0x2A, 0x86,
    0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05, 0x05, 0x00,
    0x04
};

/*!< Fixed part of the Encoding T for the MD5 hash algorithm. */

static const uint8_t au8TEncodingSHA1[] =
{
    0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E,
    0x03, 0x02, 0x1A, 0x05, 0x00, 0x04
};

/*!< Fixed part of the Encoding T for the SHA-1 hash algorithm. */

static const uint8_t au8TEncodingSHA2[] =
{
    0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05,
    0x00, 0x04
};

/*!< Fixed part of the Encoding T for the SHA-2 hash algorithm. */

int8_t m2m_crypto_rsa_sign_verify(uint8_t *pu8N, uint16_t u16NSize, uint8_t *pu8E, uint16_t u16ESize, uint8_t *pu8SignedMsgHash,
                                  uint16_t u16HashLength, uint8_t *pu8RsaSignature)
{
    int8_t s8Ret = M2M_RSA_SIGN_FAIL;

    if ((pu8N != NULL) && (pu8E != NULL) && (pu8RsaSignature != NULL) && (pu8SignedMsgHash != NULL))
    {
        uint16_t u16TLength, u16TEncodingLength;
        uint8_t  *pu8T;
        uint8_t  au8EM[512];

        /* Selection of correct T Encoding based on the hash size.
        */
        if (u16HashLength == MD5_DIGEST_SIZE)
        {
            pu8T                = (uint8_t *)au8TEncodingMD5;
            u16TEncodingLength  = sizeof(au8TEncodingMD5);
        }
        else if (u16HashLength == SHA1_DIGEST_SIZE)
        {
            pu8T                = (uint8_t *)au8TEncodingSHA1;
            u16TEncodingLength  = sizeof(au8TEncodingSHA1);
        }
        else
        {
            pu8T                = (uint8_t *)au8TEncodingSHA2;
            u16TEncodingLength  = sizeof(au8TEncodingSHA2);
        }

        u16TLength = u16TEncodingLength + 1 + u16HashLength;

        /* If emLen < tLen + 11.
        */
        if (u16NSize >= (u16TLength + 11))
        {
            uint32_t u32PSLength, u32Idx = 0;

            /*
            RSA verification
            */
            BigInt_ModExp(pu8RsaSignature, u16NSize, pu8E, u16ESize, pu8N, u16NSize, au8EM, u16NSize);

            u32PSLength = u16NSize - u16TLength - 3;

            /*
            The calculated EM must match the following pattern.
            *======*======*======*======*======*
            * 0x00 || 0x01 || PS || 0x00 || T  *
            *======*======*======*======*======*
            Where PS is all 0xFF
            T is defined based on the hash algorithm.
            */
            if ((au8EM[0] == 0x00) && (au8EM[1] == 0x01))
            {
                for (u32Idx = 2; au8EM[u32Idx] == 0xFF; u32Idx ++);

                if (u32Idx == (u32PSLength + 2))
                {
                    if (au8EM[u32Idx ++] == 0x00)
                    {
                        if (!memcmp(&au8EM[u32Idx], pu8T, u16TEncodingLength))
                        {
                            u32Idx += u16TEncodingLength;

                            if (au8EM[u32Idx ++] == u16HashLength)
                            {
                                s8Ret = memcmp(&au8EM[u32Idx], pu8SignedMsgHash, u16HashLength);
                            }
                        }
                    }
                }
            }
        }
    }

    return s8Ret;
}

int8_t m2m_crypto_rsa_sign_gen(uint8_t *pu8N, uint16_t u16NSize, uint8_t *pu8d, uint16_t u16dSize, uint8_t *pu8SignedMsgHash,
                               uint16_t u16HashLength, uint8_t *pu8RsaSignature)
{
    int8_t s8Ret = M2M_RSA_SIGN_FAIL;

    if ((pu8N != NULL) && (pu8d != NULL) && (pu8RsaSignature != NULL) && (pu8SignedMsgHash != NULL))
    {
        uint16_t u16TLength, u16TEncodingLength;
        uint8_t  *pu8T;
        uint8_t  au8EM[512];

        /* Selection of correct T Encoding based on the hash size.
        */
        if (u16HashLength == MD5_DIGEST_SIZE)
        {
            pu8T                = (uint8_t *)au8TEncodingMD5;
            u16TEncodingLength  = sizeof(au8TEncodingMD5);
        }
        else if (u16HashLength == SHA1_DIGEST_SIZE)
        {
            pu8T                = (uint8_t *)au8TEncodingSHA1;
            u16TEncodingLength  = sizeof(au8TEncodingSHA1);
        }
        else
        {
            pu8T                = (uint8_t *)au8TEncodingSHA2;
            u16TEncodingLength  = sizeof(au8TEncodingSHA2);
        }

        u16TLength = u16TEncodingLength + 1 + u16HashLength;

        /* If emLen < tLen + 11.
        */
        if (u16NSize >= (u16TLength + 11))
        {
            uint16_t u16PSLength = 0;
            uint16_t u16Offset   = 0;

            /*
            The calculated EM must match the following pattern.
            *======*======*======*======*======*
            * 0x00 || 0x01 || PS || 0x00 || T  *
            *======*======*======*======*======*
            Where PS is all 0xFF
            T is defined based on the hash algorithm.
            */
            au8EM[u16Offset ++] = 0;
            au8EM[u16Offset ++] = 1;
            u16PSLength = u16NSize - u16TLength - 3;
            memset(&au8EM[u16Offset], 0xFF, u16PSLength);
            u16Offset += u16PSLength;
            au8EM[u16Offset ++] = 0;
            memcpy(&au8EM[u16Offset], pu8T, u16TEncodingLength);
            u16Offset += u16TEncodingLength;
            au8EM[u16Offset ++] = u16HashLength;
            memcpy(&au8EM[u16Offset], pu8SignedMsgHash, u16HashLength);

            /*
            RSA Signature Generation
            */
            BigInt_ModExp(au8EM, u16NSize, pu8d, u16dSize, pu8N, u16NSize, pu8RsaSignature, u16NSize);
            s8Ret = M2M_RSA_SIGN_OK;
        }
    }

    return s8Ret;
}

#endif /* CONF_CRYPTO */

#ifdef CONF_CRYPTO_SOFT

typedef struct
{
    tpfAppCryproCb pfAppCryptoCb;
    uint8_t *pu8Digest;
    uint8_t *pu8Rsa;
    uint8_t u8CryptoBusy;
} tstrCryptoCtxt;

typedef struct
{
    uint8_t au8N[M2M_MAX_RSA_LEN];
    uint8_t au8E[M2M_MAX_RSA_LEN];
    uint8_t au8Hash[M2M_SHA256_DIGEST_LEN];
    uint16_t u16Nsz;
    uint16_t u16Esz;
    uint16_t u16Hsz;
    uint8_t _pad16_[2];
} tstrRsaPayload;

static tstrCryptoCtxt gstrCryptoCtxt;

void m2m_crypto_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
{
    UNUSED_VAR(u16DataSize);

    gstrCryptoCtxt.u8CryptoBusy = 0;

    if (u8OpCode == M2M_CRYPTO_RESP_SHA256_INIT)
    {
        tstrM2mSha256Ctxt strCtxt;

        if (winc_hif_receive(u32Addr, &strCtxt, sizeof(tstrM2mSha256Ctxt)) == M2M_SUCCESS)
        {
            tstrCyptoResp strResp;

            if (winc_hif_receive(u32Addr + sizeof(tstrM2mSha256Ctxt), &strResp, sizeof(tstrCyptoResp)) == M2M_SUCCESS)
            {
                if (gstrCryptoCtxt.pfAppCryptoCb)
                {
                    gstrCryptoCtxt.pfAppCryptoCb(u8OpCode, &strResp, &strCtxt);
                }
            }
        }
    }
    else if (u8OpCode == M2M_CRYPTO_RESP_SHA256_UPDATE)
    {
        tstrM2mSha256Ctxt strCtxt;

        if (winc_hif_receive(u32Addr, &strCtxt, sizeof(tstrM2mSha256Ctxt)) == M2M_SUCCESS)
        {
            tstrCyptoResp strResp;

            if (winc_hif_receive(u32Addr + sizeof(tstrM2mSha256Ctxt), &strResp, sizeof(tstrCyptoResp)) == M2M_SUCCESS)
            {
                if (gstrCryptoCtxt.pfAppCryptoCb)
                {
                    gstrCryptoCtxt.pfAppCryptoCb(u8OpCode, &strResp, &strCtxt);
                }
            }
        }

    }
    else if (u8OpCode == M2M_CRYPTO_RESP_SHA256_FINISH)
    {
        tstrCyptoResp strResp;

        if (winc_hif_receive(u32Addr + sizeof(tstrM2mSha256Ctxt), &strResp, sizeof(tstrCyptoResp)) == M2M_SUCCESS)
        {
            if (winc_hif_receive(u32Addr + sizeof(tstrM2mSha256Ctxt) + sizeof(tstrCyptoResp), gstrCryptoCtxt.pu8Digest, M2M_SHA256_DIGEST_LEN) == M2M_SUCCESS)
            {
                if (gstrCryptoCtxt.pfAppCryptoCb)
                {
                    gstrCryptoCtxt.pfAppCryptoCb(u8OpCode, &strResp, gstrCryptoCtxt.pu8Digest);
                }

            }
        }
    }
    else if (u8OpCode == M2M_CRYPTO_RESP_RSA_SIGN_GEN)
    {
        tstrCyptoResp strResp;

        if (winc_hif_receive(u32Addr + sizeof(tstrRsaPayload), &strResp, sizeof(tstrCyptoResp)) == M2M_SUCCESS)
        {
            if (winc_hif_receive(u32Addr + sizeof(tstrRsaPayload) + sizeof(tstrCyptoResp), gstrCryptoCtxt.pu8Rsa, M2M_MAX_RSA_LEN) == M2M_SUCCESS)
            {
                if (gstrCryptoCtxt.pfAppCryptoCb)
                {
                    gstrCryptoCtxt.pfAppCryptoCb(u8OpCode, &strResp, gstrCryptoCtxt.pu8Rsa);
                }
            }
        }
    }
    else if (u8OpCode == M2M_CRYPTO_RESP_RSA_SIGN_VERIFY)
    {
        tstrCyptoResp strResp;

        if (winc_hif_receive(u32Addr + sizeof(tstrRsaPayload), &strResp, sizeof(tstrCyptoResp)) == M2M_SUCCESS)
        {
            if (gstrCryptoCtxt.pfAppCryptoCb)
            {
                gstrCryptoCtxt.pfAppCryptoCb(u8OpCode, &strResp, NULL);
            }
        }
    }
    else
    {
        WINC_LOG_ERROR("u8Code %d ??", u8OpCode);
    }
}

int8_t m2m_crypto_init(tpfAppCryproCb pfAppCryproCb)
{
    memset(&gstrCryptoCtxt, 0, sizeof(tstrCryptoCtxt));

    if (pfAppCryproCb != NULL)
    {
        gstrCryptoCtxt.pfAppCryptoCb = pfAppCryproCb;
    }

    return M2M_SUCCESS;
}

int8_t m2m_crypto_sha256_hash_init(tstrM2mSha256Ctxt *psha256Ctxt)
{
    int8_t ret = M2M_ERR_FAIL;

    if ((psha256Ctxt != NULL) && (!gstrCryptoCtxt.u8CryptoBusy))
    {
        ret = winc_hif_send_no_data(M2M_REQ_GROUP_CRYPTO, M2M_CRYPTO_REQ_SHA256_INIT | M2M_REQ_DATA_PKT, psha256Ctxt, sizeof(tstrM2mSha256Ctxt));
    }

    return ret;
}

int8_t m2m_crypto_sha256_hash_update(tstrM2mSha256Ctxt *psha256Ctxt, uint8_t *pu8Data, uint16_t u16DataLength)
{
    int8_t ret = M2M_ERR_FAIL;

    if ((!gstrCryptoCtxt.u8CryptoBusy) && (psha256Ctxt != NULL) && (pu8Data != NULL) && (u16DataLength < M2M_SHA256_MAX_DATA))
    {
        ret = winc_hif_send(M2M_REQ_GROUP_CRYPTO, M2M_CRYPTO_REQ_SHA256_UPDATE | M2M_REQ_DATA_PKT, psha256Ctxt, sizeof(tstrM2mSha256Ctxt), pu8Data, u16DataLength, sizeof(tstrM2mSha256Ctxt) + sizeof(tstrCyptoResp));
    }

    return ret;
}

int8_t m2m_crypto_sha256_hash_finish(tstrM2mSha256Ctxt *psha256Ctxt, uint8_t *pu8Sha256Digest)
{
    int8_t ret = M2M_ERR_FAIL;

    if ((!gstrCryptoCtxt.u8CryptoBusy) && (psha256Ctxt != NULL) && (pu8Sha256Digest != NULL))
    {
        gstrCryptoCtxt.pu8Digest = pu8Sha256Digest;
        ret = winc_hif_send_no_data(M2M_REQ_GROUP_CRYPTO, M2M_CRYPTO_REQ_SHA256_FINISH | M2M_REQ_DATA_PKT, psha256Ctxt, sizeof(tstrM2mSha256Ctxt));
    }

    return ret;
}

int8_t m2m_crypto_rsa_sign_verify(uint8_t *pu8N, uint16_t u16NSize, uint8_t *pu8E, uint16_t u16ESize, uint8_t *pu8SignedMsgHash,
                                  uint16_t u16HashLength, uint8_t *pu8RsaSignature)
{
    int8_t ret = M2M_ERR_FAIL;

    if ((!gstrCryptoCtxt.u8CryptoBusy) && (pu8N != NULL) && (pu8E != NULL) && (pu8RsaSignature != NULL) && (pu8SignedMsgHash != NULL)
            && (u16NSize != 0) && (u16ESize != 0) && (u16HashLength != 0) && (pu8RsaSignature != NULL) )

    {
        tstrRsaPayload strRsa = {0};

        memcpy(strRsa.au8N, pu8N, u16NSize);
        memcpy(strRsa.au8E, pu8E, u16ESize);
        memcpy(strRsa.au8Hash, pu8SignedMsgHash, u16HashLength);

        strRsa.u16Esz = u16ESize;
        strRsa.u16Hsz = u16HashLength;
        strRsa.u16Nsz = u16NSize;

        ret = winc_hif_send_no_data(M2M_REQ_GROUP_CRYPTO, M2M_CRYPTO_REQ_RSA_SIGN_VERIFY | M2M_REQ_DATA_PKT, &strRsa, sizeof(tstrRsaPayload));

    }

    return ret;
}

int8_t m2m_crypto_rsa_sign_gen(uint8_t *pu8N, uint16_t u16NSize, uint8_t *pu8d, uint16_t u16dSize, uint8_t *pu8SignedMsgHash,
                               uint16_t u16HashLength, uint8_t *pu8RsaSignature)
{
    int8_t ret = M2M_ERR_FAIL;

    if ((!gstrCryptoCtxt.u8CryptoBusy) && (pu8N != NULL) && (pu8d != NULL) && (pu8RsaSignature != NULL) && (pu8SignedMsgHash != NULL)
            && (u16NSize != 0) && (u16dSize != 0) && (u16HashLength != 0) && (pu8RsaSignature != NULL))

    {
        tstrRsaPayload strRsa = {0};

        memcpy(strRsa.au8N, pu8N, u16NSize);
        memcpy(strRsa.au8E, pu8d, u16dSize);
        memcpy(strRsa.au8Hash, pu8SignedMsgHash, u16HashLength);

        strRsa.u16Esz = u16dSize;
        strRsa.u16Hsz = u16HashLength;
        strRsa.u16Nsz = u16NSize;

        gstrCryptoCtxt.pu8Rsa = pu8RsaSignature;
        ret = winc_hif_send_no_data(M2M_REQ_GROUP_CRYPTO, M2M_CRYPTO_REQ_RSA_SIGN_GEN | M2M_REQ_DATA_PKT, &strRsa, sizeof(tstrRsaPayload));

    }

    return ret;
}

#endif