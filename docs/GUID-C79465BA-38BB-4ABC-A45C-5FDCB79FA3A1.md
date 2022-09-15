# WDRV_WINC_NVMWrite Function

## C

```c
WDRV_WINC_STATUS WDRV_WINC_NVMWrite
(
DRV_HANDLE handle,
WDRV_WINC_NVM_REGION region,
void *pBuffer,
uint32_t offset,
uint32_t size
);
```

## Summary

Writes data to an NVM region.  

## Description

Writes the number of bytes specified to the NVM region.

## Precondition

WDRV_WINC_Initialize should have been called. WDRV_WINC_Open should have been called with the intent DRV_IO_INTENT_EXCLUSIVE to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_WINC_Open. |
| region | Region of NVM to write. |
| pBuffer | Pointer to buffer containing the data to write. |
| offset | Offset within NVM region to write the data to. |
| size | Number of bytes to be written.  

## Returns

*WDRV_WINC_STATUS_OK* - The write completed successfully.

*WDRV_WINC_STATUS_INVALID_ARG* - The parameters were incorrect.

*WDRV_WINC_STATUS_REQUEST_ERROR* - The request encountered an error.
 

## Remarks

The WINC driver must have been opened for exclusive access to the flash to ensure the WINC firmware is not operating when manipulating the SPI flash.  The write operation does not erase the flash device first, therefore the user must ensure the flash region has been erased first.  


