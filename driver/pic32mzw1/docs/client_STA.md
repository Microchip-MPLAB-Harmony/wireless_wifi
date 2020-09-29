---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client Interface - STA mode APIs
has_toc: false
nav_order: 2
---

# Client Interface - STA mode APIs

The PIC32MZW1 device can connect to a BSS as a Wi-Fi station device. 

To connect to a BSS the function WDRV_PIC32MZW_BSSConnect can be called, this takes two contexts which provide information about the BSS to be connected to and the authentication method to be used. A callback is provided to communicate events back to the application regarding connection status.

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSConnect
(
    DRV_HANDLE handle,
    const WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx,
    const WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx,
    const WDRV_PIC32MZW_BSSCON_NOTIFY_CALLBACK pfNotifyCallback
)
```
#### Description:
Using the defined BSS and authentication contexts, this function requests the PIC32MZW1 connect to the BSS as an infrastructure station.

#### Preconditions:
##### - WDRV_PIC32MZW_Initialize should have been called.
##### - WDRV_PIC32MZW_Open should have been called to obtain a valid handle.
##### - A BSS context must have been created and initialized.
##### - An authentication context must have been created and initialized.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
| ```handle```           | Client handle obtained by a call to ```WDRV_PIC32MZW_Open``` |
| ```pBSSCtx```          |	Pointer to BSS context  									|
| ```pAuthCtx```         | Pointer to authentication context 							|
| ```pfNotifyCallback``` | Pointer to function of the following prototype:  ```void func ( DRV_HANDLE handle, WDRV_PIC32MZW_ASSOC_HANDLE assocH, WDRV_PIC32MZW_CONN_STATE state )``` |
 

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK				- The request has been accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN			- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- The request to the PIC32MZW1 was rejected.
###### WDRV_PIC32MZW_STATUS_INVALID_CONTEXT	- The BSS context is not valid.
###### WDRV_PIC32MZW_STATUS_CONNECT_FAIL		- The connection has failed.

---------------------------------------------------------------------------------------------------------
```WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSDisconnect(DRV_HANDLE handle)```

#### Description:

Disconnects from an existing BSS.

#### Preconditions:
##### ```WDRV_PIC32MZW_Initialize``` should have been called.

##### ```WDRV_PIC32MZW_Open``` should have been called to obtain a valid handle.

##### Parameters:
##### ```handle```	Client handle obtained by a call to WDRV_PIC32MZW_Open.

##### Returns:
###### WDRV_PIC32MZW_STATUS_OK				- The request has been accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_DISCONNECT_FAIL	- The disconnection has failed.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- The request to the PIC32MZW1 was rejected.
