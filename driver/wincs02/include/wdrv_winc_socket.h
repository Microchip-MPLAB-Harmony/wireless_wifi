/*******************************************************************************
  WINC Wireless Driver Socket Mode Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_socket.h

  Summary:
    WINC wireless driver socket mode interface.

  Description:
    This interface provides extra functionality required for socket mode
    operation.
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

#ifndef WDRV_WINC_SOCKET_H
#define WDRV_WINC_SOCKET_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "wdrv_winc.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Socket Mode Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER)
    (
        DRV_HANDLE handle,
        const WDRV_WINC_IP_MULTI_ADDRESS *const pIPAddr,
        WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
        uint16_t rtt
    )

  Summary:
    Pointer to an ICMP echo response event handler function.

  Description:
    This data type defines a function event handler which is
    called in response to an ICMP echo request response event.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    pIPAddr    - Pointer to IP address responding.
    ipAddrType - Type of IP address.
    rtt        - Round trip time.

  Returns:
    None.

  Remarks:

*/

typedef void (*WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER)
(
    DRV_HANDLE handle,
    const WDRV_WINC_IP_MULTI_ADDRESS *const pIPAddr,
    WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
    uint16_t rtt
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Socket Mode Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_ICMPProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        const WINC_DEV_EVENT_RSP_ELEMS *const pElems
    )

  Summary:
    AEC process callback.

  Description:
    Callback will be called to process any AEC messages received.

  Precondition:
    WINC_DevAECCallbackRegister must be called to register the callback.

  Parameters:
    context   - Pointer to user context supplied when callback was registered.
    devHandle - WINC device handle.
    pElems    - Pointer to element structure.

  Returns:
    None.

  Remarks:
    Callback should call WINC_CmdReadParamElem to extract elements.

*/

void WDRV_WINC_ICMPProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

#ifdef WINC_CONF_ENABLE_NC_BERKELEY_SOCKETS
//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SocketRegisterEventCallback
    (
        DRV_HANDLE handle,
        WINC_SOCKET_EVENT_CALLBACK pfSocketEventCb
    )

  Summary:
    Register an event callback for socket events.

  Description:
    Socket events are dispatched to the application via this callback.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle          - Client handle obtained by a call to WDRV_WINC_Open.
    pfSocketEventCb - Function pointer to event callback handler.

  Returns:
    WDRV_WINC_STATUS_OK            - Callback registered.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request failed.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SocketRegisterEventCallback
(
    DRV_HANDLE handle,
    const WINC_SOCKET_EVENT_CALLBACK pfSocketEventCb
);
#endif

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_ICMPEchoRequestAddr
    (
        DRV_HANDLE handle,
        const WDRV_WINC_IP_MULTI_ADDRESS *const pIPAddr,
        WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
        const WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB
    )

  Summary:
    Send an ICMP echo request.

  Description:
    Sends an ICMP echo request to the address specified.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle               - Client handle obtained by a call to WDRV_WINC_Open.
    pIPAddr              - Pointer to IP address to send the request to.
    ipAddrType           - Address type.
    pfICMPEchoResponseCB - Pointer to callback function.

  Returns:
    WDRV_WINC_STATUS_OK             - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_ICMPEchoRequestAddr
(
    DRV_HANDLE handle,
    const WDRV_WINC_IP_MULTI_ADDRESS *const pIPAddr,
    WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
    const WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_SOCKET_H */
