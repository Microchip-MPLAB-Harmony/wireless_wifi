/*******************************************************************************
  File Name:
    m2m_ssl.c

  Summary:

  Description:
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2019, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "m2m_ssl.h"
#include "m2m_hif.h"
#include "nmasic.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
static tpfAppSSLCb gpfAppSSLCb = NULL;
static uint32_t gu32HIFAddr = 0;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
    @fn \   m2m_ssl_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
    @brief      SSL callback function
    @param [in] u8OpCode
                HIF Opcode type.
    @param [in] u16DataSize
                HIF data length.
    @param [in] u32Addr
                HIF address.
*/
static void m2m_ssl_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
{
    int8_t s8tmp = M2M_SUCCESS;
    switch(u8OpCode)
    {
        case M2M_SSL_REQ_ECC:
        {
            tstrEccReqInfo strEccREQ;
            s8tmp = hif_receive(u32Addr, (uint8_t*)&strEccREQ, sizeof(tstrEccReqInfo), 0);
            if(s8tmp == M2M_SUCCESS)
            {
                if (gpfAppSSLCb)
                {
                    gu32HIFAddr = u32Addr + sizeof(tstrEccReqInfo);
                    gpfAppSSLCb(M2M_SSL_REQ_ECC, &strEccREQ);
                }
            }
        }
        break;
        case M2M_SSL_RESP_SET_CS_LIST:
        {
            tstrSslSetActiveCsList strCsList;
            s8tmp = hif_receive(u32Addr, (uint8_t*)&strCsList, sizeof(tstrSslSetActiveCsList), 0);
            if(s8tmp == M2M_SUCCESS)
            {
                if (gpfAppSSLCb)
                    gpfAppSSLCb(M2M_SSL_RESP_SET_CS_LIST, &strCsList);
            }
        }
        break;
    }
    if(s8tmp != M2M_SUCCESS)
    {
        M2M_ERR("Error receiving SSL from the HIF\n");
    }
}


/*!
    @fn \    m2m_ssl_handshake_rsp(tstrEccReqInfo* strECCResp, uint8_t* pu8RspDataBuff, uint16_t u16RspDataSz)
    @brief   Sends ECC responses to the WINC
    @param [in] strECCResp
                ECC Response struct.
    @param [in] pu8RspDataBuffe
                Pointer of the response data to be sent.
    @param [in] u16RspDataSz
                Response data size.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_handshake_rsp(tstrEccReqInfo* strECCResp, uint8_t* pu8RspDataBuff, uint16_t u16RspDataSz)
{
    int8_t s8Ret = M2M_SUCCESS;

    s8Ret = hif_send(M2M_REQ_GROUP_SSL, (M2M_SSL_RESP_ECC | M2M_REQ_DATA_PKT), (uint8_t*)strECCResp, sizeof(tstrEccReqInfo), pu8RspDataBuff, u16RspDataSz, sizeof(tstrEccReqInfo));

    return s8Ret;
}

/*!
    @fn \   m2m_ssl_send_certs_to_winc(uint8_t* sector_buffer, uint32_t sector_size)
    @brief  Sends certificates to the WINC
    @param [in] pu8Buffer
                Pointer to the certificates.
    @param [in] u32BufferSz
                Size of the certificates.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_send_certs_to_winc(uint8_t* pu8Buffer, uint32_t u32BufferSz)
{
    int8_t s8Ret = M2M_SUCCESS;

    s8Ret = hif_send(M2M_REQ_GROUP_SSL, (M2M_SSL_IND_CERTS_ECC | M2M_REQ_DATA_PKT), NULL, 0, pu8Buffer, u32BufferSz, 0);

    return s8Ret;
}

/*!
    @fn \   m2m_ssl_retrieve_cert(uint32_t u32ReadAddr, uint16_t* pu16CurveType, uint8_t* pu8Hash, uint8_t* pu8Sig, tstrECPoint* pu8Key)
    @brief  Retrieve the certificate to be verified from the WINC
    @param [in] pu16CurveType
                Pointer to the certificate curve type.
    @param [in] pu8Hash
                Pointer to the certificate hash.
    @param [in] pu8Sig
                Pointer to the certificate signature.
    @param [in] pu8Key
                Pointer to the certificate Key.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_retrieve_cert(uint16_t* pu16CurveType, uint8_t* pu8Hash, uint8_t* pu8Sig, tstrECPoint* pu8Key)
{
    uint8_t bSetRxDone  = 1;
    uint16_t    u16HashSz, u16SigSz, u16KeySz;
    int8_t  s8Ret = M2M_SUCCESS;

    if(gu32HIFAddr == 0) return M2M_ERR_FAIL;

    if(hif_receive(gu32HIFAddr, (uint8_t*)pu16CurveType, 2, 0) != M2M_SUCCESS) goto __ERR;
    gu32HIFAddr += 2;

    if(hif_receive(gu32HIFAddr, (uint8_t*)&u16KeySz, 2, 0) != M2M_SUCCESS) goto __ERR;
    gu32HIFAddr += 2;

    if(hif_receive(gu32HIFAddr, (uint8_t*)&u16HashSz, 2, 0) != M2M_SUCCESS) goto __ERR;
    gu32HIFAddr += 2;

    if(hif_receive(gu32HIFAddr, (uint8_t*)&u16SigSz, 2, 0) != M2M_SUCCESS) goto __ERR;
    gu32HIFAddr += 2;

    (*pu16CurveType)= _htons((*pu16CurveType));
    pu8Key->u16Size = _htons(u16KeySz);
    u16HashSz       = _htons(u16HashSz);
    u16SigSz        = _htons(u16SigSz);

    if(hif_receive(gu32HIFAddr, pu8Key->X, pu8Key->u16Size * 2, 0) != M2M_SUCCESS) goto __ERR;
    gu32HIFAddr += (pu8Key->u16Size * 2);

    if(hif_receive(gu32HIFAddr, pu8Hash, u16HashSz, 0) != M2M_SUCCESS) goto __ERR;
    gu32HIFAddr += u16HashSz;

    if(hif_receive(gu32HIFAddr, pu8Sig, u16SigSz, 0) != M2M_SUCCESS) goto __ERR;
    gu32HIFAddr += u16SigSz;

    bSetRxDone = 0;

__ERR:
    if(bSetRxDone)
    {
        s8Ret = M2M_ERR_FAIL;
        hif_receive(0, NULL, 0, 1);
    }
    return s8Ret;
}

/*!
    @fn \   m2m_ssl_retrieve_hash(uint32_t u32ReadAddr, uint8_t* pu8Hash, uint16_t u16HashSz)
    @brief  Retrieve the certificate hash
    @param [in] pu8Hash
                Pointer to the certificate hash.
    @param [in] u16HashSz
                Hash size.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_retrieve_hash(uint8_t* pu8Hash, uint16_t u16HashSz)
{
    uint8_t bSetRxDone  = 1;
    int8_t  s8Ret = M2M_SUCCESS;

    if(gu32HIFAddr == 0) return M2M_ERR_FAIL;

    if(hif_receive(gu32HIFAddr, pu8Hash, u16HashSz, 0) != M2M_SUCCESS) goto __ERR;

    bSetRxDone = 0;

__ERR:
    if(bSetRxDone)
    {
        s8Ret = M2M_ERR_FAIL;
        hif_receive(0, NULL, 0, 1);
    }
    return s8Ret;
}

/*!
    @fn \   m2m_ssl_stop_processing_certs(void)
    @brief  Stops receiving from the HIF
*/
void m2m_ssl_stop_processing_certs(void)
{
    hif_receive(0, NULL, 0, 1);
}

/*!
    @fn \   m2m_ssl_ecc_process_done(void)
    @brief  Stops receiving from the HIF
*/
void m2m_ssl_ecc_process_done(void)
{
    gu32HIFAddr = 0;
}

/*!
@fn \
    m2m_ssl_set_active_ciphersuites(uint32_t u32SslCsBMP);
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
int8_t m2m_ssl_set_active_ciphersuites(uint32_t u32SslCsBMP)
{
    int8_t s8Ret = M2M_SUCCESS;
    tstrSslSetActiveCsList  strCsList;

    strCsList.u32CsBMP = u32SslCsBMP;
    s8Ret = hif_send(M2M_REQ_GROUP_SSL, M2M_SSL_REQ_SET_CS_LIST, (uint8_t*)&strCsList, sizeof(tstrSslSetActiveCsList), NULL, 0, 0);

    return s8Ret;
}

/*!
    @fn \   m2m_ssl_init(tpfAppSslCb pfAppSslCb);
    @brief  Initializes the SSL layer.
    @param [in] pfAppSslCb
    Application SSL callback function.
    @return     The function SHALL return 0 for success and a negative value otherwise.
*/
int8_t m2m_ssl_init(tpfAppSSLCb pfAppSSLCb)
{
    int8_t s8Ret = M2M_SUCCESS;

    gpfAppSSLCb = pfAppSSLCb;
    gu32HIFAddr = 0;

    s8Ret = hif_register_cb(M2M_REQ_GROUP_SSL,m2m_ssl_cb);
    if (s8Ret != M2M_SUCCESS)
    {
        M2M_ERR("hif_register_cb() failed with ret=%d", s8Ret);
    }
    return s8Ret;
}

//DOM-IGNORE-END
