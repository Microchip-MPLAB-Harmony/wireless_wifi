---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client - Custom IE API Implementation
has_toc: false
nav_order: 2
---

<br />
<br />

```
WDRV_PIC32MZW_CUST_IE_STORE_CONTEXT* WDRV_PIC32MZW_CustIEStoreCtxSetStorage
(
    uint8_t *const pStorage,
    uint16_t lenStorage
)
```

#### Description:

The caller provides storage for the custom IE store, this will be initialized
and a pointer provided which can be passed to WDRV_PIC32MZW_APSetCustIE
after custom IEs are added by ```WDRV_PIC32MZW_CustIEStoreCtxAddIE```.

#### Preconditions:

None.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
|```pStorage```		|	Pointer to storage for the custom IE store. |
|```lenStorage```	|	Length of storage pointed to by pStorage. |
|

#### Returns:

###### Pointer to custom IE store, or NULL if error occurs.

#### Remarks

lenStorage should be:
(WDRV_PIC32MZW_CUSTIE_MIN_STORAGE_LEN <= lenStorage
<= WDRV_PIC32MZW_CUSTIE_MAX_STORAGE_LEN + WDRV_PIC32MZW_CUSTIE_DATA_OFFSET).
If less than WDRV_PIC32MZW_CUSTIE_MIN_STORAGE_LEN an error will be signalled,
if more than WDRV_PIC32MZW_CUSTIE_MAX_STORAGE_LEN + WDRV_PIC32MZW_CUSTIE_DATA_OFFSET
then WDRV_PIC32MZW_CUSTIE_MAX_STORAGE_LEN + WDRV_PIC32MZW_CUSTIE_DATA_OFFSET
bytes will be used.

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_CustIEStoreCtxAddIE
(
        WDRV_PIC32MZW_CUST_IE_STORE_CONTEXT *const pCustIECtx,
        uint8_t id,
        const uint8_t *const pData,
        uint8_t dataLength
)
```

#### Description:

The data and ID provided are copied into the custom IE store.

#### Preconditions:

```WDRV_PIC32MZW_CustIEStoreCtxSetStorage``` must have been called.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
|```pCustIECtx```		|	Pointer to custom IE store. |
|```id```	|	ID of custom IE. |
|```pData```	|	Pointer to data to be stored in the custom IE. |
|```dataLength```	|  Length of data pointed to by pData. |
|

#### Returns:

######  WDRV_PIC32MZW_STATUS_OK          - The data was added successfully.
######  WDRV_PIC32MZW_STATUS_INVALID_ARG - The parameters were incorrect.
######  WDRV_PIC32MZW_STATUS_NO_SPACE    - The data will not fit into the custom IE store.


#### Remarks

None
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_CustIEStoreCtxRemoveIE
(
        WDRV_PIC32MZW_CUST_IE_STORE_CONTEXT *const pCustIECtx,
        uint8_t id
)
```

#### Description:

This function removes a custom IE from the store which matches the ID provided.

#### Preconditions:

```WDRV_PIC32MZW_CustIEStoreCtxSetStorage``` must have been called.

#### Parameters:

|	Parameter			 | 						Description								|
|-------------------------|--------------------------------------------------------------|
|```pCustIECtx```		|	Pointer to custom IE store. |
|```id```	|	ID of custom IE to remove. |
|

#### Returns:

######  WDRV_PIC32MZW_STATUS_OK          - The data was added successfully.
######  WDRV_PIC32MZW_STATUS_INVALID_ARG - The parameters were incorrect.

#### Remarks

None