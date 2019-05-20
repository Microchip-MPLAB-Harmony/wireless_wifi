/*******************************************************************************
  WINC Wireless Driver

  File Name:
    wdrv_winc_wps.c

  Summary:
    WINC wireless driver.

  Description:
    WINC wireless driver.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

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

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_wps.h"

static bool _ValidateChecksum(uint32_t pin)
{
    uint32_t accum = 0;

    accum += 3 * ((pin / 10000000) % 10);
    accum += 1 * ((pin / 1000000) % 10);
    accum += 3 * ((pin / 100000) % 10);
    accum += 1 * ((pin / 10000) % 10);
    accum += 3 * ((pin / 1000) % 10);
    accum += 1 * ((pin / 100) % 10);
    accum += 3 * ((pin / 10) % 10);
    accum += 1 * ((pin / 1) % 10);

    return (0 == (accum % 10));
}

/****************************************************************************
 * Function:        WDRV_WINC_WPSEnrolleeDiscoveryStartPIN
 * Summary: Start WPS discovery (PIN).
 *****************************************************************************/
WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStartPIN(DRV_HANDLE handle, uint32_t pin, const WDRV_WINC_WPS_DISC_CALLBACK pfWPSDiscoveryCallback)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const )handle;
    char pinStr[9];

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (false == _ValidateChecksum(pin))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    sprintf(pinStr, "%08" PRIu32, pin);

    pDcpt->pfWPSDiscoveryCB = pfWPSDiscoveryCallback;

    if (M2M_SUCCESS != m2m_wifi_wps(WPS_PIN_TRIGGER, pinStr))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

/****************************************************************************
 * Function:        WDRV_WINC_WPSEnrolleeDiscoveryStartPBC
 * Summary: Start WPS discovery (Push Button).
 *****************************************************************************/
WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStartPBC(DRV_HANDLE handle, const WDRV_WINC_WPS_DISC_CALLBACK pfWPSDiscoveryCallback)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const )handle;

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    pDcpt->pfWPSDiscoveryCB = pfWPSDiscoveryCallback;

    if (M2M_SUCCESS != m2m_wifi_wps(WPS_PBC_TRIGGER, NULL))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

/****************************************************************************
 * Function:        WDRV_WINC_WPSEnrolleeDiscoveryStop
 * Summary: Stop WPS discovery.
 *****************************************************************************/
WDRV_WINC_STATUS WDRV_WINC_WPSEnrolleeDiscoveryStop(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const )handle;

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_STATUS_NOT_OPEN;
    }

    if (M2M_SUCCESS != m2m_wifi_wps_disable())
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//DOM-IGNORE-END
