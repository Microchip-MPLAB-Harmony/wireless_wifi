/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "wdrv_winc_client_api.h"
#include "serial_bridge/serial_bridge.h"
#include "platform/platform.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

static SERIAL_BRIDGE_DECODER_STATE serialBridgeDecoderState;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

#ifdef APP_ENABLE_SEC_CONN
extern const WDRV_WINC_SYS_INIT *appWincInitData[];
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

#ifdef APP_ENABLE_SEC_CONN
void WDRV_WINC_RESETN_Clear(void)
{
    if (0 == appData.currentWincInitDataIdx)
    {
        WDRV_WINC_RESETN_EXT1_Clear();
    }
    else
    {
        WDRV_WINC_RESETN_EXT2_Clear();
    }
}

void WDRV_WINC_RESETN_Set(void)
{
    if (0 == appData.currentWincInitDataIdx)
    {
        WDRV_WINC_RESETN_EXT1_Set();
    }
    else
    {
        WDRV_WINC_RESETN_EXT2_Set();
    }
}

void WDRV_WINC_CHIP_EN_Set(void)
{
    if (0 == appData.currentWincInitDataIdx)
    {
        WDRV_WINC_CHIP_EN_EXT1_Set();
    }
    else
    {
        WDRV_WINC_CHIP_EN_EXT2_Set();
    }
}

void WDRV_WINC_CHIP_EN_Clear(void)
{
    if (0 == appData.currentWincInitDataIdx)
    {
        WDRV_WINC_CHIP_EN_EXT1_Clear();
    }
    else
    {
        WDRV_WINC_CHIP_EN_EXT2_Clear();
    }
}
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize(void)
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    appData.currentWincInitDataIdx = 0;

    SerialBridge_PlatformInit();
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks(void)
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            appData.state = APP_STATE_INIT_WINC;

            break;
        }

        case APP_STATE_INIT_WINC:
        {
            SYS_STATUS status;

            status = WDRV_WINC_Status(sysObj.drvWifiWinc);

            if (SYS_STATUS_READY == status)
            {
                appData.state = APP_STATE_WDRV_OPEN_BRIDGE;
            }
            else if (SYS_STATUS_ERROR == status)
            {
                WDRV_WINC_Deinitialize(sysObj.drvWifiWinc);

#ifdef APP_ENABLE_SEC_CONN
                if (WDRV_WINC_SYS_STATUS_ERROR_DEVICE_NOT_FOUND == WDRV_WINC_StatusExt(sysObj.drvWifiWinc))
                {
                    break;
                }
#endif

                appData.state = APP_STATE_WDRV_OPEN_BRIDGE;
            }
            else if (SYS_STATUS_UNINITIALIZED == status)
            {
                sysObj.drvWifiWinc = SYS_MODULE_OBJ_INVALID;

#ifdef APP_ENABLE_SEC_CONN
                if (0 == appData.currentWincInitDataIdx)
                {
                    appData.currentWincInitDataIdx = 1;

                    sysObj.drvWifiWinc = WDRV_WINC_Initialize(0, (SYS_MODULE_INIT*)appWincInitData[appData.currentWincInitDataIdx]);

                    if (SYS_MODULE_OBJ_INVALID != sysObj.drvWifiWinc)
                    {
                        break;
                    }
                }
#endif
                appData.state = APP_STATE_ERROR;
            }

            break;
        }

        case APP_STATE_WDRV_OPEN_BRIDGE:
        {
            SerialBridge_Init(&serialBridgeDecoderState, 115200);

            appData.state = APP_STATE_WDRV_SERIAL_BRIDGE;
            break;
        }

        case APP_STATE_WDRV_SERIAL_BRIDGE:
        {
            SerialBridge_Process(&serialBridgeDecoderState);
            break;
        }

        case APP_STATE_ERROR:
        default:
        {
            break;
        }
    }
}

/*******************************************************************************
 End of File
 */

