# WDRV_PIC32MZW_PS_NOTIFY_CALLBACK Typedef

## C

```c
typedef void (*WDRV_PIC32MZW_PS_NOTIFY_CALLBACK)

```
## Summary

Callback to signal sleep entry of SMC(WSM/WDS) and exit of powersave cycle.  

## Description

After WiFi power-save mode is set by the user, the driver will use this
callback to provide notification on each sleep entry of power-save
sleep-wakeup-sleep cycle and exit notification of power-save cycle on error
OR on user trigger to Run mode.

## Parameters

| Param | Description |
|:----- |:----------- |
| handle | Client handle obtained by a call to WDRV_PIC32MZW_Open. |
| psMode | Current power-save mode. |
| bSleepEntry | TRUE on sleep entry. FALSE on power-save cycle exit. |
| u32SleepDurationMs | Duration of sleep configured for SMC(WSM/WDS).  

## Remarks

The user can take necessary action on sleep entry For ex, configure RTCC and put PIC to sleep/idle. 


