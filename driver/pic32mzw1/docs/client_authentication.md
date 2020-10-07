---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client Interface - Authentication Context APIs
has_toc: false
nav_order: 2
---

# Client Interface - Authentication Context APIs

The authentication context contains information relevant to the authentication mechanisms used in Wi-Fi. Currently supported are Open, WEP and WPA-PSK.
The structure ```WDRV_PIC32MZW_AUTH_CONTEXT``` is provided to contain the context.
To initialize a context, the function ```WDRV_PIC32MZW_AuthCtxSetDefaults``` is provided, which ensures the context is in a known state.
The function ```WDRV_PIC32MZW_AuthCtxIsValid``` is provided to test if a context is valid.
Each authentication type has at least one function which can be used to configure the type of authentication algorithm used as well as the parameters which are required.



### Available APIs are listed below

```
bool WDRV_PIC32MZW_AuthCtxIsValid(const WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx)
```

#### Description:
Tests the elements of the authentication context to judge if their values are legal.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
|```pAuthCtx``` |	Pointer to an authentication context. |
|

#### Returns:
True or False indicating if context is valid.

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_AuthCtxSetDefaults
(
	WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx
)
```

**element of the structure is configured into a default state.**
  
#### Parameters:

|	Parameter			 | 				Description								|
|-------------------------|-----------------------------------------------------|
| ```pAuthCtx``` |	Pointer to an authentication context. |
|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK - The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG - The parameters were incorrect.

#### Remarks:
A default context is not valid until it is configured.

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_AuthCtxSetOpen
(
	WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx
)
```

#### Description:
The type and state information are configured appropriately for Open authentication.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
| ```pAuthCtx``` |	Pointer to an authentication context. |
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK			- The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 
```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_AuthCtxSetWEP
(
	WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx, 
	uint8_t idx, 
	uint8_t *pKey, 
	uint8_t size
)
```

#### Description:
The type and state information are configured appropriately for WEP authentication.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
|```pAuthCtx``` |	Pointer to an authentication context.|
|```idx``` |	WEP index.|
|```pKey``` |	Pointer to WEP key.|
|```size``` |	Size of WEP key.|
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK			- The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_AuthCtxSetPersonal
(
	WDRV_PIC32MZW_AUTH_CONTEXT *const pAuthCtx, 
	uint8_t *pPassword, 
	uint8_t size, 
	WDRV_PIC32MZW_AUTH_TYPE authType
)
```

#### Description:
The type and state information are configured appropriately for WPA-PSK authentication.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
|```pAuthCtx``` |	Pointer to an authentication context.|
|```pPassword``` |	Pointer to password (or 64-character PSK).|
|```size ```|	Size of password (or 64 for PSK).|
|```authType``` |	Authentication type (or WDRV_PIC32MZW_AUTH_TYPE_DEFAULT).|
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK - The context has been configured.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG - The parameters were incorrect.
 