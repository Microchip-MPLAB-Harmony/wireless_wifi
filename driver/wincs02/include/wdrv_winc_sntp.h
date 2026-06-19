/*******************************************************************************
  WINC Wireless Driver SNTP Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_sntp.h

  Summary:
    WINC wireless driver SNTP interface.

  Description:
    Provides an interface to configure the Network Time (NTP) client on the
    WINC.
 *******************************************************************************/

/*
Copyright (C) 2024-26 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#ifndef WDRV_WINC_SNTP_H
#define WDRV_WINC_SNTP_H

#ifndef WDRV_WINC_MOD_DISABLE_SNTP

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver SNTP Defines
// *****************************************************************************
// *****************************************************************************

/* Map original SNTP enable function to updated version. */
#define WDRV_WINC_SNTPEnableSet(HANDLE, ENABLED)        WDRV_WINC_SNTPEnableSetCallback(HANDLE, ENABLED, NULL)

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver SNTP Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  SNTP Status

  Summary:
    SNTP statuses.

  Description:
    List of possible SNTP statuses.

  Remarks:
    None.

*/

typedef enum
{
    /* SNTP status is OK, no error. */
    WDRV_WINC_SNTP_STATUS_OK,

    /* General SNTP error. */
    WDRV_WINC_SNTP_STATUS_ERROR,

    /* The SNTP server request has timed out. */
    WDRV_WINC_SNTP_STATUS_SERVER_TIMEOUT,
} WDRV_WINC_SNTP_STATUS;

// *****************************************************************************
/* SNTP Status Callback Function Pointer

  Function:
    void (*WDRV_WINC_SNTP_STATUS_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_SNTP_STATUS status,
        void *pStatusInfo
    )

  Summary:
    Pointer to an SNTP status callback function.

  Description:
    This defines an SNTP status function callback type which can be passed
    into certain SNTP functions to receive feedback.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    status      - Status of SNTP operation.
    pStatusInfo - Pointer to optional status information.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_SNTP_STATUS_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_SNTP_STATUS status,
    void *pStatusInfo
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver SNTP Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_SNTPProcessAEC
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

void WDRV_WINC_SNTPProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPEnableSetCallback
    (
        DRV_HANDLE handle,
        bool enabled,
        WDRV_WINC_SNTP_STATUS_CALLBACK pfSNTPStatusCB
    )

  Summary:
    Set the enable state of the Network Time (NTP) client function.

  Description:
    Enables or disables the Network Time (NTP) client.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    enabled        - Flag indicating if the NTP client should be enabled or disabled.
    pfSNTPStatusCB - Callback to indicate status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPEnableSetCallback
(
    DRV_HANDLE handle,
    bool enabled,
    WDRV_WINC_SNTP_STATUS_CALLBACK pfSNTPStatusCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPStaticSet
    (
        DRV_HANDLE handle,
        bool enabled
    )

  Summary:
    Set the enable state of the NTP configuration mode.

  Description:
    Configures the NTP configuration mode to either DHCP or Static.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    enabled - Flag indicating if the NTP configuration mode should be DHCP or Static.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPStaticSet
(
    DRV_HANDLE handle,
    bool enabled
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPServerAddressSet
    (
        DRV_HANDLE handle,
        const char *pAddr
    )

  Summary:
    Set the address/name of NTP server.

  Description:
    Configures the address/name to be used as the NTP server.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    pAddr  - Pointer to address/name.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPServerAddressSet
(
    DRV_HANDLE handle,
    const char *pAddr
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_MOD_DISABLE_SNTP */
#endif /* WDRV_WINC_SNTP_H */
