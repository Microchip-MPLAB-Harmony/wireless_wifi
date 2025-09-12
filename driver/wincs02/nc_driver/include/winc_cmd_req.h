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

#ifndef WINC_CMD_REQ_H
#define WINC_CMD_REQ_H

#include "winc_cmds.h"

/*****************************************************************************

 +------------------+------------------+------------------+------------------+
 +                            Next Command Handle                            +
 +------------------+------------------+------------------+------------------+
 +                         Callback Function Pointer                         +
 +------------------+------------------+------------------+------------------+
 +                         Callback Function Context                         +
 +------------------+------------------+------------------+------------------+
 + Num of Commands  + Max Num of Cmds  +  Num of Errors   +                  +
 +------------------+------------------+------------------+------------------+
 +                              Working Pointer                              +
 +------------------+------------------+------------------+------------------+
 +                Size                 +                Space                +
 +------------------+------------------+------------------+------------------+
 +                           First Header Element                            +
 +------------------+------------------+------------------+------------------+
 +                     Current (Working) Header Element                      +
 +------------------+------------------+------------------+------------------+
 +                  Current Command Request Working Pointer                  +
 +------------------+------------------+------------------+------------------+
 + Command 1                         Size                                    +
 +                                    or                                     +
 +           Response Status           +                                     +
 +------------------+------------------+------------------+------------------+
 + Command 2                         Size                                    +
 +                                    or                                     +
 +           Response Status           +                                     +
 +------------------+------------------+------------------+------------------+
  :                                                                         :
 + Command X                         Size                                    +
 +                                    or                                     +
 +           Response Status           +                                     +
 +------------------+------------------+------------------+------------------+
 +                             Header Element 1                              +
 +                        Pointer to Command Fragment 1                      +
 +------------------+------------------+------------------+------------------+
 *               Length                +     Flags        +                  +
 +------------------+------------------+------------------+------------------+
 +                             Header Element 2                              +
 +                        Pointer to Command Fragment 2                      +
 +------------------+------------------+------------------+------------------+
 *               Length                +     Flags        +                  +
 +------------------+------------------+------------------+------------------+
  :                                                                         :
 +------------------+------------------+------------------+------------------+
 +                             Header Element Y                              +
 +                        Pointer to Command Fragment Y                      +
 +------------------+------------------+------------------+------------------+
 *               Length                +     Flags        +                  +
 +------------------+------------------+------------------+------------------+
  :                                                                         :
 +------------------+------------------+------------------+------------------+
 +                             Command Fragment 1                            +
 +------------------+------------------+------------------+------------------+
 +                             Command Fragment 2                            +
 +------------------+------------------+------------------+------------------+
  :                                                                         :
 +------------------+------------------+------------------+------------------+
 +                             Command Fragment Y                            +
 +------------------+------------------+------------------+------------------+

 *****************************************************************************/

/*****************************************************************************
  Description:
    Command request header element structure.

  Remarks:
    Used to define a fragment of a command request message.

 *****************************************************************************/

typedef struct
{
    uint8_t                 *pPtr;
    uint16_t                length;
    uint8_t                 flags;
    uint8_t                 reserved;
} WINC_SEND_REQ_HDR_ELEM;

/*****************************************************************************
  Description:
    Command request state.

  Remarks:
    Encapsulates all state for a command request.

 *****************************************************************************/

typedef struct
{
    WINC_CMD_REQ_HANDLE     nextCmdReq;
    WINC_DEV_CMD_RSP_CB     pfCmdRspCallback;
    uintptr_t               cmdRspCallbackCtx;
    uint8_t                 numCmds;
    uint8_t                 maxNumCmds;
    uint8_t                 numErrors;
    uint8_t                 reserved[1];

    uint8_t                 *pPtr;

    uint16_t                size;
    uint16_t                space;

    WINC_SEND_REQ_HDR_ELEM  *pFirstHdrElem;
    WINC_SEND_REQ_HDR_ELEM  *pCurHdrElem;

    WINC_COMMAND_REQUEST    *pCmdReq;

    union
    {
        uint32_t            size;

        struct
        {
            uint16_t        status;
            uint16_t        reserved;
        } rsp;
    } cmds[];
} WINC_SEND_REQ_STATE;

/* Flag indicating the command request status has been received. */
#define WINC_FLAG_STATUS_RCVD       0x20U

/* Flag indicating this forms the first element of a burst. */
#define WINC_FLAG_FIRST_IN_BURST    0x40U

/* Flag indicating this forms the last element of a burst. */
#define WINC_FLAG_LAST_IN_BURST     0x80U

/*****************************************************************************
                     WINC Command Request Module API
 *****************************************************************************/

WINC_CMD_REQ_HANDLE WINC_CmdReqInit(uint8_t* pBuffer, size_t lenBuffer, int numCommands, WINC_DEV_CMD_RSP_CB pfCmdRspCallback, uintptr_t cmdRspCallbackCtx);
bool WINC_CmdReqDiscard(WINC_DEVICE_HANDLE devHandle, WINC_CMD_REQ_HANDLE cmdReqHandle);

#endif /* WINC_CMD_REQ_H */
