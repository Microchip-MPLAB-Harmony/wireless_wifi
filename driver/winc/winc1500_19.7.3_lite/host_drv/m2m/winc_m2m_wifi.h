/**
 *
 * \file
 *
 * \brief WINC WLAN Application Interface.
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

/** @defgroup m2m_wifi WLAN
    @{
        @defgroup   WLANCallbacks   Callbacks
        @brief
        Provides detail on the available callbacks for the WLAN APIs.

        @defgroup   WLANDefines     Defines
        @brief
        Specifies the macros and defines used by the WLAN APIs.

        @defgroup   WLANEnums       Enumerations and Typedefs
        @brief
        Specifies the enums and Data Structures used by the WLAN APIs.

        @defgroup   WLANAPI         WLAN Functions
        @brief
        Here are listed all the functions that implement the WLAN APIs.
        @{
            @defgroup   WLANINIT        Initialization
            @brief
            Here are listed all the functions that implement the WLAN Initialization APIs.
            @defgroup   WLANEVTS        WLAN Events
            @brief
            Here are listed all the functions that implement the WLAN Events APIs.
            @defgroup   WLANCONNECT     Connection
            @brief
            Here are listed all the functions that implement the WiFi Connection APIs.
            @defgroup   WLANSCAN        Scanning
            @brief
            Here are listed all the functions that implement the WiFi Scanning APIs.
            @defgroup   WLANAP          Hot-Spot (Access-Point)
            @brief
            Here are listed all the functions that implement the WiFi Hot-Spot (Access-Point) APIs.
            @defgroup   WLANETH         Bypass Mode
            @brief
            Here are listed all the functions that implement the Bypass Mode APIs.
            @defgroup   WLANROAMING     Roaming
            @brief
            Here are listed all the functions that implement the WiFi Roaming APIs.
            @defgroup   WLANPS          Power Save
            @brief
            Here are listed all the functions that implement the Power-Save APIs.
            @defgroup   WLANCONF        Configuration
            @brief
            Here are listed all the functions that implement the WLAN Configuration APIs.
            @defgroup   WLANTIME        System Time
            @brief
            Here are listed all the functions that implement the System Time APIs.
            @defgroup   WLANPROVISION   Provisioning
            @brief
            Here are listed all the functions that implement the WiFi Provisioning APIs.
            @defgroup   WLANCRYPTO      Crypto
            @brief
            Here are listed all the functions that implement the WiFi Crypto APIs.
            @defgroup   WLANWPS         WPS
            @brief
            Here are listed all the functions that implement the WiFi WPS APIs.
        @}
    @}

    @defgroup VERSION Version
    @{
        @brief
        Describes the APIs for reading the version information of the WINC firmware.
        @defgroup   VERSIONDEF  Defines
        @brief
        Specifies the macros and defines used by the version APIs.

        @defgroup   VERSIONAPI  Functions
        @brief
        Lists the APIs for reading the version information of the WINC firmware.
    @}
*/

#ifndef __M2M_WIFI_H__
#define __M2M_WIFI_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
Callbacks
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/**@addtogroup WLANCallbacks
 * @{
 */
/*!
@typedef    void (*tpfAppWifiCb)(uint8_t u8MsgType, const void *const pvMsg);

@brief
            This is the main callback function for the WiFi driver and is responsible for processing
            any M2M_WIFI events that are received on the WiFi interface.
            These events (notifications) are usually received in response to earlier WiFi requests such
            as @ref m2m_wifi_request_scan, @ref m2m_wifi_connect_open, @ref m2m_wifi_connect_wep,
            @ref m2m_wifi_connect_psk, @ref m2m_wifi_connect_1x_mschap2, @ref m2m_wifi_connect_1x_tls,
            @ref m2m_wifi_get_connection_info, @ref m2m_wifi_req_curr_rssi, @ref m2m_wifi_get_system_time, etc.

            Most WiFi APIs are implemented in an asynchronous mode and calling them generates information
            that is then passed back to the application via this callback. For instance, a set of detected
            networks to be passed back as a result to a call to @ref m2m_wifi_request_scan.

            Applications must ensure a callback function is registered with the WiFi driver by
            calling @ref m2m_wifi_init.
@param[in]  u8MsgType
            Type of notification. Possible types are:
            - @ref M2M_WIFI_RESP_CON_STATE_CHANGED
            - @ref M2M_WIFI_RESP_CONN_INFO
            - @ref M2M_WIFI_REQ_DHCP_CONF
            - @ref M2M_WIFI_REQ_WPS
            - @ref M2M_WIFI_RESP_IP_CONFLICT
            - @ref M2M_WIFI_RESP_SCAN_DONE
            - @ref M2M_WIFI_RESP_SCAN_RESULT
            - @ref M2M_WIFI_RESP_CURRENT_RSSI
            - @ref M2M_WIFI_RESP_CLIENT_INFO
            - @ref M2M_WIFI_RESP_PROVISION_INFO
            - @ref M2M_WIFI_RESP_DEFAULT_CONNECT
            - @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET (If Bypass mode is active)
            - @ref M2M_WIFI_RESP_WIFI_RX_PACKET (If monitoring mode is active)

@param[in]  pvMsg
            A pointer to a buffer containing the notification parameters (if any).
            It should be cast to the correct data type corresponding to the notification type.

@see
            tstrM2mWifiStateChanged
            tstrM2MWPSInfo
            tstrM2mScanDone
            tstrM2mWifiscanResult
            m2m_wifi_init
*/
typedef void (*tpfAppWifiCb)(uint8_t u8MsgType, const void *const pvMsg);

/*!
@typedef    void (*tpfAppEthCb) (uint8_t u8MsgType, const void *const pvMsg, const void *const pvCtrlBuf);

@brief
            Ethernet (Bypass mode) notification callback function receiving Bypass mode events as
            defined in the WiFi responses enumeration @ref tenuM2mStaCmd.

            If bypass mode is enabled, applications must ensure this callback function is registered
            with the WiFi driver by calling @ref m2m_wifi_init.

@param[in]  u8MsgType
            Type of notification. Possible types are:
            - @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET

@param[in]  pvMsg
            A pointer to a buffer containing the notification parameters (if any).
            It should be cast to the correct data type corresponding to the notification type.

            For example, it could be a pointer to the buffer holding the received frame in case
            a @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET event is received.

@param[in]  pvControlBuf
            A pointer to control buffer describing the accompanied message.
            This must be cast to the data type @ref tstrM2mIpCtrlBuf in case of @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET event.

@warning
            Make sure that the application defines ETH_MODE.

@see
            m2m_wifi_init
*/
typedef void (*tpfAppEthCb) (uint8_t u8MsgType, const void *const pvMsg, const void *const pvCtrlBuf);

/**@}*/     // WLANCallbacks
/**@addtogroup  WLANEnums
 * @{
 */
/*!
@enum       tenuWifiState
@brief
            Enumeration for WiFi state
            The following is used to track the state of the WiFi (not initialized, initialized or started)

@remarks
            This is useful when putting the WINC in "download mode" to access the flash via SPI. By using
            @ref m2m_wifi_get_state and checking against the desired state, it is possible to validate if
            the application should proceed with the SPI Flash access or not.
*/
typedef enum
{
    WIFI_STATE_DEINIT,
    /*!< WiFi is not initialized */
    WIFI_STATE_INIT,
    /*!< WiFi has been initialized */
    WIFI_STATE_START,
    /*!< WiFi has started */
} tenuWifiState;

/*!
@enum       tenuCredStoreOption
@brief

@remarks
*/
typedef enum
{
    WIFI_CRED_DONTSAVE,
    /*!< Credentials will not be stored in WINC flash. */
    WIFI_CRED_SAVE_UNENCRYPTED,
    /*!< Credentials will be stored unencrypted in WINC flash. */
    WIFI_CRED_SAVE_ENCRYPTED
    /*!< Credentials will be stored encrypted in WINC flash.
            The encryption is not secure; it is merely intended to prevent sensitive information
            being leaked by an opportunistic read of WINC flash contents.
            The encryption keys involve WINC efuse contents, so WINC efuses should not be written
            while this option is in use. */
} tenuCredStoreOption;

/*!
@enum       tenu1xOption
@brief

@remarks
*/
typedef enum
{
    WIFI_1X_BYPASS_SERVER_AUTH,
    /*!< Server authentication for 802.1x connections.

        Values are of type int
        - 0: Authenticate server; Default, Recommended.
        - 1: Bypass server authentication.
    */
    WIFI_1X_TIME_VERIF_MODE,
    /*!< Mode for checking expiry of server certificate chain.

        Values are of type @ref tenuSslCertExpSettings.

        Default is @ref SSL_CERT_EXP_CHECK_EN_IF_SYS_TIME.
    */
    WIFI_1X_SESSION_CACHING,
    /*!< TLS session caching on/off for 802.1x connections.

        Values are of type int
        - 0: Session caching off.
        - 1: Session caching on; Default.
        @note The WINC implementation of PEAPv0 does not support session caching; this setting is ignored for PEAPv0 methods.
    */
    WIFI_1X_SPECIFIC_ROOTCERT,
    /*!< SHA1 digest of subject name of the root certificate to be used during server authentication.
        Values are:
        - 20-byte arrays: authentication is limited to this particular root certificate (\b Recommended)
        - 0-byte array: authentication can succeed with any certificate in the WINC root certificate store (Default)
    */
} tenu1xOption;

/*!
@struct     tstrEthInitParam

@brief
            Structure to hold Ethernet interface parameters.
            Structure is to be defined and have its attributes set, based on the application's functionality
            before a call is made to initialize the WiFi operations by calling the
            @ref m2m_wifi_init function.
            Part of the WiFi configuration structure @ref tstrWifiInitParam.
            Applications shouldn't need to define this structure, if the bypass mode is not defined.

@see
            tpfAppEthCb
            tpfAppWifiCb
            m2m_wifi_init

@warning
            Make sure that application defines ETH_MODE before using @ref tstrEthInitParam.
*/
typedef struct
{
    tpfAppWifiCb    pfAppWifiCb;        /*!< Callback for WiFi notifications. */
    tpfAppEthCb     pfAppEthCb;         /*!< Callback for Ethernet interface. */
    uint8_t         *au8ethRcvBuf;      /*!< Pointer to Receive Buffer of Ethernet Packet */
    uint16_t        u16ethRcvBufSize;   /*!< Size of Receive Buffer for Ethernet Packet */
    uint8_t         u8EthernetEnable;   /*!< Enable Ethernet mode flag */
} tstrEthInitParam;

/*!
@struct     tstrM2mIpCtrlBuf

@brief
            Structure holding the incoming buffer's data size information, indicating the data size of the
            buffer and the remaining buffer's data size. The data of the buffer which holds the packet sent
            to the host when in the bypass mode, is placed in the @ref tstrEthInitParam::au8ethRcvBuf attribute.
            This following information is retrieved in the host when an event
            @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET is received in the WiFi callback function
            @ref tpfAppWifiCb.

            The application is expected to use this structure's information to determine if there is still incoming data to be received from the firmware.

@see
            tpfAppWifiCb
            tpfAppEthCb
            tstrEthInitParam

@warning
            Make sure that Ethernet/bypass mode is defined before using @ref tstrM2mIpCtrlBuf

*/
typedef struct
{
    uint16_t u16DataSize;          /*!< Size of the received data in bytes. */
    uint16_t u16RemainingDataSize; /*!< Size of the remaining data bytes to be delivered to host. */
} tstrM2mIpCtrlBuf;

/*!
@struct     tstrWifiInitParam

@brief
            Structure, holding the WiFi configuration attributes such as the WiFi callback, monitoring mode callback and Ethernet parameter initialization structure.
            Such configuration parameters are required to be set before calling the WiFi initialization function @ref m2m_wifi_init.
            @ref pfAppWifiCb attribute must be set to handle the WiFi callback operations.
            @ref strEthInitParam structure, is another optional configuration based on whether the bypass mode is set.

@see
            tpfAppEthCb
            tstrEthInitParam
*/
typedef struct
{
    tpfAppWifiCb        pfAppWifiCb;        /*!< Callback for WiFi notifications. */
    tstrEthInitParam    strEthInitParam;    /*!< Structure to hold Ethernet interface parameters. */
} tstrWifiInitParam;

typedef struct
{
    uint8_t         *pu8Bssid;
    /*!< Pointer to BSSID (6 bytes). Optional (may be NULL).
            If present, this restricts the connection attempt to APs that have a matching BSSID. */
    uint8_t         *pu8Ssid;
    /*!< Pointer to SSID. Required. */
    uint8_t         u8SsidLen;
    /*!< Length of SSID in bytes. Permitted values are between 0 and 32. */
    tenuM2mScanCh   enuChannel;
    /*!< WiFi channel to connect on.
            If an appropriate AP cannot be found on this channel then connection fails.
            @ref M2M_WIFI_CH_ALL may be used to allow scanning of all channels. */
} tstrNetworkId;

/* Legacy WEP param structure. */
typedef struct
{
    uint8_t u8KeyIndx;
    uint8_t u8KeySz;
    uint8_t au8WepKey[WEP_104_KEY_STRING_SIZE + 1]; // NULL terminated
    uint8_t __PAD24__[3];
} tstrM2mWifiWepParams;

/* Legacy 802.1x MsChapv2 param structure. */
typedef struct
{
    uint8_t au8UserName[21];    // NULL terminated
    uint8_t au8Passwd[41];      // NULL terminated
} tstr1xAuthCredentials;

typedef struct
{
    uint8_t *pu8Psk;
    /*!< Pointer to PSK, represented as an ASCII string (64 characters, representing 32 bytes).
            Must be NULL if Passphrase is provided instead. */
    uint8_t *pu8Passphrase;
    /*!< Pointer to Passphrase (Printable ASCII).
            Must be NULL if PSK is provided instead. */
    uint8_t u8PassphraseLen;
    /*!< Length of Passphrase. Permitted values are between 8 and 63.
            This field is ignored if pu8Passphrase == NULL. */
} tstrAuthPsk;

typedef struct
{
    uint8_t *pu8WepKey;
    /*!< Pointer to WEP Key, represented as an ASCII string.
            (10 or 26 characters, representing 5 or 13 bytes.) */
    uint8_t u8KeySz;
    /*!< Size of WEP Key string.
            Permitted values are @ref WEP_40_KEY_STRING_SIZE or @ref WEP_104_KEY_STRING_SIZE. */
    uint8_t u8KeyIndx;
    /*!< WEP Key Index in the range 1 to 4. */
} tstrAuthWep;

typedef struct
{
    uint8_t *pu8Domain;
    /*!< Pointer to Domain of authentication server (printable ASCII), including '@' or '\'
            separator character as appropriate. Use NULL if there is no domain information.
            The Domain will be either prepended or appended to the UserName, depending on the
            setting of field bPrependDomain. \n
            Example 1: if [Domain] is "@my_domain" and bPrependDomain is false, then the EAP
            identity response is "[UserName]@my_domain". \n
            Example 2: if [Domain] is "my_domain\" and bPrependDomain is true, then the EAP
            identity response is "my_domain\[UserName]". */
    uint8_t *pu8UserName;
    /*!< Pointer to UserName (ASCII).
            This will be sent (encrypted) in the tunneled EAP identity response (if applicable)
            and used during MSCHAPv2 authentication. If bUnencryptedUserName is true then it will
            also be sent (unencrypted) in the initial EAP identity response. */
    uint8_t *pu8Password;
    /*!< Pointer to MSCHAPv2 Password (ASCII).
            This will be used during MSCHAPv2 authentication. */
    uint16_t u16DomainLen;
    /*!< Length of Domain (in ASCII characters), including '@' or '\' separator character as
            appropriate.
            Permitted values are such that u16DomainLen + u16UserNameLen is between 0 and
            @ref M2M_AUTH_1X_USER_LEN_MAX. */
    uint16_t u16UserNameLen;
    /*!< Length of UserName (in ASCII characters).
            Permitted values are such that u16DomainLen + u16UserNameLen is between 0 and
            @ref M2M_AUTH_1X_USER_LEN_MAX. */
    uint16_t u16PasswordLen;
    /*!< Length of Password (in ASCII characters).
            Permitted values are between 0 and @ref M2M_AUTH_1X_PASSWORD_LEN_MAX. */
    bool    bUnencryptedUserName;
    /*!< Determines whether UserName or "anonymous" is sent (unencrypted) in the initial EAP
            identity response. Domain is sent in both cases. \n
            true: UserName is sent in the initial EAP identity response (not recommended).
            false: "anonymous" is sent in the initial EAP identity response. This setting is
            recommended for tunneled methods. MSCHAPv2 is always a tunneled method. */
    bool    bPrependDomain;
    /*!< Determines whether Domain is prepended or appended to UserName in EAP identity responses.
            true: Domain is prepended to UserName - [Domain][UserName].
            false: Domain is appended to UserName - [UserName][Domain]. */
} tstrAuth1xMschap2;

typedef struct
{
    uint8_t *pu8Domain;
    /*!< Pointer to Domain of authentication server (printable ASCII), including '@' or '\'
            separator character as appropriate. Use NULL if there is no domain information.
            The Domain will be either prepended or appended to the UserName, depending on the
            setting of field bPrependDomain. \n
            Example 1: if [Domain] is "@my_domain" and bPrependDomain is false, then the EAP
            identity response is "[UserName]@my_domain". \n
            Example 2: if [Domain] is "my_domain\" and bPrependDomain is true, then the EAP
            identity response is "my_domain\[UserName]". */
    uint8_t *pu8UserName;
    /*!< Pointer to UserName (ASCII).
            This will be sent (encrypted) in the tunneled EAP identity response.
            If bUnencryptedUserName is true then it will also be sent (unencrypted) in the initial
            EAP identity response. */
    uint8_t *pu8PrivateKey_Mod;
    /*!< Pointer to PrivateKey modulus (raw data).
            This will be used during TLS client authentication. */
    uint8_t *pu8PrivateKey_Exp;
    /*!< Pointer to PrivateKey exponent (raw data).
            This will be used during TLS client authentication. */
    uint8_t *pu8Certificate;
    /*!< Pointer to TLS client certificate corresponding to PrivateKey.
            This will be used during TLS client authentication. */
    uint16_t u16DomainLen;
    /*!< Length of Domain (in ASCII characters), including '@' or '\' separator character as
            appropriate.
            Permitted values are such that u16DomainLen + u16UserNameLen is between 0 and
            @ref M2M_AUTH_1X_USER_LEN_MAX. */
    uint16_t u16UserNameLen;
    /*!< Length of UserName (in ASCII characters).
            Permitted values are such that u16DomainLen + u16UserNameLen is between 0 and
            @ref M2M_AUTH_1X_USER_LEN_MAX. */
    uint16_t u16PrivateKeyLen;
    /*!< Length of PrivateKey_Mod (in bytes).
            Permitted values are between 0 and @ref M2M_AUTH_1X_PRIVATEKEY_LEN_MAX, typically 128 or 256.
            PrivateKey_Exp must be the same length as modulus, pre-padded with 0s if necessary. */
    uint16_t u16CertificateLen;
    /*!< Length of Certificate (in bytes).
            Permitted values are between 0 and @ref M2M_AUTH_1X_CERT_LEN_MAX. */
    bool    bUnencryptedUserName;
    /*!< Determines whether UserName or "anonymous" is sent (unencrypted) in the initial EAP
            identity response. Domain is sent in both cases. \n
            true: UserName is sent in the initial EAP identity response (required for EAP-TLS).
            false: "anonymous" is sent in the initial EAP identity response. This setting is
            recommended for tunneled methods such as EAP-PEAP/TLS. */
    bool    bPrependDomain;
    /*!< Determines whether Domain is prepended or appended to UserName in EAP identity responses.
            true: Domain is prepended to UserName - [Domain][UserName].
            false: Domain is appended to UserName - [UserName][Domain]. */
} tstrAuth1xTls;

/**@}*/     // WLANEnums

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
extern "C" {
#endif

/*!
@ingroup    WLANINIT
@fn         int8_t m2m_wifi_download_mode(void);

@brief
            Prepares the WINC before downloading any data (firmware, certificates, etc).

@details
            This function should be called before attempting to download any data to the WINC.
            Performs the appropriate WINC driver initialization, this includes bus initialization,
            interrupt enabling and it halts the chip to allow for the firmware downloads. Firmware
            can be downloaded through a number of interfaces, UART, I2C and SPI.

@pre
            Prior to call m2m_wifi_download_mode, the Application should ensure that the WiFi is not
            initialized. This can be done by calling @ref m2m_wifi_get_state and in case the WiFi state
            differs from @ref WIFI_STATE_DEINIT, a @ref m2m_wifi_deinit needs to be issued.

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_wifi_download_mode(void);

/*!
@ingroup    WLANINIT
@fn         int8_t m2m_wifi_init(tstrWifiInitParam *pWifiInitParam);

@brief
            Synchronous API to initialize the WINC driver.

@details
            This function initializes the WINC driver by registering the callback function for the M2M_WIFI layer
            (also the callback function for bypass mode/monitoring mode if defined), initializing the host
            interface layer and the bus interfaces. WiFi callback registering is essential to allow the
            handling of the events received, in response to the asynchronous WiFi operations.

            The possible WiFi events that are expected to be received through the callback
            function (provided by the application) to the M2M_WIFI layer are listed below:

            - @ref M2M_WIFI_RESP_CON_STATE_CHANGED
            - @ref M2M_WIFI_RESP_CONN_INFO
            - @ref M2M_WIFI_REQ_DHCP_CONF
            - @ref M2M_WIFI_REQ_WPS
            - @ref M2M_WIFI_RESP_IP_CONFLICT
            - @ref M2M_WIFI_RESP_SCAN_DONE
            - @ref M2M_WIFI_RESP_SCAN_RESULT
            - @ref M2M_WIFI_RESP_CURRENT_RSSI
            - @ref M2M_WIFI_RESP_CLIENT_INFO
            - @ref M2M_WIFI_RESP_PROVISION_INFO
            - @ref M2M_WIFI_RESP_DEFAULT_CONNECT
            - @ref M2M_WIFI_RESP_ETHERNET_RX_PACKET (if bypass mode is enabled)
            - @ref M2M_WIFI_RESP_WIFI_RX_PACKET (if monitoring mode is enabled)

            Any application using the WINC driver must call this function at the start of its main function.

@param[in]  pWifiInitParam
            This is a pointer to a structure of type @ref tstrWifiInitParam which contains pointers to the
            application WIFI layer callback function, monitoring mode callback and @ref tstrEthInitParam
            structure (which contains initialization settings for bypass mode).

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

@pre
            Prior to this function call, the application should initialize the BSP using @ref winc_bsp_init.
            Also, the application must provide a callback function responsible for receiving all the
            WiFi events that are received on the M2M_WIFI layer.

@warning
            Failure to successfully complete indicates that the driver could not be initialized and
            a fatal error will prevent the application from proceeding, proper error handling should be
            implemented by the application.

@see
            m2m_wifi_deinit
            m2m_wifi_init_hold
            m2m_wifi_init_start
            m2m_wifi_download_mode
            tstrWifiInitParam
            tenuM2mStaCmd
*/
int8_t m2m_wifi_init(tstrWifiInitParam *pWifiInitParam);

/*!
@ingroup    WLANINIT
@fn         int8_t m2m_wifi_deinit(void *pVArg);

@brief
            Synchronous API to de-initialize the WINC driver and host interface.

@details
            De-initialization function for the WINC driver.
            De-initializes the host interface and frees any resources used by the M2M_WIFI layer.
            This function must be called in the application closing phase to ensure that all
            resources have been correctly released.

@param[in]  pVArg
            Opaque argument, not used in current implementation. Application should use null.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC,
            and a negative value otherwise.

@note
            This function must be called at the de-initialization stage of the application.
            Generally this function should be the last function before switching off the chip
            and it should be followed only by @ref winc_bsp_deinit function call.
            Every function call of @ref m2m_wifi_init should be matched with a call to m2m_wifi_deinit.

@see
            winc_bsp_deinit
            m2m_wifi_init
            m2m_wifi_init_hold
            m2m_wifi_init_start
            m2m_wifi_download_mode
            m2m_wifi_download_mode
*/
int8_t m2m_wifi_deinit(void *pVArg);

/*!
@ingroup    WLANINIT
@fn         int8_t m2m_wifi_init_hold(void);

@brief
            First part of @ref m2m_wifi_init, up to the point of initializing SPI for flash access.

@see
            m2m_wifi_init
            m2m_wifi_deinit
            m2m_wifi_init_start
            m2m_wifi_download_mode
*/
int8_t m2m_wifi_init_hold(void);

/*!
@ingroup    WLANINIT
@fn         int8_t m2m_wifi_init_start(tstrWifiInitParam *pWifiInitParam);

@brief
            Second part of @ref m2m_wifi_init, continuing from where @ref m2m_wifi_init_hold left off.

@param[in]  pWifiInitParam
            This is a pointer to a variable of type @ref tstrWifiInitParam which contains pointers to the
            application WIFI layer callback function (see @ref tpfAppWifiCb), monitoring mode callback
            (see @ref tpfAppEthCb) and @ref tstrEthInitParam structure (which contains initialization
            settings for bypass mode).

@see
            m2m_wifi_init
            m2m_wifi_deinit
            m2m_wifi_init_hold
            m2m_wifi_download_mode
            tstrWifiInitParam
*/
int8_t m2m_wifi_init_start(tstrWifiInitParam *pWifiInitParam);

/*!
@ingroup    WLANEVTS
@fn         int8_t m2m_wifi_handle_events(void *pVArg);

@brief
            Synchronous M2M event handler function.

@details
            This function is responsible for handling interrupts received from the WINC firmware.
            Applications should call this function periodically in-order to receive the events that are to
            be handled by the callback functions implemented by the application.

            Handle the various events received from the WINC.
            Whenever an event happens in the WINC (e.g. Connection, Disconnection, DHCP, etc),
            the WINC will interrupt the host to let it know that a new event has occurred. The host driver
            will attempt to handle these events whenever the application decides to do so by calling
            the m2m_wifi_handle_events function.
            It is mandatory to call this function periodically and independently of any other condition.
            It is ideal to include this function in the main and the most frequent loop of the
            host application.

@param[in]  pVArg
            Opaque argument, not used in current implementation. Application should use NULL.

@pre
            Prior to receiving events, the WINC driver should have been successfully initialized by calling the @ref m2m_wifi_init function.

@warning
            Failure to successfully complete this function indicates bus errors and hence a fatal error that will prevent the application from proceeding.

@return
            The function returns @ref M2M_SUCCESS for successful interrupt handling and a negative value otherwise.
*/
int8_t m2m_wifi_handle_events(void *pVArg);

/*!
@ingroup    WLANCRYPTO
@fn         int8_t m2m_wifi_send_crl(tstrTlsCrlInfo *pCRL);

@brief
            Asynchronous API that notifies the WINC with the Certificate Revocation List.

@param[in]  pCRL
            Pointer to the structure containing certificate revocation list details.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.
*/
int8_t m2m_wifi_send_crl(tstrTlsCrlInfo *pCRL);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_delete_sc(char *pcSsid, uint8_t u8SsidLen);

@brief
            Asynchronous API that deletes connection credentials (PSK, WEP key, 802.1X password) from WINC
            flash. Either deletes all credentials, or for a specific SSID.

@details
            Causes WINC to delete connection credentials. If the parameter is NULL, then WINC will delete
            all credentials from flash. Otherwise WINC will only delete credentials for matching SSID.
            Callback will report the status of the operation (success or not).

@param[in]  pcSsid
            SSID to match on when deleting credentials.
            SSID must not contain '\0'.
            NULL is a valid argument here, in which case all credentials are deleted.

@param[in]  u8SsidLen
            Length of SSID provided in pcSsid. Must be less than @ref M2M_MAX_SSID_LEN.
            This parameter is ignored if pcSsid is NULL.

@pre
            Prior to deleting credentials, the WINC driver should have been successfully initialized by calling the
            @ref m2m_wifi_init function.

@warning
            The option to delete for a specific SSID is currently not supported; all credentials are
            deleted regardless of the input parameters.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.
*/
int8_t m2m_wifi_delete_sc(char *pcSsid, uint8_t u8SsidLen);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_default_connect(void);

@brief
            Asynchronous API that attempts to reconnect to the last-associated access point.

@details
            Asynchronous WiFi connection function. An application calling this function will cause
            the firmware to attempt to reconnect to the access point with which it had last successfully connected.
            A failure to connect will result in a response of @ref M2M_WIFI_RESP_DEFAULT_CONNECT
            indicating a connection error as defined in the structure @ref tstrM2MDefaultConnResp.

            Possible errors are:
            @ref M2M_DEFAULT_CONN_EMPTY_LIST indicating that the connection list is empty, or
            @ref M2M_DEFAULT_CONN_SCAN_MISMATCH indicating a mismatch for the saved AP name.

@pre
            Prior to connecting, the WINC driver should have been successfully initialized by calling the
            @ref m2m_wifi_init function.

@warning
            This function must be called in station mode only.
            It is important to note that successful completion of a call to m2m_wifi_default_connect
            does not guarantee success of the WIFI connection; a negative return value indicates only
            locally-detected errors.

@see
            m2m_wifi_connect

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.
*/
int8_t m2m_wifi_default_connect(void);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_connect_open(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId);

@brief
            Asynchronous API to connect to an access point using open authentication.

@details
            Asynchronous WiFi connection function. An application calling this function will cause the
            firmware to attempt to connect to an access point matching the details in pstrNetworkId, with
            open authentication.
            On successful connection, the connection details may be saved in WINCs flash, according to
            the option selected in enuCredStoreOption.
            Once connection has been attempted (whether successful or otherwise), a response event
            @ref M2M_WIFI_RESP_CON_STATE_CHANGED will be sent to the callback function @ref tpfAppWifiCb
            provided during initialization @ref m2m_wifi_init.

            Possible results indicated by the response event are:
            - @ref M2M_WIFI_DISCONNECTED if the connection attempt failed.
            - @ref M2M_WIFI_CONNECTED if the connection attempt succeeded.

@pre
            Prior to attempting connection, the WINC driver must have been initialized by calling the
            @ref m2m_wifi_init function.

@warning
            This function is handled in station mode only.

@param[in]  enuCredStoreOption
            Option to specify whether connection details (i.e. the contents of pstrNetworkId) are stored in
            WINCs flash and, if so, whether they are encrypted before storing.

@param[in]  pstrNetworkId
            Structure specifying SSID/BSSID and WiFi channel.

@return
            The function returns @ref M2M_SUCCESS if the connect request has been successfully passed to the firmware and a negative value otherwise.
*/
int8_t m2m_wifi_connect_open(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_connect_wep(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuthWep *pstrAuthWep);

@brief
            Asynchronous API to connect to an access point using WEP authentication.

@details
            Asynchronous WiFi connection function. An application calling this function will cause the
            firmware to attempt to connect to an access point matching the details in pstrNetworkId, with
            the WEP key provided in pstrAuthWep.
            On successful connection, the connection details may be saved in WINCs flash, according to
            the option selected in enuCredStoreOption.
            Once connection has been attempted (whether successful or otherwise), a response event
            @ref M2M_WIFI_RESP_CON_STATE_CHANGED will be sent to the callback function @ref tpfAppWifiCb
            provided during initialization @ref m2m_wifi_init.

            Possible results indicated by the response event are:
            - @ref M2M_WIFI_DISCONNECTED if the connection attempt failed.
            - @ref M2M_WIFI_CONNECTED if the connection attempt succeeded.

@pre
            Prior to attempting connection, the WINC driver must have been initialized by calling the
            @ref m2m_wifi_init function.

@warning
            This function is handled in station mode only.

@param[in]  enuCredStoreOption
            Option to specify whether connection details (i.e. the contents of pstrNetworkId and pstrAuthWep) are stored in
            WINCs flash and, if so, whether they are encrypted before storing.

@param[in]  pstrNetworkId
            Structure specifying SSID/BSSID and WiFi channel.

@param[in]  pstrAuthWep
            Structure specifying the WEP key.

@return
            The function returns @ref M2M_SUCCESS if the connect request has been successfully passed to the firmware and a negative value otherwise.
*/
int8_t m2m_wifi_connect_wep(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuthWep *pstrAuthWep);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_connect_psk(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuthPsk *pstrAuthPsk);

@brief
            Asynchronous API to connect to an access point using WPA(2) PSK authentication.

@details
            Asynchronous WiFi connection function. An application calling this function will cause the
            firmware to attempt to connect to an access point matching the details in pstrNetworkId, with
            the PSK passphrase provided in pstrAuthPsk.
            On successful connection, the connection details may be saved in WINCs flash, according to
            the option selected in enuCredStoreOption.
            Once connection has been attempted (whether successful or otherwise), a response event
            @ref M2M_WIFI_RESP_CON_STATE_CHANGED will be sent to the callback function @ref tpfAppWifiCb
            provided during initialization @ref m2m_wifi_init.

            Possible results indicated by the response event are:
            - @ref M2M_WIFI_DISCONNECTED if the connection attempt failed.
            - @ref M2M_WIFI_CONNECTED if the connection attempt succeeded.

@pre
            Prior to attempting connection, the WINC driver must have been initialized by calling the
            @ref m2m_wifi_init function.

@warning
            This function is handled in station mode only.

@param[in]  enuCredStoreOption
            Option to specify whether connection details (i.e. the contents of pstrNetworkId and pstrAuthPsk) are stored in
            WINCs flash and, if so, whether they are encrypted before storing.

@param[in]  pstrNetworkId
            Structure specifying SSID/BSSID and WiFi channel.

@param[in]  pstrAuthPsk
            Structure specifying the Passphrase/PSK.

@return
            The function returns @ref M2M_SUCCESS if the connect request has been successfully passed to the firmware and a negative value otherwise.
*/
int8_t m2m_wifi_connect_psk(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuthPsk *pstrAuthPsk);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_1x_set_option(tenu1xOption enuOptionName, const void *pOptionValue, size_t OptionLen);

@brief
            API to set (write) options relating to WiFi connection using WPA(2) Enterprise authentication.

@details
            The following options can be set:<br>
            @ref WIFI_1X_BYPASS_SERVER_AUTH<br>
            @ref WIFI_1X_TIME_VERIF_MODE<br>
            @ref WIFI_1X_SESSION_CACHING<br>
            @ref WIFI_1X_SPECIFIC_ROOTCERT<br>
            The setting applies to all subsequent connection attempts via @ref m2m_wifi_connect_1x_mschap2
            or @ref m2m_wifi_connect_1x_tls.<br>
            Connection attempts via @ref m2m_wifi_default_connect use the
            settings which were in place at the time of the original connection.

@param[in]  enuOptionName
            The option to set.

@param[in]  pOptionValue
            Pointer to a buffer containing the value to set. The buffer must be at least as long as OptionLen.
            If OptionLen is 0, then pOptionValue may be NULL.

@param[in]  OptionLen
            The length of the option value being set.

@return
            The function returns @ref M2M_SUCCESS if the parameters are valid and @ref M2M_ERR_INVALID_ARG otherwise.
*/
int8_t m2m_wifi_1x_set_option(tenu1xOption enuOptionName, const void *pOptionValue, size_t OptionLen);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_1x_get_option(tenu1xOption enuOptionName, void *pOptionValue, size_t *pOptionLen);

@brief
            API to get (read) options relating to WiFi connection using WPA(2) Enterprise authentication.

@details
            The following options can be read:<br>
            @ref WIFI_1X_BYPASS_SERVER_AUTH<br>
            @ref WIFI_1X_TIME_VERIF_MODE<br>
            @ref WIFI_1X_SESSION_CACHING<br>
            @ref WIFI_1X_SPECIFIC_ROOTCERT<br>

@param[in]  enuOptionName
            The option to get.

@param[out] pOptionValue
            Pointer to a buffer to be filled with the value being read. The buffer must be at least as long as the length in pOptionLen

@param[inout]
            pOptionLen
            Pointer to a length.
            When calling the function, this length must be the length of the buffer available for reading the option value.
            When the function returns, this length is the length of the data that has been populated by the function.

@return
            The function returns @ref M2M_SUCCESS if the parameters are valid and @ref M2M_ERR_INVALID_ARG otherwise.
*/
int8_t m2m_wifi_1x_get_option(tenu1xOption enuOptionName, void *pOptionValue, size_t *pOptionLen);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_connect_1x_mschap2(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuth1xMschap2 *pstrAuth1xMschap2);

@brief
            Asynchronous API to connect to an access point using WPA(2) Enterprise authentication with
            MS-CHAP-V2 credentials.

@details
            Asynchronous WiFi connection function. An application calling this function will cause the
            firmware to attempt to connect to an access point matching the details in pstrNetworkId, with
            the Enterprise MS-CHAP-V2 credentials provided in pstrAuth1xMschap2.
            On successful connection, the connection details may be saved in WINCs flash, according to
            the option selected in enuCredStoreOption.
            Once connection has been attempted (whether successful or otherwise), a response event
            @ref M2M_WIFI_RESP_CON_STATE_CHANGED will be sent to the callback function tpfAppWifiCb
            provided during initialization @ref m2m_wifi_init.

            Possible results indicated by the response event are:
            - @ref M2M_WIFI_DISCONNECTED if the connection attempt failed.
            - @ref M2M_WIFI_CONNECTED if the connection attempt succeeded.

@pre
            Prior to attempting connection, the WINC driver must have been initialized by calling the
            @ref m2m_wifi_init function.

@warning
            This function is handled in station mode only.

@param[in]  enuCredStoreOption
            Option to specify whether connection details (i.e. the contents of pstrNetworkId and pstrAuth1xMschap2) are stored
            in WINCs flash and, if so, whether they are encrypted before storing.

@param[in]  pstrNetworkId
            Structure specifying SSID/BSSID and WiFi channel.

@param[in]  pstrAuth1xMschap2
            Structure specifying the MS-CHAP-V2 credentials.

@return
            The function returns @ref M2M_SUCCESS if the connect request has been successfully passed to the firmware and a negative value otherwise.
*/
int8_t m2m_wifi_connect_1x_mschap2(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuth1xMschap2 *pstrAuth1xMschap2);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_connect_1x_tls(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuth1xTls *pstrAuth1xTls);

@brief
            Asynchronous API to connect to an access point using WPA(2) Enterprise authentication with
            MS-CHAP-V2 credentials.

@details
            Asynchronous WiFi connection function. An application calling this function will cause the
            firmware to attempt to connect to an access point matching the details in pstrNetworkId, with
            the Enterprise TLS credentials provided in pstrAuth1xTls.
            On successful connection, the connection details may be saved in WINCs flash, according to
            the option selected in enuCredStoreOption.
            Once connection has been attempted (whether successful or otherwise), a response event
            @ref M2M_WIFI_RESP_CON_STATE_CHANGED will be sent to the callback function @ref tpfAppWifiCb
            provided during initialization @ref m2m_wifi_init.

            Possible results indicated by the response event are:
            - @ref M2M_WIFI_DISCONNECTED if the connection attempt failed.
            - @ref M2M_WIFI_CONNECTED if the connection attempt succeeded.

@pre
            Prior to attempting connection, the WINC driver must have been initialized by calling the
            @ref m2m_wifi_init function.

@warning
            This function is handled in station mode only.

@param[in]  enuCredStoreOption
            Option to specify whether connection details (i.e. the contents of pstrNetworkId and pstrAuth1xTls) are stored in
            WINCs flash and, if so, whether they are encrypted before storing.

@param[in]  pstrNetworkId
            Structure specifying SSID/BSSID and WiFi channel.

@param[in]  pstrAuth1xTls
            Structure specifying the EAP-TLS credentials.

@return
            The function returns @ref M2M_SUCCESS if the connect request has been successfully passed to the firmware and a negative value otherwise.
*/
int8_t m2m_wifi_connect_1x_tls(tenuCredStoreOption enuCredStoreOption, tstrNetworkId *pstrNetworkId, tstrAuth1xTls *pstrAuth1xTls);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_connect(char *pcSsid, uint8_t u8SsidLen, uint8_t u8SecType, void *pvAuthInfo, uint16_t u16Ch);

@brief
            Legacy asynchronous API to request connection to a specified access point.

@details
            This API is maintained for purposes of compatibility with legacy applications. It is
            implemented as a wrapper for the following new APIs:
            @ref m2m_wifi_connect_open
            @ref m2m_wifi_connect_wep
            @ref m2m_wifi_connect_psk
            @ref m2m_wifi_connect_1x_mschap2
            @ref m2m_wifi_connect_1x_tls
            These new APIs allow more flexibility and it is recommended that applications use them instead.

@param[in]  pcSsid
            A buffer holding the SSID corresponding to the requested AP.
            SSID must not contain '\0'.

@param[in]  u8SsidLen
            Length of the given SSID (not including any NULL termination).
            A length greater than or equal to @ref M2M_MAX_SSID_LEN will result in a negative error
            @ref M2M_ERR_FAIL.

@param[in]  u8SecType
            WiFi security type security for the network. It can be one of the following types:
            -@ref M2M_WIFI_SEC_OPEN
            -@ref M2M_WIFI_SEC_WEP
            -@ref M2M_WIFI_SEC_WPA_PSK
            -@ref M2M_WIFI_SEC_802_1X
            A value outside these possible values will result in a negative return error @ref M2M_ERR_FAIL.

@param[in]  pvAuthInfo
            Authentication parameters required for completing the connection. Its type is based on the
            security type. If the authentication parameters are NULL or are greater than the maximum length
            of the authentication parameters length as defined by @ref M2M_MAX_PSK_LEN a negative error will
            return @ref M2M_ERR_FAIL indicating connection failure.

@param[in]  u16Ch
            WiFi channel number as defined in @ref tenuM2mScanCh enumeration. Specifying a channel number
            greater than @ref M2M_WIFI_CH_14 returns a negative error @ref M2M_ERR_FAIL, unless
            the value is @ref M2M_WIFI_CH_ALL, since this indicates that the firmware should scan all channels
            to find the SSID specified in parameter pcSsid.

            Failure to find the connection match will return a negative error
            @ref M2M_DEFAULT_CONN_SCAN_MISMATCH.

@pre
            Prior to a successful connection request, the WiFi driver must have been successfully initialized
            through the call of the @ref m2m_wifi_init function.

@warning
            If there is a '\0' character within the first u8SsidLen characters, then this function will assume
            that the input u8SsidLen was incorrect, set length to strlen(pcSsid) and continue.<br>
            It is recommended that the following WiFi connect APIs are used instead:
            @ref m2m_wifi_connect_open
            @ref m2m_wifi_connect_wep
            @ref m2m_wifi_connect_psk
            @ref m2m_wifi_connect_1x_mschap2
            @ref m2m_wifi_connect_1x_tls

            Additionally:
            - This function must be called in station mode only.
            - Successful completion of this function does not guarantee success of the WIFI connection, and
            a negative return value indicates only locally-detected errors.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            tstr1xAuthCredentials
            tstrM2mWifiWepParams
*/
int8_t m2m_wifi_connect(char *pcSsid, uint8_t u8SsidLen, uint8_t u8SecType, void *pvAuthInfo, uint16_t u16Ch);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_connect_sc(char *pcSsid, uint8_t u8SsidLen, uint8_t u8SecType, void *pvAuthInfo, uint16_t u16Ch, uint8_t u8NoSaveCred);

@brief
            Legacy asynchronous API to request connection to a specific AP with the option to save credentials in Flash.

@details
            This API is maintained for purposes of compatibility with legacy applications. It is
            implemented as a wrapper for the following new APIs:
            @ref m2m_wifi_connect_open
            @ref m2m_wifi_connect_wep
            @ref m2m_wifi_connect_psk
            @ref m2m_wifi_connect_1x_mschap2
            @ref m2m_wifi_connect_1x_tls
            These new APIs allow more flexibility and it is recommended that applications use them instead.

@param[in]  pcSsid
            A buffer holding the SSID corresponding to the requested AP.
            SSID must not contain '\0'.

@param[in]  u8SsidLen
            Length of the given SSID (not including any NULL termination).
            A length greater than or equal to @ref M2M_MAX_SSID_LEN will result in a negative error
            @ref M2M_ERR_FAIL.

@param[in]  u8SecType
            WiFi security type security for the network (see @ref tenuM2mSecType). It can be one of the following types:
            -@ref M2M_WIFI_SEC_OPEN
            -@ref M2M_WIFI_SEC_WEP
            -@ref M2M_WIFI_SEC_WPA_PSK
            -@ref M2M_WIFI_SEC_802_1X
            A value outside these possible values will result in a negative return error @ref M2M_ERR_FAIL.

@param[in]  pvAuthInfo
            Authentication parameters required for completing the connection. Its type is based on the
            security type. If the authentication parameters are NULL or are greater than the maximum length
            of the authentication parameters length as defined by @ref M2M_MAX_PSK_LEN a negative error will
            return @ref M2M_ERR_FAIL indicating connection failure.

@param[in]  u16Ch
            WiFi channel number as defined in @ref tenuM2mScanCh enumeration. Specification of a channel
            number greater than @ref M2M_WIFI_CH_14 returns a negative error @ref M2M_ERR_FAIL unless
            the value is @ref M2M_WIFI_CH_ALL. A channel number of @ref M2M_WIFI_CH_ALL indicates that the
            firmware should scan all channels to find the SSID specified in parameter pcSsid.

            Failure to find the connection match will return a negative error
            @ref M2M_DEFAULT_CONN_SCAN_MISMATCH.

@param[in]  u8NoSaveCred
            Option to store the access point SSID and password into the WINC flash memory or not.

@pre
            Prior to a successful connection request, the WiFi driver must have been successfully initialized through the call of the @ref m2m_wifi_init function.

@warning
            If there is a '\0' character within the first u8SsidLen characters, then this function will assume
            that the input u8SsidLen was incorrect, set length to strlen(pcSsid) and continue.<br>
            It is recommended that the following WiFi connect APIs are used instead:
            @ref m2m_wifi_connect_open
            @ref m2m_wifi_connect_wep
            @ref m2m_wifi_connect_psk
            @ref m2m_wifi_connect_1x_mschap2
            @ref m2m_wifi_connect_1x_tls

            Additionally:
            - This function must be called in station mode only.
            - Successful completion of this function does not guarantee success of the WIFI connection, and
            a negative return value indicates only locally-detected errors.

@see
            tenuM2mSecType
            tstr1xAuthCredentials
            tstrM2mWifiWepParams

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.
*/
int8_t m2m_wifi_connect_sc(char *pcSsid, uint8_t u8SsidLen, uint8_t u8SecType, void *pvAuthInfo, uint16_t u16Ch, uint8_t u8NoSaveCred);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_disconnect(void);

@brief
            Synchronous API to request disconnection from a network.

@details
            Request a WiFi disconnect from the currently connected AP.
            The connection status will be indicated to the application via a @ref M2M_WIFI_RESP_CON_STATE_CHANGED event.
            The status will be one of those defined in @ref tenuM2mConnState, with @ref M2M_WIFI_DISCONNECTED indicating
            a successful disconnection.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@pre
            Disconnection request must be made to a successfully connected AP. If the WINC is not in the connected state, a call to this function will hold insignificant.

@warning
            This function must be called in station mode only.

@see
            m2m_wifi_connect
            m2m_wifi_connect_sc
            m2m_wifi_default_connect
            m2m_wifi_connect_open
            m2m_wifi_connect_wep
            m2m_wifi_connect_psk
            m2m_wifi_connect_1x_mschap2
            m2m_wifi_connect_1x_tls
*/
int8_t m2m_wifi_disconnect(void);

/*!
@ingroup    WLANPROVISION
@fn int8_t m2m_wifi_start_provision_mode(tstrM2MAPConfig *pstrAPConfig, char *pcHttpServerDomainName, uint8_t bEnableHttpRedirect);

@brief
            Asynchronous API for control of WiFi provisioning functionality.

@details
            This function allows the application to start the WINC in 'provisioning mode', a special mode
            that triggers the WINC to create a WiFi access point, DHCP server, and HTTP server.

            The HTTP server presents a provisioning page to a connected client which lists the access points
            detected in the vicinity of the WINC, and allows one of these to be selected and any appropriate
            credentials to be entered. This allows a headless system to be provisioned (configured to
            connect with an access point).

            Provisioning status is returned in an event @ref M2M_WIFI_RESP_PROVISION_INFO.

@param[in]  pstrAPConfig
            AP configuration parameters as defined in @ref tstrM2MAPConfig configuration structure.
            If a NULL value is passed in, the call will result in a negative error @ref M2M_ERR_FAIL.

@param[in]  pcHttpServerDomainName
            Domain name of the HTTP Provision WEB server which others will use to load the provisioning Home page.
            The domain name can have one of the following 3 forms:
            - 1. "wincprov.com"
            - 2. "http://wincprov.com"
            - 3. "https://wincprov.com"

            Forms 1 and 2 are equivalent, they will both start a plain http server, while form 3
            will start a secure HTTP provisioning Session (HTTP over SSL connection).

@param[in]  bEnableHttpRedirect
            A flag to enable/disable the HTTP redirect feature. If Secure provisioning is enabled (i.e. the server
            domain name uses "https" prefix) this flag is ignored (no meaning for redirect in HTTPS).
            Possible values are:
            - Zero:     DO NOT use HTTP Redirect. In this case, the associated device could open the provisioning
            page ONLY when the HTTP Provision URL of the WINC HTTP Server is correctly written on the browser.
            - Non-zero: Use HTTP Redirect. In this case, all http traffic (http://URL) from the associated
            device (Phone, PC, etc) will be redirected to the WINC HTTP Provisioning Home page.

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at startup.
            Registering the callback is done through passing it to the initialization @ref m2m_wifi_init function.
            - The event @ref M2M_WIFI_RESP_CONN_INFO must be handled in the callback to receive the requested connection info.

@warning
            Do not use ".local" in the pcHttpServerDomainName.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            tpfAppWifiCb
            m2m_wifi_init
            M2M_WIFI_RESP_PROVISION_INFO
            m2m_wifi_stop_provision_mode
            tstrM2MAPConfig

@section    WIFIExample1 Example
            The example demonstrates a code snippet for how provisioning is triggered and the response event
            received accordingly.

@include    m2m_wifi_start_provision_mode.c
@example    m2m_wifi_start_provision_mode.c
*/
int8_t m2m_wifi_start_provision_mode(tstrM2MAPConfig *pstrAPConfig, char *pcHttpServerDomainName, uint8_t bEnableHttpRedirect);

/*!
@ingroup    WLANPROVISION
@fn         int8_t m2m_wifi_start_provision_mode_ext(tstrM2MAPModeConfig *pstrAPModeConfig, char *pcHttpServerDomainName, uint8_t bEnableHttpRedirect);

@brief
            Asynchronous API for control of WiFi provisioning functionality with extended options.

@details
            This function allows the application to start the WINC in 'provisioning mode', a special mode
            that triggers the WINC to create a WiFi access point, DHCP server, and HTTP server.

            The HTTP server presents a provisioning page to a connected client which lists the access points
            detected in the vicinity of the WINC, and allows one of these to be selected and any appropriate
            credentials to be entered. This allows a headless system to be provisioned (configured to
            connect with an access point).

            Provisioning status is returned in an event @ref M2M_WIFI_RESP_PROVISION_INFO.

@param[in]  pstrAPModeConfig
            AP configuration parameters as defined in @ref tstrM2MAPModeConfig configuration structure.
            A NULL value passed in, will result in a negative error @ref M2M_ERR_FAIL.

@param[in]  pcHttpServerDomainName
            Domain name of the HTTP Provision WEB server which others will use to load the provisioning Home page.
            The domain name can have one of the following 3 forms:
            - 1. "wincprov.com"
            - 2. "http://wincprov.com"
            - 3. "https://wincprov.com"

            The forms 1 and 2 are equivalent, they both will start a plain http server, while form 3
            will start a secure HTTP provisioning Session (HTTP over SSL connection).

@param[in]  bEnableHttpRedirect
            A flag to enable/disable the HTTP redirect feature. If Secure provisioning is enabled (i.e. the server
            domain name uses "https" prefix) this flag is ignored (no meaning for redirect in HTTPS).
            Possible values are:
            - Zero:     DO NOT use HTTP Redirect. In this case the associated device could open the
            provisioning page ONLY when the HTTP Provision URL of the WINC HTTP Server is
            correctly written on the browser.
            - Non-zero: Use HTTP Redirect. In this case, all http traffic (http://URL) from the
            associated device (Phone, PC, ...etc) will be redirected to the WINC HTTP
            Provisioning Home page.

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at startup.
            Registering the callback is done through passing it to the initialization @ref m2m_wifi_init function.
            - The event @ref M2M_WIFI_RESP_CONN_INFO must be handled in the callback to receive the requested connection info.

@warning
            Do not use ".local" in the pcHttpServerDomainName.

@see
            tpfAppWifiCb
            m2m_wifi_init
            M2M_WIFI_RESP_PROVISION_INFO
            m2m_wifi_stop_provision_mode
            tstrM2MAPModeConfig

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@section    WIFIExample1b Example
            The example demonstrates a code snippet for how provisioning is triggered and the response event
            received accordingly.

@include    m2m_wifi_start_provision_mode_ext.c
@example    m2m_wifi_start_provision_mode_ext.c
*/
int8_t m2m_wifi_start_provision_mode_ext(tstrM2MAPModeConfig *pstrAPModeConfig, char *pcHttpServerDomainName, uint8_t bEnableHttpRedirect);

/*!
@ingroup    WLANPROVISION
@fn         int8_t m2m_wifi_stop_provision_mode(void);

@brief
            Synchronous API for terminating provisioning mode on the WINC.

@details
            This function will terminate any currently active provisioning mode on the WINC, returning the WINC to idle.

@pre
            An active provisioning session must be active before it is terminated through this function.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@see
            m2m_wifi_start_provision_mode
*/
int8_t m2m_wifi_stop_provision_mode(void);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_get_connection_info(void);

@brief
            Asynchronous API for retrieving the WINC connection status.

@details
            Requests the connection status from the WINC including information regarding any access
            point to which it is currently connected, or any non-AP station that is connected to the WINC.
            All information will be returned to the application via the WiFi notification callback through
            the event @ref M2M_WIFI_RESP_CONN_INFO.

            The connection info can be retrieved using the structure @ref tstrM2MConnInfo which contains:
            - Connection Security
            - Connection RSSI
            - Remote MAC address
            - Remote IP address
            - SSID of the network (in cases where the WINC is in non-AP mode)

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at
            startup. Registering the callback is done through passing it to the initialization
            @ref m2m_wifi_init function.
            - The event @ref M2M_WIFI_RESP_CONN_INFO must be handled in the callback to receive the
            requested connection info.

@warning
            - In case the WINC is operating in AP mode, the SSID field will be returned as a NULL string.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            M2M_WIFI_RESP_CONN_INFO,
            tstrM2MConnInfo
@section    WIFIExample2 Example
            The code snippet shows an example of how WiFi connection information is retrieved .

@include    m2m_wifi_get_connection_info.c
@example    m2m_wifi_get_connection_info.c
*/
int8_t m2m_wifi_get_connection_info(void);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_set_mac_address(uint8_t au8MacAddress[6]);

@brief
            Asynchronous API for assigning a MAC address to the WINC.

@details
            This function is intended to allow non-production software to assign a MAC address to the WINC.

@warning
            This function is intended for development use only and not for use in production software.

@param[in]  au8MacAddress
            MAC Address to be provisioned to the WINC.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.
*/
int8_t m2m_wifi_set_mac_address(uint8_t au8MacAddress[6]);

/*!
@ingroup    WLANWPS
@fn         int8_t m2m_wifi_wps(uint8_t u8TriggerType, const char *pcPinNumber);

@brief
            Asynchronous API to engage the WINC WiFi Protected Setup (enrollee) function.

@details
            This function can be called to make the WINC enter WPS (WiFi Protected Setup) mode. The result
            is passed to the WiFi notification callback with the event @ref M2M_WIFI_REQ_WPS.

@param[in]  u8TriggerType
            WPS Trigger method. This may be:
            - @ref WPS_PIN_TRIGGER   Push button method
            - @ref WPS_PBC_TRIGGER   Pin method

@param[in]  pcPinNumber
            Valid only if the u8TriggerType is @ref WPS_PIN_TRIGGER, this parameter contains the PIN number.
            The number must follow the format as given in the WSC1.0 specification.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@warning
            This function is not allowed in AP modes.

@pre
            - A WiFi notification callback of type (@ref tpfAppWifiCb MUST be implemented and registered at
            startup. Registering the callback is done through passing it to @ref m2m_wifi_init.
            - The event @ref M2M_WIFI_REQ_WPS must be handled in the callback to receive the WPS status.
            - The WINC device MUST be in IDLE or STA mode. If AP is active, WPS will not be performed.
            - The @ref m2m_wifi_handle_events MUST be called periodically to receive
            the responses in the callback.

@see
            tpfAppWifiCb
            m2m_wifi_init
            M2M_WIFI_REQ_WPS
            tenuWPSTrigger
            tstrM2MWPSInfo

@section    WIFIExample3 Example
            The code snippet shows an example of how WiFi WPS is triggered .

@include    m2m_wifi_wps.c
@example    m2m_wifi_wps.c
*/
int8_t m2m_wifi_wps(uint8_t u8TriggerType, const char *pcPinNumber);

/*!
@ingroup    WLANWPS
@fn         int8_t m2m_wifi_wps_disable(void);

@brief
            Asynchronous API that disables WiFi Protected Setup mode in the WINC.

@pre
            WINC should be already in WPS mode using @ref m2m_wifi_wps.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_wps
*/
int8_t m2m_wifi_wps_disable(void);

/*!
@ingroup    WLANAP
@fn         int8_t m2m_wifi_enable_ap(const tstrM2MAPConfig *pstrM2MAPConfig);

@brief
            Asynchronous API to enable access point (AKA "hot-spot") mode on the WINC.

@details
            The WINC supports the ability to operate as an access point with the following limitations:
            - Only 1 station may be associated at any given time.
            - Open system and WEP are the only security suites supported.

@param[in]  pstrM2MAPConfig
            A structure holding the AP configurations.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@warning
            This function is not allowed in STA mode.

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb  MUST be implemented and registered at initialization. Registering the callback
            is done through passing it to the @ref m2m_wifi_init.
            - The event @ref M2M_WIFI_REQ_DHCP_CONF must be handled in the callback.
            - The @ref m2m_wifi_handle_events MUST be called to receive the responses in the callback.

@see
            tpfAppWifiCb
            tenuM2mSecType
            m2m_wifi_init
            M2M_WIFI_REQ_DHCP_CONF
            tstrM2mWifiStateChanged
            tstrM2MAPConfig

@section    WIFIExample5 Example
            The code snippet demonstrates how the AP mode is enabled after the driver is initialized in the application's main function and the handling
            of the event @ref M2M_WIFI_REQ_DHCP_CONF, to indicate successful connection.

@include    m2m_wifi_enable_ap.c
@example    m2m_wifi_enable_ap.c
*/
int8_t m2m_wifi_enable_ap(const tstrM2MAPConfig *pstrM2MAPConfig);

/*!
@ingroup    WLANAP
@fn         int8_t m2m_wifi_enable_ap_ext(const tstrM2MAPModeConfig *pstrM2MAPModeConfig);

@brief
            Asynchronous API to enable access point (AKA "hot-spot") mode on the WINC with extended options.

@details
            The WINC supports the ability to operate as an access point with the following limitations:
            - Only 1 station may be associated at any given time.
            - Open system and WEP are the only security suites supported.

@param[in]  pstrM2MAPModeConfig
            A structure holding the AP configurations.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@warning
            This function is not allowed in STA mode.

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb  MUST be implemented and registered at initialization. Registering the callback
            is done through passing it to the @ref m2m_wifi_init.
            - The event @ref M2M_WIFI_REQ_DHCP_CONF must be handled in the callback.
            - The @ref m2m_wifi_handle_events MUST be called to receive the responses in the callback.

@see
            tpfAppWifiCb
            tenuM2mSecType
            m2m_wifi_init
            M2M_WIFI_REQ_DHCP_CONF
            tstrM2mWifiStateChanged
            tstrM2MAPModeConfig
            tstrM2MAPConfigExt

@section    WIFIExample5b Example
            The code snippet demonstrates how the AP mode is enabled after the driver is initialized in the application's main function and the handling
            of the event @ref M2M_WIFI_REQ_DHCP_CONF, to indicate successful connection.

@include    m2m_wifi_enable_ap_ext.c
@example    m2m_wifi_enable_ap_ext.c
*/
int8_t m2m_wifi_enable_ap_ext(const tstrM2MAPModeConfig *pstrM2MAPModeConfig);

/*!
@ingroup    WLANAP
@fn         int8_t m2m_wifi_disable_ap(void);

@brief
            Synchronous API to disable access point mode on the WINC.

@details
            Must be called only when the AP is enabled through the @ref m2m_wifi_enable_ap
            function. Otherwise the call to this function will not be useful.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_enable_ap
*/
int8_t m2m_wifi_disable_ap(void);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_set_static_ip(tstrM2MIPConfig *pstrStaticIPConf);

@brief
            Asynchronous API to manually assign a (static) IP address to the WINC.

@details
            Assigns a static IP address to the WINC.
            Typically an infrastructure access point will be able to provide an IP address to all clients
            after they associate. The WINC will request configuration via DHCP automatically after
            successfully connecting to an access point.
            This function should only be called in the event that the network has no DHCP server or in case the application
            wants to assign a predefined known IP address and the application.
            This function can be used to assign a static IP address in case the application knows the specifics of the network.
            The user must keep in mind that assigning a static IP address might
            result in an IP address conflict. In case of an IP address conflict observed
            by the WINC the user will get a response of @ref M2M_WIFI_RESP_IP_CONFLICT
            in the WiFi callback. The application is then responsible to either solve the
            conflict or assign another IP address.

@pre
            The application must first call @ref m2m_wifi_enable_dhcp to request that DHCP functionality is
            disabled prior to calling this function.

@warning
            Exercise caution using this function.
            DHCP is the preferred method for configuring IP addresses.

@param[in]  pstrStaticIPConf
            Pointer to a structure holding the static IP configuration (IP, Gateway, subnet mask and DNS address).

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            tstrM2MIPConfig
*/
int8_t m2m_wifi_set_static_ip(tstrM2MIPConfig *pstrStaticIPConf);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_enable_dhcp(uint8_t u8DhcpEn);

@brief
            Asynchronous function to control the DHCP client functionality within the WINC.

@details
            This function allows the application to control the behaviour of the DHCP client function within
            the WINC once it has associated with an access point. DHCP client functionality is enabled by
            default.

@param[in]  u8DhcpEn
            The state of the DHCP client feature after successful association with an access point:
            - 1: Enables DHCP client after connection.
            - 0: Disables DHCP client after connection.

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.

@warning
            DHCP client is enabled by default.
            This Function should be called to disable DHCP client operation before using @ref m2m_wifi_set_static_ip.

@see
            m2m_wifi_set_static_ip
*/
int8_t m2m_wifi_enable_dhcp(uint8_t u8DhcpEn);

/*!
@ingroup    WLANSCAN
@fn         int8_t m2m_wifi_set_scan_options(tstrM2MScanOption *ptstrM2MScanOption);

@brief
            Synchronous API for configuring the behaviour of the WINC network scanning functions.

@details
            This function allows the application to tune the scanning behaviour of the WINC using the
            parameters described in @ref tstrM2MScanOption.

@param[in]  ptstrM2MScanOption;
            Pointer to the structure holding the Scan Parameters.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            tenuM2mScanCh
            m2m_wifi_request_scan
            tstrM2MScanOption
*/
int8_t m2m_wifi_set_scan_options(tstrM2MScanOption *ptstrM2MScanOption);

/*!
@ingroup    WLANSCAN
@fn         int8_t m2m_wifi_set_stop_scan_on_first(uint8_t u8StopScanOption);

@brief
            Synchronous API for enabling/disabling the stop scan on first result of the WINCs network scanning functions.

@details
            Allows for enabling/disabling of stop scan on first result. When enabled, the WINC will stop the scan as soon as
            it detects a network and return the results to the host. Setting is persistent and will need to be explicitly
            reverted back by the application if it no longer wishes for it to be enabled.

@param[in]  u8StopScanOption;
            Setting for enabling or disabling Stopping Scan on first result.
            1 = Enabled, 0 = Disabled (Default)

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            tenuM2mScanCh
            tstrM2MScanOption
            tstrM2MStopScanOption
            m2m_wifi_request_scan
            m2m_wifi_set_scan_options
*/
int8_t m2m_wifi_set_stop_scan_on_first(uint8_t u8StopScanOption);

/*!
@ingroup    WLANSCAN
@fn         int8_t m2m_wifi_set_scan_region(uint16_t u16ScanRegion);

@brief
            Synchronous API for configuring the regulatory restrictions that may affect the WINC scanning behaviour.

@details
            This function sets a property called the scan region, a parameter that affects the range of
            channels that the WINC may legally scan given a geographic region.

            For 2.4GHz, supported in the current release, the requested scan region cannot exceed the
            maximum number of channels (14).

@param[in]  u16ScanRegion
            @ref ASIA
            @ref EUROPE
            @ref NORTH_AMERICA

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            tenuM2mScanRegion
            m2m_wifi_request_scan
*/
int8_t m2m_wifi_set_scan_region(uint16_t u16ScanRegion);

/*!
@ingroup    WLANSCAN
@fn         int8_t m2m_wifi_request_scan(uint8_t u8Ch);

@brief
            Asynchronous API to request the WINC to scan for networks.

@details
            Scan statuses are delivered to the application via the WiFi event callback (@ref tpfAppWifiCb) in
            three stages. The first step involves the event @ref M2M_WIFI_RESP_SCAN_DONE which, if successful,
            provides the number of detected networks (access points). The application must then read the list
            of access points via multiple calls to the asynchronous @ref m2m_wifi_req_scan_result API. For
            each call to this function, the application will receive (step three) the event
            @ref M2M_WIFI_RESP_SCAN_RESULT.

@param[in]  u8Ch
            RF Channel ID for SCAN operation. It should be set according to @ref tenuM2mScanCh, with a
            value of @ref M2M_WIFI_CH_ALL to scan all channels.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value      otherwise.

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at
            initialization. Registration of the callback is done via @ref m2m_wifi_init.
            - The events @ref M2M_WIFI_RESP_SCAN_DONE and @ref M2M_WIFI_RESP_SCAN_RESULT must be handled in
            the (@ref tpfAppWifiCb) callback.
            - The @ref m2m_wifi_handle_events function MUST be called to receive the responses in the
            callback.

@warning
            This API is valid only for STA mode, it may be called regardless of connection state (connected or disconnected states).

@see
            M2M_WIFI_RESP_SCAN_DONE
            M2M_WIFI_RESP_SCAN_RESULT
            tpfAppWifiCb
            tstrM2mWifiscanResult
            tenuM2mScanCh
            m2m_wifi_init
            m2m_wifi_handle_events
            m2m_wifi_req_scan_result

@section    WIFIExample6 Example
            The code snippet demonstrates an example of how the scan request is called from the application's main function and the handling of
            the events received in response.

@include    m2m_wifi_request_scan.c
@example    m2m_wifi_request_scan.c
*/
int8_t m2m_wifi_request_scan(uint8_t u8Ch);

/*!
@ingroup    WLANSCAN
@fn         int8_t m2m_wifi_request_scan_passive(uint8_t u8Ch, uint16_t u16ScanTime);

@brief
            Similar to @ref m2m_wifi_request_scan but performs passive scanning instead of active scanning.

@param[in]  u8Ch
            RF Channel ID for SCAN operation. It should be set according to @ref tenuM2mScanCh.
            With a value of @ref M2M_WIFI_CH_ALL, means to scan all channels.

@param[in]  u16ScanTime
            The time in ms that passive scan is listening for beacons on each channel per one slot, enter 0 for default setting.

@warning
            This function is not allowed in AP mode. It works only for STA mode (both connected or disconnected states).

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at initialization. Registering the callback
            is done through passing it to the @ref m2m_wifi_init.
            - The events @ref M2M_WIFI_RESP_SCAN_DONE and @ref M2M_WIFI_RESP_SCAN_RESULT.
            must be handled in the callback.
            - The @ref m2m_wifi_handle_events function MUST be called to receive the responses in the callback.

@see
            M2M_WIFI_RESP_SCAN_DONE
            M2M_WIFI_RESP_SCAN_RESULT
            tpfAppWifiCb
            tstrM2MScanOption
            tstrM2mWifiscanResult
            tenuM2mScanCh
            m2m_wifi_init
            m2m_wifi_request_scan
            m2m_wifi_handle_events
            m2m_wifi_req_scan_result

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_wifi_request_scan_passive(uint8_t u8Ch, uint16_t u16ScanTime);

/*!
@ingroup    WLANSCAN
@fn         int8_t m2m_wifi_request_scan_ssid_list(uint8_t u8Ch, uint8_t *pu8Ssidlist);

@brief
            Asynchronous WiFi scan request on the given channel and the hidden scan list.

@details
            The scan status is delivered in the WiFi event callback and then the application
            is to read the scan results sequentially.
            The number of  APs found (N) is returned in event @ref M2M_WIFI_RESP_SCAN_DONE with the number of found
            APs.
            The application could read the list of APs by calling the function @ref m2m_wifi_req_scan_result N times.

@param[in]  u8Ch
            RF Channel ID for SCAN operation. It should be set according to @ref tenuM2mScanCh.
            With a value of @ref M2M_WIFI_CH_ALL, means to scan all channels.

@param[in]  pu8Ssidlist
            u8SsidList is a buffer containing a list of hidden SSIDs to
            include during the scan. The first byte in the buffer, u8SsidList[0],
            is the number of SSIDs encoded in the string. The number of hidden SSIDs
            cannot exceed @ref MAX_HIDDEN_SITES. All SSIDs are concatenated in the following
            bytes and each SSID is prefixed with a one-byte header containing its length.
            The total number of bytes in u8SsidList buffer, including length byte, cannot
            exceed 133 bytes (MAX_HIDDEN_SITES SSIDs x 32 bytes each, which is max SSID length).
            For instance, encoding the two hidden SSIDs "DEMO_AP" and "TEST"
            results in the following buffer content:

@code
            uint8_t u8SsidList[14];
            u8SsidList[0] = 2; // Number of SSIDs is 2
            u8SsidList[1] = 7; // Length of the string "DEMO_AP" without NULL termination
            memcpy(&u8SsidList[2], "DEMO_AP", 7);  // Bytes index 2-9 containing the string DEMO_AP
            u8SsidList[9] = 4; // Length of the string "TEST" without NULL termination
            memcpy(&u8SsidList[10], "TEST", 4);  // Bytes index 10-13 containing the string TEST
@endcode

@note
            It works with STA/AP mode (connected or disconnected).

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at initialization. Registering the callback
            is done through passing it to the @ref m2m_wifi_init.
            - The events @ref M2M_WIFI_RESP_SCAN_DONE and @ref M2M_WIFI_RESP_SCAN_RESULT.
            must be handled in the callback.
            - The @ref m2m_wifi_handle_events function MUST be called to receive the responses in the callback.

@see
            M2M_WIFI_RESP_SCAN_DONE
            M2M_WIFI_RESP_SCAN_RESULT
            tpfAppWifiCb
            tstrM2mWifiscanResult
            tenuM2mScanCh
            m2m_wifi_init
            m2m_wifi_handle_events
            m2m_wifi_req_scan_result

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

@section    WIFIExample6b Example
            The code snippet demonstrates an example of how the scan request is called from the application's main function and the handling of
            the events received in response.

@include    m2m_wifi_request_scan_ssid_list.c
@example    m2m_wifi_request_scan_ssid_list.c
*/
int8_t m2m_wifi_request_scan_ssid_list(uint8_t u8Ch, uint8_t *pu8Ssidlist);

/*!
@ingroup    WLANSCAN
@fn         uint8_t m2m_wifi_get_num_ap_found(void);

@brief
            Synchronous function to retrieve the number of AP's found during the last scan operation.

@details
            The function reads the number of APs from global variable which was updated in the WiFi
            callback function through the @ref M2M_WIFI_RESP_SCAN_DONE event.
            Function used only in STA mode only.

@see
            m2m_wifi_request_scan
            M2M_WIFI_RESP_SCAN_DONE
            M2M_WIFI_RESP_SCAN_RESULT

@pre
            m2m_wifi_request_scan must be called first to ensure up to date results are available.
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered at initialization. Registering the callback
            is done through passing it to the @ref m2m_wifi_init.
            - The event @ref M2M_WIFI_RESP_SCAN_DONE must be handled in the callback to receive the requested scan information.

@warning
            This function must be called only in the WiFi callback function when the events
            @ref M2M_WIFI_RESP_SCAN_DONE or @ref M2M_WIFI_RESP_SCAN_RESULT are received.
            Calling this function in any other place will result in undefined/outdated numbers.

@return
            Returns the number of AP's found in the last Scan Request.

@section    WIFIExample7 Example
            The code snippet demonstrates an example of how the scan request is called from the application's main function and the handling of
            the events received in response.

@include    m2m_wifi_get_num_ap_found.c
@example    m2m_wifi_get_num_ap_found.c
*/
uint8_t m2m_wifi_get_num_ap_found(void);

/*!
@ingroup    WLANSCAN
@fn         int8_t m2m_wifi_req_scan_result(uint8_t u8Index);

@brief
            Asynchronous API to request the information of an access point discovered via scanning.

@details
            This function allows the information of any discovered access point to be retrieved. When a
            scan is completed, the application is informed of the number of networks (access points)
            discovered. Calling this function with an index, N, will return the information for the Nth
            access point. The information will be returned to the application via a
            @ref M2M_WIFI_RESP_SCAN_RESULT event, and the response data may be obtained through casting
            the pointer (pvMsg) to @ref tstrM2mWifiscanResult.

@param[in]  u8Index
            Index for the requested result, the index range start from 0 till number of AP's found.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            tstrM2mWifiscanResult
            m2m_wifi_get_num_ap_found
            m2m_wifi_request_scan

@pre
            - @ref m2m_wifi_request_scan must be called first to ensure up to date results are available.
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered
            in order to receive scan data after calling this function. Registration of the callback
            is done via the @ref m2m_wifi_init function.
            - The event @ref M2M_WIFI_RESP_SCAN_RESULT must be handled in the callback to receive the
            requested scan information.

@warning
            - This API is valid only for STA mode, it may be called regardless of the connection state (connected or disconnected).
            - Calling this function without first issuing a scan request may lead to stale data being recovered.
            - Application code should refrain from introducing significant delays between issuing the scan
            request and scan result requests.

@section    WIFIExample8 Example
            The code snippet demonstrates an example of how the scan request is called from the application's main function and
            the handling of the events received in the response.

@include    m2m_wifi_req_scan_result.c
@example    m2m_wifi_req_scan_result.c
*/
int8_t m2m_wifi_req_scan_result(uint8_t u8Index);

/*!
@ingroup    WLANCONNECT
@fn         int8_t m2m_wifi_req_curr_rssi(void);

@brief
            Asynchronous API to request the current Receive Signal Strength (RSSI) of the current connection.

@details
            This function will result in the application receiving the RSSI via a
            @ref M2M_WIFI_RESP_CURRENT_RSSI event.

@pre
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered
            during initialization. Registration of the callback is done through passing it to @ref m2m_wifi_init
            via the @ref tstrWifiInitParam initialization structure.
            - The event @ref M2M_WIFI_RESP_CURRENT_RSSI must be handled in the callback to receive the requested Rssi information.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@section    WIFIExample9 Example
            The code snippet demonstrates how the RSSI request is called in the application's main function and the handling of the event received in the callback.

@include    m2m_wifi_req_curr_rssi.c
@example    m2m_wifi_req_curr_rssi.c
*/
int8_t m2m_wifi_req_curr_rssi(void);

/*!
@ingroup    WLANCONF
@fn int8_t m2m_wifi_get_otp_mac_address(uint8_t *pu8MacAddr, uint8_t *pu8IsValid);

@brief
            Synchronous API to query the MAC address programmed into the WINC OTP memory.

@details
            This function attempts to read the device's MAC address from the One Time Programmable (OTP)
            memory on the WINC. The presence (yes or no) of a MAC address in the OTP memory and, in the case
            of it being present, its value is returned via RAM pointed to by the input arguments.

            Request the MAC address stored on the One Time Programmable(OTP) memory of the device.
            The function is blocking until the response is received.

@pre
            Prior call to @ref m2m_wifi_init is required before any WIFI/socket function.

@param[out] pu8MacAddr
            Output MAC address buffer 6 bytes in size. Valid only if *pu8Valid=1.

@param[out] pu8IsValid
            A boolean value set by the callee to indicate the validity of pu8MacAddr in OTP. If no MAC has
            been programmed in the OTP the value of this parameter will be zero; otherwise it will be non-zero.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@see
            m2m_wifi_get_mac_address
*/
int8_t m2m_wifi_get_otp_mac_address(uint8_t *pu8MacAddr, uint8_t *pu8IsValid);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_get_mac_address(uint8_t *pu8MacAddr)

@brief
            Synchronous API to retrieve the MAC address currently in use by the device.

@details
            This function obtains the MAC address that is currently in use by the device. If the function
            returns with @ref M2M_SUCCESS then the content of the memory referenced by pu8MacAddr will be
            populated with the 6 byte MAC address; otherwise, that memory will be left unchanged.

@pre
            Prior call to @ref m2m_wifi_init is required before any WIFI/socket function.

@param[out] pu8MacAddr
            Pointer to a buffer in memory containing a 6-byte MAC address (provided function returns @ref M2M_SUCCESS).

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.

@see
            m2m_wifi_get_otp_mac_address
*/
int8_t m2m_wifi_get_mac_address(uint8_t *pu8MacAddr);

/*!
@ingroup    WLANPS
@fn         int8_t m2m_wifi_set_sleep_mode(uint8_t u8PsTyp, uint8_t u8BcastEn);

@brief
            Synchronous API to set the power-save mode of the WINC.

@details
            This is one of the two synchronous power-save setting functions that allow the host MCU application
            to tweak the system power consumption. Such tweaking can be done through one of two ways:
            - 1) Changing the power save mode, to one of the allowed power save modes (see @ref tenuPowerSaveModes). This is done by setting the first parameter.
            - 2) Configuring DTIM monitoring: Configuring beacon monitoring parameters by enabling or disabling the reception of broadcast/multicast data.
            This is done by setting the second parameter.

@param[in]  u8PsTyp
            Desired power saving mode. Supported types are enumerated in @ref tenuPowerSaveModes.

@param[in]  u8BcastEn
            Broadcast reception enable flag.
            If set to 1, the WINC will wake for each DTIM beacon to ensure broadcast traffic can be received.
            If set to 0, the WINC will not wakeup at the DTIM beacon, ignoring broadcast traffic, instead it will
            wake every N beacon periods, as per the negotiated Listen Interval.

@warning
            The function called once after initialization.

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.

@see
            tenuPowerSaveModes
            m2m_wifi_get_sleep_mode
            m2m_wifi_set_lsn_int
*/
int8_t m2m_wifi_set_sleep_mode(uint8_t u8PsTyp, uint8_t u8BcastEn);

/*!
@ingroup    WLANPS
@fn         int8_t m2m_wifi_request_sleep(uint32_t u32SlpReqTime);

@brief
            Asynchronous API to place the WINC into sleep mode for a specified period of time.

@details
            Power-save sleep request function, which requests the WINC device to sleep in the currently configured
            power save mode, as set using @ref m2m_wifi_set_sleep_mode, for a specific time as defined by the parameter
            u32SlpReqTime (measured in milliseconds).
            This function should be used when the WINC is running in @ref M2M_PS_MANUAL power save mode only.
            A wake up request is automatically performed by the WINC device when any host driver API function, e.g. WiFi or socket operation is called.

@param[in]  u32SlpReqTime
            Request sleep time in ms.<br>
            The best recommended sleep duration is left to be determined by the application.
            Taking into account that if the application sends notifications very rarely,
            sleeping for a long time can be a power-efficient decision.
            In contrast, applications that are sensitive for long periods of absence can experience
            performance degradation in the connection if long sleeping times are used.

@return
            The function returns @ref M2M_SUCCESS for successful operation and a negative value otherwise.

@warning
            The function should be called in @ref M2M_PS_MANUAL power save mode only. As enumerated in @ref tenuPowerSaveModes.
            It's also important to note that during the sleeping time while in the M2M_PS_MANUAL mode, AP beacon monitoring is
            bypassed and the WiFi-connection may drop if the sleep period is elongated.

@see
            tenuPowerSaveModes
            m2m_wifi_set_sleep_mode
*/
int8_t m2m_wifi_request_sleep(uint32_t u32SlpReqTime);

/*!
@ingroup    WLANPS
@fn         uint8_t m2m_wifi_get_sleep_mode(void);

@brief
            Synchronous API to retrieve the current power save mode of the WINC.

@return
            The current operating power saving mode. The value will be one of those from the enumerated type @ref tenuPowerSaveModes.

@see
            tenuPowerSaveModes
            m2m_wifi_set_sleep_mode
*/
uint8_t m2m_wifi_get_sleep_mode(void);

/*!
@ingroup    WLANPS
@fn         int8_t m2m_wifi_req_client_ctrl(uint8_t u8Cmd);

@brief
            Asynchronous command sending function to the PS Client.

@details
            Asynchronous command sending function to the PS Client (a WINC board running the ps_firmware)
            if the PS client sends any command, it will be received through the @ref M2M_WIFI_RESP_CLIENT_INFO event.

@param[in]  u8Cmd
            Control command sent from PS Server to PS Client (command values defined by the application)

@pre
            @ref m2m_wifi_req_server_init should be called first.

@warning
            This mode is not supported in the current release.

@see
            m2m_wifi_req_server_init
            M2M_WIFI_RESP_CLIENT_INFO

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_wifi_req_client_ctrl(uint8_t u8Cmd);

/*!
@ingroup    WLANPS
@fn         int8_t m2m_wifi_req_server_init(uint8_t u8Ch);

@brief
            Synchronous function to initialize the PS Server.

@details
            The WINC supports non secure communication with another WINC,
            (SERVER/CLIENT) through one byte command (probe request and probe response) without any connection setup.
            The server mode can't be used with any other modes (STA/AP)

@param[in]  u8Ch
            Server listening channel

@see
            m2m_wifi_req_client_ctrl

@warning
            This mode is not supported in the current release.

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_wifi_req_server_init(uint8_t u8Ch);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_set_device_name(uint8_t *pu8DeviceName, uint8_t u8DeviceNameLength);

@brief
            Asynchronous API to set the WiFi Direct "Device Name" of the WINC.

@details
            Sets the WINC device name. The name string is used as a device name in DHCP
            hostname (option 12).
            If a device is not set through this function a default name is assigned.
            The default name is WINC-XX-YY, where XX and YY are the last 2 octets of the OTP
            MAC address. If OTP (eFuse) is programmed, then the default name is WINC-00-00.

@warning
            The function called once after initialization.<br>
            Used for DHCP client hostname option (12).<br>
            Device name shall contain only characters allowed in valid internet host name as
            defined in RFC 952 and 1123.

@param[in]  pu8DeviceName
            Buffer holding the device name. Device name is a null terminated C string.

@param[in]  u8DeviceNameLength
            Length of the device name. Should not exceed the maximum device name's
            length @ref M2M_DEVICE_NAME_MAX (including null character).

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.
*/
int8_t m2m_wifi_set_device_name(uint8_t *pu8DeviceName, uint8_t u8DeviceNameLength);

/*!
@ingroup    WLANTIME
@fn         int8_t m2m_wifi_configure_sntp(uint8_t *pu8NTPServerName, uint8_t u8NTPServerNameLength, tenuSNTPUseDHCP enuUseDHCP);

@brief
            Configures what NTP server the SNTP client should use.

@details
            Configures what NTP server the SNTP client should use. Only 1 server name can be provided, if the configured server name begins with an asterisk then it will be treated as a server pool.
            The SNTP client can also use the NTP server provided by the DHCP server through option 42.
            By default the NTP server provided by DHCP will be tried first, then the built-in default NTP server (time.nist.gov) will be used.

@param[in]  pu8NTPServerName
            Buffer holding the NTP server name. If the first character is an asterisk (*) then it will be treated as a server pool, where the asterisk will
            be replaced with an incrementing value from 0 to 3 each time a server fails (example: *.pool.ntp.org).

@param[in]  u8NTPServerNameLength
            Length of the NTP server name. Should not exceed the maximum NTP server name length of @ref M2M_NTP_MAX_SERVER_NAME_LENGTH.

@param[in]  enuUseDHCP
            Explicity tell the WINC if it should use the NTP server provided by the DHCP server or not.

@warning
            SNTP should be configured before the connection takes place. If SNTP is configured after the device connects to a
            network, the new configuration can take a minimum of 24h to be applied. However, it can take even longer since it is
            triggered by DHCP renewal.
            Currently there is also a known issue in which if the WINC obtains the NTP server from DHCP and then connects to a
            different network, it will still use the NTP from the previous network.
            Configuring a server name will overwrite the built-in default server until next reboot.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.
*/
int8_t m2m_wifi_configure_sntp(uint8_t *pu8NTPServerName, uint8_t u8NTPServerNameLength, tenuSNTPUseDHCP enuUseDHCP);

/*!
@ingroup    WLANPS
@fn         int8_t m2m_wifi_set_lsn_int(tstrM2mLsnInt *pstrM2mLsnInt);

@brief
            Asynchronous API to set WiFi listen interval for power save operation.

@details
            This is one of the two synchronous power-save setting functions that
            allow the host MCU application to tweak the system power consumption. Such tweaking can be done by modifying the
            WiFi listen interval. The listen interval is how many beacon periods the station can sleep before it wakes up to receive data buffered in the AP.
            It is represented in units of AP beacon periods(100ms).

@warning
            The function should be called once after initialization.

@param[in]  pstrM2mLsnInt
            Structure holding the listen interval configuration.

@pre
            The function @ref m2m_wifi_set_sleep_mode shall be called first, to set the power saving mode required.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            tstrM2mLsnInt
            m2m_wifi_set_sleep_mode
*/
int8_t m2m_wifi_set_lsn_int(tstrM2mLsnInt *pstrM2mLsnInt);

/*!
@ingroup    WLANETH
@fn         int8_t m2m_wifi_send_ethernet_pkt(uint8_t *pu8Packet, uint16_t u16PacketSize);

@brief
            Asynchronous API to queue an Ethernet packet for transmission by the WINC.

@details
            Transmit a packet directly in ETHERNET/bypass mode where the TCP/IP stack is disabled
            and the implementation of this packet is left to the application developer.
            The Ethernet packet composition is left to the application developer.

@note
            Packets are the user's responsibility.

@warning
            This function available in ETHERNET/Bypass mode ONLY. Make sure that application defines ETH_MODE.

@param[in]  pu8Packet
            Pointer to a buffer holding the whole Ethernet frame.

@param[in]  u16PacketSize
            The size of the whole packet in bytes.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_enable_mac_mcast
            m2m_wifi_set_receive_buffer
*/
int8_t m2m_wifi_send_ethernet_pkt(uint8_t *pu8Packet, uint16_t u16PacketSize);

/*!
@ingroup    WLANTIME
@fn         int8_t m2m_wifi_enable_sntp(uint8_t u8Enable);

@brief
            Asynchronous API to enable or disable the native Simple Network Time Protocol(SNTP) client running on the WINC.

@details
            The SNTP client is enabled by default during chip initialization. This function can be used to
            disable or subsequently re-enable the service.

            The service is capable of synchronizing the WINC system clock to the UTC time from a well-known
            (and trusted) time server, for example "time.nist.gov". By default the SNTP client will update the
            system time once every 24 hours. The ability to track the time accurately is important for various
            applications such as checking expiry of X509 certificates during TLS (Transport Layer Security)
            session establishment.

            It is highly recommended to leave SNTP enabled if there is no alternative source of timing
            information. For systems including an RTC device, SNTP may not be needed and the WINCs time
            may be set using the @ref m2m_wifi_set_system_time function.

@param[in]  u8Enable
            Enables or disables the SNTP service
            '0' :disable SNTP
            '1' :enable SNTP

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_set_system_time
*/
int8_t m2m_wifi_enable_sntp(uint8_t u8Enable);

/*!
@ingroup    WLANTIME
@fn         int8_t m2m_wifi_set_system_time(uint32_t u32UTCSeconds);

@brief
            Asynchronous function for setting the system time within the WINC.

@details
            Function for setting the system time in time/date format (uint32_t).
            The @ref tstrSystemTime structure can be used as a reference to the time values that
            should be set and pass its value as uint32_t.

@param[in]  u32UTCSeconds
            Seconds elapsed since January 1, 1900 (NTP Timestamp).

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_enable_sntp
            tstrSystemTime

@note
            If there is an RTC on the host MCU, the SNTP may be disabled and the host may set the system
            time within the firmware using the API @ref m2m_wifi_set_system_time.
*/
int8_t m2m_wifi_set_system_time(uint32_t u32UTCSeconds);

/*!
@ingroup    WLANTIME
@fn         int8_t m2m_wifi_get_system_time(void);

@brief
            Asynchronous API to obtain the system time in use by the WINC.

@details
            This function will request the WINC to report its current system time to the application. The
            information will arrive at the application via the @ref tpfAppWifiCb and event @ref M2M_WIFI_RESP_GET_SYS_TIME.
            Response time retrieved is parsed into the members defined in the structure @ref tstrSystemTime.

@note
            - A WiFi notification callback of type @ref tpfAppWifiCb MUST be implemented and registered
            during initialization. Registration of the callback is done via the @ref m2m_wifi_init API.
            - The event @ref M2M_WIFI_RESP_GET_SYS_TIME must be handled in the callback.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_enable_sntp
            tstrSystemTime
*/
int8_t m2m_wifi_get_system_time(void);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_set_cust_InfoElement(uint8_t *pau8M2mCustInfoElement);

@brief
            Asynchronous API to add or remove a user-defined Information Element.

@details
            This function allows the application to provide a custom Information Element to the
            WINC that will be included in all beacon and probe response frames, while in Access Point mode.
            If it is required to delete these IEs, fill the buffer with zeros.

@param[in]  pau8M2mCustInfoElement
            Pointer to Buffer containing the IE to be used. It is the application developer's
            responsibility to ensure on the correctness of the information element's ordering
            passed in.
            If the pointer is null, this removes any current custom IE. If non-null, the pointer
            must reference data in the following format:

@verbatim
            --------------- ---------- ---------- ------------------- -------- -------- ----------- -----------------------
            | Byte[0]       | Byte[1]  | Byte[2]  | Byte[3:length1+2] | ..... | Byte[n] | Byte[n+1] | Byte[n+2:lengthx+2] |
            |---------------|----------|----------|-------------------|-------- --------|-----------|---------------------|
            | #of all Bytes | IE1 ID   | Length1  | Data1(Hex Coded)  | ..... | IEx ID  | Lengthx   | Datax(Hex Coded)    |
            --------------- ---------- ---------- ------------------- -------- -------- ----------- -----------------------
@endverbatim

@warning
            Size of All elements combined must not exceed 255 byte.
            Used in Access Point Mode.

@note
            IEs Format will follow the above format.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_enable_sntp

@section    WIFIExample11 Example
            The example demonstrates how the information elements are set using this function.

@include    m2m_wifi_set_cust_InfoElement.c
@example    m2m_wifi_set_cust_InfoElement.c
*/
int8_t m2m_wifi_set_cust_InfoElement(uint8_t *pau8M2mCustInfoElement);

/*!
@ingroup    WLANPS
@fn         int8_t m2m_wifi_set_power_profile(uint8_t u8PwrMode);

@brief
            Change the power profile mode.

@param[in]  u8PwrMode
            Change the WINC power profile to different mode based on the enumeration @ref tenuM2mPwrMode.

@warning
            May only be called after initialization, before any connection request, and may not be used to change
            the power mode thereafter.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@see
            tenuM2mPwrMode
            m2m_wifi_init
*/
int8_t m2m_wifi_set_power_profile(uint8_t u8PwrMode);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_set_tx_power(uint8_t u8TxPwrLevel);

@brief
            Set the TX power tenuM2mTxPwrLevel.

@param[in]  u8TxPwrLevel
            Change the TX power based on the enumeration @ref tenuM2mTxPwrLevel.

@pre
            Must be called after the initialization and before any connection request and can't be changed in runtime.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@see
            tenuM2mTxPwrLevel
            m2m_wifi_init
*/
int8_t m2m_wifi_set_tx_power(uint8_t u8TxPwrLevel);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_set_gain_table_idx(uint8_t u8GainTableIdx);

@brief
            Set the Gain table index corresponding to specific WiFi region.

@param[in]  u8GainTableIdx
            Change the gain table index based on the WiFi region it is suppose to be used.

@pre
            Must be called after the initialization and before any connection request.
            The corresponding gain tables must be present in the flash.

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

@see
            m2m_wifi_init
*/
int8_t m2m_wifi_set_gain_table_idx(uint8_t u8GainTableIdx);

/*!
@ingroup    WLANLOG
@fn         int8_t m2m_wifi_enable_firmware_logs(uint8_t u8Enable);

@brief
            Enable or Disable logs in run time.

@details
            Enable or Disable logs in run time (Disable Firmware logs will enhance the firmware start-up time and performance).

@param[in]  u8Enable
            Set 1 to enable the logs, 0 for disable.

@pre
            Must be called after initialization through the following function @ref m2m_wifi_init.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@see
            __DISABLE_FIRMWARE_LOGS__ (build option to disable logs from initializations)
            m2m_wifi_init
*/
int8_t m2m_wifi_enable_firmware_logs(uint8_t u8Enable);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_set_battery_voltage(uint16_t u16BattVoltx100);

@brief
            Set the battery voltage to update the firmware calculations.

@pre
            Must be called after initialization through the following function @ref m2m_wifi_init.

@param[in]  u16BattVoltx100
            Battery voltage as double (multiplied by 100).

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@see
            m2m_wifi_init
*/
int8_t m2m_wifi_set_battery_voltage(uint16_t u16BattVoltx100);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_set_gains(tstrM2mWifiGainsParams *pstrM2mGain);

@brief
            Set the chip PPA gain for 11b/11gn.

@param[in]  pstrM2mGain
            @ref tstrM2mWifiGainsParams contain gain parameters as implemented in rf document.

@pre
            Must be called after initialization through the following function @ref m2m_wifi_init.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_init
*/
int8_t m2m_wifi_set_gains(tstrM2mWifiGainsParams *pstrM2mGain);

#ifdef ETH_MODE
/*!
@ingroup    WLANETH
@fn         int8_t m2m_wifi_enable_mac_mcast(uint8_t *pu8MulticastMacAddress, uint8_t u8AddRemove);

@brief
            Asynchronous API to add or remove MAC addresses to the multicast filter.

@details
            This function will configure the WINC to receive/ignore multicast packets from certain
            MAC address groups when operating in bypass mode.
            This function requests the given MAC addresses to be added/removed from the multicast filter.

@param[in]  pu8MulticastMacAddress
            Pointer to MAC address

@param[in]  u8AddRemove
            A flag to add or remove the MAC ADDRESS, based on the following values:
            -  0 : remove MAC address
            -  1 : add MAC address

@note
            Maximum number of MAC addresses that could be added is 8.

@warning
            This function is available in ETHERNET/bypass mode ONLY.
            Make sure that the application defines ETH_MODE.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_set_receive_buffer
            m2m_wifi_send_ethernet_pkt
*/
int8_t m2m_wifi_enable_mac_mcast(uint8_t *pu8MulticastMacAddress, uint8_t u8AddRemove);

/*!
@ingroup    WLANETH
@fn         int8_t m2m_wifi_set_receive_buffer(void *pvBuffer, uint16_t u16BufferLen);

@brief
            Synchronous function for setting or modifying the receiver buffer's length.

@details
            Synchronous function for setting or modifying the receiver buffer's length.
            In the ETHERNET/bypass mode the application should define a callback of type
            @ref tpfAppEthCb, through which the application handles the received
            Ethernet frames. It is through this callback function that the user can
            dynamically modify the length of the currently used receiver buffer.

@param[in]  pvBuffer
            Pointer to Buffer to receive data.
            NULL pointer causes a negative error @ref M2M_ERR_FAIL.

@param[in]  u16BufferLen
            Length of data to be received.  Maximum length of data should not exceed the size defined by TCP/IP
            defined as @ref SOCKET_BUFFER_MAX_LENGTH

@warning
            This function is available in the Ethernet/bypass mode ONLY. Make sure that the application defines ETH_MODE.

@return
            The function returns @ref M2M_SUCCESS if the command has been successfully queued to the WINC and a negative value otherwise.

@see
            m2m_wifi_enable_mac_mcast
            m2m_wifi_send_ethernet_pkt
*/
int8_t m2m_wifi_set_receive_buffer(void *pvBuffer, uint16_t u16BufferLen);
#endif /* ETH_MODE */

/*!
@ingroup    WLANCRYPTO
@fn         int8_t m2m_wifi_prng_get_random_bytes(uint8_t *pu8PrngBuff, uint16_t u16PrngSize);

@brief
            Asynchronous function for retrieving from the firmware a pseudo-random set of bytes.

@details
            Asynchronous function for retrieving from the firmware a pseudo-random set of bytes as specified in the size passed in as a parameter.
            The registered WiFi callback function retrieves the random bytes through the response @ref M2M_WIFI_RESP_GET_PRNG

@param[in]  pu8PrngBuff
            Pointer to a buffer to receive data.

@param[in]  u16PrngSize
            Requested size in bytes. Maximum 1580.

@warning
            Size greater than the maximum specified (1580) causes a negative error @ref M2M_ERR_FAIL.

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.
*/
int8_t m2m_wifi_prng_get_random_bytes(uint8_t *pu8PrngBuff, uint16_t u16PrngSize);

/*!
@ingroup    WLANCONF
@fn         int8_t m2m_wifi_conf_auto_rate(tstrConfAutoRate *pstrConfAutoRate);

@brief
            Configures WLAN automatic TX rate adaptation algorithm.

@details
            Allow the host MCU app to configure auto TX rate selection algorithm. The application can use this
            API to tweak the algorithm performance. Moreover, it allows the application to force a specific WLAN
            PHY rate for transmitted data packets to favor range vs. throughput needs.

@param[in]  pstrConfAutoRate
            The Auto rate configuration parameters as listed in tstrConfAutoRate.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@see
            tstrConfAutoRate
*/
int8_t m2m_wifi_conf_auto_rate(tstrConfAutoRate *pstrConfAutoRate);

/*!
@ingroup    WLANROAMING
@fn         int8_t m2m_wifi_enable_roaming(uint8_t u8EnableDhcp);

@brief
            Enable WiFi STA roaming.

@details
            m2m_wifi_enable_roaming enables the firmware to trigger the roaming algorithm/steps on link loss with the current AP.
            If roaming is successful,
            the @ref M2M_WIFI_RESP_CON_STATE_CHANGED message with state as @ref M2M_WIFI_ROAMED is sent to the host.
            Additionally a @ref M2M_WIFI_REQ_DHCP_CONF message with new DHCP lease details is sent to host (only if u8EnableDhcp=1).
            If roaming is unsuccessful,
            @ref M2M_WIFI_RESP_CON_STATE_CHANGED message with state as @ref M2M_WIFI_DISCONNECTED is sent to the host.

@param[in]  u8EnableDhcp
            0 : Disables DHCP client execution after roaming to new AP
            1 : Enables DHCP client execution after roaming to new AP

@pre
            Must be called after the initialization.
            The roaming algorithm/procedure is internal to the firmware.

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

@see
            m2m_wifi_init
*/
int8_t m2m_wifi_enable_roaming(uint8_t u8EnableDhcp);

/*!
@ingroup    WLANROAMING
@fn         int8_t m2m_wifi_disable_roaming(void);

@brief
            Disable WiFi STA roaming.

@pre
            Must be called after the initialization.

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

@see
            m2m_wifi_init
*/
int8_t m2m_wifi_disable_roaming(void);

/*!
@ingroup    WLANINIT
@fn         uint8_t m2m_wifi_get_state(void);

@brief
            Get the WiFi state.

@return
            The function returns the current WiFi state (see @ref tenuWifiState for the possible states).

@note
            Check the WINC state. See @ref tenuWifiState for possible states.<br>
            @ref WIFI_STATE_INIT state represents WINC initialized but not started, this is a suitable state
            for safe flash access.

@sa
            m2m_wifi_init
            m2m_wifi_download_mode
*/
uint8_t m2m_wifi_get_state(void);

/*!
@ingroup    VERSIONAPI
@fn         int8_t m2m_wifi_get_firmware_version(tstrM2mRev *pstrRev)

@brief
            Synchronous API to obtain the firmware version currently running on the WINC.

@details
            Get the Firmware version info from the active partition, as defined in the structure @ref tstrM2mRev.

@param[out] pstrRev
            Pointer to a variable of type @ref tstrM2mRev, which contains the firmware version parameters.

@return
            The function returns @ref M2M_SUCCESS for success and a negative value otherwise.

@pre
            Must be called after initialization through the following function @ref m2m_wifi_init.

@sa
            m2m_wifi_init
*/
int8_t m2m_wifi_get_firmware_version(tstrM2mRev *pstrRev);

/*!
@ingroup    WLANPS
@fn         int8_t m2m_wifi_enable_XO_during_sleep(uint8_t bXOSleepEnable);

@brief
            Specifies whether the crystal oscillator is left on or off during deep sleep (defaults to off).

@details
            m2m_wifi_enable_XO_during_sleep controls the state of the crystal oscillator during deep sleep.

@param[in]  bXOSleepEnable
            0 : The XO will be switched off during deep sleep
            1 : The XO will be kept on during deep sleep

@return
            The function returns @ref M2M_SUCCESS for successful operations and a negative value otherwise.

*/
int8_t m2m_wifi_enable_XO_during_sleep(uint8_t bXOSleepEnable);

#ifdef __cplusplus
}
#endif
#endif /* __M2M_WIFI_H__ */

