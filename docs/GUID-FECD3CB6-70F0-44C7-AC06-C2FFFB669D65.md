# STA

This topic provides information on how the library supports connecting to BSSs as a Wi-Fi station device.

The WINC device can connect to a BSS as a Wi-Fi station device. To connect to a BSS the function WDRV_WINC_BSSConnect can be called, this takes two contexts which provide information about the BSS to be connected to and the authentication method to be used. A callback is provided to events relating to the connection.

To disconnect from a BSS the function WDRV_WINC_BSSDisconnect can be called.
To reconnect to a previous BSS the function WDRV_WINC_BSSReconnect can be called, a callback can also be provided to provide the same feedback as the function WDRV_WINC_BSSConnect.