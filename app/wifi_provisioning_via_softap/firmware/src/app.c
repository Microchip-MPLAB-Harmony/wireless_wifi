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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "socket.h"


#if defined(WDRV_WINC_DEVICE_WINC1500)
#include "system/console/sys_debug.h"
#elif defined(WDRV_WINC_DEVICE_WINC3400)
#include "system/debug/sys_debug.h"
#endif 
#include "nm_common.h"
#include "m2m_wifi.h"


#include "wdrv_winc_client_api.h"
#include <stdarg.h>



//=============================================
  


void APP_ExampleInitialize(DRV_HANDLE handle);
void APP_ExampleTasks(DRV_HANDLE handle);

#define APP_PRINT_BUFFER_SIZ    2048

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

static DRV_HANDLE wdrvHandle;

static char printBuff[APP_PRINT_BUFFER_SIZ] __attribute__((aligned(4)));
static int printBuffPtr;
static OSAL_MUTEX_HANDLE_TYPE consoleMutex;

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

static void APP_DebugPrint(uint8_t *pBuf, size_t len)
{
    if ((len > 0) && (len < APP_PRINT_BUFFER_SIZ))
    {
        if (OSAL_RESULT_TRUE == OSAL_MUTEX_Lock(&consoleMutex, OSAL_WAIT_FOREVER))
        {
            if ((len + printBuffPtr) > APP_PRINT_BUFFER_SIZ)
            {
                printBuffPtr = 0;
            }

            memcpy(&printBuff[printBuffPtr], pBuf, len);
            SYS_CONSOLE_Write(0, STDOUT_FILENO, &printBuff[printBuffPtr], len);

            printBuffPtr = (printBuffPtr + len + 3) & ~3;

            OSAL_MUTEX_Unlock(&consoleMutex);
        }
    }
}

void APP_DebugPrintf(const char* format, ...)
{
    char tmpBuf[APP_PRINT_BUFFER_SIZ];
    size_t len = 0;
    va_list args;
    va_start( args, format );

    len = vsnprintf(tmpBuf, APP_PRINT_BUFFER_SIZ, format, args);

    va_end( args );

    APP_DebugPrint((uint8_t*)tmpBuf, len);
}

char APP_HexToChar(uint8_t hex)
{
    if (hex < 10)
        return '0' + hex;

    if (hex < 16)
        return 'A' + (hex - 10);

    return '-';
}

void APP_DebugPrintBuffer(const uint8_t *pBuf, uint16_t bufLen)
{
    uint8_t tmpBuf[APP_PRINT_BUFFER_SIZ];
    size_t len = 0;
    uint16_t i;
    uint8_t *pB;

    if ((NULL == pBuf) || (0 == bufLen))
        return;

    if (bufLen > (APP_PRINT_BUFFER_SIZ/2))
        bufLen = (APP_PRINT_BUFFER_SIZ/2);

    pB = tmpBuf;
    for (i=0; i<bufLen; i++)
    {
        *pB++ = APP_HexToChar((pBuf[i] & 0xf0) >> 4);
        *pB++ = APP_HexToChar(pBuf[i] & 0x0f);
    }

    len = bufLen*2;

    APP_DebugPrint(tmpBuf, len);
}

// *****************************************************************************

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

    printBuffPtr = 0;
    OSAL_MUTEX_Create(&consoleMutex);

    WDRV_WINC_DebugRegisterCallback(APP_DebugPrintf);
}




void APP_Tasks(void)
{
    switch(appData.state)
    {
        case APP_STATE_INIT:
        {
            if (SYS_STATUS_READY == WDRV_WINC_Status(sysObj.drvWifiWinc))
            {
                appData.state = APP_STATE_WDRV_INIT_READY;
            }

            break;
        }

        case APP_STATE_WDRV_INIT_READY:
        {
            wdrvHandle = WDRV_WINC_Open(0, 0);

            if (DRV_HANDLE_INVALID != wdrvHandle)
            {
                appData.state = APP_STATE_WDRV_OPEN;
                APP_ExampleInitialize(wdrvHandle);
            }
            break;
        }

        case APP_STATE_WDRV_OPEN:
        {
            APP_ExampleTasks(wdrvHandle);
            break;
        }

        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

/*******************************************************************************
 End of File
 */
