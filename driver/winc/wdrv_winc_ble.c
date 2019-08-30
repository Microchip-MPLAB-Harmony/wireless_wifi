/*******************************************************************************
  WINC Driver BLE Implementation

  File Name:
    wdrv_winc_ble.c

  Summary:
    WINC wireless driver BLE implementation.

  Description:
    This interface provides an interface to control BLE.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "wdrv_winc.h"
#include "wdrv_winc_common.h"
#include "wdrv_winc_ble.h"

#include "at_ble_api.h"
#include "platform.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

/* This flag tracks if the BLE has been initialised. */
static bool bleInitialized = false;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BLE Callbacks Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void _WDRV_WINC_BLEWaitCallback(void)

  Summary:
    WINC BLE wait callback.

  Description:
    This function is called by the BLE stack to signal a wait event.

  Precondition:
    WDRV_WINC_Initialize should have been called.
    WDRV_WINC_Open should have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    None.

  Remarks:
    None.

*/

static void _WDRV_WINC_BLEWaitCallback(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return;
    }

    /* Check M2M HIF semaphore and process any events. */
    if (OSAL_RESULT_TRUE == OSAL_SEM_Pend(&pDcpt->isrSemaphore, OSAL_WAIT_FOREVER))
    {
        m2m_wifi_handle_events();
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver BLE Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_BLEIsStarted(DRV_HANDLE handle)

  Summary:
    Determines if BLE has been started.

  Description:
    Determines if BLE has been started.

  Remarks:
    See wdrv_winc_ble.h for usage information.

*/

bool WDRV_WINC_BLEIsStarted(DRV_HANDLE handle)
{
    const WDRV_WINC_DCPT *const pDcpt = (const WDRV_WINC_DCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return false;
    }

    /* Ensure the driver instance has been opened for use. */
    if (false == pDcpt->isOpen)
    {
        return false;
    }

    return pDcpt->bleActive;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BLEStart(DRV_HANDLE handle)

  Summary:
    Starts the BLE interface.

  Description:
    Enables the BLE interface.

  Remarks:
    See wdrv_winc_ble.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BLEStart(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;
    plf_params_t plf_params;
    tstrM2mRev info;

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

    /* Ensure the BLE is not currently active. */
    if (true == pDcpt->bleActive)
    {
        return WDRV_WINC_STATUS_OK;
    }

    if (M2M_SUCCESS != m2m_wifi_get_firmware_version(&info))
    {
        return WDRV_WINC_STATUS_REQUEST_ERROR;
    }

    /* Remove restriction on BLE use. */
    m2m_wifi_req_unrestrict_ble();

    pDcpt->bleActive = true;

    if (false == bleInitialized)
    {
        /* If BLE has never been initialised, set up initialise it. */
        plf_params.drvHandle    = handle;
        plf_params.ble_write_cb = m2m_wifi_ble_api_send;
        plf_params.plf_wait_cb  = _WDRV_WINC_BLEWaitCallback;

        plf_params.fw_ver.major = info.u8FirmwareMajor;
        plf_params.fw_ver.minor = info.u8FirmwareMinor;
        plf_params.fw_ver.patch = info.u8FirmwarePatch;

        if (AT_BLE_SUCCESS != at_ble_init(&plf_params))
        {
            pDcpt->bleActive = false;

            m2m_wifi_req_restrict_ble();

            return WDRV_WINC_STATUS_REQUEST_ERROR;
        }

        /* Mark BLE initialised. */
        bleInitialized = true;
    }

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_BLEStop(DRV_HANDLE handle)

  Summary:
    Stops the BLE interface.

  Description:
    Disables the BLE interface.

  Remarks:
    See wdrv_winc_ble.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_BLEStop(DRV_HANDLE handle)
{
    WDRV_WINC_DCPT *const pDcpt = (WDRV_WINC_DCPT *const)handle;

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

    /* Ensure the BLE is currently active. */
    if (false == pDcpt->bleActive)
    {
        return WDRV_WINC_STATUS_OK;
    }

    pDcpt->bleActive = false;

    /* Restrict BLE use. */
    m2m_wifi_req_restrict_ble();

    return WDRV_WINC_STATUS_OK;
}
