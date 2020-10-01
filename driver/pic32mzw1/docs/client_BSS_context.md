---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client - BSS Context APIs
has_toc: false
nav_order: 2
---

# Client - BSS Context APIs

The BSS context contains information relevant to a BSS such as SSID and channel.
The structure ```WDRV_PIC32MZW_BSS_CONTEXT``` is provided to contain the context.
To initialize a context, the function ```WDRV_PIC32MZW_BSSCtxSetDefaults``` is provided, this ensures the context is in a known state.
The function ```WDRV_PIC32MZW_BSSCtxIsValid``` is provided to test if a context is valid. In some applications, a BSS context is valid even if an SSID is blank. Therefore this function can be informed whether a blank SSID is valid or not.
The SSID of the BSS can be configured using the function ```WDRV_PIC32MZW_BSSCtxSetSSID```.
The channel of the BSS can be configure using the function W```DRV_PIC32MZW_BSSCtxSetChannel```.
In some applications a BSS can be cloaked therefore the state can be configured in a BSS context using the function ```WDRV_PIC32MZW_BSSCtxSetSSIDVisibility``` to indicate if an BSS is considered visible or not.

### Available APIs are listed below


```
bool WDRV_PIC32MZW_BSSCtxIsValid
(
	const WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx, 
	bool ssidValid
)
``` 

#### Description:
Tests the elements of the BSS context to judge if their values are legal.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
|```pBSSCtx``` |	Pointer to a BSS context.|
| ```ssidValid``` |	Flag indicating if the SSID within the context must be valid.|
|

#### Returns:
true or false indicating if context is valid.

#### Remarks:
A valid SSID is one which has a non-zero length. The check is optional as
it is legal for the SSID field to be zero length.

-----------------------------------------------------------------------------------------------------------
 
```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSCtxSetDefaults
(
	WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx
)
```

#### Description:
Ensures that each element of the structure is configured into a legal state.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
| ```pBSSCtx``` |	Pointer to a BSS context. |
|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.

#### Remarks:
A default context is not valid until it is configured.

-----------------------------------------------------------------------------------------------------------
 
```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSCtxSetSSID
(
	WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx, 
	uint8_t *const pSSID, 
	uint8_t ssidLength
)
```

#### Description:
The SSID string and length provided are copied into the BSS context.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
| ```pBSSCtx``` |	Pointer to a BSS context. |
| ```pSSID``` |	Pointer to buffer containing the new SSID. |
| ```ssidLength``` |	The length of the SSID held in the pSSID buffer. |
|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_INVALID_CONTEXT	- The BSS context is not valid.

-----------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSCtxSetBSSID
(
	WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx, 
	uint8_t *const pBSSID
)
```

#### Description:
The BSSID string is copied into the BSS context.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
| ```pBSSCtx```	|	Pointer to a BSS context.|
| ```pBSSID```		| Pointer to buffer containing the new BSSID.|
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK			- The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_INVALID_CONTEXT	- The BSS context is not valid.

-----------------------------------------------------------------------------------------------------------
 
```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSCtxSetChannel
(
	WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx, 
	WDRV_PIC32MZW_CHANNEL_ID channel
)
```

#### Description:
The supplied channel value is copied into the BSS context.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
| ```pBSSCtx```	|	Pointer to a BSS context.|
| ```channel```	|	Channel number.|
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK			- The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_INVALID_CONTEXT	- The BSS context is not valid.

#### Remarks:
channel may be ```WDRV_PIC32MZW_ALL_CHANNELS``` to represent no fixed channel.

-----------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSCtxSetSSIDVisibility
(
	WDRV_PIC32MZW_BSS_CONTEXT *const pBSSCtx, 
	bool visible
)
```

#### Description:
Specific to Soft-AP mode this flag defines if the BSS context will create a visible presence on air.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
| ```pBSSCtx``` |	Pointer to a BSS context.|
| ```visible```	|	Boolean flag value indicating if the BSS will be visible or not.|
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK			- The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_INVALID_CONTEXT	- The BSS context is not valid.
