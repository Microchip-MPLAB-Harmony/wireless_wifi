static CRYPT_RNG_CTX wdrvRngCtx;
static const WDRV_PIC32MZW_SYS_INIT wdrvPIC32MZW1InitData = {
    .pCryptRngCtx  = &wdrvRngCtx,
    .pRegDomName   = "${DRV_WIFI_PIC32MZW1_REG_DOMAIN}",
    .powerSaveMode = WDRV_PIC32MZW_POWERSAVE_RUN_MODE,
    .powerSavePICCorrelation = WDRV_PIC32MZW_POWERSAVE_PIC_ASYNC_MODE
};
