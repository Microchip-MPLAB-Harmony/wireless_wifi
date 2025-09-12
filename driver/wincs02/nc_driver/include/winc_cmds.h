/*
Copyright (C) 2023-25 Microchip Technology Inc. and its subsidiaries. All rights reserved.

Subject to your compliance with these terms, you may use this Microchip software and any derivatives
exclusively with Microchip products. You are responsible for complying with third party license terms
applicable to your use of third party software (including open source software) that may accompany this
Microchip software. SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR
STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-
INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL
MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS,
DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER
CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY
TO MICROCHIP FOR THIS SOFTWARE.
*/

#ifndef WINC_CMDS_H
#define WINC_CMDS_H

#include "microchip_wincs02_intf.h"

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

#define WINC_CMDCFG_ID_IGNORE_VAL                        (0)
#define WINC_CMDDHCPSC_IDX_IGNORE_VAL                    (-1)
#define WINC_CMDDHCPSC_ID_IGNORE_VAL                     (0)
#define WINC_CMDDNSC_ID_IGNORE_VAL                       (0)
#define WINC_CMDFSOP_FILETYPE_IGNORE_VAL                 (-1)
#define WINC_CMDFSLOAD_FILELENGTH_IGNORE_VAL             (0)
#define WINC_CMDFSTSFR_TSFR_HANDLE_IGNORE_VAL            (0)
#define WINC_CMDFSTSFR_BLOCK_NUM_IGNORE_VAL              (-1)
#define WINC_CMDFSTSFR_CRC_IGNORE_VAL                    (-1)
#define WINC_CMDFSRECV_DATA_LENGTH_IGNORE_VAL            (0)
#define WINC_CMDMQTTC_ID_IGNORE_VAL                      (0)
#define WINC_CMDMQTTCONN_CLEAN_IGNORE_VAL                (-1)
#define WINC_CMDMQTTDISCONN_REASON_CODE_IGNORE_VAL       (-1)
#define WINC_CMDMQTTPROPTX_ID_IGNORE_VAL                 (0)
#define WINC_CMDMQTTPROPRX_ID_IGNORE_VAL                 (0)
#define WINC_CMDMQTTPROPTXS_PROP_ID_IGNORE_VAL           (-1)
#define WINC_CMDMQTTPROPTXS_PROP_SEL_IGNORE_VAL          (-1)
#define WINC_CMDNETIFC_INTF_IGNORE_VAL                   (-1)
#define WINC_CMDNETIFC_ID_IGNORE_VAL                     (0)
#define WINC_CMDOTAC_ID_IGNORE_VAL                       (0)
#define WINC_CMDPING_PROTOCOL_VERSION_IGNORE_VAL         (0)
#define WINC_CMDSNTPC_ID_IGNORE_VAL                      (0)
#define WINC_CMDSOCKO_PROTOCOL_VERSION_IGNORE_VAL        (0)
#define WINC_CMDSOCKBL_PEND_SKTS_IGNORE_VAL              (0)
#define WINC_CMDSOCKWR_SEQ_NUM_IGNORE_VAL                (-1)
#define WINC_CMDSOCKWRTO_SEQ_NUM_IGNORE_VAL              (-1)
#define WINC_CMDSOCKLST_SOCK_ID_IGNORE_VAL               (0)
#define WINC_CMDSOCKC_SOCK_ID_IGNORE_VAL                 (0)
#define WINC_CMDSOCKC_ID_IGNORE_VAL                      (0)
#define WINC_CMDTIME_FORMAT_IGNORE_VAL                   (0)
#define WINC_CMDTLSC_CONF_IGNORE_VAL                     (0)
#define WINC_CMDTLSC_ID_IGNORE_VAL                       (0)
#define WINC_CMDTLSCSC_CSL_IDX_IGNORE_VAL                (0)
#define WINC_CMDTLSCSC_ID_IGNORE_VAL                     (0)
#define WINC_CMDWAPC_ID_IGNORE_VAL                       (0)
#define WINC_CMDWAP_STATE_IGNORE_VAL                     (-1)
#define WINC_CMDWSCNC_ID_IGNORE_VAL                      (0)
#define WINC_CMDWSTAC_ID_IGNORE_VAL                      (0)
#define WINC_CMDWSTA_STATE_IGNORE_VAL                    (-1)
#define WINC_CMDASSOC_ASSOC_ID_IGNORE_VAL                (0)
#define WINC_CMDSI_FILTER_IGNORE_VAL                     (0)
#define WINC_CMDWPROVC_ID_IGNORE_VAL                     (0)
#define WINC_CMDWPROV_STATE_IGNORE_VAL                   (-1)
#define WINC_CMDDI_ID_IGNORE_VAL                         (0)
#define WINC_CMDWIFIC_ID_IGNORE_VAL                      (0)
#define WINC_CMDNVMC_ID_IGNORE_VAL                       (0)
#define WINC_CMDPPSC_ID_IGNORE_VAL                       (0)
#define WINC_CMDSYSLOGC_ID_IGNORE_VAL                    (0)
#define WINC_CMDARB_VALUE_IGNORE_VAL                     (0)

#define WINC_CmdFormFractInt(intVal, fracVal)       ((((uint32_t)(intVal)) << 16) | (fracVal))

size_t WINC_CmdReadParamElem(const WINC_DEV_PARAM_ELEM *const pElem, WINC_TYPE typeVal, void *pVal, size_t lenVal);
bool   WINC_CmdReqSend(WINC_CMD_REQ_HANDLE handle, WINC_COMMAND_REQUEST *pCmdReq);

/*****************************************************************************
  Description:
    This function requests manufacturer identification.

  Parameters:
    handle - Command request session handle

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdGMI(WINC_CMD_REQ_HANDLE handle);

/*****************************************************************************
  Description:
    This function requests model identification.

  Parameters:
    handle - Command request session handle

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdGMM(WINC_CMD_REQ_HANDLE handle);

/*****************************************************************************
  Description:
    This function requests revision identification.

  Parameters:
    handle - Command request session handle

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdGMR(WINC_CMD_REQ_HANDLE handle);

/*****************************************************************************
  Description:
    This function is used to read or set the system configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDCFG_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdCFG(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to copy configurations to/from storage.

  Parameters:
    handle     - Command request session handle
    typeCfgsrc - Type of cfgsrc
    cfgsrc     - Configuration source
    lenCfgsrc  - Length of cfgsrc
    typeCfgdst - Type of cfgdst
    cfgdst     - Configuration destination
    lenCfgdst  - Length of cfgdst

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdCFGCP(WINC_CMD_REQ_HANDLE handle, WINC_TYPE typeCfgsrc, uintptr_t cfgsrc, size_t lenCfgsrc, WINC_TYPE typeCfgdst, uintptr_t cfgdst, size_t lenCfgdst);

/*****************************************************************************
  Description:
    This function is used to read or set the DHCP server configuration.

  Parameters:
    handle     - Command request session handle
    optIdx     - Pool index
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optIdx will be ignored if its value is WINC_CMDDHCPSC_IDX_IGNORE_VAL.

    optId will be ignored if its value is WINC_CMDDHCPSC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdDHCPSC(WINC_CMD_REQ_HANDLE handle, int32_t optIdx, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to read or set the DNS configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDDNSC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdDNSC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to resolve domain names via DNS.

  Parameters:
    handle        - Command request session handle
    type          - Type of record
    pDomainName   - Domain name to resolve
    lenDomainName - Length of pDomainName

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdDNSRESOLV(WINC_CMD_REQ_HANDLE handle, uint8_t type, const uint8_t* pDomainName, size_t lenDomainName);

/*****************************************************************************
  Description:
    This function performs a filesystem operation.

  Parameters:
    handle         - Command request session handle
    op             - Operation
    optFiletype    - File type
    pOptFilename   - The name of the file
    lenOptFilename - Length of pOptFilename

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optFiletype will be ignored if its value is WINC_CMDFSOP_FILETYPE_IGNORE_VAL.

    pOptFilename will be ignored if its value is NULL.

 *****************************************************************************/

bool WINC_CmdFSOP(WINC_CMD_REQ_HANDLE handle, uint8_t op, int32_t optFiletype, const uint8_t* pOptFilename, size_t lenOptFilename);

/*****************************************************************************
  Description:
    This function performs a filesystem operation.

  Parameters:
    handle         - Command request session handle
    filetype       - File type
    pOptFilename   - The name of the file
    lenOptFilename - Length of pOptFilename
    optFilelength  - File length

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    pOptFilename will be ignored if its value is NULL.

    optFilelength will be ignored if its value is WINC_CMDFSLOAD_FILELENGTH_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdFSLOAD(WINC_CMD_REQ_HANDLE handle, uint8_t filetype, const uint8_t* pOptFilename, size_t lenOptFilename, uint16_t optFilelength);

/*****************************************************************************
  Description:
    This function performs a filesystem operation.

  Parameters:
    handle      - Command request session handle
    filetype    - File type
    pFilename   - The name of the file
    lenFilename - Length of pFilename

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdFSSTORE(WINC_CMD_REQ_HANDLE handle, uint8_t filetype, const uint8_t* pFilename, size_t lenFilename);

/*****************************************************************************
  Description:
    This function performs a filesystem transfer operation.

  Parameters:
    handle        - Command request session handle
    optTsfrHandle - Transfer handle
    optBlockNum   - Block number
    pOptData      - Transfer data
    lenOptData    - Length of pOptData
    optCrc        - Transfer CRC-16

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optTsfrHandle will be ignored if its value is WINC_CMDFSTSFR_TSFR_HANDLE_IGNORE_VAL.

    optBlockNum will be ignored if its value is WINC_CMDFSTSFR_BLOCK_NUM_IGNORE_VAL.

    pOptData will be ignored if its value is NULL.

    optCrc will be ignored if its value is WINC_CMDFSTSFR_CRC_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdFSTSFR(WINC_CMD_REQ_HANDLE handle, uint16_t optTsfrHandle, int32_t optBlockNum, const uint8_t* pOptData, size_t lenOptData, int32_t optCrc);

/*****************************************************************************
  Description:
    This function performs a filesystem transfer operation.

  Parameters:
    handle        - Command request session handle
    tsfrHandle    - Transfer handle
    blockNum      - Block number
    optDataLength - Data length

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optDataLength will be ignored if its value is WINC_CMDFSRECV_DATA_LENGTH_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdFSRECV(WINC_CMD_REQ_HANDLE handle, uint16_t tsfrHandle, uint8_t blockNum, uint16_t optDataLength);

/*****************************************************************************
  Description:
    This function is used to read or set the MQTT configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDMQTTC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdMQTTC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to connect to an MQTT broker.

  Parameters:
    handle   - Command request session handle
    optClean - Clean Session

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optClean will be ignored if its value is WINC_CMDMQTTCONN_CLEAN_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdMQTTCONN(WINC_CMD_REQ_HANDLE handle, int32_t optClean);

/*****************************************************************************
  Description:
    This function is used to subscribe to an MQTT topic.

  Parameters:
    handle       - Command request session handle
    pTopicName   - Topic Name
    lenTopicName - Length of pTopicName
    maxQos       - Maximum QoS

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdMQTTSUB(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName, uint8_t maxQos);

/*****************************************************************************
  Description:
    This function is used to list MQTT topic subscriptions.

  Parameters:
    handle - Command request session handle

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdMQTTSUBLST(WINC_CMD_REQ_HANDLE handle);

/*****************************************************************************
  Description:
    This function is used receive data from subscriptions.

  Parameters:
    handle       - Command request session handle
    pTopicName   - Topic Name
    lenTopicName - Length of pTopicName
    msgId        - Message Identifier
    length       - The number of bytes to receive

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdMQTTSUBRD(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName, uint16_t msgId, uint16_t length);

/*****************************************************************************
  Description:
    This function is used to unsubscribe from an MQTT topic.

  Parameters:
    handle       - Command request session handle
    pTopicName   - Topic Name
    lenTopicName - Length of pTopicName

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdMQTTUNSUB(WINC_CMD_REQ_HANDLE handle, const uint8_t* pTopicName, size_t lenTopicName);

/*****************************************************************************
  Description:
    This function is used to publish a message.

  Parameters:
    handle          - Command request session handle
    dup             - Duplicate Message
    qos             - QoS
    retain          - Retain Message
    typeTopicNameId - Type of topicNameId
    topicNameId     - Topic Name or Alias
    lenTopicNameId  - Length of topicNameId
    pTopicPayload   - Topic Payload
    lenTopicPayload - Length of pTopicPayload

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdMQTTPUB(WINC_CMD_REQ_HANDLE handle, uint8_t dup, uint8_t qos, uint8_t retain, WINC_TYPE typeTopicNameId, uintptr_t topicNameId, size_t lenTopicNameId, const uint8_t* pTopicPayload, size_t lenTopicPayload);

/*****************************************************************************
  Description:
    This function is used to define a last will message.

  Parameters:
    handle          - Command request session handle
    qos             - QoS
    retain          - Retain Message
    pTopicName      - Topic Name
    lenTopicName    - Length of pTopicName
    pTopicPayload   - Topic Payload
    lenTopicPayload - Length of pTopicPayload

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdMQTTLWT(WINC_CMD_REQ_HANDLE handle, uint8_t qos, uint8_t retain, const uint8_t* pTopicName, size_t lenTopicName, const uint8_t* pTopicPayload, size_t lenTopicPayload);

/*****************************************************************************
  Description:
    This function is used to disconnect from a broker.

  Parameters:
    handle        - Command request session handle
    optReasonCode - Reason Code

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optReasonCode will be ignored if its value is WINC_CMDMQTTDISCONN_REASON_CODE_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdMQTTDISCONN(WINC_CMD_REQ_HANDLE handle, int32_t optReasonCode);

/*****************************************************************************
  Description:
    This function is used to read or set the MQTT transmit properties.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDMQTTPROPTX_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdMQTTPROPTX(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to read or set the MQTT transmit properties.

  Parameters:
    handle     - Command request session handle
    pKey       - Parameter key
    lenKey     - Length of pKey
    pOptVals   - Parameter value
    lenOptVals - Length of pOptVals

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    pOptVals will be ignored if its value is NULL.

 *****************************************************************************/

bool WINC_CmdMQTTPROPTXKV(WINC_CMD_REQ_HANDLE handle, const uint8_t* pKey, size_t lenKey, const uint8_t* pOptVals, size_t lenOptVals);

/*****************************************************************************
  Description:
    This function is used to read the MQTT receive properties.

  Parameters:
    handle - Command request session handle
    optId  - Parameter ID number

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDMQTTPROPRX_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdMQTTPROPRX(WINC_CMD_REQ_HANDLE handle, uint32_t optId);

/*****************************************************************************
  Description:
    This function is used to define which transmit properties are selected.

  Parameters:
    handle     - Command request session handle
    optPropId  - Property Identifier
    optPropSel - Property Selected

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optPropId will be ignored if its value is WINC_CMDMQTTPROPTXS_PROP_ID_IGNORE_VAL.

    optPropSel will be ignored if its value is WINC_CMDMQTTPROPTXS_PROP_SEL_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdMQTTPROPTXS(WINC_CMD_REQ_HANDLE handle, int32_t optPropId, int32_t optPropSel);

/*****************************************************************************
  Description:
    This function is used to read or set the network interface configuration.

  Parameters:
    handle     - Command request session handle
    optIntf    - Interface number
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optIntf will be ignored if its value is WINC_CMDNETIFC_INTF_IGNORE_VAL.

    optId will be ignored if its value is WINC_CMDNETIFC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdNETIFC(WINC_CMD_REQ_HANDLE handle, int32_t optIntf, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to send L2 data frames.

  Parameters:
    handle  - Command request session handle
    intf    - Interface number
    pData   - Data frame data
    lenData - Length of pData

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdNETIFTX(WINC_CMD_REQ_HANDLE handle, uint8_t intf, const uint8_t* pData, size_t lenData);

/*****************************************************************************
  Description:
    This function is used to read or set the OTA configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDOTAC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdOTAC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function allows the downloading of firmware to the WINC.

  Parameters:
    handle - Command request session handle
    state  - Enable the OTA download feature

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdOTADL(WINC_CMD_REQ_HANDLE handle, uint8_t state);

/*****************************************************************************
  Description:
    This function verifies the OTA firmware image.

  Parameters:
    handle - Command request session handle

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdOTAVFY(WINC_CMD_REQ_HANDLE handle);

/*****************************************************************************
  Description:
    This function activates the OTA firmware image.

  Parameters:
    handle - Command request session handle

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdOTAACT(WINC_CMD_REQ_HANDLE handle);

/*****************************************************************************
  Description:
    This function will invalidate the current running firmware image.
      Invalidation does not delete the current running firmware image.

  Parameters:
    handle - Command request session handle

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdOTAINV(WINC_CMD_REQ_HANDLE handle);

/*****************************************************************************
  Description:
    This function sends a ping (ICMP Echo Request) to the target address.

  Parameters:
    handle             - Command request session handle
    typeTargetAddr     - Type of targetAddr
    targetAddr         - IP address or host name of target
    lenTargetAddr      - Length of targetAddr
    optProtocolVersion - IP protocol version

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optProtocolVersion will be ignored if its value is WINC_CMDPING_PROTOCOL_VERSION_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdPING(WINC_CMD_REQ_HANDLE handle, WINC_TYPE typeTargetAddr, uintptr_t targetAddr, size_t lenTargetAddr, uint8_t optProtocolVersion);

/*****************************************************************************
  Description:
    This function is used to reset the WINC.

  Parameters:
    handle - Command request session handle

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdRST(WINC_CMD_REQ_HANDLE handle);

/*****************************************************************************
  Description:
    This function is used to read or set the SNTP configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDSNTPC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSNTPC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to open a new socket.

  Parameters:
    handle             - Command request session handle
    protocol           - The protocol to use
    optProtocolVersion - IP protocol version

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optProtocolVersion will be ignored if its value is WINC_CMDSOCKO_PROTOCOL_VERSION_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSOCKO(WINC_CMD_REQ_HANDLE handle, uint8_t protocol, uint8_t optProtocolVersion);

/*****************************************************************************
  Description:
    This function is used to bind a socket to a local port.

  Parameters:
    handle      - Command request session handle
    sockId      - The socket ID
    lclPort     - The local port number to use
    optPendSkts - Number of pending sockets connections

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optPendSkts will be ignored if its value is WINC_CMDSOCKBL_PEND_SKTS_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSOCKBL(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint16_t lclPort, uint8_t optPendSkts);

/*****************************************************************************
  Description:
    This function is used to bind a socket to a remote address.

  Parameters:
    handle      - Command request session handle
    sockId      - The socket ID
    typeRmtAddr - Type of rmtAddr
    rmtAddr     - The address of the remote device
    lenRmtAddr  - Length of rmtAddr
    rmtPort     - The port number on the remote device

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdSOCKBR(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeRmtAddr, uintptr_t rmtAddr, size_t lenRmtAddr, uint16_t rmtPort);

/*****************************************************************************
  Description:
    This function is used to bind a socket to a multicast group.

  Parameters:
    handle        - Command request session handle
    sockId        - The socket ID
    typeMcastAddr - Type of mcastAddr
    mcastAddr     - The address of the multicast group
    lenMcastAddr  - Length of mcastAddr
    mcastPort     - The port number of the multicast group

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdSOCKBM(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeMcastAddr, uintptr_t mcastAddr, size_t lenMcastAddr, uint16_t mcastPort);

/*****************************************************************************
  Description:
    This function is used to enable TLS on a socket.

  Parameters:
    handle  - Command request session handle
    sockId  - The socket ID
    tlsConf - TLS certificate configuration

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdSOCKTLS(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t tlsConf);

/*****************************************************************************
  Description:
    This function is used to send data over a socket that is bound to a remote
      address and port number.

  Parameters:
    handle    - Command request session handle
    sockId    - The socket ID
    length    - The length of the data to send
    optSeqNum - Sequence number of first byte
    pData     - The data to send
    lenData   - Length of pData

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it will be ignored. Certain subsequent parameters may
    also be ignored.

    optSeqNum will be ignored if its value is WINC_CMDSOCKWR_SEQ_NUM_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSOCKWR(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint16_t length, int32_t optSeqNum, const uint8_t* pData, size_t lenData);

/*****************************************************************************
  Description:
    This function is used to send data to an arbitrary destination using the
      connectionless UDP protocol.

  Parameters:
    handle      - Command request session handle
    sockId      - The socket ID
    typeRmtAddr - Type of rmtAddr
    rmtAddr     - The address of the remote device
    lenRmtAddr  - Length of rmtAddr
    rmtPort     - The port number on the remote device
    length      - The length of the data to send
    optSeqNum   - Sequence number of first byte
    pData       - The data to send
    lenData     - Length of pData

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it will be ignored. Certain subsequent parameters may
    also be ignored.

    optSeqNum will be ignored if its value is WINC_CMDSOCKWRTO_SEQ_NUM_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSOCKWRTO(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, WINC_TYPE typeRmtAddr, uintptr_t rmtAddr, size_t lenRmtAddr, uint16_t rmtPort, uint16_t length, int32_t optSeqNum, const uint8_t* pData, size_t lenData);

/*****************************************************************************
  Description:
    This function is used to read data from a socket.

  Parameters:
    handle     - Command request session handle
    sockId     - The socket ID
    outputMode - The format the DTE wishes to receive the data
    length     - The number of bytes the DTE wishes to read

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdSOCKRD(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t outputMode, int32_t length);

/*****************************************************************************
  Description:
    This function is used to read data from a socket buffer.

  Parameters:
    handle     - Command request session handle
    sockId     - The socket ID
    outputMode - The format the DTE wishes to receive the data
    length     - The number of bytes the DTE wishes to read

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdSOCKRDBUF(WINC_CMD_REQ_HANDLE handle, uint16_t sockId, uint8_t outputMode, int32_t length);

/*****************************************************************************
  Description:
    This function is used to close a socket.

  Parameters:
    handle - Command request session handle
    sockId - The socket ID

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdSOCKCL(WINC_CMD_REQ_HANDLE handle, uint16_t sockId);

/*****************************************************************************
  Description:
    This function is used to present a list of the WINC's open
      sockets/connections.

  Parameters:
    handle    - Command request session handle
    optSockId - The socket ID

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optSockId will be ignored if its value is WINC_CMDSOCKLST_SOCK_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSOCKLST(WINC_CMD_REQ_HANDLE handle, uint16_t optSockId);

/*****************************************************************************
  Description:
    This function is used to read or set the socket configuration.

  Parameters:
    handle     - Command request session handle
    optSockId  - The socket ID
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optSockId will be ignored if its value is WINC_CMDSOCKC_SOCK_ID_IGNORE_VAL.

    optId will be ignored if its value is WINC_CMDSOCKC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSOCKC(WINC_CMD_REQ_HANDLE handle, uint16_t optSockId, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to set or query the system time.

  Parameters:
    handle    - Command request session handle
    optFormat - Format of time

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optFormat will be ignored if its value is WINC_CMDTIME_FORMAT_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdTIME(WINC_CMD_REQ_HANDLE handle, uint8_t optFormat);

/*****************************************************************************
  Description:
    This function is used to set or query the system time.

  Parameters:
    handle - Command request session handle
    format - Format of time
    utcSec - UTC seconds

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdTIMEUTCSEC(WINC_CMD_REQ_HANDLE handle, uint8_t format, uint32_t utcSec);

/*****************************************************************************
  Description:
    This function is used to set or query the system time.

  Parameters:
    handle      - Command request session handle
    pDateTime   - Date/time in format YYYY-MM-DDTHH:MM:SS.00Z
    lenDateTime - Length of pDateTime

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdTIMERFC(WINC_CMD_REQ_HANDLE handle, const uint8_t* pDateTime, size_t lenDateTime);

/*****************************************************************************
  Description:
    This function is used to read or set the TLS configuration.

  Parameters:
    handle     - Command request session handle
    optConf    - Configuration number
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optConf will be ignored if its value is WINC_CMDTLSC_CONF_IGNORE_VAL.

    optId will be ignored if its value is WINC_CMDTLSC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdTLSC(WINC_CMD_REQ_HANDLE handle, uint8_t optConf, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to read or set the TLS cipher suite configuration.

  Parameters:
    handle     - Command request session handle
    optCslIdx  - Cipher suite list index
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optCslIdx will be ignored if its value is WINC_CMDTLSCSC_CSL_IDX_IGNORE_VAL.

    optId will be ignored if its value is WINC_CMDTLSCSC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdTLSCSC(WINC_CMD_REQ_HANDLE handle, uint8_t optCslIdx, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to read or set the WINC's hotspot access point
      configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDWAPC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdWAPC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to enable the WINC's hotspot access point
      functionality.

  Parameters:
    handle   - Command request session handle
    optState - State of the hotspot feature

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optState will be ignored if its value is WINC_CMDWAP_STATE_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdWAP(WINC_CMD_REQ_HANDLE handle, int32_t optState);

/*****************************************************************************
  Description:
    This function is used to modify or query the behavior of the active
      scanning function.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDWSCNC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdWSCNC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to scan for infrastructure networks in range of the
      WINC.

  Parameters:
    handle  - Command request session handle
    actPasv - Flag indicating active or passive scanning

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdWSCN(WINC_CMD_REQ_HANDLE handle, uint8_t actPasv);

/*****************************************************************************
  Description:
    This function is used to read or set the WINC's Wi-Fi station mode
      configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDWSTAC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdWSTAC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to control or query the WINC's station mode
      functionality.

  Parameters:
    handle   - Command request session handle
    optState - State of the Wi-Fi station feature

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optState will be ignored if its value is WINC_CMDWSTA_STATE_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdWSTA(WINC_CMD_REQ_HANDLE handle, int32_t optState);

/*****************************************************************************
  Description:
    This function is used to query current WiFi associations.

  Parameters:
    handle     - Command request session handle
    optAssocId - Association ID

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optAssocId will be ignored if its value is WINC_CMDASSOC_ASSOC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdASSOC(WINC_CMD_REQ_HANDLE handle, uint16_t optAssocId);

/*****************************************************************************
  Description:
    This function is used to query system information.

  Parameters:
    handle    - Command request session handle
    optFilter - System information filter bitmask

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optFilter will be ignored if its value is WINC_CMDSI_FILTER_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSI(WINC_CMD_REQ_HANDLE handle, uint8_t optFilter);

/*****************************************************************************
  Description:
    This function is used to read or set the provisioning service
      configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDWPROVC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdWPROVC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to control or query the provisioning service.

  Parameters:
    handle   - Command request session handle
    optState - State of the provisioning service

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optState will be ignored if its value is WINC_CMDWPROV_STATE_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdWPROV(WINC_CMD_REQ_HANDLE handle, int32_t optState);

/*****************************************************************************
  Description:
    This function is used to query the device information.

  Parameters:
    handle - Command request session handle
    optId  - Parameter ID number

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDDI_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdDI(WINC_CMD_REQ_HANDLE handle, uint32_t optId);

/*****************************************************************************
  Description:
    This function is used to provide the result of an external crypto
      operation.

  Parameters:
    handle          - Command request session handle
    opId            - Operation identifier, matching <OP_ID> in corresponding
                         AEC
    status          - Operation success or failure
    pOptSignature   - Signature (big endian). For ECDSA signatures: R then S,
                         each the size of the curve.
    lenOptSignature - Length of pOptSignature

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    pOptSignature will be ignored if its value is NULL.

 *****************************************************************************/

bool WINC_CmdEXTCRYPTO(WINC_CMD_REQ_HANDLE handle, uint16_t opId, uint8_t status, const uint8_t* pOptSignature, size_t lenOptSignature);

/*****************************************************************************
  Description:
    This function is used to read or set the device's Wi-Fi configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDWIFIC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdWIFIC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to read or set the NVM configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDNVMC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdNVMC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function allows sector erase within the alternate firmware partition.

  Parameters:
    handle       - Command request session handle
    sectorOffset - Sector offset to be used for the NVM operation
    sectors      - The number of sectors to erase during NVM operation

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdNVMER(WINC_CMD_REQ_HANDLE handle, uint8_t sectorOffset, uint8_t sectors);

/*****************************************************************************
  Description:
    This function allows data to be written to the alternate firmware
      partition. Writes are performed as normal for flash devices, therefore
      it is recommended to erase the sector of data before performing a write.
      Write operations cannot cross a sector boundary.

  Parameters:
    handle  - Command request session handle
    offset  - Byte offset to be used for the NVM operation
    length  - The number of bytes for the NVM operation
    pData   - The data to write in hexadecimal string format
    lenData - Length of pData

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdNVMWR(WINC_CMD_REQ_HANDLE handle, uint32_t offset, uint16_t length, const uint8_t* pData, size_t lenData);

/*****************************************************************************
  Description:
    This function allows checking of the alternate firmware partition.

  Parameters:
    handle - Command request session handle
    offset - Byte offset to be used for the NVM operation
    length - The number of bytes for the NVM operation

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdNVMCHK(WINC_CMD_REQ_HANDLE handle, uint32_t offset, uint32_t length);

/*****************************************************************************
  Description:
    This function allows reading of the alternate firmware partition.

  Parameters:
    handle - Command request session handle
    offset - Byte offset to be used for the NVM operation
    length - The number of bytes for the NVM operation

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdNVMRD(WINC_CMD_REQ_HANDLE handle, uint32_t offset, uint16_t length);

/*****************************************************************************
  Description:
    This function is used to get the parameters for modifying the little
      endian address field of a DFU image.

  Parameters:
    handle    - Command request session handle
    partition - The partition into which the DFU image will be programmed

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdDFUADR(WINC_CMD_REQ_HANDLE handle, uint8_t partition);

/*****************************************************************************
  Description:
    This function is used to get the parameters for modifying the little
      endian sequence number field of a DFU image.

  Parameters:
    handle     - Command request session handle
    imageState - The desired state of the DFU image after programming

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdDFUSEQ(WINC_CMD_REQ_HANDLE handle, uint8_t imageState);

/*****************************************************************************
  Description:
    This function is used to read or set the PPS configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDPPSC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdPPSC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function allows the firmware to change the state of platform power
      save.

  Parameters:
    handle   - Command request session handle
    ppsState - PPS State

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    None.

 *****************************************************************************/

bool WINC_CmdPPS(WINC_CMD_REQ_HANDLE handle, uint8_t ppsState);

/*****************************************************************************
  Description:
    This function is used to read or set the SYSLOG configuration.

  Parameters:
    handle     - Command request session handle
    optId      - Parameter ID number
    typeOptVal - Type of optVal
    optVal     - Parameter value
    lenOptVal  - Length of optVal

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optId will be ignored if its value is WINC_CMDSYSLOGC_ID_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdSYSLOGC(WINC_CMD_REQ_HANDLE handle, uint32_t optId, WINC_TYPE typeOptVal, uintptr_t optVal, size_t lenOptVal);

/*****************************************************************************
  Description:
    This function is used to get or increase the WINC's ARB value, and to get
      the current firmware's security level.

  Parameters:
    handle   - Command request session handle
    optValue - The value to which to increase the DCE's ARB

  Returns:
    true  - Success.
    false - Failure.

  Remarks:
    This function has optional parameters, when an optional parameter is set
    to its ignore value it and all subsequent parameters are ignored.
    Subsequent parameters should also be set to their ignore values.

    optValue will be ignored if its value is WINC_CMDARB_VALUE_IGNORE_VAL.

 *****************************************************************************/

bool WINC_CmdARB(WINC_CMD_REQ_HANDLE handle, uint8_t optValue);

#endif // WINC_CMDS_H
