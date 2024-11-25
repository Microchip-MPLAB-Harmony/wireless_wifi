/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#include <string.h>

#include "winc_tables.h"

typedef struct
{
    uint16_t        id;
    uint16_t        length;
    const char*     pName;
} WINC_STRING_ID_TABLE_TYPE;

static const WINC_STRING_ID_TABLE_TYPE wincCmdIdTable[WINC_CMD_ID_TABLE_SZ+1U] =
{
    {.id = WINC_CMD_ID_GMI, .length = 4, .pName = "+GMI"},
    {.id = WINC_CMD_ID_GMM, .length = 4, .pName = "+GMM"},
    {.id = WINC_CMD_ID_GMR, .length = 4, .pName = "+GMR"},
    {.id = WINC_CMD_ID_IPR, .length = 4, .pName = "+IPR"},
    {.id = WINC_CMD_ID_CFG, .length = 4, .pName = "+CFG"},
    {.id = WINC_CMD_ID_CFGCP, .length = 6, .pName = "+CFGCP"},
    {.id = WINC_CMD_ID_DHCPSC, .length = 7, .pName = "+DHCPSC"},
    {.id = WINC_CMD_ID_DNSC, .length = 5, .pName = "+DNSC"},
    {.id = WINC_CMD_ID_DNSRESOLV, .length = 10, .pName = "+DNSRESOLV"},
    {.id = WINC_CMD_ID_FS, .length = 3, .pName = "+FS"},
    {.id = WINC_CMD_ID_FSTSFR, .length = 7, .pName = "+FSTSFR"},
    {.id = WINC_CMD_ID_MQTTC, .length = 6, .pName = "+MQTTC"},
    {.id = WINC_CMD_ID_MQTTCONN, .length = 9, .pName = "+MQTTCONN"},
    {.id = WINC_CMD_ID_MQTTSUB, .length = 8, .pName = "+MQTTSUB"},
    {.id = WINC_CMD_ID_MQTTSUBLST, .length = 11, .pName = "+MQTTSUBLST"},
    {.id = WINC_CMD_ID_MQTTSUBRD, .length = 10, .pName = "+MQTTSUBRD"},
    {.id = WINC_CMD_ID_MQTTUNSUB, .length = 10, .pName = "+MQTTUNSUB"},
    {.id = WINC_CMD_ID_MQTTPUB, .length = 8, .pName = "+MQTTPUB"},
    {.id = WINC_CMD_ID_MQTTLWT, .length = 8, .pName = "+MQTTLWT"},
    {.id = WINC_CMD_ID_MQTTDISCONN, .length = 12, .pName = "+MQTTDISCONN"},
    {.id = WINC_CMD_ID_MQTTPROPTX, .length = 11, .pName = "+MQTTPROPTX"},
    {.id = WINC_CMD_ID_MQTTPROPRX, .length = 11, .pName = "+MQTTPROPRX"},
    {.id = WINC_CMD_ID_MQTTPROPTXS, .length = 12, .pName = "+MQTTPROPTXS"},
    {.id = WINC_CMD_ID_NETIFC, .length = 7, .pName = "+NETIFC"},
    {.id = WINC_CMD_ID_NETIFTX, .length = 8, .pName = "+NETIFTX"},
    {.id = WINC_CMD_ID_OTAC, .length = 5, .pName = "+OTAC"},
    {.id = WINC_CMD_ID_OTADL, .length = 6, .pName = "+OTADL"},
    {.id = WINC_CMD_ID_OTAVFY, .length = 7, .pName = "+OTAVFY"},
    {.id = WINC_CMD_ID_OTAACT, .length = 7, .pName = "+OTAACT"},
    {.id = WINC_CMD_ID_OTAINV, .length = 7, .pName = "+OTAINV"},
    {.id = WINC_CMD_ID_PING, .length = 5, .pName = "+PING"},
    {.id = WINC_CMD_ID_RST, .length = 4, .pName = "+RST"},
    {.id = WINC_CMD_ID_SNTPC, .length = 6, .pName = "+SNTPC"},
    {.id = WINC_CMD_ID_SOCKO, .length = 6, .pName = "+SOCKO"},
    {.id = WINC_CMD_ID_SOCKBL, .length = 7, .pName = "+SOCKBL"},
    {.id = WINC_CMD_ID_SOCKBR, .length = 7, .pName = "+SOCKBR"},
    {.id = WINC_CMD_ID_SOCKBM, .length = 7, .pName = "+SOCKBM"},
    {.id = WINC_CMD_ID_SOCKTLS, .length = 8, .pName = "+SOCKTLS"},
    {.id = WINC_CMD_ID_SOCKWR, .length = 7, .pName = "+SOCKWR"},
    {.id = WINC_CMD_ID_SOCKWRTO, .length = 9, .pName = "+SOCKWRTO"},
    {.id = WINC_CMD_ID_SOCKRD, .length = 7, .pName = "+SOCKRD"},
    {.id = WINC_CMD_ID_SOCKRDBUF, .length = 10, .pName = "+SOCKRDBUF"},
    {.id = WINC_CMD_ID_SOCKCL, .length = 7, .pName = "+SOCKCL"},
    {.id = WINC_CMD_ID_SOCKLST, .length = 8, .pName = "+SOCKLST"},
    {.id = WINC_CMD_ID_SOCKC, .length = 6, .pName = "+SOCKC"},
    {.id = WINC_CMD_ID_TIME, .length = 5, .pName = "+TIME"},
    {.id = WINC_CMD_ID_TLSC, .length = 5, .pName = "+TLSC"},
    {.id = WINC_CMD_ID_TLSCSC, .length = 7, .pName = "+TLSCSC"},
    {.id = WINC_CMD_ID_WAPC, .length = 5, .pName = "+WAPC"},
    {.id = WINC_CMD_ID_WAP, .length = 4, .pName = "+WAP"},
    {.id = WINC_CMD_ID_WSCNC, .length = 6, .pName = "+WSCNC"},
    {.id = WINC_CMD_ID_WSCN, .length = 5, .pName = "+WSCN"},
    {.id = WINC_CMD_ID_WSTAC, .length = 6, .pName = "+WSTAC"},
    {.id = WINC_CMD_ID_WSTA, .length = 5, .pName = "+WSTA"},
    {.id = WINC_CMD_ID_ASSOC, .length = 6, .pName = "+ASSOC"},
    {.id = WINC_CMD_ID_SI, .length = 3, .pName = "+SI"},
    {.id = WINC_CMD_ID_WPROVC, .length = 7, .pName = "+WPROVC"},
    {.id = WINC_CMD_ID_WPROV, .length = 6, .pName = "+WPROV"},
    {.id = WINC_CMD_ID_DI, .length = 3, .pName = "+DI"},
    {.id = WINC_CMD_ID_EXTCRYPTO, .length = 10, .pName = "+EXTCRYPTO"},
    {.id = WINC_CMD_ID_WIFIC, .length = 6, .pName = "+WIFIC"},
    {.id = WINC_CMD_ID_NVMC, .length = 5, .pName = "+NVMC"},
    {.id = WINC_CMD_ID_NVMER, .length = 6, .pName = "+NVMER"},
    {.id = WINC_CMD_ID_NVMWR, .length = 6, .pName = "+NVMWR"},
    {.id = WINC_CMD_ID_NVMCHK, .length = 7, .pName = "+NVMCHK"},
    {.id = WINC_CMD_ID_NVMRD, .length = 6, .pName = "+NVMRD"},
    {.id = 0, .length = 0, .pName = NULL}
};

static const WINC_STRING_ID_TABLE_TYPE wincAecIdTable[WINC_AEC_ID_TABLE_SZ+1U] =
{
    {.id = WINC_AEC_ID_ASSOC, .length = 6, .pName = "+ASSOC"},
    {.id = WINC_AEC_ID_BOOT, .length = 5, .pName = "+BOOT"},
    {.id = WINC_AEC_ID_DNSRESOLV, .length = 10, .pName = "+DNSRESOLV"},
    {.id = WINC_AEC_ID_DNSERR, .length = 7, .pName = "+DNSERR"},
    {.id = WINC_AEC_ID_EXTCRYPTO, .length = 10, .pName = "+EXTCRYPTO"},
    {.id = WINC_AEC_ID_MQTTCONN, .length = 9, .pName = "+MQTTCONN"},
    {.id = WINC_AEC_ID_MQTTCONNACK, .length = 12, .pName = "+MQTTCONNACK"},
    {.id = WINC_AEC_ID_MQTTPUBACK, .length = 11, .pName = "+MQTTPUBACK"},
    {.id = WINC_AEC_ID_MQTTPUBCOMP, .length = 12, .pName = "+MQTTPUBCOMP"},
    {.id = WINC_AEC_ID_MQTTPUBERR, .length = 11, .pName = "+MQTTPUBERR"},
    {.id = WINC_AEC_ID_MQTTSUB, .length = 8, .pName = "+MQTTSUB"},
    {.id = WINC_AEC_ID_MQTTUNSUB, .length = 10, .pName = "+MQTTUNSUB"},
    {.id = WINC_AEC_ID_MQTTSUBRX, .length = 10, .pName = "+MQTTSUBRX"},
    {.id = WINC_AEC_ID_MQTTPROPRX, .length = 11, .pName = "+MQTTPROPRX"},
    {.id = WINC_AEC_ID_NETIFRX, .length = 8, .pName = "+NETIFRX"},
    {.id = WINC_AEC_ID_NVMER, .length = 6, .pName = "+NVMER"},
    {.id = WINC_AEC_ID_NVMCHK, .length = 7, .pName = "+NVMCHK"},
    {.id = WINC_AEC_ID_NVMERR, .length = 7, .pName = "+NVMERR"},
    {.id = WINC_AEC_ID_OTA, .length = 4, .pName = "+OTA"},
    {.id = WINC_AEC_ID_OTAERR, .length = 7, .pName = "+OTAERR"},
    {.id = WINC_AEC_ID_PING, .length = 5, .pName = "+PING"},
    {.id = WINC_AEC_ID_PINGERR, .length = 8, .pName = "+PINGERR"},
    {.id = WINC_AEC_ID_SIMSTAT, .length = 8, .pName = "+SIMSTAT"},
    {.id = WINC_AEC_ID_SIMERR, .length = 7, .pName = "+SIMERR"},
    {.id = WINC_AEC_ID_SIWPKTS, .length = 8, .pName = "+SIWPKTS"},
    {.id = WINC_AEC_ID_SIHEAP, .length = 7, .pName = "+SIHEAP"},
    {.id = WINC_AEC_ID_SIHERR, .length = 7, .pName = "+SIHERR"},
    {.id = WINC_AEC_ID_SISTACK, .length = 8, .pName = "+SISTACK"},
    {.id = WINC_AEC_ID_SNTPERR, .length = 8, .pName = "+SNTPERR"},
    {.id = WINC_AEC_ID_SOCKIND, .length = 8, .pName = "+SOCKIND"},
    {.id = WINC_AEC_ID_SOCKRXT, .length = 8, .pName = "+SOCKRXT"},
    {.id = WINC_AEC_ID_SOCKRXU, .length = 8, .pName = "+SOCKRXU"},
    {.id = WINC_AEC_ID_SOCKCL, .length = 7, .pName = "+SOCKCL"},
    {.id = WINC_AEC_ID_SOCKTLS, .length = 8, .pName = "+SOCKTLS"},
    {.id = WINC_AEC_ID_SOCKERR, .length = 8, .pName = "+SOCKERR"},
    {.id = WINC_AEC_ID_TIME, .length = 5, .pName = "+TIME"},
    {.id = WINC_AEC_ID_WAPAIP, .length = 7, .pName = "+WAPAIP"},
    {.id = WINC_AEC_ID_WAPSC, .length = 6, .pName = "+WAPSC"},
    {.id = WINC_AEC_ID_WAPSD, .length = 6, .pName = "+WAPSD"},
    {.id = WINC_AEC_ID_WAPERR, .length = 7, .pName = "+WAPERR"},
    {.id = WINC_AEC_ID_WPROVAT, .length = 8, .pName = "+WPROVAT"},
    {.id = WINC_AEC_ID_WPROVDT, .length = 8, .pName = "+WPROVDT"},
    {.id = WINC_AEC_ID_WSCNIND, .length = 8, .pName = "+WSCNIND"},
    {.id = WINC_AEC_ID_WSCNDONE, .length = 9, .pName = "+WSCNDONE"},
    {.id = WINC_AEC_ID_WSTAAIP, .length = 8, .pName = "+WSTAAIP"},
    {.id = WINC_AEC_ID_WSTALD, .length = 7, .pName = "+WSTALD"},
    {.id = WINC_AEC_ID_WSTAERR, .length = 8, .pName = "+WSTAERR"},
    {.id = WINC_AEC_ID_WSTALU, .length = 7, .pName = "+WSTALU"},
    {.id = WINC_AEC_ID_WSTAROAM, .length = 9, .pName = "+WSTAROAM"},
    {.id = 0, .length = 0, .pName = NULL}
};

static const WINC_STRING_ID_TABLE_TYPE wincStatusIdTable[WINC_STATUS_ID_TABLE_SZ+1U] =
{
    {.id = WINC_STATUS_OK, .length = 2, .pName = "OK"},
    {.id = WINC_STATUS_ERROR, .length = 13, .pName = "General Error"},
    {.id = WINC_STATUS_INVALID_CMD, .length = 18, .pName = "Invalid AT Command"},
    {.id = WINC_STATUS_UNKNOWN_CMD, .length = 18, .pName = "Unknown AT Command"},
    {.id = WINC_STATUS_INVALID_PARAMETER, .length = 17, .pName = "Invalid Parameter"},
    {.id = WINC_STATUS_INCORRECT_NUM_PARAMS, .length = 30, .pName = "Incorrect Number of Parameters"},
    {.id = WINC_STATUS_STORE_UPDATE_BLOCKED, .length = 28, .pName = "Configuration Update Blocked"},
    {.id = WINC_STATUS_STORE_ACCESS_FAILED, .length = 27, .pName = "Configuration Access Failed"},
    {.id = WINC_STATUS_TIMEOUT, .length = 17, .pName = "Command Timed Out"},
    {.id = WINC_STATUS_HOST_INTERFACE_FAILED, .length = 21, .pName = "Host Interface Failed"},
    {.id = WINC_STATUS_ACCESS_DENIED, .length = 13, .pName = "Access Denied"},
    {.id = WINC_STATUS_WIFI_API_REQUEST_FAILED, .length = 20, .pName = "Wi-Fi Request Failed"},
    {.id = WINC_STATUS_STA_NOT_CONNECTED, .length = 17, .pName = "STA Not Connected"},
    {.id = WINC_STATUS_NETWORK_ERROR, .length = 13, .pName = "Network Error"},
    {.id = WINC_STATUS_FILE_SYSTEM_ERROR, .length = 17, .pName = "File System Error"},
    {.id = WINC_STATUS_CFG_NOT_PRESENT, .length = 25, .pName = "Configuration not present"},
    {.id = WINC_STATUS_DNS_TYPE_NOT_SUPPORTED, .length = 22, .pName = "DNS Type Not Supported"},
    {.id = WINC_STATUS_DNS_TIMEOUT, .length = 17, .pName = "DNS Query Timeout"},
    {.id = WINC_STATUS_DNS_ERROR, .length = 9, .pName = "DNS Error"},
    {.id = WINC_STATUS_TSFR_PROTOCOL_NOT_SUPPORTED, .length = 34, .pName = "Unsupported File Transfer Protocol"},
    {.id = WINC_STATUS_FILE_EXISTS, .length = 11, .pName = "File Exists"},
    {.id = WINC_STATUS_FILE_NOT_FOUND, .length = 14, .pName = "File Not Found"},
    {.id = WINC_STATUS_INVALID_FILE_TYPE, .length = 17, .pName = "Invalid File Type"},
    {.id = WINC_STATUS_MQTT_ERROR, .length = 10, .pName = "MQTT Error"},
    {.id = WINC_STATUS_NETWORK_INTF_DOWN, .length = 22, .pName = "Network interface down"},
    {.id = WINC_STATUS_ERASE_DONE, .length = 10, .pName = "Erase Done"},
    {.id = WINC_STATUS_WRITE_DONE, .length = 10, .pName = "Write Done"},
    {.id = WINC_STATUS_VERIFY_DONE, .length = 11, .pName = "Verify Done"},
    {.id = WINC_STATUS_ACTIVATE_DONE, .length = 13, .pName = "Activate Done"},
    {.id = WINC_STATUS_INVALIDATE_DONE, .length = 15, .pName = "Invalidate Done"},
    {.id = WINC_STATUS_OTA_ERROR, .length = 9, .pName = "OTA Error"},
    {.id = WINC_STATUS_OTA_NO_STA_CONN, .length = 17, .pName = "No STA Connection"},
    {.id = WINC_STATUS_OTA_PROTOCOL_ERROR, .length = 14, .pName = "Protocol Error"},
    {.id = WINC_STATUS_OTA_TLS_ERROR, .length = 9, .pName = "TLS Error"},
    {.id = WINC_STATUS_OTA_IMG_TOO_LARGE, .length = 13, .pName = "Image Exceeds"},
    {.id = WINC_STATUS_OTA_TIMEOUT, .length = 7, .pName = "Timeout"},
    {.id = WINC_STATUS_OTA_VERIFY_FAILED, .length = 19, .pName = "Image Verify Failed"},
    {.id = WINC_STATUS_PING_FAILED, .length = 11, .pName = "Ping Failed"},
    {.id = WINC_STATUS_SNTP_SERVER_TMO, .length = 18, .pName = "NTP Server Timeout"},
    {.id = WINC_STATUS_SNTP_PROTOCOL_ERROR, .length = 18, .pName = "NTP Protocol Error"},
    {.id = WINC_STATUS_SOCKET_ID_NOT_FOUND, .length = 19, .pName = "Socket ID Not Found"},
    {.id = WINC_STATUS_LENGTH_MISMATCH, .length = 15, .pName = "Length Mismatch"},
    {.id = WINC_STATUS_NO_FREE_SOCKETS, .length = 15, .pName = "No Free Sockets"},
    {.id = WINC_STATUS_SOCKET_INVALID_PROTOCOL, .length = 23, .pName = "Invalid Socket Protocol"},
    {.id = WINC_STATUS_SOCKET_CLOSE_FAILED, .length = 19, .pName = "Socket Close Failed"},
    {.id = WINC_STATUS_SOCKET_BIND_FAILED, .length = 18, .pName = "Socket Bind Failed"},
    {.id = WINC_STATUS_SOCKET_TLS_FAILED, .length = 17, .pName = "Socket TLS Failed"},
    {.id = WINC_STATUS_SOCKET_CONNECT_FAILED, .length = 21, .pName = "Socket Connect Failed"},
    {.id = WINC_STATUS_SOCKET_SEND_FAILED, .length = 18, .pName = "Socket Send Failed"},
    {.id = WINC_STATUS_SOCKET_SET_OPT_FAILED, .length = 24, .pName = "Socket Set Option Failed"},
    {.id = WINC_STATUS_SOCKET_REMOTE_NOT_SET, .length = 26, .pName = "Socket Destination Not Set"},
    {.id = WINC_STATUS_MULTICAST_ERROR, .length = 15, .pName = "Multicast Error"},
    {.id = WINC_STATUS_SOCKET_NOT_READY, .length = 16, .pName = "Socket Not Ready"},
    {.id = WINC_STATUS_SOCKET_SEQUENCE_ERROR, .length = 21, .pName = "Socket Sequence Error"},
    {.id = WINC_STATUS_TIME_ERROR, .length = 10, .pName = "Time Error"},
    {.id = WINC_STATUS_WAP_STOP_REFUSED, .length = 26, .pName = "Soft AP Stop Not Permitted"},
    {.id = WINC_STATUS_WAP_STOP_FAILED, .length = 19, .pName = "Soft AP Stop Failed"},
    {.id = WINC_STATUS_WAP_START_REFUSED, .length = 27, .pName = "Soft AP Start Not Permitted"},
    {.id = WINC_STATUS_WAP_START_FAILED, .length = 20, .pName = "Soft AP Start Failed"},
    {.id = WINC_STATUS_UNSUPPORTTED_SEC_TYPE, .length = 25, .pName = "Unsupported Security Type"},
    {.id = WINC_STATUS_STA_DISCONN_REFUSED, .length = 28, .pName = "STA Disconnect Not Permitted"},
    {.id = WINC_STATUS_STA_DISCONN_FAILED, .length = 21, .pName = "STA Disconnect Failed"},
    {.id = WINC_STATUS_STA_CONN_REFUSED, .length = 28, .pName = "STA Connection Not Permitted"},
    {.id = WINC_STATUS_STA_CONN_FAILED, .length = 21, .pName = "STA Connection Failed"},
    {.id = WINC_STATUS_ASSOC_NOT_FOUND, .length = 21, .pName = "Association Not Found"},
    {.id = 0, .length = 0, .pName = NULL}
};

static const uint8_t wincModuleIdList[WINC_NUM_MODULES] =
{
    0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 22, 23, 25, 26, 27, 28, 29
};

/*****************************************************************************
  Description:
    Iterate through the translation table.

  Parameters:
    pEntry - Pointer to previous entry
    pId    - Pointer to location to store ID
    ppName - Pointer to location to store name

  Returns:
    New iteration state.

  Remarks:

 *****************************************************************************/

static const WINC_STRING_ID_TABLE_TYPE* tablesIterate(const WINC_STRING_ID_TABLE_TYPE *pEntry, uint16_t *pId, const char **ppName)
{
    if (NULL == pEntry)
    {
        return NULL;
    }

    if (NULL != pId)
    {
        *pId = pEntry->id;
    }

    if (NULL != ppName)
    {
        *ppName = pEntry->pName;
    }

    pEntry++;

    return pEntry;
}

/*****************************************************************************
  Description:
    Lookup name.

  Parameters:
    pEntry - Pointer to table to use
    pName  - Pointer to name

  Returns:
    ID or zero for error.

  Remarks:

 *****************************************************************************/

static uint16_t tablesLookupByString(const WINC_STRING_ID_TABLE_TYPE *pEntry, const char *pName)
{
    if ((NULL == pEntry) || (NULL == pName))
    {
        return 0;
    }

    while (NULL != pEntry->pName)
    {
        if (0 == strncmp(pEntry->pName, pName, pEntry->length))
        {
            return pEntry->id;
        }

        pEntry++;
    }

    return 0;
}

/*****************************************************************************
  Description:
    Lookup ID.

  Parameters:
    pEntry - Pointer to table to use
    id - ID

  Returns:
    Pointer to name.

  Remarks:

 *****************************************************************************/

static const char* tablesLookupByID(const WINC_STRING_ID_TABLE_TYPE *pEntry, uint16_t id)
{
    if (NULL == pEntry)
    {
        return NULL;
    }

    while (NULL != pEntry->pName)
    {
        if (pEntry->id == id)
        {
            return pEntry->pName;
        }

        pEntry++;
    }

    return NULL;
}

/*****************************************************************************
  Description:
    Iterate through the command translation table.

  Parameters:
    iterState - Iteration state
    pId       - Pointer to location to store ID
    ppName    - Pointer to location to store name

  Returns:
    New iteration state.

  Remarks:

 *****************************************************************************/

uintptr_t WINC_TablesCommandIterate(uintptr_t iterState, uint16_t *pId, const char **ppName)
{
    const WINC_STRING_ID_TABLE_TYPE *pEntry = (WINC_STRING_ID_TABLE_TYPE*)iterState;

    if (NULL == pEntry)
    {
        pEntry = wincCmdIdTable;
    }

    return (uintptr_t)(const void*)tablesIterate(pEntry, pId, ppName);
}

/*****************************************************************************
  Description:
    Lookup command name.

  Parameters:
    pName - Pointer to command name

  Returns:
    Command ID or zero for error.

  Remarks:

 *****************************************************************************/

uint16_t WINC_TablesCommandLookupByString(const char *pName)
{
    return tablesLookupByString(wincCmdIdTable, pName);
}

/*****************************************************************************
  Description:
    Lookup command ID.

  Parameters:
    id - Command ID

  Returns:
    Pointer to command name.

  Remarks:

 *****************************************************************************/

const char* WINC_TablesCommandLookupByID(uint16_t id)
{
    return tablesLookupByID(wincCmdIdTable, id);
}

/*****************************************************************************
  Description:
    Iterate through the AEC translation table.

  Parameters:
    iterState - Iteration state
    pId       - Pointer to location to store ID
    ppName    - Pointer to location to store name

  Returns:
    New iteration state.

  Remarks:

 *****************************************************************************/

uintptr_t WINC_TablesAECIterate(uintptr_t iterState, uint16_t *pId, const char **ppName)
{
    const WINC_STRING_ID_TABLE_TYPE *pEntry = (WINC_STRING_ID_TABLE_TYPE*)iterState;

    if (NULL == pEntry)
    {
        pEntry = wincAecIdTable;
    }

    return (uintptr_t)(const void*)tablesIterate(pEntry, pId, ppName);
}

/*****************************************************************************
  Description:
    Lookup AEC name.

  Parameters:
    pName - Pointer to AEC name

  Returns:
    AEC ID or zero for error.

  Remarks:

 *****************************************************************************/

uint16_t WINC_TablesAECLookupByString(const char *pName)
{
    return tablesLookupByString(wincAecIdTable, pName);
}

/*****************************************************************************
  Description:
    Lookup AEC ID.

  Parameters:
    id - AEC ID

  Returns:
    Pointer to AEC name.

  Remarks:

 *****************************************************************************/

const char* WINC_TablesAECLookupByID(uint16_t id)
{
    return tablesLookupByID(wincAecIdTable, id);
}

/*****************************************************************************
  Description:
    Iterate through the status translation table.

  Parameters:
    iterState - Iteration state
    pId       - Pointer to location to store ID
    ppName    - Pointer to location to store status

  Returns:
    New iteration state.

  Remarks:

 *****************************************************************************/

uintptr_t WINC_TablesStatusIterate(uintptr_t iterState, uint16_t *pId, const char **ppName)
{
    const WINC_STRING_ID_TABLE_TYPE *pEntry = (WINC_STRING_ID_TABLE_TYPE*)iterState;

    if (NULL == pEntry)
    {
        pEntry = wincStatusIdTable;
    }

    return (uintptr_t)(const void*)tablesIterate(pEntry, pId, ppName);
}

/*****************************************************************************
  Description:
    Iterate through the status translation table, using module ID to filter.

  Parameters:
    iterState - Iteration state
    modId     - Module ID
    pId       - Pointer to location to store ID
    ppName    - Pointer to location to store status

  Returns:
    New iteration state.

  Remarks:

 *****************************************************************************/

uintptr_t WINC_TablesModuleStatusIterate(uintptr_t iterState, uint16_t modId, uint16_t *pId, const char **ppName)
{
    uintptr_t iterStateLocal;
    uint16_t id;

    do
    {
        iterStateLocal = WINC_TablesStatusIterate(iterState, &id, NULL);

        if ((id >> 8) == modId)
        {
            break;
        }

        iterState = iterStateLocal;
    }
    while (0U != iterStateLocal);

    return WINC_TablesStatusIterate(iterState, pId, ppName);
}

/*****************************************************************************
  Description:
    Lookup status ID.

  Parameters:
    id - Status ID

  Returns:
    Pointer to status.

  Remarks:

 *****************************************************************************/

const char* WINC_TablesStatusLookupByID(uint16_t id)
{
    return tablesLookupByID(wincStatusIdTable, id);
}

/*****************************************************************************
  Description:
    Lookup module ID from list.

  Parameters:
    idx - Module index

  Returns:
    ID of module or -1 for error.

  Remarks:

 *****************************************************************************/

int WINC_TablesModuleIDListGet(uint8_t idx)
{
    if (idx >= WINC_NUM_MODULES)
    {
        return -1;
    }

    return (int)wincModuleIdList[idx];
}
