---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client - Regulatory Domain APIs
has_toc: false
nav_order: 2
---

# Regulatory Domain Selection APIs

![note](images/Regulatory_Note_DS.png)

<br />
<br />

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_RegDomainSet
(
	DRV_HANDLE handle,
	const char *pRegDomain, 
	const WDRV_PIC32MZW_REGDOMAIN_CALLBACK pfRegDomCallback
)
```

#### Description:

Requests that the current regulatory domain is changed to that specified.

#### Preconditions:

```WDRV_PIC32MZW_Initialize``` should have been called.

```WDRV_PIC32MZW_Open``` should have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
|```handle```		|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|```pRegDomain```	|	Pointer to a string name of the regulatory domain.|
|

#### String	Region

| String  | Region |
|---------|--------------|
|GEN	|	Generic/world-wide region|
|USA	| 	North America|
|EMEA	|	Europe|
|JPN	|	Japan|
|CUST1	|	Provided for customer configuration (chip-down)|
|CUST2	|	Provided for customer configuration (chip-down)|
|

Please note that these are actually ‘free format’ strings; the above table presents values that will commonly appear in pre-programmed modules.


|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
| ```pfRegDomCallback```	|	Pointer to callback function to receive the requested regulatory domain information. A pointer to a function of the following prototype: ```bool func(DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, bool isCurrent, const char * pRegDomain )```|
|

When called in response to a SET request, the callback will receive a single message indicating:

```
	index=1 & ofTotal = 1		i.e. 1 of 1 messages
	isCurrent = True | False	True indicating success
	pRegDomain					Contains the name of the region
```

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The request has been accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- The request to the PIC32MZW was rejected.


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_RegDomainGet
(
	DRV_HANDLE handle,
	const WDRV_PIC32MZW_REGDOMAIN_SELECT selection,
	const WDRV_PIC32MZW_REGDOMAIN_CALLBACK pfRegDomCallback
)
```

#### Description:

Requests either the name of the currently active regulatory domain or the names of all regulatory domains programmed into the module.

#### Preconditions:

```WDRV_PIC32MZW_Initialize``` should have been called.

```WDRV_PIC32MZW_Open``` should have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|```handle```		|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|```selection```	|	Type of regulatory domain information to retrieve: - WDRV_PIC32MZW_REGDOMAIN_SELECT_CURRENT - WDRV_PIC32MZW_REGDOMAIN_SELECT_ALL |
| ```pfRegDomCallback```	|	Pointer to callback function to receive the requested regulatory domain information. A pointer to a function of the following prototype: ```bool func(DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, bool isCurrent, const char * pRegDomain )```|
|


When called in response to a GET request, the callback will executed multiple times (once for each region) indicating:

```
	index=x & ofTotal = y		i.e. x of y messages
	isCurrent = True | False	True indicating region is active
	pRegDomain					Contains the name of the region
```

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK		- The request has been accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN	- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG	- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- The request to the PIC32MZW was rejected.

