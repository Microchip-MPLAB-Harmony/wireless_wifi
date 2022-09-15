# WDRV_PIC32MZW_SEC_MASK Enum

## C

```c
typedef enum
{
    /* AP supports WEP-40 or WEP-104, as standalone or as GTK with 11i TSN. */
    WDRV_PIC32MZW_SEC_BIT_WEP = 0x01,
    
    /* AP supports TKIP as GTK (if not WEP) and PTK. */
    WDRV_PIC32MZW_SEC_BIT_WPA = 0x02,
    
    /* AP includes RSNE and supports CCMP-128 as GTK (if not TKIP or WEP) and PTK. */
    WDRV_PIC32MZW_SEC_BIT_WPA2OR3 = 0x04,
    
    /* AP supports BIP-CMAC-128 as IGTK. */
    WDRV_PIC32MZW_SEC_BIT_MFP_CAPABLE = 0x08,
    
    /* AP requires management frame protection. */
    WDRV_PIC32MZW_SEC_BIT_MFP_REQUIRED = 0x10,
    
    /* AP supports 802.1x authentication. */
    WDRV_PIC32MZW_SEC_BIT_ENTERPRISE = 0x20,
    
    /* AP supports PSK authentication. */
    WDRV_PIC32MZW_SEC_BIT_PSK = 0x40,
    
    /* AP supports SAE authentication. */
    WDRV_PIC32MZW_SEC_BIT_SAE = 0x80
} WDRV_PIC32MZW_SEC_MASK;

```

## Summary

List of possible security capabilities.  

## Description

The security capabilities that may be offered by an AP.

## Remarks

None. 

