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
#define WINC_FLAG_STATUS_RCVD       0x20

/* Flag indicating this forms the first element of a burst. */
#define WINC_FLAG_FIRST_IN_BURST    0x40

/* Flag indicating this forms the last element of a burst. */
#define WINC_FLAG_LAST_IN_BURST     0x80

/*****************************************************************************
                     WINC Command Request Module API
 *****************************************************************************/

WINC_CMD_REQ_HANDLE WINC_CmdReqInit(uint8_t* pBuffer, size_t lenBuffer, int numCommands, WINC_DEV_CMD_RSP_CB pfCmdRspCallback, uintptr_t cmdRspCallbackCtx);

#endif /* WINC_CMD_REQ_H */
