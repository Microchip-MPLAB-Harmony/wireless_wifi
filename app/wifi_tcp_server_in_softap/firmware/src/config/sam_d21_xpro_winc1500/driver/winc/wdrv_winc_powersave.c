/*******************************************************************************
  WINC Driver Power Save Implementation

  File Name:
    wdrv_winc_powersave.c

  Summary:
    WINC wireless driver power save implementation.

  Description:
    Provides an interface to control the power states of the WINC.
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>

#include "wdrv_winc.h"
#include "wdrv_winc_powersave.h"
#include "wdrv_winc_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Power Save Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_PowerSaveSetMode
    (
        DRV_HANDLE handle,
        WDRV_WINC_PS_MODE mode
    )

  Summary:
    Sets the power save mode.

  Description:
    Selects the current power save mode the WINC should be using.

  Remarks:
    See wdrv_winc_powersave.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_PowerSaveSetMode
(
    DRV_HANDLE handle,
    WDRV_WINC_PS_MODE mode
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

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

    /* Set the sleep mode. */
    if (M2M_SUCCESS != m2m_wifi_set_sleep_mode(mode, pDcpt->powerSaveDTIMInterval ? 0 : 1))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_PS_MODE WDRV_WINC_PowerSaveGetMode(DRV_HANDLE handle)

  Summary:
    Returns the current power save mode.

  Description:
    Returns to the caller the current power save mode in use by the WINC.

  Remarks:
    See wdrv_winc_powersave.h for usage information.

*/

WDRV_WINC_PS_MODE WDRV_WINC_PowerSaveGetMode(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return WDRV_WINC_PS_MODE_INVALID;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return WDRV_WINC_PS_MODE_INVALID;
    }

    /* Retrieve the sleep mode. */
    return m2m_wifi_get_sleep_mode();
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_PowerSaveSetBeaconInterval
    (
        DRV_HANDLE handle,
        uint16_t numBeaconIntervals
    )

  Summary:
    Configures the beacon listening interval.

  Description:
    The beacon listening interval specifies how many beacon period will occur
      between listening actions when the WINC is in low power mode.

  Remarks:
    See wdrv_winc_powersave.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_PowerSaveSetBeaconInterval
(
    DRV_HANDLE handle,
    uint16_t numBeaconIntervals
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;
    tstrM2mLsnInt listenIntervalOpt;

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

    /* Set the listen period. */
    listenIntervalOpt.u16LsnInt = numBeaconIntervals;

    if (M2M_SUCCESS != m2m_wifi_set_lsn_int(&listenIntervalOpt))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    pDcpt->powerSaveDTIMInterval = numBeaconIntervals;

    return WDRV_WINC_STATUS_OK;
}

#ifdef WDRV_WINC_DEVICE_WINC1500
//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_PowerSaveManualSleep
    (
        DRV_HANDLE handle,
        uint32_t sleepTime
    )

  Summary:
    Manual sleep power save operation.

  Description:
    If the current power save mode is WDRV_WINC_PS_MODE_MANUAL the WINC
      can be placed into low power mode by requesting sleep.

  Remarks:
    See wdrv_winc_powersave.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_PowerSaveManualSleep
(
    DRV_HANDLE handle,
    uint32_t sleepTime
)
{
    WDRV_WINC_DCPT *pDcpt = (WDRV_WINC_DCPT *)handle;

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

    /* Request sleep. */
    if (M2M_SUCCESS != m2m_wifi_request_sleep(sleepTime))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    return WDRV_WINC_STATUS_OK;
}
#endif
