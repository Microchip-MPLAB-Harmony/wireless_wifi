static const WDRV_WINC_SPI_CFG wdrvWincSpiInitData =
{
<#if drv_spi?? && DRV_WIFI_WINC_SPI_INST_IDX gte 0>
    <#if -1 < DRV_WIFI_WINC_SPI_INST_IDX>
    <#assign DRV_WIFI_WINC_SPI_PLIB = DRV_WIFI_WINC_SPI_INST?eval.DRV_SPI_PLIB >
    .drvIndex           = DRV_SPI_INDEX_${DRV_WIFI_WINC_SPI_INST_IDX},
    <#if DRV_WIFI_WINC_SPI_PLIB?starts_with("FLEXCOM")>
    .baudRateInHz       = ${.vars["${DRV_WIFI_WINC_SPI_PLIB?lower_case}"].FLEXCOM_SPI_BAUD_RATE},
    <#elseif DRV_WIFI_WINC_SPI_PLIB?starts_with("SERCOM")>
    .baudRateInHz       = ${.vars["${DRV_WIFI_WINC_SPI_PLIB?lower_case}"].SPI_BAUD_RATE},
    </#if>
    .chipSelect         = SYS_PORT_PIN_NONE
    </#if>
<#else>
    <#if DRV_WIFI_WINC_TX_RX_DMA == true>
    .txDMAChannel       = SYS_DMA_CHANNEL_${DRV_WIFI_WINC_TX_DMA_CHANNEL},
    .rxDMAChannel       = SYS_DMA_CHANNEL_${DRV_WIFI_WINC_RX_DMA_CHANNEL},
    .txAddress          = (void *)${.vars["${DRV_WIFI_WINC_PLIB?lower_case}"].TRANSMIT_DATA_REGISTER},
    .rxAddress          = (void *)${.vars["${DRV_WIFI_WINC_PLIB?lower_case}"].RECEIVE_DATA_REGISTER},
    <#else>
    .writeRead          = (WDRV_WINC_SPI_PLIB_WRITE_READ)${.vars["${DRV_WIFI_WINC_PLIB?lower_case}"].SPI_PLIB_API_PREFIX}_WriteRead,
    .callbackRegister   = (WDRV_WINC_SPI_PLIB_CALLBACK_REGISTER)${.vars["${DRV_WIFI_WINC_PLIB?lower_case}"].SPI_PLIB_API_PREFIX}_CallbackRegister,
    </#if>
</#if>
};

static const WDRV_WINC_SYS_INIT wdrvWincInitData = {
    .pSPICfg    = &wdrvWincSpiInitData,
<#if DRV_WIFI_WINC_INT_SRC == "EIC">
    .intSrc     = EIC_PIN_${DRV_WIFI_WINC_EIC_SRC_SELECT}
<#elseif DRV_WIFI_WINC_INT_SRC == "GPIO">
    .intSrc     = ${DRV_WIFI_WINC_GPIO_SRC_SELECT}
<#elseif DRV_WIFI_WINC_INT_SRC == "PIO">
    .intSrc     = ${DRV_WIFI_WINC_PIO_SRC_SELECT}
<#else>
    .intSrc     = -1
</#if>
};
