/*******************************************************************************
  WINC Driver Socket Mode Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_socket.h

  Summary:
    WINC wireless driver (Socket mode) header file.

  Description:
    This interface provides extra functionality required for socket mode operation.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef WDRV_WINC_SOCKET_H
#define WDRV_WINC_SOCKET_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "wdrv_winc.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

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
        WDRV_WINC_IP_MULTI_ADDRESS *pIPAddr,
        WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
        uint16_t rtt
    )

  Summary:
    Pointer to an ICMP echo response event handler function.

  Description:
    This data type defines a function event handler which is
    called in response to an ICMP echo request response event.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

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
    WDRV_WINC_IP_MULTI_ADDRESS *pIPAddr,
    WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
    uint16_t rtt
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Socket Mode Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_ICMPProcessAEC
    (
        uintptr_t context,
        WINC_DEVICE_HANDLE devHandle,
        WINC_DEV_EVENT_RSP_ELEMS *pElems
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
    WINC_DEV_EVENT_RSP_ELEMS *pElems
);

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
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle          - Client handle obtained by a call to WDRV_WINC_Open.
    pfSocketEventCb - Function pointer to event callback handler.

  Returns:
    WDRV_WINC_STATUS_OK          - Callback registered.
    WDRV_WINC_STATUS_INVALID_ARG - The parameters were incorrect.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SocketRegisterEventCallback
(
    DRV_HANDLE handle,
    const WINC_SOCKET_EVENT_CALLBACK pfSocketEventCb
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_ICMPEchoRequestAddr
    (
        DRV_HANDLE handle,
        WDRV_WINC_IP_MULTI_ADDRESS *pIPAddr,
        WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
        const WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB
    )

  Summary:
    Send an ICMP echo request.

  Description:
    Sends an ICMP echo request to the address specified.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle               - Client handle obtained by a call to WDRV_WINC_Open.
    pIPAddr              - Pointer to IP address to send the request to.
    ipAddrType           - Address type.
    pfICMPEchoResponseCB - Pointer to callback function.

  Returns:
    WDRV_WINC_STATUS_OK             - Callback registered.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_ICMPEchoRequestAddr
(
    DRV_HANDLE handle,
    WDRV_WINC_IP_MULTI_ADDRESS *pIPAddr,
    WDRV_WINC_IP_ADDRESS_TYPE ipAddrType,
    const WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_SOCKET_H */
