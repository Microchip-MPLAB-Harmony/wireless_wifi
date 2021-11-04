/**
 *
 * \file
 *
 * \brief Socket Interface.
 *
 * Copyright (c) 2021 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifdef  __cplusplus
extern "C" {
#endif

/** @defgroup   SocketModule    Socket
    @brief
        This is an API providing socket functionality similar to BSD sockets.

    The application can use this API to perform socket operations within the WINC firmware. This API allows
    the application to:

    - Create and destroy UDP and TCP sockets.
    - Connect to remote TCP/UDP servers.
    - Act as TCP/UDP servers for remote clients to connect to.
    - Use TLS to protect TCP socket traffic.
    - Send and receive data.
    - Translate domain names into IP addresses.
    - Ping a remote IP address.

    @{
        @defgroup   SocketDefines   Defines
        @brief
            These are preprocessor macros to define constants used within the socket API.

        @note
            These defines should not be changed as they reflect the configuration of the WINC firmware.
            Changing these values could result in unexpected behavior.

        @see SSLSocketOptions

        @defgroup   SocketByteOrder  Byte Order
        @brief
            These macros are used to convert between host representation and network byte order.

        @defgroup   SocketStructures Data Structures
        @brief
            Specific data structures used by the socket API.

        @defgroup   SocketTypes  Typedefs
        @brief
            Specific typedefs used by the socket API.

        @defgroup   SocketEnums  Enumerations
        @brief
            Specific Enumerations used by the socket API.

        @defgroup   AsyncCallback Asynchronous Events
        @brief
            Specific data structures and enumerations used for asynchronous operations.

        Asynchronous APIs make use of callback functions to return results once the corresponding socket operation has completed.

        Callbacks allows the application to resume normal execution while the socket operation is performed.

        @defgroup   SocketCallbacks Callbacks
        @brief
            These are callback types used by the socket API when calling back to the application.

        @defgroup   SocketAPI Functions
        @brief
            These functions provide socket functionality to the application.
    @}
*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/**@addtogroup  SocketDefines
   @{*/

#define HOSTNAME_MAX_SIZE                                   64
/*!< Maximum allowed size for a host name passed to the function @ref gethostbyname. Also defines
    the maximum size of the option @ref SO_SSL_SNI used with the @ref setsockopt function.

@see    gethostbyname
@see    setsockopt
@see    SO_SSL_SNI
*/

#define SOCKET_BUFFER_MAX_LENGTH                            1400
/*!< Maximum allowed size for a socket data buffer. Used with @ref send and @ref sendto socket
    functions to ensure that the buffer sent is within the allowed range.

@see    send
@see    sendto
*/

#define  AF_INET                                            2
/*!< The AF_INET is the address family used for IPv4. An IPv4 transport address is specified with the @ref sockaddr_in structure.

@see    sockaddr_in
*/

#define INET_ADDRSTRLEN                                     16
/*!< Length of the string form for IP.

 @see   inet_ntop
*/

#define  SOCK_STREAM                                        1
/*!< Socket type for reliable connection-oriented stream connection.
     Passed to the @ref socket function for the socket creation operation.

@see    socket
*/

#define  SOCK_DGRAM                                         2
/*!< Socket type for unreliable connectionless datagram connection.
     Passed to the @ref socket function for the socket creation operation.

@see    socket
*/

#define SOCKET_FLAGS_SSL                                    0x01
/*!<
    This flag may be set in the u8Config parameter of @ref socket, to create a
    TLS socket.\n
    Note that the number of TLS sockets is limited to 4.\n
    This flag is kept for legacy purposes. It is recommended that applications
    use @ref SOCKET_CONFIG_SSL_ON instead.
*/

#define SOCKET_CONFIG_DEFAULT                               0
/*!<
    This value may be passed in the u8Config parameter of @ref socket to create
    a socket with default configuration.
*/

#define SOCKET_CONFIG_SSL_OFF                               0
/*!<
    This value may be passed in the u8Config parameter of @ref socket (when the
    u8Type parameter is @ref SOCK_STREAM), to create a socket not capable of
    TLS.
*/
#define SOCKET_CONFIG_SSL_ON                                1
/*!<
    This value may be passed in the u8Config parameter of @ref socket (when the
    u8Type parameter is @ref SOCK_STREAM), to create a TLS socket.\n
    Note that the number of TLS sockets is limited to 2.
*/
#define SOCKET_CONFIG_SSL_DELAY                             2
/*!<
    This value may be passed in the u8Config parameter of @ref socket (when the
    u8Type parameter is @ref SOCK_STREAM), to create a TCP socket which has the
    potential to upgrade to a TLS socket later (by calling @ref secure).\n
    Note that the total number of TLS sockets and potential TLS sockets is
    limited to 4.
*/

#define TCP_SOCK_MAX                                        7
/*! Maximum number of simultaneous TCP sockets supported. */

#define UDP_SOCK_MAX                                        4
/*!< Maximum number of simultaneous UDP sockets supported. */

#define MAX_SOCKET                                          (TCP_SOCK_MAX + UDP_SOCK_MAX)
/*!< Total maximum number of simultaneous sockets supported. */

#define SOL_SOCKET                                          1
/*!< Socket protocol type (non-SSL) used with the @ref setsockopt function.

@see setsockopt
*/

#define SOL_SSL_SOCKET                                      2
/*!< Socket protocol type (SSL) used with the @ref setsockopt function.

@see setsockopt
*/

#define SO_SET_UDP_SEND_CALLBACK                            0x00
/*!< Socket option to enable/disable the use of UDP send callbacks.

@see setsockopt

The option value should be cast to an \b int type.
- 0: Disable UDP send callbacks.
- 1: Enable UDP send callbacks. (Default)

@warning @ref connect and @ref bind cause this setting to
be lost, so the application should only set this option
after calling @ref connect or @ref bind.
*/

#define IP_ADD_MEMBERSHIP                                   0x01
/*!< Socket option to join a multicast group.

@see setsockopt
*/

#define IP_DROP_MEMBERSHIP                                  0x02
/*!< Socket option to leave a multicast group.

@see setsockopt
*/

#define SO_TCP_KEEPALIVE                                    0x04
/*!< Enable or disable TCP keep-alive.

@see setsockopt

The option value should be cast to \b int type.
- 0: Disable TCP keep-alive. (Default)
- 1: Enable TCP keep-alive.

@warning @ref connect and @ref bind cause this setting to
be lost, so the application should only set this option
after calling @ref connect or @ref bind.
*/

#define SO_TCP_KEEPIDLE                                     0x05
/*!< Time period after which the socket will trigger keep-alive transmissions.

@see setsockopt

The option value should be cast to \b int type.

Option value is the time period in units of 500ms. Maximum 2^32 - 1.

Default setting is 120 (60 seconds).

@warning @ref connect and @ref bind cause this setting to
be lost, so the application should only set this option
after calling @ref connect or @ref bind.
*/

#define SO_TCP_KEEPINTVL                                    0x06
/*!< Time period between keep-alive retransmissions.

@see setsockopt

The option value should be cast to \b int type.

Option value is the time period in units of 500ms. Maximum 255.

Default setting is 1 (0.5 seconds).

@warning @ref connect and @ref bind cause this setting to
be lost, so the application should only set this option
after calling @ref connect or @ref bind.
*/

#define SO_TCP_KEEPCNT                                      0x07
/*!< Number of keep-alive retransmissions to be carried out before declaring that the remote end is not available.

@see setsockopt

The option value should be cast to \b int type.

Maximum 255.

Default setting is 20.

@warning @ref connect and @ref bind cause this setting to
be lost, so the application should only set this option
after calling @ref connect or @ref bind.
*/
/**@}*/     // IPDefines

/**@addtogroup TLSDefines
 * @{
 */
#define ALPN_LIST_MAX_APP_LENGTH                            30
/*!<
    Maximum length of ALPN list that can be specified by the application.
    This length includes separators (spaces) and terminator (NUL).
*/
/**@}*/     // TLSDefines

/**@addtogroup  SSLSocketOptions
   @{*/
#define SO_SSL_BYPASS_X509_VERIF                            0x01
/*!< Allow an SSL socket to bypass the X509 certificate verification process.

    The option value should be cast to an \b int and it is handled
    as a boolean flag.

@warning
    It is recommended @b NOT to use this socket option in production software
    applications. It is intended for debugging and testing purposes only.

    The option value should be cast to an \b int and it is handled
    as a boolean flag.
    - 0: Do not bypass the X509 certificate verification process (default,
    recommended).
    - 1: Bypass the X509 certificate verification process.

@warning
    This option only takes effect if it is set after calling @ref socket and
    before calling @ref connect or @ref secure.

@see setsockopt

@code
int sslOptionEnable = 1;

setsockopt(sock, SOL_SSL_SOCKET, SO_SSL_BYPASS_X509_VERIF, &sslOptionEnable, sizeof(int));
@endcode
*/

#define SO_SSL_SNI                                          0x02
/*!< Set the Server Name Indicator (SNI) for an SSL socket.

    The SNI is a NULL terminated string containing the server name associated
    with the connection. It must not exceed the size of @ref HOSTNAME_MAX_SIZE.
    If the SNI is not a null string, then TLS Client Hello messages will include
    the SNI extension.

@warning
    This option only takes effect if it is set after calling @ref socket and
    before calling @ref connect or @ref secure.

@see setsockopt

@code
setsockopt(sock, SOL_SSL_SOCKET, SO_SSL_SNI, acHostName, strlen(acHostName) + 1);
@endcode
*/

#define SO_SSL_ENABLE_SESSION_CACHING                       0x03
/*!< Allow the WINC to cache the TLS session information for fast TLS session
    establishment in future connections using the TLS Protocol session resume features.

    The option value should be cast to an \b int and it is handled
    as a boolean flag.
    - 0: Disable TLS session caching (default).
    - 1: Enable TLS session caching.

@note TLS session caching is always enabled in TLS Server Mode and this option is ignored.

@warning
    This option only takes effect if it is set after calling @ref socket and
    before calling @ref connect or @ref secure.

@see setsockopt

@code
int sslOptionEnable = 1;

setsockopt(sock, SOL_SSL_SOCKET, SO_SSL_ENABLE_SESSION_CACHING, &sslOptionEnable, sizeof(int));
@endcode
*/

#define SO_SSL_ENABLE_SNI_VALIDATION                        0x04
/*!< Enable internal validation of server name against the server's certificate
    subject common name. If there is no server name provided
    (via the @ref SO_SSL_SNI option), setting this option does nothing.

    The option value should be cast to an \b int and it is handled
    as a boolean flag.
    - 0: Disable server certificate name validation (default).
    - 1: Enable server certificate name validation (recommended).

@warning
    This option only takes effect if it is set after calling @ref socket and
    before calling @ref connect or @ref secure.

@see setsockopt

@code
int sslOptionEnable = 1;

setsockopt(sock, SOL_SSL_SOCKET, SO_SSL_ENABLE_SNI_VALIDATION, , &sslOptionEnable, sizeof(int));
@endcode
*/

#define SO_SSL_ALPN                                         0x05
/*!<
    Set the list to use for Application-Layer Protocol Negotiation
    for an SSL socket.

@note
    This option is intended for internal use and should not be
    used by the application. Applications should use the API @ref
    set_alpn_list.
*/

/**@}*/     // SSLSocketOptions

/**@addtogroup  SocketByteOrder
   @{*/

#if defined(CONF_WINC_MCU_ARCH_LITTLE_ENDIAN)
#define _htonl(l)               CONF_WINC_UINT32_SWAP(l)
/*!< Convert a 4-byte integer from the Host representation to Network byte order.
*/

#define _htons(s)               CONF_WINC_UINT16_SWAP(s)
/*!< Convert a 2-byte integer (short) from Host representation to Network byte order.
*/

#elif defined(CONF_WINC_MCU_ARCH_BIG_ENDIAN)
#define _htonl(l)               (l)
/*!< Convert a 4-byte integer from Host representation to Network byte order.
*/

#define _htons(s)               (s)
/*!< Convert a 2-byte integer (short) from Host representation to Network byte order.
*/
#endif

#define _ntohl              _htonl
/*!< Convert a 4-byte integer from Network byte order to Host representation.
*/

#define _ntohs              _htons
/*!< Convert a 2-byte integer from Network byte order to Host representation.
*/
/**@}*/     // SocketByteOrder

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/**@addtogroup  SocketEnums
   @{*/

/*!
@enum       tenuSocketStatusType

@brief
    Possible success/error status codes returned by the Socket API.
*/

typedef enum
{
    SOCK_ERR_NO_ERROR = 0,
    /*!< Successful socket operation. */
    SOCK_ERR_INVALID_ADDRESS = -1,
    /*!< Socket address is invalid. The socket operation cannot be completed successfully without specifying a valid address. */
    SOCK_ERR_ADDR_ALREADY_IN_USE = -2,
    /*!< Socket operation cannot bind on the given address. Only one IP address per socket, and one socket per IP address is permitted -
    any attempt for a new socket to bind with an IP address already bound to another open socket will return this error code.
    */
    SOCK_ERR_MAX_TCP_SOCK = -3,
    /*!< Exceeded the maximum number of TCP sockets. A maximum number of TCP sockets opened simultaneously is defined through TCP_SOCK_MAX.
    It is not permitted to exceed that number at socket creation. Identifies that @ref socket operation failed.
    */
    SOCK_ERR_MAX_UDP_SOCK = -4,
    /*!< Exceeded the maximum number of UDP sockets. A maximum number of UDP sockets opened simultaneously is defined through UDP_SOCK_MAX.
    It is not permitted to exceed that number at socket creation. Identifies that @ref socket operation failed.
    */
    SOCK_ERR_INVALID_ARG = -6,
    /*!< An invalid argument is passed to a socket function. */
    SOCK_ERR_MAX_LISTEN_SOCK = -7,
    /*!< Exceeded the maximum number of TCP passive listening sockets. Identifies that @ref listen operation failed. */
    SOCK_ERR_INVALID = -9,
    /*!< The requested socket operation is not valid in the current socket state. */
    SOCK_ERR_ADDR_IS_REQUIRED = -11,
    /*!< Destination address is required. Failure to provide the socket address required for the socket operation to be completed. */
    SOCK_ERR_CONN_ABORTED = -12,
    /*!< The socket is closed (reset) by the peer. If this error is received, the application should call @ref close. */
    SOCK_ERR_TIMEOUT = -13,
    /*!< The socket pending operation has timed out. The socket remains open. */
    SOCK_ERR_BUFFER_FULL = -14
                           /*!< No buffer space available to be used for the requested socket operation. */
}
tenuSocketStatusType;

/**@}*/     // SocketEnums
/**@addtogroup  SocketTypes
   @{*/

/*!
@typedef    SOCKET

@brief
    Definition for socket data type.

    Socket ID is used with all socket operations to uniquely identify the socket.
    The ID is uniquely assigned at socket creation when calling @ref socket function.
*/
typedef int8_t SOCKET;

/*!
@enum       tenuSockErrSource

@brief
            Source of socket error (local, remote or unknown).

@see        tstrSockErr
*/

typedef enum
{
    SOCKET_ERR_UNKNOWN = 0,
    /*!< No detail available (also used when there is no error). */
    SOCKET_ERR_TLS_REMOTE,
    /*!< TLS Error Alert received from peer. */
    SOCKET_ERR_TLS_LOCAL
    /*!< TLS Error Alert generated locally. */
} tenuSockErrSource;

/**@}*/     // SocketTypes
/**@addtogroup  SocketStructures
   @{*/

/*!
@typedef    in_addr_t

@brief
    Packed representation of IPv4 address.
*/
typedef uint32_t in_addr_t;

/*!
@struct     in_addr

@brief
            IPv4 address representation.

            This structure is used as a placeholder for IPv4 addresses in other structures.

@see        sockaddr_in
*/
struct in_addr
{
    in_addr_t   s_addr;
    /*!< Network Byte Order representation of the IPv4 address.

        For example, the address "192.168.0.10" is represented as 0x0A00A8C0.
    */
};

/*!
@struct     sockaddr

@brief
            Generic socket address structure.

@see        sockaddr_in
*/
struct sockaddr
{
    uint16_t    sa_family;
    /*!< Socket address family. */
    uint8_t     sa_data[14];
    /*!< Socket address data. */
};

/*!
@struct     sockaddr_in

@brief
            Socket address structure for IPv4 addresses. Used to specify socket address information to connect to.
            Can be cast to @ref sockaddr structure.
*/
struct sockaddr_in
{
    uint16_t        sin_family;
    /*!<
        Specifies the address family(AF).

        @note
            The only supported value for this is AF_INET.
    */
    uint16_t        sin_port;
    /*!<
        Port number of the socket.

        It must be set in the Network Byte Order format, @ref _htons (e.g. _htons(80)).

        @note
            Must @b NOT have the value zero.
    */
    struct in_addr  sin_addr;
    /*!<
        IP Address of the socket.

        The IP address is of type @ref in_addr.

        @note
            The IP address can be zero to accept any IP address for server operations.
    */
    uint8_t         sin_zero[8];
    /*!< Padding to ensure the structure size is the same size as @ref sockaddr. */
};

/*!
@struct     tstrSockErr

@brief
            Detail about socket failures. Used with @ref get_error_detail.
*/
typedef struct
{
    tenuSockErrSource   enuErrSource;
    /*!< Source of socket error (local, remote or unknown). */
    uint8_t             u8ErrCode;
    /*!<
        TLS Alert code as defined in
        https://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml#tls-parameters-6.
    */
} tstrSockErr;

/**@}*/     // SocketStructures

/**@addtogroup  AsyncCallback
   @{ */

/*!
@enum       tenuSocketCallbackMsgType

@brief
            This enum identifies the type of events that will be received by the callback function.

            To receive callbacks the application must call @ref m2m_wifi_handle_events periodically to process any returning
            event data from the WINC device.

@see        bind
@see        listen
@see        accept
@see        connect
@see        send
@see        sendto
@see        recv
@see        recvfrom
@see        gethostbyname
*/

typedef enum
{
    SOCKET_MSG_BIND = 1,
    /*!< Bind socket event. */
    SOCKET_MSG_LISTEN,
    /*!< Listen socket event. */
    SOCKET_MSG_DNS_RESOLVE,
    /*!< DNS Resolution event. */
    SOCKET_MSG_ACCEPT,
    /*!< Accept socket event. */
    SOCKET_MSG_CONNECT,
    /*!< Connect socket event. */
    SOCKET_MSG_RECV,
    /*!< Receive socket event. */
    SOCKET_MSG_SEND,
    /*!< Send socket event. */
    SOCKET_MSG_SENDTO,
    /*!< Sendto socket event. */
    SOCKET_MSG_RECVFROM,
    /*!< Recvfrom socket event. */
    SOCKET_MSG_SECURE
    /*!< Existing socket made secure event. */
} tenuSocketCallbackMsgType;

/*!
@enum       tenuPingErrorType

@brief
            This enum identifies the success/error codes returned in the ping callback from calling @ref m2m_ping_req.
*/
typedef enum
{
    PING_ERR_SUCCESS = 0,
    /*!< The ping operation was successful. */
    PING_ERR_DEST_UNREACH = 1,
    /*!< The ping destination was unreachable. */
    PING_ERR_TIMEOUT = 2
                       /*!< The ping operation timed out. */
} tenuPingErrorType;

/*!
@struct     tstrSocketBindMsg

@brief
            Socket @ref bind status message structure.

            An asynchronous call to the @ref bind socket function, returns information through this structure in response.
            This structure together with the event @ref SOCKET_MSG_BIND are passed in parameters to the callback function.
@see
            bind
*/
typedef struct
{
    int8_t      status;
    /*!< The result of the bind operation. See @ref tenuSocketStatusType. */
} tstrSocketBindMsg;

/*!
@struct     tstrSocketListenMsg

@brief
            Socket @ref listen status message structure.

            Socket listen information is returned through this structure in response to the asynchronous call to the @ref listen function.
            This structure together with the event @ref SOCKET_MSG_LISTEN are passed in parameters to the callback function.
@see
            listen
*/
typedef struct
{
    int8_t      status;
    /*!< The result of the listen operation. See @ref tenuSocketStatusType. */
} tstrSocketListenMsg;

/*!
@struct     tstrSocketAcceptMsg

@brief
            Socket @ref accept status message structure.

            Socket accept information is returned through this structure in response to the asynchronous call to the @ref accept function.
            This structure together with the event @ref SOCKET_MSG_ACCEPT are passed in parameters to the callback function.
*/
typedef struct
{
    SOCKET      sock;
    /*!<
        On a successful @ref accept operation, the returned information is the socket ID for the accepted connection with the remote peer.
        Otherwise a negative error code is returned to indicate failure of the accept operation.
    */
    struct sockaddr_in  strAddr;
    /*!< Socket address structure for the remote peer. */
} tstrSocketAcceptMsg;

/*!
@struct     tstrSocketConnectMsg

@brief
            Socket @ref connect status message structure.

@details
            Socket connect information is returned through this structure in response to an asynchronous call to the @ref connect socket function
            or the @ref secure socket function.
            This structure and the event @ref SOCKET_MSG_CONNECT or @ref SOCKET_MSG_SECURE are passed in parameters to the callback function.
            If the application receives this structure with a negative value in @p s8Error, the application should call @ref close.
*/
typedef struct
{
    SOCKET  sock;
    /*!< Socket ID referring to the socket passed to the @ref connect or @ref secure function call. */
    int8_t  s8Error;
    /*!< Connect error code. See @ref tenuSocketStatusType. */
} tstrSocketConnectMsg;

/*!
@struct     tstrSocketRecvMsg

@brief
            Socket @ref recv or @ref recvfrom status message structure.

            Socket receive information is returned through this structure in response to the asynchronous call to the @ref recv or @ref recvfrom socket functions.
            This structure, together with the events @ref SOCKET_MSG_RECV or @ref SOCKET_MSG_RECVFROM, is passed-in parameters to the callback function.
@remark
            After receiving this structure, the application should issue a new call to @ref recv or @ref recvfrom in order to receive subsequent data.<br>
            In the case of @ref SOCKET_MSG_RECVFROM (UDP), any further data in the same datagram is dropped, then subsequent datagrams are buffered on the WINC until the application provides a buffer via a new call to @ref recvfrom.<br>
            In the case of @ref SOCKET_MSG_RECV (TCP), all subsequent data is buffered on the WINC until the application provides a buffer via a new call to @ref recv.<br>

            A negative or zero buffer size indicates an error with the following values:

            | Value                         | Description                                                            |
            |-------------------------------|------------------------------------------------------------------------|
            | @ref SOCK_ERR_NO_ERROR        | Socket connection closed. The application should now call @ref close.  |
            | @ref SOCK_ERR_CONN_ABORTED    | Socket connection aborted. The application should now call @ref close. |
            | @ref SOCK_ERR_TIMEOUT         | Socket receive timed out. The socket connection remains open.          |
*/
typedef struct
{
    uint8_t                 *pu8Buffer;
    /*!< Pointer to the buffer (passed to @ref recv and @ref recvfrom function) containing the received data chunk. */
    int16_t                 s16BufferSize;
    /*!<
        The received data chunk size.
        Holds a negative value if there is a receive error or zero on success upon reception of close socket message.
    */
    uint16_t                u16RemainingSize;
    /*!< This field is used internally by the driver. In normal operation, this field will be 0 when the application receives this structure. */
    struct sockaddr_in      strRemoteAddr;
    /*!< Socket address structure for the remote peer. It is valid for @ref SOCKET_MSG_RECVFROM events. */
} tstrSocketRecvMsg;
/**@}*/     // AsyncCallback

/**@addtogroup  SocketCallbacks
   @{ */

/*!
@typedef    tpfAppSocketCb

@brief
            The main socket application callback function.

            Applications register their main socket application callback by calling @ref registerSocketCallback.
            In response to events received this callback function is called to handle process the response from the
            corresponding asynchronous function called by the application.

@param[in]  sock
            Socket ID for the callback.

@param[in]  u8Msg
            Socket event type. See @ref tenuSocketCallbackMsgType.

@param[in]  pvMsg
            Pointer to message structure. The structure type is dependant on the value of @p u8Msg:

            | Event Type                | Structure Type            |
            |---------------------------|---------------------------|
            | @ref SOCKET_MSG_BIND      | @ref tstrSocketBindMsg    |
            | @ref SOCKET_MSG_LISTEN    | @ref tstrSocketListenMsg  |
            | @ref SOCKET_MSG_ACCEPT    | @ref tstrSocketAcceptMsg  |
            | @ref SOCKET_MSG_CONNECT   | @ref tstrSocketConnectMsg |
            | @ref SOCKET_MSG_SECURE    | @ref tstrSocketConnectMsg |
            | @ref SOCKET_MSG_RECV      | @ref tstrSocketRecvMsg    |
            | @ref SOCKET_MSG_RECVFROM  | @ref tstrSocketRecvMsg    |
            | @ref SOCKET_MSG_SEND      | int16_t                   |
            | @ref SOCKET_MSG_SENDTO    | int16_t                   |

@see        bind
@see        listen
@see        accept
@see        connect
@see        send
@see        sendto
@see        recv
@see        recvfrom
*/
typedef void (*tpfAppSocketCb)(SOCKET sock, uint8_t u8Msg, void *pvMsg);

/*!
@typedef    tpfAppResolveCb

@brief
            DNS resolution callback function.

            Applications requiring DNS resolution should register this callback by calling @ref registerSocketCallback.
            The callback is triggered in response to an asynchronous call to the @ref gethostbyname function.

@param[in]  pu8DomainName
            Domain name of the host to be resolved.

@param[in]  u32IP
            Server IPv4 address encoded in Network Byte Order format. A value of zero indicates the DNS resolution failed.
*/
typedef void (*tpfAppResolveCb)(uint8_t *pu8DomainName, uint32_t u32IP);

/*!
@typedef    tpfPingCb

@brief
            Ping (ICMP echo request) callback function.

            The function delivers the ping statistics for the sent ping triggered by calling @ref m2m_ping_req.

@param[in]  u32IPAddr
            Destination IP.

@param[in]  u32RTT
            Round Trip Time.

@param[in]  u8ErrorCode
            Ping error code. See @ref tenuPingErrorType
*/
typedef void (*tpfPingCb)(uint32_t u32IPAddr, uint32_t u32RTT, uint8_t u8ErrorCode);

/**@}*/     //SocketCallbacks

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/**@addtogroup  SocketAPI
   @{ */
/*!
@fn         void socketInit(void);

@brief
            This function performs the necessary initialization of the socket library.

@remarks
            This initialization function must be invoked before any socket operation can be performed.
*/
#ifndef WINC_SOCKET_INIT
#define WINC_SOCKET_INIT socketInit
#endif
void WINC_SOCKET_INIT(void);

/*!
@fn         void socketDeinit(void);

@brief
            This function de-initializes the socket library.

            The function performs the necessary cleanup for the socket library static data.

            It must be invoked only after all desired socket operations have been performed on any active sockets.
*/
#ifndef WINC_SOCKET_DEINIT
#define WINC_SOCKET_DEINIT socketDeinit
#endif
void WINC_SOCKET_DEINIT(void);

/*!
@fn         uint8_t IsSocketReady(void);

@see        socketInit
            socketDeinit
@return     If the socket has been initialized and is ready.
            Should return 1 after @ref socketInit and 0 after @ref socketDeinit
*/
#ifndef WINC_SOCKET_ISREADY
#define WINC_SOCKET_ISREADY IsSocketReady
#endif
uint8_t WINC_SOCKET_ISREADY(void);

/*!
@fn         void registerSocketCallback(tpfAppSocketCb pfAppSocketCallback, tpfAppResolveCb pfAppResolveCallback);

@brief
            This function registers two application provided callback functions with the socket library.
            One for asynchronous socket events and the second for DNS resolution events.

@param[in]  pfAppSocketCallback
            This callback will receive socket events from the socket library.

@param[in]  pfAppResolveCallback
            This callback will receive DNS resolution events from the socket library.

@remarks
            If either callback function pointer is NULL then the corresponding events will not be received by the application.

@pre
            Callbacks must be registered after calling @ref socketInit.

@section    SocketExample2 Example
            This example demonstrates the use of @ref registerSocketCallback to register a socket callback function with DNS resolution CB set to null
            for a simple UDP server example.

@include    registerSocketCallback.c
@example    registerSocketCallback.c
*/
#ifndef WINC_SOCKET_REG_CALLBACKS
#define WINC_SOCKET_REG_CALLBACKS registerSocketCallback
#endif
void WINC_SOCKET_REG_CALLBACKS(tpfAppSocketCb pfAppSocketCallback, tpfAppResolveCb pfAppResolveCallback);

/*!
@fn         void registerSocketEventCallback(tpfAppSocketCb pfAppSocketCallback);

@brief
            This function registers an application provided callback function with the socket library for asynchronous socket events.

@param[in]  pfAppSocketCallback
            This callback will receive socket events from the socket library.

@remarks
            If the callback function pointer is NULL then the corresponding events will not be received by the application.

@pre
            Callbacks must be registered after calling @ref socketInit.
*/
#ifndef WINC_SOCKET_REG_EVENT_CALLBACK
#define WINC_SOCKET_REG_EVENT_CALLBACK registerSocketEventCallback
#endif
void WINC_SOCKET_REG_EVENT_CALLBACK(tpfAppSocketCb pfAppSocketCallback);

/*!
@fn         void registerSocketResolveCallback(tpfAppResolveCb pfAppResolveCallback);

@brief
            This function registers an application provided callback function with the socket library for DNS resolution events.

@param[in]  pfAppResolveCallback
            This callback will receive DNS resolution events from the socket library.

@remarks
            If the callback function pointer is NULL then the corresponding events will not be received by the application.

@pre
            Callbacks must be registered after calling @ref socketInit.
*/
#ifndef WINC_SOCKET_REG_RESOLVER_CALLBACK
#define WINC_SOCKET_REG_RESOLVER_CALLBACK registerSocketResolveCallback
#endif
void WINC_SOCKET_REG_RESOLVER_CALLBACK(tpfAppResolveCb pfAppResolveCallback);

/*!
@fn         SOCKET socket(uint16_t u16Domain, uint8_t u8Type, uint8_t u8Config);

@brief
            Synchronous socket allocation function based on the specified socket type. Created sockets are non-blocking and their possible types are either TCP or a UDP sockets.
            The maximum allowed number of TCP sockets is @ref TCP_SOCK_MAX sockets while the maximum number of UDP sockets that can be created simultaneously is @ref UDP_SOCK_MAX sockets.

@param[in]  u16Domain
            Socket family. The only allowed value is @ref AF_INET (IPv4) for TCP/UDP sockets.

@param[in]  u8Type
            Socket type. Allowed values are:
            - @ref SOCK_STREAM
            - @ref SOCK_DGRAM

@param[in]  u8Config
            Used to specify the socket configuration.<br>
            For socket type @ref SOCK_STREAM, the following configuration values are defined:
            - @ref SOCKET_CONFIG_SSL_OFF - The socket is not secured by TLS.<br>
            - @ref SOCKET_CONFIG_SSL_ON - The socket is secured by TLS.
            - @ref SOCKET_CONFIG_SSL_DELAY - The socket is not secured by
            TLS, but may be secured later, by calling @ref secure.
            .
            For all socket types, the following configuration values are defined:
            - @ref SOCKET_CONFIG_DEFAULT - The default configuration.<br>
            .
            All other configuration values are reserved and should not be
            used.

@pre
            The function @ref socketInit must have been called to initialize the socket library.

@see        connect
@see        secure
@see        bind
@see        setsockopt
@see        getsockopt

@return
            On successful socket creation, a non-blocking socket type is created and a socket ID is returned
            In case of failure the function returns a negative value, identifying one of the socket error codes, see @ref tenuSocketStatusType.

@section    SocketExample3 Example
            This example demonstrates the use of the socket function to allocate the socket, returning the socket handler to be used for other
            socket operations. Socket creation is dependent on the socket type.

@subsection SE3sub1 UDP example
@code
    SOCKET UdpServerSocket = -1;

    UdpServerSocket = socket(AF_INET, SOCK_DGRAM, 0);
@endcode

@subsection SE3sub2 TCP example
@code
    SOCKET tcp_client_socket = -1;

    tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0));
@endcode
@subsection SE3sub3 SSL example

@code
    SOCKET ssl_socket = -1;

    ssl_socket = socket(AF_INET, SOCK_STREAM, SOCK_FLAGS_SSL));
@endcode
*/
#ifndef WINC_SOCKET_OPEN
#define WINC_SOCKET_OPEN socket
#endif
SOCKET WINC_SOCKET_OPEN(uint16_t u16Domain, uint8_t u8Type, uint8_t u8Config);

/*!
@fn         int8_t bind(SOCKET sock, struct sockaddr *pstrAddr, uint8_t u8AddrLen);

@brief
            Asynchronous bind function associates the provided local address and port with a socket.

            This function is required to create a server socket capable of receiving connection requests
            from a socket client. The function can be used with both TCP and UDP sockets.
            Upon socket bind completion, the application will receive a @ref SOCKET_MSG_BIND message via the socket callback.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  pstrAddr
            Pointer to a socket address structure @ref sockaddr_in.

@param[in]  u8AddrLen
            Size of the socket address structure pointed to by @p pstrAddr (in bytes).

@pre
            The @ref socket function must have been called to allocate a socket.

@see        socket
@see        listen
@see        accept

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.

@section    SocketExample4 Example
            This example demonstrates opening a UDP datagram socket and binding it to a local port.

@include    bind.c
@example    bind.c
*/
#ifndef WINC_SOCKET_BIND
#define WINC_SOCKET_BIND bind
#endif
int8_t WINC_SOCKET_BIND(SOCKET sock, struct sockaddr *pstrAddr, uint8_t u8AddrLen);

/*!
@fn         int8_t listen(SOCKET sock, uint8_t u8Backlog);

@brief
            Mark a locally bound (via @ref bind) socket as a passive socket that can be used to accept incoming connection requests.

            This asynchronous function will respond via the socket callback using the event @ref SOCKET_MSG_LISTEN.

            Upon completion of the listen operation the socket is available for connection requests, for TCP sockets these requests
            are automatically accepted by the WINC and thus require no specific call to @ref accept. Applications are notified of
            an accepted socket connection via the socket callback using the event @ref SOCKET_MSG_ACCEPT.
            There is no requirement to call the @ref accept function after calling @ref listen.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  u8Backlog
            Not used by the current implementation.

@pre
            The @ref bind function must have been called to associate a local address and port with the socket.

@see        bind
@see        accept

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.

@section    SocketExample5 Example

@include    listen.c
@example    listen.c

*/
#ifndef WINC_SOCKET_LISTEN
#define WINC_SOCKET_LISTEN listen
#endif
int8_t WINC_SOCKET_LISTEN(SOCKET sock, uint8_t u8Backlog);

/*!
@fn         int8_t accept(SOCKET sock, struct sockaddr *pstrAddr, uint8_t *pu8AddrLen);

@brief
            The function has no current implementation. An empty declaration is used to prevent errors when legacy application code is used.
            As it has no effect, it can be safely removed from any application using it.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  pstrAddr
            Not used in the current implementation.

@param[in]  pu8AddrLen
            Not used in the current implementation.

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
*/
#ifndef WINC_SOCKET_ACCEPT
#define WINC_SOCKET_ACCEPT accept
#endif
int8_t WINC_SOCKET_ACCEPT(SOCKET sock, struct sockaddr *pstrAddr, uint8_t *pu8AddrLen);

/*!
@fn         int8_t connect(SOCKET sock, struct sockaddr *pstrAddr, uint8_t u8AddrLen);

@brief
            Establishes a TCP connection with a remote server.

            The application socket callback function is notified of the result of the connection attempt through the event @ref SOCKET_MSG_CONNECT,
            along with a structure @ref tstrSocketConnectMsg.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  pstrAddr
            Pointer to socket address structure @ref sockaddr.

@param[in]  u8AddrLen
            Size of the socket address structure pointed to by @p pstrAddr (in bytes).

@pre
            The @ref socket function must have been called to allocate a TCP socket.

@see
            socket

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.

@section    SocketExample6 Example

@include    connect.c
@example    connect.c
*/
#ifndef WINC_SOCKET_CONNECT
#define WINC_SOCKET_CONNECT connect
#endif
int8_t WINC_SOCKET_CONNECT(SOCKET sock, struct sockaddr *pstrAddr, uint8_t u8AddrLen);

/*!
@fn         int8_t secure(SOCKET sock);

@param[in]  sock
            Socket ID, corresponding to a connected TCP socket.

@brief
            Converts an (insecure) TCP connection with a remote server into a secure TLS-over-TCP connection.

            It may be called after both of the following:

            - A TCP socket has been created by the @ref socket function, with u8Config parameter set to
            @ref SOCKET_CONFIG_SSL_DELAY.
            - A successful connection has been made on the socket via the @ref connect function.

            This is an asynchronous API; the application socket callback function is notified of the result
            of the attempt to make the connection secure through the event @ref SOCKET_MSG_SECURE, along
            with a structure @ref tstrSocketConnectMsg.

            If the attempt to make the connection secure fails, the application should call @ref close.

@pre
            @ref socket and @ref connect must be called to connect a TCP socket before passing the socket ID to this function.
            Value @ref SOCKET_CONFIG_SSL_DELAY must have been set in the u8Config parameter that was passed to @ref socket.

@see
            socket
            connect

@return
            The function returns SOCK_ERR_NO_ERROR for successful operations and a negative error value otherwise.
            The possible error values are:
            - @ref SOCK_ERR_INVALID_ARG
            Indicating passing invalid arguments such as negative socket ID.

            - @ref SOCK_ERR_INVALID
            Indicating failure to process the request.
*/
#ifndef WINC_SOCKET_SECURE
#define WINC_SOCKET_SECURE secure
#endif
int8_t WINC_SOCKET_SECURE(SOCKET sock);

/*!
@fn         int8_t recv(SOCKET sock, void *pvRecvBuf, uint16_t u16BufLen, uint32_t u32Timeoutmsec);

@brief
            An asynchronous function, used to retrieve data from a TCP stream.

            The application receives data via the socket callback event @ref SOCKET_MSG_RECV.

            When receiving the callback event @ref SOCKET_MSG_RECV the @p s16BufferSize element of the event structure @ref tstrSocketRecvMsg
            indicates the status of the @ref recv operation:

            | @p s16BufferSize           | Meaning                                                                                         |
            |----------------------------|-------------------------------------------------------------------------------------------------|
            | @ref SOCK_ERR_NO_ERROR     | The socket connection has closed gracefully, the application should now call @ref close.        |
            | @ref SOCK_ERR_CONN_ABORTED | The socket connection was aborted, the application, the application should now call @ref close. |
            | @ref SOCK_ERR_TIMEOUT      | The socket receive exceeded the @p u32Timeoutmsec timeout, the socket remains open.             |

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  pvRecvBuf
            Pointer to a buffer that will hold the received data. The buffer will be used in the socket callback event @ref SOCKET_MSG_RECV
            to deliver the received data to the callback. The buffer must remain valid until the socket callback has completed.

@param[in]  u16BufLen
            The buffer size in bytes.

@param[in]  u32Timeoutmsec
            Timeout for the @ref recv operation in milliseconds. If the value is set to zero, the timeout will be infinite
            (the recv function will wait forever). If no data is received during this period the socket callback will
            be called with a @p s16BufferSize value of @ref SOCK_ERR_TIMEOUT.

@pre
            The @ref socket function must have been called to allocate a TCP socket.
@pre
            The socket must be in the connected state via either of the two socket events @ref SOCKET_MSG_CONNECT or @ref SOCKET_MSG_ACCEPT.

@see        socket
@see        connect
@see        bind
@see        listen
@see        close

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.

@section    SocketExample7 Example

@include    recv.c
@example    recv.c
*/
#ifndef WINC_SOCKET_RECV
#define WINC_SOCKET_RECV recv
#endif
int8_t WINC_SOCKET_RECV(SOCKET sock, void *pvRecvBuf, uint16_t u16BufLen, uint32_t u32Timeoutmsec);

/*!
@fn         int16_t recvfrom(SOCKET sock, void *pvRecvBuf, uint16_t u16BufLen, uint32_t u32Timeoutmsec);

@brief
            An asynchronous function, used to retrieve data from a UDP socket.

            The application receives data via the socket callback event @ref SOCKET_MSG_RECVFROM.

            When receiving the callback event @ref SOCKET_MSG_RECVFROM the @p s16BufferSize element of the event structure @ref tstrSocketRecvMsg
            indicates the status of the @ref recv operation:

            | @p s16BufferSize           | Meaning                                                                                         |
            |----------------------------|-------------------------------------------------------------------------------------------------|
            | @ref SOCK_ERR_NO_ERROR     | The socket connection has closed gracefully, the application should now call @ref close.        |
            | @ref SOCK_ERR_TIMEOUT      | The socket receive exceeded the @p u32Timeoutmsec timeout, the socket remains open.             |

            The callback event @ref SOCKET_MSG_RECVFROM event structure @ref tstrSocketRecvMsg also contains the address of the remote peer
            sending the data in the element @p strRemoteAddr.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  pvRecvBuf
            Pointer to a buffer that will hold the received data. The buffer will be used in the socket callback event @ref SOCKET_MSG_RECVFROM
            to deliver the received data to the callback. The buffer must remain valid until the socket callback has completed.

@param[in]  u16BufLen
            The buffer size in bytes.

@param[in]  u32Timeoutmsec
            Timeout for the @ref recvfrom operation in milliseconds. If the value is set to zero, the timeout will be infinite
            (the recvfrom function will wait forever). If no data is received during this period the socket callback will
            be called with a @p s16BufferSize value of @ref SOCK_ERR_TIMEOUT.

@pre
            The @ref socket function must have been called to allocate a UDP socket.

@see        socket
@see        connect
@see        bind
@see        close

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.

@section    SocketExample8 Example

@include    recvfrom.c
@example    recvfrom.c
*/
#ifndef WINC_SOCKET_RECVFROM
#define WINC_SOCKET_RECVFROM recvfrom
#endif
int16_t WINC_SOCKET_RECVFROM(SOCKET sock, void *pvRecvBuf, uint16_t u16BufLen, uint32_t u32Timeoutmsec);

/*!
@fn         int8_t send(SOCKET sock, void *pvSendBuffer, uint16_t u16SendLength, uint16_t u16Flags);

@brief
            Asynchronous function used, to send data via a TCP/UDP socket.

            To send data via a TCP socket the socket must be in the connected state.

            To send data via a UDP socket the application must have previously called @ref sendto with a valid remote address.

            The socket callback will receive the event @ref SOCKET_MSG_SEND with the number of bytes sent once the data has
            been sent.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  pvSendBuffer
            Pointer to a buffer of data to be transmitted.

@param[in]  u16SendLength
            The buffer size in bytes.

@param[in]  u16Flags
            Not used in the current implementation.

@pre
            The @ref socket function must have been called to allocate a socket.

@pre
            TCP sockets: The socket must be in the connected state. For server sockets a successful @ref listen / @ref accept, and
            for client sockets a successful @ref connect must have been performed  on the socket before trying to send data.

@pre
            UDP socket: At least one successful call must be made to the @ref sendto function before using the @ref send function.

@see        sendto
@see        socket
@see        connect
@see        accept
@see        listen

@warning
            @p u16SendLength must not exceed @ref SOCKET_BUFFER_MAX_LENGTH.

@note
            Successful completion of a call to @ref send does not guarantee delivery of the message,
            a negative return value indicates only locally-detected errors

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
 */
#ifndef WINC_SOCKET_SEND
#define WINC_SOCKET_SEND send
#endif
int8_t WINC_SOCKET_SEND(SOCKET sock, void *pvSendBuffer, uint16_t u16SendLength, uint16_t u16Flags);

/*!
@fn         int8_t sendto(SOCKET sock, void *pvSendBuffer, uint16_t u16SendLength, uint16_t u16Flags, struct sockaddr *pstrAddr, uint8_t u8AddrLen);

@brief
            Asynchronous function used, to send data via a UDP socket.

            The socket callback will receive the event @ref SOCKET_MSG_SEND with the number of bytes sent once the data has
            been sent.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  pvSendBuffer
            Pointer to a buffer of data to be transmitted.

@param[in]  u16SendLength
            The buffer size in bytes.

@param[in]  u16Flags
            Not used in the current implementation.

@param[in]  pstrAddr
            Pointer to socket address structure @ref sockaddr.

@param[in]  u8AddrLen
            Size of the socket address structure pointed to by @p pstrAddr (in bytes).

@pre
            The @ref socket function must have been called to allocate a socket.

@see        socket

@warning
            @p u16SendLength must not exceed @ref SOCKET_BUFFER_MAX_LENGTH.

@note
            Successful completion of a call to @ref sendto does not guarantee delivery of the message,
            a negative return value indicates only locally-detected errors

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
*/
#ifndef WINC_SOCKET_SENDTO
#define WINC_SOCKET_SENDTO sendto
#endif
int8_t WINC_SOCKET_SENDTO(SOCKET sock, void *pvSendBuffer, uint16_t u16SendLength, uint16_t u16Flags, struct sockaddr *pstrAddr, uint8_t u8AddrLen);

/*!
@fn         int8_t close(SOCKET sock);

@brief
            Synchronous function, closes a socket and releases all the assigned resources.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@pre
            The @ref socket function must have been called to allocate a socket.

@warning
            If @ref close is called while there are still outstanding messages (to be sent or received) these will be discarded.

@see        socket

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
*/
#ifndef WINC_SOCKET_CLOSE
#define WINC_SOCKET_CLOSE close
#endif
int8_t WINC_SOCKET_CLOSE(SOCKET sock);

/*!
@fn         in_addr_t inet_addr(const char *cp);

@brief
            Synchronous function which returns a BSD socket compliant Internet Protocol (IPv4) socket address.

@param[in] cp
            A NULL terminated string containing the IP address in IPv4 dotted-decimal format.

@return
            Unsigned 32-bit integer representing the IP address in Network byte order
            (eg. "192.168.10.1" will be expressed as 0x010AA8C0).
*/
#ifndef WINC_SOCKET_INET_ADDR
#define WINC_SOCKET_INET_ADDR inet_addr
#endif
in_addr_t WINC_SOCKET_INET_ADDR(const char *cp);

/*!
@fn         const char *inet_ntop(int af, const void *src, char *dst, size_t size);

@brief
            Synchronous function which converts an IPv4 address from binary to text form.

@param[in]  af
            Address family, must be AF_INET.

@param[in]  src
            src points to a struct in_addr (in network byte order) which is to be converted.

@param[in]  dst
            dst points to a buffer to receive the dotted-decimal format IP address "ddd.ddd.ddd.ddd".

@param[in]  size
            The size of the buffer pointed to by dst. The buffer dst must be at least @ref INET_ADDRSTRLEN bytes long

@return
            On success, inet_ntop() returns a non-null pointer to dst.  NULL is returned if there was an error.
*/
#ifndef WINC_SOCKET_INET_NTOP
#define WINC_SOCKET_INET_NTOP inet_ntop
#endif
const char *WINC_SOCKET_INET_NTOP(int af, const void *src, char *dst, size_t size);

/*!
@fn         int8_t gethostbyname(const char *pcHostName);

@brief
            Asynchronous function. This function uses DNS to resolve a domain name to the corresponding IP address.

            A call to this function will cause a DNS request to be sent and the response will be delivered to the DNS callback
            function registered using @ref registerSocketCallback.

@param[in]  pcHostName
            A NULL terminated string containing the domain name for the remote host.
            Its size must not exceed @ref HOSTNAME_MAX_SIZE.

@warning
            Successful completion of a call to gethostbyname() does not guarantee success of the DNS request,
            a negative return value indicates only locally-detected errors

@see        registerSocketCallback

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
*/
#ifndef WINC_SOCKET_GETHOSTBYNAME
#define WINC_SOCKET_GETHOSTBYNAME gethostbyname
#endif
int8_t WINC_SOCKET_GETHOSTBYNAME(const char *pcHostName);

/*!
@fn         int8_t sslEnableCertExpirationCheck(tenuSslCertExpSettings enuValidationSetting);

@param[in]  enuValidationSetting
            See @ref tenuSslCertExpSettings for details.

@see        tenuSslCertExpSettings

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
*/
int8_t sslEnableCertExpirationCheck(tenuSslCertExpSettings enuValidationSetting);

/*!
@fn         int8_t setsockopt(SOCKET socket, uint8_t u8Level, uint8_t option_name, const void *option_value, uint16_t u16OptionLen);

@brief
            The setsockopt function shall set the option specified by the option_name argument, at the protocol level specified
            by the level argument, to the value pointed to by the option_value argument for the socket specified by the socket argument.

@param[in]  socket
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.

@param[in]  u8Level
            Protocol level.<br>
            Supported protocol levels are @ref SOL_SOCKET and @ref SOL_SSL_SOCKET.

@param[in]  option_name
            Option to be set.<br>
            For protocol level @ref SOL_SOCKET, the supported option names are:
            - @ref SO_SET_UDP_SEND_CALLBACK
            - @ref SO_TCP_KEEPALIVE
            - @ref SO_TCP_KEEPIDLE
            - @ref SO_TCP_KEEPINTVL
            - @ref SO_TCP_KEEPCNT
            .
            For protocol level @ref SOL_SSL_SOCKET, the supported option names are:
            - @ref SO_SSL_BYPASS_X509_VERIF
            - @ref SO_SSL_SNI
            - @ref SO_SSL_ENABLE_SESSION_CACHING
            - @ref SO_SSL_ENABLE_SNI_VALIDATION
            - @ref SO_SSL_ALPN

@param[in]  option_value
            Pointer to the user provided value.

@param[in]  u16OptionLen
            Length of the option value in bytes. Refer to each option documentation for the required length.

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
*/
#ifndef WINC_SOCKET_SETSOCKOPT
#define WINC_SOCKET_SETSOCKOPT setsockopt
#endif
int8_t WINC_SOCKET_SETSOCKOPT(SOCKET socket, uint8_t u8Level, uint8_t option_name, const void *option_value, uint16_t u16OptionLen);

/*!
@fn         int8_t getsockopt(SOCKET sock, uint8_t u8Level, uint8_t u8OptName, const void *pvOptValue, uint8_t *pu8OptLen);

@brief
            Get socket options retrieves.

@note
            This Function isn't implemented yet but this is the form that will be released later.

@param[in]  sock
            Socket ID, must hold a valid non negative value obtained from the function @ref socket.
@param[in]  u8Level
            The protocol level of the option.
@param[in]  u8OptName
            The u8OptName argument specifies a single option to get.
@param[out] pvOptValue
            The pvOptValue argument contains pointer to a buffer containing the option value.
@param[out] pu8OptLen
            Option value buffer length.
@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
*/
#ifndef WINC_SOCKET_GETSOCKOPT
#define WINC_SOCKET_GETSOCKOPT getsockopt
#endif
int8_t WINC_SOCKET_GETSOCKOPT(SOCKET sock, uint8_t u8Level, uint8_t u8OptName, const void *pvOptValue, uint8_t *pu8OptLen);

/*!
@fn         int8_t m2m_ping_req(uint32_t u32DstIP, uint8_t u8TTL, tpfPingCb fpPingCb);

@brief
            This function requests the WINC send a ping request to the given IP Address.

@param[in]  u32DstIP
            Target Destination IP Address for the ping request. It must be represented in Network Byte Order.
            The function @ref inet_addr could be used to translate the dotted decimal notation IP
            to its Network Byte Order integer representative.

@param[in]  u8TTL
            IP TTL value for the ping request. If set to ZERO, the default value will be used.

@param[in]  fpPingCb
            Callback will be called to deliver the ping statistics.

@warning
            This function should only be used to request one ping at a time; calling this API invalidates callbacks
            for previous ping requests.

@see        inet_addr

@return
            The function returns @ref SOCK_ERR_NO_ERROR for successful operations and a negative value otherwise, see @ref tenuSocketStatusType.
*/
#ifndef WINC_SOCKET_ICMP_ECHO_REQ
#define WINC_SOCKET_ICMP_ECHO_REQ m2m_ping_req
#endif
int8_t WINC_SOCKET_ICMP_ECHO_REQ(uint32_t u32DstIP, uint8_t u8TTL, tpfPingCb fpPingCb);

/*!
@fn         int8_t set_alpn_list(SOCKET sock, const char *pcProtocolList);

@brief
            This function sets the protocol list used for application-layer protocol negotiation (ALPN).

@details
            If used, it must be called after creating a SSL socket (using @ref socket) and before
            connecting/binding (using @ref connect or @ref bind) or securing (using @ref secure).

@param[in]  sock
            Socket ID obtained by a call to @ref socket. This is the SSL socket to which
            the ALPN list applies.

@param[in]  pcProtocolList
            Pointer to the list of protocols available in the application.<br>
            The entries in the list must:
            - be separated with ' ' (space).
            - not contain ' ' (space) or '\0' (NUL).
            - be non-zero length.
            .
            The list itself must:
            - be terminated with '\0' (NUL).
            - be no longer than @ref ALPN_LIST_MAX_APP_LENGTH, including separators (spaces) and terminator (NUL).
            - contain at least one entry.

@return     The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

@section    SocketExample9 Examples
            These examples demonstrate an application using @ref set_alpn_list and @ref get_alpn_index to negotiate secure HTTP/2
            (with fallback option of HTTP/1.1).

@subsection sub5 Main Function
@include    set_alpn_list_main.c
@subsection sub6 Socket Callback
@include    set_alpn_list_callback.c

@example    set_alpn_list_main.c
@example    set_alpn_list_callback.c
*/
#ifndef WINC_SOCKET_SET_ALPN_LIST
#define WINC_SOCKET_SET_ALPN_LIST set_alpn_list
#endif
int8_t WINC_SOCKET_SET_ALPN_LIST(SOCKET sock, const char *pcProtocolList);
/*!
@fn         int8_t get_alpn_index(SOCKET sock);

@brief
            This function gets the index of the protocol negotiated via ALPN.

@details
            It should be called when a SSL socket connection succeeds, in order to determine which
            application-layer protocol must be used.

@param[in]  sock
            Socket ID obtained by a call to @ref socket. This is the SSL socket to which
            the ALPN applies.

@return     The function returns:<br>
            - >0: 1-based index of negotiated protocol with respect to the list previously provided to @ref set_alpn_list.<br>
            - 0: No negotiation occurred (eg TLS peer did not support ALPN).<br>
            - <0: Invalid parameters (socket is not in use, or not an SSL socket).<br>

@see        @ref SocketExample9
*/
#ifndef WINC_SOCKET_GET_ALPN_INDEX
#define WINC_SOCKET_GET_ALPN_INDEX get_alpn_index
#endif
int8_t WINC_SOCKET_GET_ALPN_INDEX(SOCKET sock);

/*!
@fn         int8_t get_error_detail(SOCKET sock, tstrSockErr *pstrErr);

@brief
            This function gets detail about a socket failure.

@details
            The application can call this when notified
            of a socket failure via @ref SOCKET_MSG_CONNECT or @ref SOCKET_MSG_RECV.
            If used, it must be called before @ref close.

@param[in]  sock
            Socket ID obtained by a call to @ref socket.

@param[out] pstrErr
            Pointer to structure to be populated with the details of the socket failure.

@return     The function returns @ref SOCK_ERR_NO_ERROR if the request is successful. In this case pstrErr
            has been populated.
            The function returns a negative value if the request is not successful. In this case pstrErr
            has not been populated.
*/
#ifndef WINC_SOCKET_GET_ERROR_DETAIL
#define WINC_SOCKET_GET_ERROR_DETAIL get_error_detail
#endif
int8_t WINC_SOCKET_GET_ERROR_DETAIL(SOCKET sock, tstrSockErr *pstrErr);
/**@}*/     // SocketAPI

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SOCKET_H__ */
