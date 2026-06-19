# WDRV_WINC_PowerSaveSetBeaconInterval Function

## C

```c
WDRV_WINC_STATUS WDRV_WINC_PowerSaveSetBeaconInterval
(
DRV_HANDLE handle,
uint16_t numBeaconIntervals
)
```

## Summary

Configures the beacon listening interval.  

## Description

The beacon listening interval specifies how many beacon period will occur
between listening actions when the WINC is in low power mode.

## Precondition

WDRV_WINC_Initialize should have been called. WDRV_WINC_Open should have been called to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_WINC_Open. |
| numBeaconIntervals | Number of beacon intervals between listening operations.  

## Returns

*WDRV_WINC_STATUS_OK* - The interval has been accepted.

*WDRV_WINC_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_WINC_STATUS_INVALID_ARG* - The parameters were incorrect.

*WDRV_WINC_STATUS_REQUEST_ERROR* - The request to the WINC was rejected.
 

## Remarks

None.  


