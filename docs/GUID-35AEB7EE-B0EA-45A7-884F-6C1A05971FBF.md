# WDRV_WINC_BSSFindSetScanParameters Function

## C

```c
WDRV_WINC_STATUS WDRV_WINC_BSSFindSetScanParameters
(
DRV_HANDLE handle,
uint8_t numSlots,
uint8_t activeSlotTime,
uint8_t passiveSlotTime,
uint8_t numProbes
)
```

## Summary

Configures the scan operation.  

## Description

The scan consists of a number of slots per channel, each slot can has a
configurable time period and number of probe requests to send.

## Precondition

WDRV_WINC_Initialize must have been called. WDRV_WINC_Open must have been called to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_WINC_Open. |
| numSlots | Number if slots (minimum is 2). |
| activeSlotTime | Time waiting for responses (10ms <= slotTime <= 250ms) for active scans. |
| passiveSlotTime | Time waiting for responses (10ms <= slotTime <= 1200ms) for passive scans. |
| numProbes | Number of probes per slot.  

## Returns

*WDRV_WINC_STATUS_OK* - The request was accepted.

*WDRV_WINC_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_WINC_STATUS_INVALID_ARG* - The parameters were incorrect.
 

## Remarks

If any parameter is zero then the configured value is unchanged.  


