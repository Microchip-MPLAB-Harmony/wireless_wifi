/*******************************************************************************
  WINC Wireless Driver Utils Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_utils.h

  Summary:
    WINC wireless driver utils interface.

  Description:
    This file provides utility functions for the WINC driver.
 *******************************************************************************/

/*
Copyright (C) 2024-25 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#ifndef WDRV_WINC_UTILS_H
#define WDRV_WINC_UTILS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "osal/osal.h"
#include "conf_winc_dev.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Utils Defines
// *****************************************************************************
// *****************************************************************************

#ifdef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
/* If NC Berkeley layer is present use it's functions to convert. */
#define WDRV_WINC_UtilsNToHL(N)                     ntohl(N)
#define WDRV_WINC_UtilsHToNL(N)                     htonl(N)
#define WDRV_WINC_UtilsNToHS(N)                     ntohs(N)
#define WDRV_WINC_UtilsHToNS(N)                     htons(N)
#else
/* If NC Berkeley layer is not present use local functions to convert. */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define WDRV_WINC_UtilsHToNL(N)                     WDRV_WINC_UtilsNToHL(N)
#define WDRV_WINC_UtilsHToNS(N)                     WDRV_WINC_UtilsNToHS(N)
#else
#define WDRV_WINC_UtilsNToHL(N)                     (N)
#define WDRV_WINC_UtilsHToNL(N)                     (N)
#define WDRV_WINC_UtilsNToHS(N)                     (N)
#define WDRV_WINC_UtilsHToNS(N)                     (N)
#endif
#endif

/* NTP epoch UTC value. */
#define WDRV_WINC_NTP_EPOCH                         2208988800UL

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Utils Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Single List Node Type

  Summary:
    Definition of single link list node.

  Description:
    This type describes a node structure within a singly link list.

  Remarks:
    None.
*/

typedef struct TAG_WDRV_WINC_SGL_LIST_NODE
{
    /* Pointer to next node in list. */
    struct TAG_WDRV_WINC_SGL_LIST_NODE*     next;

    /* Generic payload pointer. */
    void*                                   data[];
} WDRV_WINC_SGL_LIST_NODE;

// *****************************************************************************
/* Single List Type

  Summary:
    Definition of single link list.

  Description:
    This type describes a singly linked list.

  Remarks:
    None.
*/

typedef struct
{
    /* Head node of linked list. */
    WDRV_WINC_SGL_LIST_NODE*    head;

    /* Tail node of linked list. */
    WDRV_WINC_SGL_LIST_NODE*    tail;

    /* Number of nodes in the list. */
    int                         nNodes;
} WDRV_WINC_SINGLE_LIST;

// *****************************************************************************
/* Protected Single List Type

  Summary:
    Definition of protected single link list node.

  Description:
    This type describes a singly linked list protected for multi-thread access.

  Remarks:
    None.
*/

typedef struct
{
    /* Unprotected singly linked list structure. */
    WDRV_WINC_SINGLE_LIST   list;

    /* Semaphore to control access. */
    OSAL_SEM_HANDLE_TYPE    semaphore;

    /* Flag indicating if semaphore has been initialized. */
    bool                    semValid;
} WDRV_WINC_PROTECTED_SINGLE_LIST;

// *****************************************************************************
/* IPv4 Address Type

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
/* IPv6 Address Type

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
/* IP Address Type

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
    WDRV_WINC_IP_ADDRESS_TYPE_ANY = 0,
    /* IPv4 address type */
    WDRV_WINC_IP_ADDRESS_TYPE_IPV4,
    /* IPv6 address type */
    WDRV_WINC_IP_ADDRESS_TYPE_IPV6,
} WDRV_WINC_IP_ADDRESS_TYPE;

// *****************************************************************************
/* IP Multiple Address Type

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
/* IP Multiple Address Type

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

// *****************************************************************************
/* UINT32 Value Access Type

  Summary:
    Definition of a structure to convert to and from UINT32 types.

  Description:
    This type describes a union to convert between UINT32, UINT16 and UINT8
    representation of a 32-bit value.

  Remarks:
    None.
*/

typedef union
{
    /* UINT32 storage. */
    uint32_t Val;

    /* UINT16 storage. */
    uint16_t w[2] __attribute__((packed));

    /* UINT8 storage. */
    uint8_t  v[4];
} WDRV_WINC_UINT32_VAL;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Utils Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

void                        WDRV_WINC_UtilsSingleListInitialize(WDRV_WINC_SINGLE_LIST* pL);
int                         WDRV_WINC_UtilsSingleListCount(const WDRV_WINC_SINGLE_LIST* pL);
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

#ifndef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
/* If NC Berkeley layer is not present use local functions to convert. */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
uint32_t WDRV_WINC_UtilsNToHL(uint32_t N);
uint16_t WDRV_WINC_UtilsNToHS(uint16_t N);
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_UTILS_H */
