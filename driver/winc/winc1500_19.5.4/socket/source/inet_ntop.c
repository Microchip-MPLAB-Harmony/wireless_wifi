/*******************************************************************************
  WINC1500 Wireless Driver

  File Name:
    inet_ntop.c

  Summary:
    Implementation of standard inet_ntop function.

  Description:
    Implementation of standard inet_ntop function.
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

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include "socket.h"

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    uint8_t i, v, t, c, n;
    char *rp = dst;
    uint32_t ip = ((struct in_addr*)src)->s_addr;

    if ((NULL == src) || (NULL == dst) || (size < 16))
    {
        return NULL;
    }

    for (i=0; i<4; i++)
    {
        t = ip;
        v = 100;

        // Check for zero

        if (t > 0)
        {
            n = 0;

            do
            {
                c = '0';
                while (t >= v)
                {
                    c++;
                    t -= v;
                }
                v /= 10;

                if (('0' != c) || (n > 0))
                {
                    *dst++ = c;

                    n++;
                }
            }
            while (v > 0);
        }
        else
        {
            *dst++ = '0';
        }

        if (3 == i)
        {
            *dst++ = '\0';
        }
        else
        {
            *dst++ = '.';
        }

        ip >>= 8;
    }

    return rp;
}

//DOM-IGNORE-END
