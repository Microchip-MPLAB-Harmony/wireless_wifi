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

#ifndef WINC_DEV_H
#define WINC_DEV_H

#include "conf_winc_dev.h"
#include "winc_cmds.h"
#include "winc_sdio_drv.h"
#include "winc_debug.h"
#include "winc_tables.h"

#if __STDC_VERSION__ >= 201112L
#define STR_LIT_U(STR)              U##STR
#else
#define STR_LIT_U(STR)              STR
#endif

/* Driver version number. */
#define WINC_DEV_DRIVER_VERSION_MAJOR_NO    3
#define WINC_DEV_DRIVER_VERSION_MINOR_NO    1
#define WINC_DEV_DRIVER_VERSION_PATCH_NO    0

/* Definition of WINC device handle type. */
typedef uintptr_t WINC_DEVICE_HANDLE;

/* Value of an invalid WINC device handle. */
#define WINC_DEVICE_INVALID_HANDLE          0U

#ifndef WINC_CONF_LOCK_STORAGE
#define WINC_CONF_LOCK_STORAGE(NAME)
#endif
#ifndef WINC_CONF_LOCK_CREATE
#define WINC_CONF_LOCK_CREATE(NAME)
#endif
#ifndef WINC_CONF_LOCK_DESTROY
#define WINC_CONF_LOCK_DESTROY(NAME)
#endif
#ifndef WINC_CONF_LOCK_ENTER
#define WINC_CONF_LOCK_ENTER(NAME)          true
#endif
#ifndef WINC_CONF_LOCK_LEAVE
#define WINC_CONF_LOCK_LEAVE(NAME)
#endif

/*****************************************************************************
  Description:
    Command request event types.

  Remarks:

 *****************************************************************************/

typedef enum
{
    WINC_DEV_CMDREQ_EVENT_TX_COMPLETE,
    WINC_DEV_CMDREQ_EVENT_STATUS_COMPLETE,      /* WINC_DEV_EVENT_COMPLETE_ARGS */
    WINC_DEV_CMDREQ_EVENT_CMD_STATUS,           /* WINC_DEV_EVENT_STATUS_ARGS */
    WINC_DEV_CMDREQ_EVENT_RSP_RECEIVED,         /* WINC_DEV_EVENT_RSP_ELEMS */
} WINC_DEV_CMDREQ_EVENT_TYPE;

/*****************************************************************************
  Description:
    Device bus state types.

  Remarks:

 *****************************************************************************/

typedef enum
{
    WINC_DEV_BUS_STATE_UNKNOWN,
    WINC_DEV_BUS_STATE_ERROR,
    WINC_DEV_BUS_STATE_IDLE,
    WINC_DEV_BUS_STATE_ACTIVE,
} WINC_DEV_BUS_STATE_TYPE;

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
typedef void (*WINC_DEV_AEC_RSP_CB)(uintptr_t context, WINC_DEVICE_HANDLE devHandle, const WINC_DEV_EVENT_RSP_ELEMS *const pElems);

/* Receive event intercept callback type definition. */
typedef void (*WINC_DEV_RX_INTERCEPT_CB)(uintptr_t context, WINC_DEVICE_HANDLE devHandle, WINC_COMMAND_MSG_TYPE msgType, uint8_t *pMsg, size_t msgLength);

/* WINC IRQ event function check type definition. */
typedef bool (*WINC_DEV_EVENT_CHECK_FP)(void);

/* Helper macro to combine common structure 16-bit split bytes into one value. */
#define WINC_FIELD_UNPACK_16(field)     (((uint16_t)(field##_h)) << 8) | (field##_l)

#ifdef WINC_DEV_CACHE_LINE_SIZE
#define WINC_DEV_CACHE_GET_SIZE(size)       (((size) + (WINC_DEV_CACHE_LINE_SIZE-1U)) & ~(WINC_DEV_CACHE_LINE_SIZE-1U))
#define WINC_DEV_CACHE_ATTRIB               __attribute__((aligned(WINC_DEV_CACHE_LINE_SIZE)))
#else
#define WINC_DEV_CACHE_GET_SIZE(size)       (size)
#define WINC_DEV_CACHE_ATTRIB
#endif

/*****************************************************************************
                        WINC Device Module API
 *****************************************************************************/

WINC_DEVICE_HANDLE WINC_DevInit(const WINC_DEV_INIT *pInitData);
void WINC_DevDeinit(WINC_DEVICE_HANDLE devHandle);
void WINC_DevSetDebugPrintf(WINC_DEBUG_PRINTF_FP pfPrintf);
bool WINC_DevTransmitCmdReq(WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle);
bool WINC_DevUpdateEvent(WINC_DEVICE_HANDLE devHandle);
bool WINC_DevHandleEvent(WINC_DEVICE_HANDLE devHandle, WINC_DEV_EVENT_CHECK_FP pfEventIntCheck);
bool WINC_DevAECCallbackRegister(WINC_DEVICE_HANDLE devHandle, WINC_DEV_AEC_RSP_CB pfAecRspCallback, uintptr_t aecRspCallbackCtx);
bool WINC_DevAECCallbackDeregister(WINC_DEVICE_HANDLE devHandle, WINC_DEV_AEC_RSP_CB pfAecRspCallback);
bool WINC_DevUnpackElements(uint8_t numTlvs, const uint8_t *pTLVBytes, WINC_DEV_PARAM_ELEM *pElems);
bool WINC_DevInterceptCallbackRegister(WINC_DEVICE_HANDLE devHandle, WINC_DEV_RX_INTERCEPT_CB pfInterceptCallback, uintptr_t interceptCallbackCtx);
WINC_DEV_BUS_STATE_TYPE WINC_DevBusStateGet(WINC_DEVICE_HANDLE devHandle);
bool WINC_DevBusStateSet(WINC_DEVICE_HANDLE devHandle, WINC_DEV_BUS_STATE_TYPE busState);

#endif /* WINC_DEV_H */
