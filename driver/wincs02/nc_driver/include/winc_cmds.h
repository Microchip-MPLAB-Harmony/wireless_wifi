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

#ifndef WINC_CMDS_H
#define WINC_CMDS_H

#include "microchip_pic32mzw2_nc_intf.h"

#ifndef fldsiz
/* Definition of structure field size macro. */
#define fldsiz(name, field)     (sizeof(((name *)0)->field))
#endif

/*****************************************************************************
  Description:
    Command request, response and AEC element structure.

  Remarks:

 *****************************************************************************/

typedef struct
{
    WINC_TYPE   type;
    uint16_t    length;
    uint8_t     *pData;
} WINC_DEV_PARAM_ELEM;

typedef struct
{
    uint16_t    i;
    int16_t     f;
} WINC_DEV_FRACT_INT_TYPE;

/* Command request handle definition. */
typedef uintptr_t WINC_CMD_REQ_HANDLE;

/* Value for invalid command request handles. */
#define WINC_CMD_REQ_INVALID_HANDLE     0U

/* Definitions of various sizes of command and response messages. */
#define WINC_CMD_RSP_HDR_SIZE       ((unsigned)offsetof(WINC_COMMAND_RESPONSE, payload))
#define WINC_CMD_RSP_DATA_SIZE      ((unsigned)fldsiz(WINC_COMMAND_RESPONSE, payload.rsp) + sizeof(WINC_RSP_ELEMENTS))
#define WINC_CMD_RSP_STATUS_SIZE    ((unsigned)fldsiz(WINC_COMMAND_RESPONSE, payload.status))
#define WINC_CMD_RSP_DATA_MIN_LEN   (WINC_CMD_RSP_HDR_SIZE + WINC_CMD_RSP_DATA_SIZE)
#define WINC_CMD_RSP_STATUS_LEN     (WINC_CMD_RSP_HDR_SIZE + WINC_CMD_RSP_STATUS_SIZE)

/*****************************************************************************
                         WINC Commands Module API
 *****************************************************************************/

#define WINC_CMDIPR_BAUD_RATE_IGNORE_VAL                 (0)
#define WINC_CMDCFG_ID_IGNORE_VAL                        (0)
#define WINC_CMDDHCPSC_IDX_IGNORE_VAL                    (-1)
#define WINC_CMDDHCPSC_ID_IGNORE_VAL                     (0)
#define WINC_CMDDNSC_ID_IGNORE_VAL                       (0)
#define WINC_CMDDNSRESOLV_TYPE_IGNORE_VAL                (0)
#define WINC_CMDFSOP_OP_IGNORE_VAL                       (0)
#define WINC_CMDFSOP_FILETYPE_IGNORE_VAL                 (0)
#define WINC_CMDFSLOAD_FILETYPE_IGNORE_VAL               (0)
#define WINC_CMDFSLOAD_TSFRPROT_IGNORE_VAL               (0)
#define WINC_CMDFSLOAD_FILELENGTH_IGNORE_VAL             (0)
#define WINC_CMDFSTSFR_TSFR_HANDLE_IGNORE_VAL            (0)
#define WINC_CMDFSTSFR_BLOCK_NUM_IGNORE_VAL              (0)
#define WINC_CMDFSTSFR_CRC_IGNORE_VAL                    (-1)
#define WINC_CMDMQTTC_ID_IGNORE_VAL                      (0)
#define WINC_CMDMQTTCONN_CLEAN_IGNORE_VAL                (-1)
#define WINC_CMDMQTTSUB_MAX_QOS_IGNORE_VAL               (0)
#define WINC_CMDMQTTSUBRD_MSG_ID_IGNORE_VAL              (0)
#define WINC_CMDMQTTSUBRD_LENGTH_IGNORE_VAL              (0)
#define WINC_CMDMQTTPUB_DUP_IGNORE_VAL                   (0)
#define WINC_CMDMQTTPUB_QOS_IGNORE_VAL                   (0)
#define WINC_CMDMQTTPUB_RETAIN_IGNORE_VAL                (0)
#define WINC_CMDMQTTLWT_QOS_IGNORE_VAL                   (0)
#define WINC_CMDMQTTLWT_RETAIN_IGNORE_VAL                (0)
#define WINC_CMDMQTTDISCONN_REASON_CODE_IGNORE_VAL       (-1)
#define WINC_CMDMQTTPROPTX_ID_IGNORE_VAL                 (0)
#define WINC_CMDMQTTPROPRX_ID_IGNORE_VAL                 (0)
#define WINC_CMDMQTTPROPTXS_PROP_ID_IGNORE_VAL           (-1)
#define WINC_CMDMQTTPROPTXS_PROP_SEL_IGNORE_VAL          (-1)
#define WINC_CMDNETIFC_INTF_IGNORE_VAL                   (-1)
#define WINC_CMDNETIFC_ID_IGNORE_VAL                     (0)
#define WINC_CMDNETIFTX_INTF_IGNORE_VAL                  (0)
#define WINC_CMDOTAC_ID_IGNORE_VAL                       (0)
#define WINC_CMDOTADL_STATE_IGNORE_VAL                   (0)
#define WINC_CMDPING_PROTOCOL_VERSION_IGNORE_VAL         (0)
#define WINC_CMDSNTPC_ID_IGNORE_VAL                      (0)
#define WINC_CMDSOCKO_PROTOCOL_IGNORE_VAL                (0)
#define WINC_CMDSOCKO_PROTOCOL_VERSION_IGNORE_VAL        (0)
#define WINC_CMDSOCKBL_SOCK_ID_IGNORE_VAL                (0)
#define WINC_CMDSOCKBL_LCL_PORT_IGNORE_VAL               (0)
#define WINC_CMDSOCKBL_PEND_SKTS_IGNORE_VAL              (0)
#define WINC_CMDSOCKBR_SOCK_ID_IGNORE_VAL                (0)
#define WINC_CMDSOCKBR_RMT_PORT_IGNORE_VAL               (0)
#define WINC_CMDSOCKBM_SOCK_ID_IGNORE_VAL                (0)
#define WINC_CMDSOCKBM_MCAST_PORT_IGNORE_VAL             (0)
#define WINC_CMDSOCKTLS_SOCK_ID_IGNORE_VAL               (0)
#define WINC_CMDSOCKTLS_TLS_CONF_IGNORE_VAL              (0)
#define WINC_CMDSOCKWR_SOCK_ID_IGNORE_VAL                (0)
#define WINC_CMDSOCKWR_LENGTH_IGNORE_VAL                 (0)
#define WINC_CMDSOCKWR_SEQ_NUM_IGNORE_VAL                (-1)
#define WINC_CMDSOCKWRTO_SOCK_ID_IGNORE_VAL              (0)
#define WINC_CMDSOCKWRTO_RMT_PORT_IGNORE_VAL             (0)
#define WINC_CMDSOCKWRTO_LENGTH_IGNORE_VAL               (0)
#define WINC_CMDSOCKWRTO_SEQ_NUM_IGNORE_VAL              (-1)
#define WINC_CMDSOCKRD_SOCK_ID_IGNORE_VAL                (0)
#define WINC_CMDSOCKRD_OUTPUT_MODE_IGNORE_VAL            (0)
#define WINC_CMDSOCKRDBUF_SOCK_ID_IGNORE_VAL             (0)
#define WINC_CMDSOCKRDBUF_OUTPUT_MODE_IGNORE_VAL         (0)
#define WINC_CMDSOCKCL_SOCK_ID_IGNORE_VAL                (0)
#define WINC_CMDSOCKLST_SOCK_ID_IGNORE_VAL               (0)
#define WINC_CMDSOCKC_SOCK_ID_IGNORE_VAL                 (0)
#define WINC_CMDSOCKC_ID_IGNORE_VAL                      (0)
#define WINC_CMDTIME_FORMAT_IGNORE_VAL                   (0)
#define WINC_CMDTIMEUTCSEC_FORMAT_IGNORE_VAL             (0)
#define WINC_CMDTIMEUTCSEC_UTC_SEC_IGNORE_VAL            (0)
#define WINC_CMDTLSC_CONF_IGNORE_VAL                     (0)
#define WINC_CMDTLSC_ID_IGNORE_VAL                       (0)
#define WINC_CMDTLSCSC_CSL_IDX_IGNORE_VAL                (0)
#define WINC_CMDTLSCSC_ID_IGNORE_VAL                     (0)
#define WINC_CMDWAPC_ID_IGNORE_VAL                       (0)
#define WINC_CMDWAP_STATE_IGNORE_VAL                     (-1)
#define WINC_CMDWSCNC_ID_IGNORE_VAL                      (0)
#define WINC_CMDWSCN_ACT_PASV_IGNORE_VAL                 (0)
#define WINC_CMDWSTAC_ID_IGNORE_VAL                      (0)
#define WINC_CMDWSTA_STATE_IGNORE_VAL                    (-1)
#define WINC_CMDASSOC_ASSOC_ID_IGNORE_VAL                (0)
#define WINC_CMDSI_FILTER_IGNORE_VAL                     (0)
#define WINC_CMDWPROVC_ID_IGNORE_VAL                     (0)
#define WINC_CMDWPROV_STATE_IGNORE_VAL                   (-1)
#define WINC_CMDDI_ID_IGNORE_VAL                         (0)
#define WINC_CMDEXTCRYPTO_OP_ID_IGNORE_VAL               (0)
#define WINC_CMDEXTCRYPTO_STATUS_IGNORE_VAL              (0)
#define WINC_CMDWIFIC_ID_IGNORE_VAL                      (0)
#define WINC_CMDNVMC_ID_IGNORE_VAL                       (0)
#define WINC_CMDNVMER_SECTOR_OFFSET_IGNORE_VAL           (0)
#define WINC_CMDNVMER_SECTORS_IGNORE_VAL                 (0)
#define WINC_CMDNVMWR_OFFSET_IGNORE_VAL                  (0)
#define WINC_CMDNVMWR_LENGTH_IGNORE_VAL                  (0)
#define WINC_CMDNVMCHK_OFFSET_IGNORE_VAL                 (0)
#define WINC_CMDNVMCHK_LENGTH_IGNORE_VAL                 (0)
#define WINC_CMDNVMRD_OFFSET_IGNORE_VAL                  (0)
#define WINC_CMDNVMRD_LENGTH_IGNORE_VAL                  (0)

#define WINC_CmdFormFractInt(intVal, fracVal)       ((((uint32_t)(intVal)) << 16) | (fracVal))

size_t WINC_CmdReadParamElem(const WINC_DEV_PARAM_ELEM *const pElem, WINC_TYPE typeVal, void *pVal, size_t lenVal);
bool   WINC_CmdReqSend(WINC_CMD_REQ_HANDLE handle, WINC_COMMAND_REQUEST *pCmdReq);

bool WINC_CmdGMI(WINC_CMD_REQ_HANDLE handle);
bool WINC_CmdGMM(WINC_CMD_REQ_HANDLE handle);
bool WINC_CmdGMR(WINC_CMD_REQ_HANDLE handle);
bool WINC_CmdIPR(WINC_CMD_REQ_HANDLE handle, uint32_t optBaudRate);
bool WINC_CmdCFG(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdCFGCP(WINC_CMD_REQ_HANDLE handle, WINC_TYPE typeCfgsrc, uintptr_t cfgsrc, size_t lenCfgsrc, WINC_TYPE typeCfgdst, uintptr_t cfgdst, size_t lenCfgdst);
bool WINC_CmdDHCPSC(WINC_CMD_REQ_HANDLE handle, int32_t optIdx, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdDNSC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdDNSRESOLV(WINC_CMD_REQ_HANDLE handle, uint8_t type, const uint8_t* pDomainName, size_t lenDomainName);
bool WINC_CmdFSOP(WINC_CMD_REQ_HANDLE handle, uint8_t op, uint8_t optFiletype, const uint8_t* pOptFilename, size_t lenOptFilename);
bool WINC_CmdFSLOAD(WINC_CMD_REQ_HANDLE handle, uint8_t filetype, uint8_t tsfrprot, const uint8_t* pOptFilename, size_t lenOptFilename, uint16_t optFilelength);
bool WINC_CmdFSTSFR(WINC_CMD_REQ_HANDLE handle, uint16_t optTsfrHandle, uint8_t optBlockNum, const uint8_t* pOptData, size_t lenOptData, int32_t optCrc);
bool WINC_CmdMQTTC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdMQTTCONN(WINC_CMD_REQ_HANDLE handle, int32_t optClean);
bool WINC_CmdMQTTSUB(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName, uint8_t maxQos);
bool WINC_CmdMQTTSUBLST(WINC_CMD_REQ_HANDLE handle);
bool WINC_CmdMQTTSUBRD(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName, uint16_t msgId, uint16_t length);
bool WINC_CmdMQTTUNSUB(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName);
bool WINC_CmdMQTTPUB(WINC_CMD_REQ_HANDLE handle, uint8_t dup, uint8_t qos, uint8_t retain, WINC_TYPE typeTopicNameId, uintptr_t topicNameId, size_t lenTopicNameId, const uint8_t* pTopicPayload, size_t lenTopicPayload);
bool WINC_CmdMQTTLWT(WINC_CMD_REQ_HANDLE handle, uint8_t qos, uint8_t retain, const uint8_t* pTopicName, size_t lenTopicName, const uint8_t* pTopicPayload, size_t lenTopicPayload);
bool WINC_CmdMQTTDISCONN(WINC_CMD_REQ_HANDLE handle, int32_t optReasonCode);
bool WINC_CmdMQTTPROPTX(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdMQTTPROPTXKV(WINC_CMD_REQ_HANDLE handle, const uint8_t* pKey, size_t lenKey, const uint8_t* pOptVals, size_t lenOptVals);
bool WINC_CmdMQTTPROPRX(WINC_CMD_REQ_HANDLE handle, uint32_t optId);
bool WINC_CmdMQTTPROPTXS(WINC_CMD_REQ_HANDLE handle, int32_t optPropId, int32_t optPropSel);
bool WINC_CmdNETIFC(WINC_CMD_REQ_HANDLE handle, int32_t optIntf, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdNETIFTX(WINC_CMD_REQ_HANDLE handle, uint8_t intf, const uint8_t* pData, size_t lenData);
bool WINC_CmdOTAC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdOTADL(WINC_CMD_REQ_HANDLE handle, uint8_t state);
bool WINC_CmdOTAVFY(WINC_CMD_REQ_HANDLE handle);
bool WINC_CmdOTAACT(WINC_CMD_REQ_HANDLE handle);
bool WINC_CmdOTAINV(WINC_CMD_REQ_HANDLE handle);
bool WINC_CmdPING(WINC_CMD_REQ_HANDLE handle, WINC_TYPE typeTargetAddr, uintptr_t targetAddr, size_t lenTargetAddr, uint8_t optProtocolVersion);
bool WINC_CmdRST(WINC_CMD_REQ_HANDLE handle);
bool WINC_CmdSNTPC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdSOCKO(WINC_CMD_REQ_HANDLE handle, uint8_t protocol, uint8_t optProtocolVersion);
bool WINC_CmdSOCKBL(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint16_t lclPort, uint8_t optPendSkts);
bool WINC_CmdSOCKBR(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeRmtAddr, uintptr_t rmtAddr, size_t lenRmtAddr, uint16_t rmtPort);
bool WINC_CmdSOCKBM(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeMcastAddr, uintptr_t mcastAddr, size_t lenMcastAddr, uint16_t mcastPort);
bool WINC_CmdSOCKTLS(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t tlsConf);
bool WINC_CmdSOCKWR(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint16_t length, int32_t optSeqNum, const uint8_t* pData, size_t lenData);
bool WINC_CmdSOCKWRTO(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeRmtAddr, uintptr_t rmtAddr, size_t lenRmtAddr, uint16_t rmtPort, uint16_t length, int32_t optSeqNum, const uint8_t* pData, size_t lenData);
bool WINC_CmdSOCKRD(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t outputMode, int32_t length);
bool WINC_CmdSOCKRDBUF(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t outputMode, int32_t length);
bool WINC_CmdSOCKCL(WINC_CMD_REQ_HANDLE handle, uint16_t sockId);
bool WINC_CmdSOCKLST(WINC_CMD_REQ_HANDLE handle, uint16_t optSockId);
bool WINC_CmdSOCKC(WINC_CMD_REQ_HANDLE handle, uint16_t optSockId, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdTIME(WINC_CMD_REQ_HANDLE handle, uint8_t optFormat);
bool WINC_CmdTIMEUTCSEC(WINC_CMD_REQ_HANDLE handle, uint8_t format, uint32_t utcSec);
bool WINC_CmdTIMERFC(WINC_CMD_REQ_HANDLE handle, const uint8_t* pDateTime, size_t lenDateTime);
bool WINC_CmdTLSC(WINC_CMD_REQ_HANDLE handle, uint8_t optConf, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdTLSCSC(WINC_CMD_REQ_HANDLE handle, uint8_t optCslIdx, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdWAPC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdWAP(WINC_CMD_REQ_HANDLE handle, int32_t optState);
bool WINC_CmdWSCNC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdWSCN(WINC_CMD_REQ_HANDLE handle, uint8_t actPasv);
bool WINC_CmdWSTAC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdWSTA(WINC_CMD_REQ_HANDLE handle, int32_t optState);
bool WINC_CmdASSOC(WINC_CMD_REQ_HANDLE handle, uint16_t optAssocId);
bool WINC_CmdSI(WINC_CMD_REQ_HANDLE handle, uint8_t optFilter);
bool WINC_CmdWPROVC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdWPROV(WINC_CMD_REQ_HANDLE handle, int32_t optState);
bool WINC_CmdDI(WINC_CMD_REQ_HANDLE handle, uint32_t optId);
bool WINC_CmdEXTCRYPTO(WINC_CMD_REQ_HANDLE handle, uint16_t opId, uint8_t status, const uint8_t* pOptSignature, size_t lenOptSignature);
bool WINC_CmdWIFIC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdNVMC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);
bool WINC_CmdNVMER(WINC_CMD_REQ_HANDLE handle, uint8_t sectorOffset, uint8_t sectors);
bool WINC_CmdNVMWR(WINC_CMD_REQ_HANDLE handle, uint32_t offset, uint16_t length, const uint8_t* pData, size_t lenData);
bool WINC_CmdNVMCHK(WINC_CMD_REQ_HANDLE handle, uint32_t offset, uint32_t length);
bool WINC_CmdNVMRD(WINC_CMD_REQ_HANDLE handle, uint32_t offset, uint16_t length);

#endif // WINC_CMDS_H
