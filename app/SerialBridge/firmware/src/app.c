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
Copyright (c) 2017 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
//#include "config/default/system/console/sys_debug.h"
#include "serial_bridge/serial_bridge.h"
#include "wdrv_winc_client_api.h"
//#include "config/default/driver/winc/include/winc1500_19_6_1/driver/nmasic.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

#define CONSOLE_BUFFER_SIZE         512

//__attribute__ ((aligned (32))) 
static char consoleReadBuffer[CONSOLE_BUFFER_SIZE]  __attribute__((aligned(16)));// __attribute__(aligned(16));
static char consoleCmdBuffer[SB_CMD_BUFFER_SIZE];
static DRV_HANDLE consoleHandle = DRV_HANDLE_INVALID;
static int consoleReadBufferLength;
static int consoleCmdBufferLength;
static SERIAL_BRIDGE_DECODER_STATE serialBridgeDecoderState;
static DRV_USART_SERIAL_SETUP setup_sb;

#ifdef DRV_USBFS_DEVICE_SUPPORT
static USB_DEVICE_CDC_TRANSFER_HANDLE consoleReadTransferHandle;
static USB_DEVICE_CDC_TRANSFER_HANDLE consoleWriteTransferHandle;
static bool isReadComplete;
static bool isWriteComplete;
static USB_CDC_LINE_CODING setLineCodingData;
#else
static DRV_USART_BUFFER_HANDLE consoleReadTransferHandle;
static bool consoleReadReset;
#endif

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


#ifdef DRV_USBFS_DEVICE_SUPPORT
/*******************************************************
 * USB CDC Device Events - Application Event Handler
 *******************************************************/

USB_DEVICE_CDC_EVENT_RESPONSE APP_USBDeviceCDCEventHandler
(
    USB_DEVICE_CDC_INDEX index,
    USB_DEVICE_CDC_EVENT event,
    void *pData,
    uintptr_t userData
)
{
    switch(event)
    {
        case USB_DEVICE_CDC_EVENT_GET_LINE_CODING:
        {
            static USB_CDC_LINE_CODING getLineCodingData =
            {
                .dwDTERate   = 9600,
                .bParityType = 0,
                .bParityType = 0,
                .bDataBits   = 8,
            };

            USB_DEVICE_ControlSend(consoleHandle, &getLineCodingData, sizeof(USB_CDC_LINE_CODING));
            break;
        }

        case USB_DEVICE_CDC_EVENT_SET_LINE_CODING:
        {
            USB_DEVICE_ControlReceive(consoleHandle, &setLineCodingData, sizeof(USB_CDC_LINE_CODING));
            break;
        }

        case USB_DEVICE_CDC_EVENT_SET_CONTROL_LINE_STATE:
        {
            USB_DEVICE_ControlStatus(consoleHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        }

        case USB_DEVICE_CDC_EVENT_SEND_BREAK:
        {
            USB_DEVICE_ControlStatus(consoleHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        }

        case USB_DEVICE_CDC_EVENT_READ_COMPLETE:
        {
            USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE *eventDataRead = (USB_DEVICE_CDC_EVENT_DATA_READ_COMPLETE*)pData; 

            isReadComplete = true;
            consoleReadBufferLength = eventDataRead->length; 
            break;
        }

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_RECEIVED:
        {
            USB_DEVICE_ControlStatus(consoleHandle, USB_DEVICE_CONTROL_STATUS_OK);
            break;
        }

        case USB_DEVICE_CDC_EVENT_CONTROL_TRANSFER_DATA_SENT:
        {
            break;
        }

        case USB_DEVICE_CDC_EVENT_WRITE_COMPLETE:
        {
            isWriteComplete = true;
            break;
        }

        default:
        {
            break;
        }
    }

    return USB_DEVICE_CDC_EVENT_RESPONSE_NONE;
}

/***********************************************
 * Application USB Device Layer Event Handler.
 ***********************************************/
void APP_USBDeviceEventHandler(USB_DEVICE_EVENT event, void* eventData, uintptr_t context)
{
    switch(event)
    {
        case USB_DEVICE_EVENT_RESET:
        {
            appData.isConfigured = false;
            break;
        }

        case USB_DEVICE_EVENT_CONFIGURED:
        {
            USB_DEVICE_EVENT_DATA_CONFIGURED *configuredEventData = (USB_DEVICE_EVENT_DATA_CONFIGURED*)eventData;

            /* Check the configuration. We only support configuration 1 */
            if (1 == configuredEventData->configurationValue)
            {
                USB_DEVICE_CDC_EventHandlerSet(USB_DEVICE_CDC_INDEX_0, APP_USBDeviceCDCEventHandler, (uintptr_t)&appData);

                appData.isConfigured = true;
            }
            break;
        }

        case USB_DEVICE_EVENT_POWER_DETECTED:
        {
            USB_DEVICE_Attach(consoleHandle);
            break;
        }

        case USB_DEVICE_EVENT_POWER_REMOVED:
        {
            USB_DEVICE_Detach(consoleHandle);
            break;
        }

        case USB_DEVICE_EVENT_SUSPENDED:
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        default:
        {
            break;
        }
    }
}
#else
void _USARTBufferEventHandler(DRV_USART_BUFFER_EVENT event, DRV_USART_BUFFER_HANDLE handle, uintptr_t context)
{
    if (handle == consoleReadTransferHandle)
    {
        if (DRV_USART_BUFFER_EVENT_COMPLETE == event)
        {
            DRV_USART_ReadBufferAdd(consoleHandle, consoleReadBuffer, CONSOLE_BUFFER_SIZE, &consoleReadTransferHandle);
            consoleReadReset = true;
        }
    }
}
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

#ifdef DRV_USBFS_DEVICE_SUPPORT
bool APP_StateReset(void)
{
    if (true == appData.isConfigured)
    {
        return false;
    }
    
    appData.state = APP_STATE_WAIT_FOR_CONFIGURATION;

    isReadComplete = true;
    isWriteComplete = true;

    return true;
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
    //NT("\n@@@@APP_INIT\n");
    
    /* Place the App state machine in its initial state. */
    //appData.state = APP_STATE_INIT;
    setup_sb.baudRate = 115200;
    setup_sb.dataWidth = DRV_USART_DATA_8_BIT;
    setup_sb.parity = DRV_USART_PARITY_NONE;
    setup_sb.stopBits = DRV_USART_STOP_1_BIT;
  
   // memset(consoleReadBuffer,0,CONSOLE_BUFFER_SIZE);
   // memset(consoleCmdBuffer,0,SB_CMD_BUFFER_SIZE);

#ifdef DRV_USBFS_DEVICE_SUPPORT
    appData.isConfigured = false;
    
    isReadComplete = true;
    isWriteComplete = true;
#else
    consoleHandle = DRV_USART_Open(DRV_USART_INDEX_0, (DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_NONBLOCKING));

    if (DRV_HANDLE_INVALID != consoleHandle)
    {
        //DRV_USART_BaudSet(consoleHandle, DRV_USART_BAUD_RATE_IDX0); sagar

        DRV_USART_SerialSetup(consoleHandle, &setup_sb);
        //DRV_USART_LineControlSet(consoleHandle, DRV_USART_LINE_CONTROL_8NONE1);//sagar
        DRV_USART_ReadBufferAdd(consoleHandle, consoleReadBuffer, CONSOLE_BUFFER_SIZE, &consoleReadTransferHandle);
        consoleReadBufferLength = 0;
        consoleReadReset = false;
    }
#endif    

    consoleCmdBufferLength = 0;
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
            bool appInitialized = true;
            if (appInitialized)
            {
                #ifdef DRV_USBFS_DEVICE_SUPPORT
                consoleHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);

                if (consoleHandle != USB_DEVICE_HANDLE_INVALID)
                {
                    /* Register a callback with device layer to get event notification (for end point 0) */
                    USB_DEVICE_EventHandlerSet(consoleHandle, APP_USBDeviceEventHandler, 0);

                    appData.state = APP_STATE_INIT_WINC;
                }
                else
                {
                    /* The Device Layer is not ready to be opened. We should try
                    * again later. */
                }
#else
                appData.state = APP_STATE_INIT_WINC;
#endif            
                //SYS_PRINT("\n@@@@@APP STATE INT\n");
            }
            break;
        }
            
        case APP_STATE_INIT_WINC:
        {
            //uint8_t tmp;
            if (SYS_STATUS_READY == WDRV_WINC_Status(sysObj.drvWifiWinc))
            {
#ifdef DRV_USBFS_DEVICE_SUPPORT
                appData.state = APP_STATE_WAIT_FOR_CONFIGURATION;
#else                
                appData.state = APP_STATE_WDRV_OPEN_BRIDGE;
                
           //     SYS_PRINT("APP_WINC_INIT");
                
                DRV_USART_BufferEventHandlerSet(consoleHandle, _USARTBufferEventHandler, (uintptr_t)NULL);
               //SYS_PRINT("\n@@@@APP_STATE_INIT_WINC\n");
#endif                

                SB_StreamDecoderInit(&serialBridgeDecoderState, 9600);
                //nmi_get_mac_address(&tmp);
            }
            break;
        }

#ifdef DRV_USBFS_DEVICE_SUPPORT
        case APP_STATE_WAIT_FOR_CONFIGURATION:
        {
            if (true == appData.isConfigured)
            {
                appData.state = APP_STATE_SCHEDULE_READ;
            }
            break;
        }

        case APP_STATE_SCHEDULE_READ:
        {
            if (APP_StateReset())
            {
                break;
            }

            if(isReadComplete == true)
            {
                isReadComplete = false;

                USB_DEVICE_CDC_Read(USB_DEVICE_CDC_INDEX_0, &consoleReadTransferHandle, consoleReadBuffer, CONSOLE_BUFFER_SIZE);
                
                if(consoleReadTransferHandle == USB_DEVICE_CDC_TRANSFER_HANDLE_INVALID)
                {
                    appData.state = APP_STATE_ERROR;
                    break;
                }
            }

            appData.state = APP_STATE_WAIT_FOR_READ_COMPLETE;
            break;
        }

        case APP_STATE_WAIT_FOR_READ_COMPLETE:
        {
            if (APP_StateReset())
            {
                break;
            }

            /* Check if a character was received or a switch was pressed.
             * The isReadComplete flag gets updated in the CDC event handler. */

            if(isReadComplete)
            {
                consoleCmdBufferLength = SB_StreamDecoder(&serialBridgeDecoderState, (const uint8_t*)consoleReadBuffer, consoleReadBufferLength, (uint8_t*)consoleCmdBuffer);

                if (consoleCmdBufferLength > 0)
                {
                    isWriteComplete = false;
                    
                    USB_DEVICE_CDC_Write(USB_DEVICE_CDC_INDEX_0, &consoleWriteTransferHandle, consoleCmdBuffer, consoleCmdBufferLength, USB_DEVICE_CDC_TRANSFER_FLAGS_DATA_COMPLETE);
                    
                    appData.state = APP_STATE_WAIT_FOR_WRITE_COMPLETE;
                }
            }

            break;
        }

        case APP_STATE_WAIT_FOR_WRITE_COMPLETE:
        {
            if (APP_StateReset())
            {
                break;
            }

            if (true == isWriteComplete)
            {
                appData.state = APP_STATE_SCHEDULE_READ;
            }

            break;
        }
#else
        
        case APP_STATE_WDRV_OPEN_BRIDGE:
        {
            int tailOffset = DRV_USART_BufferCompletedBytesGet(consoleReadTransferHandle);
            DRV_USART_BUFFER_EVENT event;
            //SYS_PRINT("APP_STATE_WDRV_OPEN_BRIDGE@@@@@");
            if (true == consoleReadReset)
            {
                tailOffset = CONSOLE_BUFFER_SIZE;
            }

            if (consoleReadBufferLength < tailOffset)
            {
                uint32_t baudRateBefore;
                uint32_t baudRateAfter;

                baudRateBefore = SB_StreamDecoderGetBaudRate(&serialBridgeDecoderState);

                consoleCmdBufferLength = SB_StreamDecoder(&serialBridgeDecoderState, (const uint8_t*)&consoleReadBuffer[consoleReadBufferLength], (tailOffset - consoleReadBufferLength), (uint8_t*)consoleCmdBuffer);

                if (consoleCmdBufferLength > 0)
                {
                    DRV_USART_BUFFER_HANDLE consoleWriteTransferHandle;

                    DRV_USART_WriteBufferAdd(consoleHandle, consoleCmdBuffer, consoleCmdBufferLength, &consoleWriteTransferHandle);
                 
                    //Check the status of the buffer
                    //This call can be used to wait until the buffer is processed.
                  
                     while ((event = DRV_USART_BufferStatusGet(consoleWriteTransferHandle)) == DRV_USART_BUFFER_EVENT_PENDING);    
                    
                }
                if(consoleReadBuffer[0] == 0x12)
                    baudRateAfter = SB_StreamDecoderGetBaudRate(&serialBridgeDecoderState);
                else
                    baudRateAfter = SB_StreamDecoderGetBaudRate(&serialBridgeDecoderState);

                if (baudRateAfter != baudRateBefore)
                {
                   // DRV_USART_BaudSet(consoleHandle, baudRateAfter); sagar
                    setup_sb.baudRate = baudRateAfter;
                    DRV_USART_SerialSetup(consoleHandle, &setup_sb);
                }

                consoleReadBufferLength = tailOffset;
            }
            if ((CONSOLE_BUFFER_SIZE == consoleReadBufferLength) && (true == consoleReadReset))
            {
                consoleReadBufferLength = 0;
                consoleReadReset = false;
            }
               //SYS_PRINT("APP_STATE_WDRV_OPEN_BRIDGE\n");
            break;
        }
#endif

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

