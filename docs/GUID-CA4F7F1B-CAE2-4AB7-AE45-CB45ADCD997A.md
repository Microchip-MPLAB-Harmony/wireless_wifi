# WDRV_WINC_Tasks Function

## C

```c
void WDRV_WINC_Tasks(SYS_MODULE_OBJ object)
```

## Summary

Maintains the WINC drivers state machine.  

## Description

This function is used to maintain the driver's internal state machine.

## Precondition

WDRV_WINC_Initialize must have been called before calling this function.  

## Parameters

| Param | Description |
|:----- |:----------- |
| object | Driver object handle, returned from WDRV_WINC_Initialize  

## Returns

None.  

## Remarks

This function is normally not called directly by an application. It is called by the system's Tasks routine (SYS_Tasks).  


