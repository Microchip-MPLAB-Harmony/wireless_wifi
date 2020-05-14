    /* Initialize the PIC32MZW1 Driver */
    CRYPT_RNG_Initialize(wdrvPIC32MZW1InitData.pCryptRngCtx);
    sysObj.drvWifiPIC32MZW1 = WDRV_PIC32MZW_Initialize(WDRV_PIC32MZW_SYS_IDX_0, (SYS_MODULE_INIT*)&wdrvPIC32MZW1InitData);