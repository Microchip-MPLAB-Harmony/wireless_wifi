/**
 *
 * \file
 *
 * \brief Socket Interface.
 *
 * Copyright (c) 2020 Microchip Technology Inc. and its subsidiaries.
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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "bsp/winc_bsp.h"
#include "common/winc_defines.h"
#include "common/winc_debug.h"
#include "m2m/winc_m2m_types.h"
#include "winc_socket.h"
#include "m2m/winc_m2m_socket_host_if.h"
#include "driver/winc_hif.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define TLS_RECORD_HEADER_LENGTH            (5)
#define ETHERNET_HEADER_OFFSET              (34)
#define ETHERNET_HEADER_LENGTH              (14)
#define TCP_IP_HEADER_LENGTH                (40)
#define UDP_IP_HEADER_LENGTH                (28)

#define IP_PACKET_OFFSET                    (ETHERNET_HEADER_LENGTH + ETHERNET_HEADER_OFFSET - M2M_HIF_HDR_OFFSET)

#define TCP_TX_PACKET_OFFSET                (IP_PACKET_OFFSET + TCP_IP_HEADER_LENGTH)
#define UDP_TX_PACKET_OFFSET                (IP_PACKET_OFFSET + UDP_IP_HEADER_LENGTH)
#define SSL_TX_PACKET_OFFSET                (TCP_TX_PACKET_OFFSET + TLS_RECORD_HEADER_LENGTH)

#define SSL_FLAGS_ACTIVE                    NBIT0
#define SSL_FLAGS_BYPASS_X509               NBIT1
#define SSL_FLAGS_CACHE_SESSION             NBIT4
#define SSL_FLAGS_CHECK_CERTNAME            NBIT6
#define SSL_FLAGS_DELAY                     NBIT7

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
PRIVATE DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@brief
*/
typedef struct
{
    SOCKET      sock;
    uint8_t     u8Dummy;
    uint16_t    u16SessionID;
} tstrCloseCmd;

/*!
@brief
*/
typedef struct
{
    uint8_t             *pu8UserBuffer;
    uint16_t            u16UserBufferSize;
    uint16_t            u16SessionID;
    uint16_t            u16DataOffset;
    uint8_t             bIsUsed;
    uint8_t             u8SSLFlags;
    uint8_t             bIsRecvPending;
    uint8_t             u8AlpnStatus;
    uint8_t             u8ErrSource;
    uint8_t             u8ErrCode;
} tstrSocket;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
GLOBALS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

static tstrSocket               gastrSockets[MAX_SOCKET];
static uint16_t                 gu16SessionID = 0;

static tpfAppSocketCb           gpfAppSocketCb;
static tpfAppResolveCb          gpfAppResolveCb;
static uint8_t                  gbSocketInit = 0;
static tpfPingCb                gfpPingCb = NULL;
static uint32_t                 gu32PingId = 0;

static void Socket_ReadSocketData(SOCKET sock, tstrSocketRecvMsg *pstrRecv, uint8_t u8SocketMsg,
                                  uint32_t u32StartAddress, uint16_t u16ReadCount)
{
    tstrSocket *pstrSocket = &gastrSockets[sock];
    uint16_t    u16Read;

    pstrRecv->u16RemainingSize = u16ReadCount;

    if ((u16ReadCount > 0) && (pstrSocket->pu8UserBuffer != NULL) && (pstrSocket->u16UserBufferSize > 0) && (pstrSocket->bIsUsed == 1))
    {
        u16Read = u16ReadCount;

        if (u16ReadCount > pstrSocket->u16UserBufferSize)
        {
            /* We don't expect to be here. Firmware only sends data to the driver according to the application's buffer size.
             * But it is worth keeping this check, eg in case the application calls recv again with a smaller buffer size, or in case of HIF hacking. */
            u16Read = pstrSocket->u16UserBufferSize;
        }

        if (winc_hif_receive(u32StartAddress, pstrSocket->pu8UserBuffer, u16Read) == M2M_SUCCESS)
        {
            pstrRecv->pu8Buffer         = pstrSocket->pu8UserBuffer;
            pstrRecv->s16BufferSize     = (int16_t)u16Read;
            pstrRecv->u16RemainingSize  -= u16Read;

            pstrSocket->u16UserBufferSize = 0;
            pstrSocket->pu8UserBuffer = NULL;

            if (gpfAppSocketCb)
            {
                gpfAppSocketCb(sock, u8SocketMsg, pstrRecv);
            }
        }
        else
        {
            WINC_LOG_ERROR("Current <%u>", u16ReadCount);
        }
    }
}

void m2m_ip_cb(uint8_t u8OpCode, uint16_t u16BufferSize, uint32_t u32Address)
{
    if (u8OpCode == SOCKET_CMD_BIND)
    {
        tstrBindReply       strBindReply;
        tstrSocketBindMsg   strBind;

        if (winc_hif_receive(u32Address, &strBindReply, sizeof(tstrBindReply)) == M2M_SUCCESS)
        {
            strBind.status = strBindReply.s8Status;

            if (gpfAppSocketCb)
            {
                gpfAppSocketCb(strBindReply.sock, SOCKET_MSG_BIND, &strBind);
            }
        }
    }
    else if (u8OpCode == SOCKET_CMD_LISTEN)
    {
        tstrListenReply         strListenReply;
        tstrSocketListenMsg     strListen;

        if (winc_hif_receive(u32Address, &strListenReply, sizeof(tstrListenReply)) == M2M_SUCCESS)
        {
            strListen.status = strListenReply.s8Status;

            if (gpfAppSocketCb)
            {
                gpfAppSocketCb(strListenReply.sock, SOCKET_MSG_LISTEN, &strListen);
            }
        }
    }
    else if (u8OpCode == SOCKET_CMD_ACCEPT)
    {
        tstrAcceptReply         strAcceptReply;
        tstrSocketAcceptMsg     strAccept;

        if (winc_hif_receive(u32Address, &strAcceptReply, sizeof(tstrAcceptReply)) == M2M_SUCCESS)
        {
            if ((strAcceptReply.sConnectedSock >= 0) && (strAcceptReply.sConnectedSock < MAX_SOCKET))
            {
                gastrSockets[strAcceptReply.sConnectedSock].u8SSLFlags  = 0;
                gastrSockets[strAcceptReply.sConnectedSock].bIsUsed     = 1;

                /* The session ID is used to distinguish different socket connections
                    by comparing the assigned session ID to the one reported by the firmware*/
                ++gu16SessionID;

                if (gu16SessionID == 0)
                {
                    ++gu16SessionID;
                }

                gastrSockets[strAcceptReply.sConnectedSock].u16SessionID = gu16SessionID;
                WINC_LOG_DEBUG("Socket %d session ID = %u", strAcceptReply.sConnectedSock, gu16SessionID);
            }

            strAccept.sock = strAcceptReply.sConnectedSock;
            strAccept.strAddr.sin_family        = AF_INET;
            strAccept.strAddr.sin_port = strAcceptReply.strAddr.u16Port;
            strAccept.strAddr.sin_addr.s_addr = strAcceptReply.strAddr.u32IPAddr;

            if (gpfAppSocketCb)
            {
                gpfAppSocketCb(strAcceptReply.sListenSock, SOCKET_MSG_ACCEPT, &strAccept);
            }
        }
    }
    else if ((u8OpCode == SOCKET_CMD_CONNECT) || (u8OpCode == SOCKET_CMD_SSL_CONNECT) || (u8OpCode == SOCKET_CMD_SSL_CONNECT_ALPN))
    {
        /* Note that for successful connections the firmware always sends SOCKET_CMD_CONNECT, even for SSL connections. */
        tstrConnectAlpnReply    strConnectAlpnReply = {0};
        tstrSocketConnectMsg    strConnMsg;
        uint16_t                u16HifSz = sizeof(tstrConnectAlpnReply);

        if (u8OpCode != SOCKET_CMD_SSL_CONNECT_ALPN)
        {
            u16HifSz = sizeof(tstrConnectReply);
        }

        if (winc_hif_receive(u32Address, &strConnectAlpnReply, u16HifSz) == M2M_SUCCESS)
        {
            if ((strConnectAlpnReply.strConnReply.sock >= 0) && (strConnectAlpnReply.strConnReply.sock < MAX_SOCKET))
            {
                uint8_t u8Msg = SOCKET_MSG_CONNECT;

                strConnMsg.sock     = strConnectAlpnReply.strConnReply.sock;
                strConnMsg.s8Error  = strConnectAlpnReply.strConnReply.s8Error;

                /* If the SOCKET_CMD_SSL_CONNECT op code is received and the socket was already connected, then the
                    callback corresponds to an attempt to make the socket secure. */
                if (0 != gastrSockets[strConnMsg.sock].u16DataOffset)
                {
                    u8Msg = SOCKET_MSG_SECURE;
                }

                if (strConnectAlpnReply.strConnReply.s8Error == SOCK_ERR_NO_ERROR)
                {
                    gastrSockets[strConnMsg.sock].u16DataOffset = strConnectAlpnReply.strConnReply.u16AppDataOffset - M2M_HIF_HDR_OFFSET;
                    gastrSockets[strConnMsg.sock].u8AlpnStatus = strConnectAlpnReply.u8AppProtocolIdx;
                }
                else
                {
                    gastrSockets[strConnMsg.sock].u8ErrSource = strConnectAlpnReply.strConnReply.u8ErrSource;
                    gastrSockets[strConnMsg.sock].u8ErrCode = strConnectAlpnReply.strConnReply.u8ErrCode;
                }

                if (gpfAppSocketCb)
                {
                    gpfAppSocketCb(strConnMsg.sock, u8Msg, &strConnMsg);
                }
            }
        }
    }
    else if (u8OpCode == SOCKET_CMD_DNS_RESOLVE)
    {
        tstrDnsReply    strDnsReply;

        if (winc_hif_receive(u32Address, &strDnsReply, sizeof(tstrDnsReply)) == M2M_SUCCESS)
        {
            strDnsReply.u32HostIP = strDnsReply.u32HostIP;

            if (gpfAppResolveCb)
            {
                gpfAppResolveCb((uint8_t *)strDnsReply.acHostName, strDnsReply.u32HostIP);
            }
        }
    }
    else if ((u8OpCode == SOCKET_CMD_RECV) || (u8OpCode == SOCKET_CMD_RECVFROM) || (u8OpCode == SOCKET_CMD_SSL_RECV))
    {
        SOCKET              sock;
        int16_t             s16RecvStatus;
        tstrRecvReply       strRecvReply;
        uint16_t            u16ReadSize;
        tstrSocketRecvMsg   strRecvMsg;
        uint8_t             u8CallbackMsgID = SOCKET_MSG_RECV;
        uint16_t            u16DataOffset;

        if (u8OpCode == SOCKET_CMD_RECVFROM)
        {
            u8CallbackMsgID = SOCKET_MSG_RECVFROM;
        }

        /* Read RECV REPLY data structure.
        */
        u16ReadSize = sizeof(tstrRecvReply);

        if (winc_hif_receive(u32Address, &strRecvReply, u16ReadSize) == M2M_SUCCESS)
        {
            if ((strRecvReply.sock >= 0) && (strRecvReply.sock < MAX_SOCKET))
            {
                uint16_t u16SessionID = 0;

                sock            = strRecvReply.sock;
                u16SessionID = strRecvReply.u16SessionID;
                WINC_LOG_DEBUG("recv callback session ID = %u", u16SessionID);

                /* Reset the Socket RX Pending Flag.
                */
                gastrSockets[sock].bIsRecvPending = 0;

                s16RecvStatus   = WINC_TO_HOST_U16(strRecvReply.s16RecvStatus);
                u16DataOffset   = WINC_TO_HOST_U16(strRecvReply.u16DataOffset);
                strRecvMsg.strRemoteAddr.sin_port           = strRecvReply.strRemoteAddr.u16Port;
                strRecvMsg.strRemoteAddr.sin_addr.s_addr    = strRecvReply.strRemoteAddr.u32IPAddr;

                if (u16SessionID == gastrSockets[sock].u16SessionID)
                {
                    if ((s16RecvStatus > 0) && ((uint16_t)s16RecvStatus < u16BufferSize))
                    {
                        /* Skip incoming bytes until reaching the Start of Application Data.
                        */
                        u32Address += u16DataOffset;

                        /* Read the Application data and deliver it to the application callback in
                        the given application buffer. Firmware only sends data up to
                        the size of the application buffer. For TCP, a new call to recv is needed
                        in order to retrieve any outstanding data from firmware.
                        */
                        u16ReadSize = (uint16_t)s16RecvStatus;
                        Socket_ReadSocketData(sock, &strRecvMsg, u8CallbackMsgID, u32Address, u16ReadSize);
                    }
                    else
                    {
                        /* Don't tidy up here. Application must call close().
                        */
                        strRecvMsg.s16BufferSize    = s16RecvStatus;
                        strRecvMsg.pu8Buffer        = NULL;

                        if (gpfAppSocketCb)
                        {
                            gpfAppSocketCb(sock, u8CallbackMsgID, &strRecvMsg);
                        }
                    }
                }
                else
                {
                    WINC_LOG_DEBUG("Discard recv callback %u %u", u16SessionID, gastrSockets[sock].u16SessionID);
                }
            }
        }
    }
    else if ((u8OpCode == SOCKET_CMD_SEND) || (u8OpCode == SOCKET_CMD_SENDTO) || (u8OpCode == SOCKET_CMD_SSL_SEND))
    {
        SOCKET          sock;
        int16_t         s16Rcvd;
        tstrSendReply   strReply;
        uint8_t         u8CallbackMsgID = SOCKET_MSG_SEND;

        if (u8OpCode == SOCKET_CMD_SENDTO)
        {
            u8CallbackMsgID = SOCKET_MSG_SENDTO;
        }

        if (winc_hif_receive(u32Address, &strReply, sizeof(tstrSendReply)) == M2M_SUCCESS)
        {
            if ((strReply.sock >= 0) && (strReply.sock < MAX_SOCKET))
            {
                uint16_t u16SessionID = 0;

                sock = strReply.sock;
                u16SessionID = strReply.u16SessionID;
                WINC_LOG_DEBUG("send callback session ID = %u", u16SessionID);

                s16Rcvd = WINC_TO_HOST_U16(strReply.s16SentBytes);

                if (u16SessionID == gastrSockets[sock].u16SessionID)
                {
                    if (gpfAppSocketCb)
                    {
                        gpfAppSocketCb(sock, u8CallbackMsgID, &s16Rcvd);
                    }
                }
                else
                {
                    WINC_LOG_DEBUG("Discard send callback %u %u", u16SessionID, gastrSockets[sock].u16SessionID);
                }
            }
        }
    }
    else if (u8OpCode == SOCKET_CMD_PING)
    {
        tstrPingReply   strPingReply;

        if (winc_hif_receive(u32Address, &strPingReply, sizeof(tstrPingReply)) == M2M_SUCCESS)
        {
            if ((gu32PingId == strPingReply.u32CmdPrivate) && (gfpPingCb != NULL))
            {
                gfpPingCb(strPingReply.u32IPAddr, strPingReply.u32RTT, strPingReply.u8ErrorCode);
            }

            gfpPingCb = NULL;
        }
    }
}

void WINC_SOCKET_INIT(void)
{
    if (gbSocketInit == 0)
    {
        memset(gastrSockets, 0, MAX_SOCKET * sizeof(tstrSocket));
        gbSocketInit    = 1;
        gu16SessionID   = 0;
    }
}

void WINC_SOCKET_DEINIT(void)
{
    memset(gastrSockets, 0, MAX_SOCKET * sizeof(tstrSocket));
    gpfAppSocketCb  = NULL;
    gpfAppResolveCb = NULL;
    gfpPingCb       = NULL;
    gbSocketInit    = 0;
}

void WINC_SOCKET_REG_CALLBACKS(tpfAppSocketCb pfAppSocketCb, tpfAppResolveCb pfAppResolveCb)
{
    gpfAppSocketCb  = pfAppSocketCb;
    gpfAppResolveCb = pfAppResolveCb;
}

void WINC_SOCKET_REG_EVENT_CALLBACK(tpfAppSocketCb pfAppSocketCb)
{
    gpfAppSocketCb = pfAppSocketCb;
}

void WINC_SOCKET_REG_RESOLVER_CALLBACK(tpfAppResolveCb pfAppResolveCb)
{
    gpfAppResolveCb = pfAppResolveCb;
}

SOCKET WINC_SOCKET_OPEN(uint16_t u16Domain, uint8_t u8Type, uint8_t u8Config)
{
    SOCKET         sock = -1;
    uint8_t        u8SockID;
    uint8_t        u8Count;
    tstrSocket     *pstrSock       = NULL;
    static uint8_t u8NextTcpSock   = 0;
    static uint8_t u8NextUdpSock   = 0;

    /* The only supported family is the AF_INET for UDP and TCP transport layer protocols. */
    if (u16Domain == AF_INET)
    {
        if (u8Type == SOCK_STREAM)
        {
            for (u8Count = 0; u8Count < TCP_SOCK_MAX; u8Count ++)
            {
                u8SockID    = u8NextTcpSock;
                pstrSock    = &gastrSockets[u8NextTcpSock];
                u8NextTcpSock = (u8NextTcpSock + 1) % TCP_SOCK_MAX;

                if (!pstrSock->bIsUsed)
                {
                    sock = (SOCKET)u8SockID;
                    memset(pstrSock, 0, sizeof(tstrSocket));

                    /* In the current implementation, SSL flags only have meaning for the TCP socket case. */
                    if (u8Config != SOCKET_CONFIG_SSL_OFF)
                    {
                        tstrSSLSocketCreateCmd  strSSLCreate;

                        strSSLCreate.sslSock = sock;
                        winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_SSL_CREATE, &strSSLCreate, sizeof(tstrSSLSocketCreateCmd));

                        pstrSock->u8SSLFlags = SSL_FLAGS_ACTIVE;

                        if (u8Config == SOCKET_CONFIG_SSL_DELAY)
                        {
                            pstrSock->u8SSLFlags |= SSL_FLAGS_DELAY;
                        }
                    }

                    break;
                }
            }
        }
        else if (u8Type == SOCK_DGRAM)
        {
            tstrSocket *pastrUDPSockets = &gastrSockets[TCP_SOCK_MAX];

            for (u8Count = 0; u8Count < UDP_SOCK_MAX; u8Count ++)
            {
                u8SockID        = u8NextUdpSock;
                pstrSock        = &pastrUDPSockets[u8NextUdpSock];
                u8NextUdpSock   = (u8NextUdpSock + 1) % UDP_SOCK_MAX;

                if (!pstrSock->bIsUsed)
                {
                    sock = (SOCKET)(u8SockID + TCP_SOCK_MAX);
                    memset(pstrSock, 0, sizeof(tstrSocket));
                    break;
                }
            }
        }
        else if (u8Type == SOCK_RAW)
        {
            /* Only raw IP packets are supported */
            if (u8Config == SOCKET_CONFIG_IPPROTO_RAW)
            {
                pstrSock = &gastrSockets[RAW_SOCK_ID];

                if (!pstrSock->bIsUsed)
                {
                    /* Socket identified by RAW_SOCK_ID is reserved */
                    sock = (SOCKET)RAW_SOCK_ID;
                    memset(pstrSock, 0, sizeof(tstrSocket));
                }
            }
        }

        if (pstrSock)
        {
            pstrSock->bIsUsed = 1;

            /* The session ID is used to distinguish different socket connections
                by comparing the assigned session ID to the one reported by the firmware*/
            ++gu16SessionID;

            if (gu16SessionID == 0)
            {
                ++gu16SessionID;
            }

            pstrSock->u16SessionID = gu16SessionID;
            WINC_LOG_INFO("Socket %d session ID = %u", sock, gu16SessionID );
        }
    }

    return sock;
}

int8_t WINC_SOCKET_BIND(SOCKET sock, struct sockaddr *pstrAddr, uint8_t u8AddrLen)
{
    tstrBindCmd         strBind;

    if ((pstrAddr == NULL) || (sock < 0) || (sock >= MAX_SOCKET) || (!gastrSockets[sock].bIsUsed) || (!u8AddrLen))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    /* Build the bind request. */
    strBind.sock = sock;
    memcpy((uint8_t *)&strBind.strAddr, (uint8_t *)pstrAddr, sizeof(tstrSockAddr));

    strBind.u16SessionID        = gastrSockets[sock].u16SessionID;

    /* Send the request. */
    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_BIND, &strBind, sizeof(tstrBindCmd)))
    {
        return SOCK_ERR_INVALID;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_LISTEN(SOCKET sock, uint8_t u8Backlog)
{
    tstrListenCmd       strListen;

    if ((sock < 0)  || (sock >= MAX_SOCKET) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    strListen.sock = sock;
    strListen.u8BackLog = u8Backlog;
    strListen.u16SessionID      = gastrSockets[sock].u16SessionID;

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_LISTEN, &strListen, sizeof(tstrListenCmd)))
    {
        return SOCK_ERR_INVALID;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_ACCEPT(SOCKET sock, struct sockaddr *pstrAddr, uint8_t *pu8AddrLen)
{
    UNUSED_VAR(pstrAddr);
    UNUSED_VAR(pu8AddrLen);

    if ((sock < 0)  || (sock >= MAX_SOCKET) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_CONNECT(SOCKET sock, struct sockaddr *pstrAddr, uint8_t u8AddrLen)
{
    tstrConnectCmd  strConnect;
    uint8_t         u8Cmd = SOCKET_CMD_CONNECT;

    if ((sock < 0)  || (sock >= MAX_SOCKET) || (pstrAddr == NULL) || (!gastrSockets[sock].bIsUsed) || (!u8AddrLen))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if ((gastrSockets[sock].u8SSLFlags) & SSL_FLAGS_ACTIVE)
    {
        u8Cmd = SOCKET_CMD_SSL_CONNECT;
        strConnect.u8SslFlags = gastrSockets[sock].u8SSLFlags;
    }

    strConnect.sock = sock;
    memcpy((uint8_t *)&strConnect.strAddr, (uint8_t *)pstrAddr, sizeof(tstrSockAddr));

    strConnect.u16SessionID     = gastrSockets[sock].u16SessionID;

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, u8Cmd, &strConnect, sizeof(tstrConnectCmd)))
    {
        return SOCK_ERR_INVALID;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_SECURE(SOCKET sock)
{
    tstrConnectCmd  strConnect = {0};

    if ((sock < 0)  || (sock >= MAX_SOCKET) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if ((SSL_FLAGS_ACTIVE | SSL_FLAGS_DELAY) != (gastrSockets[sock].u8SSLFlags & (SSL_FLAGS_ACTIVE | SSL_FLAGS_DELAY)))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if (0 == gastrSockets[sock].u16DataOffset)
    {
        return SOCK_ERR_INVALID_ARG;
    }

    gastrSockets[sock].u8SSLFlags &= ~SSL_FLAGS_DELAY;

    strConnect.u8SslFlags   = gastrSockets[sock].u8SSLFlags;
    strConnect.sock         = sock;
    strConnect.u16SessionID = gastrSockets[sock].u16SessionID;

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_SECURE, &strConnect, sizeof(tstrConnectCmd)))
    {
        return SOCK_ERR_INVALID;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_SEND(SOCKET sock, void *pvSendBuffer, uint16_t u16SendLength, uint16_t u16Flags)
{
    UNUSED_VAR(u16Flags);

    uint16_t        u16DataOffset;
    tstrSendCmd     strSend;
    uint8_t         u8Cmd;

    if ((sock < 0) || (sock >= MAX_SOCKET) || (pvSendBuffer == NULL) || (u16SendLength > SOCKET_BUFFER_MAX_LENGTH) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    u8Cmd           = SOCKET_CMD_SEND;
    u16DataOffset   = TCP_TX_PACKET_OFFSET;

    strSend.sock            = sock;
    strSend.u16DataSize     = HOST_TO_WINC_U16(u16SendLength);
    strSend.u16SessionID    = gastrSockets[sock].u16SessionID;

    if (sock >= TCP_SOCK_MAX)
    {
        u16DataOffset = UDP_TX_PACKET_OFFSET;
    }

    if (SSL_FLAGS_ACTIVE == (gastrSockets[sock].u8SSLFlags & (SSL_FLAGS_ACTIVE | SSL_FLAGS_DELAY)))
    {
        u8Cmd           = SOCKET_CMD_SSL_SEND;
        u16DataOffset   = gastrSockets[sock].u16DataOffset;
    }

    if (M2M_SUCCESS != winc_hif_send(M2M_REQ_GROUP_IP, u8Cmd | M2M_REQ_DATA_PKT, &strSend, sizeof(tstrSendCmd), pvSendBuffer, u16SendLength, u16DataOffset))
    {
        return SOCK_ERR_BUFFER_FULL;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_SENDTO(SOCKET sock, void *pvSendBuffer, uint16_t u16SendLength, uint16_t u16Flags, struct sockaddr *pstrAddr, uint8_t u8AddrLen)
{
    tstrSendCmd strSendTo;
    uint16_t u16MaxLength = (sock == RAW_SOCK_ID) ? (M2M_HIF_MAX_PACKET_SIZE - M2M_HIF_HDR_OFFSET - sizeof(tstrSendCmd)) : SOCKET_BUFFER_MAX_LENGTH;

    UNUSED_VAR(u16Flags);
    UNUSED_VAR(u8AddrLen);

    if ((sock < 0) || (sock >= MAX_SOCKET) || (pvSendBuffer == NULL) || (u16SendLength > u16MaxLength) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    memset(&strSendTo, 0, sizeof(tstrSendCmd));

    strSendTo.sock          = sock;
    strSendTo.u16DataSize   = HOST_TO_WINC_U16(u16SendLength);
    strSendTo.u16SessionID  = gastrSockets[sock].u16SessionID;

    if (pstrAddr != NULL)
    {
        struct sockaddr_in *pstrDestAddr;
        pstrDestAddr = (struct sockaddr_in *)pstrAddr;

        strSendTo.strAddr.u16Family = pstrDestAddr->sin_family;
        strSendTo.strAddr.u16Port   = pstrDestAddr->sin_port;
        strSendTo.strAddr.u32IPAddr = pstrDestAddr->sin_addr.s_addr;
    }

    if (M2M_SUCCESS != winc_hif_send(M2M_REQ_GROUP_IP, SOCKET_CMD_SENDTO | M2M_REQ_DATA_PKT, &strSendTo, sizeof(tstrSendCmd),
                                     pvSendBuffer, u16SendLength, (sock == RAW_SOCK_ID) ? IP_PACKET_OFFSET : UDP_TX_PACKET_OFFSET))
    {
        return SOCK_ERR_BUFFER_FULL;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_RECV(SOCKET sock, void *pvRecvBuf, uint16_t u16BufLen, uint32_t u32Timeoutmsec)
{
    tstrRecvCmd strRecv;
    uint8_t     u8Cmd = SOCKET_CMD_RECV;

    if ((sock < 0) || (sock >= MAX_SOCKET) || (pvRecvBuf == NULL) || (!u16BufLen) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    gastrSockets[sock].pu8UserBuffer        = (uint8_t *)pvRecvBuf;
    gastrSockets[sock].u16UserBufferSize    = u16BufLen;

    if (gastrSockets[sock].bIsRecvPending)
    {
        return SOCK_ERR_NO_ERROR;
    }

    gastrSockets[sock].bIsRecvPending = 1;

    if (SSL_FLAGS_ACTIVE != (gastrSockets[sock].u8SSLFlags & (SSL_FLAGS_ACTIVE | SSL_FLAGS_DELAY)))
    {
        u8Cmd = SOCKET_CMD_SSL_RECV;
    }

    /* Check the timeout value. */
    if (u32Timeoutmsec == 0)
    {
        strRecv.u32Timeoutmsec = 0xFFFFFFFF;
    }
    else
    {
        strRecv.u32Timeoutmsec = HOST_TO_WINC_U32(u32Timeoutmsec);
    }

    strRecv.sock = sock;
    strRecv.u16SessionID        = gastrSockets[sock].u16SessionID;
    strRecv.u16BufLen           = u16BufLen;

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, u8Cmd, &strRecv, sizeof(tstrRecvCmd)))
    {
        return SOCK_ERR_BUFFER_FULL;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_CLOSE(SOCKET sock)
{
    uint8_t u8Cmd = SOCKET_CMD_CLOSE;
    tstrCloseCmd strclose;

    WINC_LOG_INFO("Sock to delete <%d> (used %u)", sock, gastrSockets[sock].bIsUsed);

    if ((sock < 0) || (sock >= MAX_SOCKET) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    strclose.sock = sock;
    strclose.u16SessionID       = gastrSockets[sock].u16SessionID;

    if (gastrSockets[sock].u8SSLFlags & SSL_FLAGS_ACTIVE)
    {
        u8Cmd = SOCKET_CMD_SSL_CLOSE;
    }

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, u8Cmd, &strclose, sizeof(tstrCloseCmd)))
    {
        return SOCK_ERR_INVALID;
    }

    memset(&gastrSockets[sock], 0, sizeof(tstrSocket));

    return SOCK_ERR_NO_ERROR;
}

int16_t WINC_SOCKET_RECVFROM(SOCKET sock, void *pvRecvBuf, uint16_t u16BufLen, uint32_t u32Timeoutmsec)
{
    tstrRecvCmd strRecv;

    if ((sock < 0) || (sock >= MAX_SOCKET) || (pvRecvBuf == NULL) || (!u16BufLen) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    gastrSockets[sock].pu8UserBuffer = (uint8_t *)pvRecvBuf;
    gastrSockets[sock].u16UserBufferSize = u16BufLen;

    if (gastrSockets[sock].bIsRecvPending)
    {
        return SOCK_ERR_NO_ERROR;
    }

    gastrSockets[sock].bIsRecvPending = 1;

    /* Check the timeout value. */
    if (u32Timeoutmsec == 0)
    {
        strRecv.u32Timeoutmsec = 0xFFFFFFFF;
    }
    else
    {
        strRecv.u32Timeoutmsec = HOST_TO_WINC_U32(u32Timeoutmsec);
    }

    strRecv.sock = sock;
    strRecv.u16SessionID        = gastrSockets[sock].u16SessionID;
    strRecv.u16BufLen           = u16BufLen;

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_RECVFROM, &strRecv, sizeof(tstrRecvCmd)))
    {
        return SOCK_ERR_BUFFER_FULL;
    }

    return SOCK_ERR_NO_ERROR;
}

in_addr_t WINC_SOCKET_INET_ADDR(const char *cp)
{
    uint8_t i, l;
    uint16_t t;
    uint32_t ip;
    char c;

    ip = 0;

    for (i = 0; i < 4; i++)
    {
        t = 0;
        ip >>= 8;

        // Count non-delimiter or terminator characters

        for (l = 0; l < 4; l++)
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

        while (l--)
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

const char *WINC_SOCKET_INET_NTOP(int af, const void *src, char *dst, size_t size)
{
    uint8_t i, v, t, c, n;
    char *rp = dst;
    uint32_t ip = ((struct in_addr *)src)->s_addr;

    UNUSED_VAR(af);

    if ((NULL == src) || (NULL == dst) || (size < INET_ADDRSTRLEN))
    {
        return NULL;
    }

    for (i = 0; i < 4; i++)
    {
        t = ip & 0xff;
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

int8_t WINC_SOCKET_GETHOSTBYNAME(const char *pcHostName)
{
    uint8_t u8HostNameSize = (uint8_t)strlen(pcHostName);

    if (u8HostNameSize > HOSTNAME_MAX_SIZE)
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_DNS_RESOLVE | M2M_REQ_DATA_PKT, pcHostName, u8HostNameSize + 1))
    {
        return SOCK_ERR_INVALID;
    }

    return SOCK_ERR_NO_ERROR;
}

static int8_t rawSetSockOpt(SOCKET sock, uint8_t u8Opt, const void *pvOptVal, uint16_t u16OptLen)
{
    tstrSetSocketOptCmd strSetSockOpt;

    if ((RAW_SOCK_ID != sock) || (u16OptLen != sizeof(uint32_t)))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    strSetSockOpt.u8Option       = u8Opt;
    strSetSockOpt.sock           = sock;
    strSetSockOpt.u32OptionValue = *(uint32_t *)pvOptVal;
    strSetSockOpt.u16SessionID   = gastrSockets[sock].u16SessionID;

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_RAW_SET_SOCK_OPT, &strSetSockOpt, sizeof(tstrSetSocketOptCmd)))
    {
        return SOCK_ERR_INVALID;
    }

    return SOCK_ERR_NO_ERROR;
}

static int8_t sslSetSockOpt(SOCKET sock, uint8_t u8Opt, const void *pvOptVal, uint16_t u16OptLen)
{
    uint8_t sslFlag = 0;

    if (sock >= TCP_SOCK_MAX)
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if (!(gastrSockets[sock].u8SSLFlags & SSL_FLAGS_ACTIVE))
    {
        WINC_LOG_ERROR("Not SSL Socket");
        return SOCK_ERR_INVALID_ARG;
    }

    if (u16OptLen == sizeof(int))
    {
        if (u8Opt == SO_SSL_BYPASS_X509_VERIF)
        {
            sslFlag = SSL_FLAGS_BYPASS_X509;
        }
        else if (u8Opt == SO_SSL_ENABLE_SESSION_CACHING)
        {
            sslFlag = SSL_FLAGS_CACHE_SESSION;
        }
        else if (u8Opt == SO_SSL_ENABLE_CERTNAME_VALIDATION)
        {
            sslFlag = SSL_FLAGS_CHECK_CERTNAME;
        }

        if (sslFlag)
        {
            int optVal = *((int *)pvOptVal);

            if (optVal)
            {
                gastrSockets[sock].u8SSLFlags |= sslFlag;
            }
            else
            {
                gastrSockets[sock].u8SSLFlags &= ~sslFlag;
            }
        }
    }
    else if (   ((u8Opt == SO_SSL_SNI) && (u16OptLen < HOSTNAME_MAX_SIZE))
                || ((u8Opt == SO_SSL_ALPN) && (u16OptLen <= ALPN_LIST_MAX_SIZE))
            )
    {
        tstrSSLSetSockOptCmd    strCmd = {0};

        strCmd.sock         = sock;
        strCmd.u16SessionID = gastrSockets[sock].u16SessionID;
        strCmd.u8Option     = u8Opt;
        strCmd.u32OptLen    = u16OptLen;
        memcpy(strCmd.au8OptVal, (uint8_t *)pvOptVal, u16OptLen);

        if (M2M_ERR_MEM_ALLOC == winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_SSL_SET_SOCK_OPT, &strCmd, sizeof(tstrSSLSetSockOptCmd)))
        {
            if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_SSL_SET_SOCK_OPT | M2M_REQ_DATA_PKT,
                    &strCmd, sizeof(tstrSSLSetSockOptCmd)))
            {
                return SOCK_ERR_BUFFER_FULL;
            }
        }

        return SOCK_ERR_NO_ERROR;
    }
    else
    {
        WINC_LOG_ERROR("Unknown SSL Socket Option %u", u8Opt);
    }

    return SOCK_ERR_INVALID_ARG;
}

int8_t WINC_SOCKET_SETSOCKOPT(SOCKET sock, uint8_t u8Level, uint8_t option_name,
                  const void *option_value, uint16_t u16OptionLen)
{
    if ((sock < 0) || (sock >= MAX_SOCKET) || (option_value == NULL) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if (u8Level == SOL_RAW)
    {
        return rawSetSockOpt(sock, option_name, option_value, u16OptionLen);
    }
    else if (u8Level == SOL_SSL_SOCKET)
    {
        return sslSetSockOpt(sock, option_name, option_value, u16OptionLen);
    }
    else if (u8Level == SOL_SOCKET)
    {
        if (u16OptionLen == sizeof(uint32_t))
        {
            uint8_t u8Cmd = SOCKET_CMD_SET_SOCKET_OPTION;
            tstrSetSocketOptCmd strSetSockOpt;
            strSetSockOpt.u8Option = option_name;
            strSetSockOpt.sock = sock;
            strSetSockOpt.u32OptionValue = *(uint32_t *)option_value;
            strSetSockOpt.u16SessionID      = gastrSockets[sock].u16SessionID;

            if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, u8Cmd, &strSetSockOpt, sizeof(tstrSetSocketOptCmd)))
            {
                return SOCK_ERR_INVALID;
            }
        }
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_GETSOCKOPT(SOCKET sock, uint8_t u8Level, uint8_t u8OptName, const void *pvOptValue, uint8_t *pu8OptLen)
{
    UNUSED_VAR(sock);
    UNUSED_VAR(u8Level);
    UNUSED_VAR(u8OptName);
    UNUSED_VAR(pvOptValue);
    UNUSED_VAR(pu8OptLen);

    /* TBD */
    return SOCK_ERR_INVALID_ARG;
}

int8_t WINC_SOCKET_ICMP_ECHO_REQ(uint32_t u32DstIP, uint8_t u8TTL, tpfPingCb fpPingCb)
{
    tstrPingCmd strPingCmd;

    if ((!u32DstIP) || (fpPingCb == NULL))
    {
        return M2M_ERR_INVALID_ARG;
    }

    strPingCmd.u16PingCount     = 1;
    strPingCmd.u32DestIPAddr    = u32DstIP;
    strPingCmd.u32CmdPrivate    = ++gu32PingId;
    strPingCmd.u8TTL            = u8TTL;

    gfpPingCb = fpPingCb;

    if (M2M_SUCCESS != winc_hif_send_no_data(M2M_REQ_GROUP_IP, SOCKET_CMD_PING, &strPingCmd, sizeof(tstrPingCmd)))
    {
        return SOCK_ERR_INVALID;
    }

    return SOCK_ERR_NO_ERROR;
}

int8_t WINC_SOCKET_SET_ALPN_LIST(SOCKET sock, const char *pcProtocolList)
{
    size_t length = strlen(pcProtocolList);

    if ((sock < 0) || (sock >= MAX_SOCKET) || (pcProtocolList == NULL))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if ((length == 0) || (length >= ALPN_LIST_MAX_APP_LENGTH))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    /*
        ALPN socket option requires Alpn list in this format:
         0       1       2       3 ... (bytes)
        +-------+-------+-------+  ...        +-------+  ...        +-------+  ...
        | Length L (BE) | len1  | name1...    | len2  | name2...    | len3  | name3...
        +-------+-------+-------+  ...        +-------+  ...        +-------+  ...
        Length fields do not include themselves.
    */
    uint8_t au8AlpnList[ALPN_LIST_MAX_SIZE] = {0};
    uint8_t *pu8Ptr = &au8AlpnList[3] + length;
    uint8_t u8Len = 0;

    memcpy(&au8AlpnList[3], pcProtocolList, length);
    length++;
    au8AlpnList[1] = (uint8_t)length;
    au8AlpnList[2] = ' ';

    /* Convert space characters into length fields. */
    while (length--)
    {
        if (*--pu8Ptr == ' ')
        {
            if (u8Len == 0)
            {
                return SOCK_ERR_INVALID_ARG;
            }

            *pu8Ptr = u8Len;
            u8Len = 0;
        }
        else
        {
            u8Len++;
        }
    }

    return setsockopt(sock, SOL_SSL_SOCKET, SO_SSL_ALPN, au8AlpnList, sizeof(au8AlpnList));
}

int8_t WINC_SOCKET_GET_ALPN_INDEX(SOCKET sock)
{
    if ((sock < 0) || (sock >= MAX_SOCKET))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if (!(gastrSockets[sock].u8SSLFlags & SSL_FLAGS_ACTIVE) || (!gastrSockets[sock].bIsUsed))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    return (int8_t)gastrSockets[sock].u8AlpnStatus;
}

int8_t WINC_SOCKET_GET_ERROR_DETAIL(SOCKET sock, tstrSockErr *pstrErr)
{
    if ((sock < 0) || (sock >= MAX_SOCKET) || (pstrErr == NULL))
    {
        return SOCK_ERR_INVALID_ARG;
    }

    if (!gastrSockets[sock].bIsUsed)
    {
        return SOCK_ERR_INVALID_ARG;
    }

    pstrErr->enuErrSource = gastrSockets[sock].u8ErrSource;
    pstrErr->u8ErrCode    = gastrSockets[sock].u8ErrCode;

    return SOCK_ERR_NO_ERROR;
}

uint8_t WINC_SOCKET_ISREADY(void)
{
    return gbSocketInit;
}