# WDRV_PIC32MZW_BSSFindReset Function

## C

```c
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSFindReset
(
DRV_HANDLE handle,
WDRV_PIC32MZW_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
)
```

## Summary

Request the first scan results again  

## Description

The information structure of the first BSS is requested from the PIC32MZW.

## Precondition

WDRV_PIC32MZW_Initialize must have been called. WDRV_PIC32MZW_Open must have been called to obtain a valid handle. WDRV_PIC32MZW_BSSFindFirst must have been called.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_PIC32MZW_Open. |
| pfNotifyCallback | Callback to receive notification of next BSS found.  

## Returns

*WDRV_PIC32MZW_STATUS_OK* - The request was accepted.

*WDRV_PIC32MZW_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_PIC32MZW_STATUS_INVALID_ARG* - The parameters were incorrect.

*WDRV_PIC32MZW_STATUS_SCAN_IN_PROGRESS* - A scan is already in progress.

*WDRV_PIC32MZW_STATUS_BSS_FIND_END* - No more results are available.
 

## Remarks

None.  


