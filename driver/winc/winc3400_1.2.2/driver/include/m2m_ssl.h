/*******************************************************************************
  File Name:
    m2m_ssl.h

  Summary:
    WINC SSL Interface.

  Description:
    WINC SSL Interface.
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
//DOM-IGNORE-END

/**@defgroup SSLAPI SSL
*/

#ifndef __M2M_SSL_H__
#define __M2M_SSL_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "nm_common.h"
#include "m2m_types.h"
#include "nmdrv.h"
#include "ecc_types.h"
#include "socket.h"

/**@defgroup  SSLEnums Enumeration/Typedefs
 * @ingroup SSLAPI
 * @{*/

/*!
@typedef \
    void (*tpfAppSSLCb) (uint8_t u8MsgType, void * pvMsg);

@brief A callback to get SSL notifications.

@param[in] u8MsgType
@param[in] pvMsg A structure to provide notification payload.
*/
typedef void (*tpfAppSSLCb) (uint8_t u8MsgType, void * pvMsg);

/**@}
*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/** \defgroup SSLFUNCTIONS Functions
*  @ingroup SSLAPI
*/

/**@{*/
/*!
    @fn \   int8_t m2m_ssl_init(tpfAppSslCb pfAppSslCb);
    @brief  Initializes the SSL layer.
    @param [in]     pfAppSslCb
                        Application SSL callback function.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_init(tpfAppSSLCb pfAppSSLCb);

/*!
    @fn \    int8_t m2m_ssl_handshake_rsp(tstrEccReqInfo* strECCResp, uint8_t* pu8RspDataBuff, uint16_t u16RspDataSz)
    @brief   Sends ECC responses to the WINC
    @param [in]     strECCResp
                        ECC Response struct.
    @param [in]     pu8RspDataBuff
                        Pointer of the response data to be sent.
    @param [in]     u16RspDataSz
                        Response data size.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_handshake_rsp(tstrEccReqInfo* strECCResp, uint8_t* pu8RspDataBuff, uint16_t u16RspDataSz);

/*!
    @fn \   int8_t m2m_ssl_send_certs_to_winc(uint8_t* pu8Buffer, uint32_t u32BufferSz)
    @brief  Sends certificates to the WINC
    @param [in]     pu8Buffer
                        Pointer to the certificates. The buffer format must match the format of @ref tstrTlsSrvSecHdr
    @param [in]     u32BufferSz
                        Size of the certificates.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_send_certs_to_winc(uint8_t* pu8Buffer, uint32_t u32BufferSz);

/*!
    @fn \   int8_t m2m_ssl_retrieve_cert(uint16_t* pu16CurveType, uint8_t* pu8Hash, uint8_t* pu8Sig, tstrECPoint* pu8Key)
    @brief  Retrieve the ECDSA certificate to be verified from the WINC
    @param [out]    pu16CurveType
                        Pointer to the certificate curve type.
    @param [out]    pu8Hash
                        Pointer to the certificate hash.
    @param [out]    pu8Sig
                        Pointer to the certificate signature.
    @param [out]    pu8Key
                        Pointer to the certificate Key.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_retrieve_cert(uint16_t* pu16CurveType, uint8_t* pu8Hash, uint8_t* pu8Sig, tstrECPoint* pu8Key);

/*!
    @fn \   int8_t m2m_ssl_retrieve_hash(uint8_t* pu8Hash, uint16_t u16HashSz)
    @brief  Retrieve the certificate hash
    @param [out]    pu8Hash
                        Pointer to the certificate hash.
    @param [in]     u16HashSz
                        Hash size.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_retrieve_hash(uint8_t* pu8Hash, uint16_t u16HashSz);

/*!
    @fn \   void m2m_ssl_stop_processing_certs(void)
    @brief  Allow ssl driver to tidy up in case application does not read all available certificates.
    @warning    This API must only be called if some certificates are left unread.
    @return     None.
*/
void m2m_ssl_stop_processing_certs(void);

/*!
    @fn \   void m2m_ssl_ecc_process_done(void)
    @brief  Allow ssl driver to tidy up after application has finished processing ecc message.
    @warning    This API must be called after receiving a SSL callback with type @ref M2M_SSL_REQ_ECC
    @return     None.
*/
void m2m_ssl_ecc_process_done(void);

/*!
@fn \
    int8_t m2m_ssl_set_active_ciphersuites(uint32_t u32SslCsBMP);
    Override the default Active SSL ciphers in the SSL module with a certain combination selected by the caller in the form of
    a bitmap containing the required ciphers to be on.
    There is no need to call this function if the application will not change the default ciphersuites.

@param [in] u32SslCsBMP
                Bitmap containing the desired ciphers to be enabled for the SSL module. The ciphersuites are defined in
                @ref SSLCipherSuiteID.
                The default ciphersuites are all ciphersuites supported by the firmware with the exception of ECC ciphersuites.
                The caller can override the default with any desired combination, except for combinations involving both RSA
                and ECC; if any RSA ciphersuite is enabled, then firmware will disable all ECC ciphersuites.
                If u32SslCsBMP does not contain any ciphersuites supported by firmware, then the current active list will not
                be changed.

@return
    - [SOCK_ERR_NO_ERROR](@ref SOCK_ERR_NO_ERROR)
    - [SOCK_ERR_INVALID_ARG](@ref SOCK_ERR_INVALID_ARG)
*/
int8_t m2m_ssl_set_active_ciphersuites(uint32_t u32SslCsBMP);

 /**@}*/
#endif /* __M2M_SSL_H__ */