/*******************************************************************************
  WINC Wireless Driver System Log Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_syslog.h

  Summary:
    WINC wireless driver system log interface.

  Description:
    Provides an interface to configure the system log on the WINC.
 *******************************************************************************/

/*
Copyright (C) 2025 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#ifndef WDRV_WINC_SYSLOG_H
#define WDRV_WINC_SYSLOG_H

#ifndef WDRV_WINC_MOD_DISABLE_SYSLOG

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver System Log Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  SYSLOG Event

  Summary:
    SYSLOG event severity levels.

  Description:
    List of possible SYSLOG event severity levels.

  Remarks:
    None.

*/

typedef enum
{
    WDRV_WINC_SYSLOG_LOG_EMERGENCY = WINC_CONST_SYSLOG_EVENT_SEVERITY_EMERGENCY,
    WDRV_WINC_SYSLOG_LOG_ALERT     = WINC_CONST_SYSLOG_EVENT_SEVERITY_ALERT,
    WDRV_WINC_SYSLOG_LOG_CRITICAL  = WINC_CONST_SYSLOG_EVENT_SEVERITY_CRITICAL,
    WDRV_WINC_SYSLOG_LOG_ERROR     = WINC_CONST_SYSLOG_EVENT_SEVERITY_ERROR,
    WDRV_WINC_SYSLOG_LOG_WARNING   = WINC_CONST_SYSLOG_EVENT_SEVERITY_WARNING,
    WDRV_WINC_SYSLOG_LOG_NOTICE    = WINC_CONST_SYSLOG_EVENT_SEVERITY_NOTICE,
    WDRV_WINC_SYSLOG_LOG_INFO      = WINC_CONST_SYSLOG_EVENT_SEVERITY_INFO,
    WDRV_WINC_SYSLOG_LOG_DEBUG     = WINC_CONST_SYSLOG_EVENT_SEVERITY_DEBUG
} WDRV_WINC_SYSLOG_SEVERITY;

// *****************************************************************************
/*  Syslog Callback.

  Function:
    typedef void (*WDRV_WINC_SYSLOG_CALLBACK)
    (
        DRV_HANDLE handle,
        uint32_t timestamp,
        uint8_t source,
        WDRV_WINC_SYSLOG_SEVERITY severity,
        const char* pLogMessage
    )

  Summary:
    A callback to provide SYSLOG events.

  Description:
    This callback provides details of the SYSLOG event.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    timestamp   - Timestamp when event occurred.
    source      - Source of the event.
    severity    - Severity of the event.
    pLogMessage - Pointer to log message string.

  Returns:
    None.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_SYSLOG_CALLBACK)
(
    DRV_HANDLE handle,
    uint32_t timestamp,
    uint8_t source,
    WDRV_WINC_SYSLOG_SEVERITY severity,
    const char* pLogMessage
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver System Log Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_SYSLOGProcessAEC
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

void WDRV_WINC_SYSLOGProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SystemLogEnableSet
    (
        DRV_HANDLE handle,
        const WDRV_WINC_SYSLOG_CALLBACK pfSyslogEventCB
    )

  Summary:
      Sets the enable state of the system log on the WINC. The system log will
      be enabled if a function pointer is provided

  Description:
      Enables of disables the system log.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    pfSyslogEventCB - Function pointer to callback for the log.


  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SystemLogEnableSet
(   DRV_HANDLE handle,
    const WDRV_WINC_SYSLOG_CALLBACK pfSyslogEventCB
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_MOD_DISABLE_SYSLOG */
#endif /* WDRV_WINC_SYSLOG_H */
