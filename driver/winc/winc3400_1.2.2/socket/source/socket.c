/*******************************************************************************
  File Name:
    socket.c

  Summary:
    WINC3400 BSD Compatible Socket Interface

  Description:
    WINC3400 BSD Compatible Socket Interface
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "nm_bsp.h"
#include "socket.h"
#include "m2m_hif.h"
#include "m2m_socket_host_if.h"

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

#define SOCKET_REQUEST(reqID, reqArgs, reqSize, reqPayload, reqPayloadSize, reqPayloadOffset)       \
    hif_send(M2M_REQ_GROUP_IP, reqID, reqArgs, reqSize, reqPayload, reqPayloadSize, reqPayloadOffset)


#define SSL_FLAGS_ACTIVE                    NBIT0
#define SSL_FLAGS_BYPASS_X509               NBIT1
#define SSL_FLAGS_CACHE_SESSION             NBIT4
#define SSL_FLAGS_CHECK_CERTNAME            NBIT6

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
PRIVATE DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


/*!
*  @brief
*/
typedef struct{
    SOCKET      sock;
    uint8_t     u8Dummy;
    uint16_t    u16SessionID;
}tstrCloseCmd;


/*!
*  @brief
*/
typedef struct{
    uint8_t             *pu8UserBuffer;
    uint16_t            u16UserBufferSize;
    uint16_t            u16SessionID;
    uint16_t            u16DataOffset;
    uint8_t             bIsUsed;
    uint8_t             u8SSLFlags;
    uint8_t             bIsRecvPending;
}tstrSocket;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
GLOBALS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

volatile tstrSocket             gastrSockets[MAX_SOCKET];
volatile uint16_t               gu16SessionID = 0;

volatile tpfAppSocketCb         gpfAppSocketCb;
volatile tpfAppResolveCb        gpfAppResolveCb;
volatile uint8_t                gbSocketInit = 0;

/*********************************************************************
Function
        Socket_ReadSocketData

Description
        Callback function used by the WINC3400 driver to deliver messages
        for socket layer.

Return
        None.
*********************************************************************/
static void Socket_ReadSocketData(SOCKET sock, tstrSocketRecvMsg *pstrRecv,uint8_t u8SocketMsg,
                                  uint32_t u32StartAddress,uint16_t u16ReadCount)
{
    uint32_t    u32Address = u32StartAddress;
    uint16_t    u16Read;
    int16_t     s16Diff;

    pstrRecv->u16RemainingSize = u16ReadCount;
    if((u16ReadCount > 0) && (gastrSockets[sock].pu8UserBuffer != NULL) && (gastrSockets[sock].u16UserBufferSize > 0) && (gastrSockets[sock].bIsUsed == 1))
    {
        u16Read = u16ReadCount;
        s16Diff = u16Read - gastrSockets[sock].u16UserBufferSize;
        if(s16Diff > 0)
            u16Read = gastrSockets[sock].u16UserBufferSize;

        if(hif_receive(u32Address, gastrSockets[sock].pu8UserBuffer, u16Read, 1) == M2M_SUCCESS)
        {
            pstrRecv->pu8Buffer         = gastrSockets[sock].pu8UserBuffer;
            pstrRecv->s16BufferSize     = u16Read;
            pstrRecv->u16RemainingSize  -= u16Read;

            gastrSockets[sock].u16UserBufferSize -= u16Read;
            if(gastrSockets[sock].u16UserBufferSize == 0)
                gastrSockets[sock].pu8UserBuffer = NULL;

            if (gpfAppSocketCb)
                gpfAppSocketCb(sock,u8SocketMsg, pstrRecv);
        }
        else
        {
            M2M_INFO("(ERRR)Current <%d>\r\n", u16ReadCount);
        }
    }
}
/*********************************************************************
Function
        m2m_ip_cb

Description
        Callback function used by the NMC1000 driver to deliver messages
        for socket layer.

Return
        None.
*********************************************************************/
static void m2m_ip_cb(uint8_t u8OpCode, uint16_t u16BufferSize,uint32_t u32Address)
{
    if(u8OpCode == SOCKET_CMD_BIND)
    {
        tstrBindReply       strBindReply;
        tstrSocketBindMsg   strBind;

        if(hif_receive(u32Address, (uint8_t*)&strBindReply, sizeof(tstrBindReply), 0) == M2M_SUCCESS)
        {
            strBind.status = strBindReply.s8Status;
            if(gpfAppSocketCb)
                gpfAppSocketCb(strBindReply.sock,SOCKET_MSG_BIND,&strBind);
        }
    }
    else if(u8OpCode == SOCKET_CMD_LISTEN)
    {
        tstrListenReply         strListenReply;
        tstrSocketListenMsg     strListen;
        if(hif_receive(u32Address, (uint8_t*)&strListenReply, sizeof(tstrListenReply), 0) == M2M_SUCCESS)
        {
            strListen.status = strListenReply.s8Status;
            if(gpfAppSocketCb)
                gpfAppSocketCb(strListenReply.sock,SOCKET_MSG_LISTEN, &strListen);
        }
    }
    else if(u8OpCode == SOCKET_CMD_ACCEPT)
    {
        tstrAcceptReply         strAcceptReply;
        tstrSocketAcceptMsg     strAccept;
        if(hif_receive(u32Address, (uint8_t*)&strAcceptReply, sizeof(tstrAcceptReply), 0) == M2M_SUCCESS)
        {
            if(strAcceptReply.sConnectedSock >= 0)
            {
                gastrSockets[strAcceptReply.sConnectedSock].u8SSLFlags  = 0;
                gastrSockets[strAcceptReply.sConnectedSock].bIsUsed     = 1;

                /* The session ID is used to distinguish different socket connections
                    by comparing the assigned session ID to the one reported by the firmware*/
                ++gu16SessionID;
                if(gu16SessionID == 0)
                    ++gu16SessionID;

                gastrSockets[strAcceptReply.sConnectedSock].u16SessionID = gu16SessionID;
                M2M_DBG("Socket %d session ID = %d\r\n",strAcceptReply.sConnectedSock , gu16SessionID );
            }
            strAccept.sock = strAcceptReply.sConnectedSock;
            strAccept.strAddr.sin_family        = AF_INET;
            strAccept.strAddr.sin_port = strAcceptReply.strAddr.u16Port;
            strAccept.strAddr.sin_addr.s_addr = strAcceptReply.strAddr.u32IPAddr;
            if(gpfAppSocketCb)
                gpfAppSocketCb(strAcceptReply.sListenSock, SOCKET_MSG_ACCEPT, &strAccept);
        }
    }
    else if((u8OpCode == SOCKET_CMD_CONNECT) || (u8OpCode == SOCKET_CMD_SSL_CONNECT))
    {
        tstrConnectReply        strConnectReply;
        tstrSocketConnectMsg    strConnMsg;
        if(hif_receive(u32Address, (uint8_t*)&strConnectReply, sizeof(tstrConnectReply), 0) == M2M_SUCCESS)
        {
            strConnMsg.sock     = strConnectReply.sock;
            strConnMsg.s8Error  = strConnectReply.s8Error;
            if(strConnectReply.s8Error == SOCK_ERR_NO_ERROR)
            {
                gastrSockets[strConnectReply.sock].u16DataOffset = strConnectReply.u16AppDataOffset - M2M_HIF_HDR_OFFSET;
            }
            if(gpfAppSocketCb)
                gpfAppSocketCb(strConnectReply.sock,SOCKET_MSG_CONNECT, &strConnMsg);
        }
    }
    else if(u8OpCode == SOCKET_CMD_DNS_RESOLVE)
    {
        tstrDnsReply    strDnsReply;
        if(hif_receive(u32Address, (uint8_t*)&strDnsReply, sizeof(tstrDnsReply), 0) == M2M_SUCCESS)
        {
            strDnsReply.u32HostIP = strDnsReply.u32HostIP;
            if(gpfAppResolveCb)
                gpfAppResolveCb((uint8_t*)strDnsReply.acHostName, strDnsReply.u32HostIP);
        }
    }
    else if((u8OpCode == SOCKET_CMD_RECV) || (u8OpCode == SOCKET_CMD_RECVFROM) || (u8OpCode == SOCKET_CMD_SSL_RECV))
    {
        SOCKET              sock;
        int16_t             s16RecvStatus;
        tstrRecvReply       strRecvReply;
        uint16_t            u16ReadSize;
        tstrSocketRecvMsg   strRecvMsg;
        uint8_t             u8CallbackMsgID = SOCKET_MSG_RECV;
        uint16_t            u16DataOffset;

        if(u8OpCode == SOCKET_CMD_RECVFROM)
            u8CallbackMsgID = SOCKET_MSG_RECVFROM;

        /* Read RECV REPLY data structure.
        */
        u16ReadSize = sizeof(tstrRecvReply);
        if(hif_receive(u32Address, (uint8_t*)&strRecvReply, u16ReadSize, 0) == M2M_SUCCESS)
        {
            uint16_t u16SessionID = 0;

            sock            = strRecvReply.sock;
            u16SessionID = strRecvReply.u16SessionID;
            M2M_DBG("recv callback session ID = %d\r\n",u16SessionID);

            /* Reset the Socket RX Pending Flag.
            */
            gastrSockets[sock].bIsRecvPending = 0;

            s16RecvStatus   = NM_BSP_B_L_16(strRecvReply.s16RecvStatus);
            u16DataOffset   = NM_BSP_B_L_16(strRecvReply.u16DataOffset);
            strRecvMsg.strRemoteAddr.sin_port           = strRecvReply.strRemoteAddr.u16Port;
            strRecvMsg.strRemoteAddr.sin_addr.s_addr    = strRecvReply.strRemoteAddr.u32IPAddr;

            if(u16SessionID == gastrSockets[sock].u16SessionID)
            {
                if((s16RecvStatus > 0) && (s16RecvStatus < u16BufferSize))
                {
                    /* Skip incoming bytes until reaching the Start of Application Data.
                    */
                    u32Address += u16DataOffset;

                    /* Read the Application data and deliver it to the application callback in
                    the given application buffer. If the buffer is smaller than the received data,
                    the data is passed to the application in chunks according to its buffer size.
                    */
                    u16ReadSize = (uint16_t)s16RecvStatus;
                    Socket_ReadSocketData(sock, &strRecvMsg, u8CallbackMsgID, u32Address, u16ReadSize);
                }
                else
                {
                    /* Don't tidy up here. Application must call shutdown().
                    */
                    strRecvMsg.s16BufferSize    = s16RecvStatus;
                    strRecvMsg.pu8Buffer        = NULL;
                    if(gpfAppSocketCb)
                        gpfAppSocketCb(sock,u8CallbackMsgID, &strRecvMsg);
                }
            }
            else
            {
                M2M_DBG("Discard recv callback %d %d\r\n",u16SessionID , gastrSockets[sock].u16SessionID);
                if(u16ReadSize < u16BufferSize)
                    hif_receive(0, NULL, 0, 1);
            }
        }
    }
    else if((u8OpCode == SOCKET_CMD_SEND) || (u8OpCode == SOCKET_CMD_SENDTO) || (u8OpCode == SOCKET_CMD_SSL_SEND))
    {
        SOCKET          sock;
        int16_t         s16Rcvd;
        tstrSendReply   strReply;
        uint8_t         u8CallbackMsgID = SOCKET_MSG_SEND;

        if(u8OpCode == SOCKET_CMD_SENDTO)
            u8CallbackMsgID = SOCKET_MSG_SENDTO;

        if(hif_receive(u32Address, (uint8_t*)&strReply, sizeof(tstrSendReply), 0) == M2M_SUCCESS)
        {
            uint16_t u16SessionID = 0;

            sock = strReply.sock;
            u16SessionID = strReply.u16SessionID;
            M2M_DBG("send callback session ID = %d\r\n",u16SessionID);

            s16Rcvd = NM_BSP_B_L_16(strReply.s16SentBytes);

            if(u16SessionID == gastrSockets[sock].u16SessionID)
            {
                if(gpfAppSocketCb)
                    gpfAppSocketCb(sock,u8CallbackMsgID, &s16Rcvd);
            }
            else
            {
                M2M_DBG("Discard send callback %d %d\r\n",u16SessionID , gastrSockets[sock].u16SessionID);
            }
        }
    }
    else if(u8OpCode == SOCKET_CMD_PING)
    {
        tstrPingReply   strPingReply;
        if(hif_receive(u32Address, (uint8_t*)&strPingReply, sizeof(tstrPingReply), 1) == M2M_SUCCESS)
        {
            tpfPingCb fpPingCb;

            fpPingCb = (void (*)(uint32_t , uint32_t , uint8_t))strPingReply.u32CmdPrivate;
            if(fpPingCb != NULL)
            {
                fpPingCb(strPingReply.u32IPAddr, strPingReply.u32RTT, strPingReply.u8ErrorCode);
            }
        }
    }
}
/*********************************************************************
Function
        socketInit

Description

Return
        None.
*********************************************************************/
void socketInit(void)
{
    if(gbSocketInit==0)
    {
        memset((uint8_t*)gastrSockets, 0, MAX_SOCKET * sizeof(tstrSocket));
        hif_register_cb(M2M_REQ_GROUP_IP,m2m_ip_cb);
        gbSocketInit=1;
        gu16SessionID = 0;
    }
}

/*********************************************************************
Function
        socketDeinit

Description

Return
        None.
*********************************************************************/
void socketDeinit(void)
{
    memset((uint8_t*)gastrSockets, 0, MAX_SOCKET * sizeof(tstrSocket));
    hif_register_cb(M2M_REQ_GROUP_IP, NULL);
    gpfAppSocketCb = NULL;
    gpfAppResolveCb = NULL;
    gbSocketInit = 0;
}


/*********************************************************************
Function
        registerSocketCallback

Description

Return
        None.
*********************************************************************/
void registerSocketCallback(tpfAppSocketCb pfAppSocketCb, tpfAppResolveCb pfAppResolveCb)
{
    gpfAppSocketCb = pfAppSocketCb;
    gpfAppResolveCb = pfAppResolveCb;
}
void registerSocketEventCallback(tpfAppSocketCb socket_cb)
{
    gpfAppSocketCb = socket_cb;
}
void registerSocketResolveCallback(tpfAppResolveCb resolve_cb)
{
    gpfAppResolveCb = resolve_cb;
}

/*********************************************************************
Function
        socket

Description
        Creates a socket.

Return
        - Negative value for error.
        - ZERO or positive value as a socket ID if successful.
*********************************************************************/
SOCKET socket(uint16_t u16Domain, uint8_t u8Type, uint8_t u8Flags)
{
    SOCKET                  sock = -1;
    uint8_t                 u8SockID;
    uint8_t                 u8Count;
    volatile tstrSocket     *pstrSock       = NULL;
    static volatile uint8_t u8NextTcpSock = 0;
    static volatile uint8_t u8NextUdpSock = 0;

    /* The only supported family is the AF_INET for UDP and TCP transport layer protocols. */
    if(u16Domain == AF_INET)
    {
        if(u8Type == SOCK_STREAM)
        {
            for(u8Count = 0; u8Count < TCP_SOCK_MAX; u8Count ++)
            {
                u8SockID    = u8NextTcpSock;
                pstrSock    = &gastrSockets[u8NextTcpSock];
                u8NextTcpSock = (u8NextTcpSock + 1) % TCP_SOCK_MAX;
                if(!pstrSock->bIsUsed)
                {
                    sock = (SOCKET)u8SockID;
                    break;
                }
            }
        }
        else if(u8Type == SOCK_DGRAM)
        {
            volatile tstrSocket *pastrUDPSockets = &gastrSockets[TCP_SOCK_MAX];
            for(u8Count = 0; u8Count < UDP_SOCK_MAX; u8Count ++)
            {
                u8SockID        = u8NextUdpSock;
                pstrSock        = &pastrUDPSockets[u8NextUdpSock];
                u8NextUdpSock   = (u8NextUdpSock + 1) % UDP_SOCK_MAX;
                if(!pstrSock->bIsUsed)
                {
                    sock = (SOCKET)(u8SockID + TCP_SOCK_MAX);
                    break;
                }
            }
        }

        if(sock >= 0)
        {
            memset((uint8_t*)pstrSock, 0, sizeof(tstrSocket));
            pstrSock->bIsUsed = 1;

            /* The session ID is used to distinguish different socket connections
                by comparing the assigned session ID to the one reported by the firmware*/
            ++gu16SessionID;
            if(gu16SessionID == 0)
                ++gu16SessionID;

            pstrSock->u16SessionID = gu16SessionID;
            M2M_INFO("Socket %d session ID = %d\r\n",sock, gu16SessionID );

            if(u8Flags & SOCKET_FLAGS_SSL)
            {
                tstrSSLSocketCreateCmd  strSSLCreate;
                strSSLCreate.sslSock = sock;
                pstrSock->u8SSLFlags = SSL_FLAGS_ACTIVE;
                SOCKET_REQUEST(SOCKET_CMD_SSL_CREATE, (uint8_t*)&strSSLCreate, sizeof(tstrSSLSocketCreateCmd), 0, 0, 0);
            }
        }
    }
    return sock;
}

/*********************************************************************
Function
        bind

Description
        Request to bind a socket on a local address.

Return
*********************************************************************/
int8_t bind(SOCKET sock, struct sockaddr *pstrAddr, uint8_t u8AddrLen)
{
    int8_t s8Ret = SOCK_ERR_INVALID_ARG;
    if((pstrAddr != NULL) && (sock >= 0) && (gastrSockets[sock].bIsUsed == 1) && (u8AddrLen != 0))
    {
        tstrBindCmd         strBind;

        /* Build the bind request. */
        strBind.sock = sock;
        memcpy((uint8_t *)&strBind.strAddr, (uint8_t *)pstrAddr, sizeof(tstrSockAddr));

        strBind.strAddr.u16Family   = strBind.strAddr.u16Family;
        strBind.strAddr.u16Port     = strBind.strAddr.u16Port;
        strBind.strAddr.u32IPAddr   = strBind.strAddr.u32IPAddr;
        strBind.u16SessionID        = gastrSockets[sock].u16SessionID;

        /* Send the request. */
        s8Ret = SOCKET_REQUEST(SOCKET_CMD_BIND, (uint8_t*)&strBind,sizeof(tstrBindCmd) , NULL , 0, 0);
        if(s8Ret != SOCK_ERR_NO_ERROR)
        {
            s8Ret = SOCK_ERR_INVALID;
        }
    }
    return s8Ret;
}
/*********************************************************************
Function
        listen

Description


Return
*********************************************************************/
int8_t listen(SOCKET sock, uint8_t backlog)
{
    int8_t s8Ret = SOCK_ERR_INVALID_ARG;

    if(sock >= 0 && (gastrSockets[sock].bIsUsed == 1))
    {
        tstrListenCmd       strListen;

        strListen.sock = sock;
        strListen.u8BackLog = backlog;
        strListen.u16SessionID      = gastrSockets[sock].u16SessionID;

        s8Ret = SOCKET_REQUEST(SOCKET_CMD_LISTEN, (uint8_t*)&strListen, sizeof(tstrListenCmd), NULL, 0, 0);
        if(s8Ret != SOCK_ERR_NO_ERROR)
        {
            s8Ret = SOCK_ERR_INVALID;
        }
    }
    return s8Ret;
}
/*********************************************************************
Function
        accept

Description

Return
*********************************************************************/
int8_t accept(SOCKET sock, struct sockaddr *addr, uint8_t *addrlen)
{
    int8_t s8Ret = SOCK_ERR_INVALID_ARG;

    if(sock >= 0 && (gastrSockets[sock].bIsUsed == 1) )
    {
        s8Ret = SOCK_ERR_NO_ERROR;
    }
    return s8Ret;
}
/*********************************************************************
Function
        connect

Description
        Connect to a remote TCP Server.

Return
*********************************************************************/
int8_t connect(SOCKET sock, struct sockaddr *pstrAddr, uint8_t u8AddrLen)
{
    int8_t s8Ret = SOCK_ERR_INVALID_ARG;
    if((sock >= 0) && (pstrAddr != NULL) && (gastrSockets[sock].bIsUsed == 1) && (u8AddrLen != 0))
    {
        tstrConnectCmd  strConnect;
        uint8_t         u8Cmd = SOCKET_CMD_CONNECT;
        if((gastrSockets[sock].u8SSLFlags) & SSL_FLAGS_ACTIVE)
        {
            u8Cmd = SOCKET_CMD_SSL_CONNECT;
            strConnect.u8SslFlags = gastrSockets[sock].u8SSLFlags;
        }
        strConnect.sock = sock;
        memcpy((uint8_t *)&strConnect.strAddr, (uint8_t *)pstrAddr, sizeof(tstrSockAddr));

        strConnect.u16SessionID     = gastrSockets[sock].u16SessionID;
        s8Ret = SOCKET_REQUEST(u8Cmd, (uint8_t*)&strConnect,sizeof(tstrConnectCmd), NULL, 0, 0);
        if(s8Ret != SOCK_ERR_NO_ERROR)
        {
            s8Ret = SOCK_ERR_INVALID;
        }
    }
    return s8Ret;
}
/*********************************************************************
Function
        send

Description

Return
*********************************************************************/
int16_t send(SOCKET sock, void *pvSendBuffer, uint16_t u16SendLength, uint16_t flags)
{
    int16_t s16Ret = SOCK_ERR_INVALID_ARG;

    if((sock >= 0) && (pvSendBuffer != NULL) && (u16SendLength <= SOCKET_BUFFER_MAX_LENGTH) && (gastrSockets[sock].bIsUsed == 1))
    {
        uint16_t        u16DataOffset;
        tstrSendCmd     strSend;
        uint8_t         u8Cmd;

        u8Cmd           = SOCKET_CMD_SEND;
        u16DataOffset   = TCP_TX_PACKET_OFFSET;

        strSend.sock            = sock;
        strSend.u16DataSize     = NM_BSP_B_L_16(u16SendLength);
        strSend.u16SessionID    = gastrSockets[sock].u16SessionID;

        if(sock >= TCP_SOCK_MAX)
        {
            u16DataOffset = UDP_TX_PACKET_OFFSET;
        }
        if(gastrSockets[sock].u8SSLFlags & SSL_FLAGS_ACTIVE)
        {
            u8Cmd           = SOCKET_CMD_SSL_SEND;
            u16DataOffset   = gastrSockets[sock].u16DataOffset;
        }

        s16Ret =  SOCKET_REQUEST(u8Cmd|M2M_REQ_DATA_PKT, (uint8_t*)&strSend, sizeof(tstrSendCmd), pvSendBuffer, u16SendLength, u16DataOffset);
        if(s16Ret != SOCK_ERR_NO_ERROR)
        {
            s16Ret = SOCK_ERR_BUFFER_FULL;
        }
    }
    return s16Ret;
}
/*********************************************************************
Function
        sendto

Description

Return
*********************************************************************/
int16_t sendto(SOCKET sock, void *pvSendBuffer, uint16_t u16SendLength, uint16_t flags, struct sockaddr *pstrDestAddr, uint8_t u8AddrLen)
{
    int16_t s16Ret = SOCK_ERR_INVALID_ARG;

    if((sock >= 0) && (pvSendBuffer != NULL) && (u16SendLength <= SOCKET_BUFFER_MAX_LENGTH) && (gastrSockets[sock].bIsUsed == 1))
    {
        if(gastrSockets[sock].bIsUsed)
        {
            tstrSendCmd strSendTo;

            memset((uint8_t*)&strSendTo, 0, sizeof(tstrSendCmd));

            strSendTo.sock          = sock;
            strSendTo.u16DataSize   = NM_BSP_B_L_16(u16SendLength);
            strSendTo.u16SessionID  = gastrSockets[sock].u16SessionID;

            if(pstrDestAddr != NULL)
            {
                struct sockaddr_in  *pstrAddr;
                pstrAddr = (void*)pstrDestAddr;

                strSendTo.strAddr.u16Family = pstrAddr->sin_family;
                strSendTo.strAddr.u16Port   = pstrAddr->sin_port;
                strSendTo.strAddr.u32IPAddr = pstrAddr->sin_addr.s_addr;
            }
            s16Ret = SOCKET_REQUEST(SOCKET_CMD_SENDTO|M2M_REQ_DATA_PKT, (uint8_t*)&strSendTo,  sizeof(tstrSendCmd),
                pvSendBuffer, u16SendLength, UDP_TX_PACKET_OFFSET);

            if(s16Ret != SOCK_ERR_NO_ERROR)
            {
                s16Ret = SOCK_ERR_BUFFER_FULL;
            }
        }
    }
    return s16Ret;
}
/*********************************************************************
Function
        recv

Description

Return

*********************************************************************/
int16_t recv(SOCKET sock, void *pvRecvBuf, uint16_t u16BufLen, uint32_t u32Timeoutmsec)
{
    int16_t s16Ret = SOCK_ERR_INVALID_ARG;

    if((sock >= 0) && (pvRecvBuf != NULL) && (u16BufLen != 0) && (gastrSockets[sock].bIsUsed == 1))
    {
        s16Ret = SOCK_ERR_NO_ERROR;
        gastrSockets[sock].pu8UserBuffer        = (uint8_t*)pvRecvBuf;
        gastrSockets[sock].u16UserBufferSize    = u16BufLen;

        if(!gastrSockets[sock].bIsRecvPending)
        {
            tstrRecvCmd strRecv;
            uint8_t     u8Cmd = SOCKET_CMD_RECV;

            gastrSockets[sock].bIsRecvPending = 1;
            if(gastrSockets[sock].u8SSLFlags & SSL_FLAGS_ACTIVE)
            {
                u8Cmd = SOCKET_CMD_SSL_RECV;
            }

            /* Check the timeout value. */
            if(u32Timeoutmsec == 0)
                strRecv.u32Timeoutmsec = 0xFFFFFFFF;
            else
                strRecv.u32Timeoutmsec = NM_BSP_B_L_32(u32Timeoutmsec);
            strRecv.sock = sock;
            strRecv.u16SessionID        = gastrSockets[sock].u16SessionID;
            strRecv.u16BufLen           = u16BufLen;

            s16Ret = SOCKET_REQUEST(u8Cmd, (uint8_t*)&strRecv, sizeof(tstrRecvCmd), NULL , 0, 0);
            if(s16Ret != SOCK_ERR_NO_ERROR)
            {
                s16Ret = SOCK_ERR_BUFFER_FULL;
            }
        }
    }
    return s16Ret;
}
/*********************************************************************
Function
        shutdown

Description

Return
        None.
*********************************************************************/
int8_t shutdown(SOCKET sock)
{
    int8_t s8Ret = SOCK_ERR_INVALID_ARG;

    M2M_INFO("Sock to delete <%d>\r\n", sock);

    if(sock >= 0 && (gastrSockets[sock].bIsUsed == 1))
    {
        uint8_t u8Cmd = SOCKET_CMD_CLOSE;
        tstrCloseCmd strclose;
        strclose.sock = sock;
        strclose.u16SessionID       = gastrSockets[sock].u16SessionID;

        if(gastrSockets[sock].u8SSLFlags & SSL_FLAGS_ACTIVE)
        {
            u8Cmd = SOCKET_CMD_SSL_CLOSE;
        }
        s8Ret = SOCKET_REQUEST(u8Cmd, (uint8_t*)&strclose, sizeof(tstrCloseCmd), NULL,0, 0);
        if(s8Ret != SOCK_ERR_NO_ERROR)
        {
            s8Ret = SOCK_ERR_INVALID;
        }
        memset((uint8_t*)&gastrSockets[sock], 0, sizeof(tstrSocket));
    }
    return s8Ret;
}
/*********************************************************************
Function
        recvfrom

Description

Return

*********************************************************************/
int16_t recvfrom(SOCKET sock, void *pvRecvBuf, uint16_t u16BufLen, uint32_t u32Timeoutmsec)
{
    int16_t s16Ret = SOCK_ERR_NO_ERROR;
    if((sock >= 0) && (pvRecvBuf != NULL) && (u16BufLen != 0) && (gastrSockets[sock].bIsUsed == 1))
    {
        if(gastrSockets[sock].bIsUsed)
        {
            s16Ret = SOCK_ERR_NO_ERROR;
            gastrSockets[sock].pu8UserBuffer = (uint8_t*)pvRecvBuf;
            gastrSockets[sock].u16UserBufferSize = u16BufLen;

            if(!gastrSockets[sock].bIsRecvPending)
            {
                tstrRecvCmd strRecv;

                gastrSockets[sock].bIsRecvPending = 1;

                /* Check the timeout value. */
                if(u32Timeoutmsec == 0)
                    strRecv.u32Timeoutmsec = 0xFFFFFFFF;
                else
                    strRecv.u32Timeoutmsec = NM_BSP_B_L_32(u32Timeoutmsec);
                strRecv.sock = sock;
                strRecv.u16SessionID        = gastrSockets[sock].u16SessionID;
                strRecv.u16BufLen           = u16BufLen;

                s16Ret = SOCKET_REQUEST(SOCKET_CMD_RECVFROM, (uint8_t*)&strRecv, sizeof(tstrRecvCmd), NULL , 0, 0);
                if(s16Ret != SOCK_ERR_NO_ERROR)
                {
                    s16Ret = SOCK_ERR_BUFFER_FULL;
                }
            }
        }
    }
    else
    {
        s16Ret = SOCK_ERR_INVALID_ARG;
    }
    return s16Ret;
}

/*********************************************************************
Function
        gethostbyname

Description

Return
        None.
*********************************************************************/
int8_t gethostbyname(const char * pcHostName)
{
    int8_t  s8Err = SOCK_ERR_INVALID_ARG;
    uint8_t u8HostNameSize = (uint8_t)strlen(pcHostName);
    if(u8HostNameSize <= HOSTNAME_MAX_SIZE)
    {
        s8Err = SOCKET_REQUEST(SOCKET_CMD_DNS_RESOLVE|M2M_REQ_DATA_PKT, (uint8_t*)pcHostName, u8HostNameSize + 1, NULL,0, 0);
        if(s8Err != SOCK_ERR_NO_ERROR)
        {
            s8Err = SOCK_ERR_INVALID;
        }
    }
    return s8Err;
}
/*********************************************************************
Function
        setsockopt

Description

Return
        None.
*********************************************************************/
static int8_t sslSetSockOpt(SOCKET sock, uint8_t  u8Opt, const void *pvOptVal, uint16_t u16OptLen)
{
    int8_t s8Ret = SOCK_ERR_INVALID_ARG;
    if(sock < TCP_SOCK_MAX)
    {
        if(gastrSockets[sock].u8SSLFlags & SSL_FLAGS_ACTIVE)
        {
            if(u8Opt == SO_SSL_BYPASS_X509_VERIF)
            {
                int32_t optVal = *((int32_t*)pvOptVal);
                if(optVal)
                {
                    gastrSockets[sock].u8SSLFlags |= SSL_FLAGS_BYPASS_X509;
                }
                else
                {
                    gastrSockets[sock].u8SSLFlags &= ~SSL_FLAGS_BYPASS_X509;
                }
                s8Ret = SOCK_ERR_NO_ERROR;
            }
            else if(u8Opt == SO_SSL_ENABLE_SESSION_CACHING)
            {
                int32_t optVal = *((int32_t*)pvOptVal);
                if(optVal)
                {
                    gastrSockets[sock].u8SSLFlags |= SSL_FLAGS_CACHE_SESSION;
                }
                else
                {
                    gastrSockets[sock].u8SSLFlags &= ~SSL_FLAGS_CACHE_SESSION;
                }
                s8Ret = SOCK_ERR_NO_ERROR;
            }
            else if(u8Opt == SO_SSL_ENABLE_CERTNAME_VALIDATION)
            {
                int32_t optVal = *((int32_t*)pvOptVal);
                if(optVal)
                {
                    gastrSockets[sock].u8SSLFlags |= SSL_FLAGS_CHECK_CERTNAME;
                }
                else
                {
                    gastrSockets[sock].u8SSLFlags &= ~SSL_FLAGS_CHECK_CERTNAME;
                }
                s8Ret = SOCK_ERR_NO_ERROR;
            }
            else if(u8Opt == SO_SSL_SNI)
            {
                if(u16OptLen < HOSTNAME_MAX_SIZE)
                {
                    uint8_t                 *pu8SNI = (uint8_t*)pvOptVal;
                    tstrSSLSetSockOptCmd    strCmd;

                    strCmd.sock         = sock;
                    strCmd.u16SessionID = gastrSockets[sock].u16SessionID;
                    strCmd.u8Option     = u8Opt;
                    strCmd.u32OptLen    = u16OptLen;
                    memcpy(strCmd.au8OptVal, pu8SNI, HOSTNAME_MAX_SIZE);

                    if(SOCKET_REQUEST(SOCKET_CMD_SSL_SET_SOCK_OPT, (uint8_t*)&strCmd, sizeof(tstrSSLSetSockOptCmd),
                        0, 0, 0) == M2M_ERR_MEM_ALLOC)
                    {
                        s8Ret = SOCKET_REQUEST(SOCKET_CMD_SSL_SET_SOCK_OPT | M2M_REQ_DATA_PKT,
                            (uint8_t*)&strCmd, sizeof(tstrSSLSetSockOptCmd), 0, 0, 0);
                    }
                    s8Ret = SOCK_ERR_NO_ERROR;
                }
                else
                {
                    M2M_ERR("SNI Exceeds Max Length\r\n");
                }
            }
            else
            {
                M2M_ERR("Unknown SSL Socket Option %d\r\n",u8Opt);
            }
        }
        else
        {
            M2M_ERR("Not SSL Socket\r\n");
        }
    }
    return s8Ret;
}
/*********************************************************************
Function
        setsockopt

Description

Return
        None.
*********************************************************************/
int8_t setsockopt(SOCKET sock, uint8_t  u8Level, uint8_t  option_name,
       const void *option_value, uint16_t u16OptionLen)
{
    int8_t s8Ret = SOCK_ERR_INVALID_ARG;
    if((sock >= 0)  && (option_value != NULL)  && (gastrSockets[sock].bIsUsed == 1))
    {
        if(u8Level == SOL_SSL_SOCKET)
        {
            s8Ret = sslSetSockOpt(sock, option_name, option_value, u16OptionLen);
        }
        else
        {
            if(u16OptionLen == sizeof(uint32_t))
            {
                uint8_t u8Cmd = SOCKET_CMD_SET_SOCKET_OPTION;
                tstrSetSocketOptCmd strSetSockOpt;
                strSetSockOpt.u8Option=option_name;
                strSetSockOpt.sock = sock;
                strSetSockOpt.u32OptionValue = *(uint32_t*)option_value;
                strSetSockOpt.u16SessionID      = gastrSockets[sock].u16SessionID;

                s8Ret = SOCKET_REQUEST(u8Cmd, (uint8_t*)&strSetSockOpt, sizeof(tstrSetSocketOptCmd), NULL,0, 0);
                if(s8Ret != SOCK_ERR_NO_ERROR)
                {
                    s8Ret = SOCK_ERR_INVALID;
                }
            }
        }
    }
    return s8Ret;
}
/*********************************************************************
Function
        getsockopt

Description

Return
        None.
*********************************************************************/
int8_t getsockopt(SOCKET sock, uint8_t u8Level, uint8_t u8OptName, const void *pvOptValue, uint8_t* pu8OptLen)
{
    // This is not implemented so return a value that will cause failure should this be used.
    return SOCK_ERR_INVALID_ARG;
}
/*********************************************************************
Function
    m2m_ping_req

Description
    Send Ping request.

Return
*********************************************************************/
int8_t m2m_ping_req(uint32_t u32DstIP, uint8_t u8TTL, tpfPingCb fpPingCb)
{
    int8_t s8Ret = M2M_ERR_INVALID_ARG;

    if((u32DstIP != 0) && (fpPingCb != NULL))
    {
        tstrPingCmd strPingCmd;

        strPingCmd.u16PingCount     = 1;
        strPingCmd.u32DestIPAddr    = u32DstIP;
        strPingCmd.u32CmdPrivate    = (uint32_t)fpPingCb;
        strPingCmd.u8TTL            = u8TTL;

        s8Ret = SOCKET_REQUEST(SOCKET_CMD_PING, (uint8_t*)&strPingCmd, sizeof(tstrPingCmd), NULL, 0, 0);
    }
    return s8Ret;
}
//DOM-IGNORE-END