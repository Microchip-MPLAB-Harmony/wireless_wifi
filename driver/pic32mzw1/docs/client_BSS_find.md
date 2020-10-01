---
grand_parent: WLAN API Guide
parent: PIC32MZW1 WLAN API Guide
title: Client - BSS Find APIs
has_toc: false
nav_order: 2
---

# Client - BSS Find APIs


The application can use this interface to request a scan for local BSSs. Once requested, the device will conduct the search using the configured search parameters and report the results back to the application one BSS at a time. This interface can be used in either a callback mode, a polled mode, or a combination of both callback and polled depending on how the application wishes to receive the BSS information.

How a scan is conducted depends on the parameters and channel lists provided by the application?

The channels which can be scanned may be set by calling ```WDRV_PIC32MZW_BSSFindSetEnabledChannels24```.

When an application wishes to begin a scan operation it must call ```WDRV_PIC32MZW_BSSFindFirst```. It is possible to request a scan on only a single channel or on all channels enabled by calling ```WDRV_PIC32MZW_BSSFindSetEnabledChannels```. The scan can be performed using active mode (where probe requests are transmitted) or passive mode (where beacons are listened for).

```WDRV_PIC32MZW_BSSFindFirst``` takes an optional callback function to use for notifying the application when the scan operation is complete, and the first result is available. If this isn’t provided, the application can poll this interface using ```WDRV_PIC32MZW_BSSFindInProgress``` to determine if the device is still scanning.

### Getting Results – Callback Only

If a callback function was provided to ```WDRV_PIC32MZW_BSSFindFirst```, the Wi-Fi driver will call this callback when the first results are available. The callback is provided with the scan result for a single BSS as well as the index of the results within the full set of BSSs discovered.
If the callback function returns the value true to the Wi-Fi driver, it will cause the driver to request the next result from the PIC32MZW1 device. When this result is available, the Wi-Fi driver will again call the callback and provide the BSS information. It is thus possible to receive all the results via the callback.

### Getting Results – Callback Notification, Foreground Retrieval

While the application may wish to be notified of a BSS result being available via the callback mechanism, it may be preferable to retrieve the result information from a foreground task. For example, in an OS environment the callback may simply signal a semaphore triggering the main application task to retrieve the BSS information.

In this model the callback called by the Wi-Fi driver should return the value false. The Wi-Fi driver will not request the next set of BSS information from the PIC32MZW1 device.

The foreground task may then call ```WDRV_PIC32MZW_BSSFindGetInfo``` with a pointer to a ```WDRV_PIC32MZW_BSS_INFO``` structure to receive the BSS information. If the function is called when there is no valid BSS information present in the PIC32MZW driver, the function will return ```WDRV_PIC32MZW_STATUS_NO_BSS_INFO```.

When the application wishes to request the next set of BSS information, it must call WDRV_PIC32MZW_BSSFindNext. It is possible to change the callback function at this time or even turn off callback operation if a NULL pointer is used. Assuming the callback function is again specified the Wi-Fi driver will request the next set of BSS information from the device and inform the application via the callback.

### Getting Results – Polled

The operations of the BSS scan can be inferred by polling this interface.
```WDRV_PIC32MZW_BSSFindInProgress``` indicates if the scan operation is currently active and results are not yet available. 
```WDRV_PIC32MZW_BSSFindGetNumBSSResults``` returns the number of sets of BSS information available as a result of a scan operation.
```WDRV_PIC32MZW_BSSFindGetInfo``` can be called to retrieve the BSS information. If the information is not yet available this function will return WDRV_PIC32MZW_STATUS_NO_BSS_INFO.

### Terminating a BSS Search

Once a scan operation has been started by calling ```WDRV_PIC32MZW_BSSFindFirst``` it must be allowed to complete, once it has the application may decide to ignore some or all of the results. To abort the BSS information retrieval and abandon any remaining results the application can call ```WDRV_PIC32MZW_BSSFindReset```.



### Available APIs are listed below

``` 
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSFindFirst
(
	DRV_HANDLE handle, 
	WDRV_PIC32MZW_CHANNEL_ID channel, 
	bool active, 
	const WDRV_PIC32MZW_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
)
```

#### Description:
A scan is requested on the specified channels. An optional callback can be provided to receive notification of the first BSS discovered.

#### Preconditions:

```WDRV_PIC32MZW_Initialize``` must have been called.

```WDRV_PIC32MZW_Open``` must have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description						|
|-------------------------|--------------------------------------------------------------|
|handle	| Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|channel |	Channel to scan, can be WDRV_PIC32MZW_ALL_CHANNELS in which case all enabled channels are scanned.|
|active |	Use active vs passive scanning.|
|pfNotifyCallback |	Callback to receive notification of first BSS found. A pointer to a function of the following prototype: ```bool func (DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, WDRV_PIC32MZW_BSS_INFO *pBssInfo )``` |
|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- A scan was initiated.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR	- The request to the PIC32MZW1 was rejected.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_SCAN_IN_PROGRESS	- A scan is already in progress.

#### Remarks:

If channel is ```WDRV_PIC32MZW_ALL_CHANNELS``` then all enabled channels are scanned. The enabled channels can be configured using ```WDRV_PIC32MZW_BSSFindSetEnabledChannels```. How the scan is performed can be configured using ```WDRV_PIC32MZW_BSSFindSetScanParameters```.


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSFindNext
(
	DRV_HANDLE handle, 
	WDRV_PIC32MZW_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
)
```

#### Description:
The information structure of the next BSS is requested from the PIC32MZW1.

#### Preconditions:

```WDRV_PIC32MZW_Initialize``` must have been called.

```WDRV_PIC32MZW_Open``` must have been called to obtain a valid handle.

```WDRV_PIC32MZW_BSSFindFirst``` must have been called.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|Handle		|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|pfNotifyCallback |	Callback to receive notification of first BSS found. A pointer to a function of the following prototype: ```bool func (DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, WDRV_PIC32MZW_BSS_INFO *pBssInfo )``` |
|

#### Returns:

###### WDRV_PIC32MZW_STATUS_OK			- The request was accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_SCAN_IN_PROGRESS	- A scan is already in progress.
###### WDRV_PIC32MZW_STATUS_BSS_FIND_END	- No more results are available.


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

``` 
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSFindReset
(
	DRV_HANDLE handle, 
	WDRV_PIC32MZW_BSSFIND_NOTIFY_CALLBACK pfNotifyCallback
)
```

#### Description:
The information structure of the first BSS is requested from the PIC32MZW1.

#### Preconditions:
``` WDRV_PIC32MZW_Initialize```  must have been called.

``` WDRV_PIC32MZW_Open```  must have been called to obtain a valid handle.

``` WDRV_PIC32MZW_BSSFindFirst```  must have been called.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|``` handle``` 		|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|``` pfNotifyCallback```  |	Callback to receive notification of first BSS found. A pointer to a function of the following prototype: ```bool func (DRV_HANDLE handle, uint8_t index, uint8_t ofTotal, WDRV_PIC32MZW_BSS_INFO *pBssInfo )``` |
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK			- The request was accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN		- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG		- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_SCAN_IN_PROGRESS	- A scan is already in progress.
###### WDRV_PIC32MZW_STATUS_BSS_FIND_END	- No more results are available.


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

``` 
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSFindGetInfo
(
	DRV_HANDLE handle, 
	WDRV_PIC32MZW_BSS_INFO *const pBSSInfo
)
```

#### Description:
After each call to either WDRV_PIC32MZW_BSSFindFirst or WDRV_PIC32MZW_BSSFindNext the driver receives a single BSS information structure which it stores. This function retrieves that structure.

#### Preconditions:
``` WDRV_PIC32MZW_Initialize```  must have been called.

``` WDRV_PIC32MZW_Open```  must have been called to obtain a valid handle.

``` WDRV_PIC32MZW_BSSFindFirst```  must have been called.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|``` handle``` 	|Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|``` pBSSInfo``` |	Pointer to structure to populate with the current BSS information. |
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK		- The request was accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN	- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG	- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_NO_BSS_INFO	- There is no current BBS information available.

#### Remarks:
This function may be polled after calling ```WDRV_PIC32MZW_BSSFindFirst``` or ```WDRV_PIC32MZW_BSSFindNext``` until it returns ```WDRV_PIC32MZW_STATUS_OK```.

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

``` 
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSFindSetScanParameters
(
	DRV_HANDLE handle, 
	uint16_t activeScanTime, 
	uint16_t passiveListenTime
)
```

#### Description:
This function set various parameters to control the scan behavior.

#### Preconditions:
``` WDRV_PIC32MZW_Initialize```  must have been called.

``` WDRV_PIC32MZW_Open```  must have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|``` handle``` 	|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|``` activeScanTime``` 	|	Time spent on each active channel probing for BSS's.|
|``` passiveListenTime``` 	|	Time spent on each passive channel listening for beacons.|
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK		- The request was accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN	- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG	- The parameters were incorrect.

#### Remarks:
If any parameter is zero then the configured value is unchanged.


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

```
WDRV_PIC32MZW_STATUS WDRV_PIC32MZW_BSSFindSetEnabledChannels24
(
	DRV_HANDLE handle, 
	WDRV_PIC32MZW_CHANNEL24_MASK channelMask24
)
```

#### Description:
To comply with regulatory domains certain channels must not be scanned. This function configures which channels are enabled to be used.

#### Preconditions:
``` WDRV_PIC32MZW_Initialize```  must have been called.

``` WDRV_PIC32MZW_Open```  must have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|```handle```	|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|```channelMask24```	|	A 2.4GHz channel mask detailing all the enabled channels.|
|

#### Returns:
###### WDRV_PIC32MZW_STATUS_OK		- The request was accepted.
###### WDRV_PIC32MZW_STATUS_NOT_OPEN	- The driver instance is not open.
###### WDRV_PIC32MZW_STATUS_INVALID_ARG	- The parameters were incorrect.
###### WDRV_PIC32MZW_STATUS_REQUEST_ERROR - The PIC32MZW1 was unable to accept this request.


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


```
uint8_t WDRV_PIC32MZW_BSSFindGetNumBSSResults(DRV_HANDLE handle)
```

#### Description:
Returns the number of BSS scan results found.

#### Preconditions:
```WDRV_PIC32MZW_Initialize``` must have been called.

```WDRV_PIC32MZW_Open``` must have been called to obtain a valid handle.

```WDRV_PIC32MZW_BSSFindFirst``` must have been called to start a scan.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|```handle```	|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|

#### Returns:
Number of BSS scan results available. Zero indicates no results or an error occurred.


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


```
bool WDRV_PIC32MZW_BSSFindInProgress(DRV_HANDLE handle)
```

#### Description:
Returns a flag indicating if a BSS scan operation is currently running.

#### Preconditions:

```WDRV_PIC32MZW_Initialize``` must have been called.

```WDRV_PIC32MZW_Open``` must have been called to obtain a valid handle.

#### Parameters:

|	Parameter			 | 						Description								|
-------------------------|--------------------------------------------------------------|
|```handle```	|	Client handle obtained by a call to WDRV_PIC32MZW_Open.|
|

#### Returns:
Flag indicating if a scan is in progress. If an error occurs the result is false.
