# WDRV_PIC32MZW_BSSFindSetScanParameters Function

## C

```c
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSFindSetScanParameters
(
DRV_HANDLE handle,
uint8_t numSlots,
uint16_t activeSlotTime,
uint16_t passiveSlotTime,
uint8_t numProbes
)
```

## Summary

Configures the scan operation.  

## Description

Configures the time periods of active and passive scanning operations.

## Precondition

WDRV_PIC32MZW_Initialize must have been called. WDRV_PIC32MZW_Open must have been called to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_PIC32MZW_Open. |
| numSlots | Number of slots (minimum is 2). |
| activeSlotTime | Time spent on each active channel probing for BSS's. |
| passiveSlotTime | Time spent on each passive channel listening for beacons. |
| numProbes | Number of probes per slot.  

## Returns

*WDRV_PIC32MZW_STATUS_OK* - The request was accepted.

*WDRV_PIC32MZW_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_PIC32MZW_STATUS_INVALID_ARG* - The parameters were incorrect.
 

## Remarks

If any parameter is zero then the configured value is unchanged.  


