# WDRV_PIC32MZW_PowerSaveListenIntervalSet Function

## C

```c
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_PowerSaveListenIntervalSet
(
DRV_HANDLE handle,
uint16_t u16ListenInt
);
```

## Summary

Set the Wi-Fi listen interval for power save operation(in beacon period count).  

## Description

Set the Wi-Fi listen interval value for power save operation.It is given in
units of Beacon period.

Periodically after the listen interval fires, the WiFi wakes up and listen
to the beacon and check for any buffered frames for it from the AP.

A default value of 10 is used by the WiFi stack for listen interval. The user
can override that value via this API.

## Precondition

WDRV_PIC32MZW_Initialize should have been called. WDRV_PIC32MZW_Open should have been called to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_PIC32MZW_Open. |
| u16ListenInt | Listen interval in units of beacon period.  

## Returns

*WDRV_PIC32MZW_STATUS_OK* - The request has been accepted.

*WDRV_PIC32MZW_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_PIC32MZW_STATUS_INVALID_ARG* - The parameters were incorrect.

*WDRV_PIC32MZW_STATUS_REQUEST_ERROR* - The request to the PIC32MZW was rejected.
 

## Remarks

WDRV_PIC32MZW_PowerSaveListenIntervalSet should be called before WDRV_PIC32MZW_BSSConnect.  


