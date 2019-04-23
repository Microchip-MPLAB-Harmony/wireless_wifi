/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    netinet_in.h

  Summary:

  Description:
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

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
// DOM-IGNORE-END

#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <stdint.h>
#include <sys/types.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

typedef uint32_t in_addr_t;

struct in_addr {
    /*!<
        Network Byte Order representation of the IPv4 address. For example,
        the address "192.168.0.10" is represented as 0x0A00A8C0.
    */
    in_addr_t s_addr;
};

struct sockaddr_in{
    uint16_t        sin_family;
    /*!<
        Specifies the address family(AF).
        Members of AF_INET address family are IPv4 addresses.
        Hence,the only supported value for this is AF_INET.
    */
    uint16_t        sin_port;
    /*!<
        Port number of the socket.
        Network sockets are identified by a pair of IP addresses and port number.
        Must be set in the Network Byte Order format , @ref _htons (e.g. _htons(80)).
        Can NOT have zero value.
    */
    struct in_addr  sin_addr;
    /*!<
        IP Address of the socket.
        The IP address is of type @ref in_addr structure.
        Can be set to "0" to accept any IP address for server operation.
    */
    uint8_t         sin_zero[8];
    /*!<
        Padding to make structure the same size as @ref sockaddr.
    */
};

const char *inet_ntop(int af, const void *src, char *dst, u_int32_t size);
in_addr_t inet_addr(const char *cp);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* _NETINET_IN_H */
