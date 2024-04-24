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

#ifndef WDRV_WINC_UTILS_H
#define WDRV_WINC_UTILS_H

#include "osal/osal.h"

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif

#if defined(TCPIP_STACK_USE_IPV4) || defined(TCPIP_STACK_USE_IPV6)

#include "tcpip/src/link_list.h"

#define WDRV_WINC_SGL_LIST_NODE         SGL_LIST_NODE
#define WDRV_WINC_SINGLE_LIST           SINGLE_LIST
#define WDRV_WINC_PROTECTED_SINGLE_LIST PROTECTED_SINGLE_LIST
#define WDRV_WINC_IPV4_ADDR             IPV4_ADDR
#define WDRV_WINC_IPV6_ADDR             IPV6_ADDR

#define WDRV_WINC_UtilsSingleListInitialize(LIST)                       TCPIP_Helper_SingleListInitialize(LIST)
#define WDRV_WINC_UtilsSingleListCount(LIST)                            TCPIP_Helper_SingleListCount(LIST)
#define WDRV_WINC_UtilsSingleListHeadRemove(LIST)                       TCPIP_Helper_SingleListHeadRemove(LIST)
#define WDRV_WINC_UtilsSingleListTailAdd(LIST, NODE)                    TCPIP_Helper_SingleListTailAdd(LIST, NODE)
#define WDRV_WINC_UtilsSingleListAppend(DEST_LIST, APP_LIST)            TCPIP_Helper_SingleListAppend(DEST_LIST, APP_LIST)
#define WDRV_WINC_UtilsSingleListHeadAdd(LIST, NODE)                    TCPIP_Helper_SingleListHeadAdd(LIST, NODE)
#define WDRV_WINC_UtilsSingleListNodeRemove(LIST, NODE)                 TCPIP_Helper_SingleListNodeRemove(LIST, NODE)

#define WDRV_WINC_UtilsProtectedSingleListInitialize(LIST)              TCPIP_Helper_ProtectedSingleListInitialize(LIST)
#define WDRV_WINC_UtilsProtectedSingleListCount(LIST)                   TCPIP_Helper_ProtectedSingleListCount(LIST)
#define WDRV_WINC_UtilsProtectedSingleListAppend(DEST_LIST, APP_LIST)   TCPIP_Helper_ProtectedSingleListAppend(DEST_LIST, APP_LIST)
#define WDRV_WINC_UtilsProtectedSingleListHeadRemove(LIST)              TCPIP_Helper_ProtectedSingleListHeadRemove(LIST)
#define WDRV_WINC_UtilsProtectedSingleListTailAdd(LIST, NODE)           TCPIP_Helper_ProtectedSingleListTailAdd(LIST, NODE)
#define WDRV_WINC_UtilsProtectedSingleListHeadAdd(LIST, NODE)           TCPIP_Helper_ProtectedSingleListHeadAdd(LIST, NODE)

#define WDRV_WINC_UtilsStringToIPAddress(STR, ADDR)                     TCPIP_Helper_StringToIPAddress(STR, ADDR)
#define WDRV_WINC_UtilsStringToIPv6Address(STR, ADDR)                   TCPIP_Helper_StringToIPv6Address(STR, ADDR)

#define WDRV_WINC_NTP_EPOCH                                             TCPIP_NTP_EPOCH
#else
typedef struct TAG_WDRV_WINC_SGL_LIST_NODE
{
    struct TAG_WDRV_WINC_SGL_LIST_NODE*     next;       // next node in list
    void*                                   data[];     // generic payload
} WDRV_WINC_SGL_LIST_NODE;

typedef struct
{
    WDRV_WINC_SGL_LIST_NODE*    head;   // list head
    WDRV_WINC_SGL_LIST_NODE*    tail;
    int                         nNodes; // number of nodes in the list
} WDRV_WINC_SINGLE_LIST;

typedef struct
{
    WDRV_WINC_SINGLE_LIST   list;
    OSAL_SEM_HANDLE_TYPE    semaphore;
    bool                    semValid;
} WDRV_WINC_PROTECTED_SINGLE_LIST;

// *****************************************************************************
/* IPv4 Address type

  Summary:
    Definition to represent an IPv4 address

  Description:
    This type describes the IPv4 address type.

  Remarks:
    None.
*/

typedef union
{
    uint32_t Val;
    uint16_t w[2];
    uint8_t  v[4];
} WDRV_WINC_IPV4_ADDR;

// *****************************************************************************
/* IPv6 Address type

  Summary:
    Definition to represent an IPv6 address.

  Description:
    This type describes the IPv6 address type.

  Remarks:
    None.
*/

typedef union
{
    uint8_t  v[16];
    uint16_t w[8];
    uint32_t d[4];
} WDRV_WINC_IPV6_ADDR;

// *****************************************************************************
/* IP Address type

  Summary:
    Definition of the IP supported address types.

  Description:
    This type describes the supported IP address types.

  Remarks:
    8 bit value only.
*/

typedef enum
{
    /* either IPv4 or IPv6, unspecified; */
    WDRV_WINC_IP_ADDRESS_TYPE_ANY /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    /* IPv4 address type */
    WDRV_WINC_IP_ADDRESS_TYPE_IPV4,
    /* IPv6 address type */
    WDRV_WINC_IP_ADDRESS_TYPE_IPV6,
} WDRV_WINC_IP_ADDRESS_TYPE;

// *****************************************************************************
/* IP Multiple Address type

  Summary:
    Definition to represent multiple IP address types.

  Description:
    This type describes both IPv4 and IPv6 addresses.

  Remarks:
    None.
*/

typedef union
{
    WDRV_WINC_IPV4_ADDR v4;
    WDRV_WINC_IPV6_ADDR v6;
} WDRV_WINC_IP_MULTI_ADDRESS;

// *****************************************************************************
/* IP Multiple Address type

  Summary:
    Definition to represent multiple IP address types.

  Description:
    This type describes both IPv4 and IPv6 addresses and their type.

  Remarks:
    None.
*/

typedef struct
{
    WDRV_WINC_IP_ADDRESS_TYPE type;
    WDRV_WINC_IP_MULTI_ADDRESS addr;
} WDRV_WINC_IP_MULTI_TYPE_ADDRESS;

typedef union
{
    uint32_t Val;
    uint16_t w[2] __attribute__((packed));
    uint8_t  v[4];
} WDRV_WINC_UINT32_VAL;

void                        WDRV_WINC_UtilsSingleListInitialize(WDRV_WINC_SINGLE_LIST* pL);
int                         WDRV_WINC_UtilsSingleListCount(WDRV_WINC_SINGLE_LIST* pL);
WDRV_WINC_SGL_LIST_NODE*    WDRV_WINC_UtilsSingleListHeadRemove(WDRV_WINC_SINGLE_LIST* pL);
void                        WDRV_WINC_UtilsSingleListTailAdd(WDRV_WINC_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN);
void                        WDRV_WINC_UtilsSingleListAppend(WDRV_WINC_SINGLE_LIST* pDstL, WDRV_WINC_SINGLE_LIST* pAList);
void                        WDRV_WINC_UtilsSingleListHeadAdd(WDRV_WINC_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN);
WDRV_WINC_SGL_LIST_NODE*    WDRV_WINC_UtilsSingleListNodeRemove(WDRV_WINC_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN);

bool                        WDRV_WINC_UtilsProtectedSingleListInitialize(WDRV_WINC_PROTECTED_SINGLE_LIST* pL);
int                         WDRV_WINC_UtilsProtectedSingleListCount(WDRV_WINC_PROTECTED_SINGLE_LIST* pL);
void                        WDRV_WINC_UtilsProtectedSingleListAppend(WDRV_WINC_PROTECTED_SINGLE_LIST* pDstL, WDRV_WINC_SINGLE_LIST* pAList);
WDRV_WINC_SGL_LIST_NODE*    WDRV_WINC_UtilsProtectedSingleListHeadRemove(WDRV_WINC_PROTECTED_SINGLE_LIST* pL);
void                        WDRV_WINC_UtilsProtectedSingleListTailAdd(WDRV_WINC_PROTECTED_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN);
void                        WDRV_WINC_UtilsProtectedSingleListHeadAdd(WDRV_WINC_PROTECTED_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN);

bool WDRV_WINC_UtilsStringToIPAddress(const char* str, WDRV_WINC_IPV4_ADDR* addr);
bool WDRV_WINC_UtilsIPAddressToString(const WDRV_WINC_IPV4_ADDR* ipAdd, char* buff, size_t buffSize);

bool WDRV_WINC_UtilsStringToIPv6Address(const char * addStr, WDRV_WINC_IPV6_ADDR * addr);
bool WDRV_WINC_UtilsIPv6AddressToString (const WDRV_WINC_IPV6_ADDR * v6Addr, char* buff, size_t buffSize);

#define WDRV_WINC_NTP_EPOCH     2208988800ul
#endif

#ifdef __cplusplus
}
#endif

#endif /* WDRV_WINC_UTILS_H */
