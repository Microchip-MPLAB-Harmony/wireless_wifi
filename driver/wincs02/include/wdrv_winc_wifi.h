/*******************************************************************************
  WINC Wireless Driver Wifi Configuration Header File

  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_winc_wifi.h

  Summary:
    WINC wireless driver Wifi Configuration header file.

  Description:
    This interface provides functionality required for configuring the Wifi,
    this includes powersave mode, regulatory domain and coex settings.
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

#ifndef WDRV_WINC_WIFI_H
#define WDRV_WINC_WIFI_H

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

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Wifi Configuration Defines
// *****************************************************************************
// *****************************************************************************

/* Maximum length of regulatory domain name. */
#define WDRV_WINC_REGDOMAIN_MAX_NAME_LEN    WINC_CFG_PARAM_SZ_WIFI_REGDOMAIN_SELECTED

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Wifi Configuration Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  Powersave Mode

  Summary:
    Possible powersave modes.

  Description:
    Enumeration describing available powersave modes.

  Remarks:
    None.
*/

typedef enum
{
    /* Run mode : No power-save. Both TX and RX are active */
    WDRV_WINC_POWERSAVE_RUN_MODE = 0,

    /* WSM mode : Wireless sleep mode. TX and RX are stopped, clocks will be running.
       STA will be in power-save mode keeping the connection active with AP. */
    WDRV_WINC_POWERSAVE_WSM_MODE = 1
} WDRV_WINC_POWERSAVE_MODE;

// *****************************************************************************
/*  Regulatory Domain Selection

  Summary:
    Defines possible selections of regulatory domains.

  Description:
    Specifies a grouping of regulatory domains.

  Remarks:
    None.
*/

typedef enum
{
    /* No regulatory domain selected. */
    WDRV_WINC_REGDOMAIN_SELECT_NONE,

    /* Current regulatory domain selected. */
    WDRV_WINC_REGDOMAIN_SELECT_CURRENT,

    /* All regulatory domains selected. */
    WDRV_WINC_REGDOMAIN_SELECT_ALL
} WDRV_WINC_REGDOMAIN_SELECT;

// *****************************************************************************
/*  Powersave Information

  Summary:
    Defines a powersave information structure.

  Description:
    Specified the powersave mode.

  Remarks:
    None.
*/

typedef struct
{
    /* Powersave mode. */
   WDRV_WINC_POWERSAVE_MODE psMode;
} WDRV_WINC_POWERSAVE_INFO;

// *****************************************************************************
/*  Regulatory Domain Information

  Summary:
    Defines a regulatory domain's name.

  Description:
    Specifies the country code length, country code associated with the
    regulatory domain.

  Remarks:
    None.
*/

typedef struct
{
    /* Length of regulatory domain. */
    uint8_t regDomainLen;

    /* Regulatory domain name. */
    uint8_t regDomain[WDRV_WINC_REGDOMAIN_MAX_NAME_LEN];

    /* Regulatory domain channel mask. */
    WDRV_WINC_CHANNEL24_MASK channelMask;
} WDRV_WINC_REGDOMAIN_INFO;

// *****************************************************************************
/*  Coexistence Arbiter Configuration

  Summary:
    Structure containing coexistence arbiter configuration.

  Description:
    Contains arbiter configuration for interface mode, priority and antenna mode.

  Remarks:
    None.
*/

typedef struct
{
    /* Use 2-wire interface (BT_Prio, WLAN_Act) or 3-wire interface (BT_Act, BT_Prio, WLAN_Act). */
    bool use2Wire;

    /* BT/Wi-Fi coexistence arbiter WLAN Rx priority over BT Low Priority. */
    bool wlanRxHigherThanBt;

    /* BT/Wi-Fi coexistence arbiter WLAN Tx priority over BT Low Priority. */
    bool wlanTxHigherThanBt;

    /* BT/Wi-Fi coexistence arbiter antenna mode (shared vs dedicated). */
    bool sharedAntenna;
} WDRV_WINC_COEX_CFG;

// *****************************************************************************
/*  MAC Options

  Summary:
    Structure containing MAC options.

  Description:
    Contains MAC options.

  Remarks:
    None.
*/

typedef struct
{
    /* Wi-Fi A-MPDU Tx Enable. */
    bool useTxAmpdu;
} WDRV_WINC_MAC_OPTIONS;

// *****************************************************************************
/*  WiFi Connection Configuration

  Summary:
    Structure containing WiFi connection configuration.

  Description:
    Contains WiFi configuration for use during connection.

  Remarks:
    None.
*/

typedef struct
{
    /* Network interface to assign. (see WDRV_WINC_NETIF_IDX) */
    uint8_t ifIdx;

    /* Layer 2 (no IP services) configuration only. */
    bool l2Only;

    /* STA specific configuration. */
    struct
    {
        /* Connection timeout in milliseconds. */
        uint32_t connTimeoutMs;

        /* Roaming configuration option. (see WDRV_WINC_BSS_ROAMING_CFG) */
        uint8_t roaming;
    } sta;

    /* AP specific configuration. */
    struct
    {
        /* Rekey interval in seconds. */
        uint32_t rekeyInterval;

        /* Flag indicating if SSID is cloaked. */
        bool cloaked;
    } ap;
} WDRV_WINC_CONN_CFG;

// *****************************************************************************
/* Powersave Information Callback

  Function:
    void (*WDRV_WINC_POWERSAVE_CALLBACK)
    (
        DRV_HANDLE handle,
        const WDRV_WINC_POWERSAVE_INFO *const pPowersaveInfo
    )

  Summary:
    Pointer to a powersave callback.

  Description:
    This defines a function pointer to a powersave information callback.

  Parameters:
    handle         - Client handle obtained by a call to WDRV_WINC_Open.
    pPowersaveInfo - Pointer to a powersave information structure.

  Returns:
    None.

  Remarks:
    None.

*/

typedef void (*WDRV_WINC_POWERSAVE_CALLBACK)
(
    DRV_HANDLE handle,
    const WDRV_WINC_POWERSAVE_INFO *const pPowersaveInfo
);

// *****************************************************************************
/* Regulatory Domain Information Callback

 Function:
    void (*WDRV_WINC_REGDOMAIN_CALLBACK)
    (
        DRV_HANDLE handle,
        uint8_t index,
        uint8_t ofTotal,
        bool isCurrent,
        const WDRV_WINC_REGDOMAIN_INFO *const pRegDomInfo
    )

  Summary:
    Pointer to a regulatory domain callback.

  Description:
    This defines a function pointer to a regulatory domain callback which will
      receive information about a single domain.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    index       - Index with a grouping of regulatory domains.
    ofTotal     - Total number of domain within a grouping.
    isCurrent   - Is this domain the currently active one.
    pRegDomInfo - Pointer to WDRV_WINC_REGDOMAIN_INFO of regulatory domain.

  Returns:
    None.

  Remarks:
    When called in response to WDRV_WINC_WifiRegDomainGet this callback
      will receive a grouping of 'ofTotal' domain records. An empty grouping is
      indicated by pRegDomain being blank (empty or NULL) and ofTotal being zero.

    When called in response to WDRV_WINC_WifiRegDomainSet this callback
      will receive either a single group of the request domain as confirmation of
      application or ofTotal will be zero.

*/

typedef void (*WDRV_WINC_REGDOMAIN_CALLBACK)
(
    DRV_HANDLE handle,
    uint8_t index,
    uint8_t ofTotal,
    bool isCurrent,
    const WDRV_WINC_REGDOMAIN_INFO *const pRegDomInfo
);

// *****************************************************************************
// *****************************************************************************
// Section: WINC Driver Wifi Configuration Routines
// *****************************************************************************
// *****************************************************************************

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiPowerSaveModeSet
    (
        DRV_HANDLE handle,
        WDRV_WINC_POWERSAVE_MODE psMode
    )

  Summary:
    Set the mode of the powersave.

  Description:
    Configures the powersave mode to one of the available powersave modes.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle - Client handle obtained by a call to WDRV_WINC_Open.
    psMode - The mode to change to. See WDRV_WINC_POWERSAVE_MODE.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiPowerSaveModeSet
(
    DRV_HANDLE handle,
    WDRV_WINC_POWERSAVE_MODE psMode
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiPowerSaveModeGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_POWERSAVE_CALLBACK pfPowersaveEventCB
    )

  Summary:
    Get the current powersave mode.

  Description:
    Retrieves the currently applied powersave mode.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle             - Client handle obtained by a call to WDRV_WINC_Open.
    pfPowersaveEventCB - Pointer to the callback to receive the powersave mode.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiPowerSaveModeGet
(
    DRV_HANDLE handle,
    WDRV_WINC_POWERSAVE_CALLBACK pfPowersaveEventCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiRegDomainSet
    (
        DRV_HANDLE handle,
        const char* pRegDomainName,
        uint8_t regDomainNameLen,
        WDRV_WINC_WIFIC_EVENT_HANDLER pfWifiCEventCB
    )

  Summary:
    Set the active regulatory domain.

  Description:
    Configures the regulatory domain to one available in NVM.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle             - Client handle obtained by a call to WDRV_WINC_Open.
    pRegDomainName     - A pointer to the regulatory domain to be selected.
    regDomainNameLen   - The length of the regulatory domain.
    pfRegDomainEventCB - Callback to indicate the result of the request.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiRegDomainSet
(
    DRV_HANDLE handle,
    const uint8_t* pRegDomainName,
    uint8_t regDomainNameLen,
    WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiRegDomainGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_REGDOMAIN_SELECT regDomainSel,
        WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB
    )

  Summary:
    Get the active or all of the available regulatory domain(s).

  Description:
    Read the active or all available regulatory domain(s).

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle             - Client handle obtained by a call to WDRV_WINC_Open.
    regDomainSel       - Option to read all or current regulatory domain(s).
    pfRegDomainEventCB - Callback to indicate the result of the request.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiRegDomainGet
(
    DRV_HANDLE handle,
    WDRV_WINC_REGDOMAIN_SELECT regDomainSel,
    WDRV_WINC_REGDOMAIN_CALLBACK pfRegDomainEventCB
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiCoexEnableSet
    (
        DRV_HANDLE handle,
        bool enableCoexArbiter
    )

  Summary:
    Set the enable state of the coexistence arbiter.

  Description:
    Enables or disables the coexistence arbiter.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle            - Client handle obtained by a call to WDRV_WINC_Open.
    enableCoexArbiter - Flag indicating if the coexistence arbiter should be enabled.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiCoexEnableSet
(
    DRV_HANDLE handle,
    bool enableCoexArbiter
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiCoexConfSet
    (
        DRV_HANDLE handle,
        const WDRV_WINC_COEX_CFG *const pCoexCfg
    )

  Summary:
    Set the coexistence arbiter configuration.

  Description:
    Sets the interface mode, priority and antenna mode of the coexistence arbiter.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle   - Client handle obtained by a call to WDRV_WINC_Open.
    pCoexCfg - Pointer to a structure containing coexistence arbiter settings.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiCoexConfSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_COEX_CFG *const pCoexCfg
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiCoexConfGet
    (
        DRV_HANDLE handle,
        WDRV_WINC_COEX_CFG *pCoexCfg,
        bool *pIsEnabled
    )

  Summary:
    Returns the coexistence arbiter configuration.

  Description:
    Returns the current coexistence arbiter configuration related to interface
    mode, priority and antenna mode as well as if the arbiter is enabled or not.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle     - Client handle obtained by a call to WDRV_WINC_Open.
    pCoexCfg   - Pointer to structure to receive the coexistence arbiter settings.
    pIsEnabled - Pointer to a flag to receive the enabled state or the arbiter.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.
    WDRV_WINC_STATUS_RETRY_REQUEST - The request should be retried.

  Remarks:
    pCoexCfg can be NULL if the arbiter settings are not required.

    pIsEnabled can be NULL if the arbiter enabled state is not required.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiCoexConfGet
(
    DRV_HANDLE handle,
    WDRV_WINC_COEX_CFG *pCoexCfg,
    bool *pIsEnabled
);

//*******************************************************************************
/*
  Function:
    WDRV_WINC_STATUS WDRV_WINC_WifiMACOptionsSet
    (
        DRV_HANDLE handle,
        const WDRV_WINC_MAC_OPTIONS *const pMACOptions
    )

  Summary:
    Sets the MAC options.

  Description:
    Configures WiFi MAC options.

  Precondition:
    WDRV_WINC_Initialize must have been called.
    WDRV_WINC_Open must have been called to obtain a valid handle.

  Parameters:
    handle      - Client handle obtained by a call to WDRV_WINC_Open.
    pMACOptions - Pointer to new MAC options to configure.

  Returns:
    WDRV_WINC_STATUS_OK            - The request has been accepted.
    WDRV_WINC_STATUS_NOT_OPEN      - The driver instance is not open.
    WDRV_WINC_STATUS_INVALID_ARG   - The parameters were incorrect.
    WDRV_WINC_STATUS_REQUEST_ERROR - The request to the WINC was rejected.

  Remarks:
    None.

*/

WDRV_WINC_STATUS WDRV_WINC_WifiMACOptionsSet
(
    DRV_HANDLE handle,
    const WDRV_WINC_MAC_OPTIONS *const pMACOptions
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* WDRV_WINC_WIFI_H */
