/*******************************************************************************
  WINC Driver File Operation Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_file.h

  Summary:
    WINC wireless driver file operation header file.

  Description:
    This interface manages file operations.
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

#ifndef WDRV_WINC_FILE_H
#define WDRV_WINC_FILE_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver File Operation Defines
// *****************************************************************************
// *****************************************************************************

#define WDRV_WINC_FILE_INVALID_HANDLE                   0

#define WDRV_WINC_FILE_LOAD_BUF_SZ                      128

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver File Operation Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* File Transfer Handle.

  Summary:
    WINC file handle type definition.

  Description:
    WINC file handle type definition.

  Remarks:
    None.
*/

typedef uintptr_t WDRV_WINC_FILE_HANDLE;

// *****************************************************************************
/* File Types.

  Summary:
    Supported file types.

  Description:
    Enumeration of possible file types supported.

  Remarks:
    None.
*/

typedef enum
{
    /* Invalid file type. */
    WDRV_WINC_FILE_TYPE_INVALID = 0,

    /* Certificates. */
    WDRV_WINC_FILE_TYPE_CERTS   = WINC_CONST_FS_FILETYPE_CERT,

    /* Private keys. */
    WDRV_WINC_FILE_TYPE_PRIKEYS = WINC_CONST_FS_FILETYPE_PRIKEY,
} WDRV_WINC_FILE_TYPE;

// *****************************************************************************
/* File Mode.

  Summary:
    Possible file modes.

  Description:
    Possible file modes used when opening a file transfer operation.

  Remarks:
    None.
*/

typedef enum
{
    /* Invalid file mode. */
    WDRV_WINC_FILE_MODE_INVALID = 0,

    /* Write operation, local application to network controller. */
    WDRV_WINC_FILE_MODE_WRITE
} WDRV_WINC_FILE_MODE_TYPE;

// *****************************************************************************
/* File Status.

  Summary:
    List of file status values.

  Description:
    Enumeration of possible file status indication values.

  Remarks:
    None.
*/

typedef enum
{
    /* The file operation was in error. */
    WDRV_WINC_FILE_STATUS_ERROR,

    /* The file operation was successful. */
    WDRV_WINC_FILE_STATUS_OK,

    /* The file open operation completed. */
    WDRV_WINC_FILE_STATUS_OPEN,

    /* The file write operation completed. */
    WDRV_WINC_FILE_STATUS_WRITE_COMPLETE,

    /* The file was closed. */
    WDRV_WINC_FILE_STATUS_CLOSE,
} WDRV_WINC_FILE_STATUS_TYPE;

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_FILE_STATUS_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_FILE_HANDLE fHandle,
        uintptr_t userCtx,
        WDRV_WINC_FILE_STATUS_TYPE status
    )

  Summary:
    Definition of file status callback.

  Description:
    Definition of file status callback, used to indicate the status of a file
    transfer operation.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    fHandle - File transfer handle.
    userCtx - User context supplied to WDRV_WINC_FileOpen.
    status  - Current status of file operation.

  Returns:
    None.

  Remarks:
    WDRV_WINC_FILE_STATUS_OK:
        A transfer command has succeeded.

    WDRV_WINC_FILE_STATUS_ERROR:
        A file open or transfer command has failed.

    WDRV_WINC_FILE_STATUS_CLOSE:
        A file transfer has been completed and is closing.

    WDRV_WINC_FILE_STATUS_WRITE_COMPLETE:
        A write operation has completed.

*/

typedef void (*WDRV_WINC_FILE_STATUS_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_FILE_HANDLE fHandle,
    uintptr_t userCtx,
    WDRV_WINC_FILE_STATUS_TYPE status
);

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_FILE_FIND_CALLBACK)
    (
        DRV_HANDLE handle,
        uintptr_t userCtx,
        WDRV_WINC_FILE_TYPE type,
        const char *pFilename,
        WDRV_WINC_FILE_STATUS_TYPE status
    )

  Summary:
    Definition of file find result and status callback.

  Description:
    Definition of file find callback, used to indicate found files and operation
    status.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    userCtx   - User context supplied to WDRV_WINC_FileFind.
    type      - File type found.
    pFilename - Name of file found.
    status    - Status of operation.

  Returns:
    None.

  Remarks:
    WDRV_WINC_FILE_STATUS_OK:
        If pFilename is NULL this indicates success of the whole find operation.
        If pFilename is not NULL this indicates a single file found operation.

    WDRV_WINC_FILE_STATUS_ERROR:
        Indicates the file found operation failed.

*/

typedef void (*WDRV_WINC_FILE_FIND_CALLBACK)
(
    DRV_HANDLE handle,
    uintptr_t userCtx,
    WDRV_WINC_FILE_TYPE type,
    const char *pFilename,
    WDRV_WINC_FILE_STATUS_TYPE status
);

// *****************************************************************************
/*
  Function:
    void (*WDRV_WINC_FILE_DELETE_CALLBACK)
    (
        DRV_HANDLE handle,
        uintptr_t userCtx,
        WDRV_WINC_FILE_TYPE type,
        WDRV_WINC_FILE_STATUS_TYPE status
    )

  Summary:
    Definition of file delete callback.

  Description:
    Definition of file delete callback, used to indicate the status of a file
    delete operation.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    userCtx   - User context supplied to WDRV_WINC_FileFind.
    type      - File type found.
    status    - Status of operation.

  Returns:

  Remarks:
    WDRV_WINC_FILE_STATUS_OK:
        Indicate the file delete operation succeeded.

    WDRV_WINC_FILE_STATUS_ERROR:
        Indicates the file delete operation failed.

*/

typedef void (*WDRV_WINC_FILE_DELETE_CALLBACK)
(
    DRV_HANDLE handle,
    uintptr_t userCtx,
    WDRV_WINC_FILE_TYPE type,
    WDRV_WINC_FILE_STATUS_TYPE status
);

// *****************************************************************************
/* File Transfer Context.

  Summary:
    Structure for file transfer data context.

  Description:
    Maintains state information about a file transfer.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if structure is in use. */
    bool inUse;

    /* Flag indicating if the transfer is closing. */
    bool close;

    /* File type of file being transfered. */
    WDRV_WINC_FILE_TYPE type;

    /* Transfer handle returned by FS=LOAD operation. */
    uint16_t tsfrHandle;

    /* Command request handle of FS=LOAD, only valid until transfer
     handle is returned. */
    WINC_CMD_REQ_HANDLE cmdReqHandle;

    /* Pointer to transfer status callback. */
    WDRV_WINC_FILE_STATUS_CALLBACK pfFileStatusCb;

    /* User supplied context value passed to callback. */
    uintptr_t fileStatusCbCtx;

    /* Size of the file being transferred. */
    size_t fileSize;

    /* Union of operation specified data. */
    union
    {
        /* FS=LOAD operation. */
        struct
        {
            /* Last known block number. */
            uint16_t blockNum;

            /* Pointer to user supplied data buffer. */
            const uint8_t *pData;

            /* Length of data in user supplied data buffer. */
            size_t lenData;

            /* Length of data in the internal buffer. */
            uint8_t bufLen;

            /* Internal buffer to build up data for next transfer. */
            uint8_t buffer[WDRV_WINC_FILE_LOAD_BUF_SZ];
        } load;
    } op;
} WDRV_WINC_FILE_CTX;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver File Operation Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_FILE_HANDLE WDRV_WINC_FileOpen
    (
        DRV_HANDLE handle,
        const char *pFilename,
        WDRV_WINC_FILE_TYPE type,
        WDRV_WINC_FILE_MODE_TYPE mode,
        size_t lenFile,
        WDRV_WINC_FILE_STATUS_CALLBACK pfStatusCb,
        uintptr_t statusCbCtx
    )

  Summary:
    Open a file transfer.

  Description:
    Open a file transfer between user application and WINC device.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle      - WINC driver handle obtained from WDRV_WINC_Open.
    pFilename   - Pointer to filename to open.
    type        - Type of file.
    mode        - Mode of opening.
    lenFile     - Length of file to transfer.
    pfStatusCb  - Pointer to application callback to receive status.
    statusCbCtx - User context value to pass to callback.

  Returns:
    File handle or WDRV_WINC_FILE_INVALID_HANDLE.

  Remarks:
    None.

*/

WDRV_WINC_FILE_HANDLE WDRV_WINC_FileOpen
(
    DRV_HANDLE handle,
    const char *pFilename,
    WDRV_WINC_FILE_TYPE type,
    WDRV_WINC_FILE_MODE_TYPE mode,
    size_t lenFile,
    WDRV_WINC_FILE_STATUS_CALLBACK pfStatusCb,
    uintptr_t statusCbCtx
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_FileClose
    (
        DRV_HANDLE handle,
        WDRV_WINC_FILE_HANDLE fHandle
    )

  Summary:
    Close a file transfer.

  Description:
    Close a file transfer between user application and WINC device.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_FileOpen should have been called to obtain a valid file handle.

  Parameters:
    handle  - WINC driver handle obtained from WDRV_WINC_Open.
    fHandle - File handle received from WDRV_WINC_FileOpen.

  Returns:
    WDRV_WINC_STATUS_OK             - A scan was initiated.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_BUSY           - Transfer is busy

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_FileClose
(
    DRV_HANDLE handle,
    WDRV_WINC_FILE_HANDLE fHandle
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_FileWrite
    (
        DRV_HANDLE handle,
        WDRV_WINC_FILE_HANDLE fHandle,
        const uint8_t *pData,
        size_t lenData
    )

  Summary:
    Write data to a file.

  Description:
    Write data to a file from user application to WINC device.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.
    WDRV_WINC_FileOpen should have been called to obtain a valid file handle.

  Parameters:
    handle  - WINC driver handle obtained from WDRV_WINC_Open.
    fHandle - File handle received from WDRV_WINC_FileOpen.
    pData   - Pointer to data to write.
    lenData - Length of data to write.

  Returns:
    WDRV_WINC_STATUS_OK             - A scan was initiated.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_BUSY           - Transfer is busy

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_FileWrite
(
    DRV_HANDLE handle,
    WDRV_WINC_FILE_HANDLE fHandle,
    const uint8_t *pData,
    size_t lenData
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_FileFind
    (
        DRV_HANDLE handle,
        WDRV_WINC_FILE_TYPE type,
        WDRV_WINC_FILE_FIND_CALLBACK pfFindCb,
        uintptr_t findCbCtx
    )

  Summary:
    Find a type of file.

  Description:
    Requests the WINC device send a list of a particular file type.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle    - WINC driver handle obtained from WDRV_WINC_Open.
    type      - Type of file.
    pfFindCb  - Pointer to application callback to receive file information and status.
    findCbCtx - User context value to pass to callback.

  Returns:
    WDRV_WINC_STATUS_OK             - A scan was initiated.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_RETRY_REQUEST  - Another request is being processed, try again.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_FileFind
(
    DRV_HANDLE handle,
    WDRV_WINC_FILE_TYPE type,
    WDRV_WINC_FILE_FIND_CALLBACK pfFindCb,
    uintptr_t findCbCtx
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_FILE_HANDLE WDRV_WINC_FileDelete
    (
        DRV_HANDLE handle,
        const char *pFilename,
        WDRV_WINC_FILE_TYPE type,
        WDRV_WINC_FILE_DELETE_CALLBACK pfDeleteCb,
        uintptr_t deleteCbCtx
    )

  Summary:
    Delete a file.

  Description:
    Requests the WINC device deletes a file of a particular name and type.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle      - WINC driver handle obtained from WDRV_WINC_Open.
    pFilename   - Pointer to filename to delete.
    type        - Type of file.
    pfDeleteCb  - Pointer to application callback to receive status.
    deleteCbCtx - User context value to pass to callback.

  Returns:
    WDRV_WINC_STATUS_OK             - A scan was initiated.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_RETRY_REQUEST  - Another request is being processed, try again.

  Remarks:
    None.

*/

WDRV_WINC_FILE_HANDLE WDRV_WINC_FileDelete
(
    DRV_HANDLE handle,
    const char *pFilename,
    WDRV_WINC_FILE_TYPE type,
    WDRV_WINC_FILE_DELETE_CALLBACK pfDeleteCb,
    uintptr_t deleteCbCtx
);

#endif /* WDRV_WINC_FILE_H */
