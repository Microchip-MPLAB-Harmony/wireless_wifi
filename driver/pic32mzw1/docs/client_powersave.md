---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: 802.11 Powersave API
has_toc: false
nav_order: 2
---

# 802.11 Powersave API


```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_PowerSaveModeSet
(
        DRV_HANDLE handle,
        const WDRV_PIC32MZW_POWERSAVE_CONFIG *const pPowerSaveCfg
)
```

#### Description:

Requests that the current WiFi powersave mode is changed to the one specified according to config parameters.

In case of WDRV_PIC32MZW_POWERSAVE_PIC_ASYNC_MODE(only mode supported),

- Call to ```WDRV_PIC32MZW_PowerSaveModeSet``` with powerSaveMode set to WSM will put WiFi into respective powersave/sleep mode.
- Call to ```WDRV_PIC32MZW_PowerSaveModeSet``` with powerSaveMode set to RUN to exit from powersave/sleep mode.
- Call to ```WDRV_PIC32MZW_PowerSaveModeSet``` with ```BcastTrafficEnabled``` set to TRUE, will make the device wakeup each DTIM interval to receive buffered WiFi data from AP, if any. Make sure to set the DTIM interval value to greater than 1 in AP for the powersave to work in this scenario.
- Call to ```WDRV_PIC32MZW_PowerSaveModeSet``` with ```BcastTrafficEnabled``` set to FALSE, will wakeup the device each Listen Interval(configured via ```DRV_PIC32MZW_PS_LISTEN_INTERVAL```) instead of   DTIM interval to receive buffered WiFi data from AP, if any.

#### Preconditions:

- ```WDRV_PIC32MZW_Initialize``` should have been called.
- ```WDRV_PIC32MZW_Open``` should have been called to obtain a valid handle.
- Operation mode of the device should be STA.

#### Parameters:

|	Parameter			 | 						Description						|
|-------------------------|--------------------------------------------------------------|
|```handle```		|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|```pPowerSaveCfg ```	|	Pointer to power save configuration.|




#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The request has been accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- The request to the PIC32MZW was rejected.
######    WDRV_PIC32MZW_STATUS_OK              - The request has been accepted.
######    WDRV_PIC32MZW_STATUS_NOT_OPEN        - The driver instance is not open.
######    WDRV_PIC32MZW_STATUS_INVALID_ARG     - The parameters were incorrect.
######    WDRV_PIC32MZW_STATUS_REQUEST_ERROR   - The request to the PIC32MZW was rejected.
######    WDRV_PIC32MZW_STATUS_NOT_CONNECTED	 - Not currently connected to an AP
######	WDRV_PIC32MZW_STATUS_OPERATION_NOT_SUPPORTED -Request operation is not supported.
