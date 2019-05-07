/*******************************************************************************
  WINC MAC Driver TCP/IP Interface Implementation

  File Name:
    wdrv_winc_ctrl_itf.c

  Summary:
    Wireless driver PIC32 implementation for WINC support.

  Description:
    Wireless driver PIC32 implementation for WINC support.
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

#include "wdrv_winc_stack_drv.h"

// *****************************************************************************
// *****************************************************************************
// Section: WINC MAC Driver Interface Implementation
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MACBSSCtxGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_BSS_CONTEXT *pBSSCtx
    )

  Summary:
    Obtain a copy of the BSS context.

  Description:
    Obtain a copy of the current BSS context from within the driver.

  Remarks:
    See wdrv_winc_stack_drv.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MACBSSCtxGet
(
    DRV_HANDLE handle,
    WDRV_WINC_BSS_CONTEXT *pBSSCtx
)
{
    WDRV_WINC_MACDCPT *const pDcpt = (WDRV_WINC_MACDCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((NULL == pDcpt) || (NULL == pBSSCtx))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    OSAL_SEM_Pend(&pDcpt->bssCtxSemaphore, OSAL_WAIT_FOREVER);
    memcpy(pBSSCtx, &pDcpt->bssCtx, sizeof(WDRV_WINC_BSS_CONTEXT));
    OSAL_SEM_Post(&pDcpt->bssCtxSemaphore);

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MACBSSCtxSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_BSS_CONTEXT *pBSSCtx
    )

  Summary:
    Load a new BSS context into the driver.

  Description:
    A new BSS context should be created and initialized before being loaded into
      the MAC driver.

  Remarks:
    See wdrv_winc_stack_drv.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MACBSSCtxSet
(
    DRV_HANDLE handle,
    WDRV_WINC_BSS_CONTEXT *pBSSCtx
)
{
    WDRV_WINC_MACDCPT *const pDcpt = (WDRV_WINC_MACDCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((NULL == pDcpt) || (NULL == pBSSCtx))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    OSAL_SEM_Pend(&pDcpt->bssCtxSemaphore, OSAL_WAIT_FOREVER);
    memcpy(&pDcpt->bssCtx, pBSSCtx, sizeof(WDRV_WINC_BSS_CONTEXT));
    OSAL_SEM_Post(&pDcpt->bssCtxSemaphore);

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MACAuthCtxGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_AUTH_CONTEXT *pAuthCtx
    )

  Summary:
    Obtain a copy of the authentication context.

  Description:
    Obtain a copy of the current authentication context from within the driver.

  Remarks:
    See wdrv_winc_stack_drv.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MACAuthCtxGet
(
    DRV_HANDLE handle,
    WDRV_WINC_AUTH_CONTEXT *pAuthCtx
)
{
    WDRV_WINC_MACDCPT *const pDcpt = (WDRV_WINC_MACDCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((NULL == pDcpt) || (NULL == pAuthCtx))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    OSAL_SEM_Pend(&pDcpt->authCtxSemaphore, OSAL_WAIT_FOREVER);
    memcpy(pAuthCtx, &pDcpt->authCtx, sizeof(WDRV_WINC_AUTH_CONTEXT));
    OSAL_SEM_Post(&pDcpt->authCtxSemaphore);

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MACAuthCtxSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_AUTH_CONTEXT *pAuthCtx
    )

  Summary:
    Load a new authentication context into the driver.

  Description:
    A new authentication context should be created and initialized before being
      loaded into the MAC driver.

  Remarks:
    See wdrv_winc_stack_drv.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MACAuthCtxSet
(
    DRV_HANDLE handle,
    WDRV_WINC_AUTH_CONTEXT *pAuthCtx
)
{
    WDRV_WINC_MACDCPT *const pDcpt = (WDRV_WINC_MACDCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((NULL == pDcpt) || (NULL == pAuthCtx))
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    OSAL_SEM_Pend(&pDcpt->authCtxSemaphore, OSAL_WAIT_FOREVER);
    memcpy(&pDcpt->authCtx, pAuthCtx, sizeof(WDRV_WINC_AUTH_CONTEXT));
    OSAL_SEM_Post(&pDcpt->authCtxSemaphore);

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_OP_MODE WDRV_WINC_MACOperatingModeGet(DRV_HANDLE handle)

  Summary:
    Requests the current operation mode.

  Description:
    The driver can operate as either an infrastructure station or Soft-AP.
      This function returns the current mode of operation.

  Remarks:
    See wdrv_winc_stack_drv.h for usage information.

*/

WDRV_WINC_OP_MODE WDRV_WINC_MACOperatingModeGet(DRV_HANDLE handle)
{
    WDRV_WINC_MACDCPT *const pDcpt = (WDRV_WINC_MACDCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return WDRV_WINC_OP_MODE_UNKNOWN;
    }

    return pDcpt->operatingMode;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MACOperatingModeSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_OP_MODE operatingMode
    )

  Summary:
    Configures the current operating mode.

  Description:
    The driver can operate as either an infrastructure station or Soft-AP.
      This function configures the current mode of operation.

  Remarks:
    See wdrv_winc_stack_drv.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MACOperatingModeSet
(
    DRV_HANDLE handle,
    WDRV_WINC_OP_MODE operatingMode
)
{
    WDRV_WINC_MACDCPT *const pDcpt = (WDRV_WINC_MACDCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pDcpt->operatingMode = operatingMode;

    return WDRV_WINC_STATUS_OK;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_MAC_CONN_STATE WDRV_WINC_MACConnectStateGet(DRV_HANDLE handle)

  Summary:
    Requests the current connection state.

  Description:

  Remarks:
    See wdrv_winc_stack_drv.h for usage information.

*/

WDRV_WINC_MAC_CONN_STATE WDRV_WINC_MACConnectStateGet(DRV_HANDLE handle)
{
    WDRV_WINC_MACDCPT *const pDcpt = (WDRV_WINC_MACDCPT *const)handle;

    /* Ensure the driver handle and user pointer is valid. */
    if ((NULL == pDcpt) || (false == pDcpt->isOpen))
    {
        return WDRV_WINC_MAC_CONN_STATE_CLOSED;
    }

    if (true == pDcpt->isLinkActive)
    {
        return WDRV_WINC_MAC_CONN_STATE_CONNECTED;
    }
    else if (WDRV_WINC_DRV_STATE_CONNECTING == pDcpt->wincDrvState)
    {
        return WDRV_WINC_MAC_CONN_STATE_CONNECTING;
    }

    return WDRV_WINC_MAC_CONN_STATE_IDLE;
}

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_MACAutoConnectSet(DRV_HANDLE handle, bool autoConnect)

  Summary:
    Configures the auto-connect functionality.

  Description:
    If auto-connect is enabled then whenever the driver is opened it will
      attempt to connect using the current BSS and authentication contexts.
      If auto-connect is disabled then the driver will not attempt to connect
      when it is opened.

  Remarks:
    See wdrv_winc_stack_drv.h for usage information.

*/

WDRV_WINC_STATUS WDRV_WINC_MACAutoConnectSet(DRV_HANDLE handle, bool autoConnect)
{
    WDRV_WINC_MACDCPT *const pDcpt = (WDRV_WINC_MACDCPT *const)handle;

    /* Ensure the driver handle is valid. */
    if (NULL == pDcpt)
    {
        return WDRV_WINC_STATUS_INVALID_ARG;
    }

    pDcpt->autoConnect = autoConnect;

    return WDRV_WINC_STATUS_OK;
}
