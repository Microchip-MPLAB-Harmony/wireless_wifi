/*******************************************************************************
  FLEXCOM4 SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_flexcom4_spi_master.h

  Summary:
   FLEXCOM4 SPI Master PLIB Header File.

  Description
    This file defines the interface to the FLEXCOM SPI peripheral library.
    This library provides access to and control of the associated
    peripheral instance.

  Remarks:
    None.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

#ifndef PLIB_FLEXCOM4_SPI_MASTER_H // Guards against multiple inclusion
#define PLIB_FLEXCOM4_SPI_MASTER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/* This section lists the other files that are included in this file.
*/

#include "device.h"
#include "plib_flexcom_spi_master_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

extern "C" {

#endif

// DOM-IGNORE-END

/****************************** FLEXCOM4 SPI Interface *********************************/

void FLEXCOM4_SPI_Initialize( void );
bool FLEXCOM4_SPI_WriteRead( void * pTransmitData, size_t txSize, void * pReceiveData, size_t rxSize );
bool FLEXCOM4_SPI_Write( void * pTransmitData, size_t txSize );
bool FLEXCOM4_SPI_Read( void * pReceiveData, size_t rxSize );
bool FLEXCOM4_SPI_TransferSetup( FLEXCOM_SPI_TRANSFER_SETUP * setup, uint32_t spiSourceClock );
bool FLEXCOM4_SPI_IsTransmitterBusy(void);
void FLEXCOM4_SPI_ChipSelectSetup(FLEXCOM_SPI_CHIP_SELECT chipSelect);
bool FLEXCOM4_SPI_IsBusy( void );
void FLEXCOM4_SPI_CallbackRegister( FLEXCOM_SPI_CALLBACK callback, uintptr_t context );

/* Provide C++ Compatibility */
#ifdef __cplusplus

    }

#endif

#endif // PLIB_FLEXCOM4_SPI_MASTER_H

/*******************************************************************************
 End of File
*/
