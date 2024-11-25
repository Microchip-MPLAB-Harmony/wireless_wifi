/*******************************************************************************
  WINC Wireless Driver Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc.h

  Summary:
    WINC Wireless Driver Header File

  Description:
    The WINC device driver provides an interface to manage the WINC device.
    This file contains the main driver descriptor structure and ancillary functions
    and definitions.

    Other API's are provided in other header files, specifically:
      assoc       - Current association.
      bssfind     - BSS scan functionality.
      dhcps       - DHCP server functionality.
      dns         - DNS client functionality.
      extcrypto   - Crypto operations external to the WINC device.
      file        - File operation functionality.
      mqtt        - MQTT client functionality.
      netif       - Network interface functionality.
      ota         - OTA functionality.
      prov        - Provisioning functionality.
      sntp        - SNTP client functionality.
      socket      - TCP/IP sockets.
      softap      - Soft-AP mode.
      sta         - Infrastructure stations mode.
      systime     - System time.
      tls         - TLS functionality.
      wifi        - WiFi configuration functionality.

      authctx     - Authentication context management.
      bssctx      - BSS context management.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef WDRV_WINC_H
#define WDRV_WINC_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "configuration.h"
#include "definitions.h"
#include "wdrv_winc_bssfind.h"
#include "wdrv_winc_assoc.h"
#include "wdrv_winc_file.h"
#include "wdrv_winc_systime.h"
#include "wdrv_winc_softap.h"
#include "wdrv_winc_sta.h"
#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
#include "wdrv_winc_socket.h"
#endif
#include "wdrv_winc_netif.h"
#ifndef WDRV_WINC_MOD_DISABLE_DHCPS
#include "wdrv_winc_dhcps.h"
#endif
#include "wdrv_winc_tls.h"
#ifndef WDRV_WINC_MOD_DISABLE_MQTT
#include "wdrv_winc_mqtt.h"
#endif
#include "wdrv_winc_extcrypto.h"
#ifndef WDRV_WINC_MOD_DISABLE_DNS
#include "wdrv_winc_dns.h"
#endif
#include "wdrv_winc_wifi.h"
#ifndef WDRV_WINC_MOD_DISABLE_SNTP
#include "wdrv_winc_sntp.h"
#endif
#ifndef WDRV_WINC_MOD_DISABLE_PROV
#include "wdrv_winc_prov.h"
#endif
#ifndef WDRV_WINC_MOD_DISABLE_OTA
#include "wdrv_winc_ota.h"
#endif
#ifndef WDRV_WINC_MOD_DISABLE_NVM
#include "wdrv_winc_nvm.h"
#endif

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

/* Number of WiFi associations allowed. */
#define WDRV_WINC_NUM_ASSOCS            8U

/* Number of TLS contexts. */
#define WDRV_WINC_TLS_CTX_NUM           2U

/* Number of cipher suites. */
#define WDRV_WINC_TLS_CIPHER_SUITE_NUM  2U

/* Number of file contexts. */
#define WDRV_WINC_FILE_CTX_NUM          1U

// *****************************************************************************
/* L2 Data Frame Monitor Callback Function Pointer

  Function:
    void (*WDRV_WINC_L2DATA_MONITOR_CALLBACK)
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        const uint8_t *const pL2DataPtr,
        size_t l2DataLen
    )

  Summary:
    Pointer to a callback function for receiving L2 data frames.

  Description:
    This defines a function pointer for a callback to receive L2 data frames.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx        - Network interface index.
    pL2DataPtr   - Pointer to L2 data frame payload.
    l2DataLen    - Length of L2 data frame payload.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_L2DATA_MONITOR_CALLBACK)
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    const uint8_t *const pL2DataPtr,
    size_t l2DataLen
);

// *****************************************************************************
/*  Firmware Version Information

  Summary:
    Defines the version information of the firmware.

  Description:
    This data type defines the version information of the firmware running on
    the WINC.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if this information is valid. */
    bool isValid;

    /* Version number structure. */
    struct
    {
        /* Major version number. */
        uint16_t major;

        /* Major minor number. */
        uint16_t minor;

        /* Major patch number. */
        uint16_t patch;
    } version;

    /* Build date/time structure. */
    struct
    {
        /* Build hash. */
        uint8_t hash[WINC_CFG_PARAM_SZ_CFG_BUILD_HASH];

        /* Build time. */
        uint32_t timeUTC;
    } build;
} WDRV_WINC_FIRMWARE_VERSION_INFO;

// *****************************************************************************
/*  Device Information

  Summary:
    Defines the device information.

  Description:
    This data type defines the device information of the WINC.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if this information is valid. */
    bool isValid;

    /* WINC device ID. */
    uint32_t id;

    /* Number of flash images. */
    uint8_t numImages;

    /* Information for each device image. */
    struct
    {
        /* Sequence number. */
        uint32_t seqNum;

        /* Version information. */
        uint32_t version;

        /* Source address. */
        uint32_t srcAddr;
    } image[WINC_CFG_PARAM_NUM_DI_IMAGE_INFO];
} WDRV_WINC_DEVICE_INFO;

// *****************************************************************************
/*  Driver Version Information

  Summary:
    Defines the version information of the driver.

  Description:
    This data type defines the version information of the driver.

  Remarks:
    None.
*/

typedef struct
{
    /* Version number structure. */
    struct
    {
        /* Major version number. */
        uint16_t major;

        /* Major minor number. */
        uint16_t minor;

        /* Major patch number. */
        uint16_t patch;
    } version;
} WDRV_WINC_DRIVER_VERSION_INFO;

// *****************************************************************************
/*  WINC Control Driver Descriptor

  Summary:
    Driver descriptor for the WINC Control driver.

  Description:
    Structure containing the system level descriptor for the WINC driver.
    This structure is initialized by a call to WDRV_WINC_Initialize.

  Remarks:
    None.
*/

typedef struct
{
    /* Primary driver handle. */
    DRV_HANDLE handle;

    /* Extended system status which can be queried via WDRV_WINC_StatusExt. */
    WDRV_WINC_SYS_STATUS extSysStat;

    /* WINC device handle. */
    WINC_DEVICE_HANDLE wincDevHandle;

    /* Current state of WINC device SDIO. */
    WINC_SDIO_STATE_TYPE wincSDIOState;

    /* Flag indicating if WINC device was hard reset. */
    bool wincHardReset;

    /* WINC device receive buffer pointer. */
    void *pWINCDevReceiveBuffer;

    /* Timeout counter for WINC device SDIO reset. */
    uint8_t wincSDIOResetTimeoutCount;

    /* Interrupt source. */
    int intSrc;

    /* Delay timer handle. */
    SYS_TIME_HANDLE delayTimer;

    /* Firmware version information. */
    WDRV_WINC_FIRMWARE_VERSION_INFO fwVersion;

    /* Device information. */
    WDRV_WINC_DEVICE_INFO devInfo;

    /* Flag indicating if this instance is operating as s station or soft-AP. */
    bool isAP;

    /* Variable to store the state of the connection. */
    WDRV_WINC_CONN_STATE connectedState;

    /* Flag indicating if a BSS scan is currently in progress. */
    bool scanInProgress;

    /* Current index of the BSS scan results. */
    uint8_t scanIndex;

    /* Flag indicating if the scan parameters have been modified. */
    bool scanParamDefault;

    /* Number of available reg-domains. */
    uint8_t availRegDomNum;

    /* Currently active reg-domain. */
    WDRV_WINC_REGDOMAIN_INFO activeRegDomain;

    /* A set regulatory domain operation is in progress. */
    bool regDomainSetInProgress;

    /* The number of scan slots per channel. */
    uint8_t scanNumSlots;

    /* Number of probe requests to be sent each scan slot. */
    uint8_t scanNumProbes;

    /* Time spent on each active channel probing for BSS's. */
    uint16_t scanActiveScanTime;

    /* Time spent on each passive channel listening for beacons. */
    uint16_t scanPassiveListenTime;

    /* Main event semaphore. */
    OSAL_SEM_HANDLE_TYPE drvEventSemaphore;

    /* Bitmap of enabled 2.4GHz channels for multi-channel scanning. */
    WDRV_WINC_CHANNEL24_MASK scanChannelMask24;

    /* Scanning RSSI threshold. */
    int8_t scanRssiThreshold;

    /* Bitmap of enabled 2.4GHz channels for the WINC. */
    WDRV_WINC_CHANNEL24_MASK regulatoryChannelMask24;

    /* Current operating channel. */
    WDRV_WINC_CHANNEL_ID opChannel;

    /* Association information (STA). */
    WDRV_WINC_ASSOC_INFO assocInfoSTA;

    /* Association information (AP). */
    WDRV_WINC_ASSOC_INFO assocInfoAP[WDRV_WINC_NUM_ASSOCS];

#ifndef WDRV_WINC_MOD_DISABLE_TLS
    /* TLS context information */
    WDRV_WINC_TLSCTX_INFO tlscInfo[WDRV_WINC_TLS_CTX_NUM];

    struct
    {
        /* TLS cipher suite information */
        WDRV_WINC_TLSCSCTX_INFO tlsCsInfo[WDRV_WINC_TLS_CIPHER_SUITE_NUM];

        /* Number of cipher suites returned. */
        uint8_t numAlgorithms;

        /* List of cipher suites returned. */
        uint16_t algorithms[WINC_CFG_PARAM_NUM_TLS_CSL_CIPHER_SUITES];

        /* Callback to use for getting TLS cipher suite configurations. */
        WDRV_WINC_TLS_CS_CALLBACK pfTlsCsResponseCB;
    } tlsCipherSuites;
#endif
    /* File operation contexts. */
    WDRV_WINC_FILE_CTX fileCtx[WDRV_WINC_FILE_CTX_NUM];

    union
    {
        /* File find callback. */
        WDRV_WINC_FILE_FIND_CALLBACK pfFileFindCB;

        /* File delete callback. */
        WDRV_WINC_FILE_DELETE_CALLBACK pfFileDeleteCB;
    };

    /* File operation user context for callbacks. */
    uintptr_t fileOpUserCtx;

    union
    {
        struct
        {
            uint8_t confValid : 1;
            uint8_t enabled : 1;
            uint8_t use2Wire : 1;
            uint8_t wlanRxHigherThanBt : 1;
            uint8_t wlanTxHigherThanBt : 1;
            uint8_t sharedAntenna : 1;
        };
        uint8_t flags;
    } coex;

#ifndef WDRV_WINC_MOD_DISABLE_MQTT
    struct
    {
        /* MQTT protocol version. */
        WDRV_WINC_MQTT_PROTO_VER protocolVer;

        /* MQTT connection status callback. */
        WDRV_WINC_MQTT_CONN_STATUS_CALLBACK pfConnCB;

        /* MQTT connection state. */
        WDRV_WINC_MQTT_CONN_STATUS_TYPE connState;

        /* Connection information. */
        WDRV_WINC_MQTT_CONN_INFO connInfo;

        /* User context for MQTT connection status callback. */
        uintptr_t connCbCtx;

        /* Publish status callback function pointer. */
        WDRV_WINC_MQTT_PUB_STATUS_CALLBACK pfPubStatusCb;

        /* Publish status callback user context. */
        uintptr_t pubStatusCbCtx;

        /* Subscribe callback function pointer. */
        WDRV_WINC_MQTT_SUBSCRIBE_CALLBACK pfSubscribeCb;

        /* Subscribe callback user context. */
        uintptr_t subscribeCbCtx;

        /* Publish message properties. */
        WDRV_WINC_MQTT_PUB_PROP pubProps;

        /* Flag indicating if user properties are included. */
        bool includeUserProps;

        /* User property callback function pointer. */
        WDRV_WINC_MQTT_USER_PROP_CALLBACK pfUserPropCb;
    } mqtt;
#endif

    /* Callback to use for BSS find operations. */
    WDRV_WINC_BSSFIND_NOTIFY_CALLBACK pfBSSFindNotifyCB;

    /* Callback to use for notifying changes in connection state. */
    WDRV_WINC_BSSCON_NOTIFY_CALLBACK pfConnectNotifyCB;

    /* Callback to use for events relating to the WINC system time. */
    WDRV_WINC_SYSTIME_CURRENT_CALLBACK pfSystemTimeGetCurrentCB;

    /* Callback to use for retrieving association RSSI information from the WINC. */
    WDRV_WINC_ASSOC_RSSI_CALLBACK pfAssociationRSSICB;

#ifndef WDRV_WINC_MOD_DISABLE_OTA
    /* State of OTA operation. */
    WDRV_WINC_OTA_OPERATION_STATE otaState;
#endif
#ifndef WDRV_WINC_MOD_DISABLE_NVM
    /* State of NVM operation. */
    WDRV_WINC_NVM_OPERATION_STATE nvmState;
#endif
#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
    /* Callback to use for ICHO echo responses. */
    WDRV_WINC_ICMP_ECHO_RSP_EVENT_HANDLER pfICMPEchoResponseCB;

    /* Callback to use for DNS resolve responses. */
    WDRV_WINC_DNS_RESOLVE_CALLBACK pfDNSResolveResponseCB;
#endif
    /* Callback to use for network interface events. */
    WDRV_WINC_NETIF_EVENT_HANDLER pfNetIfEventCB;

    /* Callback to use for network interface information requests. */
    WDRV_WINC_NETIF_INFO_HANDLER pfNetIfInfoCB;

    /* Callback to use for regulatory domain information events. */
    WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB;

    /* Callback to use for receiving powersave information events. */
    WDRV_WINC_POWERSAVE_CALLBACK pfPowersaveEventCB;
#ifndef WDRV_WINC_DISABLE_L3_SUPPORT
    /* Callback to use for receiving DHCP server events. */
    WDRV_WINC_DHCPS_EVENT_HANDLER pfDHCPSEventCB;
#endif
    /* Callback to monitor L2 data frames. */
    WDRV_WINC_L2DATA_MONITOR_CALLBACK pfL2DataMonitorCB;
#ifndef WDRV_WINC_MOD_DISABLE_PROV
    /* Callback for provisioning service attach events */
    WDRV_WINC_PROV_ATTACH_CALLBACK pfProvAttachCB;
#endif
    WDRV_WINC_NETIF_IDX netIfSTA;

    WDRV_WINC_NETIF_IDX netIfAP;
} WDRV_WINC_CTRLDCPT;

// *****************************************************************************
/*  WINC Driver Descriptor

  Summary:
    The defines the WINC driver descriptor.

  Description:
    This data type defines the system level descriptor for the WINC driver.
    This structure is initialized by a call to WDRV_WINC_Initialize.

  Remarks:
    None.
*/

typedef struct
{
    /* Flag indicating if the driver has been initialized. */
    bool isInit;

    /* Driver status which can be queried via WDRV_WINC_Status. */
    SYS_STATUS sysStat;

    /* Flag indicating if this instance of the driver has been opened by
        a call to WDRV_WINC_Open. */
    bool isOpen;

    WDRV_WINC_SYSTEM_EVENT_CALLBACK pfEventCallback;

    /* Pointer to instance specific descriptor (Control). */
    WDRV_WINC_CTRLDCPT  *pCtrl;
} WDRV_WINC_DCPT;

// *****************************************************************************
// *****************************************************************************
// Section: WINC Debugging Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_DebugRegisterCallback
    (
        WDRV_WINC_DEBUG_PRINT_CALLBACK const pfDebugPrintCallback
    )

  Summary:
    Register callback for debug serial stream.

  Description:
    The debug serial stream provides a printf-like stream of messages from within
    the WINC driver. The caller can provide a function to be called when
    output is available.

  Precondition:
    None.

  Parameters:
    pfDebugPrintCallback - Function pointer to printf-like function.

  Returns:
    None.

  Remarks:
    None.

 */

#ifndef WDRV_WINC_DEVICE_USE_SYS_DEBUG
void WDRV_WINC_DebugRegisterCallback(WDRV_WINC_DEBUG_PRINT_CALLBACK const pfDebugPrintCallback);
#else
#define WDRV_WINC_DebugRegisterCallback(...)
#endif

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Client Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    DRV_HANDLE WDRV_WINC_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent)

  Summary:
    Opens an instance of the WINC driver.

  Description:
    Opens an instance of the WINC driver and returns a handle which must be
    used when calling other driver functions.

  Precondition:
    WDRV_WINC_Initialize must have been called.

  Parameters:
    index  - Identifier for the driver instance to be opened.
    intent - Zero or more of the values from the enumeration
                DRV_IO_INTENT ORed together to indicate the intended use
                of the driver

  Returns:
    - Valid handle - if the open function succeeded
    - DRV_HANDLE_INVALID - if an error occurs

  Remarks:
    The index and intent parameters are not used in the current implementation
    and are maintained only for compatibility with the generic driver Open
    function signature.

*/

DRV_HANDLE WDRV_WINC_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT intent);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_Close(DRV_HANDLE handle)

  Summary:
    Closes an instance of the WINC driver.

  Description:
    This is the function that closes an instance of the MAC.
    All per client data is released and the handle can no longer be used
    after this function is called.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.

  Returns:
    None.

  Remarks:
    None.

*/

void WDRV_WINC_Close(DRV_HANDLE handle);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_SYS_STATUS WDRV_WINC_StatusExt(SYS_MODULE_OBJ object)

  Summary:
    Provides the extended system status of the WINC driver module.

  Description:
    This function provides the extended system status of the WINC driver
    module.

  Precondition:
    WDRV_WINC_Initialize must have been called before calling this function.

  Parameters:
    object - Driver object handle, returned from WDRV_WINC_Initialize.

  Returns:
    SYS_STATUS_READY - Indicates that any previous module operation for the
                        specified module has completed.
    SYS_STATUS_BUSY - Indicates that a previous module operation for the
                        specified module has not yet completed.
    SYS_STATUS_ERROR - Indicates that the specified module is in an error state.
    SYS_STATUS_UNINITIALIZED - Driver uninitialized.
    WDRV_WINC_SYS_STATUS_ERROR_DEVICE_NOT_FOUND - Device not found.
    WDRV_WINC_SYS_STATUS_ERROR_DEVICE_FAILURE - Device failure.

  Remarks:
    None.

*/

WDRV_WINC_SYS_STATUS WDRV_WINC_StatusExt(SYS_MODULE_OBJ object);

// *****************************************************************************
// *****************************************************************************
// Section: WINC L2 Access Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_L2DataMonitorCallbackSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_L2DATA_MONITOR_CALLBACK pfL2DataMonitorCB
    )

  Summary:
    Set the L2 data frame monitor callback.

  Description:
    Sets the callback used to report L2 data frames.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle            - Client handle obtained by a call to WDRV_WINC_Open.
    pfL2DataMonitorCB - Pointer to callback to set.

  Returns:
    WDRV_WINC_STATUS_OK             - The information has been returned.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_L2DataMonitorCallbackSet
(
    DRV_HANDLE handle,
    WDRV_WINC_L2DATA_MONITOR_CALLBACK pfL2DataMonitorCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_L2DataFrameSend
    (
        DRV_HANDLE handle,
        WDRV_WINC_NETIF_IDX ifIdx,
        const uint8_t *const pl2Data,
        size_t l2DataLen
    )

  Summary:
    Sends an L2 frame to the WiFi layer.

  Description:
    Queues an L2 frame to the WiFi subsystem for transmission.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    ifIdx     - Network interface index.
    pl2Data   - Pointer to L2 data frame.
    l2DataLen - Length of L2 data frame payload.

  Returns:
    WDRV_WINC_STATUS_OK             - The information has been returned.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_L2DataFrameSend
(
    DRV_HANDLE handle,
    WDRV_WINC_NETIF_IDX ifIdx,
    const uint8_t *const pl2Data,
    size_t l2DataLen
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Information Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_InfoDriverVersionGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DRIVER_VERSION_INFO *const pDriverVersion
    )

  Summary:
    Returns the drivers version information.

  Description:
    Returns information on the drivers version.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    pDriverVersion - Pointer to structure to receive version information.

  Returns:
    WDRV_WINC_STATUS_OK             - The information has been returned.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR  - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_InfoDriverVersionGet
(
    DRV_HANDLE handle,
    WDRV_WINC_DRIVER_VERSION_INFO *const pDriverVersion
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_InfoDeviceFirmwareVersionGet
    (
        DRV_HANDLE handle,
        bool active,
        WDRV_WINC_FIRMWARE_VERSION_INFO *const pFirmwareVersion
    )

  Summary:
    Returns the WINC firmware version information.

  Description:
    Returns information on the WINC firmware version and build date/time.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle           - Client handle obtained by a call to WDRV_WINC_Open.
    active           - Flag indicating if the active of inactive image is queried.
    pFirmwareVersion - Pointer to structure to receive version information.

  Returns:
    WDRV_WINC_STATUS_OK             - The information has been returned.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_RETRY_REQUEST  - Version information not available, try again.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_InfoDeviceFirmwareVersionGet
(
    DRV_HANDLE handle,
    bool active,
    WDRV_WINC_FIRMWARE_VERSION_INFO *const pFirmwareVersion
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_InfoDeviceGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DEVICE_INFO *const pDeviceInfo
    )

  Summary:
    Returns the WINC device information.

  Description:
    Returns information on the WINC device.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    pDeviceInfo - Pointer to structure to receive device information.

  Returns:
    WDRV_WINC_STATUS_OK             - The information has been returned.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_RETRY_REQUEST  - Version information not available, try again.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_InfoDeviceGet
(
    DRV_HANDLE handle,
    WDRV_WINC_DEVICE_INFO *const pDeviceInfo
);


// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Configuration Archive Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_CfgArchiveStore
    (
        DRV_HANDLE handle,
        const char *pFilename
    )

  Summary:
    Archives a configuration set.

  Description:
    Archives the currently active configuration set to a flash file.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    pFilename - Pointer to a filename to store the configuration in.

  Returns:
    WDRV_WINC_STATUS_OK             - The information has been returned.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_RETRY_REQUEST  - Version information not available, try again.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_CfgArchiveStore
(
    DRV_HANDLE handle,
    const char *pFilename
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_CfgArchiveRecall
    (
        DRV_HANDLE handle,
        const char *pFilename
    )

  Summary:
    Recall a configuration set.

  Description:
    Recall a flash file to the currently active configuration.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle    - Client handle obtained by a call to WDRV_WINC_Open.
    pFilename - Pointer to a filename to recall the configuration from.

  Returns:
    WDRV_WINC_STATUS_OK             - The information has been returned.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_RETRY_REQUEST  - Version information not available, try again.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_CfgArchiveRecall
(
    DRV_HANDLE handle,
    const char *pFilename
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver General Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_DebugUARTSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_DEBUG_UART uartPort,
        uint32_t uartBaudRate
    )

  Summary:
    Configures the debug UART on the WINC device.

  Description:
    Sets the debug UART and baud rate on the WINC device.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle       - Client handle obtained by a call to WDRV_WINC_Open.
    uartPort     - UART port selection.
    uartBaudRate - UART baud rate.

  Returns:
    WDRV_WINC_STATUS_OK             - The information has been returned.
    WDRV_WINC_STATUS_NOT_OPEN       - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG    - The parameters were incorrect.
    WDRV_WINC_STATUS_RETRY_REQUEST  - Version information not available, try again.

  Remarks:
    Setting uartPort to WDRV_WINC_DEBUG_UART_OFF will turn off the debug UART.

    Setting uartBaudRate to zero will result in the previous baud rate being
    used.

*/

WDRV_WINC_STATUS WDRV_WINC_DebugUARTSet
(
    DRV_HANDLE handle,
    WDRV_WINC_DEBUG_UART uartPort,
    uint32_t uartBaudRate
);

//*******************************************************************************
/*
  Function:
    WINC_CMD_REQ_HANDLE WDRV_WINC_CmdReqInit
    (
        unsigned int numCommands,
        size_t extraDataLen,
        WINC_DEV_CMD_RSP_CB pfCmdRspCallback,
        uintptr_t cmdRspCallbackCtx
    )

  Summary:
    Initialise a command request.

  Description:
    Allocates memory and initialises a command request.

  Precondition:
    None.

  Parameters:
    numCommands       - Number of commands in request burst.
    extraDataLen      - Additional space required for data.
    pfCmdRspCallback  - Callback function to receive request notifications.
    cmdRspCallbackCtx - Context to provide to callback function.

  Returns:
    Command request handle or WINC_CMD_REQ_INVALID_HANDLE for error.

  Remarks:
    None.

*/

WINC_CMD_REQ_HANDLE WDRV_WINC_CmdReqInit
(
    unsigned int numCommands,
    size_t extraDataLen,
    WINC_DEV_CMD_RSP_CB pfCmdRspCallback,
    uintptr_t cmdRspCallbackCtx
);

//*******************************************************************************
/*
  Function:
    bool WDRV_WINC_DevTransmitCmdReq
    (
        WINC_DEVICE_HANDLE devHandle,
        WINC_CMD_REQ_HANDLE cmdReqHandle
    )

  Summary:
    Transmits a command request.

  Description:
    Queues a command request for transmission to the WINCS02 device.

  Precondition:
    WDRV_WINC_CmdReqInit must have been called to create a command request.

  Parameters:
    devHandle    - WINCS02 device handle.
    cmdReqHandle - Command request handle.

  Returns:
    true or false indicating success of transmission operation.

  Remarks:
    None.

*/

bool WDRV_WINC_DevTransmitCmdReq
(
    WINC_DEVICE_HANDLE devHandle,
    WINC_CMD_REQ_HANDLE cmdReqHandle
);

//*******************************************************************************
/*
  Function:
    void WDRV_WINC_DevDiscardCmdReq(WINC_CMD_REQ_HANDLE cmdReqHandle)

  Summary:
    Discards a command request.

  Description:
    Deallocates and discards a previously created command request.

  Precondition:
    WDRV_WINC_CmdReqInit must have been called to create a command request.

  Parameters:
    cmdReqHandle - Command request handle.

  Returns:
    None.

  Remarks:
    None.

*/

void WDRV_WINC_DevDiscardCmdReq(WINC_CMD_REQ_HANDLE cmdReqHandle);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_H */
