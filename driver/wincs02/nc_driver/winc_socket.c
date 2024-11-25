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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#include "winc_socket.h"
#include "winc_dev.h"
#include "winc_cmd_req.h"
#include "winc_debug.h"

/* Number of sockets supported. */
#ifndef WINC_SOCK_NUM_SOCKETS
#define WINC_SOCK_NUM_SOCKETS               10U
#endif

/* Buffer size to allocate per socket when in use. */
#ifndef WINC_SOCK_BUF_RX_SZ
#define WINC_SOCK_BUF_RX_SZ                (MAX_SOCK_PAYLOAD_SZ*5)
#endif

#ifndef WINC_SOCK_BUF_TX_SZ
#define WINC_SOCK_BUF_TX_SZ                (MAX_SOCK_PAYLOAD_SZ*5)
#endif

#ifndef WINC_SOCK_BUF_RX_PKT_BUF_NUM
#define WINC_SOCK_BUF_RX_PKT_BUF_NUM        5
#endif

#ifndef WINC_SOCK_BUF_TX_PKT_BUF_NUM
#define WINC_SOCK_BUF_TX_PKT_BUF_NUM        5
#endif

/* Convert a socket handle into pointer into the socket array. */
#define WINC_SOCK_HANDLE_TO_PTR(HANDLE)     (void*)(((uintptr_t)wincSockets & ~INT_MAX) | (HANDLE))

/* Convert a socket array pointer to a handle. */
#define WINC_SOCK_PTR_TO_HANDLE(PTR)        (((int)(PTR)) & INT_MAX)

/*****************************************************************************
  Description:
    End point union.

  Remarks:
    Combines IPv4 and IPv6 with family and port common to both exposed.

 *****************************************************************************/

typedef union
{
    struct
    {
        sa_family_t             sin_family;
        in_port_t               sin_port;
    };
    struct sockaddr_in          v4;
    struct sockaddr_in6         v6;
} WINC_SOCK_END_PT;

/*****************************************************************************
  Description:
    UDP packet structure.

  Remarks:
    Associated with a UDP datagram being held in the socket data buffer FIFO.

 *****************************************************************************/

typedef struct
{
    WINC_SOCK_END_PT            endPt;
    uint16_t                    pktLength;
    uint16_t                    pktOffset;
} WINC_SOCK_UDP_PKT;

/*****************************************************************************
  Description:
    UDP packet buffer.

  Remarks:
    FIFO buffer within socket buffer to track datagrams.

 *****************************************************************************/

typedef struct
{
    uint8_t                     numPkts;
    uint8_t                     inIdx;
    uint8_t                     outIdx;
    uint8_t                     pktDepth;
    WINC_SOCK_UDP_PKT           pkts[];
} WINC_SOCK_UDP_PKT_BUFFER;

/*****************************************************************************
  Description:
    Socket data buffer.

  Remarks:
    Stores data for a socket, with optional UDP packet buffer to track datagrams.

 *****************************************************************************/

typedef struct
{
    uint16_t                    inOffset;
    uint16_t                    outOffset;
    uint16_t                    length;
    uint16_t                    totalSize;
    uint16_t                    outstandingDataLen;
    WINC_SOCK_UDP_PKT_BUFFER    *pUdpPktBuffers;
    uint8_t                     *pData;
} WINC_SOCK_BUFFER;

/*****************************************************************************
  Description:
    Socket context.

  Remarks:
    Complete management context for a single TCP/UDP socket.

 *****************************************************************************/

typedef struct
{
    struct
    {
        bool                    inUse:1;
        bool                    listening:1;
        bool                    connected:1;
        bool                    accepted:1;
        bool                    newRecvData:1;
        unsigned int            readMode:2;
        bool                    seqNumUpdateSent:1;
        bool                    tlsPending;
    };

    uint16_t                    sockId;
    uint8_t                     type;
    uint8_t                     protocol;
    uint8_t                     domain;
    WINC_SOCK_END_PT            localEndPt;
    WINC_SOCK_END_PT            remoteEndPt;
    uint16_t                    unAckedSeqNum;

    union
    {
        uint16_t                pendingRecvDataLen;
        uint16_t                nextSeqNum;
    };

    uint8_t                     udpUnackedPktBufs;
    WINC_SOCK_BUFFER            recvBuffer;
    WINC_SOCK_BUFFER            sendBuffer;

    WINC_CONF_LOCK_STORAGE(accessMutex);
} WINC_SOCK_CTX;

/*****************************************************************************
  Description:
    DNS request structure.

  Remarks:
    Stores a DNS request.

 *****************************************************************************/

typedef struct
{
    size_t                      hostNameLen;
    struct addrinfo             *pRes;
    uint8_t                     recordType;
    uint8_t                     hostName[];
} WINC_DNS_REQ;

/*****************************************************************************
  Description:
    Memory slab allocator context.

  Remarks:
    Store the location, geometry and index of the slab allocator.

 *****************************************************************************/

typedef struct
{
    uint8_t                     *pRootAddr;
    size_t                      slabSize;
    int8_t                      numSlabs;
    int8_t                      slabIdxs[];
} WINC_SOCK_SLAB_CTX;

/******************************************************************************/

/* Forward declaration of command response callback handler. */
static void sockCmdRspCallbackHandler(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle, WINC_DEV_CMDREQ_EVENT_TYPE event, uintptr_t eventArg);

/******************************************************************************/

/* WINC device handle. */
static WINC_DEVICE_HANDLE           wincDevHandle;

/* Socket context structure array. */
static WINC_SOCK_CTX                wincSockets[WINC_SOCK_NUM_SOCKETS];

/* Socket event callback function pointer. */
static WINC_SOCKET_EVENT_CALLBACK   pfSocketEventCallback;

/* Socket event callback function context to be passed back. */
static uintptr_t                    socketEventCallbackContext;

/* Current outstanding DNS request pointer. */
static WINC_DNS_REQ                 *pCurrentDnsRequest;

/* Pointer to memory slab allocator context. */
static WINC_SOCK_SLAB_CTX           *pSlabAllocCtx;

/* Copy of socket initialisation data. */
static WINC_SOCKET_INIT_TYPE        initData;

/*****************************************************************************
  Description:
    Slab memory allocator initialisation.

  Parameters:
    slabSize - Size of each slab or memory.
    numSlabs - Number of slabs to allocate.

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void slabInit(size_t slabSize, int8_t numSlabs)
{
    if (NULL == initData.pfMemAlloc)
    {
        return;
    }

    if ((0U == slabSize) || (numSlabs <= 0))
    {
        pSlabAllocCtx = NULL;
        return;
    }

    pSlabAllocCtx = initData.pfMemAlloc(sizeof(WINC_SOCK_SLAB_CTX) + (size_t)numSlabs + (slabSize * (size_t)numSlabs));

    if (NULL == pSlabAllocCtx)
    {
        return;
    }

    (void)memset(pSlabAllocCtx, 0xff, sizeof(WINC_SOCK_SLAB_CTX) + (size_t)numSlabs);

    pSlabAllocCtx->pRootAddr = &((uint8_t*)pSlabAllocCtx)[sizeof(WINC_SOCK_SLAB_CTX) + (size_t)numSlabs];
    pSlabAllocCtx->slabSize  = slabSize;
    pSlabAllocCtx->numSlabs  = numSlabs;
}

/*****************************************************************************
  Description:
    Allocate memory from the slab allocator.

  Parameters:
    size - Size of memory to allocate

  Returns:
    Pointer to allocated slab, or NULL or error.

  Remarks:

 *****************************************************************************/

static void* slabAlloc(size_t size)
{
    int8_t i;
    int8_t consecFree;
    int8_t slabIdx;
    int8_t reqSlabs;

    if ((NULL == pSlabAllocCtx) || (NULL == pSlabAllocCtx->pRootAddr))
    {
        /* Slab allocator isn't present, use the basic memory allocator. */
        if (NULL != initData.pfMemAlloc)
        {
            return initData.pfMemAlloc(size);
        }

        return NULL;
    }

    /* Calculate the number of slabs required to hold the allocation. */
    reqSlabs = ((size + (pSlabAllocCtx->slabSize-1U)) / pSlabAllocCtx->slabSize);

    for (i=0; i<pSlabAllocCtx->numSlabs; i++)
    {
        /* Search the slab indexes for a free slab. */
        if (-1 == pSlabAllocCtx->slabIdxs[i])
        {
            consecFree = 0;
            slabIdx    = i;

            /* Continue the search for a consecutive group of slabs. */
            for (; i<pSlabAllocCtx->numSlabs; i++)
            {
                if (-1 == pSlabAllocCtx->slabIdxs[i])
                {
                    consecFree++;

                    if (consecFree == reqSlabs)
                    {
                        void *p;
                        int8_t j;

                        /* Write a counting pattern into the slab indexes to reserve them. */
                        for (j=0; j<consecFree; j++)
                        {
                            pSlabAllocCtx->slabIdxs[slabIdx+j] = consecFree-j;
                        }

                        /* Calculate pointer to memory within the slabs. */
                        p = &pSlabAllocCtx->pRootAddr[(size_t)slabIdx * pSlabAllocCtx->slabSize];

                        WINC_VERBOSE_PRINT("SLAB[+%08x %d (%d %d)]\n", p, slabIdx, size, reqSlabs);

                        return p;
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    WINC_ERROR_PRINT("error, failed to allocate slab of %d bytes\n", size);

    return NULL;
}

/*****************************************************************************
  Description:
    De-allocate memory from the slab allocator.

  Parameters:
    p - Pointer to memory to be freed.

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void slabFree(void *p)
{
    int slabIdx;

    if (NULL == p)
    {
        return;
    }

    if ((NULL == pSlabAllocCtx) || (NULL == pSlabAllocCtx->pRootAddr))
    {
        /* Slab allocator isn't present, use the basic memory free. */
        if (NULL != initData.pfMemFree)
        {
            initData.pfMemFree(p);
        }

        return;
    }

    /* Ensure pointer is within the allocators region. */
    if (((uint8_t*)p < pSlabAllocCtx->pRootAddr) || ((uint8_t*)p >= &pSlabAllocCtx->pRootAddr[(size_t)pSlabAllocCtx->numSlabs * pSlabAllocCtx->slabSize]))
    {
        return;
    }

    /* Calculate the slab index. */
    slabIdx = (((uint8_t*)p - pSlabAllocCtx->pRootAddr) / pSlabAllocCtx->slabSize);

    WINC_VERBOSE_PRINT("SLAB[-%08x %d + %d]\n", p, slabIdx, pSlabAllocCtx->slabIdxs[slabIdx]);

    /* Use the count in the indexes to determine the length of the original allocation. */
    (void)memset(&pSlabAllocCtx->slabIdxs[slabIdx], 0xff, (size_t)pSlabAllocCtx->slabIdxs[slabIdx]);
}

/*****************************************************************************
  Description:
    Allocator for socket command requests.

  Parameters:
    size - Size of allocation.

  Returns:
    Pointer to allocation or NULL for error.

  Remarks:

 *****************************************************************************/

static void* sockAllocCmdReq(size_t size)
{
    void *p;

    if (NULL == initData.pfMemAlloc)
    {
        return NULL;
    }

    p = slabAlloc(size);

    if (NULL != p)
    {
    }

    return p;
}

/*****************************************************************************
  Description:
    Free socket command request memory.

  Parameters:
    p - Pointer to allocated memory.

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void sockFreeCmdReq(void *p)
{
    if (NULL == initData.pfMemFree)
    {
        return;
    }

    if (NULL != p)
    {
        slabFree(p);
    }
}

/*****************************************************************************
  Description:
    Allocator for DNS command requests.

  Parameters:
    size - Size of request to allocate.

  Returns:
    Pointer to allocation or NULL for error.

  Remarks:

 *****************************************************************************/

static void* dnsAllocRequest(size_t size)
{
    WINC_DNS_REQ *pDnsRequest;

    if (NULL == initData.pfMemAlloc)
    {
        return NULL;
    }

    /* Allocate memory of request structure in addition to size requested. */
    pDnsRequest = initData.pfMemAlloc(sizeof(WINC_DNS_REQ) + size + 1U);

    if (NULL != pDnsRequest)
    {
        (void)memset(pDnsRequest, 0, sizeof(WINC_DNS_REQ));
    }

    return pDnsRequest;
}

/*****************************************************************************
  Description:
    Free DNS command request memory.

  Parameters:
    pDnsRequest - Pointer to DNS request.

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void dnsFreeRequest(WINC_DNS_REQ *pDnsRequest)
{
    struct addrinfo *pRes;
    struct addrinfo *pNextRes;

    if (NULL == pDnsRequest)
    {
        return;
    }

    /* Check if this is the current global request, remove that if it is. */
    if (pCurrentDnsRequest == pDnsRequest)
    {
        pCurrentDnsRequest = NULL;
    }

    if (NULL == initData.pfMemFree)
    {
        return;
    }

    pRes = pDnsRequest->pRes;

    /* Free all memory associated with this request. */
    while (NULL != pRes)
    {
        pNextRes = pRes->ai_next;
        initData.pfMemFree(pRes);
        pRes = pNextRes;
    }

    initData.pfMemFree(pDnsRequest);
}

/*****************************************************************************
  Description:
    Write data to socket buffer.

  Parameters:
    pBuffer  - Pointer to socket buffer to write data into
    pData    - Pointer to new data to write
    dataLen  - Length of new data
    pAddr    - Pointer to datagram end point address structure
    fragment - Flag indicating if write can be fragment or should be contiguous

  Returns:
    true or false indicating success or failure.

  Remarks:
    In some cases, for UDP datagram it's required that the data is store contiguously.
    If so a padding write of data is added to the buffer with a blank address to align
    the write to the start of the buffer memory.

 *****************************************************************************/

static bool sockBufferWrite(WINC_SOCK_BUFFER *pBuffer, const uint8_t *pData, size_t dataLen, const WINC_SOCK_END_PT *pAddr, bool fragment)
{
    size_t fragmentLen;

    if ((NULL == pBuffer) || (NULL == pData) || (0U == dataLen))
    {
        return false;
    }

    /* Determine if this write would fit into the available space. */
    if (dataLen > (pBuffer->totalSize - pBuffer->length))
    {
        /* Not enough buffer space available. */
        return false;
    }

    if (NULL != pBuffer->pUdpPktBuffers)
    {
        /* Dealing with UDP datagrams, need to store the destination address with the data. */

        WINC_VERBOSE_PRINT("PB+ %d %d %d\n", pBuffer->pUdpPktBuffers->pktDepth, pBuffer->pUdpPktBuffers->inIdx, dataLen);

        if (false == fragment)
        {
            /* If not allowed to fragment the data, check there is enough space in the buffer before the end or do we
                need to wrap the buffer first and fit the data in the beginning, if there is space. */

            /* Only need to consider the case of out <= in, in the other case the space check above would verify if the
                write can happen. */
            if (pBuffer->outOffset <= pBuffer->inOffset)
            {
                fragmentLen = (pBuffer->totalSize - pBuffer->inOffset);

                if (dataLen > fragmentLen)
                {
                    /* There is not enough space at the end of the buffer to fit the datagram, check if there is space
                        to wrap the buffer for both the data and the packet buffers. */
                    if ((dataLen > pBuffer->outOffset) || ((pBuffer->pUdpPktBuffers->pktDepth+2U) >= pBuffer->pUdpPktBuffers->numPkts))
                    {
                        /* Not enough packet buffers available. */
                        return false;
                    }

                    WINC_VERBOSE_PRINT("PB@ %d\n", fragmentLen);

                    /* Create a dummy entry for the fragment to bring the write point back to the buffer start. */
                    (void)memset(&pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->inIdx].endPt, 0, sizeof(WINC_SOCK_END_PT));
                    pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->inIdx].pktLength = (uint16_t)fragmentLen;
                    pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->inIdx].pktOffset = pBuffer->inOffset;

                    pBuffer->pUdpPktBuffers->pktDepth++;
                    pBuffer->pUdpPktBuffers->inIdx++;

                    if (pBuffer->pUdpPktBuffers->inIdx == pBuffer->pUdpPktBuffers->numPkts)
                    {
                        pBuffer->pUdpPktBuffers->inIdx = 0;
                    }

                    pBuffer->length  += (uint16_t)fragmentLen;
                    pBuffer->inOffset = 0;
                }
            }
        }

        /* Check space in packet buffers. */
        if (pBuffer->pUdpPktBuffers->pktDepth == pBuffer->pUdpPktBuffers->numPkts)
        {
            return false;
        }

        /* Copy in the destination address, if provided, to a packet buffer to track destination and length of the datagram. */
        if (NULL != pAddr)
        {
            (void)memcpy(&pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->inIdx].endPt, pAddr, sizeof(WINC_SOCK_END_PT));
        }
        else
        {
            (void)memset(&pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->inIdx].endPt, 0, sizeof(WINC_SOCK_END_PT));
        }
        pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->inIdx].pktLength = (uint16_t)dataLen;
        pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->inIdx].pktOffset = pBuffer->inOffset;

        pBuffer->pUdpPktBuffers->pktDepth++;
        pBuffer->pUdpPktBuffers->inIdx++;

        if (pBuffer->pUdpPktBuffers->inIdx == pBuffer->pUdpPktBuffers->numPkts)
        {
            pBuffer->pUdpPktBuffers->inIdx = 0;
        }
    }

    while (dataLen > 0U)
    {
        /* Copy the data into the buffer, wrapping if need be. */
        fragmentLen = dataLen;

        if (fragmentLen > (pBuffer->totalSize - pBuffer->inOffset))
        {
            fragmentLen = (pBuffer->totalSize - pBuffer->inOffset);
        }

        (void)memcpy(&pBuffer->pData[pBuffer->inOffset], pData, fragmentLen);

        pBuffer->length   += (uint16_t)fragmentLen;
        pBuffer->inOffset += (uint16_t)fragmentLen;
        dataLen           -= fragmentLen;

        pData = pData + fragmentLen;

        if (pBuffer->totalSize == pBuffer->inOffset)
        {
            pBuffer->inOffset = 0;
        }
    }

    return true;
}

/*****************************************************************************
  Description:
    Read data from a socket buffer.

  Parameters:
    pBuffer  - Pointer to socket buffer to read data from
    pData    - Pointer to buffer to receive data
    dataLen  - Length of data to read
    pAddr    - Pointer to datagram end point address structure
    addrLen  - Length of address structure

  Returns:
    Number of bytes read, or -1 for error.

  Remarks:

 *****************************************************************************/

static ssize_t sockBufferRead(WINC_SOCK_BUFFER *pBuffer, uint8_t *pData, size_t dataLen, void *pAddr, size_t addrLen, bool peek, size_t *pLenRemain)
{
    ssize_t readData = 0;
    size_t discardLen = 0;
    size_t fragmentLen;
    uint16_t outOffset;
    uint16_t length;

    if (NULL == pBuffer)
    {
        return -1;
    }

    if (NULL == pBuffer->pUdpPktBuffers)
    {
        /* For TCP sockets, limit the data read to what is available. */
        if (dataLen > pBuffer->length)
        {
            dataLen = pBuffer->length;
        }
    }
    else
    {
        /* For UDP sockets, find the packet buffer to get the next datagram length. */
        if (0U == pBuffer->pUdpPktBuffers->pktDepth)
        {
            if (NULL != pLenRemain)
            {
                *pLenRemain = 0;
            }

            return 0;
        }

        if (AF_UNSPEC == pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->outIdx].endPt.sin_family)
        {
            /* If the next packet buffer is blank, this is a padding datagram, it and the data length
                specified by it need to be removed first. */
            fragmentLen = pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->outIdx].pktLength;

            WINC_VERBOSE_PRINT("PB# %d %d\n", fragmentLen, pBuffer->pUdpPktBuffers->outIdx);

            pBuffer->length    -= (uint16_t)fragmentLen;
            pBuffer->outOffset  = 0;

            pBuffer->pUdpPktBuffers->pktDepth--;
            pBuffer->pUdpPktBuffers->outIdx++;

            if (pBuffer->pUdpPktBuffers->outIdx == pBuffer->pUdpPktBuffers->numPkts)
            {
                pBuffer->pUdpPktBuffers->outIdx = 0;
            }

            if (0U == pBuffer->pUdpPktBuffers->pktDepth)
            {
                return 0;
            }
        }

        /* If an address pointer and length are provided, copy what we can from the packet buffer end point. */
        if ((NULL != pAddr) && (addrLen > 0U))
        {
            (void)memcpy(pAddr, &pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->outIdx], addrLen);
        }

        if (dataLen > pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->outIdx].pktLength)
        {
            /* If the requested length is too much, limit it to the datagram length. */
            dataLen = pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->outIdx].pktLength;
        }
        else
        {
            /* If the requested length is less than the datagrams length, the remaining data needs to be discarded. */
            discardLen = pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->outIdx].pktLength - dataLen;
        }

        WINC_VERBOSE_PRINT("PB- %d %d %d\n", pBuffer->pUdpPktBuffers->pktDepth, pBuffer->pUdpPktBuffers->outIdx, pBuffer->pUdpPktBuffers->pkts[pBuffer->pUdpPktBuffers->outIdx].pktLength);

        if (false == peek)
        {
            /* Discard the current packet buffer. */
            pBuffer->pUdpPktBuffers->pktDepth--;
            pBuffer->pUdpPktBuffers->outIdx++;

            if (pBuffer->pUdpPktBuffers->outIdx == pBuffer->pUdpPktBuffers->numPkts)
            {
                pBuffer->pUdpPktBuffers->outIdx = 0;
            }
        }
    }

    outOffset = pBuffer->outOffset;
    length    = pBuffer->length;

    while (dataLen > 0U)
    {
        /* Copy the data to the buffer provided. */
        fragmentLen = dataLen;

        if (fragmentLen > (pBuffer->totalSize - outOffset))
        {
            fragmentLen = (pBuffer->totalSize - outOffset);
        }

        if (NULL != pData)
        {
            (void)memcpy(pData, &pBuffer->pData[outOffset], fragmentLen);
            pData = pData + fragmentLen;
        }

        length    -= (uint16_t)fragmentLen;
        outOffset += (uint16_t)fragmentLen;

        if (outOffset >= pBuffer->totalSize)
        {
            outOffset -= pBuffer->totalSize;
        }

        readData += (ssize_t)fragmentLen;
        dataLen  -= fragmentLen;
    }

    /* Discard any data from the datagram that wasn't read. */
    if (discardLen > 0U)
    {
        WINC_VERBOSE_PRINT("DL %d\n", discardLen);

        outOffset += (uint16_t)discardLen;
        length    -= (uint16_t)discardLen;

        if (outOffset >= pBuffer->totalSize)
        {
            outOffset -= pBuffer->totalSize;
        }
    }

    if (false == peek)
    {
        pBuffer->outOffset = outOffset;
        pBuffer->length    = length;
    }

    if (NULL != pLenRemain)
    {
        if (NULL == pBuffer->pUdpPktBuffers)
        {
            *pLenRemain = pBuffer->length;
        }
        else
        {
            *pLenRemain = discardLen;
        }
    }

    return readData;
}

/*****************************************************************************
  Description:
    Find a free socket context.

  Parameters:
    None.

  Returns:
    Pointer to free socket context or NULL for error.

  Remarks:

 *****************************************************************************/

static WINC_SOCK_CTX* sockFindFreeSocket(void)
{
    int i;

    for (i=0; i<WINC_SOCK_NUM_SOCKETS; i++)
    {
        if (false == wincSockets[i].inUse)
        {
            (void)memset(&wincSockets[i], 0, sizeof(WINC_SOCK_CTX));
            return &wincSockets[i];
        }
    }

    return NULL;
}

/*****************************************************************************
  Description:
    Creates a new socket context.

  Parameters:
    pSockCtx - Pointer to socket context to create.
    type     - Type of socket.
    sockId   - ID of socket, zero if not known yet.

  Returns:
    true or false.

  Remarks:

 *****************************************************************************/

static bool sockCreateSocket(WINC_SOCK_CTX *pSockCtx, uint8_t type, uint16_t sockId)
{
    if (NULL == pSockCtx)
    {
        return false;
    }

    WINC_CONF_LOCK_CREATE(&pSockCtx->accessMutex);

    pSockCtx->inUse  = true;
    pSockCtx->type   = type;
    pSockCtx->sockId = sockId;

    return true;
}

/*****************************************************************************
  Description:
    Free the use of a socket context.

  Parameters:
    pSockCtx - Pointer to socket context to release.

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void sockDestroySocket(WINC_SOCK_CTX *pSockCtx)
{
    if (NULL == pSockCtx)
    {
        return;
    }

    if (false == pSockCtx->inUse)
    {
        return;
    }

    /* Release any allocated buffer. */
    slabFree(pSockCtx->recvBuffer.pData);
    slabFree(pSockCtx->sendBuffer.pData);
    slabFree(pSockCtx->recvBuffer.pUdpPktBuffers);
    slabFree(pSockCtx->sendBuffer.pUdpPktBuffers);

    WINC_CONF_LOCK_DESTROY(&pSockCtx->accessMutex);

    (void)memset(pSockCtx, 0, sizeof(WINC_SOCK_CTX));
}

/*****************************************************************************
  Description:
    Lock access to a socket context.

  Parameters:
    pSockCtx - Pointer to socket context to lock.

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static bool sockLockSocket(WINC_SOCK_CTX *pSockCtx)
{
    if (NULL == pSockCtx)
    {
        return false;
    }

    if (false == pSockCtx->inUse)
    {
        return false;
    }

    if (false == WINC_CONF_LOCK_ENTER(&pSockCtx->accessMutex))
    {
        return false;
    }

    return true;
}

/*****************************************************************************
  Description:
    Unlock access to a socket context.

  Parameters:
    pSockCtx - Pointer to socket context to lock.

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void sockUnlockSocket(WINC_SOCK_CTX *pSockCtx)
{
    if (NULL == pSockCtx)
    {
        return;
    }

    WINC_CONF_LOCK_LEAVE(&pSockCtx->accessMutex);
}

/*****************************************************************************
  Description:
    Allocate (or re-allocate) send and receive socket buffers.

  Parameters:
    pSockCtx  - Pointer to socket context
    recvBufSz - Size of receive buffer to allocate
    sendBufSz - Size of send buffer to allocate

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

static bool sockAllocSocketBuffers(WINC_SOCK_CTX *pSockCtx, size_t recvBufSz, size_t sendBufSz)
{
    if (NULL == pSockCtx)
    {
        return false;
    }

    /* Free receive data buffer if already allocated. */
    if (NULL != pSockCtx->recvBuffer.pData)
    {
        slabFree(pSockCtx->recvBuffer.pData);
    }

    (void)memset(&pSockCtx->recvBuffer, 0, sizeof(WINC_SOCK_BUFFER));

    /* Free send data buffer if already allocated. */
    if (NULL != pSockCtx->sendBuffer.pData)
    {
        slabFree(pSockCtx->sendBuffer.pData);
    }

    (void)memset(&pSockCtx->sendBuffer, 0, sizeof(WINC_SOCK_BUFFER));

    /* Allocate new receive buffer. */
    if (recvBufSz > 0U)
    {
        pSockCtx->recvBuffer.pData = slabAlloc(recvBufSz);

        if (NULL != pSockCtx->recvBuffer.pData)
        {
            pSockCtx->recvBuffer.totalSize = (uint16_t)recvBufSz;
        }
    }

    /* Allocate new send buffer. */
    if (sendBufSz > 0U)
    {
        pSockCtx->sendBuffer.pData = slabAlloc(sendBufSz);

        if (NULL != pSockCtx->sendBuffer.pData)
        {
            pSockCtx->sendBuffer.totalSize = (uint16_t)sendBufSz;
        }
    }

    return true;
}

/*****************************************************************************
  Description:
    Allocate (or re-allocate) socket datagram packet buffers.

  Parameters:
    pSockCtx  - Pointer to socket context
    numRxPkts - Number of receive datagrams supported
    numTxPkts - Number of send datagrams supported

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

static bool sockAllocUdpPktBuffers(WINC_SOCK_CTX *pSockCtx, uint8_t numRxPkts, uint8_t numTxPkts)
{
    if (NULL == pSockCtx)
    {
        return false;
    }

    /* Free receive packet buffers is already allocated. */
    if (NULL != pSockCtx->recvBuffer.pUdpPktBuffers)
    {
        slabFree(pSockCtx->recvBuffer.pUdpPktBuffers);
        pSockCtx->recvBuffer.pUdpPktBuffers = NULL;
    }

    /* Free send packet buffers is already allocated. */
    if (NULL != pSockCtx->sendBuffer.pUdpPktBuffers)
    {
        slabFree(pSockCtx->sendBuffer.pUdpPktBuffers);
        pSockCtx->sendBuffer.pUdpPktBuffers = NULL;
    }

    if (numRxPkts > 0U)
    {
        /* Allocate new receive packet buffers. */
        pSockCtx->recvBuffer.pUdpPktBuffers = slabAlloc(sizeof(WINC_SOCK_UDP_PKT_BUFFER) + (sizeof(WINC_SOCK_UDP_PKT) * numRxPkts));

        if (NULL == pSockCtx->recvBuffer.pUdpPktBuffers)
        {
            return false;
        }

        (void)memset(pSockCtx->recvBuffer.pUdpPktBuffers, 0, sizeof(WINC_SOCK_UDP_PKT_BUFFER));

        pSockCtx->recvBuffer.pUdpPktBuffers->numPkts = numRxPkts;
    }

    if (numTxPkts > 0U)
    {
        /* Allocate new send packet buffers. */
        pSockCtx->sendBuffer.pUdpPktBuffers = slabAlloc(sizeof(WINC_SOCK_UDP_PKT_BUFFER) + (sizeof(WINC_SOCK_UDP_PKT) * numTxPkts));

        if (NULL == pSockCtx->sendBuffer.pUdpPktBuffers)
        {
            return false;
        }

        (void)memset(pSockCtx->sendBuffer.pUdpPktBuffers, 0, sizeof(WINC_SOCK_UDP_PKT_BUFFER));

        pSockCtx->sendBuffer.pUdpPktBuffers->numPkts = numTxPkts;
    }

    return true;
}

/*****************************************************************************
  Description:
    Find socket context by it's ID.

  Parameters:
    sockId - Socket ID to find

  Returns:
    Pointer to socket context or NULL for error/not found.

  Remarks:

 *****************************************************************************/

static WINC_SOCK_CTX* sockFindByID(uint16_t sockId)
{
    int i;

    for (i=0; i<WINC_SOCK_NUM_SOCKETS; i++)
    {
        if ((true == wincSockets[i].inUse) && (sockId == wincSockets[i].sockId))
        {
            return &wincSockets[i];
        }
    }

    return NULL;
}

/*****************************************************************************
  Description:
    Find the TCP listening socket associated with the specified socket context.

  Parameters:
    pSockCtx - Pointer to socket context

  Returns:
    Pointer to socket context or NULL for error/not found

  Remarks:

 *****************************************************************************/

static WINC_SOCK_CTX* sockFindTCPListeningSocket(WINC_SOCK_CTX *pSockCtx)
{
    int i;

    if (NULL == pSockCtx)
    {
        return NULL;
    }

    /* The socket must be in use and be TCP. */
    if ((false == pSockCtx->inUse) || (SOCK_STREAM != pSockCtx->type))
    {
        return NULL;
    }

    for (i=0; i<WINC_SOCK_NUM_SOCKETS; i++)
    {
        if (&wincSockets[i] == pSockCtx)
        {
            continue;
        }

        /* Match on a TCP socket which is in use, listening sharing the same source port. */
        if ((true == wincSockets[i].inUse) && (true == wincSockets[i].listening) && (SOCK_STREAM == wincSockets[i].type) && (pSockCtx->localEndPt.sin_port == wincSockets[i].localEndPt.sin_port))
        {
            return &wincSockets[i];
        }
    }

    return NULL;
}

/*****************************************************************************
  Description:
    Find a TCP socket in the pending state.

  Parameters:
    pSockCtx - Pointer to socket context

  Returns:
    Pointer to socket context or NULL for error/not found.

  Remarks:

 *****************************************************************************/

static WINC_SOCK_CTX* sockFindTCPPendingSocket(WINC_SOCK_CTX *pSockCtx)
{
    int i;

    if (NULL == pSockCtx)
    {
        return NULL;
    }

    /* Socket must be TCP, in use and listening. */
    if ((false == pSockCtx->inUse) || (SOCK_STREAM != pSockCtx->type) || (false == pSockCtx->listening))
    {
        return NULL;
    }

    for (i=0; i<WINC_SOCK_NUM_SOCKETS; i++)
    {
        if (&wincSockets[i] == pSockCtx)
        {
            continue;
        }

        /* Match on TCP sockets, in use sharing the same source port but haven't yet accepted the connection. */
        if ((true == wincSockets[i].inUse) && (false == wincSockets[i].accepted) && (SOCK_STREAM == wincSockets[i].type) && (pSockCtx->localEndPt.sin_port == wincSockets[i].localEndPt.sin_port))
        {
            return &wincSockets[i];
        }
    }

    return NULL;
}

/*****************************************************************************
  Description:
    Add async mode SOCKC commands to command request.

  Parameters:
    cmdReqHandle - Command request handle
    pSockCtx     - Pointer to socket context

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

static bool sockAsyncModeConf(WINC_CMD_REQ_HANDLE cmdReqHandle, const WINC_SOCK_CTX *pSockCtx)
{
    uint16_t asyncWinSize  = 0;
    uint16_t asyncMaxFrmSz = 0;

    if ((WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle) || (NULL == pSockCtx))
    {
        return false;
    }

    if (WINC_SOCKET_ASYNC_MODE_OFF != (WINC_SOCKET_ASYNC_MODE_TYPE)pSockCtx->readMode)
    {
        if (WINC_SOCKET_ASYNC_MODE_ACKED == (WINC_SOCKET_ASYNC_MODE_TYPE)pSockCtx->readMode)
        {
            asyncWinSize = WINC_SOCK_BUF_RX_SZ;
        }

        if (SOCK_STREAM == pSockCtx->type)
        {
            asyncMaxFrmSz = (3*512)-38;
        }
        else
        {
            if (AF_INET == pSockCtx->domain)
            {
                asyncMaxFrmSz = (3*512)-46;
            }
            else if (AF_INET6 == pSockCtx->domain)
            {
                asyncMaxFrmSz = (3*512)-58;
            }
            else
            {
                /* Do nothing. */
            }
        }
    }

    (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_ASYNC_WIN_SZ, WINC_TYPE_INTEGER_UNSIGNED, asyncWinSize, 0);
    (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_ASYNC_MAX_FRM_SZ, WINC_TYPE_INTEGER_UNSIGNED, asyncMaxFrmSz, 0);

    return true;
}

/*****************************************************************************
  Description:
    Read data from the socket on the device.

  Parameters:
    pSockCtx - Pointer to socket context

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

static bool sockRead(WINC_SOCK_CTX *pSockCtx)
{
    ssize_t dataLenToRead;

    if (NULL == pSockCtx)
    {
        return false;
    }

    /* Clear received data flag, ready for new possible data. */
    pSockCtx->newRecvData = false;

    /* If no outstanding data to read, stop now. */
    if (0U == pSockCtx->pendingRecvDataLen)
    {
        return true;
    }

    WINC_VERBOSE_PRINT("SR %d %d %d\n", pSockCtx->pendingRecvDataLen, pSockCtx->recvBuffer.outstandingDataLen, pSockCtx->recvBuffer.length);

    /* Length of data to read, start based on pending minus outstanding, i.e. what we know is left. */
    dataLenToRead = (pSockCtx->pendingRecvDataLen - pSockCtx->recvBuffer.outstandingDataLen);

    /* Limit based on the space in the receive socket buffer. */
    if ((uint16_t)dataLenToRead > (pSockCtx->recvBuffer.totalSize - pSockCtx->recvBuffer.length - pSockCtx->recvBuffer.outstandingDataLen))
    {
        dataLenToRead = (pSockCtx->recvBuffer.totalSize - pSockCtx->recvBuffer.length - pSockCtx->recvBuffer.outstandingDataLen);
    }

    if (dataLenToRead > 0)
    {
        WINC_CMD_REQ_HANDLE cmdReqHandle;
        void *pCmdReqBuffer;
        ssize_t maxDataLenToRead;

        if (SOCK_DGRAM == pSockCtx->type)
        {
            maxDataLenToRead = MAX_UDP_SOCK_PAYLOAD_SZ;
        }
        else
        {
            maxDataLenToRead = MAX_TCP_SOCK_PAYLOAD_SZ;
        }

        if (dataLenToRead > maxDataLenToRead)
        {
            dataLenToRead = maxDataLenToRead;
        }

        WINC_VERBOSE_PRINT("SRreq %d\n", dataLenToRead);

        /* Allocate command request. */
        pCmdReqBuffer = sockAllocCmdReq(128);

        if (NULL == pCmdReqBuffer)
        {
            return false;
        }

        /* Initialise command request for a single command. */
        cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

        if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
        {
            sockFreeCmdReq(pCmdReqBuffer);
            return false;
        }

        /* Perform a SOCKRD command, request extended output information to update pending counts. */
        (void)WINC_CmdSOCKRD(cmdReqHandle, pSockCtx->sockId, WINC_CONST_SOCKET_OUTPUT_MODE_ASCII_EXT, dataLenToRead);

        if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
        {
            sockFreeCmdReq(pCmdReqBuffer);
            return false;
        }

        WINC_VERBOSE_PRINT("SR+ %d + %d\n", pSockCtx->recvBuffer.outstandingDataLen, dataLenToRead);

        /* Update the outstanding length to reflect this request. */
        pSockCtx->recvBuffer.outstandingDataLen += (uint16_t)dataLenToRead;

        if (SOCK_DGRAM == pSockCtx->type)
        {
            /* Discard the rest of the pending datagram */
            pSockCtx->pendingRecvDataLen = (uint16_t)dataLenToRead;
        }
    }

    return true;
}

/*****************************************************************************
  Description:
    Write data to a socket on the device.

  Parameters:
    pSockCtx - Pointer to socket context

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

static bool sockWrite(WINC_SOCK_CTX *pSockCtx)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    uint16_t dataLenToWrite;
    uint16_t outOffset;
    uint16_t seqNum;
    uint8_t pkkBufIdx = 0;

    if (NULL == pSockCtx)
    {
        return false;
    }

    if (SOCK_DGRAM == pSockCtx->type)
    {
        int numRemainingPkts;

        numRemainingPkts = (pSockCtx->sendBuffer.pUdpPktBuffers->pktDepth - pSockCtx->udpUnackedPktBufs);

        /* For datagrams check the remaining number of packet buffers. */
        if ((0U == pSockCtx->sendBuffer.pUdpPktBuffers->pktDepth) || (0 == numRemainingPkts))
        {
            return false;
        }

        dataLenToWrite = 0;

        /* Search through the packet buffers looking for a valid one to use. */
        do
        {
            pkkBufIdx = pSockCtx->sendBuffer.pUdpPktBuffers->outIdx + pSockCtx->udpUnackedPktBufs;

            if (pkkBufIdx >= pSockCtx->sendBuffer.pUdpPktBuffers->numPkts)
            {
                pkkBufIdx -= pSockCtx->sendBuffer.pUdpPktBuffers->numPkts;
            }

            /* If the address is specified, use this packet buffer. */
            if (AF_UNSPEC != pSockCtx->sendBuffer.pUdpPktBuffers->pkts[pkkBufIdx].endPt.sin_family)
            {
                dataLenToWrite = pSockCtx->sendBuffer.pUdpPktBuffers->pkts[pkkBufIdx].pktLength;
                outOffset      = pSockCtx->sendBuffer.pUdpPktBuffers->pkts[pkkBufIdx].pktOffset;
                break;
            }

            WINC_VERBOSE_PRINT("SW#\n");

            pSockCtx->udpUnackedPktBufs++;
        }
        while (0U != (unsigned)(numRemainingPkts--));
    }
    else
    {
        uint16_t length;

        /* For TCP sockets, start with the largest write size depending on IP version. */
        if (AF_INET == pSockCtx->remoteEndPt.sin_family)
        {
            dataLenToWrite = MAX_TCP_IPV4_SOCK_PAYLOAD_SZ;
        }
        else if (AF_INET6 == pSockCtx->remoteEndPt.sin_family)
        {
            dataLenToWrite = MAX_TCP_IPV6_SOCK_PAYLOAD_SZ;
        }
        else
        {
            return false;
        }

        /* Calculate offset and length of suitable data block which can be written. */
        outOffset = pSockCtx->sendBuffer.outOffset + pSockCtx->sendBuffer.outstandingDataLen;
        length    = pSockCtx->sendBuffer.length - pSockCtx->sendBuffer.outstandingDataLen;

        if (outOffset >= pSockCtx->sendBuffer.totalSize)
        {
            outOffset -= pSockCtx->sendBuffer.totalSize;
        }

        if (dataLenToWrite > length)
        {
            dataLenToWrite = length;
        }

        if (dataLenToWrite > (pSockCtx->sendBuffer.totalSize - outOffset))
        {
            dataLenToWrite = (pSockCtx->sendBuffer.totalSize - outOffset);
        }
    }

    if (0U == dataLenToWrite)
    {
        return true;
    }

    WINC_VERBOSE_PRINT("SW+ [%d] +%d %d\n", pSockCtx->unAckedSeqNum, pSockCtx->sendBuffer.outstandingDataLen, dataLenToWrite);

    /* Allocate a command request structure. */
    pCmdReqBuffer = sockAllocCmdReq((160U+dataLenToWrite));

    if (NULL == pCmdReqBuffer)
    {
        //WINC_ERROR_PRINT("error: unable to allocate socket write command request\n");
        return false;
    }

    /* Initialise the command request for a single command. */
    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, (160U+dataLenToWrite), 1, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        WINC_ERROR_PRINT("error: socket write command request initialisation failed\n");
        sockFreeCmdReq(pCmdReqBuffer);
        return false;
    }

    /* Calculate the pseudo-sequence number of the data being written. */
    seqNum = pSockCtx->unAckedSeqNum + pSockCtx->sendBuffer.outstandingDataLen;

    if (SOCK_DGRAM == pSockCtx->type)
    {
        WINC_TYPE typeRmtAddr;
        size_t lenRmtAddr;
        uintptr_t rmtAddr;

        /* For UDP datagrams locate the destination address from the packet buffer. */
        if (AF_INET == pSockCtx->sendBuffer.pUdpPktBuffers->pkts[pkkBufIdx].endPt.sin_family)
        {
            typeRmtAddr = WINC_TYPE_IPV4ADDR;
            lenRmtAddr  = 4;
            rmtAddr     = (uintptr_t)&pSockCtx->sendBuffer.pUdpPktBuffers->pkts[pkkBufIdx].endPt.v4.sin_addr;
        }
        else
        {
            typeRmtAddr = WINC_TYPE_IPV6ADDR;
            lenRmtAddr  = 16;
            rmtAddr     = (uintptr_t)&pSockCtx->sendBuffer.pUdpPktBuffers->pkts[pkkBufIdx].endPt.v6.sin6_addr;
        }

        /* Issue a SOCKWRTO command. */
        (void)WINC_CmdSOCKWRTO(cmdReqHandle, pSockCtx->sockId, typeRmtAddr, rmtAddr, lenRmtAddr, ntohs(pSockCtx->sendBuffer.pUdpPktBuffers->pkts[pkkBufIdx].endPt.sin_port), dataLenToWrite, (int32_t)seqNum, &pSockCtx->sendBuffer.pData[outOffset], dataLenToWrite);
    }
    else
    {
        /* For TCP sockets issue a SOCKWR command. */
        (void)WINC_CmdSOCKWR(cmdReqHandle, pSockCtx->sockId, dataLenToWrite, (int32_t)seqNum, &pSockCtx->sendBuffer.pData[outOffset], dataLenToWrite);
    }

    if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
    {
        WINC_ERROR_PRINT("error: socket write command request tranmission failed\n");
        sockFreeCmdReq(pCmdReqBuffer);
        return false;
    }

    pSockCtx->sendBuffer.outstandingDataLen += dataLenToWrite;
    pSockCtx->udpUnackedPktBufs++;

    return true;
}

/*****************************************************************************
  Description:
    Send a socket event via the callback.

  Parameters:
    pSockCtx - Pointer to socket context
    event    - Socket event.
    status   - Socket status.

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

static void sockEventCallback(WINC_SOCK_CTX *pSockCtx, WINC_SOCKET_EVENT event, WINC_SOCKET_STATUS status)
{
    if (NULL == pSockCtx)
    {
        return;
    }

    if (NULL != pfSocketEventCallback)
    {
        sockUnlockSocket(pSockCtx);

        pfSocketEventCallback(socketEventCallbackContext, WINC_SOCK_PTR_TO_HANDLE(pSockCtx), event, status);

        if (false == sockLockSocket(pSockCtx))
        {
        }
    }
}

/*****************************************************************************
  Description:
    Process a socket command request response.

  Parameters:
    pSockCtx - Pointer to socket context
    pElems   - Pointer to response elements received.

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void sockProcessRsp(WINC_SOCK_CTX *pSockCtx, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)
{
    if ((NULL == pSockCtx) || (NULL == pElems))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_CMD_ID_SOCKO:
        {
            if (1U != pElems->numElems)
            {
                break;
            }

            /* Read the socket ID from the SOCKO response. */
            if (0U == pSockCtx->sockId)
            {
                (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &pSockCtx->sockId, sizeof(pSockCtx->sockId));

                WINC_VERBOSE_PRINT("Socket ID is %d (%04x)\n", pSockCtx->sockId, pSockCtx->sockId);
            }
            break;
        }

        case WINC_CMD_ID_SOCKRD:
        {
            uint16_t sockId;
            uint16_t length;
            uint16_t reqLength;
            WINC_DEV_PARAM_ELEM elems[10];

            /* Read the socket ID and verify it matches this socket. */
            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &sockId, sizeof(sockId));

            /* Unpack original command parameters. */
            if (false == WINC_DevUnpackElements(pElems->srcCmd.numParams, pElems->srcCmd.pParams, elems))
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&elems[2], WINC_TYPE_INTEGER, &reqLength, sizeof(reqLength));

            if (sockId == pSockCtx->sockId)
            {
                WINC_SOCK_END_PT *pSourceAddr = NULL;
                WINC_SOCK_END_PT sourceAddr;

                /* Read the data length of the SOCKRD response. */
                (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &length, sizeof(length));

                if (length != reqLength)
                {
                    WINC_ERROR_PRINT("Error, length mismatch %d vs %d\n", length, reqLength);
                }

                if (pElems->numElems >= 4U)
                {
                    /* For extended SOCKRD response, read the pending data count and update the socket context. */
                    (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pSockCtx->pendingRecvDataLen, sizeof(pSockCtx->pendingRecvDataLen));

                    WINC_VERBOSE_PRINT("SP %d\n", pSockCtx->pendingRecvDataLen);

                    if (7U == pElems->numElems)
                    {
                        /* For UDP datagram reads read out the address information received. */
                        if (NULL == pSockCtx->recvBuffer.pUdpPktBuffers)
                        {
                            WINC_ERROR_PRINT("error: no allocated packet buffers\n");
                            break;
                        }

                        /* Check we have packet buffers to store the datagram into. */
                        if (pSockCtx->recvBuffer.pUdpPktBuffers->pktDepth == pSockCtx->recvBuffer.pUdpPktBuffers->numPkts)
                        {
                            WINC_ERROR_PRINT("error: packet buffers full\n");
                            break;
                        }

                        pSourceAddr = &sourceAddr;

                        /* Read the source address. */
                        if (WINC_TYPE_IPV4ADDR == pElems->elems[4].type)
                        {
                            pSourceAddr->v4.sin_family = AF_INET;

                            (void)WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_IPV4ADDR, &pSourceAddr->v4.sin_addr, sizeof(struct in_addr));
                            (void)WINC_CmdReadParamElem(&pElems->elems[5], WINC_TYPE_INTEGER, &pSourceAddr->v4.sin_port, sizeof(in_port_t));

                            pSourceAddr->v4.sin_port = htons(pSourceAddr->v4.sin_port);
                        }
                        else if (WINC_TYPE_IPV6ADDR == pElems->elems[4].type)
                        {
                            pSourceAddr->v6.sin6_family = AF_INET6;

                            (void)WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_IPV6ADDR, &pSourceAddr->v6.sin6_addr, sizeof(struct in6_addr));
                            (void)WINC_CmdReadParamElem(&pElems->elems[5], WINC_TYPE_INTEGER, &pSourceAddr->v6.sin6_port, sizeof(in_port_t));

                            pSourceAddr->v6.sin6_port = htons(pSourceAddr->v6.sin6_port);
                        }
                        else
                        {
                            WINC_ERROR_PRINT("error: unknown address type (%d)\n", pElems->elems[4].type);
                            break;
                        }
                    }
                }
                else
                {
                    pSockCtx->pendingRecvDataLen -= length;
                    WINC_VERBOSE_PRINT("SP %d (-%d)\n", pSockCtx->pendingRecvDataLen, length);
                }

                /* Write the received data into the socket buffer for the application to pull. */
                if (true == sockBufferWrite(&pSockCtx->recvBuffer, pElems->elems[pElems->numElems-1U].pData, length, pSourceAddr, true))
                {
                    WINC_VERBOSE_PRINT("SR-: %d - %d\n", pSockCtx->recvBuffer.outstandingDataLen, length);
                    pSockCtx->recvBuffer.outstandingDataLen -= length;
                    pSockCtx->newRecvData = true;
                }
                else
                {
                    WINC_ERROR_PRINT("error: failed to write to receive buffer\n");
                }
            }

            break;
        }

        case WINC_CMD_ID_SOCKC:
        {
            uint8_t id;
            int32_t value;

            if (2U != pElems->numElems)
            {
                break;
            }

            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &id, sizeof(id));
            (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &value, sizeof(value));

            WINC_VERBOSE_PRINT("Sock ID %04x CFG: %2d = %d\n", pSockCtx->sockId, id, value);
            break;
        }

        default:
        {
            WINC_VERBOSE_PRINT("Sock CmdRspCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

/*****************************************************************************
  Description:
    Socket command response callback.

  Parameters:
    context      - Context provided to WINC_CmdReqInit for callback
    devHandle    - WINC device handle
    cmdReqHandle - Command request handle
    event        - Event being raised
    eventArg     - Optional argument for event

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void sockCmdRspCallbackHandlerSeqUpdate(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle, WINC_DEV_CMDREQ_EVENT_TYPE event, uintptr_t eventArg)
{
    /* context provided was the socket address. */
    WINC_SOCK_CTX *pSockCtx = (WINC_SOCK_CTX*)context;

    WINC_VERBOSE_PRINT("SOCK CmdRspCBSeqUp %08x Event (Sock %08x) %d\n", cmdReqHandle, context, event);

    /* Command request is complete, no further need for it. */
    if (WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE == event)
    {
        sockFreeCmdReq((WINC_COMMAND_REQUEST*)cmdReqHandle);
    }

    /* Only take further action if socket is not actually in use. */
    if ((NULL == pSockCtx) || (false == pSockCtx->inUse))
    {
        return;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        return;
    }

    if (WINC_DEV_CMDREQ_EVENT_TX_COMPLETE == event)
    {
        pSockCtx->seqNumUpdateSent = false;
    }

    sockUnlockSocket(pSockCtx);
}

/*****************************************************************************
  Description:
    Socket command response callback.

  Parameters:
    context      - Context provided to WINC_CmdReqInit for callback
    devHandle    - WINC device handle
    cmdReqHandle - Command request handle
    event        - Event being raised
    eventArg     - Optional argument for event

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void sockCmdRspCallbackHandler(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle, WINC_DEV_CMDREQ_EVENT_TYPE event, uintptr_t eventArg)
{
    /* context provided was the socket address. */
    WINC_SOCK_CTX *pSockCtx = (WINC_SOCK_CTX*)context;

    WINC_VERBOSE_PRINT("SOCK CmdRspCB %08x Event (Sock %08x) %d\n", cmdReqHandle, context, event);

    /* Command request is complete, no further need for it. */
    if (WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE == event)
    {
        sockFreeCmdReq((WINC_COMMAND_REQUEST*)cmdReqHandle);
    }

    /* Only take further action if socket is not actually in use. */
    if ((NULL == pSockCtx) || (false == pSockCtx->inUse))
    {
        return;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        return;
    }

    switch (event)
    {
        case WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
        {
            /* Command request transmission complete to WINC device. */
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
        {
            /* Status response received for command. */

            WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (WINC_DEV_EVENT_STATUS_ARGS*)eventArg;
            WINC_SOCKET_STATUS status = WINC_SOCKET_STATUS_OK;

            if (NULL != pStatusInfo)
            {
                /* Set status based on success/failure of command. */
                if (WINC_STATUS_OK != pStatusInfo->status)
                {
//                    WINC_ERROR_PRINT("error: command %04x error %04x\n", pStatusInfo->rspCmdId, pStatusInfo->status);
                    status = WINC_SOCKET_STATUS_ERROR;
                }

                switch (pStatusInfo->rspCmdId)
                {
                    case WINC_CMD_ID_SOCKO:
                    {
                        if (WINC_SOCKET_ASYNC_MODE_OFF != (WINC_SOCKET_ASYNC_MODE_TYPE)pSockCtx->readMode)
                        {
                            WINC_CMD_REQ_HANDLE sockCmdReqHandle;
                            void *pCmdReqBuffer;

                            pCmdReqBuffer = sockAllocCmdReq(256);

                            if (NULL == pCmdReqBuffer)
                            {
                                break;
                            }

                            sockCmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 2, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

                            if (WINC_CMD_REQ_INVALID_HANDLE == sockCmdReqHandle)
                            {
                                sockFreeCmdReq(pCmdReqBuffer);
                                break;
                            }

                            (void)sockAsyncModeConf(sockCmdReqHandle, pSockCtx);

                            if (false == WINC_DevTransmitCmdReq(wincDevHandle, sockCmdReqHandle))
                            {
                                sockFreeCmdReq(pCmdReqBuffer);
                                break;
                            }

                            pSockCtx->nextSeqNum = 0;
                        }

                        sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_OPEN, status);
                        break;
                    }

                    case WINC_CMD_ID_SOCKBL:
                    {
                        sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_LISTEN, status);
                        break;
                    }

                    case WINC_CMD_ID_SOCKBR:
                    case WINC_CMD_ID_SOCKBM:
                    {
                        if ((SOCK_DGRAM == pSockCtx->type) && (IPPROTO_TLS != pSockCtx->protocol))
                        {
                            /* SOCKBR success indicates a connected socket. */
                            pSockCtx->connected = true;

                            sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_CONNECT, status);
                        }

                        break;
                    }

                    case WINC_CMD_ID_SOCKCL:
                    {
                        sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_CLOSE, status);

                        /* SOCKCL complete, close socket. */
                        sockDestroySocket(pSockCtx);
                        pSockCtx = NULL;
                        break;
                    }

                    case WINC_CMD_ID_SOCKWR:
                    case WINC_CMD_ID_SOCKWRTO:
                    {
                        WINC_DEV_PARAM_ELEM elems[10];
                        uint16_t seqNum;

                        if (pStatusInfo->srcCmd.numParams < 3U)
                        {
                            break;
                        }

                        /* Unpack the SOCKWR/SOCKWRTO command. */
                        if (true == WINC_DevUnpackElements(pStatusInfo->srcCmd.numParams, pStatusInfo->srcCmd.pParams, elems))
                        {
                            uint16_t dataLength;

                            /* Read the length and sequence number of the socket write command. */
                            (void)WINC_CmdReadParamElem(&elems[pStatusInfo->srcCmd.numParams-3U], WINC_TYPE_INTEGER, &dataLength, sizeof(dataLength));
                            (void)WINC_CmdReadParamElem(&elems[pStatusInfo->srcCmd.numParams-2U], WINC_TYPE_INTEGER, &seqNum, sizeof(seqNum));

                            WINC_VERBOSE_PRINT("SW- [%d] %04x %d %d\n", seqNum, pStatusInfo->status, dataLength, pSockCtx->sendBuffer.outstandingDataLen);

                            if (WINC_STATUS_OK == pStatusInfo->status)
                            {
                                uint8_t pktDepth = 0;

                                if ((SOCK_DGRAM == pSockCtx->type) && (NULL != pSockCtx->sendBuffer.pUdpPktBuffers))
                                {
                                    pktDepth = pSockCtx->sendBuffer.pUdpPktBuffers->pktDepth;
                                }

                                /* Dummy socket buffer read to remove the data this write has completed. */
                                (void)sockBufferRead(&pSockCtx->sendBuffer, NULL, dataLength, NULL, 0, false, NULL);

                                /* Update outstanding data length to reflect completed write. */
                                pSockCtx->sendBuffer.outstandingDataLen -= dataLength;

                                if ((SOCK_DGRAM == pSockCtx->type) && (NULL != pSockCtx->sendBuffer.pUdpPktBuffers))
                                {
                                    /* Update the outstanding packet buffer count with the number of buffers freed
                                        by the dummy socket buffer read. */
                                    pSockCtx->udpUnackedPktBufs -= (pktDepth - pSockCtx->sendBuffer.pUdpPktBuffers->pktDepth);
                                }

                                /* Update the expected sequence number for the next write operation. */
                                pSockCtx->unAckedSeqNum = seqNum + dataLength;
                            }
                            else if (pSockCtx->unAckedSeqNum == seqNum)
                            {
                                /* On write failure, reset the outstanding counts to cause resend. */
                                pSockCtx->sendBuffer.outstandingDataLen = 0;
                                pSockCtx->udpUnackedPktBufs             = 0;
                            }
                            else
                            {
                                /* Do nothing. */
                            }
                        }

                        if (WINC_STATUS_OK == pStatusInfo->status)
                        {
                            sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_SEND, status);

                            /* Perform a new device write, on success. */
                            (void)sockWrite(pSockCtx);
                        }
                        else if ((WINC_STATUS_SOCKET_NOT_READY == pStatusInfo->status) || (WINC_STATUS_SOCKET_SEQUENCE_ERROR == pStatusInfo->status))
                        {
                            /* Perform a new device write, non-fatal error. */
                            (void)sockWrite(pSockCtx);
                        }
                        else
                        {
                            sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_SEND, status);
                        }

                        break;
                    }

                    case WINC_CMD_ID_SOCKRD:
                    {
                        if (true == pSockCtx->newRecvData)
                        {
                            sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_RECV, status);
                        }

                        if (WINC_STATUS_OK == pStatusInfo->status)
                        {
                            if ((true == pSockCtx->connected) || (true == pSockCtx->newRecvData))
                            {
                                /* Perform another device read, only if connected or data waiting. */
                                (void)sockRead(pSockCtx);
                            }
                        }

                        break;
                    }

                    default:
                    {
                        WINC_VERBOSE_PRINT("Sock CmdRspCB %08x ID %04x status %04x not handled\r\n", cmdReqHandle, pStatusInfo->rspCmdId, pStatusInfo->status);
                        break;
                    }
                }
            }
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *const pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS *const)eventArg;

            if (NULL != pRspElems)
            {
                sockProcessRsp(pSockCtx, pRspElems);
            }
            break;
        }

        default:
        {
            WINC_VERBOSE_PRINT("Sock CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }

    sockUnlockSocket(pSockCtx);
}

/*****************************************************************************
  Description:
    Process socket AEC messages.

  Parameters:
    context   - Context provided to WINC_DevAECCallbackRegister for callback
    devHandle - WINC device handle
    pElems    - Pointer to AEC elements

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void sockProcessAEC(uintptr_t context, WINC_DEVICE_HANDLE devHandle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)
{
    WINC_SOCK_CTX *pSockCtx;
    uint16_t sockId;

    if (NULL == pElems)
    {
        return;
    }

    if (pElems->numElems < 1U)
    {
        return;
    }

    /* Read the socket ID from the AEC. */
    (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &sockId, sizeof(sockId));

    /* Find relevant socket. */
    pSockCtx = sockFindByID(sockId);

    if ((NULL != pSockCtx) && (false == sockLockSocket(pSockCtx)))
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_SOCKIND:
        {
            WINC_SOCK_CTX *pListeningSockCtx;
            bool newSocket = false;

            if (5U != pElems->numElems)
            {
                break;
            }

            if (NULL == pSockCtx)
            {
                /* Socket ID not recognised, need to add for TCP server. */
                pSockCtx = sockFindFreeSocket();

                if (NULL == pSockCtx)
                {
                    WINC_ERROR_PRINT("error: no free sockets\n");
                    break;
                }

                if (false == sockCreateSocket(pSockCtx, SOCK_STREAM, sockId))
                {
                    return;
                }

                if (false == sockLockSocket(pSockCtx))
                {
                    return;
                }

                newSocket = true;
            }

            /* Check socket address types match. */
            if (pElems->elems[1].type != pElems->elems[3].type)
            {
                WINC_ERROR_PRINT("error: address type mismatch (%d != %d)\n", pElems->elems[1].type, pElems->elems[3].type);
            }

            if (WINC_TYPE_IPV4ADDR == pElems->elems[1].type)
            {
                /* Extract and store local and remote IPv4 addresses. */
                pSockCtx->localEndPt.v4.sin_family = AF_INET;

                (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_IPV4ADDR, &pSockCtx->localEndPt.v4.sin_addr, sizeof(struct in_addr));
                (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pSockCtx->localEndPt.v4.sin_port, sizeof(in_port_t));

                pSockCtx->localEndPt.v4.sin_port = htons(pSockCtx->localEndPt.v4.sin_port);

                pSockCtx->remoteEndPt.v4.sin_family = AF_INET;

                (void)WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_IPV4ADDR, &pSockCtx->remoteEndPt.v4.sin_addr, sizeof(struct in_addr));
                (void)WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_INTEGER, &pSockCtx->remoteEndPt.v4.sin_port, sizeof(in_port_t));

                pSockCtx->remoteEndPt.v4.sin_port = htons(pSockCtx->remoteEndPt.v4.sin_port);
            }
            else if (WINC_TYPE_IPV6ADDR == pElems->elems[1].type)
            {
                /* Extract and store local and remote IPv6 addresses. */
                pSockCtx->localEndPt.v6.sin6_family = AF_INET6;

                (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_IPV6ADDR, &pSockCtx->localEndPt.v6.sin6_addr, sizeof(struct in6_addr));
                (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pSockCtx->localEndPt.v6.sin6_port, sizeof(in_port_t));

                pSockCtx->localEndPt.v6.sin6_port = htons(pSockCtx->localEndPt.v6.sin6_port);

                pSockCtx->remoteEndPt.v6.sin6_family = AF_INET6;

                (void)WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_IPV6ADDR, &pSockCtx->remoteEndPt.v6.sin6_addr, sizeof(struct in6_addr));
                (void)WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_INTEGER, &pSockCtx->remoteEndPt.v6.sin6_port, sizeof(in_port_t));

                pSockCtx->remoteEndPt.v6.sin6_port = htons(pSockCtx->remoteEndPt.v6.sin6_port);
            }
            else
            {
                WINC_ERROR_PRINT("error: unknown address type (%d)\n", pElems->elems[1].type);
                break;
            }

            if (false == pSockCtx->tlsPending)
            {
                pSockCtx->connected = true;
            }

            if (true == newSocket)
            {
                /* A new socket was created, find the listening socket it goes with. */
                pListeningSockCtx = sockFindTCPListeningSocket(pSockCtx);

                if (NULL == pListeningSockCtx)
                {
                    WINC_ERROR_PRINT("error: unable to find listening socket\n");
                    sockDestroySocket(pSockCtx);
                    pSockCtx = NULL;
                    break;
                }

                /* Send connect request event. */
                sockEventCallback(pListeningSockCtx, WINC_SOCKET_EVENT_CONNECT_REQ, WINC_SOCKET_STATUS_OK);

                /* Copy configuration from listening socket. */
                pSockCtx->readMode = pListeningSockCtx->readMode;
                pSockCtx->domain   = pListeningSockCtx->domain;
                pSockCtx->protocol = pListeningSockCtx->protocol;
            }
            else if (true == pSockCtx->connected)
            {
                /* Send connect event. */
                sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_CONNECT, WINC_SOCKET_STATUS_OK);
            }
            else
            {
                /* Do nothing. */
            }

            WINC_VERBOSE_PRINT("Socket ID is %d (%04x)\n", pSockCtx->sockId, pSockCtx->sockId);

            break;
        }

        case WINC_AEC_ID_SOCKRXT:
        {
            if (NULL == pSockCtx)
            {
                break;
            }

            if (WINC_SOCKET_ASYNC_MODE_OFF == (WINC_SOCKET_ASYNC_MODE_TYPE)pSockCtx->readMode)
            {
                if (2U != pElems->numElems)
                {
                    break;
                }

                /* Read the new pending data length values. */
                (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER, &pSockCtx->pendingRecvDataLen, sizeof(uint16_t));

                WINC_VERBOSE_PRINT("RXT %d\n", pSockCtx->pendingRecvDataLen);

                /* Trigger a socket read from device. */
                (void)sockRead(pSockCtx);
            }
            else
            {
                if ((4U == pElems->numElems) || (3U == pElems->numElems))
                {
                    uint16_t length;
                    uint16_t seqNum;

                    (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_INTEGER_UNSIGNED, &length, sizeof(length));

                    if (4U == pElems->numElems)
                    {
                        (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER_UNSIGNED, &seqNum, sizeof(seqNum));

                        WINC_VERBOSE_PRINT("RXT %d %d\n", seqNum, length);
                    }
                    else
                    {
                        WINC_VERBOSE_PRINT("RXT %d\n", length);
                    }

                    /* Write the received data into the socket buffer for the application to pull. */
                    if (true == sockBufferWrite(&pSockCtx->recvBuffer, pElems->elems[pElems->numElems-1U].pData, length, NULL, true))
                    {
                        pSockCtx->newRecvData = true;
                    }
                    else
                    {
                        WINC_ERROR_PRINT("error: failed to write to receive buffer\n");
                    }

                    sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_RECV, WINC_SOCKET_STATUS_OK);
                }
            }

            break;
        }

        case WINC_AEC_ID_SOCKRXU:
        {
            if (NULL == pSockCtx)
            {
                break;
            }

            if (WINC_SOCKET_ASYNC_MODE_OFF == (WINC_SOCKET_ASYNC_MODE_TYPE)pSockCtx->readMode)
            {
                /* Read the new pending data length values. */
                (void)WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_INTEGER, &pSockCtx->pendingRecvDataLen, sizeof(uint16_t));

                WINC_VERBOSE_PRINT("RXU %d\n", pSockCtx->pendingRecvDataLen);

                /* Trigger a socket read from device. */
                (void)sockRead(pSockCtx);
            }
            else
            {
                if ((6U == pElems->numElems) || (5U == pElems->numElems))
                {
                    WINC_SOCK_END_PT *pSourceAddr = NULL;
                    WINC_SOCK_END_PT sourceAddr;
                    uint16_t length;
                    uint16_t seqNum;

                    if (NULL == pSockCtx->recvBuffer.pUdpPktBuffers)
                    {
                        WINC_ERROR_PRINT("error: no allocated packet buffers\n");
                        break;
                    }

                    /* Check we have packet buffers to store the datagram into. */
                    if (pSockCtx->recvBuffer.pUdpPktBuffers->pktDepth == pSockCtx->recvBuffer.pUdpPktBuffers->numPkts)
                    {
                        WINC_ERROR_PRINT("error: packet buffers full\n");
                        break;
                    }

                    pSourceAddr = &sourceAddr;

                    /* Read the source address. */
                    if (WINC_TYPE_IPV4ADDR == pElems->elems[1].type)
                    {
                        pSourceAddr->v4.sin_family = AF_INET;

                        (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_IPV4ADDR, &pSourceAddr->v4.sin_addr, sizeof(struct in_addr));
                        (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pSourceAddr->v4.sin_port, sizeof(in_port_t));

                        pSourceAddr->v4.sin_port = htons(pSourceAddr->v4.sin_port);
                    }
                    else if (WINC_TYPE_IPV6ADDR == pElems->elems[1].type)
                    {
                        pSourceAddr->v6.sin6_family = AF_INET6;

                        (void)WINC_CmdReadParamElem(&pElems->elems[1], WINC_TYPE_IPV6ADDR, &pSourceAddr->v6.sin6_addr, sizeof(struct in6_addr));
                        (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_INTEGER, &pSourceAddr->v6.sin6_port, sizeof(in_port_t));

                        pSourceAddr->v6.sin6_port = htons(pSourceAddr->v6.sin6_port);
                    }
                    else
                    {
                        WINC_ERROR_PRINT("error: unknown address type (%d)\n", pElems->elems[1].type);
                        break;
                    }

                    (void)WINC_CmdReadParamElem(&pElems->elems[3], WINC_TYPE_INTEGER_UNSIGNED, &length, sizeof(length));

                    if (6U == pElems->numElems)
                    {
                        (void)WINC_CmdReadParamElem(&pElems->elems[4], WINC_TYPE_INTEGER_UNSIGNED, &seqNum, sizeof(seqNum));

                        WINC_VERBOSE_PRINT("RXU %d %d\n", seqNum, length);
                    }
                    else
                    {
                        WINC_VERBOSE_PRINT("RXU %d\n", length);
                    }

                    /* Write the received data into the socket buffer for the application to pull. */
                    if (true == sockBufferWrite(&pSockCtx->recvBuffer, pElems->elems[pElems->numElems-1U].pData, length, pSourceAddr, true))
                    {
                        pSockCtx->newRecvData = true;

                        sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_RECV, WINC_SOCKET_STATUS_OK);
                    }
                    else
                    {
                        WINC_ERROR_PRINT("error: failed to write to receive buffer\n");
                    }
                }
            }

            break;
        }

        case WINC_AEC_ID_SOCKCL:
        {
            if (1U != pElems->numElems)
            {
                break;
            }

            if (NULL != pSockCtx)
            {
                /* Disconnect the socket. */
                pSockCtx->connected = false;
            }
            break;
        }

        case WINC_AEC_ID_SOCKTLS:
        {
            if (1U != pElems->numElems)
            {
                break;
            }

            if (NULL != pSockCtx)
            {
                pSockCtx->connected  = true;
                pSockCtx->tlsPending = false;

                sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_CONNECT, WINC_SOCKET_STATUS_OK);
                sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_TLS_CONNECT, WINC_SOCKET_STATUS_OK);
            }
            break;
        }

        case WINC_AEC_ID_SOCKERR:
        {
            if (2U != pElems->numElems)
            {
                break;
            }

            if (NULL != pSockCtx)
            {
                sockEventCallback(pSockCtx, WINC_SOCKET_EVENT_ERROR, WINC_SOCKET_STATUS_ERROR);
            }
            break;
        }

        default:
        {
            WINC_VERBOSE_PRINT("Sock AECCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }

    sockUnlockSocket(pSockCtx);
}

/*****************************************************************************
  Description:
    DNS command response callback.

  Parameters:
    context      - Context provided to WINC_CmdReqInit for callback
    devHandle    - WINC device handle
    cmdReqHandle - Command request handle
    event        - Event being raised
    eventArg     - Optional argument for event

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void dnsCmdRspCallbackHandler(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle, WINC_DEV_CMDREQ_EVENT_TYPE event, uintptr_t eventArg)
{
    /* context provided was the DNS request address. */
    WINC_DNS_REQ *pDnsRequest = (WINC_DNS_REQ*)context;

    switch (event)
    {
        case WINC_DEV_CMDREQ_EVENT_TX_COMPLETE:
        {
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE:
        {
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_CMD_STATUS:
        {
            const WINC_DEV_EVENT_STATUS_ARGS *pStatusInfo = (const WINC_DEV_EVENT_STATUS_ARGS*)eventArg;

            if (WINC_STATUS_OK != pStatusInfo->status)
            {
                /* DNS request failed, remove the allocate request. */
                dnsFreeRequest(pDnsRequest);
            }
            break;
        }

        case WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED:
        {
            const WINC_DEV_EVENT_RSP_ELEMS *pRspElems = (const WINC_DEV_EVENT_RSP_ELEMS*)eventArg;

            if (NULL != pRspElems)
            {
            }
            break;
        }

        default:
        {
            WINC_VERBOSE_PRINT("DNS CmdRspCB %08x event %d not handled\r\n", cmdReqHandle, event);
            break;
        }
    }
}

/*****************************************************************************
  Description:
    Process DNS AEC messages.

  Parameters:
    context   - Context provided to WINC_DevAECCallbackRegister for callback
    devHandle - WINC device handle
    pElems    - Pointer to AEC elements

  Returns:
    None.

  Remarks:

 *****************************************************************************/

static void dnsProcessAEC(uintptr_t context, WINC_DEVICE_HANDLE devHandle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems)
{
//    WINC_SOCK_CTX *pDnsRequest;

    if (NULL == pElems)
    {
        return;
    }

    switch (pElems->rspId)
    {
        case WINC_AEC_ID_DNSRESOLV:
        {
            int recordType;

            if ((3U != pElems->numElems) || (NULL == pCurrentDnsRequest))
            {
                break;
            }

            if (NULL == initData.pfMemAlloc)
            {
                break;
            }

            /* Read the record type from the AEC. */
            (void)WINC_CmdReadParamElem(&pElems->elems[0], WINC_TYPE_INTEGER, &recordType, sizeof(recordType));

            WINC_VERBOSE_PRINT("DNS '%.*s':%d\n", pCurrentDnsRequest->hostNameLen, pElems->elems[1].pData, pCurrentDnsRequest->hostNameLen);

            /* Verify the response matches the request. */
            if ((pElems->elems[1].length == pCurrentDnsRequest->hostNameLen) && (0 == memcmp(pElems->elems[1].pData, pCurrentDnsRequest->hostName, pCurrentDnsRequest->hostNameLen)))
            {
                struct addrinfo **pRes;

                /* Load the current requests result pointer. */
                pRes = &pCurrentDnsRequest->pRes;

                /* Iterate to the end of the list. */
                while (NULL != *pRes)
                {
                    pRes = &(*pRes)->ai_next;
                }

                /* Allocate space for the new results. */
                *pRes = initData.pfMemAlloc(sizeof(struct addrinfo) + sizeof(struct sockaddr));

                if (NULL != *pRes)
                {
                    /* Copy the response into the results structure. */
                    (void)memset(*pRes, 0, sizeof(struct addrinfo) + sizeof(struct sockaddr));

                    (*pRes)->ai_addr = (struct sockaddr*)&(*pRes)[1];
                    (*pRes)->ai_next = NULL;

                    if (WINC_TYPE_IPV4ADDR == pElems->elems[2].type)
                    {
                        /* Read an IPv4 address. */
                        (*pRes)->ai_family  = (int)AF_INET;
                        (*pRes)->ai_addrlen = sizeof(struct sockaddr_in);

                        ((struct sockaddr_in*)(*pRes)->ai_addr)->sin_family = AF_INET;
                        (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_IPV4ADDR, &((struct sockaddr_in*)(*pRes)->ai_addr)->sin_addr, sizeof(struct in_addr));
                    }
                    else if (WINC_TYPE_IPV6ADDR == pElems->elems[2].type)
                    {
                        /* Read an IPv6 address. */
                        (*pRes)->ai_family  = (int)AF_INET6;
                        (*pRes)->ai_addrlen = sizeof(struct sockaddr_in6);

                        ((struct sockaddr_in6*)(*pRes)->ai_addr)->sin6_family = AF_INET6;
                        (void)WINC_CmdReadParamElem(&pElems->elems[2], WINC_TYPE_IPV6ADDR, &((struct sockaddr_in6*)(*pRes)->ai_addr)->sin6_addr, sizeof(struct in6_addr));
                    }
                    else
                    {
                        WINC_ERROR_PRINT("error: unknown address type (%d)\n", pElems->elems[2].type);
                    }
                }
            }

            break;
        }

        case WINC_AEC_ID_DNSERR:
        {
            break;
        }

        default:
        {
            WINC_VERBOSE_PRINT("DNS AECCB ID %04x not handled\r\n", pElems->rspId);
            break;
        }
    }
}

/*****************************************************************************
  Description:
    Initialise the socket module.

  Parameters:
    devHandle - WINC device handle
    pInitData - Pointer to initialisation data

  Returns:
    true or false indicating success or failure

  Remarks:

 *****************************************************************************/

bool WINC_SockInit(WINC_DEVICE_HANDLE devHandle, WINC_SOCKET_INIT_TYPE *pInitData)
{
    if (WINC_DEVICE_INVALID_HANDLE == devHandle)
    {
        return false;
    }

    if (NULL != pInitData)
    {
        /* Copy the initialisation data if provided. */
        (void)memcpy(&initData, pInitData, sizeof(WINC_SOCKET_INIT_TYPE));

        /* Initialise the slab allocator. */
        slabInit(initData.slabSize, initData.numSlabs);

        (void)memset(wincSockets, 0, sizeof(wincSockets));

        /* Register AEC callbacks for sockets and DNS commands. */
        (void)WINC_DevAECCallbackRegister(devHandle, sockProcessAEC, 0);
        (void)WINC_DevAECCallbackRegister(devHandle, dnsProcessAEC, 0);
    }
    else
    {
        int i;

        /* Re-initialise, close all sockets and free resources. */

        for (i=0; i<WINC_SOCK_NUM_SOCKETS; i++)
        {
            if (true == wincSockets[i].inUse)
            {
                sockEventCallback(&wincSockets[i], WINC_SOCKET_EVENT_CLOSE, WINC_SOCKET_STATUS_ERROR);

                sockDestroySocket(&wincSockets[i]);
            }
        }

        dnsFreeRequest(pCurrentDnsRequest);
    }

    wincDevHandle           = devHandle;
    pfSocketEventCallback   = NULL;
    pCurrentDnsRequest      = NULL;

    return true;
}

/*****************************************************************************
  Description:
    De-initialise the socket module.

  Parameters:
    devHandle - WINC device handle

  Returns:
    true or false indicating success or failure

  Remarks:

 *****************************************************************************/

bool WINC_SockDeinit(WINC_DEVICE_HANDLE devHandle)
{
    if (false == WINC_SockInit(devHandle, NULL))
    {
        return false;
    }

    (void)WINC_DevAECCallbackDeregister(wincDevHandle, sockProcessAEC);
    (void)WINC_DevAECCallbackDeregister(wincDevHandle, dnsProcessAEC);

    dnsFreeRequest(pCurrentDnsRequest);

    if (NULL != pSlabAllocCtx)
    {
        initData.pfMemFree(pSlabAllocCtx);
    }

    wincDevHandle = WINC_DEVICE_INVALID_HANDLE;

    return true;
}

/*****************************************************************************
  Description:
    Register a socket event callback and context.

  Parameters:
    devHandle       - WINC device handle
    pfSocketEventCB - Callback function pointer
    context         - Callback function context

  Returns:
    true or false indicating success or failure.

  Remarks:

 *****************************************************************************/

bool WINC_SockRegisterEventCallback(WINC_DEVICE_HANDLE devHandle, WINC_SOCKET_EVENT_CALLBACK pfSocketEventCB, uintptr_t context)
{
    if (WINC_DEVICE_INVALID_HANDLE == devHandle)
    {
        return false;
    }

    pfSocketEventCallback      = pfSocketEventCB;
    socketEventCallbackContext = context;

    return true;
}

/******************************************************************************

    Berkeley Style Socket Interface

 ******************************************************************************/
const struct in6_addr               in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr               in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

#define bswap_16(x) int_bswap_16(x)
#define bswap_32(x) int_bswap_32(x)
#define bswap_64(x) int_bswap_64(x)

/*****************************************************************************
  Description:
    Byte swap 16-bit value

  Parameters:
    __x - Value to be byte swapped

  Returns:
    Swapped value

  Remarks:

 *****************************************************************************/

static __inline uint16_t int_bswap_16(uint16_t x)
{
    return (x<<8) | (x>>8);
}

/*****************************************************************************
  Description:
    Byte swap 32-bit value

  Parameters:
    __x - Value to be byte swapped

  Returns:
    Swapped value

  Remarks:

 *****************************************************************************/

static __inline uint32_t int_bswap_32(uint32_t x)
{
    return (x>>24) | ((x>>8)&0xff00U) | ((x<<8)&0xff0000U) | (x<<24);
}

/*****************************************************************************
  Description:
    Byte swap 64-bit value

  Parameters:
    __x - Value to be byte swapped

  Returns:
    Swapped value

  Remarks:

 *****************************************************************************/

static __inline uint64_t int_bswap_64(uint64_t x)
{
    return (int_bswap_32((uint32_t)x)+(0ULL<<32)) | (int_bswap_32((uint32_t)(x>>32)));
}

/*****************************************************************************
  Description:
    Convert hex character into integer.

  Parameters:
    c - Character to be converted

  Returns:
    Integer value of character or -1 for error

  Remarks:

 *****************************************************************************/

static int hexval(unsigned char c)
{
    if (c < u'0')
    {
        return -1;
    }

    if (c <= u'9')
    {
        return (c - u'0');
    }

    c |= 32U;

    if ((c < u'a') || (c > u'f'))
    {
        return -1;
    }

    return (c - u'a' + 10U);
}

/*****************************************************************************
  Description:
    Size of sockaddr structure depending on IP version.

  Parameters:
    addr - Pointer to sockaddr structure to size.

  Returns:
    Size of structure or 0 for error.

  Remarks:

 *****************************************************************************/

static size_t sizeof_sockaddr(const struct sockaddr *addr)
{
    if (NULL == addr)
    {
        return 0;
    }

    if (AF_INET == addr->sa_family)
    {
        return sizeof(struct sockaddr_in);
    }
    else if (AF_INET6 == addr->sa_family)
    {
        return sizeof(struct sockaddr_in6);
    }
    else
    {
        /* Do nothing. */
    }

    return 0;
}

/*****************************************************************************
  Description:
    Convert IPv4 and IPv6 addresses from binary to text form.

  Parameters:
    af - Address family, either AF_INET or AF_INET6.
    a0 - Pointer to IP address in network byte order.
    s  - Pointer to string buffer to output to.
    l  - Size of buffer supplied.

  Returns:
    Pointer to converted string within supplied buffer or 0 for error (errno set).

  Remarks:
    errno:
        EAFNOSUPPORT
            af was not a valid address family.
        ENOSPC
            The converted address string would exceed the size given by size.

 *****************************************************************************/

const char *inet_ntop(int af, const void *a0, char *s, socklen_t l)
{
    const unsigned char *a = a0;

    switch ((unsigned)af)
    {
        case AF_INET:
        {
            if (snprintf(s, l, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]) < (int)l)
            {
                return s;
            }

            errno = ENOSPC;
            break;
        }

        case AF_INET6:
        {
            int i, j, max, best;
            char buf[100];

            static uint8_t transPrefix[12] = {0,0,0,0,0,0,0,0,0,0,0xffU,0xffU};

            if (0 != memcmp(a, transPrefix, 12))
            {
                (void)snprintf(buf, sizeof buf, "%x:%x:%x:%x:%x:%x:%x:%x",
                                        (256U*a[0])+a[1],(256U*a[2])+a[3],
                                        (256U*a[4])+a[5],(256U*a[6])+a[7],
                                        (256U*a[8])+a[9],(256U*a[10])+a[11],
                                        (256U*a[12])+a[13],(256U*a[14])+a[15]);
            }
            else
            {
                (void)snprintf(buf, sizeof buf, "%x:%x:%x:%x:%x:%x:%d.%d.%d.%d",
                                        (256U*a[0])+a[1],(256U*a[2])+a[3],
                                        (256U*a[4])+a[5],(256U*a[6])+a[7],
                                        (256U*a[8])+a[9],(256U*a[10])+a[11],
                                        a[12],a[13],a[14],a[15]);
            }

            /* Replace longest /(^0|:)[:0]{2,}/ with "::" */
            best = 0;
            max  = 2;
            for (i=0; '\0' != buf[i]; i++)
            {
                if ((0 != i) && (buf[i] != ':'))
                {
                    continue;
                }

                j = (int)strspn(buf+i, ":0");

                if (j>max)
                {
                    best = i;
                    max  = j;
                }
            }

            if (max > 3)
            {
                buf[best]   = ':';
                buf[best+1] = ':';
                (void)memmove(buf+best+2, buf+best+max, (i-best-max+1));
            }

            if (strlen(buf) < l)
            {
                (void)strcpy(s, buf);
                return s;
            }

            errno = ENOSPC;
            break;
        }

        default:
        {
            errno = EAFNOSUPPORT;
            break;
        }
    }

    return NULL;
}

/*****************************************************************************
  Description:
    Convert IPv4 and IPv6 addresses from text to binary form.

  Parameters:
    af - Address family, either AF_INET or AF_INET6.
    s  - Pointer to input IP address string.
    a0 - Pointer to output binary form IP address.

  Returns:
    -1 - Error, errno is set.
    0  - No valid IP address string found.
    1  - Success.

  Remarks:
    errno:
        EAFNOSUPPORT
            af was not a valid address family.

 *****************************************************************************/

int inet_pton(int af, const char *s, void *a0)
{
    uint16_t ip[8];
    unsigned char *a = a0;
    int i, j, v, brk = -1, need_v4 = 0;

    if (AF_INET == (unsigned)af)
    {
        for (i=0; i<4; i++)
        {
            v = 0;
            for (j=0; (j<3) && (0 != isdigit((int)s[j])); j++)
            {
                v = (10*v) + (s[j] - (char)'0');
            }

            if ((0 == j) || ((j > 1) && ('0' == s[0])) || (v > 255))
            {
                return 0;
            }

            a[i] = (unsigned char)v;

            if (('\0' == s[j]) && (3 == i))
            {
                return 1;
            }

            if ('.' != s[j])
            {
                return 0;
            }

            s = &s[j+1];
        }

        return 0;
    }
    else if (AF_INET6 != (unsigned)af)
    {
        errno = EAFNOSUPPORT;
        return -1;
    }
    else
    {
    }

    if (':' == s[0])
    {
        if (':' != s[1])
        {
            return 0;
        }

        s++;
    }

    for (i=0; ; i++)
    {
        if ((':' == s[0]) && (brk < 0))
        {
            brk = i;
            ip[(unsigned)i & 7U] = 0;

            if ('\0' == *++s)
            {
                break;
            }

            if (7 == i)
            {
                return 0;
            }

            continue;
        }

        v = 0;
        for (j=0; (j < 4) && ((hexval((unsigned)s[j])) >= 0); j++)
        {
            v = (16*v) + hexval((unsigned)s[j]);
        }

        if (0 == j)
        {
            return 0;
        }

        ip[(unsigned)i & 7U] = (uint16_t)v;

        if (('\0' == s[j]) && ((brk >= 0) || (7 == i)))
        {
            break;
        }

        if (7 == i)
        {
            return 0;
        }

        if (':' != s[j])
        {
            if (('.' != s[j]) || ((i < 6) && (brk < 0)))
            {
                return 0;
            }

            need_v4 = 1;

            i++;
            ip[(unsigned)i & 7U] = 0;

            break;
        }

        s = &s[j+1];
    }

    if (brk >= 0)
    {
        (void)memmove(ip+brk+7-i, ip+brk, (2*(i+1-brk)));

        for (j=0; j<(7-i); j++)
        {
            ip[brk + j] = 0;
        }
    }

    for (j=0; j<8; j++)
    {
        *a++ = (unsigned char)(ip[j] >> 8);
        *a++ = (unsigned char)(ip[j]);
    }

    if ((0 != need_v4) && (inet_pton((int)AF_INET, s, a-4) <= 0))
    {
        return 0;
    }

    return 1;
}

/*****************************************************************************
  Description:
    Create an endpoint for communication.

  Parameters:
    domain   - Communication domain, must be AF_INET or AF_INET6.
    type     - Socket type, must be SOCK_STREAM or SOCK_DGRAM.
    protocol - Specific socket protocol, should be zero or IPPROTO_TLS.

  Returns:
    Socket file descriptor or -1 for error (errno set).

  Remarks:
    errno:
        EAFNOSUPPORT
            The implementation does not support the specified address
            family.
        EINVAL
            Unknown protocol, or protocol family not available.
        EPROTONOSUPPORT
            The protocol type or the specified protocol is not
            supported within this domain.
        EFAULT
            System fault
        ENFILE The system-wide limit on the total number of open files
            has been reached.
        ENOMEM
            Insufficient memory is available.  The socket cannot be
            created until sufficient resources are freed.
        EBADMSG
            Bad message.

 *****************************************************************************/

int socket(int domain, int type, int protocol)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    WINC_SOCK_CTX *pSockCtx = NULL;

    if ((AF_INET != (unsigned)domain) && (AF_INET6 != (unsigned)domain))
    {
        errno = EAFNOSUPPORT;
        return -1;
    }

    if (SOCK_STREAM == (unsigned)type)
    {
        if (0 == protocol)
        {
            protocol = (int)IPPROTO_TCP;
        }

        if ((IPPROTO_TCP != (unsigned)protocol) && (IPPROTO_TLS != (unsigned)protocol))
        {
            errno = EPROTONOSUPPORT;
            return -1;
        }
    }
    else if (SOCK_DGRAM == (unsigned)type)
    {
        if (0 == protocol)
        {
            protocol = (int)IPPROTO_UDP;
        }

        if (IPPROTO_UDP != (unsigned)protocol)
        {
            errno = EPROTONOSUPPORT;
            return -1;
        }
    }
    else
    {
        errno = EINVAL;
        return -1;
    }

    if (WINC_DEVICE_INVALID_HANDLE == wincDevHandle)
    {
        errno = EFAULT;
        return -1;
    }

    pSockCtx = sockFindFreeSocket();

    if (NULL == pSockCtx)
    {
        errno = ENFILE;
        return -1;
    }

    if (false == sockCreateSocket(pSockCtx, (uint8_t)type, 0))
    {
        errno = ENFILE;
        return -1;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    pCmdReqBuffer = sockAllocCmdReq(128);

    if (NULL == pCmdReqBuffer)
    {
        sockUnlockSocket(pSockCtx);
        sockDestroySocket(pSockCtx);
        errno = ENOMEM;
        return -1;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        sockDestroySocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    (void)WINC_CmdSOCKO(cmdReqHandle, (uint8_t)type, (uint8_t)domain);

    if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        sockDestroySocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    pSockCtx->protocol = (uint8_t)protocol;
    pSockCtx->domain   = (uint8_t)domain;
    pSockCtx->readMode = (uint8_t)WINC_SOCKET_ASYNC_MODE_ACKED;

    WINC_VERBOSE_PRINT("Socket %08x -> %d\n", pSockCtx, WINC_SOCK_PTR_TO_HANDLE(pSockCtx));

    sockUnlockSocket(pSockCtx);
    return WINC_SOCK_PTR_TO_HANDLE(pSockCtx);
}

/*****************************************************************************
  Description:
    Shut down part of a full-duplex connection.

  Parameters:
    fd  - Socket file descriptor.
    how - Shutdown flags, must be SHUT_RDWR.

  Returns:
    0  - Success.
    -1 - Error, errno set.

  Remarks:
    errno:
        EBADF
            fd is not a valid file descriptor.
        ENOTSOCK
            The file descriptor fd does not refer to a socket.
        ENOMEM
            Insufficient memory is available.  The socket cannot be
            created until sufficient resources are freed.
        EBADMSG
            Bad message.

 *****************************************************************************/

int shutdown(int fd, int how)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    WINC_SOCK_CTX *pSockCtx = WINC_SOCK_HANDLE_TO_PTR(fd);

    if (NULL == pSockCtx)
    {
        errno = EBADF;
        return -1;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    if (0U == pSockCtx->sockId)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOTSOCK;
        return -1;
    }

    pCmdReqBuffer = sockAllocCmdReq(128);

    if (NULL == pCmdReqBuffer)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOMEM;
        return -1;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    (void)WINC_CmdSOCKCL(cmdReqHandle, pSockCtx->sockId);

    if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    sockUnlockSocket(pSockCtx);
    return 0;
}

/*****************************************************************************
  Description:
    Bind a name to a socket.

  Parameters:
    fd   - Socket file descriptor.
    addr - Pointer to socket address.
    len  - Length of socket address.

  Returns:
    0  - Success.
    -1 - Error, errno set.

  Remarks:
    errno:
        EFAULT
            System fault.
        ENOTSOCK
            The file descriptor fd does not refer to a socket.
        EAFNOSUPPORT
            The implementation does not support the specified address
            family.
        ENOMEM
            Insufficient memory is available.
        EBADMSG
            Bad message.

 *****************************************************************************/

int bind(int fd, const struct sockaddr *addr, socklen_t len)
{
    WINC_SOCK_CTX *pSockCtx = WINC_SOCK_HANDLE_TO_PTR(fd);
    const struct sockaddr_in *pSockBindAddr = (const struct sockaddr_in*)addr;

    if ((NULL == pSockCtx) || (NULL == addr) || (len < sizeof(struct sockaddr_in)))
    {
        errno = EFAULT;
        return -1;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    if (0U == pSockCtx->sockId)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOTSOCK;
        return -1;
    }

    if ((AF_INET != pSockBindAddr->sin_family) && (AF_INET6 != pSockBindAddr->sin_family))
    {
        sockUnlockSocket(pSockCtx);
        errno = EAFNOSUPPORT;
        return -1;
    }

    (void)memset(&pSockCtx->localEndPt, 0, sizeof(WINC_SOCK_END_PT));
    (void)memcpy(&pSockCtx->localEndPt, pSockBindAddr, len);

    /* Perform bind here for datagram sockets. */
    if (SOCK_DGRAM == pSockCtx->type)
    {
        WINC_CMD_REQ_HANDLE cmdReqHandle;
        void *pCmdReqBuffer;

        pCmdReqBuffer = sockAllocCmdReq(128);

        if (NULL == pCmdReqBuffer)
        {
            sockUnlockSocket(pSockCtx);
            errno = ENOMEM;
            return -1;
        }

        cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

        if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
        {
            sockFreeCmdReq(pCmdReqBuffer);
            sockUnlockSocket(pSockCtx);
            errno = EBADMSG;
            return -1;
        }

        (void)WINC_CmdSOCKBL(cmdReqHandle, pSockCtx->sockId, ntohs(pSockCtx->localEndPt.sin_port), 0);

        if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
        {
            sockFreeCmdReq(pCmdReqBuffer);
            sockUnlockSocket(pSockCtx);
            errno = EBADMSG;
            return -1;
        }

        if (false == sockAllocSocketBuffers(pSockCtx, WINC_SOCK_BUF_RX_SZ, WINC_SOCK_BUF_TX_SZ))
        {
            sockUnlockSocket(pSockCtx);
            errno = ENOMEM;
            return -1;
        }

        if (false == sockAllocUdpPktBuffers(pSockCtx, WINC_SOCK_BUF_RX_PKT_BUF_NUM, WINC_SOCK_BUF_TX_PKT_BUF_NUM))
        {
            sockUnlockSocket(pSockCtx);
            errno = ENOMEM;
            return -1;
        }
    }

    sockUnlockSocket(pSockCtx);
    return 0;
}

/*****************************************************************************
  Description:
    Listen for connections on a socket.

  Parameters:
    fd      - Socket file descriptor.
    backlog - Maximum number of pending connections, currently ignored.

  Returns:
    0  - Success.
    -1 - Error, errno set.

  Remarks:
    errno:
        EADDRINUSE
            The referred to by fd is not bound to a local port.
        EBADF
            The argument fd is not a valid file descriptor.
        EOPNOTSUPP
            The socket is not of a type that supports the listen() operation.

 *****************************************************************************/

int listen(int fd, int backlog)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    WINC_SOCK_CTX *pSockCtx = WINC_SOCK_HANDLE_TO_PTR(fd);

    if (NULL == pSockCtx)
    {
        errno = EBADF;
        return -1;
    }

    if ((unsigned)backlog > SOMAXCONN)
    {
        backlog = SOMAXCONN;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    if ((0U == pSockCtx->sockId) || (0U == pSockCtx->localEndPt.sin_port))
    {
        sockUnlockSocket(pSockCtx);
        errno = EADDRINUSE;
        return -1;
    }

    if (SOCK_STREAM != pSockCtx->type)
    {
        sockUnlockSocket(pSockCtx);
        errno = EOPNOTSUPP;
        return -1;
    }

    /* Perform bind of stream sockets to allow setting backlog. */

    pCmdReqBuffer = sockAllocCmdReq(128);

    if (NULL == pCmdReqBuffer)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOMEM;
        return -1;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    (void)WINC_CmdSOCKBL(cmdReqHandle, pSockCtx->sockId, ntohs(pSockCtx->localEndPt.sin_port), (uint8_t)backlog);

    if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    pSockCtx->listening = true;

    sockUnlockSocket(pSockCtx);
    return 0;
}

/*****************************************************************************
  Description:
    Accept a connection on a socket.

  Parameters:
    fd   - Socket file descriptor.
    addr - Pointer to a structure to filled in with the address of the peer socket.
    len  - Length of address structure.

  Returns:
    0  - Success.
    -1 - Error, errno set.

  Remarks:
    errno:
        EBADF
            The argument fd is not a valid file descriptor.
        ENOMEM
            Insufficient memory is available.
        EWOULDBLOCK
            The socket is marked nonblocking and no connections are
            present to be accepted.

 *****************************************************************************/

int accept(int fd, struct sockaddr *addr, socklen_t *len)
{
    WINC_SOCK_CTX *pSockCtx = WINC_SOCK_HANDLE_TO_PTR(fd);
    WINC_SOCK_CTX *pNewSockCtx;

    if (NULL == pSockCtx)
    {
        errno = EBADF;
        return -1;
    }

    pNewSockCtx = sockFindTCPPendingSocket(pSockCtx);

    if (NULL == pNewSockCtx)
    {
        errno = EWOULDBLOCK;
        return -1;
    }

    if (false == sockLockSocket(pNewSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    if ((NULL != addr) && (NULL != len))
    {
        socklen_t addrLen = (AF_INET == pNewSockCtx->remoteEndPt.sin_family) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

        if (*len > addrLen)
        {
            *len = addrLen;
        }

        (void)memcpy(addr, &pNewSockCtx->remoteEndPt, *len);

        *len = addrLen;
    }

    if (false == sockAllocSocketBuffers(pNewSockCtx, WINC_SOCK_BUF_RX_SZ, WINC_SOCK_BUF_TX_SZ))
    {
        sockUnlockSocket(pNewSockCtx);
        errno = ENOMEM;
        return -1;
    }

    pNewSockCtx->accepted = true;

    sockUnlockSocket(pNewSockCtx);
    return WINC_SOCK_PTR_TO_HANDLE(pNewSockCtx);
}

/*****************************************************************************
  Description:
    Initiate a connection on a socket.

  Parameters:
    fd   - Socket file descriptor.
    addr - Pointer to socket address.
    len  - Length of socket address.

  Returns:
    0  - Success.
    -1 - Error, errno set.

  Remarks:
    errno:
        EFAULT
            System fault.
        ENOTSOCK
            The file descriptor fd does not refer to a socket.
        EAFNOSUPPORT
            The implementation does not support the specified address
            family.
        ENOMEM
            Insufficient memory is available.
        EBADMSG
            Bad message.
        EINPROGRESS
            The socket is nonblocking and the connection cannot be
            completed immediately.

 *****************************************************************************/

int connect(int fd, const struct sockaddr *addr, socklen_t len)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle;
    void *pCmdReqBuffer;
    WINC_SOCK_CTX *pSockCtx = WINC_SOCK_HANDLE_TO_PTR(fd);
    const struct sockaddr_in *pSockConnAddr = (const struct sockaddr_in*)addr;
    WINC_TYPE typeRmtAddr;
    size_t lenRmtAddr;
    uintptr_t rmtAddr;

    if ((NULL == pSockCtx) || (NULL == addr) || (len < sizeof_sockaddr(addr)))
    {
        errno = EFAULT;
        return -1;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    if (0U == pSockCtx->sockId)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOTSOCK;
        return -1;
    }

    if ((AF_INET != pSockConnAddr->sin_family) && (AF_INET6 != pSockConnAddr->sin_family))
    {
        sockUnlockSocket(pSockCtx);
        errno = EAFNOSUPPORT;
        return -1;
    }

    (void)memset(&pSockCtx->remoteEndPt, 0, sizeof(WINC_SOCK_END_PT));
    (void)memcpy(&pSockCtx->remoteEndPt, pSockConnAddr, len);

    if (false == sockAllocSocketBuffers(pSockCtx, WINC_SOCK_BUF_RX_SZ, WINC_SOCK_BUF_TX_SZ))
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOMEM;
        return -1;
    }

    if (SOCK_DGRAM == pSockCtx->type)
    {
        if (false == sockAllocUdpPktBuffers(pSockCtx, WINC_SOCK_BUF_RX_PKT_BUF_NUM, WINC_SOCK_BUF_TX_PKT_BUF_NUM))
        {
            sockUnlockSocket(pSockCtx);
            errno = ENOMEM;
            return -1;
        }
    }

    pCmdReqBuffer = sockAllocCmdReq(128);

    if (NULL == pCmdReqBuffer)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOMEM;
        return -1;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    if (AF_INET == pSockConnAddr->sin_family)
    {
        typeRmtAddr = WINC_TYPE_IPV4ADDR;
        lenRmtAddr  = 4;
        rmtAddr     = (uintptr_t)&pSockCtx->remoteEndPt.v4.sin_addr;
    }
    else
    {
        typeRmtAddr = WINC_TYPE_IPV6ADDR;
        lenRmtAddr  = 16;
        rmtAddr     = (uintptr_t)&pSockCtx->remoteEndPt.v6.sin6_addr;
    }

    (void)WINC_CmdSOCKBR(cmdReqHandle, pSockCtx->sockId, typeRmtAddr, rmtAddr, lenRmtAddr, ntohs(pSockCtx->remoteEndPt.sin_port));

    if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    sockUnlockSocket(pSockCtx);
    errno = EINPROGRESS;
    return -1;
}

/*****************************************************************************
  Description:
    Receive a message from a socket.

  Parameters:
    fd    - Socket file descriptor.
    buf   - Pointer to buffer to receive data.
    len   - Length of data to receive.
    flags - Option for reception, currently ignored.

  Returns:
    The number of bytes received or -1 for error, see errno.
    If the return value is 0, the socket has performed an orderly shutdown.

  Remarks:
    errno:
        EBADF
            The argument fd is not a valid file descriptor.
        EFAULT
            System fault.
        ENOTSOCK
            The file descriptor fd does not refer to a socket.
        EWOULDBLOCK
            The socket is marked nonblocking and the receive operation
            would block.

 *****************************************************************************/

ssize_t recv(int fd, void *buf, size_t len, int flags)
{
    return recvfrom(fd, buf, len, flags, NULL, NULL);
}

/*****************************************************************************
  Description:
    Receive a message from a socket.

  Parameters:
    fd    - Socket file descriptor.
    buf   - Pointer to buffer to receive data.
    len   - Length of data to receive.
    flags - Option for reception, currently ignored.
    addr  - Pointer to a structure to receive the peer address.
    alen  - Pointer to length of address structure.

  Returns:
    The number of bytes received or -1 for error, see errno.
    If the return value is 0, the socket has performed an orderly shutdown.

  Remarks:
    errno:
        EBADF
            The argument fd is not a valid file descriptor.
        EFAULT
            System fault.
        ENOTSOCK
            The file descriptor fd does not refer to a socket.
        EWOULDBLOCK
            The socket is marked nonblocking and the receive operation
            would block.

 *****************************************************************************/

ssize_t recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t *alen)
{
    WINC_SOCK_CTX *pSockCtx = WINC_SOCK_HANDLE_TO_PTR(fd);
    ssize_t recvLen;
    size_t lenRemain = 0;
    socklen_t addrLen = 0;

    if (NULL == pSockCtx)
    {
        errno = EBADF;
        return -1;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    if (0U == pSockCtx->sockId)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOTSOCK;
        return -1;
    }

    if (0U == pSockCtx->recvBuffer.length)
    {
        if ((SOCK_STREAM == pSockCtx->type) && (false == pSockCtx->connected))
        {
            sockUnlockSocket(pSockCtx);
            return 0;
        }

        if (WINC_SOCKET_ASYNC_MODE_OFF == (WINC_SOCKET_ASYNC_MODE_TYPE)pSockCtx->readMode)
        {
            (void)sockRead(pSockCtx);
        }

        sockUnlockSocket(pSockCtx);
        errno = EWOULDBLOCK;
        return -1;
    }

    if ((SOCK_DGRAM == pSockCtx->type) && (NULL != pSockCtx->recvBuffer.pUdpPktBuffers))
    {
        if (0U == pSockCtx->recvBuffer.pUdpPktBuffers->pktDepth)
        {
            sockUnlockSocket(pSockCtx);
            errno = EWOULDBLOCK;
            return -1;
        }

        if (NULL != alen)
        {
            addrLen = *alen;

            if (addrLen > sizeof(WINC_SOCK_END_PT))
            {
                addrLen = sizeof(WINC_SOCK_END_PT);
            }

            *alen = addrLen;
        }
    }
    else if (NULL != alen)
    {
        *alen = 0;
    }
    else
    {
    }

    recvLen = sockBufferRead(&pSockCtx->recvBuffer, buf, len, addr, addrLen, ((unsigned)flags & MSG_PEEK) ? true : false, &lenRemain);

    if (-1 == recvLen)
    {
        errno = EFAULT;
    }

    if ((len > 0U) && (0 == recvLen))
    {
        if (false == pSockCtx->connected)
        {
            sockUnlockSocket(pSockCtx);
            return 0;
        }

        sockUnlockSocket(pSockCtx);
        errno = EWOULDBLOCK;
        return -1;
    }

    if (0U != ((unsigned)flags & MSG_TRUNC))
    {
        if (SOCK_DGRAM == pSockCtx->type)
        {
            recvLen = (len + lenRemain);
        }
        else
        {
            recvLen = lenRemain;
        }
    }

    WINC_VERBOSE_PRINT("RV %d\n", recvLen);

    if ((0U == ((unsigned)flags & MSG_PEEK)) && (recvLen > 0))
    {
        if (WINC_SOCKET_ASYNC_MODE_OFF == (WINC_SOCKET_ASYNC_MODE_TYPE)pSockCtx->readMode)
        {
            (void)sockRead(pSockCtx);
        }
        else if (WINC_SOCKET_ASYNC_MODE_ACKED == (WINC_SOCKET_ASYNC_MODE_TYPE)pSockCtx->readMode)
        {
            pSockCtx->nextSeqNum += (uint16_t)recvLen;

            if (false == pSockCtx->seqNumUpdateSent)
            {
                WINC_CMD_REQ_HANDLE cmdReqHandle;
                void *pCmdReqBuffer;

                pCmdReqBuffer = sockAllocCmdReq(128);

                if (NULL == pCmdReqBuffer)
                {
                    sockUnlockSocket(pSockCtx);
                    return -1;
                }

                cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128, 1, sockCmdRspCallbackHandlerSeqUpdate, (uintptr_t)pSockCtx);

                if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
                {
                    sockUnlockSocket(pSockCtx);
                    sockFreeCmdReq(pCmdReqBuffer);
                    return -1;
                }

                (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_ASYNC_NEXT_SN, WINC_TYPE_INTEGER_UNSIGNED, pSockCtx->nextSeqNum, 0);

                if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
                {
                    sockUnlockSocket(pSockCtx);
                    sockFreeCmdReq(pCmdReqBuffer);
                    return -1;
                }

                pSockCtx->seqNumUpdateSent = true;
            }
        }
        else
        {
        }
    }

    sockUnlockSocket(pSockCtx);
    return recvLen;
}

/*****************************************************************************
  Description:
    Send a message on a socket.

  Parameters:
    fd    - Socket file descriptor.
    buf   - Pointer to buffer containing data to send.
    len   - Length of data to send.
    flags - Option for transmission, currently ignored.

  Returns:
    The number of bytes sent or -1 for error, see errno.

  Remarks:
    errno:
        EBADF
            The argument fd is not a valid file descriptor.
        EFAULT
            System fault.
        ENOTSOCK
            The file descriptor fd does not refer to a socket.
        EWOULDBLOCK
            The socket is marked nonblocking and the send operation
            would block.
        ENOMEM
            Insufficient memory is available.
        EDESTADDRREQ
            The socket is not connection-mode, and no peer address is set.
        ENOTCONN
            The socket is not connected, and no target has been given.

 *****************************************************************************/

ssize_t send(int fd, const void *buf, size_t len, int flags)
{
    return sendto(fd, buf, len, flags, NULL, 0);
}

/*****************************************************************************
  Description:
    Send a message on a socket.

  Parameters:
    fd    - Socket file descriptor.
    buf   - Pointer to buffer containing data to send.
    len   - Length of data to send.
    flags - Option for transmission, currently ignored.
    addr  - Pointer to address structure.
    alen  - Length of address structure.

  Returns:
    The number of bytes sent or -1 for error, see errno.

  Remarks:
    errno:
        EBADF
            The argument fd is not a valid file descriptor.
        EFAULT
            System fault.
        ENOTSOCK
            The file descriptor fd does not refer to a socket.
        EWOULDBLOCK
            The socket is marked nonblocking and the send operation
            would block.
        ENOMEM
            Insufficient memory is available.
        EMSGSIZE
            The argument data is too long to pass atomically through the underlying protocol
        EDESTADDRREQ
            The socket is not connection-mode, and no peer address is set.
        ENOTCONN
            The socket is not connected, and no target has been given.

 *****************************************************************************/

ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t alen)
{
    WINC_SOCK_CTX *pSockCtx = WINC_SOCK_HANDLE_TO_PTR(fd);
    const WINC_SOCK_END_PT *pDestAddr = NULL;

    if (NULL == pSockCtx)
    {
        errno = EBADF;
        return -1;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    if (0U == pSockCtx->sockId)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOTSOCK;
        return -1;
    }

    if (SOCK_DGRAM == pSockCtx->type)
    {
        if (NULL != addr)
        {
            if (AF_UNSPEC == pSockCtx->remoteEndPt.sin_family)
            {
                if (alen > sizeof(WINC_SOCK_END_PT))
                {
                    alen = sizeof(WINC_SOCK_END_PT);
                }

                (void)memset(&pSockCtx->remoteEndPt, 0, sizeof(WINC_SOCK_END_PT));
                (void)memcpy(&pSockCtx->remoteEndPt, addr, alen);

                if (false == sockAllocSocketBuffers(pSockCtx, WINC_SOCK_BUF_RX_SZ, WINC_SOCK_BUF_TX_SZ))
                {
                    sockUnlockSocket(pSockCtx);
                    errno = ENOMEM;
                    return -1;
                }

                if (SOCK_DGRAM == pSockCtx->type)
                {
                    if (false == sockAllocUdpPktBuffers(pSockCtx, WINC_SOCK_BUF_RX_PKT_BUF_NUM, WINC_SOCK_BUF_TX_PKT_BUF_NUM))
                    {
                        sockUnlockSocket(pSockCtx);
                        errno = ENOMEM;
                        return -1;
                    }
                }
            }

            if (alen < sizeof_sockaddr(addr))
            {
                sockUnlockSocket(pSockCtx);
                errno = EDESTADDRREQ;
                return -1;
            }

            pDestAddr = (const WINC_SOCK_END_PT*)(const void*)addr;
        }
        else
        {
            if (false == pSockCtx->connected)
            {
                sockUnlockSocket(pSockCtx);
                errno = EDESTADDRREQ;
                return -1;
            }

            pDestAddr = &pSockCtx->remoteEndPt;
        }
    }
    else
    {
        if (false == pSockCtx->connected)
        {
            sockUnlockSocket(pSockCtx);
            errno = ENOTCONN;
            return -1;
        }
    }

    if ((pSockCtx->sendBuffer.length + len) > pSockCtx->sendBuffer.totalSize)
    {
        (void)sockWrite(pSockCtx);
        sockUnlockSocket(pSockCtx);
        errno = EWOULDBLOCK;
        return -1;
    }

    if ((SOCK_DGRAM == pSockCtx->type) && (NULL != pSockCtx->sendBuffer.pUdpPktBuffers))
    {
        size_t maxLen;

        if (AF_INET == pSockCtx->remoteEndPt.sin_family)
        {
            maxLen = MAX_UDP_IPV4_SOCK_PAYLOAD_SZ;
        }
        else
        {
            maxLen = MAX_UDP_IPV6_SOCK_PAYLOAD_SZ;
        }

        if (len > maxLen)
        {
            sockUnlockSocket(pSockCtx);
            errno = EMSGSIZE;
            return -1;
        }
    }

    if (false == sockBufferWrite(&pSockCtx->sendBuffer, buf, len, pDestAddr, false))
    {
        sockUnlockSocket(pSockCtx);
        errno = EWOULDBLOCK;
        return -1;
    }

    WINC_VERBOSE_PRINT("SND %d\n", len);

    (void)sockWrite(pSockCtx);

    sockUnlockSocket(pSockCtx);
    return (ssize_t)len;
}

/*****************************************************************************
  Description:
    Network address and service translation.

  Parameters:
    host - Pointer to host to translate.
    serv - Pointer to service to translate, currently ignored.
    hint - Hint to affect the translation.
    res  - Pointer to results pointer.

  Returns:
    EAI_NONAME
        The node or service is not known.
    EAI_AGAIN
        The name server returned a temporary failure indication. Try again later.
    EAI_SYSTEM
        Other system error; errno is set to indicate the error.
    EAI_MEMORY
        Out of memory.
    EAI_SERVICE
        The requested service is not available for the requested socket type.

  Remarks:
    errno:
        EFAULT
            System fault.
        EBADMSG
            Bad message.

    The getaddrinfo() function allocates and initializes a linked
    list of addrinfo structures, one for each network address that
    matches node and service, subject to any restrictions imposed by
    hints, and returns a pointer to the start of the list in res.
    The items in the linked list are linked by the ai_next field.

    The freeaddrinfo() function frees the memory that was allocated
    for the dynamically allocated linked list res.

 *****************************************************************************/

int getaddrinfo(const char *host, const char *serv, const struct addrinfo *hint, struct addrinfo **res)
{
    WINC_DNS_REQ *pDnsRequest;

    if (NULL != host)
    {
        WINC_CMD_REQ_HANDLE cmdReqHandle;
        void *pCmdReqBuffer;
        size_t hostNameLen;

        hostNameLen = strnlen(host, 256);

        if (0U == hostNameLen)
        {
            return EAI_NONAME;
        }

        if (NULL != pCurrentDnsRequest)
        {
            if ((hostNameLen == pCurrentDnsRequest->hostNameLen) && (0 == memcmp(host, pCurrentDnsRequest->hostName, pCurrentDnsRequest->hostNameLen)))
            {
                if (NULL == pCurrentDnsRequest->pRes)
                {
                    return EAI_AGAIN;
                }
                else if (NULL != res)
                {
                    *res = pCurrentDnsRequest->pRes;
                    return 0;
                }
                else
                {
                    errno = EFAULT;
                    return EAI_SYSTEM;
                }
            }
        }

        pDnsRequest = dnsAllocRequest(hostNameLen);

        if (NULL == pDnsRequest)
        {
            return EAI_MEMORY;
        }

        pCurrentDnsRequest = pDnsRequest;

        pCmdReqBuffer = sockAllocCmdReq(128U+hostNameLen);

        if (NULL == pCmdReqBuffer)
        {
            dnsFreeRequest(pDnsRequest);
            return EAI_MEMORY;
        }

        (void)memcpy(pDnsRequest->hostName, host, hostNameLen);
        pDnsRequest->hostNameLen = hostNameLen;
        pDnsRequest->recordType  = WINC_CONST_DNS_TYPE_A;

        if (NULL != hint)
        {
            if (AF_INET6 == (unsigned)hint->ai_family)
            {
                pDnsRequest->recordType = WINC_CONST_DNS_TYPE_AAAA;
            }
        }

        cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 128U+hostNameLen, 1, dnsCmdRspCallbackHandler, (uintptr_t)pDnsRequest);

        if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
        {
            sockFreeCmdReq(pCmdReqBuffer);
            dnsFreeRequest(pDnsRequest);
            errno = EBADMSG;
            return EAI_SYSTEM;
        }

        (void)WINC_CmdDNSRESOLV(cmdReqHandle, pDnsRequest->recordType, pDnsRequest->hostName, pDnsRequest->hostNameLen);

        if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
        {
            sockFreeCmdReq(pCmdReqBuffer);
            dnsFreeRequest(pDnsRequest);
            errno = EBADMSG;
            return EAI_SYSTEM;
        }

        return EAI_AGAIN;
    }

    if (NULL != serv)
    {
        return EAI_SERVICE;
    }

    return EAI_NONAME;
}

/*****************************************************************************
  Description:
    Frees the memory allocated by getaddrinfo().

  Parameters:
    p - Pointer to address linked list returned by getaddrinfo().

  Returns:
    None.

  Remarks:
    None.

 *****************************************************************************/

void freeaddrinfo(const struct addrinfo *p)
{
    if (NULL == p)
    {
        return;
    }

    if (p == pCurrentDnsRequest->pRes)
    {
        dnsFreeRequest(pCurrentDnsRequest);

        pCurrentDnsRequest = NULL;
    }
}

/*****************************************************************************
  Description:
    Set options on sockets.

  Parameters:
    fd      - Socket file descriptor.
    level   - Option level.
    optname - Option name.
    optval  - Option value.
    optlen  - Length of option value.

  Returns:
    0  - Success.
    -1 - Error, errno set.

  Remarks:
    errno:
        EBADF
            The argument fd is not a valid file descriptor.
        ENOPROTOOPT
            The option is unknown at the level indicated.
        EINVAL
            optlen invalid.
        ENOMEM
            Insufficient memory is available.
        EBADMSG
            Bad message.

    Socket options:
        IP supports some protocol-specific socket options that can be set
        with setsockopt.  The socket option level for IP is IPPROTO_IP.
        A boolean integer flag is zero when it is false, otherwise true.
        Unless otherwise noted, optval is a pointer to an int.

        When an invalid socket option is specified, setsockopt fail with
        the error ENOPROTOOPT.

        IP_TOS
            Set or receive the Type-Of-Service (TOS) field that is
            sent with every IP packet originating from this socket.
            It is used to prioritize packets on the network.  TOS is an
            int.  There are some standard TOS flags defined:
            IPTOS_LOWDELAY to minimize delays for interactive traffic,
            IPTOS_THROUGHPUT to optimize throughput, IPTOS_RELIABILITY
            to optimize for reliability.
            At most one of these TOS values can be specified.  Other
            bits are invalid and shall be cleared.

        IP_ADD_MEMBERSHIP
            Join a multicast group.  Argument is an ip_mreqn
            structure.

                struct ip_mreqn {
                    struct in_addr imr_multiaddr;
                    struct in_addr imr_address;
                    int            imr_ifindex;
                };

            imr_multiaddr contains the address of the multicast group
            the application wants to join or leave.  It must be a
            valid multicast address (or setsockopt fails with the
            error EINVAL).

        IPV6_ADD_MEMBERSHIP
            Control membership in multicast groups.  Argument is a
            pointer to a struct ipv6_mreq.


        The socket options listed below can be set by using setsockopt
        with the socket level set to SOL_SOCKET for all sockets.

        SO_ASYNC_MODE
            Set the asynchronous read mode.  The argument is a integer.
            A value of 0 turns this mode off, 1 enables a simple protocol
            and 2 enables an acknowledged protocol.

        SO_KEEPALIVE
            Enable sending of keep-alive messages on connection-
            oriented sockets.  Expects an integer boolean flag.

        SO_LINGER
            Sets the SO_LINGER option.  The argument is a integer.

        SO_RCVBUF
            Sets the maximum socket receive buffer in bytes.

        SO_SNDBUF
            Sets the maximum socket send buffer in bytes.


        To set a TCP socket option, call setsockopt to write the option with
        the option level argument set to IPPROTO_TCP.

        TCP_NODELAY
            If set, disable the Nagle algorithm. This means that segments are
            always sent as soon as possible, even if there is only a small
            amount of data. When not set, data is buffered until there is a
            sufficient amount to send out, thereby avoiding the frequent
            sending of small packets, which results in poor utilization of
            the network.


        To set a TCP(TLS) socket option, call setsockopt to write the option with
        the option level argument set to IPPROTO_TLS.

        TLS_CONF_IDX
            Sets the TLS configuration index.

 *****************************************************************************/

int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
    WINC_CMD_REQ_HANDLE cmdReqHandle = WINC_CMD_REQ_INVALID_HANDLE;
    void *pCmdReqBuffer = NULL;
    WINC_SOCK_CTX *pSockCtx = WINC_SOCK_HANDLE_TO_PTR(fd);
    int locErrno = 0;

    if (NULL == pSockCtx)
    {
        errno = EBADF;
        return -1;
    }

    if (false == sockLockSocket(pSockCtx))
    {
        errno = EFAULT;
        return -1;
    }

    pCmdReqBuffer = sockAllocCmdReq(256);

    if (NULL == pCmdReqBuffer)
    {
        sockUnlockSocket(pSockCtx);
        errno = ENOMEM;
        return -1;
    }

    cmdReqHandle = WINC_CmdReqInit(pCmdReqBuffer, 256, 2, sockCmdRspCallbackHandler, (uintptr_t)pSockCtx);

    if (WINC_CMD_REQ_INVALID_HANDLE == cmdReqHandle)
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    locErrno = ENOPROTOOPT;

    if (IPPROTO_IP == (unsigned)level)
    {
        switch ((unsigned)optname)
        {
            case IP_TOS:
            {
                if (sizeof(int) == optlen)
                {
                    (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_IP_TOS, WINC_TYPE_INTEGER, (uintptr_t)*((const int*)optval), 0);
                    locErrno = 0;
                }
                else
                {
                    locErrno = EINVAL;
                }
                break;
            }

            case IP_ADD_MEMBERSHIP:
            case IPV6_ADD_MEMBERSHIP:
            {
                WINC_TYPE typeMcAddr;
                size_t lenMcAddr;
                uintptr_t mcAddr;

                if (NULL == optval)
                {
                    break;
                }

                (void)memset(&pSockCtx->remoteEndPt, 0, sizeof(WINC_SOCK_END_PT));

                if ((IP_ADD_MEMBERSHIP == (unsigned)optname) && (sizeof(struct ip_mreqn) == optlen) && (AF_INET == pSockCtx->localEndPt.sin_family))
                {
                    const struct ip_mreqn *addr = optval;

                    if (224U != ((htonl(addr->imr_multiaddr.s_addr) >> 24) & 0xf0U))
                    {
                        locErrno = EINVAL;
                        break;
                    }

                    typeMcAddr = WINC_TYPE_IPV4ADDR;
                    lenMcAddr  = 4;
                    mcAddr     = (uintptr_t)&addr->imr_multiaddr;

                    (void)memcpy((uint8_t*)&pSockCtx->remoteEndPt.v4.sin_addr, (uint8_t*)mcAddr, lenMcAddr);
                }
                else if ((IPV6_ADD_MEMBERSHIP == optname) && (sizeof(struct ipv6_mreq) == optlen) && (AF_INET6 == pSockCtx->localEndPt.sin_family))
                {
                    const struct ipv6_mreq *addr = optval;

                    if (0xffU != addr->ipv6mr_multiaddr.__in6_union.__s6_addr[0])
                    {
                        locErrno = EINVAL;
                        break;
                    }

                    typeMcAddr = WINC_TYPE_IPV6ADDR;
                    lenMcAddr  = 16;
                    mcAddr     = (uintptr_t)&addr->ipv6mr_multiaddr;

                    (void)memcpy((uint8_t*)&pSockCtx->remoteEndPt.v6.sin6_addr, (uint8_t*)mcAddr, lenMcAddr);
                }
                else
                {
                    break;
                }

                pSockCtx->remoteEndPt.sin_family = pSockCtx->localEndPt.sin_family;
                pSockCtx->remoteEndPt.sin_port   = pSockCtx->localEndPt.sin_port;

                if (false == sockAllocSocketBuffers(pSockCtx, WINC_SOCK_BUF_RX_SZ, WINC_SOCK_BUF_TX_SZ))
                {
                    locErrno = ENOMEM;
                    break;
                }

                if (false == sockAllocUdpPktBuffers(pSockCtx, WINC_SOCK_BUF_RX_PKT_BUF_NUM, WINC_SOCK_BUF_TX_PKT_BUF_NUM))
                {
                    locErrno = ENOMEM;
                    break;
                }

                (void)WINC_CmdSOCKBM(cmdReqHandle, pSockCtx->sockId, typeMcAddr, mcAddr, lenMcAddr, ntohs(pSockCtx->localEndPt.sin_port));
                locErrno = 0;
                break;
            }

            default:
            {
                WINC_VERBOSE_PRINT("Set socket, unknown IP protocol option %d\n", optname);
                break;
            }
        }
    }
    else if (SOL_SOCKET == (unsigned)level)
    {
        switch (optname)
        {
            case SO_SNDBUF:
            {
                if (sizeof(int) == optlen)
                {
                    (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_SO_SNDBUF, WINC_TYPE_INTEGER, (uintptr_t)*((const int*)optval), 0);
                    locErrno = 0;
                }
                else
                {
                    locErrno = EINVAL;
                }
                break;
            }

            case SO_RCVBUF:
            {
                if (sizeof(int) == optlen)
                {
                    (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_SO_RCVBUF, WINC_TYPE_INTEGER, (uintptr_t)*((const int*)optval), 0);
                    locErrno = 0;
                }
                else
                {
                    locErrno = EINVAL;
                }
                break;
            }

            case SO_KEEPALIVE:
            {
                if (sizeof(int) == optlen)
                {
                    (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_SO_KEEPALIVE, WINC_TYPE_INTEGER, (uintptr_t)*((const int*)optval), 0);
                    locErrno = 0;
                }
                else
                {
                    locErrno = EINVAL;
                }
                break;
            }

            case SO_LINGER:
            {
                if (sizeof(int) == optlen)
                {
                    (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_SO_LINGER, WINC_TYPE_INTEGER, (uintptr_t)*((const int*)optval), 0);
                    locErrno = 0;
                }
                else
                {
                    locErrno = EINVAL;
                }
                break;
            }

            case SO_ASYNC_MODE:
            {
                locErrno = EINVAL;

                if (sizeof(int) == optlen)
                {
                    WINC_SOCKET_ASYNC_MODE_TYPE readMode = *((const int*)optval);

                    if (readMode <= WINC_SOCKET_ASYNC_MODE_ACKED)
                    {
                        pSockCtx->readMode = *((const int*)optval);
                        (void)sockAsyncModeConf(cmdReqHandle, pSockCtx);
                        locErrno = 0;
                    }
                }
                break;
            }

            default:
            {
                WINC_VERBOSE_PRINT("Set socket, unknown socket option %d\n", optname);
                break;
            }
        }
    }
    else if (IPPROTO_TCP == (unsigned)level)
    {
        if (SOCK_STREAM == pSockCtx->type)
        {
            switch (optname)
            {
                case TCP_NODELAY:
                {
                    if (sizeof(int) == optlen)
                    {
                        (void)WINC_CmdSOCKC(cmdReqHandle, pSockCtx->sockId, WINC_CFG_PARAM_ID_SOCK_SO_NODELAY, WINC_TYPE_INTEGER, (uintptr_t)*((const int*)optval), 0);
                        locErrno = 0;
                    }
                    else
                    {
                        locErrno = EINVAL;
                    }
                    break;
                }

                default:
                {
                    WINC_VERBOSE_PRINT("Set socket, unknown TCP protocol option %d\n", optname);
                    break;
                }
            }
        }
    }
    else if (IPPROTO_TLS == (unsigned)level)
    {
        if ((SOCK_STREAM == pSockCtx->type) && (IPPROTO_TLS == pSockCtx->protocol))
        {
            switch (optname)
            {
                case TLS_CONF_IDX:
                {
                    if (sizeof(int) == optlen)
                    {
                        pSockCtx->tlsPending = true;
                        (void)WINC_CmdSOCKTLS(cmdReqHandle, pSockCtx->sockId, (uint8_t)*((const int*)optval));
                        locErrno = 0;
                    }
                    else
                    {
                        locErrno = EINVAL;
                    }
                    break;
                }

                default:
                {
                    WINC_VERBOSE_PRINT("Set socket, unknown TLS protocol option %d\n", optname);
                    break;
                }
            }
        }
    }
    else
    {
    }

    if (0 != locErrno)
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = locErrno;
        return -1;
    }

    if (false == WINC_DevTransmitCmdReq(wincDevHandle, cmdReqHandle))
    {
        sockFreeCmdReq(pCmdReqBuffer);
        sockUnlockSocket(pSockCtx);
        errno = EBADMSG;
        return -1;
    }

    sockUnlockSocket(pSockCtx);
    return 0;
}

/*****************************************************************************
  Description:
    Convert 16-bit value from host to network byte order.

  Parameters:
    n - Value to convert.

  Returns:
    Converted value.

  Remarks:
    None.

 *****************************************************************************/

uint16_t htons(uint16_t n)
{
    union { int i; char c; } u = { 1 };
    return (0 != u.c) ? bswap_16(n) : n;
}

/*****************************************************************************
  Description:
    Convert 32-bit value from host to network byte order.

  Parameters:
    n - Value to convert.

  Returns:
    Converted value.

  Remarks:
    None.

 *****************************************************************************/

uint32_t htonl(uint32_t n)
{
    union { int i; char c; } u = { 1 };
    return (0 != u.c) ? bswap_32(n) : n;
}

/*****************************************************************************
  Description:
    Convert 16-bit value from network to host byte order.

  Parameters:
    n - Value to convert.

  Returns:
    Converted value.

  Remarks:

 *****************************************************************************/

uint16_t ntohs(uint16_t n)
{
    union { int i; char c; } u = { 1 };
    return u.c ? bswap_16(n) : n;
}

/*****************************************************************************
  Description:
    Convert 32-bit value from network to host byte order.

  Parameters:
    n - Value to convert.

  Returns:
    Converted value.

  Remarks:

 *****************************************************************************/

uint32_t ntohl(uint32_t n)
{
    union { int i; char c; } u = { 1 };
    return u.c ? bswap_32(n) : n;
}
