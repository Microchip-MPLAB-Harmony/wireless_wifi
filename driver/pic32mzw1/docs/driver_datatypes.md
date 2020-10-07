---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Wi-Fi Driver Data Types
has_toc: false
nav_order: 2
---

# Wi-Fi Driver Data Types

```
WDRV_PIC32MZW_ASSOC_HANDLE
```

### Description:
An ‘opaque’ type (implemented as a pointer). Application code should never set this directly and should instead only store values returned from the Wi-Fi driver for later use in subsequent calls (to the Wi-Fi driver).

<br />
<br />

```
WDRV_PIC32MZW_AUTH_TYPE
```

### Description:
Defines various authentication types for use in connecting-to or hosting Wi-Fi networks.

### Definitions:

Please note: the following definitions are all prefixed with ```‘WDRV_PIC32MZW_AUTH_TYPE_’:```


|	Data Type		 | 						Description						|
|-------------------------|--------------------------------------------------------------|
| DEFAULT |	Recommended for general use when connecting to a network – will automatically use the most secure type available|
| OPEN	| Open system (no security) |
| WPAWPA2_PERSONAL | 	WPA2 mixed mode / compatibility mode with pre-shared key
| WPA2_PERSONAL	| WPA2-only |
|WPA2WPA3_PERSONAL	| WPA3 (Simultaneous Authentication of Equals) transition mode accepts either WPA2 and WPA3 security |
| WPA3_PERSONAL	| WPA3-only |
|

<br />
<br />

```
WDRV_PIC32MZW_AUTH_MOD_MASK
```

### Description:
Defines modifiers (optional settings) for a configured WDRV_PIC32MZW_AUTH_TYPE. Not all modifiers are relevant to all authentication types.
### Definitions:
Please note: the following definitions are all prefixed with ‘WDRV_PIC32MZW_AUTH_MOD_’:


|	Data Type		 | 						Description						|
|-------------------------|--------------------------------------------------------------|
| NONE | Recommended – no modifiers, default behavior for the configured AUTH_TYPE| 
|MFP_REQ |Management Frame Protection (802.11w) is required. Relevant only if the authentication type is …WPA2_PERSONAL or …WPA2WPA3_PERSONAL.|
|MFP_OFF |Management Frame Protection (802.11w) is disabled. Relevant only if the authentication type is …WPAWPA2_PERSONAL or …WPA2_PERSONAL.|
|SHARED_KEY| Enables shared key authentication, this is generally not recommended and should only be used if the network refuses to accept the default open system authentication. Relevant only if the authentication type is WEP.|
|

<br />
<br />

```
WDRV_PIC32MZW_CHANNEL_ID
```

### Description:
Defines the supported Wi-Fi channels on the PIC322MZW1.

### Definitions:
Please note: the following definitions are all prefixed with ```‘WDRV_PIC32MZW_CID_’```:


|	Definition		 | 		Description					|
|-------------------------|-----------------------------|
|ANY |	Use any valid channel|
|2_4G_CH1	|Use 2.4GHz band, channel 1 (2412 MHz)
|2_4G_CH2	|Use 2.4GHz band, channel 2 (2417 MHz)|
|2_4G_CH3	|Use 2.4GHz band, channel 3 (2422 MHz)|
|2_4G_CH4	|Use 2.4GHz band, channel 4 (2427 MHz)|
|2_4G_CH5	|Use 2.4GHz band, channel 5 (2432 MHz)|
|2_4G_CH6	|Use 2.4GHz band, channel 6 (2437 MHz)|
|2_4G_CH7	|Use 2.4GHz band, channel 7 (2442 MHz)|
|2_4G_CH8	|Use 2.4GHz band, channel 8 (2447 MHz)|
|2_4G_CH9	|Use 2.4GHz band, channel 9 (2452 MHz)|
|2_4G_CH10	|Use 2.4GHz band, channel 10 (2457 MHz)|
|2_4G_CH11	|Use 2.4GHz band, channel 11 (2462 MHz)|
|2_4G_CH12	|Use 2.4GHz band, channel 12 (2467 MHz)|
|2_4G_CH13	|Use 2.4GHz band, channel 13 (2472 MHz)|
|

<br />
<br />

```
WDRV_PIC32MZW_CHANNEL24_MASK
```

### Description:
A bitmap identifying the legal channels for the current regulatory region. This type also contains definitions for specific regulatory regions such as North America, Europe, Asia for convenience when using the regulatory APIs.

### Definitions:
Please note: the following definitions are all prefixed with ‘WDRV_PIC32MZW_CM_2_4G_’:

|	Definition		 | 		Description					|
|-------------------------|-----------------------------|
|CH1|	2.4GHz band, channel 1 (2412 MHz) is valid/enabled|
|:	| : |
|CH13|	2.4GHz band, channel 13 (2472 MHz) is valid/enabled|
|DEFAULT|	Channels 1 through 11 are valid/enabled – typically used with region ‘GEN’|
|NORTH_AMERICA|	Channels 1 through 11 are valid/enabled – typically used with region ‘USA’|
|EUROPE|	Channels 1 through 13 are valid/enabled – typically used with region ‘EMEA’|
|ASIA|	Channels 1 through 14 are valid/enabled – typically used with region ‘JPN’|
|

<br />
<br />

```
WDRV_PIC32MZW_CONN_STATE
```

### Description:
Defines the possible Wi-Fi connection states of the driver.
### Definitions:
Please note: the following definitions are all prefixed with ```‘WDRV_PIC32MZW_CONN_STATE_’```:


|	Definition		 | 		Description					|
|-------------------------|-----------------------------|
|DISCONNECTED| 	Not connected / idle|
|CONNECTING	|In the process of connecting to a network|
|CONNECTED	|Associated (connected) |
|FAILED	|Association attempt failed|

<br />
<br />

```
WDRV_PIC32MZW_POWERSAVE_MODE
```

### Description:
    This enumeration defines the various WiFi power save modes supported by WiFi.

### Definitions
Please note: the following definitions are all prefixed with ```‘WDRV_PIC32MZW_POWERSAVE_’```:

|	Definition		 | 		Description					|
|-------------------------|-----------------------------|
| RUN_MODE | No powersave. Both Tx and Rx are active |
| WSM_MODE | Wireless sleep mode. Tx,Rx is stopped, clocks will be running.	STA will be in powersave mode keeping the connection active with AP |
|    | **Below Modes will be supported in future releases**         |
| WDS_MODE | Wireless Deep sleep mode. Tx,Rx is stopped. clocks will be cutoff.STA will be in powersave mode keeping the connection active with AP|
| WXDS_MODE | ireless Extreme Deep sleep mode. Everything shutsdown. No Tx,Rx. Reinitialize WiFi driver/context to recover. STA will disconnect from AP. Restore the connection with the saved context |
| WOFF_MODE | Wireless OFF mode. Turns off wifi LDO(WLDO). No Tx,Rx Reinitialize WiFi driver/context to recover. STA will disconnect from AP. Restore the connection with the saved context |
|

<br />
<br />

```
WDRV_PIC32MZW_POWERSAVE_CORRELATION
```

### Description:
This enumeration defines the correlation between WiFi and PIC sleep modes.

### Definitions
Please note: the following definitions are all prefixed with ```‘WDRV_PIC32MZW_POWERSAVE_PIC_’```:

|	Definition		 | 		Description					|
|-------------------------|-----------------------------|
|ASYNC_MODE | PIC sleep entry forces WiFi into Sleep, PIC wakeup (non WIFI) can be independent of the WIFI Sleep modes. WiFi sleep entry can be independent of the PIC sleep mode entry, WIFI wakeup to RUN mode will force PIC into RUN mode. ASYNC mode is not applicable for WXDS and WOFF powersave modes.|
| | **NOTE - This below mode will be supported in future releases**|
|SYNC_MODE | PIC sleep entry forces the WiFi into sleep mode and vice-versa. PIC wakeup forces the WiFi sleep exit(Run) and vice-versa|
|

<br />
<br />

```
WDRV_PIC32MZW_POWERSAVE_CONFIG
{
    WDRV_PIC32MZW_POWERSAVE_MODE powerSaveMode;
	WDRV_PIC32MZW_POWERSAVE_CORRELATION picCorrelation;
	bool saveContextEnabled;
	bool BcastTrafficEnabled;
}
```

### Description:
The configuration structure contains information related to powersave mode, correlation with the pic state, listen interval, configuring DTIM monitoring etc.

### Fields:

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|powerSaveMode| Set the power save mode(Run, WSM)  |
|picCorrelation| Specify the correlation between PIC powersave state and WiFi powersave modes |
|saveContextEnabled| Specify whether to save WiFi context data to WCM(WiFi Context Memory) Set to FALSE currently|
|BcastTrafficEnabled| If TRUE, STA will awake each DTIM beacon for recieving broadcast traffic. If FALSE, STA will not wakeup at the DTIM beacon,but it will wakeup depends only on the configured Listen Interval (```DRV_PIC32MZW_PS_LISTEN_INTERVAL```) (Applicable now for WSM mode only) */  |
|

<br />
<br />

```
WDRV_PIC32MZW_MAC_ADDR
{
	uint8_t addr[WDRV_PIC32MZW_MAC_ADDR_LEN];
	bool valid;
}
```

### Description:
Defines a Wi-Fi network name (Service Set Identifier) – this is used to identify a specific Wi-Fi network to connect to, or to specify the name of a the network being hosted when the device is in SoftAP mode.

### Fields:

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|addr| 	A 6 byte array containing the BSSID / MAC-address of the network|
| valid|	Identifies if the address is valid (True) or invalid (False)|
|

<br />
<br />

``` 
WDRV_PIC32MZW_SSID
{
	uint8_t name[WDRV_PIC32MZW_MAX_SSID_LEN];
	uint8_t length;
} 
```

### Description:
Defines a Wi-Fi network name (Service Set Identifier) – this is used to identify a specific Wi-Fi network to connect to, or to specify the name of a the network being hosted when the device is in SoftAP mode.

### Fields:

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|name |	A 32-character long string – this is the maximum allowed length of an SSID|
|length|	The length of the SSID (number of bytes populated in the name field)|
|

<br />
<br />

```
WDRV_PIC32MZW_BSS_CONTEXT
{
	WDRV_PIC32MZW_SSID ssid;
	WDRV_PIC32MZW_MAC_ADDR bssid;
	WDRV_PIC32MZW_CHANNEL_ID channel;
	Bool cloaked;
}
``` 

### Description:
Context structure containing information related to Basic Service Sets (BSS). The BSS context is used to identify a network to connect to, or to define the parameters for the device if in softAP mode.

### Fields:

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|ssid| 	The network name |
|bssid|	The MAC address of the access point|
|channel|	The channel (1-13) that the network resides on|
|cloaked|	Identifies whether the network is visible (False) or hidden (True)|
|

<br />
<br />

```
WDRV_PIC32MZW_BSS_INFO
{
	WDRV_PIC32MZW_BSS_CONTEXT ctx;
	int8_t rssi;
	WDRV_PIC32MZW_SEC_MASK secCapabilities;
	WDRV_PIC32MZW_AUTH_TYPE authTypeRecommended;
}
``` 

### Description:
A structure containing information related to a discovered network (Basic Service Set). This is typically used to inspect a network’s capabilities prior to association/connection.

### Fields:

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|ctx |	The BSS (Wi-Fi network) context, refer to WDRV_PIC32MZW_BSS_CONTEXT|
|rssi|	The received signal strength of the network|
|secCapabilities|	The network security capabilities, refer to WDRV_PIC32MZW_SEC_MASK|
|authTypeRecommended|	Recommended authentication type for this network|
|

<br />
<br />

```
WDRV_PIC32MZW_AUTH_CONTEXT
{
	WDRV_PIC32MZW_AUTH_TYPE authType;
	WDRV_PIC32MZW_AUTH_MOD_MASK authMod;
	authInfo;
}
```

### Description:
Context structure containing information related to the authentication requirements of the Wi-Fi network (typically used in conjunction with the ```WDRV_PIC32MZW_BSS_CONTEXT```).

### Fields:

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|authType |	The security suite to use – recommend to use PIC32MZW1_AUTH_TYPE_DEFAULT
|authMod|	Security type modifiers/options – recommend to use PIC32MZW1_AUTH_MOD_NONE
|authInfo	|A union of two structures defining credentials used for either WEP or WPA/WPA2/WPA3 (Personal) security. The structures within this union have the following fields:
|

WEP

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|uint8_t idx	|WEP key index (range 1 – 4)|
|uint8_t size	|WEP Key size (10 for WEP_40, or 26 for WEP_104)|
|uint8_t key[]	|A 27 byte array for storing the WEP key|
|

WPA/WPA2/WPA3 Personal

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|uint8_t size|	Password length in bytes (maximum of 64)|
|uint8_t password[]	|A 64 byte array for storing the password||
|

<br />
<br />

```
WDRV_PIC32MZW_REGDOMAIN_SELECT
```

### Description:
Defines the set of regulatory configurations to return to the application when querying via the WDRV_PIC32MZW_RegDomainGet API.

### Definitions:
Please note: the following definitions are all prefixed with ```‘WDRV_PIC32MZW_REGDOMAIN_SELECT _’```:

|	Definition		 | 		Description					|
|-------------------------|-----------------------------|
|NONE|  | 
|CURRENT|	Request only the information for the currently active region |
|ALL|	Request information on all the regulatory configurations stored on the module|
|

<br />
<br />

```
WDRV_PIC32MZW_REGDOMAIN_INFO
{
	uint8_t regDomainLen;
    uint8_t regDomain[WDRV_PIC32MZW_REGDOMAIN_MAX_NAME_LEN];
    struct
    {
        uint16_t major;
        uint16_t minor;
    } version;
}
```

### Description:
Specifies the country code length, country code associated with the
    regulatory domain, major and minor values of RF PHY version.

### Fields:

|	Field		 | 		Description					|
|-------------------------|-----------------------------|
|regDomainLen |	Length of Country code|
|regDomain|	Country code for the regulatory domain|
|version	| RF version values|
|


<br />
<br />

```
WDRV_PIC32MZW_SEC_MASK
```

### Description:
Defines the various security suites supported by a network.

### Definitions:
Please note: the following definitions are all prefixed with ```‘WDRV_PIC32MZW_SEC_BIT_’```:

|	Definition		 | 		Description					|
|-------------------------|-----------------------------|
|WEP|	Network supports WEP|
|WPA|	Network supports WPA (uses TKIP encryption for all traffic)|
|WPA2OR3|	Network supports CCMP for pairwise traffic, support CCMP for group traffic only if the WPA bit (above) is not set|
|MFP_CAPABLE|	Network supports (does not mandate) management frame protection (802.11w)|
|MFP_REQUIRED|	Network mandates use of management frame protection (802.11w)|
|ENTERPRISE|	Network supports WPA/WPA2/WPA3 Enterprise|
|PSK|	Network supports passphrase (WPA/WPA2 Personal) authentication|
|SAE|	Network supports WPA3 (Simultaneous Authentication of Equals) authentication|
|
