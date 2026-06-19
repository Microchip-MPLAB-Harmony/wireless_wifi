# WDRV_PIC32MZW_InfoOpChanGet Function

## C

```c
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_InfoOpChanGet
(
DRV_HANDLE handle,
uint8_t *const pMACAddress
)
```

## Summary

Retrieves the operating channel of the PIC32MZW.  

## Description

Retrieves the current operating channel.

## Precondition

WDRV_PIC32MZW_Initialize should have been called. WDRV_PIC32MZW_Open should have been called to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_PIC32MZW_Open. |
| pOpChan | Pointer to variable to receive the operating channel.  

## Returns

*WDRV_PIC32MZW_STATUS_OK* - The information has been returned.

*WDRV_PIC32MZW_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_PIC32MZW_STATUS_INVALID_ARG* - The parameters were incorrect.

*WDRV_PIC32MZW_STATUS_NOT_CONNECTED* - No current connection.
 

## Remarks

None.  


