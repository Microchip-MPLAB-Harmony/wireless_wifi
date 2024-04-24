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

#ifndef WINC_SOCKET_H
#define WINC_SOCKET_H

#include <stdint.h>
#include <errno.h>

#include "winc_dev.h"

/* Maximum TCP socket payload size */
#define MAX_TCP_IPV4_SOCK_PAYLOAD_SZ        1460
#define MAX_TCP_IPV6_SOCK_PAYLOAD_SZ        1440
#define MAX_TCP_SOCK_PAYLOAD_SZ             MAX_TCP_IPV4_SOCK_PAYLOAD_SZ

/* Maximum UDP socket payload size */
#define MAX_UDP_IPV4_SOCK_PAYLOAD_SZ        1472
#define MAX_UDP_IPV6_SOCK_PAYLOAD_SZ        1452
#define MAX_UDP_SOCK_PAYLOAD_SZ             MAX_UDP_IPV4_SOCK_PAYLOAD_SZ

/* Maximum socket payload size */
#define MAX_SOCK_PAYLOAD_SZ                 MAX_UDP_SOCK_PAYLOAD_SZ


/*****************************************************************************
  Description:
    Socket module initialisation structure.

  Remarks:
    Settings for malloc/free function use and slab memory allocator.

 *****************************************************************************/

typedef struct
{
    void*   (*pfMemAlloc)(size_t);
    void    (*pfMemFree)(void*);
    size_t  slabSize;
    int     numSlabs;
} WINC_SOCKET_INIT_TYPE;

/*****************************************************************************
  Description:
    Socket event callback types.

  Remarks:

 *****************************************************************************/

typedef enum
{
    WINC_SOCKET_EVENT_OPEN,
    WINC_SOCKET_EVENT_LISTEN,
    WINC_SOCKET_EVENT_CONNECT_REQ,
    WINC_SOCKET_EVENT_CONNECT,
    WINC_SOCKET_EVENT_SEND,
    WINC_SOCKET_EVENT_RECV,
    WINC_SOCKET_EVENT_CLOSE,
    WINC_SOCKET_EVENT_TLS_CONNECT,
    WINC_SOCKET_EVENT_ERROR
} WINC_SOCKET_EVENT;

/*****************************************************************************
  Description:
    Socket status types.

  Remarks:

 *****************************************************************************/

typedef enum
{
    WINC_SOCKET_STATUS_OK,
    WINC_SOCKET_STATUS_ERROR
} WINC_SOCKET_STATUS;

/* Socket event callback function type. */
typedef void (*WINC_SOCKET_EVENT_CALLBACK)(uintptr_t context, int socket, WINC_SOCKET_EVENT event, WINC_SOCKET_STATUS status);

/*****************************************************************************
                            Socket Module API
 *****************************************************************************/

bool WINC_SockInit(WINC_DEVICE_HANDLE devHandle, WINC_SOCKET_INIT_TYPE *pInitData);
bool WINC_SockDeinit(WINC_DEVICE_HANDLE devHandle);
bool WINC_SockRegisterEventCallback(WINC_DEVICE_HANDLE devHandle, WINC_SOCKET_EVENT_CALLBACK pfSocketEventCB, uintptr_t context);

/*****************************************************************************
                          Berekeley Sockets API
 *****************************************************************************/

/***************************** alltypes.h ************************************/

typedef unsigned int socklen_t;
typedef unsigned short sa_family_t;

/***************************** netinet/in.h **********************************/

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
struct in_addr { in_addr_t s_addr; };

struct sockaddr_in {
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;
    uint8_t sin_zero[8];
};

struct in6_addr {
    union {
        uint8_t __s6_addr[16];
        uint16_t __s6_addr16[8];
        uint32_t __s6_addr32[4];
    } __in6_union;
};
#define s6_addr __in6_union.__s6_addr
#define s6_addr16 __in6_union.__s6_addr16
#define s6_addr32 __in6_union.__s6_addr32

struct sockaddr_in6 {
    sa_family_t     sin6_family;
    in_port_t       sin6_port;
    uint32_t        sin6_flowinfo;
    struct in6_addr sin6_addr;
    uint32_t        sin6_scope_id;
};

#define INADDR_ANY        ((in_addr_t) 0x00000000)
#define INADDR_BROADCAST  ((in_addr_t) 0xffffffff)
#define INADDR_NONE       ((in_addr_t) 0xffffffff)
#define INADDR_LOOPBACK   ((in_addr_t) 0x7f000001)

#define IN6ADDR_ANY_INIT      { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }
#define IN6ADDR_LOOPBACK_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }

extern const struct in6_addr in6addr_any, in6addr_loopback;

uint16_t htons(uint16_t n);
uint32_t htonl(uint32_t n);
uint16_t ntohs(uint16_t n);
uint32_t ntohl(uint32_t n);

#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#define IPPROTO_TLS     253

#define IP_TOS                  1
#define IP_ADD_MEMBERSHIP       35

struct ip_mreqn {
    struct in_addr imr_multiaddr; /* IP multicast group address */
    struct in_addr imr_address;   /* IP address of local interface */
    int            imr_ifindex;   /* interface index */
};

#define IPV6_ADD_MEMBERSHIP     20
#define IPV6_JOIN_GROUP         IPV6_ADD_MEMBERSHIP

struct ipv6_mreq {
    struct in6_addr ipv6mr_multiaddr;   /* IPv6 multicast addr */
    unsigned int    ipv6mr_interface;   /* interface index */
};

/***************************** netinet/tcp.h *********************************/

#define TCP_NODELAY     1

/***************************** netinet/tls.h *********************************/

#define TLS_CONF_IDX    1

/***************************** arpa/inet.h ***********************************/

int inet_pton(int af, const char *s, void *a0);
const char *inet_ntop(int af, const void *a0, char *s, socklen_t l);

/******************************* socket.h ************************************/

#define SHUT_RD         0
#define SHUT_WR         1
#define SHUT_RDWR       2

#define SOCK_DGRAM      WINC_CONST_SOCKET_PROTOCOL_UDP
#define SOCK_STREAM     WINC_CONST_SOCKET_PROTOCOL_TCP

#define SOL_SOCKET      65535

#define SO_SNDBUF       7
#define SO_RCVBUF       8
#define SO_KEEPALIVE    9
#define SO_LINGER       13

#define PF_UNSPEC       0
#define PF_INET         WINC_CONST_NETIF_PROTOCOL_VERSION_IPV4
#define PF_INET6        WINC_CONST_NETIF_PROTOCOL_VERSION_IPV6

#define AF_UNSPEC       PF_UNSPEC
#define AF_INET         PF_INET
#define AF_INET6        PF_INET6

#define MSG_PEEK        0x0002
#define MSG_TRUNC       0x0020

struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
};

/******************************** netdb.h ************************************/

struct addrinfo {
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	socklen_t ai_addrlen;
	struct sockaddr *ai_addr;
	char *ai_canonname;
	struct addrinfo *ai_next;
};

#define EAI_BADFLAGS   -1
#define EAI_NONAME     -2
#define EAI_AGAIN      -3
#define EAI_FAIL       -4
#define EAI_NODATA     -5
#define EAI_FAMILY     -6
#define EAI_SOCKTYPE   -7
#define EAI_SERVICE    -8
#define EAI_MEMORY     -10
#define EAI_SYSTEM     -11
#define EAI_OVERFLOW   -12

/* sockets API */
int socket(int domain, int type, int protocol);
int shutdown(int fd, int how);
int bind(int fd, const struct sockaddr *addr, socklen_t len);
int listen(int fd, int backlog);
int accept(int fd, struct sockaddr *addr, socklen_t *len);
int connect(int fd, const struct sockaddr *addr, socklen_t len);
ssize_t recv(int fd, void *buf, size_t len, int flags);
ssize_t recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *addr, socklen_t *alen);
ssize_t send(int fd, const void *buf, size_t len, int flags);
ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *addr, socklen_t alen);
int getaddrinfo(const char *host, const char *serv, const struct addrinfo *hint, struct addrinfo **res);
void freeaddrinfo(struct addrinfo *p);
int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

#endif /* WINC_SOCKET_H */
