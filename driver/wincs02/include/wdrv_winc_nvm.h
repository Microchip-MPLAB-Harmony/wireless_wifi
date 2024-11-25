/*******************************************************************************
  WINC Wireless Driver NVM Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_nvm.h

  Summary:
    WINC wireless driver NVM header file.

  Description:
    This interface provides functionality required for the NVM service.
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

#ifndef WDRV_WINC_NVM_H
#define WDRV_WINC_NVM_H

#ifndef WDRV_WINC_MOD_DISABLE_NVM

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver NVM Defines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver NVM Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  NVM Operation

  Summary:
    NVM operation types.

  Description:
    List of possible NVM operation types.

  Remarks:
    None.

*/

typedef enum
{
    /* No operation. */
    WDRV_WINC_NVM_OPERATION_NONE,

    /* Erase sector operation. */
    WDRV_WINC_NVM_OPERATION_ERASE,

    /* Write data operation. */
    WDRV_WINC_NVM_OPERATION_WRITE,

    /* Read data operation. */
    WDRV_WINC_NVM_OPERATION_READ,

    /* Check data validity operation. */
    WDRV_WINC_NVM_OPERATION_CHECK
} WDRV_WINC_NVM_OPERATION_TYPE;

// *****************************************************************************
/* NVM Status Type

  Summary:
    NVM status codes.

  Description:
    Possible NVM status codes.

  Remarks:
    None.

*/

typedef enum
{
    /* Operation was a success. */
    WDRV_WINC_NVM_STATUS_SUCCESS,

    /* Operation encountered an error. */
    WDRV_WINC_NVM_STATUS_ERROR,
} WDRV_WINC_NVM_STATUS_TYPE;

// *****************************************************************************
/* NVM Check Type

  Summary:
    NVM check algorithm types.

  Description:
    Possible hashing and CRC algorithms to use for checking NVM integrity.

  Remarks:
    None.

*/

typedef enum
{
    /* CRC16. */
    WDRV_WINC_NVM_CHECK_MODE_CRC16 = WINC_CFG_PARAM_OPT_NVM_CHECK_MODE_CRC16,

    /* SHA1. */
    WDRV_WINC_NVM_CHECK_MODE_SHA1 = WINC_CFG_PARAM_OPT_NVM_CHECK_MODE_SHA,

    /* SHA-224. */
    WDRV_WINC_NVM_CHECK_MODE_SHA224 = WINC_CFG_PARAM_OPT_NVM_CHECK_MODE_SHA224,

    /* SHA-256. */
    WDRV_WINC_NVM_CHECK_MODE_SHA256 = WINC_CFG_PARAM_OPT_NVM_CHECK_MODE_SHA256,

    /* SHA=384. */
    WDRV_WINC_NVM_CHECK_MODE_SHA384 = WINC_CFG_PARAM_OPT_NVM_CHECK_MODE_SHA384,

    /* SHA-512. */
    WDRV_WINC_NVM_CHECK_MODE_SHA512 = WINC_CFG_PARAM_OPT_NVM_CHECK_MODE_SHA512
} WDRV_WINC_NVM_CHECK_MODE_TYPE;

// *****************************************************************************
/* NVM Geometry

  Summary:
    Structure containing NVM geometry.

  Description:
    Geometry information about NVM geometry like sector size and number.

  Remarks:
    None.

*/

typedef struct
{
    struct
    {
        /* Number of NVM sectors. */
        uint16_t number;

        /* Size of NVM sectors in bytes. */
        uint16_t size;
    } sector;

    struct
    {
        /* Start address of NVM partition. */
        uint32_t start;
    } address;
} WDRV_WINC_NVM_GEOM_INFO;

// *****************************************************************************
/* NVM Check Status Information

  Summary:
    Information pertaining to NVM check operations.

  Description:
    Status information provided upon successful completion of NVM check operation.

  Remarks:
    None.

*/

typedef struct
{
    /* Offset check began at. */
    uint32_t offset;

    /* Number of bytes checked. */
    uint32_t length;

    /* Check algorithm/method used. */
    WDRV_WINC_NVM_CHECK_MODE_TYPE mode;

    union
    {
        /* CRC16 result. */
        uint16_t crc16;

        /* Hash operation result. */
        struct
        {
            /* Length of hash value. */
            uint8_t l;

            /* Pointer to hash value. */
            const uint8_t *p;
        } hash;
    };
} WDRV_WINC_NVM_CHK_STATUS_INFO;

// *****************************************************************************
/* NVM Read Status Information

  Summary:
    Information pertaining to NVM read operations.

  Description:
    Status information provided upon successful completion of NVM read operation.

  Remarks:
    None.

*/

typedef struct
{
    /* Offset of read block within NVM partition. */
    uint32_t offset;

    /* Length of data read. */
    uint32_t length;

    /* Pointer to read data. */
    const uint8_t *pData;
} WDRV_WINC_NVM_READ_STATUS_INFO;

// *****************************************************************************
/* NVM Status Callback Function Pointer

  Function:
    void (*WDRV_WINC_NVM_STATUS_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_NVM_OPERATION_TYPE operation,
        WDRV_WINC_NVM_STATUS_TYPE status,
        uintptr_t opStatusInfo
    )

  Summary:
    Pointer to an NVM status callback function.

  Description:
    This defines an NVM status function callback type which can be passed
    into certain NVM functions to receive feedback.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    operation    - NVM operation.
    status       - Status of NVM operation.
    opStatusInfo - Operation status information.

  Returns:
    None.

  Remarks:
    opStatusInfo is:
        WDRV_WINC_NVM_OPERATION_ERASE:
            Not used.

        WDRV_WINC_NVM_OPERATION_WRITE:
            Not used.

        WDRV_WINC_NVM_OPERATION_READ:
            WDRV_WINC_READ_STATUS_INFO

        WDRV_WINC_NVM_OPERATION_CHECK:
            WDRV_WINC_CHK_STATUS_INFO

*/

typedef void (*WDRV_WINC_NVM_STATUS_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_NVM_OPERATION_TYPE operation,
    WDRV_WINC_NVM_STATUS_TYPE status,
    uintptr_t opStatusInfo
);

// *****************************************************************************
/*  NVM Operation State

  Summary:
    NVM operation state structure.

  Description:
    Structure containing details relating to an NVM operation.

  Remarks:
    None.

*/

typedef struct
{
    /* Indicate what NVM operation is in progress. */
    WDRV_WINC_NVM_OPERATION_TYPE operation;

    /* Callback to use for events relating to NVM access. */
    WDRV_WINC_NVM_STATUS_CALLBACK pfOperationStatusCB;

    /* Pointer to application buffer for read operation. */
    uint8_t *pBuffer;

    /* Offset within NVM partition. */
    uint32_t offset;

    /* Length of data within NVM operation. */
    uint32_t length;

    /* NVM geometry information. */
    WDRV_WINC_NVM_GEOM_INFO geom;
} WDRV_WINC_NVM_OPERATION_STATE;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver NVM Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_NVMProcessAEC
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

void WDRV_WINC_NVMProcessAEC
(
    uintptr_t context,
    WINC_DEVICE_HANDLE devHandle,
    const WINC_DEV_EVENT_RSP_ELEMS *const pElems
);

//*******************************************************************************
/*
  Function:
    const WDRV_WINC_NVM_GEOM_INFO* const WDRV_WINC_NVMGeometryGet(DRV_HANDLE handle)

  Summary:
    Returns the NVM geometry.

  Description:
    Returns information on the geometry (sector size, number of sectors, etc)
      of the NVM region.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    Pointer to geometry structure or NULL for error.

  Remarks:
    None.

*/

const WDRV_WINC_NVM_GEOM_INFO* const WDRV_WINC_NVMGeometryGet(DRV_HANDLE handle);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMEraseSector
    (
        DRV_HANDLE handle,
        uint8_t startSector,
        uint8_t numSectors,
        WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Erases sectors.

  Description:
    Erases a number of sectors.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    startSector      - Sector offset to erase.
    numSectors       - Number of sectors to erase.
    pfUpdateStatusCB - Callback to indicate update status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_NVMEraseSector
(
    DRV_HANDLE handle,
    uint8_t startSector,
    uint8_t numSectors,
    WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMWrite
    (
        DRV_HANDLE handle,
        void *pBuffer,
        uint32_t offset,
        uint32_t size,
        WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Writes data to the NVM.

  Description:
    Writes a number of bytes to the NVM.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    pBuffer          - Pointer to buffer containing the data to write.
    offset           - Offset within the NVM to write the data to.
    length           - Number of bytes to be written.
    pfUpdateStatusCB - Callback to indicate update status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_NVMWrite
(
    DRV_HANDLE handle,
    void *pBuffer,
    uint32_t offset,
    uint32_t length,
    WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMRead
    (
        DRV_HANDLE handle,
        void *pBuffer,
        uint32_t offset,
        uint32_t length,
        WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Reads data from the NVM.

  Description:
    Reads a number of bytes from the NVM.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    pBuffer          - Pointer to buffer to write the data read into.
    offset           - Offset within the NVM to read the data from.
    length           - Number of bytes to be read.
    pfUpdateStatusCB - Callback to indicate update status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    When a read operation completes the callback is called with opStatusInfo
      being the structure WDRV_WINC_READ_STATUS_INFO.

    If pBuffer is NULL the callback will be called for each data block read
      from the NVM, this may be smaller than the requested length, but the
      callback will be called multiple times with the offset and length
      reflecting each block as it is received.

    If pBuffer is a user supplied buffer the NVM read will accumulate blocks
      into the buffer and call the callback once it is complete.

*/

WDRV_WINC_STATUS WDRV_WINC_NVMRead
(
    DRV_HANDLE handle,
    void *pBuffer,
    uint32_t offset,
    uint32_t length,
    WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_NVMCheck
    (
        DRV_HANDLE handle,
        uint32_t offset,
        uint32_t size,
        WDRV_WINC_NVM_CHECK_MODE_TYPE mode,
        WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
    )

  Summary:
    Checks data from the NVM.

  Description:
    Performs a CRC/hash check of the number of bytes from the NVM.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    offset           - Offset within the NVM to read the data from.
    size             - Number of bytes to be checked.
    mode             - Algorithm to be used to check memory.
    pfUpdateStatusCB - Callback to indicate update status.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    When a check operation completes the callback is called with opStatusInfo
      being the structure WDRV_WINC_CHK_STATUS_INFO.

*/

WDRV_WINC_STATUS WDRV_WINC_NVMCheck
(
    DRV_HANDLE handle,
    uint32_t offset,
    uint32_t size,
    WDRV_WINC_NVM_CHECK_MODE_TYPE mode,
    WDRV_WINC_NVM_STATUS_CALLBACK pfUpdateStatusCB
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_MOD_DISABLE_NVM */
#endif /* WDRV_WINC_NVM_H */
