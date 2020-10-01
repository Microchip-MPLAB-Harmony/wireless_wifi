---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client Interface - Soft AP mode APIs
has_toc: false
nav_order: 2
---

# Client Interface - Soft AP mode APIs

The PIC32MZW1 device can create a Soft-AP allowing a Wi-Fi station device to connect to the PIC32MZW1.
To create a Soft-AP the function ```WDRV_PIC32MZW_APStart``` is called.

This function takes several context structures to config aspects of the Soft-AP operation, these contexts are:
-	BSS		- Describes the BSS which will be created.
-	Authentication	- Describes the security level to be used.

Specification of a BSS context is required. If no authentication context is provided, the Wi-Fi driver will create a Soft-AP with no security.

A callback is also provided to update the application on events.
To stop using the Soft-AP the function ```WDRV_PIC32MZW_APStop``` is called.

...

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_APStart
(
    DRV_HANDLE handle,
    const WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx,
    const WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx,
    const WDRV_PIC32MZW_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
)
```

#### Description:
Using the defined BSS and authentication contexts this function creates and starts a Soft-AP instance.

#### Preconditions:

###### ```WDRV_PIC32MZW_Initialize``` should have been called.
###### ```WDRV_PIC32MZW_Open``` should have been called to obtain a valid handle.
###### A BSS context must have been created and initialized.
###### An authentication context must have been created and initialized.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
| ```handle``` | Client handle obtained by a call to ```WDRV_PIC32MZW_Open```. |
| ```pBSSCtx``` |	Pointer to BSS context. |
| ```pAuthCtx``` | Pointer to authentication context. |
| ```pfNotifyCallback``` | Pointer to function of the following prototype:  ```void func ( DRV_HANDLE handle, WDRV_PIC32MZW_ASSOC_HANDLE assocH, WDRV_PIC32MZW_CONN_STATE state )``` |
|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The request has been accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- The request to the PIC32MZW1 was rejected.
###### WDRV_PIC32MZW_STATUS_INVALID_CONTEXT	- The BSS context is not valid.

-------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_APStop(DRV_HANDLE handle)
```

#### Description:

Stops an instance of Soft-AP.

#### Precondition:

###### ```WDRV_PIC32MZW_Initialize``` should have been called.
###### ```WDRV_PIC32MZW_Open``` should have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|```handle```  |	Client handle obtained by a call to WDRV_PIC32MZW_Open. |
|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The request has been accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- The request to the PIC32MZW1 was rejected.
 
