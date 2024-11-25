/*
Copyright (C) 2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#ifndef WINC_TABLES_H
#define WINC_TABLES_H

#include "microchip_pic32mzw2_nc_intf.h"

#define WINC_CMD_ID_TABLE_SZ        66U
#define WINC_AEC_ID_TABLE_SZ        49U
#define WINC_STATUS_ID_TABLE_SZ     65U
#define WINC_NUM_MODULES            26U

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
