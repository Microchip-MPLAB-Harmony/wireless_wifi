---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client Open/Close APIs
has_toc: false
nav_order: 2
---

# Client Open/Close APIs

The PIC32MZW1 Wi-Fi module consists of two instances, a control instance and a data instance.
The control instance is driven as a conventional Harmony system interface by the system kernel or scheduler to initialize and run the module, the data instance implements a Harmony TCP/IP stack MAC Driver module instance driven by the Harmony TCP/IP stack.


```
DRV_HANDLE WDRV_PIC32MZW_Open
(
	const SYS_MODULE_INDEX index,
	const DRV_IO_INTENT intent
)
```

#### Description:
Opens an instance of the PIC32MZW driver and returns a handle which must be used when calling other driver functions.

#### Preconditions:
```WDRV_PIC32MZW_Initialize``` must have been called.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
| ```index``` | Identifier for the driver instance to be opened. | 
|```intent```	| Zero or more of the values from the enumeration DRV_IO_INTENT ORed together to indicate the intended use of the driver |
|

#### Returns:

##### Valid handle - if the open function succeeded.

##### DRV_HANDLE_INVALID - if an error occurs

#### Remarks:
The index and intent parameters are not used in the current implementation and are maintained only for compatibility with the generic driver Open function signature.

---------------------------------------------------------------------------------------------------------

```
void WDRV_PIC32MZW_Close(DRV_HANDLE handle)
```

#### Description:
This is the function that closes an instance of the MAC. All per client data is released and the handle can no longer be used after this function is called.

#### Preconditions:
```WDRV_PIC32MZW_Initialize``` must have been called.
```WDRV_PIC32MZW_Open``` should have been called to obtain a valid handle.

#### Parameters:
```handle```  - Client handle obtained by a call to WDRV_PIC32MZW_Open.

#### Returns:
None


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_SYS_STATUS WDRV_PIC32MZW_StatusExt
(
	SYS_MODULE_OBJ object
)
```

#### Description:
This function provides the extended system status of the PIC32MZW driver module.

#### Preconditions:
```WDRV_PIC32MZW_Initialize``` must have been called.

#### Parameters:
object	 Driver object handle, returned from ```WDRV_PIC32MZW_Initialize```

#### Returns:
WDRV_PIC32MZW_SYS_STATUS_RF_INIT_BUSY    - RF initialisation is in progress

WDRV_PIC32MZW_SYS_STATUS_RF_CONF_MISSING - RF configuration is missing

WDRV_PIC32MZW_SYS_STATUS_RF_READY        - RF is configured and is ready
 

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_PMKCacheFlush
(
    DRV_HANDLE handle
)
```

#### Description:
This function provides the extended system status of the PIC32MZW driver module.

#### Preconditions:
WDRV_PIC32MZW_Initialize must have been called.

#### Parameters:
handle	 Client handle obtained by a call to ```WDRV_PIC32MZW_Open```.

#### Returns:

WDRV_PIC32MZW_STATUS_OK              - The request has been accepted.

WDRV_PIC32MZW_STATUS_NOT_OPEN        - The driver instance is not open.

WDRV_PIC32MZW_STATUS_INVALID_ARG     - The parameters were incorrect.

WDRV_PIC32MZW_STATUS_REQUEST_ERROR   - The request to the PIC32MZW was rejected.

