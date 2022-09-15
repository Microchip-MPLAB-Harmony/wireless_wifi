# WDRV_WINC_WPSEnrolleeDiscoveryStartPIN Function

## C

```c
WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStartPIN
(
DRV_HANDLE handle,
uint32_t pin,
const WDRV_WINC_WPS_DISC_CALLBACK pfWPSDiscoveryCallback
)
```

## Summary

Start WPS discovery (PIN).  

## Description

Starts the WPS discovery as an enrollee using the PIN method.

## Precondition

WDRV_WINC_Initialize should have been called. WDRV_WINC_Open should have been called to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_WINC_Open. |
| pin | WPS PIN number. |
| pfWPSDiscoveryCallback | Callback function to return discovery results to.  

## Returns

*WDRV_WINC_STATUS_OK* - Discovery request accepted.

*WDRV_WINC_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_WINC_STATUS_INVALID_ARG* - The parameters were incorrect.

*WDRV_WINC_STATUS_REQUEST_ERROR* - The request to the WINC was rejected.
 

## Remarks

The pin number is an 8-digit number where the least significant digit is a checksum as defined by WPS.  


