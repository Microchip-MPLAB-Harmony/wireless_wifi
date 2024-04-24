/*
Copyright (C) 2023-24, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef WINC_DEBUG_H
#define WINC_DEBUG_H

#define WINC_DEBUG_TYPE_NONE       0
#define WINC_DEBUG_TYPE_ERROR      1
#define WINC_DEBUG_TYPE_INFORM     2
#define WINC_DEBUG_TYPE_TRACE      3
#define WINC_DEBUG_TYPE_VERBOSE    4

#define WINC_ANSI_ESC_SEQ_END           "\033[0m"
#define WINC_ANSI_ESC_SEQ_RED_BOLD      "\033[31;1m"
#define WINC_ANSI_ESC_SEQ_RED           "\033[31m"
#define WINC_ANSI_ESC_SEQ_YELLOW        "\033[33m"

#define WINC_DEBUG_ANSI_SEQ_END         WINC_ANSI_ESC_SEQ_END
#define WINC_DEBUG_ANSI_SEQ_ERROR       WINC_ANSI_ESC_SEQ_RED_BOLD
#define WINC_DEBUG_ANSI_SEQ_INFORM      WINC_ANSI_ESC_SEQ_RED
#define WINC_DEBUG_ANSI_SEQ_TRACE       WINC_ANSI_ESC_SEQ_RED
#define WINC_DEBUG_ANSI_SEQ_WERBOSE     WINC_ANSI_ESC_SEQ_YELLOW

#ifndef WINC_DEBUG_LEVEL
#define WINC_DEBUG_LEVEL                WINC_DEBUG_TYPE_ERROR
#endif

#define WINC_VERBOSE_PRINT(...)
#define WINC_TRACE_PRINT(...)
#define WINC_INFORM_PRINT(...)
#define WINC_ERROR_PRINT(...)

#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_ERROR)
#undef WINC_ERROR_PRINT
#define WINC_ERROR_PRINT(format, ...)                       do { if (NULL != pfWINCDevDebugPrintf) { pfWINCDevDebugPrintf(WINC_DEBUG_ANSI_SEQ_ERROR format WINC_DEBUG_ANSI_SEQ_END, ##__VA_ARGS__); } } while (0)
#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_INFORM)
#undef WINC_INFORM_PRINT
#define WINC_INFORM_PRINT(format, ...)                      do { if (NULL != pfWINCDevDebugPrintf) { pfWINCDevDebugPrintf(WINC_DEBUG_ANSI_SEQ_INFORM format WINC_DEBUG_ANSI_SEQ_END, ##__VA_ARGS__); } } while (0)
#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_TRACE)
#undef WINC_TRACE_PRINT
#define WINC_TRACE_PRINT(format, ...)                       do { if (NULL != pfWINCDevDebugPrintf) { pfWINCDevDebugPrintf(WINC_DEBUG_ANSI_SEQ_TRACE format WINC_DEBUG_ANSI_SEQ_END, ##__VA_ARGS__); } } while (0)
#if (WINC_DEBUG_LEVEL >= WINC_DEBUG_TYPE_VERBOSE)
#undef WINC_VERBOSE_PRINT
#define WINC_VERBOSE_PRINT(format, ...)                     do { if (NULL != pfWINCDevDebugPrintf) { pfWINCDevDebugPrintf(WINC_DEBUG_ANSI_SEQ_WERBOSE format WINC_DEBUG_ANSI_SEQ_END, ##__VA_ARGS__); } } while (0)
#endif /* WINC_DEBUG_TYPE_VERBOSE */
#endif /* WINC_DEBUG_TYPE_TRACE */
#endif /* WINC_DEBUG_TYPE_INFORM */
#endif /* WINC_DEBUG_TYPE_ERROR */

typedef void (*WINC_DEBUG_PRINTF_FP)(const char *format, ...);

extern WINC_DEBUG_PRINTF_FP pfWINCDevDebugPrintf;

#endif /* WINC_DEBUG_H */
