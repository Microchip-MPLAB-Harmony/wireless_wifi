/*******************************************************************************
  WINC Wireless Driver Utils Source File

  File Name:
    wdrv_winc_utils.c

  Summary:
    WINC wireless driver utils implementation.

  Description:
    This provides useful utility function to the driver.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "wdrv_winc_common.h"
#include "wdrv_winc_utils.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Utils Internal Implementation
// *****************************************************************************
// *****************************************************************************

#if !defined(TCPIP_STACK_USE_IPV4) && !defined(TCPIP_STACK_USE_IPV6)

static uint8_t btohexa_high(uint8_t b)
{
    b >>= 4;
    return (uint8_t)((b > 0x9U) ? (b + STR_LIT_U('a') - 10U) : (b + STR_LIT_U('0')));
}

static uint8_t btohexa_low(uint8_t b)
{
    b &= 0x0FU;
    return (uint8_t)((b > 9U) ? (b + STR_LIT_U('a') - 10U) : (b + STR_LIT_U('0')));
}
#endif /* TCPIP_STACK_USE_IPV4 || TCPIP_STACK_USE_IPV6 */

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Utils Implementation
// *****************************************************************************
// *****************************************************************************

#if !defined(TCPIP_STACK_USE_IPV4) && !defined(TCPIP_STACK_USE_IPV6)
/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsSingleListInitialize(WDRV_WINC_SINGLE_LIST* pL)
{
    pL->head = NULL;
    pL->tail = NULL;
    pL->nNodes = 0;
}

/*****************************************************************************
 *
 *****************************************************************************/
int WDRV_WINC_UtilsSingleListCount(const WDRV_WINC_SINGLE_LIST* pL)
{
    return pL->nNodes;
}

/*****************************************************************************
 *
 *****************************************************************************/
WDRV_WINC_SGL_LIST_NODE* WDRV_WINC_UtilsSingleListHeadRemove(WDRV_WINC_SINGLE_LIST* pL)
{
    WDRV_WINC_SGL_LIST_NODE* pN = pL->head;
    if (NULL != pN)
    {
        if (pL->head == pL->tail)
        {
            pL->head = NULL;
            pL->tail = NULL;
        }
        else
        {
            pL->head = pN->next;
        }
        pL->nNodes--;
    }

    return pN;
}

/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsSingleListTailAdd(WDRV_WINC_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN)
{
    pN->next = NULL;
    if (pL->tail == NULL)
    {
        pL->head = pN;
        pL->tail = pN;
    }
    else
    {
        pL->tail->next = pN;
        pL->tail = pN;
    }
    pL->nNodes++;
}

/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsSingleListAppend(WDRV_WINC_SINGLE_LIST* pDstL, WDRV_WINC_SINGLE_LIST* pAList)
{
    WDRV_WINC_SGL_LIST_NODE* pN;

    pN = WDRV_WINC_UtilsSingleListHeadRemove(pAList);
    while (NULL != pN)
    {
        WDRV_WINC_UtilsSingleListTailAdd(pDstL, pN);

        pN = WDRV_WINC_UtilsSingleListHeadRemove(pAList);
    }
}

/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsSingleListHeadAdd(WDRV_WINC_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN)
{
    pN->next = pL->head;
    pL->head = pN;
    if (pL->tail == NULL)
    {
        // empty list
        pL->tail = pN;
    }
    pL->nNodes++;
}

/*****************************************************************************
 *
 *****************************************************************************/
WDRV_WINC_SGL_LIST_NODE* WDRV_WINC_UtilsSingleListNodeRemove(WDRV_WINC_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN)
{
    if (pN == pL->head)
    {
        (void)WDRV_WINC_UtilsSingleListHeadRemove(pL);
    }
    else
    {
        WDRV_WINC_SGL_LIST_NODE* prev;
        for(prev = pL->head; ((prev != NULL) && (prev->next != pN)); prev = prev->next)
        {
        }

        if (prev == NULL)
        {
            // no such node
            return NULL;
        }
        // found it
        prev->next = pN->next;
        if (pN == pL->tail)
        {
            pL->tail = prev;
        }
        pL->nNodes--;
    }

    return pN;
}

/*****************************************************************************
 *
 *****************************************************************************/
bool WDRV_WINC_UtilsProtectedSingleListInitialize(WDRV_WINC_PROTECTED_SINGLE_LIST* pL)
{
    WDRV_WINC_UtilsSingleListInitialize(&pL->list);
    pL->semValid = (OSAL_SEM_Create(&pL->semaphore, OSAL_SEM_TYPE_BINARY, 1, 1) == OSAL_RESULT_TRUE);
    return pL->semValid;
}

/*****************************************************************************
 *
 *****************************************************************************/
int WDRV_WINC_UtilsProtectedSingleListCount(WDRV_WINC_PROTECTED_SINGLE_LIST* pL)
{
    return pL->list.nNodes;
}

/*****************************************************************************
 *
 *****************************************************************************/
void  WDRV_WINC_UtilsProtectedSingleListAppend(WDRV_WINC_PROTECTED_SINGLE_LIST* pDstL, WDRV_WINC_SINGLE_LIST* pAList)
{
    if (true == pDstL->semValid)
    {
        if (OSAL_SEM_Pend(&pDstL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
        {
            //SYS_DEBUG LOG
        }
        WDRV_WINC_UtilsSingleListAppend(&pDstL->list, pAList);
        if (OSAL_SEM_Post(&pDstL->semaphore) != OSAL_RESULT_TRUE)
        {
            //SYS_DEBUG LOG
        }
    }
}

/*****************************************************************************
 *
 *****************************************************************************/
WDRV_WINC_SGL_LIST_NODE* WDRV_WINC_UtilsProtectedSingleListHeadRemove(WDRV_WINC_PROTECTED_SINGLE_LIST* pL)
{

    if (true == pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
        {
            //SYS_DEBUG LOG
        }
        WDRV_WINC_SGL_LIST_NODE * ret = WDRV_WINC_UtilsSingleListHeadRemove(&pL->list);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_TRUE)
        {
            //SYS_DEBUG LOG
        }
        return ret;
    }

    return NULL;
}

/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsProtectedSingleListTailAdd(WDRV_WINC_PROTECTED_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN)
{
    if (true == pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
        {
            //SYS_DEBUG LOG
        }
        WDRV_WINC_UtilsSingleListTailAdd(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_TRUE)
        {
            //SYS_DEBUG LOG
        }
    }

}

/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsProtectedSingleListHeadAdd(WDRV_WINC_PROTECTED_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN)
{
    if (true == pL->semValid)
    {
        if (OSAL_SEM_Pend(&pL->semaphore, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
        {
            //SYS_DEBUG LOG
        }
        WDRV_WINC_UtilsSingleListHeadAdd(&pL->list, pN);
        if (OSAL_SEM_Post(&pL->semaphore) != OSAL_RESULT_TRUE)
        {
            //SYS_DEBUG LOG
        }
    }
}

/*****************************************************************************
 *
 *****************************************************************************/
bool WDRV_WINC_UtilsStringToIPAddress(const char* str, WDRV_WINC_IPV4_ADDR* addr)
{
    WDRV_WINC_UINT32_VAL dwVal;
    WDRV_WINC_IPV4_ADDR convAddr;
    uint8_t charLen, currentOctet;
    char c;

    if (NULL != addr)
    {
        addr->Val = 0;
    }

    if ((NULL == str) || (strlen(str) == 0U))
    {
        return false;
    }

    charLen = 0;
    currentOctet = 0;
    dwVal.Val = 0;

    while ('\0' != *str)
    {
        c = *str++;

        if (currentOctet > 3u)
        {
            break;
        }

        // Validate the character is a numerical digit or dot, depending on location
        if (charLen == 0u)
        {
            if ((c > '9') || (c < '0'))
            {
                return false;
            }
        }
        else if (charLen == 3u)
        {
            if (c != '.')
            {
                return false;
            }

            if (dwVal.Val > 0x00020505UL)
            {
                return false;
            }

            convAddr.v[currentOctet++] = (dwVal.v[2]*(100U)) + (dwVal.v[1]*(10U)) + dwVal.v[0];
            charLen = 0;
            dwVal.Val = 0;
            continue;
        }
        else
        {
            if (c == '.')
            {
                if (dwVal.Val > 0x00020505UL)
                {
                    return false;
                }

                convAddr.v[currentOctet++] = (dwVal.v[2]*(100U)) + (dwVal.v[1]*(10U)) + dwVal.v[0];
                charLen = 0;
                dwVal.Val = 0;
                continue;
            }
            if ((c > '9') || (c < '0'))
            {
                return false;
            }
        }

        charLen++;
        dwVal.Val <<= 8;
        dwVal.v[0] = (c - '0');
    }

    c = *str;

    // Make sure the very last character is a valid termination character
    // (i.e., not more hostname, which could be legal and not an IP
    // address as in "10.5.13.233.picsaregood.com"
    if ((currentOctet != 3U) || ((c != '\0') && (c != '/') && (c != '\r') && (c != '\n') && (c != ' ') && (c != '\t') && (c != ':')))
    {
        return false;
    }

    // Verify and convert the last octet and return the result
    if (dwVal.Val > 0x00020505UL)
    {
        return false;
    }

    convAddr.v[3] = (dwVal.v[2]*(100U)) + (dwVal.v[1]*(10U)) + dwVal.v[0];

    if (NULL != addr)
    {
        addr->Val = convAddr.Val;
    }

    return true;
}

/*****************************************************************************
 *
 *****************************************************************************/
bool WDRV_WINC_UtilsIPAddressToString(const WDRV_WINC_IPV4_ADDR* ipAdd, char* buff, size_t buffSize)
{
    if ((NULL != ipAdd) && (NULL != buff))
    {
        size_t len;
        char tempBuff[20];  // enough to hold largest IPv4 address string

        len = sprintf(tempBuff, "%d.%d.%d.%d", ipAdd->v[0], ipAdd->v[1], ipAdd->v[2], ipAdd->v[3]) + 1;
        if (buffSize >= len)
        {
            (void)strcpy(buff, tempBuff);
            return true;
        }
    }

    return false;
}

/*****************************************************************************
 *
 *****************************************************************************/
bool WDRV_WINC_UtilsStringToIPv6Address(const char * addStr, WDRV_WINC_IPV6_ADDR * addr)
{
    int shiftIndex = -1;
    uint8_t subString[5];
    size_t len;
    int conv_base = 16;
    int j;
    unsigned char c;
    int currentWord;
    char *endPtr;
    unsigned char *str;
    WDRV_WINC_IPV6_ADDR   convAddr;
    char str_buff[64 + 1];     // enough space for longest address: 1111:2222:3333:4444:5555:6666:192.250.250.250

    if (NULL != addr)
    {
        (void)memset(addr, 0, sizeof(*addr));
    }

    if (NULL == addStr)
    {
        return false;
    }

    len = strlen(addStr);

    if (0U == len)
    {
        return false;
    }

    (void)memset(&convAddr, 0, sizeof(convAddr));

    while (0 != isspace((int)*addStr))
    {
        // skip leading space
        addStr++;
        len--;
    }
    while (0 != isspace((int)*(addStr + len - 1U)))
    {
        // skip trailing space
        len--;
    }

    if (len > sizeof(str_buff) - 1U)
    {
        // not enough room to store
        return false;
    }

    (void)strncpy(str_buff, addStr, len);
    str_buff[len] = '\0';
    str = (unsigned char*)str_buff;

    if (*str == STR_LIT_U('['))
    {
        // match the end ]
        if (str[len - 1U] != STR_LIT_U(']'))
        {
            // bracket mismatch
            return false;
        }
        str[len - 1U] = STR_LIT_U('\0');   // delete trailing ]
        len--;
        str++;  // skip leading [
        len--;
    }

    currentWord = 0;
    while (0 != isspace((int)*str))
    {
        // skip leading space
        str++;
        len--;
    }
    endPtr = (char*)&str[len];
    while (0 != isspace((int)*(endPtr - 1)))
    {
        // skip trailing space
        endPtr--;
    }
    *endPtr = '\0';

    if (*str == STR_LIT_U(':'))
    {
        if (*++str != STR_LIT_U(':'))
        {
            // cannot start with stray :
            return false;
        }
        str++;
        shiftIndex = 0;
    }

    if (0 == isxdigit((int)*str))
    {
        return false;
    }

    c = *str++;
    while ((c != STR_LIT_U(':')) && (c != STR_LIT_U('\0')) && (c != STR_LIT_U('.')) && (c != STR_LIT_U('/')) && (c != STR_LIT_U('\r')) && (c != STR_LIT_U('\n')) && (c != STR_LIT_U(' ')) && (c != STR_LIT_U('\t')))
    {
        uint16_t convertedValue;

        j = 0;
        while ((c != STR_LIT_U(':')) && (c != STR_LIT_U('\0')) && (c != STR_LIT_U('.')) && (c != STR_LIT_U('/')) && (c != STR_LIT_U('\r')) && (c != STR_LIT_U('\n')) && (c != STR_LIT_U(' ')) && (c != STR_LIT_U('\t')))
        {
            if (j == 4)
            {
                return false;
            }

            subString[j++] = (uint8_t)c;
            c = *str++;
        }
        subString[j] = 0;

        if (c == STR_LIT_U('.'))
        {
            conv_base = 10;
        }
        else if ((c == STR_LIT_U(':')) && (conv_base == 10))
        {
            return false;
        }
        else
        {
            /* Do nothing. */
        }

        errno = 0;
        convertedValue = (uint16_t)strtol((const char *)subString, &endPtr, conv_base);
        if (0 != errno)
        {
            return false;
        }

        if ((convertedValue == 0U) && (endPtr != (char*)subString + j))
        {
            // could not convert all data in there
            return false;
        }

        convAddr.w[currentWord++] = WDRV_WINC_UtilsHToNS(convertedValue);

        if (c == STR_LIT_U('\0'))
        {
            // end of stream
            break;
        }

        if (c == STR_LIT_U(':'))
        {
            if (*str == STR_LIT_U(':'))
            {
                // Double colon - pad with zeros here
                if (shiftIndex == -1)
                {
                    shiftIndex = currentWord;
                }
                else
                {
                    // Can't have two double colons
                    return false;
                }
                c = *str++;
            }
        }

        if (c == STR_LIT_U(','))
        {
            return false;
        }

        c = *str++;
    }

    if ((currentWord > 8) || ((currentWord < 8) && (shiftIndex == -1)))
    {
        // more than 8 words entered or less, but no ::
        return false;
    }

    if (shiftIndex != -1)
    {
        int i;

        i = 7;
        for (j=(currentWord - 1); j >= shiftIndex; j--)
        {
            convAddr.w[i--] = convAddr.w[j];
        }

        for (i=shiftIndex; i <= (7 - (currentWord - shiftIndex)); i++)
        {
            convAddr.w[i] = 0x0000;
        }
    }

    if (NULL != addr)
    {
        (void)memcpy((uint8_t*)addr, convAddr.v, sizeof(*addr));
    }

    return true;
}

/*****************************************************************************
 *
 *****************************************************************************/
bool WDRV_WINC_UtilsIPv6AddressToString (const WDRV_WINC_IPV6_ADDR * v6Addr, char* buff, size_t buffSize)
{
    if ((NULL != v6Addr) && (NULL != buff) && (buffSize >= 41U))
    {
        uint8_t i;
        char* str = buff;

        for (i = 0; i < 8U; i++)
        {
            bool j;
            char k;

            j = false;
            k = (char)btohexa_high(v6Addr->v[(i<<1)]);
            if (k != '0')
            {
                j = true;
                *str++ = k;
            }
            k = (char)btohexa_low(v6Addr->v[(i<<1)]);
            if ((k != '0') || (j == true))
            {
                j = true;
                *str++ = k;
            }
            k = (char)btohexa_high(v6Addr->v[1U + (i<<1)]);
            if ((k != '0') || (j == true))
            {
                *str++ = k;
            }
            k = (char)btohexa_low(v6Addr->v[1U + (i<<1)]);
            *str++ = k;
            if (i != 7U)
            {
                *str++ = ':';
            }
        }
        *str = '\0';

        return true;
    }

    return false;
}

#ifndef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
/*****************************************************************************
 *
 *****************************************************************************/
uint32_t WDRV_WINC_UtilsNToHL(uint32_t N)
{
    uint8_t *pN = (uint8_t*)&N;
    uint32_t L;

    L = *pN++;
    L = (L << 8) | *pN++;
    L = (L << 8) | *pN++;
    L = (L << 8) | *pN;

    return L;
}

/*****************************************************************************
 *
 *****************************************************************************/
uint16_t WDRV_WINC_UtilsNToHS(uint16_t N)
{
    uint8_t *pN = (uint8_t*)&N;
    uint16_t L;

    L = *pN++;
    L = (L << 8) | *pN;

    return L;
}
#endif /* __BYTE_ORDER__ */
#endif /* WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS */

#endif /* TCPIP_STACK_USE_IPV4 || TCPIP_STACK_USE_IPV6 */
