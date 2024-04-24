/*******************************************************************************
  WINC Driver SNTP Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_sntp.h

  Summary:
    WINC wireless driver SNTP header file.

  Description:
    Provides an interface to configure the Network Time (NTP) client on the WINC.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
// DOM-IGNORE-END

#ifndef WDRV_WINC_SNTP_H
#define WDRV_WINC_SNTP_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver SNTP Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPEnableSet
    (
        DRV_HANDLE handle,
        bool enabled
    )

  Summary:
    Set the enable state of the Network Time (NTP) client function.

  Description:
    Enables or disables the Network Time (NTP) client.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    enabled - Flag indicating if the NTP client should be enabled or disabled.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPEnableSet
(
    DRV_HANDLE handle,
    bool enabled
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPStaticSet
    (
        DRV_HANDLE handle,
        bool enabled
    )

  Summary:
    Set the enable state of the NTP configuration mode.

  Description:
    Configures the NTP configuration mode to either DHCP or Static.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle  - Client handle obtained by a call to WDRV_WINC_Open.
    enabled - Flag indicating if the NTP configuration mode should be DHCP or Static.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPStaticSet
(
    DRV_HANDLE handle,
    bool enabled
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_SNTPServerAddressSet
    (
        DRV_HANDLE handle,
        const char *pAddr
    )

  Summary:
    Set the address/name of NTP server.

  Description:
    Configures the address/name to be used as the NTP server.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    pAddr  - Pointer to address/name.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_SNTPServerAddressSet
(
    DRV_HANDLE handle,
    const char *pAddr
);

#endif /* WDRV_WINC_SNTP_H */
