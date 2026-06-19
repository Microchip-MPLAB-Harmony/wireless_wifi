# WDRV_WINC_HTTPProvCtxSetDomainName Function

## C

```c
WDRV_WINC_STATUS WDRV_WINC_HTTPProvCtxSetDomainName
(
WDRV_WINC_HTTP_PROV_CONTEXT *const pHTTPProvCtx,
const char *const pname
)
```

## Summary

Define the DNS domain name for the provisioning server.  

## Description

This defines the DNS domain name associated with the Soft-AP instance web server.

## Precondition

WDRV_WINC_HTTPProvCtxSetDefaults must have been called.  

## Parameters

| Param | Description |
|:----- |:----------- |
| pHTTPProvCtx | Pointer to provisioning store structure. |
| pname | Pointer to domain name string.  

## Returns

*WDRV_WINC_STATUS_OK* - The domain name is set.

*WDRV_WINC_STATUS_INVALID_ARG* - The parameters were incorrect.
 

## Remarks

None.  


