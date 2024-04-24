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

#ifndef WINC_DEV_H
#define WINC_DEV_H

#include "conf_winc_dev.h"
#include "winc_cmds.h"
#include "winc_sdio_drv.h"
#include "winc_debug.h"

/* Driver version number. */
#define WINC_DEV_DRIVER_VERSION_MAJOR_NO    2
#define WINC_DEV_DRIVER_VERSION_MINOR_NO    0
#define WINC_DEV_DRIVER_VERSION_PATCH_NO    0

/* Definition of WINC device handle type. */
typedef uintptr_t WINC_DEVICE_HANDLE;

/* Value of an invalid WINC device handle. */
#define WINC_DEVICE_INVALID_HANDLE      0

/*****************************************************************************
  Description:
    Command request event types.

  Remarks:

 *****************************************************************************/

typedef enum
{                                               /* eventArg: */
    WINC_DEV_CMDREQ_EVENT_TX_COMPLETE,
    WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE,      /* WINC_DEV_EVENT_COMPLETE_ARGS */
    WINC_DEV_CMDREQ_EVENT_CMD_STATUS,           /* WINC_DEV_EVENT_STATUS_ARGS */
    WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED,         /* WINC_DEV_EVENT_RSP_ELEMS */
} WINC_DEV_CMDREQ_EVENT_TYPE;

/*****************************************************************************
  Description:
    WINC device initialisation structure.

  Remarks:

 *****************************************************************************/

typedef struct
{
    uint8_t *pReceiveBuffer;
    size_t  receiveBufferSize;
} WINC_DEV_INIT;

/*****************************************************************************
  Description:
    Structure describing source command for events.

  Remarks:

 *****************************************************************************/

typedef struct
{
    uint8_t idx;
    uint8_t numParams;
    uint8_t *pParams;
} WINC_DEV_EVENT_SRC_CMD;

/*****************************************************************************
  Description:
    Structure describing a command response status event.

  Remarks:

 *****************************************************************************/

typedef struct
{
    uint16_t                rspCmdId;
    uint16_t                status;
    uint16_t                seqNum;
    WINC_DEV_EVENT_SRC_CMD  srcCmd;
} WINC_DEV_EVENT_STATUS_ARGS;

/*****************************************************************************
  Description:
    Structure describing a complete command request burst.

  Remarks:

 *****************************************************************************/

typedef struct
{
    uint8_t                 numCmds;
    uint8_t                 numErrors;
} WINC_DEV_EVENT_COMPLETE_ARGS;

/*****************************************************************************
  Description:
    Structure describing a command response or an AEC event.

  Remarks:

 *****************************************************************************/

typedef struct
{
    uint16_t                rspCmdId;
    uint16_t                rspId;
    uint8_t                 numElems;
    WINC_DEV_EVENT_SRC_CMD  srcCmd;
    WINC_DEV_PARAM_ELEM     elems[10];
} WINC_DEV_EVENT_RSP_ELEMS;

/* Command response event callback type definition. */
typedef void (*WINC_DEV_CMD_RSP_CB)(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle, WINC_DEV_CMDREQ_EVENT_TYPE event, uintptr_t eventArg);

/* AEC event callback type definition. */
typedef void (*WINC_DEV_AEC_RSP_CB)(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_DEV_EVENT_RSP_ELEMS *pElems);

/* WINC IRQ event function check type definition. */
typedef bool (*WINC_DEV_EVENT_CHECK_FP)(void);

/* Helper macro to combine common structure 16-bit split bytes into one value. */
#define WINC_FIELD_UNPACK_16(field)     (((uint16_t)field##_h) << 8) | field##_l;

#ifdef WINC_DEV_CACHE_LINE_SIZE
#define WINC_DEV_CACHE_GET_SIZE(size)       ((size + (WINC_DEV_CACHE_LINE_SIZE-1)) & ~(WINC_DEV_CACHE_LINE_SIZE-1))
#else
#define WINC_DEV_CACHE_GET_SIZE(size)       (size)
#endif

/*****************************************************************************
                        WINC Device Module API
 *****************************************************************************/

WINC_DEVICE_HANDLE WINC_DevInit(WINC_DEV_INIT *pInitData);
void WINC_DevDeinit(WINC_DEVICE_HANDLE devHandle);
void WINC_DevSetDebugPrintf(WINC_DEBUG_PRINTF_FP pfPrintf);
bool WINC_DevTransmitCmdReq(WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle);
bool WINC_DevUpdateEvent(WINC_DEVICE_HANDLE devHandle);
bool WINC_DevHandleEvent(WINC_DEVICE_HANDLE devHandle, WINC_DEV_EVENT_CHECK_FP pfEventIntCheck);
bool WINC_DevAECCallbackRegister(WINC_DEVICE_HANDLE devHandle, WINC_DEV_AEC_RSP_CB pfAecRspCallback, uintptr_t aecRspCallbackCtx);
bool WINC_DevAECCallbackDeregister(WINC_DEVICE_HANDLE devHandle, WINC_DEV_AEC_RSP_CB pfAecRspCallback);
bool WINC_DevUnpackElements(uint8_t numTlvs, uint8_t *pTLVBytes, WINC_DEV_PARAM_ELEM *pElems);

#endif /* WINC_DEV_H */
