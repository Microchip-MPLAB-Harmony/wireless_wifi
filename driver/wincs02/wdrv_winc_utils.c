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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "wdrv_winc_common.h"
#include "wdrv_winc_utils.h"

#if !defined(TCPIP_STACK_USE_IPV4) && !defined(TCPIP_STACK_USE_IPV6)

static uint8_t btohexa_high(uint8_t b)
{
    b >>= 4;
    return (b>0x9u) ? b+'a'-10:b+'0';
}

static uint8_t btohexa_low(uint8_t b)
{
    b &= 0x0F;
    return (b>9u) ? b+'a'-10:b+'0';
}

/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsSingleListInitialize(WDRV_WINC_SINGLE_LIST* pL)
{
    pL->head = pL->tail = 0;
    pL->nNodes = 0;
}

/*****************************************************************************
 *
 *****************************************************************************/
int WDRV_WINC_UtilsSingleListCount(WDRV_WINC_SINGLE_LIST* pL)
{
    return pL->nNodes;
}

/*****************************************************************************
 *
 *****************************************************************************/
WDRV_WINC_SGL_LIST_NODE* WDRV_WINC_UtilsSingleListHeadRemove(WDRV_WINC_SINGLE_LIST* pL)
{
    WDRV_WINC_SGL_LIST_NODE* pN = pL->head;
    if(pN)
    {
        if(pL->head == pL->tail)
        {
            pL->head = pL->tail = 0;
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
    pN->next = 0;
    if(pL->tail == 0)
    {
        pL->head = pL->tail = pN;
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
    while((pN = WDRV_WINC_UtilsSingleListHeadRemove(pAList)))
    {
        WDRV_WINC_UtilsSingleListTailAdd(pDstL, pN);
    }
}

/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsSingleListHeadAdd(WDRV_WINC_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN)
{
    pN->next = pL->head;
    pL->head = pN;
    if(pL->tail == 0)
    {  // empty list
        pL->tail = pN;
    }
    pL->nNodes++;
}

/*****************************************************************************
 *
 *****************************************************************************/
WDRV_WINC_SGL_LIST_NODE* WDRV_WINC_UtilsSingleListNodeRemove(WDRV_WINC_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN)
{
    if(pN == pL->head)
    {
        WDRV_WINC_UtilsSingleListHeadRemove(pL);
    }
    else
    {
        WDRV_WINC_SGL_LIST_NODE* prev;
        for(prev = pL->head; prev != 0 && prev->next != pN; prev = prev->next);
        if(prev == 0)
        {   // no such node
            return 0;
        }
        // found it
        prev->next = pN->next;
        if(pN == pL->tail)
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
    if(pDstL->semValid)
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

    if(pL->semValid)
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

    return 0;
}

/*****************************************************************************
 *
 *****************************************************************************/
void WDRV_WINC_UtilsProtectedSingleListTailAdd(WDRV_WINC_PROTECTED_SINGLE_LIST* pL, WDRV_WINC_SGL_LIST_NODE* pN)
{
    if(pL->semValid)
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
    if(pL->semValid)
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
    WDRV_WINC_IPV4_ADDR   convAddr;
    uint8_t i, charLen, currentOctet;

    if(addr)
    {
        addr->Val = 0;
    }

    if(str == 0 || strlen(str) == 0)
    {
        return false;
    }

    charLen = 0;
    currentOctet = 0;
    dwVal.Val = 0;

    while((i = *str++))
    {
        if(currentOctet > 3u)
            break;

        i -= '0';


        // Validate the character is a numerical digit or dot, depending on location
        if(charLen == 0u)
        {
            if(i > 9u)
                return false;
        }
        else if(charLen == 3u)
        {
            if(i != (uint8_t)('.' - '0'))
                return false;

            if(dwVal.Val > 0x00020505ul)
                return false;

            convAddr.v[currentOctet++] = dwVal.v[2]*((uint8_t)100) + dwVal.v[1]*((uint8_t)10) + dwVal.v[0];
            charLen = 0;
            dwVal.Val = 0;
            continue;
        }
        else
        {
            if(i == (uint8_t)('.' - '0'))
            {
                if(dwVal.Val > 0x00020505ul)
                    return false;

                convAddr.v[currentOctet++] = dwVal.v[2]*((uint8_t)100) + dwVal.v[1]*((uint8_t)10) + dwVal.v[0];
                charLen = 0;
                dwVal.Val = 0;
                continue;
            }
            if(i > 9u)
                return false;
        }

        charLen++;
        dwVal.Val <<= 8;
        dwVal.v[0] = i;
    }

    // Make sure the very last character is a valid termination character
    // (i.e., not more hostname, which could be legal and not an IP
    // address as in "10.5.13.233.picsaregood.com"
    if(currentOctet != 3 || (i != 0u && i != '/' && i != '\r' && i != '\n' && i != ' ' && i != '\t' && i != ':'))
        return false;

    // Verify and convert the last octet and return the result
    if(dwVal.Val > 0x00020505ul)
        return false;

    convAddr.v[3] = dwVal.v[2]*((uint8_t)100) + dwVal.v[1]*((uint8_t)10) + dwVal.v[0];

    if(addr)
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
    if(ipAdd && buff)
    {
        size_t len;
        char tempBuff[20];  // enough to hold largest IPv4 address string

        sprintf(tempBuff, "%d.%d.%d.%d", ipAdd->v[0], ipAdd->v[1], ipAdd->v[2], ipAdd->v[3]);
        len = strlen(tempBuff) + 1;
        if(buffSize >= len)
        {
            strcpy(buff, tempBuff);
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
    uint8_t shiftIndex = 0xFF;
    uint8_t subString[5];
    uint16_t convertedValue;
    int len;
    int conv_base = 16;
    uint8_t i, j;
    uint8_t currentWord;
    char * endPtr;
    char*  str;
    WDRV_WINC_IPV6_ADDR   convAddr;
    char   str_buff[64 + 1];     // enough space for longest address: 1111:2222:3333:4444:5555:6666:192.250.250.250

    if(addr)
    {
        memset(addr, 0, sizeof(*addr));
    }

    if(addStr == 0 || (len = strlen(addStr)) == 0)
    {
        return false;
    }

    memset(convAddr.v, 0, sizeof(convAddr));

    while(isspace(*addStr))
    {   // skip leading space
        addStr++;
        len--;
    }
    while(isspace(*(addStr + len - 1)))
    {   // skip trailing space
        len--;
    }

    if(len > sizeof(str_buff) - 1)
    {   // not enough room to store
        return false;
    }

    strncpy(str_buff, addStr, len);
    str_buff[len] = 0;
    str = str_buff;

    if (*str == '[')
    {   // match the end ]
        if(str[len - 1] != ']')
        {   // bracket mismatch
            return false;
        }
        str[len - 1] = 0;   // delete trailing ]
        len--;
        str++;  // skip leading [
        len--;
    }

    currentWord = 0;
    while(isspace(*str))
    {   // skip leading space
        str++;
        len--;
    }
    endPtr = str + len;
    while(isspace(*(endPtr - 1)))
    {   // skip trailing space
        endPtr--;
    }
    *endPtr = 0;

    if(*str == ':')
    {
        if(*++str != ':')
        {   // cannot start with stray :
            return false;
        }
        str++;
        shiftIndex = 0;
    }

    if(!isxdigit(*str))
    {
        return false;
    }

    i = *str++;
    while (i != ':' && i != 0u && i != '.' && i != '/' && i != '\r' && i != '\n' && i != ' ' && i != '\t')
    {
        j = 0;
        while (i != ':' && i != 0u && i != '.' && i != '/' && i != '\r' && i != '\n' && i != ' ' && i != '\t')
        {
            if (j == 4)
                return false;

            subString[j++] = i;
            i = *str++;
        }
        subString[j] = 0;

        if(i == '.')
        {
            conv_base = 10;
        }
        else if(i == ':' && conv_base == 10)
        {
            return false;
        }

        convertedValue = (uint16_t)strtol((const char *)subString, &endPtr, conv_base);
        if(convertedValue == 0 && endPtr != (char*)subString + j)
        {   // could not convert all data in there
            return false;
        }

        convAddr.w[currentWord++] = htons(convertedValue);

        if(i == 0)
        {   // end of stream
            break;
        }

        if (i == ':')
        {
            if (*str == ':')
            {
                // Double colon - pad with zeros here
                if (shiftIndex == 0xFF)
                {
                    shiftIndex = currentWord;
                }
                else
                {
                    // Can't have two double colons
                    return false;
                }
                i = *str++;
            }
        }

        if (i == ',')
        {
            return false;
        }

        i = *str++;
    }

    if(currentWord > 8 || (currentWord < 8 && shiftIndex == 0xff))
    {   // more than 8 words entered or less, but no ::
        return false;
    }

    if (shiftIndex != 0xFF)
    {
        for (i = 7, j = currentWord - 1; (int8_t)j >= (int8_t)shiftIndex; i--, j--)
        {
            convAddr.w[i] = convAddr.w[j];
        }
        for (i = shiftIndex; i <= 7 - (currentWord - shiftIndex); i++)
        {
            convAddr.w[i] = 0x0000;
        }
    }

    if(addr)
    {
        memcpy(addr, convAddr.v, sizeof(*addr));
    }

    return true;
}

/*****************************************************************************
 *
 *****************************************************************************/
bool WDRV_WINC_UtilsIPv6AddressToString (const WDRV_WINC_IPV6_ADDR * v6Addr, char* buff, size_t buffSize)
{
    if(v6Addr && buff && buffSize >= 41)
    {
        uint8_t i, j;
        char k;
        char* str = buff;

        for (i = 0; i < 8; i++)
        {
            j = false;
            k = btohexa_high(v6Addr->v[(i<<1)]);
            if (k != '0')
            {
                j = true;
                *str++ = k;
            }
            k = btohexa_low(v6Addr->v[(i<<1)]);
            if (k != '0' || j == true)
            {
                j = true;
                *str++ = k;
            }
            k = btohexa_high(v6Addr->v[1 + (i<<1)]);
            if (k != '0' || j == true)
                *str++ = k;
            k = btohexa_low(v6Addr->v[1 + (i<<1)]);
            *str++ = k;
            if (i != 7)
                *str++ = ':';
        }
        *str = 0;

        return true;
    }

    return false;
}


#endif
