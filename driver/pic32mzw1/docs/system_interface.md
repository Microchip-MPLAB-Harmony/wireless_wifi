---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: System Interface APIs
has_toc: false
nav_order: 2
---

# System Interface APIs

The PIC32MZW1 Wi-Fi module consists of two instances, a control instance and a data instance.
The control instance is driven as a conventional Harmony system interface by the system kernel or scheduler to initialize and run the module, the data instance implements a Harmony TCP/IP stack MAC Driver module instance driven by the Harmony TCP/IP stack.

```
SYS_MODULE_OBJ WDRV_PIC32MZW_Initialize
(
	const SYS_MODULE_INDEX index, 
	const SYS_MODULE_INIT *const init
)
```

#### Description:
This function initializes the PIC32MZW1 Wi-Fi module instance.

#### Preconditions:
None.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
| ```index``` | Zero-based index of the module instance to be initialized. This value is either: - Zero: Module Control Instance, - Non_Zero: Module Data Instance | 
| ```init```	| Pointer to the data structure containing any data necessary to initialize the module. This pointer may be null if no data is required. |
|

#### Returns:
A handle to the instance of the module that was initialized.
NOTE: This handle is a necessary parameter to all the other system level routines for that module.

#### Remarks:
For the control instance this function will normally only be called once during system initialization.

---------------------------------------------------------------------------------------------------------

```
void WDRV_PIC32MZW_Deinitialize(SYS_MODULE_OBJ object)
```

#### Description:
This function deinitializes a PIC32MZW1 Wi-Fi module instance.

#### Preconditions:
WDRV_PIC32MZW_Initialize must have been called.

#### Parameters:
object	Handle to the module instance.

#### Remarks:
If the module instance must be used again, the module's "initialize" function must first be called.
 
