# WDRV_PIC32MZW_StatusExt Function

## C

```c
WDRV_PIC32MZW_SYS_STATUS WDRV_PIC32MZW_StatusExt(SYS_MODULE_OBJ object)
```

## Summary

Provides the extended system status of the PIC32MZW driver module.  

## Description

This function provides the extended system status of the PIC32MZW driver
module.

## Precondition

WDRV_PIC32MZW_Initialize must have been called before calling this function.  

## Parameters

| Param | Description |
|:----- |:----------- |
| object | Driver object handle, returned from WDRV_PIC32MZW_Initialize  

## Returns

*WDRV_PIC32MZW_SYS_STATUS_RF_INIT_BUSY* - RF initialisation is in progress

*WDRV_PIC32MZW_SYS_STATUS_RF_CONF_MISSING* - RF configuration is missing

*WDRV_PIC32MZW_SYS_STATUS_RF_READY* - RF is configured and is ready
 

## Remarks

None.  


