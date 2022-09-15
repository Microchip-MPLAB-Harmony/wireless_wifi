# Association Management

This topic provides information on Wi-Fi association management.

The application can query information about the current Wi-Fi association by calling one of these interface functions. The requested information may not be immediately available to the MCU driver, if this is the case the driver will request the information from the WINC device.

Each interface function can be passed a pointer to a buffer to receive the requested information value and/or a callback function to use for notification. If no buffer pointer is provided each interface function will request up to date information from the WINC device and provide it through the callback provided.

If the information is available within the driver it will be copied into the supplied buffer and the function will return WDRV_WINC_STATUS_OK. If the information is not available, or the buffer pointer is not supplied, the function will attempt to request the information from the WINC and if successfully requested will return WDRV_WINC_STATUS_RETRY_REQUEST indicating the request could be retired if required. The function returns WDRV_WINC_STATUS_REQUEST_ERROR if an error occurred during make the request.