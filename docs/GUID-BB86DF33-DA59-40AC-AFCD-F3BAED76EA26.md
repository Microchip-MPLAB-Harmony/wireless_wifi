# WDRV_WINC_NVM_REGION Enum

## C

```c
typedef enum
{
    /* Access to the entire flash address space. */
    WDRV_WINC_NVM_REGION_RAW,
    
    /* Access to the firmware region (active image). */
    WDRV_WINC_NVM_REGION_FIRMWARE_ACTIVE,
    
    /* Access to the firmware region (inactive image). */
    WDRV_WINC_NVM_REGION_FIRMWARE_INACTIVE,
    
    /* Access to PLL table region. */
    WDRV_WINC_NVM_REGION_PLL_TABLE,
    
    /* Access to gain table region. */
    WDRV_WINC_NVM_REGION_GAIN_TABLE,
    
    /* Access to PLL and gain tables region. */
    WDRV_WINC_NVM_REGION_PLL_AND_GAIN_TABLES,
    
    /* Access to root certificate region. */
    WDRV_WINC_NVM_REGION_ROOT_CERTS,
    
    /* Access to TLS local certificate region. */
    WDRV_WINC_NVM_REGION_LOCAL_CERTS,
    
    /* Access to connection parameters region. */
    WDRV_WINC_NVM_REGION_CONN_PARAM,
    
    /* Access to HTTP files region. */
    WDRV_WINC_NVM_REGION_HTTP_FILES,
    /* Access to host download region. */
    WDRV_WINC_NVM_REGION_HOST_FILE,
    /* Number of regions. */
    NUM_WDRV_WINC_NVM_REGIONS
} WDRV_WINC_NVM_REGION;

```
## Summary

Defines the available NVM regions.  

## Description

This enumeration defines the possible NVM regions which can be manipulated
using this interface. The WDRV_WINC_NVM_REGION_RAW can be used to access
the entire flash.

## Remarks

None. 


