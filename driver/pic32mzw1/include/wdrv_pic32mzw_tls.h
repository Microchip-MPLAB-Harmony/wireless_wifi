/*
Copyright (C) 2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef _WDRV_PIC32MZW_TLS_H
#define _WDRV_PIC32MZW_TLS_H

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  The maximum number of sessions

  Summary:
    The maximum number of sessions

  Description:
    None

  Remarks:
    None.
*/

#define WDRV_PIC32MZW_TLS_MAX_NUM_SESSIONS 2

// *****************************************************************************
/*  Session handle

  Summary:
    A handle representing a TLS session

  Description:
    None

  Remarks:
    None.
*/

typedef void* WDRV_PIC32MZW_TLS_SESSION_HANDLE;

// *****************************************************************************
/*  Invaid session handle

  Summary:
    Invalid session handle

  Description:
    Defines a value for a session handle which isn't valid.

  Remarks:
    None.
*/

#define WDRV_PIC32MZW_TLS_INVALID_SESSION_HANDLE  NULL

// *****************************************************************************
/*  State of a TLS session

  Summary:
    Defines the states of a TLS session.

  Description:
    This enumeration defines the TLS session states.

  Remarks:
    None.
*/
typedef enum
{
    /* Invalid */
    WDRV_PIC32MZW_TLS_SESSION_STATE_INVALID,
    /* Initialized */
    WDRV_PIC32MZW_TLS_SESSION_STATE_INITIALIZED,
    /* Negotiating */
    WDRV_PIC32MZW_TLS_SESSION_STATE_NEGOTIATING,
    /* Connected */
    WDRV_PIC32MZW_TLS_SESSION_STATE_CONNECTED,    

} WDRV_PIC32MZW_TLS_SESSION_STATE;

// *****************************************************************************
/*  User callback event type

  Summary:
    Defines the user callback events.

  Description:
    This enumeration defines the user callback events.

  Remarks:
    None.
*/
typedef enum
{
    /* Create a TLS session */
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_CREATE_SESSION,
    /* Start a TLS session */
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_START_SESSION,
    /* Update a TLS session */
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_UPDATE_SESSION,
    /* Terminate a TLS session */
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_TERMINATE_SESSION,
    /* Write data received from lower layer to the TLS stack */
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_RECV_BUFF,
    /* Write data received from upper layer to the TLS stack */
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_WRITE_BUFF,
    /* Generate key material */
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_GENERATE_KEY,

} WDRV_PIC32MZW_TLS_USER_CB_EVENT;

// *****************************************************************************
/*  User TLS callback parameters for create session event

  Summary:
    Structure that defines the parameters for the user TLS callback
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_CREATE_SESSION event.

  Description:
    None.

  Remarks:
    None.

*/
typedef struct
{
    /* User context handle (IN) */
    uintptr_t userContextHandle;

    /* User session handle (OUT) */
    uintptr_t userSessionHandle;

    /* Allow session caching (IN) */
    bool allowSessionCaching;

    /* Server domain required (IN) */
    bool srvDomainRequired;

} WDRV_PIC32MZW_TLS_USER_CB_PARAM_CREATE_SESSION;

// *****************************************************************************
/*  User TLS callback parameters for start session event

  Summary:
    Structure that defines the parameters for the user TLS callback
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_START_SESSION event.

  Description:
    None.

  Remarks:
    None.

*/
typedef struct
{
    /* User session handle (IN) */
    uintptr_t userSessionHandle;

    /* Driver session handle (IN) */
    WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle;

} WDRV_PIC32MZW_TLS_USER_CB_PARAM_START_SESSION;

// *****************************************************************************
/*  User TLS callback parameters for update session event

  Summary:
    Structure that defines the parameters for the user TLS callback
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_UPDATE_SESSION event.

  Description:
    None.

  Remarks:
    None.

*/
typedef struct
{
    /* User session handle (IN) */
    uintptr_t userSessionHandle;

    /* Driver session state (IN) */
    WDRV_PIC32MZW_TLS_SESSION_STATE drvSessionState;

    /* Driver TLS session handle (IN) */
    WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle;

} WDRV_PIC32MZW_TLS_USER_CB_PARAM_UPDATE_SESSION;

// *****************************************************************************
/*  User TLS callback parameters for terminate session event

  Summary:
    Structure that defines the parameters for the user TLS callback
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_TERMINATE_SESSION event.

  Description:
    None.

  Remarks:
    None.

*/
typedef struct
{
    /* User session handle (IN) */
    uintptr_t userSessionHandle;

    /* Delete session entry from cache (IN) */
    bool delCachedSession;

} WDRV_PIC32MZW_TLS_USER_CB_PARAM_TERMINATE_SESSION;

// *****************************************************************************
/*  User TLS callback parameters for generate key event

  Summary:
    Structure that defines the parameters for the user TLS callback
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_GENERATE_KEY event.

  Description:
    None.

  Remarks:
    None.

*/
typedef struct
{
    /* User session handle (IN) */
    uintptr_t userSessionHandle;

    /* Pointer to Master Session Key (IN) */
    uint8_t *pMsk;

    /* Key length (IN) */
    uint16_t lenKey;

    /* Label (IN) */
    const char *pLabel;

} WDRV_PIC32MZW_TLS_USER_CB_PARAM_GENERATE_KEY;


// *****************************************************************************
/*  User TLS callback parameters for receive and write buffer events

  Summary:
    Structure that defines the parameters for the user TLS callback
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_RECV_BUFF and 
    WDRV_PIC32MZW_TLS_USER_CB_EVENT_WRITE_BUFF events.

  Description:
    None.

  Remarks:
    None.

*/
typedef struct
{
    /* User session handle (IN) */
    uintptr_t userSessionHandle;

    /* Pointer to data buffer (IN) */
    uint8_t *pBuffer;

    /* Data buffer length (IN) */
    uint16_t lenBuffer;

} WDRV_PIC32MZW_TLS_USER_CB_PARAM_BUFF;

// *****************************************************************************
/*  TLS User Callback

  Summary:
    A callback for TLS events which need to be processed by the application's TLS stack.

  Description:
    This callback provides details of TLS related events which need to be processed by the application's TLS stack.

  Parameters:
    event             - Event the user callback needs to process.
    pParam            - Pointer to parameters structure.

  Returns:
    True upon success, false otherwise.

  Remarks:
    None.

*/
typedef bool (*WDRV_PIC32MZW_TLS_USER_CB)
(
    WDRV_PIC32MZW_TLS_USER_CB_EVENT event, 
    void *pParam
);

// *****************************************************************************
/*  TLS buffer

  Summary:
    Structure that represents a buffer holding TLS data

  Description:
    None.

  Remarks:
    None.

*/

typedef struct
{
    /* Pointer to buffer holding the data */
    uint8_t *pData;

    /* Size of the data buffer*/
    uint16_t size;

} WDRV_PIC32MZW_TLS_BUFFER;

// *****************************************************************************
// *****************************************************************************
// Section: PIC32MZW Driver TLS Routines
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    bool WDRV_PIC32MZW_TLS_NotifyRxBufferReady
    (
        WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, 
        WDRV_PIC32MZW_TLS_BUFFER *pBuffer
    )

  Summary:
    User callback helper function to notify the driver that a buffer has been received.

  Description:
    Notifies the driver that a buffer has been received.

  Precondition:
    None.

  Parameters:
    drvSessionHandle - Driver session handle.
    pBuffer          - Pointer to buffer holding the data

  Returns:
    True upon success, false otherwise.

  Remarks:
    None.

*/

bool WDRV_PIC32MZW_TLS_NotifyRxBufferReady
(
    WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, 
    WDRV_PIC32MZW_TLS_BUFFER *pBuffer
);

//*******************************************************************************
/*
  Function:
    bool WDRV_PIC32MZW_TLS_NotifyTxPacketReady
    (
        WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, 
        WDRV_PIC32MZW_TLS_BUFFER *pBuffer
    )

  Summary:
    User callback helper function to notify the driver that a packet is ready for transmission.

  Description:
    Notifies the driver that a packet is ready for transmission.

  Precondition:
    None.

  Parameters:
    drvSessionHandle - Driver session handle.
    pBuffer          - Pointer to buffer holding the packet data

  Returns:
    True upon success, false otherwise.

  Remarks:
    None.

*/

bool WDRV_PIC32MZW_TLS_NotifyTxPacketReady
(
    WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, 
    WDRV_PIC32MZW_TLS_BUFFER *pBuffer
);

//*******************************************************************************
/*
  Function:
    bool WDRV_PIC32MZW_TLS_NotifyConnectionEstablished(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle)

  Summary:
    User callback helper function to notify the driver that a TLS connection has been established.

  Description:
    Notifies the driver that a TLS connection has been established.

  Precondition:
    None.

  Parameters:
    drvSessionHandle - Driver session handle.
    isResumed        - Flag indicating if the session was resumed.

  Returns:
    True upon success, false otherwise.

  Remarks:
    None.

*/

bool WDRV_PIC32MZW_TLS_NotifyConnectionEstablished(WDRV_PIC32MZW_TLS_SESSION_HANDLE drvSessionHandle, bool isResumed);

// *****************************************************************************
// *****************************************************************************
// Section: PIC32MZW Driver Internal Routines
// *****************************************************************************

bool DRV_PIC32MZW_TLS_Init
(
    WDRV_PIC32MZW_TLS_USER_CB userCallback, 
    uintptr_t userContextHandle[WDRV_PIC32MZW_TLS_MAX_NUM_SESSIONS], 
    uint8_t numSessions,
    bool checkSrvDomain
);

void DRV_PIC32MZW_TLS_DeInit(void);

void DRV_PIC32MZW_TLS_Update(void);

#endif /* _WDRV_PIC32MZW_TLS_H */
