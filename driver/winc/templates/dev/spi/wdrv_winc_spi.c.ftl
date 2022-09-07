/*******************************************************************************
  WINC Wireless Driver SPI Communication Support

  File Name:
    wdrv_winc_spi.c

  Summary:
    WINC Wireless Driver SPI Communications Support

  Description:
    Supports SPI communications to the WINC module.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019-22 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "configuration.h"
#include "definitions.h"
#include "osal/osal.h"
<#if drv_spi?? && DRV_WIFI_WINC_SPI_INST_IDX gte 0 >
#include "driver/spi/drv_spi.h"
</#if>
#include "wdrv_winc_common.h"
#include "wdrv_winc_spi.h"

<#if DRV_WIFI_WINC_TX_RX_DMA == true || DRV_WIFI_WINC_SPI_INST_IDX gte 0 && core.DMA_ENABLE?has_content && drv_spi?? && drv_spi.DRV_SPI_SYS_DMA_ENABLE == true>
#ifdef USE_CACHE_MAINTENANCE
/* Cache Management to be enabled in core & system components of MHC Project Graph*/
#include "system/cache/sys_cache.h"
#include "sys/kmem.h"
#endif

</#if>
// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

<#if DRV_WIFI_WINC_TX_RX_DMA == true || DRV_WIFI_WINC_SPI_INST_IDX gte 0 && core.DMA_ENABLE?has_content && drv_spi?? && drv_spi.DRV_SPI_SYS_DMA_ENABLE == true>
#if defined(__PIC32MZ__) && defined(USE_CACHE_MAINTENANCE)
#define SPI_DMA_DCACHE_CLEAN(addr, size) _DataCacheClean(addr, size)
#else
#define SPI_DMA_DCACHE_CLEAN(addr, size) do { } while (0)
#endif

</#if>
typedef struct
{
    /* This is the SPI configuration. */
    WDRV_WINC_SPI_CFG       cfg;
<#if DRV_WIFI_WINC_TX_RX_DMA == false>
<#if drv_spi?? && DRV_WIFI_WINC_SPI_INST_IDX gte 0 >
    DRV_HANDLE              spiHandle;
    DRV_SPI_TRANSFER_HANDLE transferTxHandle;
    DRV_SPI_TRANSFER_HANDLE transferRxHandle;
</#if>
    OSAL_SEM_HANDLE_TYPE    txSyncSem;
    OSAL_SEM_HANDLE_TYPE    rxSyncSem;
</#if>
} WDRV_WINC_SPIDCPT;

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

static WDRV_WINC_SPIDCPT spiDcpt;
<#if DRV_WIFI_WINC_TX_RX_DMA == true>
static CACHE_ALIGN uint8_t dummyData[CACHE_ALIGNED_SIZE_GET(4)];
</#if>

<#if DRV_WIFI_WINC_TX_RX_DMA == true || DRV_WIFI_WINC_SPI_INST_IDX gte 0 && core.DMA_ENABLE?has_content && drv_spi?? && drv_spi.DRV_SPI_SYS_DMA_ENABLE == true>
#if defined(__PIC32MZ__) && defined(USE_CACHE_MAINTENANCE)
/****************************************************************************
 * Function:        _DataCacheClean
 * Summary: Used in Cache management to clean cache based on address.
 * Cache Management to be enabled in core & system components of MHC.
 *****************************************************************************/
static void _DataCacheClean(unsigned char *address, uint32_t size)
{
    if (IS_KVA0(address))
    {
        uint32_t a = (uint32_t)address & 0xfffffff0;
        uint32_t r = (uint32_t)address & 0x0000000f;
        uint32_t s = ((size + r + 15) >> 4) << 4;

        SYS_CACHE_CleanDCache_by_Addr((uint32_t *)a, s);
    }
}
#endif

</#if>
// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

<#if DRV_WIFI_WINC_TX_RX_DMA == true>
<#elseif drv_spi?? && DRV_WIFI_WINC_SPI_INST_IDX gte 0 >
static void _WDRV_WINC_SPITransferEventHandler(DRV_SPI_TRANSFER_EVENT event,
        DRV_SPI_TRANSFER_HANDLE handle, uintptr_t context)
{
    switch(event)
    {
        case DRV_SPI_TRANSFER_EVENT_COMPLETE:
            // This means the data was transferred.
            if (spiDcpt.transferTxHandle == handle)
            {
                OSAL_SEM_PostISR(&spiDcpt.txSyncSem);
            }
            else if (spiDcpt.transferRxHandle == handle)
            {
                OSAL_SEM_PostISR(&spiDcpt.rxSyncSem);
            }

            break;

        case DRV_SPI_TRANSFER_EVENT_ERROR:
            // Error handling here.
            break;

        default:
            break;
    }
}

<#else>
static void _DRV_SPI_PlibCallbackHandler(uintptr_t contextHandle)
{
    OSAL_SEM_PostISR((OSAL_SEM_HANDLE_TYPE*)contextHandle);
}

</#if>
//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_SPISend(void* pTransmitData, size_t txSize)

  Summary:
    Sends data out to the module through the SPI bus.

  Description:
    This function sends data out to the module through the SPI bus.

  Remarks:
    See wdrv_winc_spi.h for usage information.
 */

bool WDRV_WINC_SPISend(void* pTransmitData, size_t txSize)
{
<#if DRV_WIFI_WINC_TX_RX_DMA == true || DRV_WIFI_WINC_SPI_INST_IDX gte 0 && core.DMA_ENABLE?has_content && drv_spi?? && drv_spi.DRV_SPI_SYS_DMA_ENABLE == true>
    SPI_DMA_DCACHE_CLEAN(pTransmitData, txSize);
</#if>
<#if DRV_WIFI_WINC_TX_RX_DMA == true>

    /* Configure the RX DMA channel - to receive dummy data */
    SYS_DMA_AddressingModeSetup(spiDcpt.cfg.rxDMAChannel, SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED, SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED);
    SYS_DMA_ChannelTransfer(spiDcpt.cfg.rxDMAChannel, (const void*)spiDcpt.cfg.rxAddress, (const void *)dummyData, txSize);

    /* Configure the transmit DMA channel - to send data from transmit buffer */
    SYS_DMA_AddressingModeSetup(spiDcpt.cfg.txDMAChannel, SYS_DMA_SOURCE_ADDRESSING_MODE_INCREMENTED, SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED);
    SYS_DMA_ChannelTransfer(spiDcpt.cfg.txDMAChannel, pTransmitData, (const void*)spiDcpt.cfg.txAddress, txSize);

    while (true == SYS_DMA_ChannelIsBusy(spiDcpt.cfg.rxDMAChannel))
    {
    }
<#elseif drv_spi?? && DRV_WIFI_WINC_SPI_INST_IDX gte 0>

    DRV_SPI_WriteTransferAdd(spiDcpt.spiHandle, pTransmitData, txSize, &spiDcpt.transferTxHandle);

    if (DRV_SPI_TRANSFER_HANDLE_INVALID == spiDcpt.transferTxHandle)
    {
        return false;
    }

    while (OSAL_RESULT_FALSE == OSAL_SEM_Pend(&spiDcpt.txSyncSem, OSAL_WAIT_FOREVER))
    {
    }
<#else>
    if ((NULL == spiDcpt.cfg.callbackRegister) || (NULL == spiDcpt.cfg.writeRead))
    {
        return false;
    }

    spiDcpt.cfg.callbackRegister(_DRV_SPI_PlibCallbackHandler, (uintptr_t)&spiDcpt.txSyncSem);
    spiDcpt.cfg.writeRead(pTransmitData, txSize, NULL, 0);

    while (OSAL_RESULT_FALSE == OSAL_SEM_Pend(&spiDcpt.txSyncSem, OSAL_WAIT_FOREVER))
    {
    }
</#if>

    return true;
}

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_SPIReceive(void* pReceiveData, size_t rxSize)

  Summary:
    Receives data from the module through the SPI bus.

  Description:
    This function receives data from the module through the SPI bus.

  Remarks:
    See wdrv_winc_spi.h for usage information.
 */

bool WDRV_WINC_SPIReceive(void* pReceiveData, size_t rxSize)
{
<#if DRV_WIFI_WINC_TX_RX_DMA == true>
    SPI_DMA_DCACHE_CLEAN(buf, size);

    /* Configure the RX DMA channel - to receive data in receive buffer */
    SYS_DMA_AddressingModeSetup(spiDcpt.cfg.rxDMAChannel, SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED, SYS_DMA_DESTINATION_ADDRESSING_MODE_INCREMENTED);
    SYS_DMA_ChannelTransfer(spiDcpt.cfg.rxDMAChannel, (const void*)spiDcpt.cfg.rxAddress, pReceiveData, rxSize);

    /* Configure the TX DMA channel - to send dummy data */
    SYS_DMA_AddressingModeSetup(spiDcpt.cfg.txDMAChannel, SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED, SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED);
    SYS_DMA_ChannelTransfer(spiDcpt.cfg.txDMAChannel, (const void *)dummyData, (const void*)spiDcpt.cfg.txAddress, rxSize);

    while (true == SYS_DMA_ChannelIsBusy(spiDcpt.cfg.rxDMAChannel))
    {
    }
<#elseif drv_spi?? && DRV_WIFI_WINC_SPI_INST_IDX gte 0>
    static uint8_t dummy = 0;

<#if core.DMA_ENABLE?has_content && drv_spi?? && drv_spi.DRV_SPI_SYS_DMA_ENABLE == true>
    SPI_DMA_DCACHE_CLEAN(pReceiveData, rxSize);
</#if>

    DRV_SPI_WriteReadTransferAdd(spiDcpt.spiHandle, &dummy, 1, pReceiveData, rxSize, &spiDcpt.transferRxHandle);

    if (DRV_SPI_TRANSFER_HANDLE_INVALID == spiDcpt.transferRxHandle)
    {
        return false;
    }

    while (OSAL_RESULT_FALSE == OSAL_SEM_Pend(&spiDcpt.rxSyncSem, OSAL_WAIT_FOREVER))
    {
    }
<#else>
    static uint8_t dummy = 0;

    if ((NULL == spiDcpt.cfg.callbackRegister) || (NULL == spiDcpt.cfg.writeRead))
    {
        return false;
    }

    spiDcpt.cfg.callbackRegister(_DRV_SPI_PlibCallbackHandler, (uintptr_t)&spiDcpt.rxSyncSem);
    spiDcpt.cfg.writeRead(&dummy, 1, pReceiveData, rxSize);

    while (OSAL_RESULT_FALSE == OSAL_SEM_Pend(&spiDcpt.rxSyncSem, OSAL_WAIT_FOREVER))
    {
    }
</#if>

    return true;
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_SPIOpen(void)

  Summary:
    Opens the SPI object for the WiFi driver.

  Description:
    This function opens the SPI object for the WiFi driver.

  Remarks:
    See wdrv_winc_spi.h for usage information.
 */

void WDRV_WINC_SPIOpen(void)
{
<#if DRV_WIFI_WINC_TX_RX_DMA == true>
    SYS_DMA_DataWidthSetup(spiDcpt.cfg.rxDMAChannel, SYS_DMA_WIDTH_8_BIT);
    SYS_DMA_DataWidthSetup(spiDcpt.cfg.txDMAChannel, SYS_DMA_WIDTH_8_BIT);

    memset(dummyData, 0, sizeof(dummyData));
<#else>
    if (OSAL_RESULT_TRUE != OSAL_SEM_Create(&spiDcpt.txSyncSem, OSAL_SEM_TYPE_COUNTING, 10, 0))
    {
        return;
    }

    if (OSAL_RESULT_TRUE != OSAL_SEM_Create(&spiDcpt.rxSyncSem, OSAL_SEM_TYPE_COUNTING, 10, 0))
    {
        return;
    }
<#if drv_spi?? && DRV_WIFI_WINC_SPI_INST_IDX gte 0>

    if (DRV_HANDLE_INVALID == spiDcpt.spiHandle)
    {
        spiDcpt.spiHandle = DRV_SPI_Open(spiDcpt.cfg.drvIndex, DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_BLOCKING);

        if (DRV_HANDLE_INVALID == spiDcpt.spiHandle)
        {
            WDRV_DBG_ERROR_PRINT("SPI open failed\r\n");
        }
    }

    DRV_SPI_TransferEventHandlerSet(spiDcpt.spiHandle, _WDRV_WINC_SPITransferEventHandler, 0);
</#if>
</#if>
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_SPIInitialize(const WDRV_WINC_SPI_CFG *const pInitData)

  Summary:
    Initializes the SPI object for the WiFi driver.

  Description:
    This function initializes the SPI object for the WiFi driver.

  Remarks:
    See wdrv_winc_spi.h for usage information.
 */

void WDRV_WINC_SPIInitialize(const WDRV_WINC_SPI_CFG *const pInitData)
{
    if (NULL == pInitData)
    {
        return;
    }

    memcpy(&spiDcpt.cfg, pInitData, sizeof(WDRV_WINC_SPI_CFG));
<#if DRV_WIFI_WINC_TX_RX_DMA == false && drv_spi?? && DRV_WIFI_WINC_SPI_INST_IDX gte 0 >

    spiDcpt.spiHandle = DRV_HANDLE_INVALID;
</#if>
}

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_SPIDeinitialize(void)

  Summary:
    Deinitializes the SPI object for the WiFi driver.

  Description:
    This function deinitializes the SPI object for the WiFi driver.

  Remarks:
    See wdrv_winc_spi.h for usage information.
 */

void WDRV_WINC_SPIDeinitialize(void)
{
<#if DRV_WIFI_WINC_TX_RX_DMA == false>
    OSAL_SEM_Post(&spiDcpt.txSyncSem);
    OSAL_SEM_Delete(&spiDcpt.txSyncSem);

    OSAL_SEM_Post(&spiDcpt.rxSyncSem);
    OSAL_SEM_Delete(&spiDcpt.rxSyncSem);
</#if>
}

//DOM-IGNORE-END
