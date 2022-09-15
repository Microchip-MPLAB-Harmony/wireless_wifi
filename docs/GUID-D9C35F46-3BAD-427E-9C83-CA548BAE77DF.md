# WDRV_PIC32MZW_AssocTransitionDisableGet Function

## C

```c
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_AssocTransitionDisableGet
(
WDRV_PIC32MZW_ASSOC_HANDLE assocHandle,
bool *const pTransitionDisable
)
```

## Summary

Determine whether transition algorithms should be disabled for this network.  

## Description

Determines whether a WPA3 transition disable element was sent by the device
(if the device is in AP mode) or received from the peer AP (if the device
is in STA mode) during the 4-way handshake of the current association.

## Precondition

WDRV_PIC32MZW_Initialize should have been called. WDRV_PIC32MZW_Open should have been called to obtain a valid handle. A peer device needs to be connected and associated.  

## Parameters

| Param | Description |
|:----- |:----------- |
| assocHandle | Association handle. |
| pTransitionDisable | Pointer to variable to receive the transition disable status (true if transition algorithms should be disabled, false otherwise).  

## Returns

*WDRV_PIC32MZW_STATUS_OK* - pPeerAddress will contain the network address.

*WDRV_PIC32MZW_STATUS_NOT_OPEN* - The driver instance is not open.

*WDRV_PIC32MZW_STATUS_INVALID_ARG* - The parameters were incorrect.

*WDRV_PIC32MZW_STATUS_REQUEST_ERROR* - The request to the PIC32MZW was rejected
or there is no current association. 
*WDRV_PIC32MZW_STATUS_NOT_CONNECTED* - Not currently connected.
 

## Remarks

In STA mode, if the output is true, then the application should set the WDRV_PIC32MZW_AUTH_MOD_STA_TD modifier in the Auth context for all subsequent connection requests to this ESS. See WDRV_PIC32MZW_AuthCtxStaTransitionDisable.  


