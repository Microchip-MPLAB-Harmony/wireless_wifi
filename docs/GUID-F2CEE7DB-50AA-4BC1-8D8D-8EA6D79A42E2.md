# WDRV_WINC_SSL_CIPHERSUITELIST_CALLBACK Typedef

## C

```c
typedef void (*WDRV_WINC_SSL_CIPHERSUITELIST_CALLBACK)

```

## Summary

Callback to report cipher suites.  

## Description

Called when the cipher suites are changed.

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_WINC_Open. |
| pSSLCipherSuiteCtx | Pointer to cipher suite list.  

## Returns

None.  

## Remarks

None.  


