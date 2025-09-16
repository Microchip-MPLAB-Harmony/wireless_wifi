/**
 *
 * \file
 *
 * \brief WINC Crypto Application Interface.
 */
/*
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef __M2M_CRYPTO_H__
#define __M2M_CRYPTO_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define M2M_MAX_RSA_LEN                 (256)
#define M2M_SHA256_DIGEST_LEN           32
#define M2M_SHA256_MAX_DATA             (M2M_BUFFER_MAX_SIZE - M2M_SHA256_CONTEXT_BUFF_LEN - M2M_HIF_HDR_OFFSET)
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@struct     tstrM2mSha256Ctxt

@brief
            SHA256 context data
*/
typedef struct sha256ctxt
{
    uint32_t au32Sha256CtxtBuff[M2M_SHA256_CONTEXT_BUFF_LEN / sizeof(uint32_t)];
} tstrM2mSha256Ctxt;

/*!
@enum       tenuRsaSignStatus

@brief
            RSA Signature status: pass or fail.

@see
            m2m_crypto_rsa_sign_gen
*/
typedef enum
{
    M2M_RSA_SIGN_OK,
    M2M_RSA_SIGN_FAIL
} tenuRsaSignStatus;

/*!
@typedef    tpfAppCryproCb

@brief      Crypto Callback function receiving the crypto related messages
@param[in]  u8MsgType
            Crypto command about which the notification is received.
@param[in]  pvResp
            A pointer to the result associated with the notification.
@param[in]  pvMsg
            A pointer to a buffer containing the notification parameters (if any). It should be
            Casted to the correct data type corresponding to the notification type.
@see
            m2m_crypto_init
            tenuM2mCryptoCmd
*/
typedef void (*tpfAppCryproCb) (uint8_t u8MsgType, void *pvResp, void *pvMsg);
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
extern "C" {
#endif
/*!
@fn         int8_t m2m_crypto_init();

@brief      crypto initialization.

@param[in]  pfAppCryproCb
            Pointer to the Crypto Callback function receiving the crypto related messages.
@see
            tpfAppCryproCb

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.
*/
int8_t m2m_crypto_init(tpfAppCryproCb pfAppCryproCb);
/*!
@fn         int8_t m2m_crypto_sha256_hash_init(tstrM2mSha256Ctxt *psha256Ctxt);

@brief      SHA256 hash initialization

@param[in]  psha256Ctxt
            Pointer to a SHA256 context allocated by the caller.
@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.
*/
int8_t m2m_crypto_sha256_hash_init(tstrM2mSha256Ctxt *psha256Ctxt);

/*!
@fn         int8_t m2m_crypto_sha256_hash_update(tstrM2mSha256Ctxt *psha256Ctxt, uint8_t *pu8Data, uint16_t u16DataLength);

@brief      SHA256 hash update

@param[in]  psha256Ctxt
            Pointer to the SHA256 context.

@param[in]  pu8Data
            Buffer holding the data submitted to the hash.

@param[in]  u16DataLength
            Size of the data buffer in bytes.
@pre        SHA256 module should be initialized first through m2m_crypto_sha256_hash_init function.

@see        m2m_crypto_sha256_hash_init

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.

*/
int8_t m2m_crypto_sha256_hash_update(tstrM2mSha256Ctxt *psha256Ctxt, uint8_t *pu8Data, uint16_t u16DataLength);

/*!
@fn         int8_t m2m_crypto_sha256_hash_finish(tstrM2mSha256Ctxt *psha256Ctxt, uint8_t *pu8Sha256Digest);

@brief      SHA256 hash finalization

@param[in]  psha256Ctxt
            Pointer to a SHA256 context allocated by the caller.

@param[in]  pu8Sha256Digest
            Buffer allocated by the caller which will hold the resultant SHA256 Digest. It must be allocated no less than M2M_SHA256_DIGEST_LEN.

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.
*/
int8_t m2m_crypto_sha256_hash_finish(tstrM2mSha256Ctxt *psha256Ctxt, uint8_t *pu8Sha256Digest);

/*!
@fn         int8_t m2m_crypto_rsa_sign_verify(uint8_t *pu8N, uint16_t u16NSize, uint8_t *pu8E, uint16_t u16ESize, uint8_t *pu8SignedMsgHash, \
            uint16_t u16HashLength, uint8_t *pu8RsaSignature);

@brief      RSA Signature Verification

            The function shall request the RSA Signature verification from the WINC Firmware for the given message. The signed message shall be
            compressed to the corresponding hash algorithm before calling this function.
            The hash type is identified by the given hash length. For example, if the hash length is 32 bytes, then it is SHA256.

@param[in]  pu8N
            RSA Key modulus n.

@param[in]  u16NSize
            Size of the RSA modulus n in bytes.

@param[in]  pu8E
            RSA public exponent.

@param[in]  u16ESize
            Size of the RSA public exponent in bytes.

@param[in]  pu8SignedMsgHash
            The hash digest of the signed message.

@param[in]  u16HashLength
            The length of the hash digest.

@param[out] pu8RsaSignature
            Signature value to be verified.

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.
*/
int8_t m2m_crypto_rsa_sign_verify(uint8_t *pu8N, uint16_t u16NSize, uint8_t *pu8E, uint16_t u16ESize, uint8_t *pu8SignedMsgHash,
                                  uint16_t u16HashLength, uint8_t *pu8RsaSignature);

/*!
@fn         int8_t m2m_crypto_rsa_sign_gen(uint8_t *pu8N, uint16_t u16NSize, uint8_t *pu8d, uint16_t u16dSize, uint8_t *pu8SignedMsgHash, \
            uint16_t u16HashLength, uint8_t *pu8RsaSignature);

@brief      RSA Signature Generation

            The function shall request the RSA Signature generation from the WINC Firmware for the given message. The signed message shall be
            compressed to the corresponding hash algorithm before calling this function.
            The hash type is identified by the given hash length. For example, if the hash length is 32 bytes, then it is SHA256.

@param[in]  pu8N
            RSA Key modulus n.

@param[in]  u16NSize
            Size of the RSA modulus n in bytes.

@param[in]  pu8d
            RSA private exponent.

@param[in]  u16dSize
            Size of the RSA private exponent in bytes.

@param[in]  pu8SignedMsgHash
            The hash digest of the signed message.

@param[in]  u16HashLength
            The length of the hash digest.

@param[out] pu8RsaSignature
            Pointer to a user buffer allocated by the caller shall hold the generated signature.

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.
*/
int8_t m2m_crypto_rsa_sign_gen(uint8_t *pu8N, uint16_t u16NSize, uint8_t *pu8d, uint16_t u16dSize, uint8_t *pu8SignedMsgHash,
                               uint16_t u16HashLength, uint8_t *pu8RsaSignature);
#ifdef __cplusplus
}
#endif

#endif /* __M2M_CRYPTO_H__ */
