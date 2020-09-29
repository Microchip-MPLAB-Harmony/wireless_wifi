---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client - Association APIs
has_toc: false
nav_order: 2
---

# Client - Association APIs

This interface provides information about the current association with a peer device.


### Available APIs are listed below


```WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_AssocPeerAddressGet
(
	WDRV_PIC32MZW_ASSOC_HANDLE assocHandle, 
	WDRV_PIC32MZW_MAC_ADDR *const pPeerAddress
)
```
#### Description:
Attempts to retrieve the network address of the peer device in the current association.

#### Preconditions:

```WDRV_PIC32MZW_Initialize``` must have been called.
```WDRV_PIC32MZW_Open``` must have been called to obtain a valid handle.

A peer device needs to be connected and associated, the association handle should be obtained from the ```WDRV_PIC32MZW_BSSCON_NOTIFY_CALLBACK``` callback

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|```assocHandle```	|	Association handle.|
|```pPeerAddress```	|	Pointer to structure to receive the network address.|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK	The request was accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN	The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG	The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	The PIC32MZW1 was unable to accept this request.
###### WDRV_PIC32MZW_STATUS_RETRY_REQUEST	The network address is not available, but it will be requested from the PIC32MZW1.
###### WDRV_PIC32MZW_STATUS_NOT_CONNECTED	Not currently connected.


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

 
```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_AssocRSSIGet
(
	WDRV_PIC32MZW_ASSOC_HANDLE assocHandle, 
	int8_t *const pRSSI, 
	WDRV_PIC32MZW_ASSOC_RSSI_CALLBACK const pfAssociationRSSICB
)
```

#### Description:

Attempts to retrieve the RSSI of the current association.

#### Preconditions:

```WDRV_PIC32MZW_Initialize``` must have been called.
```WDRV_PIC32MZW_Open``` must have been called to obtain a valid handle.
A peer device needs to be connected and associated.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|```assocHandle```	|	Association handle.|
|```pRSSI```	|	Pointer to variable to receive RSSI if available.|
|```pfAssociationRSSICB```	|	Pointer to callback function to be used when RSSI value is available.|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The request was accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- PIC32MZW1 was unable to accept request.
###### WDRV_PIC32MZW_STATUS_RETRY_REQUEST	- Network address unavailable.
###### WDRV_PIC32MZW_STATUS_NOT_CONNECTED	- Not currently connected.

 
