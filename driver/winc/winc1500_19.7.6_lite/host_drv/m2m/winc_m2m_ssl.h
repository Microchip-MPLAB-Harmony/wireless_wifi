/**
 *
 * \file
 *
 * \brief WINC SSL Interface.
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

/** @defgroup SSLAPI SSL
    @brief
        Provides a description of the SSL Layer.
    @{
        @defgroup SSLCallbacks Callbacks
        @brief
            Provides detail on the available callbacks for the SSL Layer.

        @defgroup SSLEnums Enumerations and Typedefs
        @brief
            Specifies the enums and Data Structures used by the SSL APIs.

        @defgroup SSLFUNCTIONS Functions
        @brief
            Provides detail on the available APIs for the SSL Layer.

        @defgroup   TLSDefines  TLS Defines
        @brief
            The following list of macros are used to define constants used in the SSL layer.
        @{
            @defgroup  SSLSocketOptions TLS Socket Options
            @brief
                The following list of macros are used to define SSL Socket options.

            @see    setsockopt
        @}
    @}
*/

#ifndef __M2M_SSL_H__
#define __M2M_SSL_H__

/**@defgroup  LegacySSLCipherSuite Legacy names for TLS Cipher Suite IDs
 * @ingroup TLSDefines
 * The following list of macros MUST NOT be used. Instead use the new names under SSLCipherSuiteID
 * @sa m2m_ssl_set_active_ciphersuites
 * @{
 */

#define SSL_ENABLE_RSA_SHA_SUITES                           0x01
/*!<
    Enable RSA Hmac_SHA based Cipher suites. For example,
        TLS_RSA_WITH_AES_128_CBC_SHA
*/

#define SSL_ENABLE_RSA_SHA256_SUITES                        0x02
/*!<
    Enable RSA Hmac_SHA256 based Cipher suites. For example,
        TLS_RSA_WITH_AES_128_CBC_SHA256
*/

#define SSL_ENABLE_DHE_SHA_SUITES                           0x04
/*!<
    Enable DHE Hmac_SHA based Cipher suites. For example,
        TLS_DHE_RSA_WITH_AES_128_CBC_SHA
*/

#define SSL_ENABLE_DHE_SHA256_SUITES                        0x08
/*!<
    Enable DHE Hmac_SHA256 based Cipher suites. For example,
        TLS_DHE_RSA_WITH_AES_128_CBC_SHA256
*/

#define SSL_ENABLE_RSA_GCM_SUITES                           0x10
/*!<
    Enable RSA AEAD based Cipher suites. For example,
        TLS_RSA_WITH_AES_128_GCM_SHA256
*/

#define SSL_ENABLE_DHE_GCM_SUITES                           0x20
/*!<
    Enable DHE AEAD based Cipher suites. For example,
        TLS_DHE_RSA_WITH_AES_128_GCM_SHA256
*/

#define SSL_ENABLE_ALL_SUITES                               0x0000003F
/*!<
    Enable all possible supported cipher suites.
*/
/**@}*/     //LegacySSLCipherSuite

/**@defgroup  SSLCipherSuiteID TLS Cipher Suite IDs
 * @ingroup TLSDefines
 * The following list of macros defined the list of supported TLS Cipher suites.
 * Each MACRO defines a single Cipher suite.
 * @sa m2m_ssl_set_active_ciphersuites
 * @{
 */

#define SSL_CIPHER_RSA_WITH_AES_128_CBC_SHA                 NBIT0
#define SSL_CIPHER_RSA_WITH_AES_128_CBC_SHA256              NBIT1
#define SSL_CIPHER_DHE_RSA_WITH_AES_128_CBC_SHA             NBIT2
#define SSL_CIPHER_DHE_RSA_WITH_AES_128_CBC_SHA256          NBIT3
#define SSL_CIPHER_RSA_WITH_AES_128_GCM_SHA256              NBIT4
#define SSL_CIPHER_DHE_RSA_WITH_AES_128_GCM_SHA256          NBIT5
#define SSL_CIPHER_RSA_WITH_AES_256_CBC_SHA                 NBIT6
#define SSL_CIPHER_RSA_WITH_AES_256_CBC_SHA256              NBIT7
#define SSL_CIPHER_DHE_RSA_WITH_AES_256_CBC_SHA             NBIT8
#define SSL_CIPHER_DHE_RSA_WITH_AES_256_CBC_SHA256          NBIT9
#define SSL_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA           NBIT10
#define SSL_CIPHER_ECDHE_RSA_WITH_AES_256_CBC_SHA           NBIT11
#define SSL_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA256        NBIT12
#define SSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256      NBIT13
#define SSL_CIPHER_ECDHE_RSA_WITH_AES_128_GCM_SHA256        NBIT14
#define SSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256      NBIT15

#define SSL_ECC_ONLY_CIPHERS        \
(\
    SSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256  |   \
    SSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256      \
)
/*!<
    All ciphers that use ECC crypto only. This excludes ciphers that use RSA. They use ECDSA instead.
    These ciphers are turned off by default at startup.
    The application may enable them if it has an ECC math engine (like ATECC508).
*/
#define SSL_ECC_ALL_CIPHERS     \
(\
    SSL_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA       |   \
    SSL_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA256    |   \
    SSL_CIPHER_ECDHE_RSA_WITH_AES_128_GCM_SHA256    |   \
    SSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256  |   \
    SSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256      \
)
/*!<
    All supported ECC Ciphers including those ciphers that depend on RSA and ECC.
    These ciphers are turned off by default at startup.
    The application may enable them if it has an ECC math engine (like ATECC508).
*/

#define SSL_NON_ECC_CIPHERS_AES_128 \
(\
    SSL_CIPHER_RSA_WITH_AES_128_CBC_SHA             |   \
    SSL_CIPHER_RSA_WITH_AES_128_CBC_SHA256          |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_128_CBC_SHA         |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_128_CBC_SHA256      |   \
    SSL_CIPHER_RSA_WITH_AES_128_GCM_SHA256          |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_128_GCM_SHA256          \
)
/*!<
    All supported AES-128 Ciphers (ECC ciphers are not counted). This is the default active group after startup.
*/

#define SSL_ECC_CIPHERS_AES_256     \
(\
    SSL_CIPHER_ECDHE_RSA_WITH_AES_256_CBC_SHA   \
)
/*!<
    ECC AES-256 supported ciphers.
*/

#define SSL_NON_ECC_CIPHERS_AES_256 \
(\
    SSL_CIPHER_RSA_WITH_AES_256_CBC_SHA         |   \
    SSL_CIPHER_RSA_WITH_AES_256_CBC_SHA256      |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_256_CBC_SHA     |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_256_CBC_SHA256      \
)
/*!<
    AES-256 Ciphers.
    This group is disabled by default at startup because the WINC HW Accelerator
    supports only AES-128. If the application needs to force AES-256 cipher support,
    it could enable them (or any of them) explicitly by calling m2m_ssl_set_active_ciphersuites.
*/

#define SSL_CIPHER_ALL              \
(\
    SSL_CIPHER_RSA_WITH_AES_128_CBC_SHA             |   \
    SSL_CIPHER_RSA_WITH_AES_128_CBC_SHA256          |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_128_CBC_SHA         |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_128_CBC_SHA256      |   \
    SSL_CIPHER_RSA_WITH_AES_128_GCM_SHA256          |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_128_GCM_SHA256      |   \
    SSL_CIPHER_RSA_WITH_AES_256_CBC_SHA             |   \
    SSL_CIPHER_RSA_WITH_AES_256_CBC_SHA256          |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_256_CBC_SHA         |   \
    SSL_CIPHER_DHE_RSA_WITH_AES_256_CBC_SHA256      |   \
    SSL_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA       |   \
    SSL_CIPHER_ECDHE_RSA_WITH_AES_128_CBC_SHA256    |   \
    SSL_CIPHER_ECDHE_RSA_WITH_AES_128_GCM_SHA256    |   \
    SSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256  |   \
    SSL_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256  |   \
    SSL_CIPHER_ECDHE_RSA_WITH_AES_256_CBC_SHA           \
)
/*!<
    Turn On All TLS Ciphers.
*/
/**@}*/     // SSLCipherSuiteID

/*!
@ingroup    SSLCallbacks
@typedef    void (*tpfAppSSLCb)(uint8_t u8MsgType, void *pvMsg);
@brief      A callback to get SSL notifications.
@param[in]  u8MsgType
            The type of the message received.
@param[in]  pvMsg
            A structure to provide notification payload.
*/
typedef void (*tpfAppSSLCb)(uint8_t u8MsgType, void *pvMsg);

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@ingroup    SSLFUNCTIONS
@fn         int8_t m2m_ssl_init(tpfAppSSLCb pfAppSSLCb);
@brief      Initializes the SSL layer.
@param[in]  pfAppSSLCb
            Application SSL callback function.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ssl_init(tpfAppSSLCb pfAppSSLCb);

/*!
@ingroup    SSLFUNCTIONS
@fn         int8_t m2m_ssl_handshake_rsp(tstrEccReqInfo *pstrECCResp, uint8_t *pu8RspDataBuff, uint16_t u16RspDataSz);
@brief      Sends ECC responses to the WINC.
@param[in]  pstrECCResp
            ECC Response struct.
@param[in]  pu8RspDataBuff
            Pointer of the response data to be sent.
@param[in]  u16RspDataSz
            Response data size.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ssl_handshake_rsp(tstrEccReqInfo *pstrECCResp, uint8_t *pu8RspDataBuff, uint16_t u16RspDataSz);

/*!
@ingroup    SSLFUNCTIONS
@fn         int8_t m2m_ssl_send_certs_to_winc(uint8_t *pu8Buffer, uint32_t u32BufferSz);
@brief      Sends certificates to the WINC.
@param[in]  pu8Buffer
            Pointer to the certificates. The buffer format must match the format of @ref tstrTlsSrvSecHdr.
@param[in]  u32BufferSz
            Size of the certificates.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ssl_send_certs_to_winc(uint8_t *pu8Buffer, uint32_t u32BufferSz);

/*!
@ingroup    SSLFUNCTIONS
@fn         int8_t m2m_ssl_retrieve_next_for_verifying(tenuEcNamedCurve *penuCurve, uint8_t *pu8Value, uint16_t *pu16ValueSz, uint8_t *pu8Sig, uint16_t *pu16SigSz, tstrECPoint *pstrKey);
@brief      Retrieve the next set of information from the WINC for ECDSA verification.
@param[out] penuCurve
            The named curve.
@param[out] pu8Value
            Value retrieved for verification. This is the digest of the message, truncated/prepended to the appropriate size.
@param[inout]
            pu16ValueSz
            in: Size of value buffer provided by caller.
            out: Size of value retrieved (provided for convenience; the value size is in fact determined by the curve).
@param[out] pu8Sig
            Signature retrieved for verification.
@param[inout]
            pu16SigSz
            in: Size of signature buffer provided by caller.
            out: Size of signature retrieved (provided for convenience; the signature size is in fact determined by the curve).
@param[out] pstrKey
            Public key retrieved for verification.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@pre        This function should only be called after the application has been notified that
            verification information is ready via @ref ECC_REQ_SIGN_VERIFY.

@warning    If this function returns @ref M2M_ERR_FAIL, then any remaining verification info from
            the WINC is lost.
*/
int8_t m2m_ssl_retrieve_next_for_verifying(tenuEcNamedCurve *penuCurve, uint8_t *pu8Value, uint16_t *pu16ValueSz, uint8_t *pu8Sig, uint16_t *pu16SigSz, tstrECPoint *pstrKey);

/*!
@ingroup    SSLFUNCTIONS
@fn         int8_t m2m_ssl_retrieve_cert(uint16_t *pu16Curve, uint8_t *pu8Value, uint8_t *pu8Sig, tstrECPoint *pstrKey);
@brief      Retrieve the next set of information from the WINC for ECDSA verification.
@param[out] pu16Curve
            The named curve, to be cast to type @ref tenuEcNamedCurve.
@param[out] pu8Value
            Value retrieved for verification. This is the digest of the message, truncated/prepended to the appropriate size.
            The size of the value is equal to the field size of the curve, hence is determined by pu16Curve.
@param[out] pu8Sig
            Signature retrieved for verification.
            The size of the signature is equal to twice the field size of the curve, hence is determined by pu16Curve.
@param[out] pstrKey
            Public key retrieved for verification.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@pre        This function should only be called after the application has been notified that
            verification information is ready via @ref ECC_REQ_SIGN_VERIFY.

@warning    If this function returns @ref M2M_ERR_FAIL, then any remaining verification info from
            the WINC is lost.

@warning    This API has been deprecated and is kept for legacy purposes only. It is recommended
            that @ref m2m_ssl_retrieve_next_for_verifying is used instead.
*/
int8_t m2m_ssl_retrieve_cert(uint16_t *pu16Curve, uint8_t *pu8Value, uint8_t *pu8Sig, tstrECPoint *pstrKey);

/*!
@ingroup    SSLFUNCTIONS
@fn         int8_t m2m_ssl_retrieve_hash(uint8_t *pu8Value, uint16_t u16ValueSz)
@brief      Retrieve the value from the WINC for ECDSA signing.
@param[out] pu8Value
            Value retrieved for signing. This is the digest of the message, truncated/prepended to the appropriate size.
@param[in]  u16ValueSz
            Size of value to be retrieved. (The application should obtain this information,
            along with the curve, from the associated @ref ECC_REQ_SIGN_GEN notification.)
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@pre        This function should only be called after the application has been notified that
            signing information is ready via @ref ECC_REQ_SIGN_GEN.

@warning    If this function returns @ref M2M_ERR_FAIL, then the value for signing is lost.
*/
int8_t m2m_ssl_retrieve_hash(uint8_t *pu8Value, uint16_t u16ValueSz);

/*!
@ingroup    SSLFUNCTIONS
@fn         void m2m_ssl_stop_retrieving(void);
@brief      Allow SSL driver to tidy up when the application chooses not to retrieve all available
            information.

@return     None.

@warning    The application must call this function if it has been notified (via
            @ref ECC_REQ_SIGN_GEN or @ref ECC_REQ_SIGN_VERIFY) that information is available for
            retrieving from the WINC, but chooses not to retrieve it all.
            The application must not call this function if it has retrieved all the available
            information, or if a retrieve function returned @ref M2M_ERR_FAIL indicating that any
            remaining information has been lost.

@see        m2m_ssl_retrieve_next_for_verifying<br>
            m2m_ssl_retrieve_cert<br>
            m2m_ssl_retrieve_hash
*/
void m2m_ssl_stop_retrieving(void);

/*!
@ingroup    SSLFUNCTIONS
@fn         void m2m_ssl_stop_processing_certs(void);
@brief      Allow SSL driver to tidy up in case application does not read all available certificates.
@return     None.

@warning    This API has been deprecated and is kept for legacy purposes only. It is recommended
            that @ref m2m_ssl_stop_retrieving is used instead.
*/
void m2m_ssl_stop_processing_certs(void);

/*!
@ingroup    SSLFUNCTIONS
@fn         void m2m_ssl_ecc_process_done(void);
@brief      Allow SSL driver to tidy up after application has finished processing ECC message.

@return     None.

@warning    The application should call this function after receiving an SSL callback with message
            type @ref M2M_SSL_REQ_ECC, after retrieving any related information, and before
            calling @ref m2m_ssl_handshake_rsp.
*/
void m2m_ssl_ecc_process_done(void);

/*!
@ingroup    SSLFUNCTIONS
@fn         int8_t m2m_ssl_set_active_ciphersuites(uint32_t u32SslCsBMP);
@brief      Sets the active cipher suites.
@details    Override the default Active SSL ciphers in the SSL module with a certain combination selected by
            the caller in the form of a bitmap containing the required ciphers to be on.<br>
            There is no need to call this function if the application will not change the default cipher suites.
@param[in]  u32SslCsBMP
            Bitmap containing the desired ciphers to be enabled for the SSL module. The cipher suites are defined in
            @ref SSLCipherSuiteID.
            The default cipher suites are all cipher suites supported by the firmware with the exception of ECC cipher suites.
            The caller can override the default with any desired combination.
            If u32SslCsBMP does not contain any cipher suites supported by firmware, then the current active list will not
            change.
@return     The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_ssl_set_active_ciphersuites(uint32_t u32SslCsBMP);

#endif /* __M2M_SSL_H__ */
