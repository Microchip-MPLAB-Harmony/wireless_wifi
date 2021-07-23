/**
 *
 * \file
 *
 * \brief This module contains M2M WiFi API implementation.
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

#include "bsp/winc_bsp.h"
#include "common/winc_defines.h"
#include "common/winc_debug.h"
#include "winc_m2m_types.h"
#include "winc_m2m_wifi.h"
#include "winc_m2m_fwinfo.h"
#include "driver/winc_hif.h"
#include "driver/winc_drv.h"
#include "driver/winc_asic.h"

CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mPwrMode)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mTxPwrLevel)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWiFiGainIdx)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mEnableLogs)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mBatteryVoltage)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWiFiRoaming)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mConnCredHdr)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mConnCredCmn)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWifiWep)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWifiPsk)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWifi1xHdr)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWifiAuthInfoHdr)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWifiConnHdr)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWifiApId)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MGenericResp)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MWPSConnect)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MDefaultConnResp)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MScanRegion)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrCyptoResp)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mScanDone)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mReqScanResult)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWifiscanResult)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mWifiStateChanged)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mPsType)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mLsnInt)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MWifiMonitorModeCtrl)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MWifiRxPacketInfo)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MAPConfig)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MAPConfigExt)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MAPModeConfig)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mServerInit)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mClientState)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2Mservercmd)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2mSetMacAddress)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MProvisionModeConfig)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MConnInfo)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrTlsCrlEntry)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MSNTPConfig)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrOtaUpdateStatusResp)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrOtaUpdateInfo)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrOtaHostFileGetStatusResp)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrOtaHostFileReadStatusResp)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrOtaHostFileEraseStatusResp)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrSystemTime)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrM2MMulticastMac)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrPrng)
CONF_WINC_HIF_STRUCT_SIZE_CHECK(tstrConfAutoRate)

#ifdef CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE
typedef union
{
    uint8_t             u8Reserved[CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE];
    tstrM2mWifi1xHdr    strHdr;
} tuM2mWifi1xHdr;
#endif

#ifdef CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_STATIC
static tuM2mWifi1xHdr       u1xHdr;
#endif

/* Require authentication of server. */
#define WIFI_1X_TLS_HS_FLAGS_PEER_AUTH          NBIT1
/* Enable expiry checking of server certificate chain. */
#define WIFI_1X_TLS_HS_FLAGS_PEER_CERTTIMECHECK NBIT2
/* Require local system time to be known (i.e. fail expiry checking if time is not known locally). */
#define WIFI_1X_TLS_HS_FLAGS_REQUIRE_TIME       NBIT3
/* Enable TLS session caching. */
#define WIFI_1X_TLS_HS_FLAGS_SESSION_CACHING    NBIT4
/* Reserved, this bit must be clear. */
#define WIFI_1X_TLS_HS_FLAGS_RSV5               NBIT5
/* Require server authentication to be against a specified root certificate. */
#define WIFI_1X_TLS_HS_FLAGS_SPECIFY_ROOTCERT   NBIT6
/* Reserved, this bit must be clear. */
#define WIFI_1X_TLS_HS_FLAGS_RSV7               NBIT7

#define WIFI_1X_TLS_HS_FLAGS_DEFAULT    (           \
            WIFI_1X_TLS_HS_FLAGS_PEER_AUTH          \
        |   WIFI_1X_TLS_HS_FLAGS_PEER_CERTTIMECHECK \
        |   WIFI_1X_TLS_HS_FLAGS_SESSION_CACHING    \
)

static uint8_t              gu8ChNum;
static uint8_t              gu8WifiState        = WIFI_STATE_DEINIT;
static tpfAppWifiCb         gpfAppWifiCb        = NULL;
static uint8_t              gu8scanInProgress   = 0;
static tenuPowerSaveModes   genuPsMode          = M2M_NO_PS;
static uint32_t             gu321xTlsHsFlags    = WIFI_1X_TLS_HS_FLAGS_DEFAULT;
static uint8_t              gau81xRootSha1[20]  = {0};

#ifdef ETH_MODE
static tpfAppEthCb          gpfAppEthCb = NULL;
static uint8_t              *gau8ethRcvBuf = NULL;
static uint16_t             gu16ethRcvBufSize;
#endif

typedef struct
{
    uint_fast8_t    u8OpCode;
    uint_fast16_t   u16DataSize;
} tstrWiFiCbEntry;

static tstrWiFiCbEntry WiFiCbTable[] =
{
    {M2M_WIFI_RESP_CON_STATE_CHANGED,   sizeof(tstrM2mWifiStateChanged)},
    {M2M_WIFI_RESP_GET_SYS_TIME,        sizeof(tstrSystemTime)},
    {M2M_WIFI_RESP_CONN_INFO,           sizeof(tstrM2MConnInfo)},
    {M2M_WIFI_REQ_DHCP_CONF,            sizeof(tstrM2MIPConfig)},
    {M2M_WIFI_REQ_DHCP_FAILURE,         0},
    {M2M_WIFI_REQ_WPS,                  sizeof(tstrM2MWPSInfo)},
    {M2M_WIFI_RESP_IP_CONFLICT,         sizeof(uint32_t)},
    {M2M_WIFI_RESP_SCAN_DONE,           sizeof(tstrM2mScanDone)},
    {M2M_WIFI_RESP_SCAN_RESULT,         sizeof(tstrM2mWifiscanResult)},
    {M2M_WIFI_RESP_CURRENT_RSSI,        4},
    {M2M_WIFI_RESP_CLIENT_INFO,         4},
    {M2M_WIFI_RESP_PROVISION_INFO,      sizeof(tstrM2MProvisionInfo)},
    {M2M_WIFI_RESP_DEFAULT_CONNECT,     sizeof(tstrM2MDefaultConnResp)},
    {M2M_WIFI_REQRSP_DELETE_APID,       sizeof(tstrM2MGenericResp)},
#ifdef ETH_MODE
    {M2M_WIFI_RESP_ETHERNET_RX_PACKET,  sizeof(tstrM2mIpRsvdPkt)},
#endif
    {M2M_WIFI_MAX_STA_ALL,              0}
};

typedef union
{
    tstrM2mWifiStateChanged strStateChanged;
    tstrSystemTime          strSysTime;
    tstrM2MConnInfo         strConnInfo;
    tstrM2MIPConfig         strIpConfig;
    tstrM2MWPSInfo          strWps;
    uint32_t                u32ConflictedIP;
    tstrM2mScanDone         strScanDone;
    tstrM2mWifiscanResult   strScanResult;
    tstrM2MProvisionInfo    strProvInfo;
    tstrM2MDefaultConnResp  strDefaultConnResp;
    tstrM2MGenericResp      strGenericResp;
#ifdef ETH_MODE
    tstrM2mIpRsvdPkt        strM2mRsvd;
#endif
} tuWiFiCbMsgs;

void m2m_wifi_cb(uint8_t u8OpCode, uint16_t u16DataSize, uint32_t u32Addr)
{
    UNUSED_VAR(u16DataSize);

    tstrWiFiCbEntry *pWiFiCbEntry;
    tuWiFiCbMsgs wifiMsg;

    pWiFiCbEntry = WiFiCbTable;

    while (pWiFiCbEntry->u8OpCode != M2M_WIFI_MAX_STA_ALL)
    {
        if (pWiFiCbEntry->u8OpCode == u8OpCode)
        {
            if (pWiFiCbEntry->u16DataSize)
            {
                if (M2M_SUCCESS != winc_hif_receive(u32Addr, &wifiMsg, pWiFiCbEntry->u16DataSize))
                {
                    return;
                }

                if (u8OpCode == M2M_WIFI_RESP_IP_CONFLICT)
                {
                    WINC_LOG_INFO("Conflicted IP \" %u.%u.%u.%u \"",
                                  WINC_BYTE_0(wifiMsg.u32ConflictedIP),
                                  WINC_BYTE_1(wifiMsg.u32ConflictedIP),
                                  WINC_BYTE_2(wifiMsg.u32ConflictedIP),
                                  WINC_BYTE_3(wifiMsg.u32ConflictedIP));
                }
                else if (u8OpCode == M2M_WIFI_RESP_SCAN_DONE)
                {
                    gu8scanInProgress = 0;
                    gu8ChNum = wifiMsg.strScanDone.u8NumofCh;
                }
            }

            if (NULL != gpfAppWifiCb)
            {
                if (pWiFiCbEntry->u16DataSize)
                {
                    gpfAppWifiCb(u8OpCode, &wifiMsg);
                }
                else
                {
                    gpfAppWifiCb(u8OpCode, NULL);
                }
            }

            break;
        }

        pWiFiCbEntry++;
    }

    if (u8OpCode == M2M_WIFI_RESP_GET_PRNG)
    {
        tstrPrng strPrng;

        if (winc_hif_receive(u32Addr, &strPrng, sizeof(tstrPrng)) == M2M_SUCCESS)
        {
            if (winc_hif_receive(u32Addr + sizeof(tstrPrng), strPrng.pu8RngBuff, strPrng.u16PrngSize) == M2M_SUCCESS)
            {
                if (gpfAppWifiCb)
                {
                    gpfAppWifiCb(M2M_WIFI_RESP_GET_PRNG, &strPrng);
                }
            }
        }
    }

#ifdef ETH_MODE
    else if (u8OpCode == M2M_WIFI_RESP_ETHERNET_RX_PACKET)
    {
        tstrM2mIpCtrlBuf  strM2mIpCtrlBuf;
        uint16_t u16Offset = wifiMsg.strM2mRsvd.u16PktOffset;
        strM2mIpCtrlBuf.u16RemainingDataSize = wifiMsg.strM2mRsvd.u16PktSz;

        if ((gpfAppEthCb) && (gau8ethRcvBuf) && (gu16ethRcvBufSize > 0))
        {
            do
            {
                if (strM2mIpCtrlBuf.u16RemainingDataSize > gu16ethRcvBufSize)
                {
                    strM2mIpCtrlBuf.u16DataSize = gu16ethRcvBufSize;
                }
                else
                {
                    strM2mIpCtrlBuf.u16DataSize = strM2mIpCtrlBuf.u16RemainingDataSize;
                }

                if (winc_hif_receive(u32Addr + u16Offset, gau8ethRcvBuf, strM2mIpCtrlBuf.u16DataSize) == M2M_SUCCESS)
                {
                    strM2mIpCtrlBuf.u16RemainingDataSize -= strM2mIpCtrlBuf.u16DataSize;
                    u16Offset += strM2mIpCtrlBuf.u16DataSize;
                    gpfAppEthCb(M2M_WIFI_RESP_ETHERNET_RX_PACKET, gau8ethRcvBuf, &(strM2mIpCtrlBuf));
                }
                else
                {
                    break;
                }
            }
            while (strM2mIpCtrlBuf.u16RemainingDataSize > 0);
        }
    }

#endif  /* ETH_MODE */
}

int8_t m2m_wifi_download_mode(void)
{
    if (WINC_DRV_SUCCESS != winc_drv_init(true))
    {
        return M2M_ERR_FAIL;
    }

    winc_chip_interrupts_enable();
    gu8WifiState = WIFI_STATE_INIT;

    return M2M_SUCCESS;
}

static int8_t m2m_validate_ap_parameters(const tstrM2MAPModeConfig *pstrM2MAPModeConfig)
{
    int8_t s8Ret = M2M_SUCCESS;

    /* Check for incoming pointer */
    if (pstrM2MAPModeConfig == NULL)
    {
        WINC_LOG_ERROR("INVALID POINTER");
        s8Ret = M2M_ERR_FAIL;
        goto ERR1;
    }

    /* Check for SSID */
    if ((strlen((const char *)pstrM2MAPModeConfig->strApConfig.au8SSID) <= 0) || (strlen((const char *)pstrM2MAPModeConfig->strApConfig.au8SSID) >= M2M_MAX_SSID_LEN))
    {
        WINC_LOG_ERROR("INVALID SSID");
        s8Ret = M2M_ERR_FAIL;
        goto ERR1;
    }

    /* Check for Channel */
    if (pstrM2MAPModeConfig->strApConfig.u8ListenChannel > M2M_WIFI_CH_14 || pstrM2MAPModeConfig->strApConfig.u8ListenChannel < M2M_WIFI_CH_1)
    {
        WINC_LOG_ERROR("INVALID CH");
        s8Ret = M2M_ERR_FAIL;
        goto ERR1;
    }

    /* Check for DHCP Server IP address */
    if (!(pstrM2MAPModeConfig->strApConfig.au8DHCPServerIP[0] || pstrM2MAPModeConfig->strApConfig.au8DHCPServerIP[1]))
    {
        if (!(pstrM2MAPModeConfig->strApConfig.au8DHCPServerIP[2]))
        {
            WINC_LOG_ERROR("INVALID DHCP SERVER IP");
            s8Ret = M2M_ERR_FAIL;
            goto ERR1;
        }
    }

    /* Check for Security */
    if (pstrM2MAPModeConfig->strApConfig.u8SecType == M2M_WIFI_SEC_OPEN)
    {
        goto ERR1;
    }
    else if (pstrM2MAPModeConfig->strApConfig.u8SecType == M2M_WIFI_SEC_WEP)
    {
        /* Check for WEP Key index */
        if ((pstrM2MAPModeConfig->strApConfig.u8KeyIndx == 0) || (pstrM2MAPModeConfig->strApConfig.u8KeyIndx > WEP_KEY_MAX_INDEX))
        {
            WINC_LOG_ERROR("INVALID KEY INDEX");
            s8Ret = M2M_ERR_FAIL;
            goto ERR1;
        }

        /* Check for WEP Key size */
        if ((pstrM2MAPModeConfig->strApConfig.u8KeySz != WEP_40_KEY_STRING_SIZE) &&
                (pstrM2MAPModeConfig->strApConfig.u8KeySz != WEP_104_KEY_STRING_SIZE)
           )
        {
            WINC_LOG_ERROR("INVALID KEY STRING SIZE");
            s8Ret = M2M_ERR_FAIL;
            goto ERR1;
        }

        if ((strlen((const char *)pstrM2MAPModeConfig->strApConfig.au8WepKey) <= 0) || (strlen((const char *)pstrM2MAPModeConfig->strApConfig.au8WepKey) > WEP_104_KEY_STRING_SIZE))
        {
            WINC_LOG_ERROR("INVALID KEY SIZE");
            s8Ret = M2M_ERR_FAIL;
            goto ERR1;
        }
    }
    else if (pstrM2MAPModeConfig->strApConfig.u8SecType == M2M_WIFI_SEC_WPA_PSK)
    {
        /* Check for WPA Key size */
        if ( ((pstrM2MAPModeConfig->strApConfig.u8KeySz + 1) < M2M_MIN_PSK_LEN) || ((pstrM2MAPModeConfig->strApConfig.u8KeySz + 1) > M2M_MAX_PSK_LEN))
        {
            WINC_LOG_ERROR("INVALID WPA KEY SIZE");
            s8Ret = M2M_ERR_FAIL;
            goto ERR1;
        }
    }
    else
    {
        WINC_LOG_ERROR("INVALID AUTHENTICATION MODE");
        s8Ret = M2M_ERR_FAIL;
        goto ERR1;
    }

ERR1:
    return s8Ret;
}

static int8_t m2m_validate_scan_options(tstrM2MScanOption *ptstrM2MScanOption)
{
    /* Check for incoming pointer */
    if (ptstrM2MScanOption == NULL)
    {
        WINC_LOG_ERROR("INVALID POINTER");
        return M2M_ERR_FAIL;
    }

    /* Check for valid No of slots */
    if (ptstrM2MScanOption->u8NumOfSlot == 0)
    {
        WINC_LOG_ERROR("INVALID No of scan slots! %u", ptstrM2MScanOption->u8NumOfSlot);
        return M2M_ERR_FAIL;
    }

    /* Check for valid time of slots */
    if ((ptstrM2MScanOption->u8SlotTime < 10) || (ptstrM2MScanOption->u8SlotTime > 250))
    {
        WINC_LOG_ERROR("INVALID scan slot time! %u", ptstrM2MScanOption->u8SlotTime);
        return M2M_ERR_FAIL;
    }

    /* Check for valid No of probe requests per slot */
    if ((ptstrM2MScanOption->u8ProbesPerSlot == 0) || (ptstrM2MScanOption->u8ProbesPerSlot > M2M_SCAN_DEFAULT_NUM_PROBE))
    {
        WINC_LOG_ERROR("INVALID No of probe requests per scan slot %u", ptstrM2MScanOption->u8ProbesPerSlot);
        return M2M_ERR_FAIL;
    }

    /* Check for valid RSSI threshold */
    if (ptstrM2MScanOption->s8RssiThresh >= 0)
    {
        WINC_LOG_ERROR("INVALID RSSI threshold %d", ptstrM2MScanOption->s8RssiThresh);
        return M2M_ERR_FAIL;
    }

    return M2M_SUCCESS;
}

int8_t m2m_wifi_send_crl(tstrTlsCrlInfo *pCRL)
{
    return winc_hif_send(M2M_REQ_GROUP_SSL, M2M_SSL_IND_CRL | M2M_REQ_DATA_PKT, NULL, 0, pCRL, sizeof(tstrTlsCrlInfo), 0);
}

int8_t m2m_wifi_init_hold(void)
{
    if (WINC_DRV_SUCCESS != winc_drv_init(false))
    {
        return M2M_ERR_FAIL;
    }

    gu8WifiState = WIFI_STATE_INIT;

    return M2M_SUCCESS;
}

int8_t m2m_wifi_init_start(tstrWifiInitParam *pWifiInitParam)
{
    tstrM2mRev strtmp;
    int8_t s8Ret;
    uint32_t u32DriverVerInfo = M2M_MAKE_VERSION_INFO(M2M_RELEASE_VERSION_MAJOR_NO, \
                                M2M_RELEASE_VERSION_MINOR_NO, M2M_RELEASE_VERSION_PATCH_NO, \
                                M2M_MIN_REQ_DRV_VERSION_MAJOR_NO, M2M_MIN_REQ_DRV_VERSION_MINOR_NO, \
                                M2M_MIN_REQ_DRV_VERSION_PATCH_NO);

    if (pWifiInitParam == NULL)
    {
        return M2M_ERR_INVALID_ARG;
    }

    gpfAppWifiCb = pWifiInitParam->pfAppWifiCb;

#ifdef ETH_MODE
    gpfAppEthCb         = pWifiInitParam->strEthInitParam.pfAppEthCb;
    gau8ethRcvBuf       = pWifiInitParam->strEthInitParam.au8ethRcvBuf;
    gu16ethRcvBufSize   = pWifiInitParam->strEthInitParam.u16ethRcvBufSize;
#endif /* ETH_MODE */

    gu8scanInProgress = 0;
    /* Apply device specific initialization. */

    /* u8WifiMode              | u8BootATE | u8EthMode | u32StateRegVal
       ----------------------------------------------------------------------
       M2M_WIFI_MODE_NORMAL    |   false   |   false   | u32DriverVerInfo
       M2M_WIFI_MODE_ATE_HIGH  |   true    |   false   | NBIT20
       M2M_WIFI_MODE_ATE_LOW   |   true    |   false   | 0
       M2M_WIFI_MODE_ETHERNET  |   false   |   true    | u32DriverVerInfo
    */

#ifdef ETH_MODE

    if (WINC_DRV_SUCCESS != winc_drv_start(false, (pWifiInitParam->strEthInitParam.u8EthernetEnable == M2M_WIFI_MODE_ETHERNET) ? true : false, u32DriverVerInfo))
    {
        return M2M_ERR_INIT;
    }

#else
    if (WINC_DRV_SUCCESS != winc_drv_start(false, false, u32DriverVerInfo))
    {
        return M2M_ERR_INIT;
    }
#endif

    WINC_LOG_INFO("DriverVerInfo: 0x%08" PRIx32, u32DriverVerInfo);

    gu8WifiState = WIFI_STATE_START;

    /* Initialize host interface module */
    if (M2M_SUCCESS != winc_hif_init())
    {
        return M2M_ERR_INIT;
    }

    s8Ret = m2m_wifi_get_firmware_version(&strtmp);

    WINC_LOG_INFO("Firmware ver   : %u.%u.%u Svnrev %u", strtmp.u8FirmwareMajor, strtmp.u8FirmwareMinor, strtmp.u8FirmwarePatch, strtmp.u16FirmwareSvnNum);
    WINC_LOG_INFO("Firmware Build %s Time %s", strtmp.BuildDate, strtmp.BuildTime);
    WINC_LOG_INFO("Firmware Min driver ver : %u.%u.%u", strtmp.u8DriverMajor, strtmp.u8DriverMinor, strtmp.u8DriverPatch);
    WINC_LOG_INFO("Driver ver: %u.%u.%u", M2M_RELEASE_VERSION_MAJOR_NO, M2M_RELEASE_VERSION_MINOR_NO, M2M_RELEASE_VERSION_PATCH_NO);
    WINC_LOG_INFO("Driver built at %s\t%s", __DATE__, __TIME__);

    if (M2M_ERR_FW_VER_MISMATCH == s8Ret)
    {
        WINC_LOG_ERROR("Mismatch Firmware Version");
    }

    return s8Ret;
}

int8_t m2m_wifi_init(tstrWifiInitParam *pWifiInitParam)
{
    int8_t ret = M2M_SUCCESS;

    ret = m2m_wifi_init_hold();

    if (ret == M2M_SUCCESS)
    {
        ret = m2m_wifi_init_start(pWifiInitParam);
    }

    return ret;
}

int8_t m2m_wifi_deinit(void *pVArg)
{
    UNUSED_VAR(pVArg);

    gu8WifiState = WIFI_STATE_DEINIT;
    winc_hif_deinit();

    winc_drv_deinit();

    return M2M_SUCCESS;
}

int8_t m2m_wifi_handle_events(void *pVArg)
{
    UNUSED_VAR(pVArg);

    return winc_hif_handle_isr();
}

int8_t m2m_wifi_delete_sc(char *pcSsid, uint8_t u8SsidLen)
{
    UNUSED_VAR(pcSsid);
    UNUSED_VAR(u8SsidLen);

    tstrM2mWifiApId strApId;
    memset(&strApId, 0, sizeof(strApId));
    strApId.au8SSID[0] = 0xFF;  // Special value used to cause firmware to delete all entries.
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQRSP_DELETE_APID, &strApId, sizeof(tstrM2mWifiApId));
}

int8_t m2m_wifi_default_connect(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_DEFAULT_CONNECT, NULL, 0);
}

/*************************************************************************************************/
/*                                WIFI CONNECT INTERNAL FUNCTIONS                                */
/*************************************************************************************************/
static int8_t m2m_wifi_connect_prepare_msg(
    tenuCredStoreOption enuCredStoreOption,
    tenuM2mSecType      enuAuthType,
    uint16_t            u16AuthSize,
    tstrNetworkId       *pstrNetworkId,
    tstrM2mWifiConnHdr  *pstrWifiConn
)
{
    int8_t  ret = M2M_ERR_FAIL;
    uint16_t u16CredSize = sizeof(tstrM2mConnCredCmn) + u16AuthSize;

    /* Check application params. */
    if (
        (pstrNetworkId == NULL)
        ||  (pstrNetworkId->pu8Ssid == NULL)
        ||  (pstrNetworkId->u8SsidLen >= M2M_MAX_SSID_LEN)
    )
    {
        goto INVALID_ARG;
    }

    if (pstrWifiConn != NULL)
    {
        tstrM2mConnCredHdr  *pstrHdr = &pstrWifiConn->strConnCredHdr;
        tstrM2mConnCredCmn  *pstrCmn = &pstrWifiConn->strConnCredCmn;

        memset(pstrWifiConn, 0, sizeof(tstrM2mWifiConnHdr));

        pstrHdr->u16CredSize = u16CredSize;

        switch (enuCredStoreOption)
        {
            case WIFI_CRED_SAVE_ENCRYPTED:
                pstrHdr->u8CredStoreFlags |= M2M_CRED_ENCRYPT_FLAG;

            // intentional fall through...
            case WIFI_CRED_SAVE_UNENCRYPTED:
                pstrHdr->u8CredStoreFlags |= M2M_CRED_STORE_FLAG;

            // intentional fall through...
            case WIFI_CRED_DONTSAVE:
                break;

            default:
                goto INVALID_ARG;
        }

        if (pstrNetworkId->enuChannel == M2M_WIFI_CH_ALL)
        {
            pstrHdr->u8Channel = (uint8_t)(pstrNetworkId->enuChannel);
        }
        else if ((pstrNetworkId->enuChannel <= M2M_WIFI_CH_14) && (pstrNetworkId->enuChannel >= M2M_WIFI_CH_1))
        {
            pstrHdr->u8Channel = (uint8_t)(pstrNetworkId->enuChannel) - 1;
        }
        else
        {
            goto INVALID_ARG;
        }

        if ((enuAuthType == M2M_WIFI_SEC_INVALID) || (enuAuthType >= M2M_WIFI_NUM_AUTH_TYPES))
        {
            goto INVALID_ARG;
        }

        pstrCmn->u8AuthType = (uint8_t)enuAuthType;

        pstrCmn->u8SsidLen = pstrNetworkId->u8SsidLen;
        memcpy(pstrCmn->au8Ssid, pstrNetworkId->pu8Ssid, pstrNetworkId->u8SsidLen);

        if (pstrNetworkId->pu8Bssid != NULL)
        {
            pstrCmn->u8Options = M2M_WIFI_CONN_BSSID_FLAG;
            memcpy(pstrCmn->au8Bssid, pstrNetworkId->pu8Bssid, M2M_MAC_ADDRES_LEN);
        }

        /* Everything is ok, set return value. */
        ret = M2M_SUCCESS;
    }

    return ret;
INVALID_ARG:
    return M2M_ERR_INVALID_ARG;
}

/* Convert hexchar to value 0-15 */
static uint8_t hexchar_2_val(uint8_t ch)
{
    ch -= 0x30;

    if (ch <= 9)
    {
        return ch;
    }

    ch |= 0x20;
    ch -= 0x31;

    if (ch <= 5)
    {
        return ch + 10;
    }

    return 0xFF;
}

/* Convert hexstring to bytes */
static int8_t hexstr_2_bytes(uint8_t *pu8Out, uint8_t *pu8In, uint8_t u8SizeOut)
{
    while (u8SizeOut--)
    {
        uint8_t u8Out = hexchar_2_val(*pu8In++);

        if (u8Out > 0xF)
        {
            return M2M_ERR_INVALID_ARG;
        }

        *pu8Out = u8Out * 0x10;
        u8Out = hexchar_2_val(*pu8In++);

        if (u8Out > 0xF)
        {
            return M2M_ERR_INVALID_ARG;
        }

        *pu8Out += u8Out;
        pu8Out++;
    }

    return M2M_SUCCESS;
}

/*************************************************************************************************/
/*                                        WIFI CONNECT APIS                                      */
/*************************************************************************************************/
int8_t m2m_wifi_connect_open(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId)
{
    int8_t              ret = M2M_ERR_INVALID_ARG;
    tstrM2mWifiConnHdr  strConnHdr;

    ret = m2m_wifi_connect_prepare_msg(enuCredStoreOption, M2M_WIFI_SEC_OPEN, 0, pstrNetworkId, &strConnHdr);

    if (ret == M2M_SUCCESS)
    {
        ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CONN, &strConnHdr, sizeof(strConnHdr));
    }

    return ret;
}

int8_t m2m_wifi_connect_wep(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuthWep *pstrAuthWep)
{
    int8_t  ret = M2M_ERR_INVALID_ARG;

    if (
        (pstrAuthWep != NULL) && (pstrAuthWep->pu8WepKey != NULL)
        &&  (pstrAuthWep->u8KeyIndx > 0) && (pstrAuthWep->u8KeyIndx <= WEP_KEY_MAX_INDEX)
        &&  ((pstrAuthWep->u8KeySz == WEP_104_KEY_STRING_SIZE) || (pstrAuthWep->u8KeySz == WEP_40_KEY_STRING_SIZE))
    )
    {
        tstrM2mWifiConnHdr  strConnHdr;

        ret = m2m_wifi_connect_prepare_msg( enuCredStoreOption,
                                            M2M_WIFI_SEC_WEP,
                                            sizeof(tstrM2mWifiWep),
                                            pstrNetworkId,
                                            &strConnHdr);

        if (ret == M2M_SUCCESS)
        {
            tstrM2mWifiWep strWep;

            strWep.u8KeyIndex = pstrAuthWep->u8KeyIndx - 1;
            strWep.u8KeyLen = pstrAuthWep->u8KeySz / 2;
            hexstr_2_bytes(strWep.au8WepKey, (pstrAuthWep->pu8WepKey), strWep.u8KeyLen);

            ret = winc_hif_send(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CONN | M2M_REQ_DATA_PKT,
                                &strConnHdr, sizeof(tstrM2mWifiConnHdr),
                                &strWep, sizeof(tstrM2mWifiWep), sizeof(tstrM2mWifiConnHdr));
        }
    }

    return ret;
}

int8_t m2m_wifi_connect_psk(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuthPsk *pstrAuthPsk)
{
    int8_t ret = M2M_ERR_INVALID_ARG;

    if (pstrAuthPsk != NULL)
    {
        tstrM2mWifiConnHdr  strConnHdr;

        ret = m2m_wifi_connect_prepare_msg( enuCredStoreOption,
                                            M2M_WIFI_SEC_WPA_PSK,
                                            sizeof(tstrM2mWifiPsk),
                                            pstrNetworkId,
                                            &strConnHdr);

        if (ret == M2M_SUCCESS)
        {
            tstrM2mWifiPsk strPsk;

            memset(&strPsk, 0, sizeof(tstrM2mWifiPsk));

            if (pstrAuthPsk->pu8Psk != NULL)
            {
                if (pstrAuthPsk->pu8Passphrase != NULL)
                {
                    ret = M2M_ERR_INVALID_ARG;
                }
                else
                {
                    strPsk.u8PassphraseLen = M2M_MAX_PSK_LEN - 1;

                    /* Use hexstr_2_bytes to verify pu8Psk input. */
                    if (M2M_SUCCESS != hexstr_2_bytes(strPsk.au8Passphrase, pstrAuthPsk->pu8Psk, strPsk.u8PassphraseLen / 2))
                    {
                        ret = M2M_ERR_INVALID_ARG;
                    }

                    memcpy(strPsk.au8Passphrase, pstrAuthPsk->pu8Psk, strPsk.u8PassphraseLen);
                }
            }
            else if (pstrAuthPsk->pu8Passphrase != NULL)
            {
                if (pstrAuthPsk->u8PassphraseLen > M2M_MAX_PSK_LEN - 1)
                {
                    ret = M2M_ERR_INVALID_ARG;
                }
                else
                {
                    strPsk.u8PassphraseLen = pstrAuthPsk->u8PassphraseLen;
                    memcpy(strPsk.au8Passphrase, pstrAuthPsk->pu8Passphrase, strPsk.u8PassphraseLen);
                }
            }
            else
            {
                ret = M2M_ERR_INVALID_ARG;
            }

            if (ret == M2M_SUCCESS)
            {
                ret = winc_hif_send(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CONN | M2M_REQ_DATA_PKT,
                                    &strConnHdr, sizeof(tstrM2mWifiConnHdr),
                                    &strPsk, sizeof(tstrM2mWifiPsk), sizeof(tstrM2mWifiConnHdr));
            }
        }
    }

    return ret;
}

int8_t m2m_wifi_1x_set_option(tenu1xOption enuOptionName, const void *pOptionValue, size_t OptionLen)
{
    if ((pOptionValue == NULL) && (OptionLen > 0))
    {
        return M2M_ERR_INVALID_ARG;
    }

    switch (enuOptionName)
    {
        case WIFI_1X_BYPASS_SERVER_AUTH:
            if (OptionLen != sizeof(int))
            {
                return M2M_ERR_INVALID_ARG;
            }

            switch (*(int *)pOptionValue)
            {
                case 1:
                    gu321xTlsHsFlags &= (uint32_t)(~WIFI_1X_TLS_HS_FLAGS_PEER_AUTH);
                    break;

                case 0:
                    gu321xTlsHsFlags |= (uint32_t)(WIFI_1X_TLS_HS_FLAGS_PEER_AUTH);
                    break;

                default:
                    return M2M_ERR_INVALID_ARG;
            }

            break;

        case WIFI_1X_TIME_VERIF_MODE:
            if (OptionLen != sizeof(tenuSslCertExpSettings))
            {
                return M2M_ERR_INVALID_ARG;
            }

            switch (*(tenuSslCertExpSettings *)pOptionValue)
            {
                case SSL_CERT_EXP_CHECK_DISABLE:
                    gu321xTlsHsFlags &= (uint32_t)(~WIFI_1X_TLS_HS_FLAGS_PEER_CERTTIMECHECK);
                    gu321xTlsHsFlags &= (uint32_t)(~WIFI_1X_TLS_HS_FLAGS_REQUIRE_TIME);
                    break;

                case SSL_CERT_EXP_CHECK_ENABLE:
                    gu321xTlsHsFlags |= WIFI_1X_TLS_HS_FLAGS_PEER_CERTTIMECHECK;
                    gu321xTlsHsFlags |= WIFI_1X_TLS_HS_FLAGS_REQUIRE_TIME;
                    break;

                case SSL_CERT_EXP_CHECK_EN_IF_SYS_TIME:
                    gu321xTlsHsFlags |= WIFI_1X_TLS_HS_FLAGS_PEER_CERTTIMECHECK;
                    gu321xTlsHsFlags &= (uint32_t)(~WIFI_1X_TLS_HS_FLAGS_REQUIRE_TIME);
                    break;

                default:
                    return M2M_ERR_INVALID_ARG;
            }

            break;

        case WIFI_1X_SESSION_CACHING:
            if (OptionLen != sizeof(int))
            {
                return M2M_ERR_INVALID_ARG;
            }

            switch (*(int *)pOptionValue)
            {
                case 1:
                    gu321xTlsHsFlags |= WIFI_1X_TLS_HS_FLAGS_SESSION_CACHING;
                    break;

                case 0:
                    gu321xTlsHsFlags &= (uint32_t)(~WIFI_1X_TLS_HS_FLAGS_SESSION_CACHING);
                    break;

                default:
                    return M2M_ERR_INVALID_ARG;
            }

            break;

        case WIFI_1X_SPECIFIC_ROOTCERT:
            switch (OptionLen)
            {
                case 20:
                    gu321xTlsHsFlags |= WIFI_1X_TLS_HS_FLAGS_SPECIFY_ROOTCERT;
                    memcpy(gau81xRootSha1, pOptionValue, sizeof(gau81xRootSha1));
                    break;

                case 0:
                    gu321xTlsHsFlags &= (uint32_t)(~WIFI_1X_TLS_HS_FLAGS_SPECIFY_ROOTCERT);
                    memset(gau81xRootSha1, 0, sizeof(gau81xRootSha1));
                    break;

                default:
                    return M2M_ERR_INVALID_ARG;
            }

            break;

        default:
            return M2M_ERR_INVALID_ARG;
    }

    return M2M_SUCCESS;
}

int8_t m2m_wifi_1x_get_option(tenu1xOption enuOptionName, void *pOptionValue, size_t *pOptionLen)
{
    if (pOptionValue == NULL)
    {
        return M2M_ERR_INVALID_ARG;
    }

    switch (enuOptionName)
    {
        case WIFI_1X_BYPASS_SERVER_AUTH:
            if (*pOptionLen < sizeof(int))
            {
                return M2M_ERR_INVALID_ARG;
            }

            *pOptionLen = sizeof(int);
            *(int *)pOptionValue = (gu321xTlsHsFlags & WIFI_1X_TLS_HS_FLAGS_PEER_AUTH) ? 0 : 1;
            break;

        case WIFI_1X_TIME_VERIF_MODE:
            if (*pOptionLen < sizeof(tenuSslCertExpSettings))
            {
                return M2M_ERR_INVALID_ARG;
            }

            *pOptionLen = sizeof(tenuSslCertExpSettings);

            if (!(gu321xTlsHsFlags & WIFI_1X_TLS_HS_FLAGS_PEER_CERTTIMECHECK))
            {
                *(tenuSslCertExpSettings *)pOptionValue = SSL_CERT_EXP_CHECK_DISABLE;
            }
            else if (gu321xTlsHsFlags & WIFI_1X_TLS_HS_FLAGS_REQUIRE_TIME)
            {
                *(tenuSslCertExpSettings *)pOptionValue = SSL_CERT_EXP_CHECK_ENABLE;
            }
            else
            {
                *(tenuSslCertExpSettings *)pOptionValue = SSL_CERT_EXP_CHECK_EN_IF_SYS_TIME;
            }

            break;

        case WIFI_1X_SESSION_CACHING:
            if (*pOptionLen < sizeof(int))
            {
                return M2M_ERR_INVALID_ARG;
            }

            *pOptionLen = sizeof(int);
            *(int *)pOptionValue = (gu321xTlsHsFlags & WIFI_1X_TLS_HS_FLAGS_SESSION_CACHING) ? 1 : 0;
            break;

        case WIFI_1X_SPECIFIC_ROOTCERT:
            if (gu321xTlsHsFlags & WIFI_1X_TLS_HS_FLAGS_SPECIFY_ROOTCERT)
            {
                if (*pOptionLen < sizeof(gau81xRootSha1))
                {
                    return M2M_ERR_INVALID_ARG;
                }

                *pOptionLen = sizeof(gau81xRootSha1);
                memcpy(pOptionValue, gau81xRootSha1, sizeof(gau81xRootSha1));
            }
            else
            {
                *pOptionLen = 0;
            }

            break;

        default:
            return M2M_ERR_INVALID_ARG;
    }

    return M2M_SUCCESS;
}

int8_t m2m_wifi_connect_1x_mschap2(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuth1xMschap2 *pstrAuth1xMschap2)
{
#ifdef CONF_WINC_SUPPORT_ENTERPRISE_CONNECT
    int8_t ret = M2M_ERR_INVALID_ARG;

    if (pstrAuth1xMschap2 != NULL)
    {
        if (pstrAuth1xMschap2->pu8Domain == NULL)
        {
            pstrAuth1xMschap2->u16DomainLen = 0;
        }

        if (
            (pstrAuth1xMschap2->pu8UserName != NULL)
            &&  (pstrAuth1xMschap2->pu8Password != NULL)
            &&  ((uint32_t)(pstrAuth1xMschap2->u16DomainLen) + pstrAuth1xMschap2->u16UserNameLen <= M2M_AUTH_1X_USER_LEN_MAX)
            &&  (pstrAuth1xMschap2->u16PasswordLen <= M2M_AUTH_1X_PASSWORD_LEN_MAX)
        )
        {
            tstrM2mWifiConnHdr  strConnHdr;
            uint16_t            u16AuthSize =   sizeof(tstrM2mWifi1xHdr) +
                                                pstrAuth1xMschap2->u16DomainLen +
                                                pstrAuth1xMschap2->u16UserNameLen +
                                                pstrAuth1xMschap2->u16PasswordLen;

#ifdef CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE

            if (u16AuthSize > CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE)
            {
                return M2M_ERR_FAIL;
            }

#endif
            ret = m2m_wifi_connect_prepare_msg( enuCredStoreOption, M2M_WIFI_SEC_802_1X, u16AuthSize, pstrNetworkId, &strConnHdr);

            if (ret == M2M_SUCCESS)
            {
                tstrM2mWifi1xHdr    *pstr1xHdr;

#if defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_STACK)
                tuM2mWifi1xHdr      u1xHdr;

                pstr1xHdr = &u1xHdr.strHdr;
#elif defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_STATIC)
                pstr1xHdr = &u1xHdr.strHdr;
#elif defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_DYNAMIC)
                pstr1xHdr = (tstrM2mWifi1xHdr *)CONF_WINC_LARGE_MEMORY_ALLOC_FUNC(u16AuthSize);
#endif

                if (pstr1xHdr != NULL)
                {
                    uint8_t *pu8AuthPtr = pstr1xHdr->au81xAuthDetails;

                    memset(pstr1xHdr, 0, u16AuthSize);

                    pstr1xHdr->u8Flags = M2M_802_1X_MSCHAP2_FLAG;

                    if (pstrAuth1xMschap2->bUnencryptedUserName == true)
                    {
                        pstr1xHdr->u8Flags |= M2M_802_1X_UNENCRYPTED_USERNAME_FLAG;
                    }

                    if (pstrAuth1xMschap2->bPrependDomain == true)
                    {
                        pstr1xHdr->u8Flags |= M2M_802_1X_PREPEND_DOMAIN_FLAG;
                    }

                    pstr1xHdr->u8HdrLength = sizeof(tstrM2mWifi1xHdr);
                    pstr1xHdr->u32TlsHsFlags = gu321xTlsHsFlags;
                    memcpy(pstr1xHdr->au8TlsSpecificRootNameSha1, gau81xRootSha1, sizeof(gau81xRootSha1));

                    pstr1xHdr->u8DomainLength = 0;

                    if (pstrAuth1xMschap2->pu8Domain != NULL)
                    {
                        pstr1xHdr->u8DomainLength = (uint8_t)(pstrAuth1xMschap2->u16DomainLen);
                        memcpy(pu8AuthPtr, pstrAuth1xMschap2->pu8Domain, pstr1xHdr->u8DomainLength);
                        pu8AuthPtr += pstr1xHdr->u8DomainLength;
                    }

                    pstr1xHdr->u8UserNameLength = (pstrAuth1xMschap2->u16UserNameLen);
                    memcpy(pu8AuthPtr, pstrAuth1xMschap2->pu8UserName, pstr1xHdr->u8UserNameLength);
                    pu8AuthPtr += pstr1xHdr->u8UserNameLength;

                    pstr1xHdr->u16PrivateKeyOffset = (uint16_t)(pu8AuthPtr - pstr1xHdr->au81xAuthDetails);
                    pstr1xHdr->u16PrivateKeyLength = pstrAuth1xMschap2->u16PasswordLen;
                    memcpy(pu8AuthPtr, pstrAuth1xMschap2->pu8Password, pstr1xHdr->u16PrivateKeyLength);

                    ret = winc_hif_send(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CONN | M2M_REQ_DATA_PKT,
                                        &strConnHdr, sizeof(tstrM2mWifiConnHdr),
                                        pstr1xHdr, u16AuthSize,
                                        sizeof(tstrM2mWifiConnHdr));
#ifdef CONF_WINC_LARGE_MEMORY_FREE_FUNC
                    CONF_WINC_LARGE_MEMORY_FREE_FUNC(pstr1xHdr);
#endif
                }
                else
                {
                    ret = M2M_ERR_MEM_ALLOC;
                }
            }
        }
    }

    return ret;
#else
    UNUSED_VAR(enuCredStoreOption);
    UNUSED_VAR(pstrNetworkId);
    UNUSED_VAR(pstrAuth1xMschap2);

    return M2M_ERR_FAIL;
#endif
}

int8_t m2m_wifi_connect_1x_tls(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuth1xTls *pstrAuth1xTls)
{
#ifdef CONF_WINC_SUPPORT_ENTERPRISE_CONNECT
    int8_t ret = M2M_ERR_INVALID_ARG;

    if (pstrAuth1xTls != NULL)
    {
        if (pstrAuth1xTls->pu8Domain == NULL)
        {
            pstrAuth1xTls->u16DomainLen = 0;
        }

        if (
            (pstrAuth1xTls->pu8UserName != NULL)
            &&  (pstrAuth1xTls->pu8PrivateKey_Mod != NULL)
            &&  (pstrAuth1xTls->pu8PrivateKey_Exp != NULL)
            &&  (pstrAuth1xTls->pu8Certificate != NULL)
            &&  ((uint32_t)(pstrAuth1xTls->u16DomainLen) + pstrAuth1xTls->u16UserNameLen <= M2M_AUTH_1X_USER_LEN_MAX)
            &&  (pstrAuth1xTls->u16PrivateKeyLen <= M2M_AUTH_1X_PRIVATEKEY_LEN_MAX)
            &&  (pstrAuth1xTls->u16CertificateLen <= M2M_AUTH_1X_CERT_LEN_MAX)
        )
        {
            tstrM2mWifiConnHdr  strConnHdr;
            uint16_t            u16AuthSize =   sizeof(tstrM2mWifi1xHdr) +
                                                pstrAuth1xTls->u16DomainLen +
                                                pstrAuth1xTls->u16UserNameLen +
                                                (2 * pstrAuth1xTls->u16PrivateKeyLen) +
                                                pstrAuth1xTls->u16CertificateLen;

#ifdef CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE

            if (u16AuthSize > CONF_WINC_ENTERPRISE_CONNECT_MEMORY_RESERVE_SIZE)
            {
                return M2M_ERR_FAIL;
            }

#endif
            ret = m2m_wifi_connect_prepare_msg( enuCredStoreOption,
                                                M2M_WIFI_SEC_802_1X,
                                                u16AuthSize,
                                                pstrNetworkId,
                                                &strConnHdr);

            if (ret == M2M_SUCCESS)
            {
                uint16_t            u16Payload1Size = u16AuthSize - pstrAuth1xTls->u16CertificateLen;
                tstrM2mWifi1xHdr    *pstr1xHdr;

#if defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_STACK)
                tuM2mWifi1xHdr      u1xHdr;

                pstr1xHdr = &u1xHdr.strHdr;
#elif defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_STATIC)
                pstr1xHdr = &u1xHdr.strHdr;
#elif defined(CONF_WINC_LARGE_MEMORY_ALLOC_STYLE_DYNAMIC)
                pstr1xHdr = (tstrM2mWifi1xHdr *)CONF_WINC_LARGE_MEMORY_ALLOC_FUNC(u16Payload1Size);
#endif

                if (pstr1xHdr != NULL)
                {
                    tstrM2mWifiAuthInfoHdr strInfoHdr = {0};
                    uint8_t *pu8AuthPtr = pstr1xHdr->au81xAuthDetails;

                    memset(pstr1xHdr, 0, u16Payload1Size);

                    pstr1xHdr->u8Flags = M2M_802_1X_TLS_FLAG;

                    if (pstrAuth1xTls->bUnencryptedUserName == true)
                    {
                        pstr1xHdr->u8Flags |= M2M_802_1X_UNENCRYPTED_USERNAME_FLAG;
                    }

                    if (pstrAuth1xTls->bPrependDomain == true)
                    {
                        pstr1xHdr->u8Flags |= M2M_802_1X_PREPEND_DOMAIN_FLAG;
                    }

                    pstr1xHdr->u8HdrLength = sizeof(tstrM2mWifi1xHdr);
                    pstr1xHdr->u32TlsHsFlags = gu321xTlsHsFlags;
                    memcpy(pstr1xHdr->au8TlsSpecificRootNameSha1, gau81xRootSha1, sizeof(gau81xRootSha1));

                    pstr1xHdr->u8DomainLength = 0;

                    if (pstrAuth1xTls->pu8Domain != NULL)
                    {
                        pstr1xHdr->u8DomainLength = (uint8_t)(pstrAuth1xTls->u16DomainLen);
                        memcpy(pu8AuthPtr, pstrAuth1xTls->pu8Domain, pstr1xHdr->u8DomainLength);
                        pu8AuthPtr += pstr1xHdr->u8DomainLength;
                    }

                    pstr1xHdr->u8UserNameLength = (pstrAuth1xTls->u16UserNameLen);
                    memcpy(pu8AuthPtr, pstrAuth1xTls->pu8UserName, pstr1xHdr->u8UserNameLength);
                    pu8AuthPtr += pstr1xHdr->u8UserNameLength;

                    pstr1xHdr->u16PrivateKeyOffset = (uint16_t)(pu8AuthPtr - pstr1xHdr->au81xAuthDetails);
                    pstr1xHdr->u16PrivateKeyLength = pstrAuth1xTls->u16PrivateKeyLen;
                    memcpy(pu8AuthPtr, pstrAuth1xTls->pu8PrivateKey_Mod, pstr1xHdr->u16PrivateKeyLength);
                    pu8AuthPtr += pstr1xHdr->u16PrivateKeyLength;
                    memcpy(pu8AuthPtr, pstrAuth1xTls->pu8PrivateKey_Exp, pstr1xHdr->u16PrivateKeyLength);
                    pu8AuthPtr += pstr1xHdr->u16PrivateKeyLength;

                    pstr1xHdr->u16CertificateOffset = (uint16_t)(pu8AuthPtr - pstr1xHdr->au81xAuthDetails);
                    pstr1xHdr->u16CertificateLength = pstrAuth1xTls->u16CertificateLen;

                    strInfoHdr.u8Type = M2M_802_1X_TLS_CLIENT_CERTIFICATE;
                    strInfoHdr.u16InfoPos = pstr1xHdr->u16CertificateOffset;
                    strInfoHdr.u16InfoLen = pstr1xHdr->u16CertificateLength;
                    ret = winc_hif_send(M2M_REQ_GROUP_WIFI, M2M_WIFI_IND_CONN_PARAM | M2M_REQ_DATA_PKT,
                                        &strInfoHdr, sizeof(tstrM2mWifiAuthInfoHdr),
                                        pstrAuth1xTls->pu8Certificate, pstrAuth1xTls->u16CertificateLen,
                                        sizeof(tstrM2mWifiAuthInfoHdr));

                    if (ret == M2M_SUCCESS)
                    {
                        ret = winc_hif_send(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CONN | M2M_REQ_DATA_PKT,
                                            &strConnHdr, sizeof(tstrM2mWifiConnHdr),
                                            pstr1xHdr, u16Payload1Size,
                                            sizeof(tstrM2mWifiConnHdr));
                    }

#ifdef CONF_WINC_LARGE_MEMORY_FREE_FUNC
                    CONF_WINC_LARGE_MEMORY_FREE_FUNC(pstr1xHdr);
#endif
                }
                else
                {
                    ret = M2M_ERR_MEM_ALLOC;
                }
            }
        }
    }

    return ret;
#else
    UNUSED_VAR(enuCredStoreOption);
    UNUSED_VAR(pstrNetworkId);
    UNUSED_VAR(pstrAuth1xTls);

    return M2M_ERR_FAIL;
#endif
}

int8_t m2m_wifi_connect(char *pcSsid, uint8_t u8SsidLen, uint8_t u8SecType, void *pvAuthInfo, uint16_t u16Ch)
{
    return m2m_wifi_connect_sc(pcSsid, u8SsidLen, u8SecType, pvAuthInfo, u16Ch, 0);
}

int8_t m2m_wifi_connect_sc(char *pcSsid, uint8_t u8SsidLen, uint8_t u8SecType, void *pvAuthInfo, uint16_t u16Ch, uint8_t u8NoSaveCred)
{
    int8_t              s8Ret              = M2M_ERR_INVALID_ARG;
    tstrNetworkId       strNetworkId       = {NULL, (uint8_t *)pcSsid, u8SsidLen, (tenuM2mScanCh)u16Ch};
    tenuCredStoreOption enuCredStoreOption = u8NoSaveCred ? WIFI_CRED_DONTSAVE : WIFI_CRED_SAVE_ENCRYPTED;

    /* This API does not support SSIDs which contain '\0'. If there is a '\0' character within the
     * first u8SsidLen characters, then assume that the input u8SsidLen was incorrect - set length
     * to strlen(pcSsid) and continue. This is to avoid a change from the behavior of previously
     * released drivers. */
    if (u8SsidLen < M2M_MAX_SSID_LEN)
        while (u8SsidLen--)
            if (strNetworkId.pu8Ssid[u8SsidLen] == 0)
            {
                strNetworkId.u8SsidLen = u8SsidLen;
            }

    switch ((tenuM2mSecType)u8SecType)
    {
        case M2M_WIFI_SEC_OPEN:
            s8Ret = m2m_wifi_connect_open(enuCredStoreOption, &strNetworkId);
            break;

        case M2M_WIFI_SEC_WPA_PSK:
            if (pvAuthInfo != NULL)
            {
                tstrAuthPsk strAuthPsk = {NULL, NULL, 0};
                uint16_t    len        = (uint16_t)strlen((const char *)pvAuthInfo);

                if (len == M2M_MAX_PSK_LEN - 1)
                {
                    strAuthPsk.pu8Psk = (uint8_t *)pvAuthInfo;
                }
                else
                {
                    strAuthPsk.pu8Passphrase   = (uint8_t *)pvAuthInfo;
                    strAuthPsk.u8PassphraseLen = (uint8_t)len;
                }

                s8Ret = m2m_wifi_connect_psk(enuCredStoreOption, &strNetworkId, &strAuthPsk);
            }

            break;

        case M2M_WIFI_SEC_WEP:
            if (pvAuthInfo != NULL)
            {
                tstrM2mWifiWepParams    *pstrWepParams = (tstrM2mWifiWepParams *)pvAuthInfo;
                tstrAuthWep             strAuthWep     = {pstrWepParams->au8WepKey, pstrWepParams->u8KeySz - 1, pstrWepParams->u8KeyIndx};

                s8Ret = m2m_wifi_connect_wep(enuCredStoreOption, &strNetworkId, &strAuthWep);
            }

            break;

        case M2M_WIFI_SEC_802_1X:
            if (pvAuthInfo != NULL)
            {
                tstr1xAuthCredentials   *pstr1xParams    = (tstr1xAuthCredentials *)pvAuthInfo;
                tstrAuth1xMschap2       strAuth1xMschap2 = {NULL,
                                                            pstr1xParams->au8UserName,
                                                            pstr1xParams->au8Passwd,
                                                            0,
                                                            (uint16_t)strlen((const char *)pstr1xParams->au8UserName),
                                                            (uint16_t)strlen((const char *)pstr1xParams->au8Passwd),
                                                            false,
                                                            false
                                                           };

                s8Ret = m2m_wifi_connect_1x_mschap2(enuCredStoreOption, &strNetworkId, &strAuth1xMschap2);
            }

            break;

        default:
            break;
    }

    return s8Ret;
}

int8_t m2m_wifi_disconnect(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_DISCONNECT, NULL, 0);
}

int8_t m2m_wifi_set_mac_address(uint8_t au8MacAddress[6])
{
    tstrM2mSetMacAddress strTmp;
    memcpy(strTmp.au8Mac, au8MacAddress, 6);
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_MAC_ADDRESS, &strTmp, sizeof(tstrM2mSetMacAddress));
}

int8_t m2m_wifi_set_static_ip(tstrM2MIPConfig *pstrStaticIPConf)
{
    pstrStaticIPConf->u32DNS = HOST_TO_WINC_U32(pstrStaticIPConf->u32DNS);
    pstrStaticIPConf->u32Gateway = HOST_TO_WINC_U32(pstrStaticIPConf->u32Gateway);
    pstrStaticIPConf->u32StaticIP = HOST_TO_WINC_U32(
                                        pstrStaticIPConf->u32StaticIP);
    pstrStaticIPConf->u32SubnetMask = HOST_TO_WINC_U32(
                                          pstrStaticIPConf->u32SubnetMask);
    return winc_hif_send_no_data(M2M_REQ_GROUP_IP, M2M_IP_REQ_STATIC_IP_CONF, pstrStaticIPConf, sizeof(tstrM2MIPConfig));
}

int8_t m2m_wifi_enable_dhcp(uint8_t u8DhcpEn)
{
    uint8_t u8Req;
    u8Req = u8DhcpEn ? M2M_IP_REQ_ENABLE_DHCP : M2M_IP_REQ_DISABLE_DHCP;
    return winc_hif_send_no_data(M2M_REQ_GROUP_IP, u8Req, NULL, 0);
}

int8_t m2m_wifi_set_lsn_int(tstrM2mLsnInt *pstrM2mLsnInt)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_LSN_INT, pstrM2mLsnInt, sizeof(tstrM2mLsnInt));
}

int8_t m2m_wifi_set_cust_InfoElement(uint8_t *pau8M2mCustInfoElement)
{
    int8_t ret = M2M_ERR_FAIL;

    if (pau8M2mCustInfoElement != NULL)
    {
        if ((pau8M2mCustInfoElement[0] + 1) < M2M_CUST_IE_LEN_MAX)
        {
            ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CUST_INFO_ELEMENT | M2M_REQ_DATA_PKT, pau8M2mCustInfoElement, pau8M2mCustInfoElement[0] + 1);
        }
    }

    return ret;
}

int8_t m2m_wifi_set_scan_options(tstrM2MScanOption *ptstrM2MScanOption)
{
    int8_t  s8Ret = M2M_ERR_FAIL;

    if (m2m_validate_scan_options(ptstrM2MScanOption) == M2M_SUCCESS)
    {
        s8Ret =  winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_SCAN_OPTION, ptstrM2MScanOption, sizeof(tstrM2MScanOption));
    }

    return s8Ret;
}

int8_t m2m_wifi_set_stop_scan_on_first(uint8_t u8StopScanOption)
{
    int8_t  s8Ret = M2M_ERR_FAIL;

    tstrM2MStopScanOption StopScanOption = { 0 };

    if (1 >= u8StopScanOption)
    {
        StopScanOption.u8StopOnFirstResult = u8StopScanOption;

        s8Ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_STOP_SCAN_OPTION, &StopScanOption, sizeof(tstrM2MStopScanOption));

        WINC_LOG_INFO("Scan will %sstop on first result.", StopScanOption.u8StopOnFirstResult ? "" : "NOT ");
    }
    else
    {
        s8Ret = M2M_ERR_INVALID_ARG;
    }

    return s8Ret;
}

int8_t m2m_wifi_set_scan_region(uint16_t u16ScanRegion)
{
    tstrM2MScanRegion strScanRegion;

    strScanRegion.u16ScanRegion = u16ScanRegion;
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_SCAN_REGION, &strScanRegion, sizeof(tstrM2MScanRegion));
}

int8_t m2m_wifi_request_scan(uint8_t u8Ch)
{
    int8_t s8Ret = M2M_SUCCESS;

    if (!gu8scanInProgress)
    {
        if (((u8Ch >= M2M_WIFI_CH_1) && (u8Ch <= M2M_WIFI_CH_14)) || (u8Ch == M2M_WIFI_CH_ALL))
        {
            tstrM2MScan strtmp;
            strtmp.u8ChNum = u8Ch;
            s8Ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SCAN, &strtmp, sizeof(tstrM2MScan));

            if (s8Ret == M2M_SUCCESS)
            {
                gu8scanInProgress = 1;
            }
        }
        else
        {
            s8Ret = M2M_ERR_INVALID_ARG;
        }
    }
    else
    {
        s8Ret = M2M_ERR_SCAN_IN_PROGRESS;
    }

    return s8Ret;
}

int8_t m2m_wifi_request_scan_passive(uint8_t u8Ch, uint16_t u16ScanTime)
{
    int8_t  s8Ret = M2M_SUCCESS;

    if (!gu8scanInProgress)
    {
        if (((u8Ch >= M2M_WIFI_CH_1) && (u8Ch <= M2M_WIFI_CH_14)) || (u8Ch == M2M_WIFI_CH_ALL))
        {
            tstrM2MScan strtmp;
            strtmp.u8ChNum = u8Ch;

            strtmp.u16PassiveScanTime = u16ScanTime;

            s8Ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_PASSIVE_SCAN, &strtmp, sizeof(tstrM2MScan));

            if (s8Ret == M2M_SUCCESS)
            {
                gu8scanInProgress = 1;
            }
        }
        else
        {
            s8Ret = M2M_ERR_INVALID_ARG;
        }
    }
    else
    {
        s8Ret = M2M_ERR_SCAN_IN_PROGRESS;
    }

    return s8Ret;
}

int8_t m2m_wifi_request_scan_ssid_list(uint8_t u8Ch, uint8_t *pu8Ssidlist)
{
    int8_t  s8Ret = M2M_ERR_INVALID_ARG;

    if (!gu8scanInProgress)
    {
        if ((((u8Ch >= M2M_WIFI_CH_1) && (u8Ch <= M2M_WIFI_CH_14)) || (u8Ch == M2M_WIFI_CH_ALL)) && (pu8Ssidlist != NULL))
        {
            tstrM2MScan strtmp;
            uint16_t u16Lsize = 0;
            uint8_t u8Apnum = pu8Ssidlist[u16Lsize];

            if (u8Apnum <= MAX_HIDDEN_SITES)
            {
                u16Lsize++;

                while (u8Apnum)
                {
                    if (pu8Ssidlist[u16Lsize] >= M2M_MAX_SSID_LEN)
                    {
                        goto EXIT;
                    }
                    else
                    {
                        u16Lsize += pu8Ssidlist[u16Lsize] + 1;
                        u8Apnum--;
                    }
                }

                strtmp.u8ChNum = u8Ch;
                s8Ret = winc_hif_send(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SCAN_SSID_LIST | M2M_REQ_DATA_PKT, &strtmp, sizeof(tstrM2MScan), pu8Ssidlist, u16Lsize, sizeof(tstrM2MScan));

                if (s8Ret == M2M_SUCCESS)
                {
                    gu8scanInProgress = 1;
                }
            }
        }
    }
    else
    {
        s8Ret = M2M_ERR_SCAN_IN_PROGRESS;
    }

EXIT:
    return s8Ret;
}

int8_t m2m_wifi_wps(uint8_t u8TriggerType, const char *pcPinNumber)
{
    tstrM2MWPSConnect strtmp;

    /* Stop Scan if it is ongoing.
    */
    gu8scanInProgress = 0;
    strtmp.u8TriggerType = u8TriggerType;

    /*If WPS is using PIN METHOD*/
    if (u8TriggerType == WPS_PIN_TRIGGER)
    {
        memcpy(strtmp.acPinNumber, pcPinNumber, 8);
    }

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_WPS, &strtmp, sizeof(tstrM2MWPSConnect));
}

int8_t m2m_wifi_wps_disable(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_DISABLE_WPS, NULL, 0);
}

int8_t m2m_wifi_req_client_ctrl(uint8_t u8Cmd)
{
#ifdef _PS_SERVER_
    tstrM2Mservercmd    strCmd;

    strCmd.u8cmd = u8Cmd;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CLIENT_CTRL, &strCmd, sizeof(tstrM2Mservercmd));
#else
    UNUSED_VAR(u8Cmd);
    WINC_LOG_ERROR("_PS_SERVER_ is not defined");

    return M2M_ERR_FAIL;
#endif
}

int8_t m2m_wifi_req_server_init(uint8_t u8Ch)
{
#ifdef _PS_SERVER_
    tstrM2mServerInit strServer;

    strServer.u8Channel = u8Ch;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SERVER_INIT, &strServer, sizeof(tstrM2mServerInit));
#else
    UNUSED_VAR(u8Ch);
    WINC_LOG_ERROR("_PS_SERVER_ is not defined");

    return M2M_ERR_FAIL;
#endif
}

int8_t m2m_wifi_enable_ap(const tstrM2MAPConfig *pstrM2MAPConfig)
{
    tstrM2MAPModeConfig strM2MAPModeConfig;

    memcpy(&strM2MAPModeConfig.strApConfig, pstrM2MAPConfig, sizeof(tstrM2MAPConfig));

    memcpy(strM2MAPModeConfig.strApConfigExt.au8DefRouterIP, pstrM2MAPConfig->au8DHCPServerIP, 4);
    memcpy(strM2MAPModeConfig.strApConfigExt.au8DNSServerIP, pstrM2MAPConfig->au8DHCPServerIP, 4);

    strM2MAPModeConfig.strApConfigExt.au8SubnetMask[0] = 0;

    return m2m_wifi_enable_ap_ext(&strM2MAPModeConfig);
}

int8_t m2m_wifi_enable_ap_ext(const tstrM2MAPModeConfig *pstrM2MAPModeConfig)
{
    int8_t ret = M2M_ERR_FAIL;

    if (M2M_SUCCESS == m2m_validate_ap_parameters(pstrM2MAPModeConfig))
    {
        ret = winc_hif_send(M2M_REQ_GROUP_WIFI, (M2M_REQ_DATA_PKT | M2M_WIFI_REQ_ENABLE_AP), NULL, 0, pstrM2MAPModeConfig, sizeof(tstrM2MAPModeConfig), 0);
    }

    return ret;
}

int8_t m2m_wifi_set_gains(tstrM2mWifiGainsParams *pstrM2mGain)
{
    int8_t ret = M2M_ERR_FAIL;

    if (pstrM2mGain != NULL)
    {
        ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_GAINS, pstrM2mGain, sizeof(tstrM2mWifiGainsParams));
    }

    return ret;
}

int8_t m2m_wifi_disable_ap(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_DISABLE_AP, NULL, 0);
}

int8_t m2m_wifi_req_curr_rssi(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CURRENT_RSSI, NULL, 0);
}

int8_t m2m_wifi_send_ethernet_pkt(uint8_t *pu8Packet, uint16_t u16PacketSize)
{
    int8_t  s8Ret = -1;

    if ((pu8Packet != NULL) && (u16PacketSize > 0))
    {
        tstrM2MWifiTxPacketInfo     strTxPkt;

        strTxPkt.u16PacketSize      = u16PacketSize;
        strTxPkt.u16HeaderLength    = M2M_ETHERNET_HDR_LEN;
        s8Ret = winc_hif_send(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SEND_ETHERNET_PACKET | M2M_REQ_DATA_PKT,
                              &strTxPkt, sizeof(tstrM2MWifiTxPacketInfo), pu8Packet, u16PacketSize, M2M_ETHERNET_HDR_OFFSET - M2M_HIF_HDR_OFFSET);
    }

    return s8Ret;
}

int8_t m2m_wifi_get_otp_mac_address(uint8_t *pu8MacAddr, uint8_t *pu8IsValid)
{
    uint8_t u8IsValid = 0;

    if (M2M_SUCCESS != winc_hif_chip_wake())
    {
        return M2M_ERR_FAIL;
    }

    if (winc_chip_get_otp_mac_address(pu8MacAddr))
    {
        u8IsValid = 1;
    }

    if (M2M_SUCCESS != winc_hif_chip_sleep())
    {
        return M2M_ERR_FAIL;
    }

    if (pu8IsValid)
    {
        *pu8IsValid = u8IsValid;
    }

    return M2M_SUCCESS;
}

int8_t m2m_wifi_get_mac_address(uint8_t *pu8MacAddr)
{
    if (M2M_SUCCESS != winc_hif_chip_wake())
    {
        return M2M_ERR_FAIL;
    }

    if (!winc_chip_get_mac_address(pu8MacAddr))
    {
        return M2M_ERR_FAIL;
    }

    return winc_hif_chip_sleep();
}

int8_t m2m_wifi_req_scan_result(uint8_t u8Index)
{
    tstrM2mReqScanResult strReqScanRlt;

    strReqScanRlt.u8Index = u8Index;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SCAN_RESULT, &strReqScanRlt, sizeof(tstrM2mReqScanResult));
}

uint8_t m2m_wifi_get_num_ap_found(void)
{
    return gu8ChNum;
}

uint8_t m2m_wifi_get_sleep_mode(void)
{
    return (uint8_t)genuPsMode;
}

int8_t m2m_wifi_set_sleep_mode(uint8_t u8PsTyp, uint8_t u8BcastEn)
{
    int8_t ret = M2M_SUCCESS;
    tstrM2mPsType strPs;
    strPs.u8PsType = u8PsTyp;
    strPs.u8BcastEn = u8BcastEn;
    ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SLEEP, &strPs, sizeof(tstrM2mPsType));
    WINC_LOG_INFO("POWER SAVE %u", u8PsTyp);
    genuPsMode = u8PsTyp;
    winc_hif_set_power_save((u8PsTyp != M2M_NO_PS) ? true : false);
    return ret;
}

int8_t m2m_wifi_request_sleep(uint32_t u32SlpReqTime)
{
    int8_t ret = M2M_SUCCESS;

    if (genuPsMode == M2M_PS_MANUAL)
    {
        tstrM2mSlpReqTime strPs;
        strPs.u32SleepTime = u32SlpReqTime;
        ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_DOZE, &strPs, sizeof(tstrM2mSlpReqTime));
    }

    return ret;
}

int8_t m2m_wifi_set_device_name(uint8_t *pu8DeviceName, uint8_t u8DeviceNameLength)
{
    tstrM2MDeviceNameConfig strDeviceName;

    u8DeviceNameLength++;

    if (u8DeviceNameLength >= M2M_DEVICE_NAME_MAX)
    {
        u8DeviceNameLength = M2M_DEVICE_NAME_MAX;
    }

    memcpy(strDeviceName.au8DeviceName, pu8DeviceName, u8DeviceNameLength);
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_DEVICE_NAME, &strDeviceName, sizeof(tstrM2MDeviceNameConfig));
}

int8_t m2m_wifi_configure_sntp(uint8_t *pu8NTPServerName, uint8_t u8NTPServerNameLength, tenuSNTPUseDHCP enuUseDHCP)
{
    tstrM2MSNTPConfig strSNTPConfig;

    if (u8NTPServerNameLength > M2M_NTP_MAX_SERVER_NAME_LENGTH)
    {
        return M2M_ERR_FAIL;
    }

    memcpy(strSNTPConfig.acNTPServer, pu8NTPServerName, u8NTPServerNameLength);
    strSNTPConfig.acNTPServer[u8NTPServerNameLength] = '\0';
    strSNTPConfig.enuUseDHCP                         = (uint8_t)enuUseDHCP;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CONFIG_SNTP, &strSNTPConfig, sizeof(tstrM2MSNTPConfig));
}

int8_t m2m_wifi_get_firmware_version(tstrM2mRev *pstrRev)
{
    int8_t ret;

    ret = winc_hif_chip_wake();

    if (ret != M2M_SUCCESS)
    {
        return ret;
    }

    ret = m2m_fwinfo_get_firmware_info(true, pstrRev);
    winc_hif_chip_sleep();

    if (ret != M2M_SUCCESS)
    {
        return ret;
    }

    return m2m_fwinfo_version_check(pstrRev);
}

int8_t m2m_wifi_start_provision_mode(tstrM2MAPConfig *pstrM2MAPConfig, char *pcHttpServerDomainName, uint8_t bEnableHttpRedirect)
{
    tstrM2MAPModeConfig strM2MAPModeConfig;

    memcpy(&strM2MAPModeConfig.strApConfig, pstrM2MAPConfig, sizeof(tstrM2MAPConfig));

    memcpy(strM2MAPModeConfig.strApConfigExt.au8DefRouterIP, pstrM2MAPConfig->au8DHCPServerIP, 4);
    memcpy(strM2MAPModeConfig.strApConfigExt.au8DNSServerIP, pstrM2MAPConfig->au8DHCPServerIP, 4);

    strM2MAPModeConfig.strApConfigExt.au8SubnetMask[0] = 0;

    return m2m_wifi_start_provision_mode_ext(&strM2MAPModeConfig, pcHttpServerDomainName, bEnableHttpRedirect);
}

int8_t m2m_wifi_start_provision_mode_ext(tstrM2MAPModeConfig *pstrAPModeConfig, char *pcHttpServerDomainName, uint8_t bEnableHttpRedirect)
{
    int8_t  s8Ret = M2M_ERR_FAIL;

    if (pstrAPModeConfig != NULL)
    {
        tstrM2MProvisionModeConfig  strProvConfig;

        if (M2M_SUCCESS == m2m_validate_ap_parameters(pstrAPModeConfig))
        {
            memcpy(&strProvConfig.strApConfig, &pstrAPModeConfig->strApConfig, sizeof(tstrM2MAPConfig));
            memcpy(&strProvConfig.strApConfigExt, &pstrAPModeConfig->strApConfigExt, sizeof(tstrM2MAPConfigExt));

            if ((strlen((const char *)pcHttpServerDomainName) <= 0) || (NULL == pcHttpServerDomainName))
            {
                WINC_LOG_ERROR("INVALID DOMAIN NAME");
                goto ERR1;
            }

            memcpy(strProvConfig.acHttpServerDomainName, pcHttpServerDomainName, 64);
            strProvConfig.u8EnableRedirect = bEnableHttpRedirect;

            /* Stop Scan if it is ongoing.
            */
            gu8scanInProgress = 0;
            s8Ret = winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_START_PROVISION_MODE | M2M_REQ_DATA_PKT,
                                          &strProvConfig, sizeof(tstrM2MProvisionModeConfig));
        }
        else
        {
            /*goto ERR1;*/
        }
    }

ERR1:
    return s8Ret;
}

int8_t m2m_wifi_stop_provision_mode(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_STOP_PROVISION_MODE, NULL, 0);
}

int8_t m2m_wifi_get_connection_info(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_GET_CONN_INFO, NULL, 0);
}

int8_t m2m_wifi_set_system_time(uint32_t u32UTCSeconds)
{
    /*
        The firmware accepts timestamps relative to 1900 like NTP Timestamp.
    */
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_SYS_TIME, &u32UTCSeconds, sizeof(tstrSystemTime));
}

int8_t m2m_wifi_get_system_time(void)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_GET_SYS_TIME, NULL, 0);
}

int8_t m2m_wifi_enable_sntp(uint8_t u8Enable)
{
    uint8_t u8Req;

    u8Req = u8Enable ? M2M_WIFI_REQ_ENABLE_SNTP_CLIENT : M2M_WIFI_REQ_DISABLE_SNTP_CLIENT;
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, u8Req, NULL, 0);
}

int8_t m2m_wifi_set_power_profile(uint8_t u8PwrMode)
{
    tstrM2mPwrMode strM2mPwrMode;

    strM2mPwrMode.u8PwrMode = u8PwrMode;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_POWER_PROFILE, &strM2mPwrMode, sizeof(tstrM2mPwrMode));
}

int8_t m2m_wifi_set_tx_power(uint8_t u8TxPwrLevel)
{
    tstrM2mTxPwrLevel strM2mTxPwrLevel;

    strM2mTxPwrLevel.u8TxPwrLevel = u8TxPwrLevel;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_TX_POWER, &strM2mTxPwrLevel, sizeof(tstrM2mTxPwrLevel));
}

int8_t m2m_wifi_set_gain_table_idx(uint8_t u8GainTableIdx)
{
    tstrM2mWiFiGainIdx strM2mGainTableIdx;

    strM2mGainTableIdx.u8GainTableIdx = u8GainTableIdx;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_GAIN_TABLE_IDX, &strM2mGainTableIdx, sizeof(tstrM2mWiFiGainIdx));
}

int8_t m2m_wifi_enable_firmware_logs(uint8_t u8Enable)
{
    tstrM2mEnableLogs strM2mEnableLogs;

    strM2mEnableLogs.u8Enable = u8Enable;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_ENABLE_LOGS, &strM2mEnableLogs, sizeof(tstrM2mEnableLogs));
}

int8_t m2m_wifi_set_battery_voltage(uint16_t u16BattVoltx100)
{
    tstrM2mBatteryVoltage strM2mBattVol = {0};

    strM2mBattVol.u16BattVolt = u16BattVoltx100;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_BATTERY_VOLTAGE, &strM2mBattVol, sizeof(tstrM2mBatteryVoltage));
}

int8_t m2m_wifi_prng_get_random_bytes(uint8_t *pu8PrngBuff, uint16_t u16PrngSize)
{
    tstrPrng   strRng = {0};

    if ((u16PrngSize <= (M2M_HIF_MAX_PACKET_SIZE - (M2M_HIF_HDR_OFFSET + sizeof(tstrPrng)))) && (pu8PrngBuff != NULL))
    {
        strRng.u16PrngSize = u16PrngSize;
        strRng.pu8RngBuff  = pu8PrngBuff;
        return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_GET_PRNG | M2M_REQ_DATA_PKT, &strRng, sizeof(tstrPrng));
    }

    WINC_LOG_ERROR("PRNG Buffer exceeded maximum size %u or NULL Buffer", u16PrngSize);

    return M2M_ERR_FAIL;
}

int8_t m2m_wifi_conf_auto_rate(tstrConfAutoRate *pstrConfAutoRate)
{
    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_CONG_AUTO_RATE, pstrConfAutoRate, sizeof(tstrConfAutoRate));
}

#ifdef ETH_MODE
int8_t m2m_wifi_enable_mac_mcast(uint8_t *pu8MulticastMacAddress, uint8_t u8AddRemove)
{
    tstrM2MMulticastMac  strMulticastMac;

    if (pu8MulticastMacAddress == NULL)
    {
        return M2M_ERR_FAIL;
    }

    strMulticastMac.u8AddRemove = u8AddRemove;

    memcpy(strMulticastMac.au8macaddress, pu8MulticastMacAddress, M2M_MAC_ADDRES_LEN);

    WINC_LOG_DEBUG("mac multicast: %x:%x:%x:%x:%x:%x", strMulticastMac.au8macaddress[0], strMulticastMac.au8macaddress[1], strMulticastMac.au8macaddress[2],
                   strMulticastMac.au8macaddress[3], strMulticastMac.au8macaddress[4], strMulticastMac.au8macaddress[5]);

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_SET_MAC_MCAST, &strMulticastMac, sizeof(tstrM2MMulticastMac));
}

int8_t m2m_wifi_set_receive_buffer(void *pvBuffer, uint16_t u16BufferLen)
{
    if (pvBuffer == NULL)
    {
        WINC_LOG_ERROR("Buffer NULL pointer");
        return M2M_ERR_FAIL;
    }

    gau8ethRcvBuf = pvBuffer;
    gu16ethRcvBufSize = u16BufferLen;

    return M2M_SUCCESS;
}
#endif /* ETH_MODE */

uint8_t m2m_wifi_get_state(void)
{
    return gu8WifiState;
}

int8_t m2m_wifi_enable_roaming(uint8_t u8EnableDhcp)
{
    tstrM2mWiFiRoaming  strWiFiRoaming;
    strWiFiRoaming.u8EnableRoaming = 1;

    if (u8EnableDhcp > 1)
    {
        return M2M_ERR_INVALID_ARG;
    }

    strWiFiRoaming.u8EnableDhcp = u8EnableDhcp;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_ROAMING, &strWiFiRoaming, sizeof(tstrM2mWiFiRoaming));
}

int8_t m2m_wifi_disable_roaming(void)
{
    tstrM2mWiFiRoaming  strWiFiRoaming;

    strWiFiRoaming.u8EnableRoaming = 0;

    return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_ROAMING, &strWiFiRoaming, sizeof(tstrM2mWiFiRoaming));
}

int8_t m2m_wifi_enable_XO_during_sleep(uint8_t bXOSleepEnable)
{
    tstrM2mWiFiXOSleepEnable  strM2mWiFiXOSleepEnable;

    if ((0 == bXOSleepEnable) || (1 == bXOSleepEnable))
    {
        strM2mWiFiXOSleepEnable.u8EnableXODuringSleep = bXOSleepEnable;
        return winc_hif_send_no_data(M2M_REQ_GROUP_WIFI, M2M_WIFI_REQ_XO_SLEEP_ENABLE,
                                     (uint8_t *)&strM2mWiFiXOSleepEnable, sizeof(strM2mWiFiXOSleepEnable));
    }
    else
    {
        return M2M_ERR_INVALID_ARG;
    }
}
