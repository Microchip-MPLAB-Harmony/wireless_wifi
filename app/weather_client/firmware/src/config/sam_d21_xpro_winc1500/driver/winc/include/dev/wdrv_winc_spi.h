/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_spi.h

  Summary:
    WINC wireless driver SPI APIs.

  Description:
    Provides interface for using the SPI bus.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

#ifndef _WDRV_WINC_SPI_H
#define _WDRV_WINC_SPI_H

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_SPISend(unsigned char const *buf, uint32_t size)

  Summary:
    Sends data out to the module through the SPI bus.

  Description:
    This function sends data out to the module through the SPI bus.

  Precondition:
    WDRV_WINC_SPIInitialize must have been called.

  Parameters:
    buf  - buffer pointer of output data
    size - the output data size

  Returns:
    true  - Indicates success
    false - Indicates failure

  Remarks:
    None.
 */
bool WDRV_WINC_SPISend(unsigned char *const buf, uint32_t size);

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_SPIReceive(unsigned char *const buf, uint32_t size)

  Summary:
    Receives data from the module through the SPI bus.

  Description:
    This function receives data from the module through the SPI bus.

  Precondition:
    WDRV_WINC_SPIInitialize must have been called.

  Parameters:
    buf  - buffer pointer of input data
    size - the input data size

  Returns:
    true  - Indicates success
    false - Indicates failure

  Remarks:
    None.
 */
bool WDRV_WINC_SPIReceive(unsigned char *const buf, uint32_t size);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_SPIInitialize(void)

  Summary:
    Initializes the SPI object for the WiFi driver.

  Description:
    This function initializes the SPI object for the WiFi driver.

  Precondition:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_SPIInitialize(void);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_SPIDeinitialize(void)

  Summary:
    Deinitializes the SPI object for the WiFi driver.

  Description:
    This function deinitializes the SPI object for the WiFi driver.

  Precondition:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WINC_SPIDeinitialize(void);

#endif /* _WDRV_WINC_SPI_H */
