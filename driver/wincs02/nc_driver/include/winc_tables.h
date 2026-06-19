/*
Copyright (C) 2024-26 Microchip Technology Inc. and its subsidiaries. All rights reserved.

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

#ifndef WINC_TABLES_H
#define WINC_TABLES_H

#include "microchip_wincs02_intf.h"

#define WINC_CMD_ID_TABLE_SZ        76U
#define WINC_AEC_ID_TABLE_SZ        59U
#define WINC_STATUS_ID_TABLE_SZ     92U
#define WINC_NUM_MODULES            31U

uintptr_t   WINC_TablesCommandIterate(uintptr_t iterState, uint16_t *pId, const char **ppName);
uint16_t    WINC_TablesCommandLookupByString(const char *pName);
const char* WINC_TablesCommandLookupByID(uint16_t id);

uintptr_t   WINC_TablesAECIterate(uintptr_t iterState, uint16_t *pId, const char **ppName);
uint16_t    WINC_TablesAECLookupByString(const char *pName);
const char* WINC_TablesAECLookupByID(uint16_t id);

uintptr_t   WINC_TablesStatusIterate(uintptr_t iterState, uint16_t *pId, const char **ppName);
uintptr_t   WINC_TablesModuleStatusIterate(uintptr_t iterState, uint16_t modId, uint16_t *pId, const char **ppName);
const char* WINC_TablesStatusLookupByID(uint16_t id);

int         WINC_TablesModuleIDListGet(uint8_t idx);

#endif // WINC_TABLES_H
