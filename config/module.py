######################  WiFi Driver  ######################
def loadModule():
    print('Load Module: Harmony WiFi Driver')

    ## WIFI Driver
    drvWifiWincComponent = Module.CreateComponent('drvWifiWinc', 'WINC', '/Wireless/Drivers', 'driver/winc/config/drv_winc.py')
    drvWifiWincComponent.addCapability('wdrv_winc', 'WDRV_WINC', True)
    drvWifiWincComponent.addCapability('libdrvWincMac', 'MAC')

    appSerBridge = Module.CreateComponent('appWincSerialBridge', 'Serial Bridge Application', '/Wireless/SerialBridge/', 'apps/serial_bridge/config/app_winc_serial_bridge.py')
	