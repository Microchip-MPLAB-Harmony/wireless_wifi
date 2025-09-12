/*
Copyright (C) 2023-25 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#ifndef WINC_DEBUG_H
#define WINC_DEBUG_H

#define WINC_DEBUG_TYPE_NONE       0
#define WINC_DEBUG_TYPE_ERROR      1
#define WINC_DEBUG_TYPE_INFORM     2
#define WINC_DEBUG_TYPE_TRACE      3
#define WINC_DEBUG_TYPE_VERBOSE    4

#define WINC_ANSI_ESC_SEQ_END           "\033" "[0m"
#define WINC_ANSI_ESC_SEQ_RED_BOLD      "\033" "[31;1m"
#define WINC_ANSI_ESC_SEQ_RED           "\033" "[31m"
#define WINC_ANSI_ESC_SEQ_YELLOW        "\033" "[33m"
#define WINC_ANSI_ESC_SEQ_BLUE          "\033" "[34m"
#define WINC_ANSI_ESC_SEQ_MAGENTA       "\033" "[35m"

#define WINC_DEBUG_ANSI_SEQ_END         WINC_ANSI_ESC_SEQ_END
#define WINC_DEBUG_ANSI_SEQ_ERROR       WINC_ANSI_ESC_SEQ_RED_BOLD
#define WINC_DEBUG_ANSI_SEQ_INFORM      WINC_ANSI_ESC_SEQ_BLUE
#define WINC_DEBUG_ANSI_SEQ_TRACE       WINC_ANSI_ESC_SEQ_MAGENTA
#define WINC_DEBUG_ANSI_SEQ_VERBOSE     WINC_ANSI_ESC_SEQ_YELLOW

#ifndef WINC_DEBUG_LEVEL
#define WINC_DEBUG_LEVEL                WINC_DEBUG_TYPE_ERROR
#endif

#define WINC_VERBOSE_PRINT(...)
#define WINC_TRACE_PRINT(...)
#define WINC_INFORM_PRINT(...)
#define WINC_ERROR_PRINT(...)

#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_ERROR)
#undef WINC_ERROR_PRINT
#define WINC_ERROR_PRINT(format, ...)                       do { if (NULL != pfWINCDevDebugPrintf) { pfWINCDevDebugPrintf(WINC_DEBUG_ANSI_SEQ_ERROR format WINC_DEBUG_ANSI_SEQ_END, ##__VA_ARGS__); } } while (false)
#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_INFORM)
#undef WINC_INFORM_PRINT
#define WINC_INFORM_PRINT(format, ...)                      do { if (NULL != pfWINCDevDebugPrintf) { pfWINCDevDebugPrintf(WINC_DEBUG_ANSI_SEQ_INFORM format WINC_DEBUG_ANSI_SEQ_END, ##__VA_ARGS__); } } while (false)
#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_TRACE)
#undef WINC_TRACE_PRINT
#define WINC_TRACE_PRINT(format, ...)                       do { if (NULL != pfWINCDevDebugPrintf) { pfWINCDevDebugPrintf(WINC_DEBUG_ANSI_SEQ_TRACE format WINC_DEBUG_ANSI_SEQ_END, ##__VA_ARGS__); } } while (false)
#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_VERBOSE)
#undef WINC_VERBOSE_PRINT
#define WINC_VERBOSE_PRINT(format, ...)                     do { if (NULL != pfWINCDevDebugPrintf) { pfWINCDevDebugPrintf(WINC_DEBUG_ANSI_SEQ_VERBOSE format WINC_DEBUG_ANSI_SEQ_END, ##__VA_ARGS__); } } while (false)
#endif /* WINC_DEBUG_TYPE_VERBOSE */
#endif /* WINC_DEBUG_TYPE_TRACE */
#endif /* WINC_DEBUG_TYPE_INFORM */
#endif /* WINC_DEBUG_TYPE_ERROR */

typedef void (*WINC_DEBUG_PRINTF_FP)(const char *format, ...);

extern WINC_DEBUG_PRINTF_FP pfWINCDevDebugPrintf;

#endif /* WINC_DEBUG_H */
