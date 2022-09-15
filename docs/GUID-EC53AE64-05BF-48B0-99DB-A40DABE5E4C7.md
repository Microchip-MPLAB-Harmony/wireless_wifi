# WDRV_PIC32MZW_Initialize Function

## C

```c
SYS_MODULE_OBJ WDRV_PIC32MZW_Initialize
(
const SYS_MODULE_INDEX index,
const SYS_MODULE_INIT *const init
)
```

## Summary

System interface initialization of the PIC32MZW driver.  

## Description

This is the function that initializes the PIC32MZW driver. It is called by
the system.

## Precondition

None.  

## Parameters

| Param | Description |
|:----- |:----------- |
| index | Index for the PIC32MZW driver instance to be initialized. |
| init | Pointer to initialization data, currently not used.  

## Returns

*Valid handle to a driver object* - if successful

*SYS_MODULE_OBJ_INVALID* - if initialization failed
 

## Remarks

The returned object must be passed as argument to WDRV_PIC32MZW_Reinitialize, WDRV_PIC32MZW_Deinitialize, WDRV_PIC32MZW_Tasks and WDRV_PIC32MZW_Status routines.  


