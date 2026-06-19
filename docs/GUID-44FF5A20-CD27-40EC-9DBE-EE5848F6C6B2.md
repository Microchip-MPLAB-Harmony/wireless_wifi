# WDRV_WINC_IPDHCPServerConfigure Function

## C

```c
WDRV_WINC_STATUS WDRV_WINC_IPDHCPServerConfigure
(
DRV_HANDLE handle,
uint32_t ipAddress,
uint32_t netMask,
WDRV_WINC_DHCP_ADDRESS_EVENT_HANDLER const pfDHCPAddressEventCallback
)
```

## Summary

Configures the Soft-AP DHCP server.  

## Description

Configures the IPv4 address and callback for the DHCP server.

## Precondition

WDRV_WINC_Initialize should have been called. WDRV_WINC_Open should have been called to obtain a valid handle. The WINC must not be connected.  

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_WINC_Open. |
| ipAddress | IPv4 address. |
| netMask | IPv4 netmask. |
| pfDHCPAddressEventCallback | Callback for DHCP address events.  

## Returns

*WDRV_WINC_STATUS_OK* - The request has been accepted.

*WDRV_WINC_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_WINC_STATUS_INVALID_ARG* - The parameters were incorrect.
 

## Remarks

Only applicable when connected in Soft-AP mode. Parameter netMask is only applicable for WINC drivers supporting the extended Soft-AP API m2m_wifi_enable_ap_ext and m2m_wifi_start_provision_mode_ext.  


