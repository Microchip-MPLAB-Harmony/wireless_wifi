static CRYPT_RNG_CTX wdrvRngCtx;
static const WDRV_PIC32MZW_SYS_INIT wdrvPIC32MZW1InitData = {
    .pCryptRngCtx = &wdrvRngCtx,
	.pRegDomName  = "${DRV_WIFI_PIC32MZW1_REG_DOMAIN}"
};