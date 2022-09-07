<#if APP_ENABLE_SEC_CONN>
static const WDRV_WINC_SPI_CFG appWincStubSpiInitData =
{
<#if drv_spi?? && APP_SPI_INST_IDX gte 0>
    <#if -1 < APP_SPI_INST_IDX>
    .drvIndex           = DRV_SPI_INDEX_${APP_SPI_INST_IDX}
    </#if>
<#else>
    <#if APP_TX_RX_DMA == true>
    .txDMAChannel       = SYS_DMA_CHANNEL_${APP_TX_DMA_CHANNEL},
    .rxDMAChannel       = SYS_DMA_CHANNEL_${APP_RX_DMA_CHANNEL},
    .txAddress          = (void *)${.vars["${APP_PLIB?lower_case}"].TRANSMIT_DATA_REGISTER},
    .rxAddress          = (void *)${.vars["${APP_PLIB?lower_case}"].RECEIVE_DATA_REGISTER},
    <#else>
    .writeRead          = (WDRV_WINC_SPI_PLIB_WRITE_READ)${.vars["${APP_PLIB?lower_case}"].SPI_PLIB_API_PREFIX}_WriteRead,
    .callbackRegister   = (WDRV_WINC_SPI_PLIB_CALLBACK_REGISTER)${.vars["${APP_PLIB?lower_case}"].SPI_PLIB_API_PREFIX}_CallbackRegister,
    </#if>
</#if>
};

static const WDRV_WINC_SYS_INIT appWincStubInitData = {
    .pSPICfg    = &appWincStubSpiInitData,
<#if APP_INT_SRC == "EIC">
    .intSrc     = EIC_PIN_${APP_EIC_SRC_SELECT}
<#elseif APP_INT_SRC == "GPIO">
    .intSrc     = ${APP_GPIO_SRC_SELECT}
<#elseif APP_INT_SRC == "PIO">
    .intSrc     = ${APP_PIO_SRC_SELECT}
</#if>
};

const WDRV_WINC_SYS_INIT *appWincInitData[2] =
{
    &wdrvWincInitData,
    &appWincStubInitData
};
</#if>