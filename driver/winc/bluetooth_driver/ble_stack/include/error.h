// DOM-IGNORE-BEGIN
/*
Copyright (c) RivieraWaves 2009-2014
Copyright (C) 2017, Microchip Technology Inc., and its subsidiaries. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

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
// DOM-IGNORE-END
/*
 * dbg_task.h
 *
 *  Created on: Oct 28, 2014
 *      Author: lali
 */
#ifndef ERROR_H_
#define ERROR_H_

#include "cmn_defs.h"

at_ble_status_t at_ble_att_error(uint8_t att_error_code);

at_ble_status_t at_ble_gap_error(uint8_t gap_error_code);


#define IS_ERR(__status__) (__status__ != AT_BLE_SUCCESS)

#define ERRORCHECK(__status__) do{\
    if(IS_ERR(__status__))\
    {\
        printf("ERRORCHECK(%d)\n", __status__);\
        goto ERRORHANDLER;\
    }\
}while(0)

#define ERRORREPORT(__status__, __err__) do{\
    printf("NMI_ERRORREPORT(%d)\n", __err__);\
    __status__ = __err__;\
    goto ERRORHANDLER;\
}while(0)

#define  NULLCHECK(__status__, __ptr__) do{\
    if(__ptr__ == NULL)\
    {\
        ERRORREPORT(__status__, AT_BLE_FAILURE);\
    }\
}while(0)

#define CATCH(__status__) \
ERRORHANDLER :\
if(IS_ERR(__status__)) \

#endif

