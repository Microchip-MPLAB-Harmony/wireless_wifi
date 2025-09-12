/*******************************************************************************
  WINC Wireless Driver Debug Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_debug.h

  Summary:
    WINC wireless driver debug interface.

  Description:
    Provides methods to send formatted debugging information.
 *******************************************************************************/

/*
Copyright (C) 2024-25 Microchip Technology Inc. and its subsidiaries. All rights reserved.

Subject to your compliance with these terms, you may use this Microchip software and any derivatives
exclusively with Microchip products. You are responsible for complying with third party license terms
applicable to your use of third party software (including open source software) that may accompany this
Microchip software. SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR
STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-
INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL
MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS,
DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER
CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY
TO MICROCHIP FOR THIS SOFTWARE.
*/

#ifndef WDRV_WINC_DEBUG_H
#define WDRV_WINC_DEBUG_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Debug Defines
// *****************************************************************************
// *****************************************************************************

#ifndef WDRV_WINC_DEVICE_USE_SYS_DEBUG
// *****************************************************************************
/*  Debug Levels

  Summary:
    Defines for debugging verbosity level.

  Description:
    These defines can be assigned to WDRV_WINC_DEBUG_LEVEL to affect the level of
      verbosity provided by the debug output channel.

  Remarks:
    None.
*/

#define WDRV_WINC_DEBUG_TYPE_NONE       0
#define WDRV_WINC_DEBUG_TYPE_ERROR      1
#define WDRV_WINC_DEBUG_TYPE_INFORM     2
#define WDRV_WINC_DEBUG_TYPE_TRACE      3
#define WDRV_WINC_DEBUG_TYPE_VERBOSE    4

// *****************************************************************************
/*  Debug Verbosity

  Summary:
    Defines the chosen level of debugging verbosity supported.

  Description:
    This define sets the debugging output verbosity level.

  Remarks:
    None.
*/

#ifndef WDRV_WINC_DEBUG_LEVEL
#define WDRV_WINC_DEBUG_LEVEL   WDRV_WINC_DEBUG_TYPE_TRACE
#endif

// *****************************************************************************
/*  Debugging Macros

  Summary:
    Macros to define debugging output.

  Description:
    Macros are provided for each level of verbosity.

  Remarks:
    None.
*/

#define WDRV_DBG_VERBOSE_PRINT(...)
#define WDRV_DBG_TRACE_PRINT(...)
#define WDRV_DBG_INFORM_PRINT(...)
#define WDRV_DBG_ERROR_PRINT(...)

#if (WDRV_WINC_DEBUG_LEVEL >= WDRV_WINC_DEBUG_TYPE_ERROR)
#undef WDRV_DBG_ERROR_PRINT
#define WDRV_DBG_ERROR_PRINT(...) do { if (NULL != pfWINCDebugPrintCb) { pfWINCDebugPrintCb(__VA_ARGS__); } } while (false)
#if (WDRV_WINC_DEBUG_LEVEL >= WDRV_WINC_DEBUG_TYPE_INFORM)
#undef WDRV_DBG_INFORM_PRINT
#define WDRV_DBG_INFORM_PRINT(...) do { if (NULL != pfWINCDebugPrintCb) { pfWINCDebugPrintCb(__VA_ARGS__); } } while (false)
#if (WDRV_WINC_DEBUG_LEVEL >= WDRV_WINC_DEBUG_TYPE_TRACE)
#undef WDRV_DBG_TRACE_PRINT
#define WDRV_DBG_TRACE_PRINT(...) do { if (NULL != pfWINCDebugPrintCb) { pfWINCDebugPrintCb(__VA_ARGS__); } } while (false)
#if (WDRV_WINC_DEBUG_LEVEL >= WDRV_WINC_DEBUG_TYPE_VERBOSE)
#undef WDRV_DBG_VERBOSE_PRINT
#define WDRV_DBG_VERBOSE_PRINT(...) do { if (NULL != pfWINCDebugPrintCb) { pfWINCDebugPrintCb(__VA_ARGS__); } } while (false)
#endif /* WDRV_WINC_DEBUG_TYPE_VERBOSE */
#endif /* WDRV_WINC_DEBUG_TYPE_TRACE */
#endif /* WDRV_WINC_DEBUG_TYPE_INFORM */
#endif /* WDRV_WINC_DEBUG_TYPE_ERROR */
#else
#define WDRV_DBG_VERBOSE_PRINT(...)         do { SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, __VA_ARGS__); } while (false)
#define WDRV_DBG_TRACE_PRINT(...)           do { SYS_DEBUG_PRINT(SYS_ERROR_INFO, __VA_ARGS__); } while (false)
#define WDRV_DBG_INFORM_PRINT(...)          do { SYS_DEBUG_PRINT(SYS_ERROR_WARNING, __VA_ARGS__); } while (false)
#define WDRV_DBG_ERROR_PRINT(...)           do { SYS_DEBUG_PRINT(SYS_ERROR_ERROR, __VA_ARGS__); } while (false)

#endif

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Debug Data Types
// *****************************************************************************
// *****************************************************************************

#ifndef WDRV_WINC_DEVICE_USE_SYS_DEBUG
// *****************************************************************************
/*  Debug Callback

  Function:
    void (*WDRV_WINC_DEBUG_PRINT_CALLBACK)(const char *format, ...)

  Summary:
    Defines the debug callback.

  Description:
    The function callback provides a printf-like prototype.

 Parameters:
    format - Format specifiers in printf style.
    ...    - Variable number of arguments.

 Returns:
    None.

  Remarks:
    None.
*/

typedef void (*WDRV_WINC_DEBUG_PRINT_CALLBACK)(const char *format, ...);

#endif

#ifndef WDRV_WINC_DEVICE_USE_SYS_DEBUG
// Reference debug output channel printf-like function.
extern WDRV_WINC_DEBUG_PRINT_CALLBACK pfWINCDebugPrintCb;
#endif
#endif /* WDRV_WINC_DEBUG_H */
