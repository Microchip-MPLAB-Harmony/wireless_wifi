# WDRV_PIC32MZW_BSS_CONTEXT Struct

## C

```c
typedef struct
{
    /* SSID of BSS. */
    WDRV_PIC32MZW_SSID ssid;
    
    /* BSSID. */
    WDRV_PIC32MZW_MAC_ADDR bssid;
    
    /* Current channel of BSS. */
    WDRV_PIC32MZW_CHANNEL_ID channel;
    
    /* Flag indicating if SSID is cloaked. */
    bool cloaked;
} WDRV_PIC32MZW_BSS_CONTEXT;

```
## Summary

Context structure containing information about BSSs.  

## Description

The context contains the SSID, channel and visibility of a BSS.

## Remarks

None. 


