/*******************************************************************************
  WINC3400 Wireless Driver

  File Name:
    inet_addr.c

  Summary:
    Implementation of standard inet_addr function.

  Description:
    Implementation of standard inet_addr function.
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
#include "socket.h"

in_addr_t inet_addr(const char *cp)
{
    uint8_t i,l;
    uint16_t t;
    uint32_t ip;
    char c;

    ip = 0;

    for (i=0; i<4; i++)
    {
        t = 0;
        ip >>= 8;

        // Count non-delimiter or terminator characters

        for (l=0; l<4; l++)
        {
            c = cp[l];

            if (('.' == c) || ('\0' == c))
            {
                break;
            }
        }

        // There must be 1 to 3 characters

        if ((0 == l) || (4 == l))
        {
            return 0;
        }

        c = *cp++;

        // First digit can't be '0' unless it's the only one

        if ((l > 1) && (c == '0'))
        {
            return 0;
        }

        while(l--)
        {
            // Each digit must be decimal

            if ((c < '0') || (c > '9'))
            {
                return 0;
            }

            t = (t * 10) + (c - '0');

            c = *cp++;
        }

        // Total accumulated number must be less than 256

        if (t > 255)
        {
            return 0;
        }

        // Pack number into 32 bit IP address representation

        ip |= ((uint32_t)t << 24);

        // First three numbers must terminate with '.', last one with '\0's

        if ((('\0' == c) && (i != 3)) || (('\0' != c) && (i == 3)))
        {
            return 0;
        }
    }

    return ip;
}

//DOM-IGNORE-END
