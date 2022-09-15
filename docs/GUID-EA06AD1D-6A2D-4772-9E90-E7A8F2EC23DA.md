# WDRV_PIC32MZW_BSSFindInProgress Function

## C

```c
bool WDRV_PIC32MZW_BSSFindInProgress(DRV_HANDLE handle)
```

## Summary

Indicates if a BSS scan is in progress.  

## Description

Returns a flag indicating if a BSS scan operation is currently running.

## Precondition

WDRV_PIC32MZW_Initialize must have been called. WDRV_PIC32MZW_Open must have been called to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_PIC32MZW_Open.  

## Returns

Flag indicating if a scan is in progress. If an error occurs the result is false.  

## Remarks

None.  
