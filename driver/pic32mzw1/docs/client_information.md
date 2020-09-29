---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client - Information APIs
has_toc: false
nav_order: 2
---

# Client - Information APIs

This interface provides general information about the device.

### Available APIs are listed below


```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_InfoDeviceMACAddressGet
(
	DRV_HANDLE handle, uint8_t *const pMACAddress
)

```

#### Description:

Retrieves the current working MAC address.
#### Preconditions:

```WDRV_PIC32MZW_Initialize``` must have been called.
```WDRV_PIC32MZW_Open``` must have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|```handle```		|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|```pMACAddress```	|	Pointer to buffer (of at least 6 bytes in length) to receive the MAC address.|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The information has been returned.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_InfoOpChanGet
(
	DRV_HANDLE handle, WDRV_PIC32MZW_CHANNEL_ID *const pMACAddress
)
```

#### Description:
Retrieves the current operating channel.

#### Preconditions:

```WDRV_PIC32MZW_Initialize``` must have been called.
```WDRV_PIC32MZW_Open``` must have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
| ```handle```		| Client handle obtained by a call to WDRV_PIC32MZW_Open.	|
| ```pOpChan```	| Pointer to variable to receive the operating channel. |

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The information has been returned.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
