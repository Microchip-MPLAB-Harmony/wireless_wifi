# WDRV_WINC_SSLRetrieveCert Function

## C

```c
WDRV_WINC_STATUS WDRV_WINC_SSLRetrieveCert
(
DRV_HANDLE handle,
uint16_t *pCurveType,
uint8_t *pHash,
uint8_t *pSig,
WDRV_WINC_EC_POINT_REP *pKey
)
```

## Summary

Retrieve the certificate to be verified from the WINC  

## Description

Retrieve the certificate to be verified from the WINC

## Precondition

WDRV_WINC_Initialize should have been called. WDRV_WINC_Open should have been called to obtain a valid handle.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_WINC_Open. |
| pCurveType | Pointer to the certificate curve type. |
| pHash | Pointer to the certificate hash. |
| pSig | Pointer to the certificate signature. |
| pKey | Pointer to the certificate Key.  

## Returns

*WDRV_WINC_STATUS_OK* - The operation was performed.

*WDRV_WINC_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_WINC_STATUS_INVALID_ARG* - The parameters were incorrect.

*WDRV_WINC_STATUS_REQUEST_ERROR* - The operation is not success
 

## Remarks

None.  


