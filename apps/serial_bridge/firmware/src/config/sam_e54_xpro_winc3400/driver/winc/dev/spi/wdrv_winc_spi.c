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
/*
Copyright (C) 2019-22, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

#include "configuration.h"
#include "definitions.h"
#include "osal/osal.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_spi.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

typedef struct
{
    /* This is the SPI configuration. */
    WDRV_WINC_SPI_CFG       cfg;
} WDRV_WINC_SPIDCPT;

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

static WDRV_WINC_SPIDCPT spiDcpt;
static CACHE_ALIGN uint8_t dummyDataTx[CACHE_ALIGNED_SIZE_GET(4)];
static CACHE_ALIGN uint8_t dummyDataRx[CACHE_ALIGNED_SIZE_GET(4)];

// *****************************************************************************
// *****************************************************************************
// Section: File scope functions
// *****************************************************************************
// *****************************************************************************

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

    /* Configure the RX DMA channel - to receive dummy data */
    SYS_DMA_AddressingModeSetup(spiDcpt.cfg.rxDMAChannel, SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED, SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED);
    SYS_DMA_ChannelTransfer(spiDcpt.cfg.rxDMAChannel, (const void*)spiDcpt.cfg.rxAddress, (const void *)dummyDataRx, txSize);

    /* Configure the transmit DMA channel - to send data from transmit buffer */
    SYS_DMA_AddressingModeSetup(spiDcpt.cfg.txDMAChannel, SYS_DMA_SOURCE_ADDRESSING_MODE_INCREMENTED, SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED);
    SYS_DMA_ChannelTransfer(spiDcpt.cfg.txDMAChannel, pTransmitData, (const void*)spiDcpt.cfg.txAddress, txSize);

    while (true == SYS_DMA_ChannelIsBusy(spiDcpt.cfg.rxDMAChannel))
    {
    }

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
    /* Configure the RX DMA channel - to receive data in receive buffer */
    SYS_DMA_AddressingModeSetup(spiDcpt.cfg.rxDMAChannel, SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED, SYS_DMA_DESTINATION_ADDRESSING_MODE_INCREMENTED);
    SYS_DMA_ChannelTransfer(spiDcpt.cfg.rxDMAChannel, (const void*)spiDcpt.cfg.rxAddress, pReceiveData, rxSize);

    /* Configure the TX DMA channel - to send dummy data */
    SYS_DMA_AddressingModeSetup(spiDcpt.cfg.txDMAChannel, SYS_DMA_SOURCE_ADDRESSING_MODE_FIXED, SYS_DMA_DESTINATION_ADDRESSING_MODE_FIXED);
    SYS_DMA_ChannelTransfer(spiDcpt.cfg.txDMAChannel, (const void *)dummyDataTx, (const void*)spiDcpt.cfg.txAddress, rxSize);

    while (true == SYS_DMA_ChannelIsBusy(spiDcpt.cfg.rxDMAChannel))
    {
    }

    return true;
}

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_SPIOpen(void)

  Summary:
    Opens the SPI object for the WiFi driver.

  Description:
    This function opens the SPI object for the WiFi driver.

  Remarks:
    See wdrv_winc_spi.h for usage information.
 */

bool WDRV_WINC_SPIOpen(void)
{
    SYS_DMA_DataWidthSetup(spiDcpt.cfg.rxDMAChannel, SYS_DMA_WIDTH_8_BIT);
    SYS_DMA_DataWidthSetup(spiDcpt.cfg.txDMAChannel, SYS_DMA_WIDTH_8_BIT);

    memset(dummyDataTx, 0, sizeof(dummyDataTx));

    return true;
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
}

//DOM-IGNORE-END
