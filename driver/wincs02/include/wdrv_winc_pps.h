/*******************************************************************************
  WINC Wireless Driver PPS Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_pps.h

  Summary:
    WINC wireless driver PPS interface.

  Description:
    This interface provides functionality required for the PPS service.
 *******************************************************************************/

/*
Copyright (C) 2025-26 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#ifndef WDRV_WINC_PPS_H
#define WDRV_WINC_PPS_H

#ifndef WDRV_WINC_MOD_DISABLE_PPS

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver PPS Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  PPS Update Status

  Summary:
    PPS update statuses.

  Description:
    List of possible PPS update statuses.

  Remarks:
    None.

*/

typedef enum
{
    /* PPS is disabled. */
    WDRV_WINC_PPS_DISABLED,

    /* PPS is enabled. */
    WDRV_WINC_PPS_ENABLED,

    /* XDS is enabled. */
    WDRV_WINC_XDS_ENABLED
} WDRV_WINC_PPS_STATE;

// *****************************************************************************
/*  PPS XDS Status

  Summary:
    PPS XDS statuses.

  Description:
    List of possible PPS XDS statuses.

  Remarks:
    None.

*/

typedef enum
{
    /* XDS is idle. */
    WDRV_WINC_XDS_IDLE,

    /* XDS sleep has been requested. */
    WDRV_WINC_XDS_SLEEP_REQ,

    /* XDS is in sleep mode. */
    WDRV_WINC_XDS_SLEEP,

    /* XDS wake has been requested. */
    WDRV_WINC_XDS_WAKE_REQ
} WDRV_WINC_XDS_STATE;


// *****************************************************************************
/*  PPS Events

  Summary:
    PPS event types.

  Description:
    List of possible PPS events.

  Remarks:
    None.

*/

typedef enum
{
    /* PPS timeout event. */
    WDRV_WINC_PPS_TIMEOUT = WINC_STATUS_PPS_TIMEOUT,

    /* PPS pause duration has expired. */
    WDRV_WINC_PPS_PAUSE_EXPIRED = WINC_STATUS_PPS_PAUSE_EXPIRED,

    /* PPS error occurred. */
    WDRV_WINC_PPS_ERROR
} WDRV_WINC_PPS_EVENT;

// *****************************************************************************
/*  PPS Options

  Summary:
    Structure containing PPS options.

  Description:
    Contains PPS options.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if the secondary oscillator is enabled. */
    uint32_t secOscEnabled;

    /* Auto sleep timeout value in milliseconds. */
    uint32_t autoSleepTimeout;

    /* PPS duration value in milliseconds. */
    uint32_t duration;
} WDRV_WINC_PPS_OPTIONS;

// *****************************************************************************
/*

  Function:
    typedef void (*WDRV_WINC_PPS_EVENT_HANDLER)
    (
        DRV_HANDLE handle,
        WDRV_WINC_PPS_EVENT event
    )

  Summary:
    PPS event callback.

  Description:
    Callback to be used to convey PPS events.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    event  - Event type.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_PPS_EVENT_HANDLER)
(
    DRV_HANDLE             handle,
    WDRV_WINC_PPS_EVENT    event
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver PPS Routines
// *****************************************************************************
// *****************************************************************************

#ifdef __cplusplus // Provide C++ Compatibility
extern "C"
{
#endif

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_PPSProcessAEC
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

void WDRV_WINC_PPSProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_PPSOptionsSet
    (
        DRV_HANDLE handle,
        const WDRV_WINC_PPS_OPTIONS *const pPPSOptions
    )

  Summary:
    Sets the PPS configuration options.

  Description:
    Configures PPS configuration options.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    pPPSOptions - Pointer to new PPS options to configure.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_PPSOptionsSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_PPS_OPTIONS *const pPPSOptions
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_PPSEnableSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_POWERSAVE_MODE mode,
        bool enabled,
        WDRV_WINC_PPS_EVENT_HANDLER pfPPSEventCB
    )

  Summary:
    Set the enable state of PPS.

  Description:
    Enables or disables PPS.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    mode         - The PPS mode to engage either PPS or XDS.
    enabled      - Flag to enable.
    pfPPSEventCB - Pointer to PPS event callback.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_PPSEnableSet
(
    DRV_HANDLE handle,
    WDRV_WINC_POWERSAVE_MODE mode,
    bool enabled,
    const WDRV_WINC_PPS_EVENT_HANDLER pfPPSEventCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_PPSPause
    (
        DRV_HANDLE handle,
        uint16_t pauseDuration
    )

  Summary:
    Pause PPS mode temporarily.

  Description:
    Pause PPS mode temporarily.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.
    WDRV_WINC_PPSEnableSet has been called to enable PPS mode, event callback
    should be set here if required.

  Parameters:
    handle                 - Client handle obtained by a call to WDRV_WINC_Open.
    pauseDuration          - Duration of the pause in seconds.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_PPSPause
(
    DRV_HANDLE handle,
    uint16_t pauseDuration
);

#ifdef __cplusplus
}
#endif
#endif /* WDRV_WINC_MOD_DISABLE_PPS */
#endif /* WDRV_WINC_PPS_H */
