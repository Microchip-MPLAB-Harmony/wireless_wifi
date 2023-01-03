<#if APP_ENABLE_SEC_CONN>
    <#lt>#define APP_ENABLE_SEC_CONN
    <#if APP_SPI_INST?has_content>
        <#assign SPI_PLIB_NAME = "${APP_SPI_INST?eval.DRV_SPI_PLIB}">
        <#lt>#define APP_SPI_PLIB_NAME             ${SPI_PLIB_NAME}
        <#if SPI_PLIB_NAME?lower_case?eval.FLEXCOM_SPI_NUM_CSR?? && SPI_PLIB_NAME?lower_case?eval.FLEXCOM_SPI_NUM_CSR != 1>
            <#lt>#define APP_SPI_PLIB_FLEXCOM_CS_FUNC  ${SPI_PLIB_NAME}_SPI_ChipSelectSetup
        </#if>
    </#if>
</#if>