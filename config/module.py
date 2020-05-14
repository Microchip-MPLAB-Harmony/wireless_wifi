######################  WiFi Driver  ######################
def loadModule():
    print('Load Module: Harmony WiFi Driver')

    ## WIFI Driver
    drvWifiWincComponent = Module.CreateComponent('drvWifiWinc', 'WINC', '/Wireless/Drivers', 'driver/winc/config/drv_winc.py')
    drvWifiWincComponent.addCapability('wdrv_winc', 'WDRV_WINC', True)
    drvWifiWincComponent.addCapability('libdrvWincMac', 'MAC')

    drvWifiPic32mzw1Component = Module.CreateComponent('drvWifiPic32mzw1', 'PIC32MZW1', '/Wireless/Drivers/', 'driver/pic32mzw1/config/drv_pic32mzw1.py')
    drvWifiPic32mzw1Component.addCapability('wdrv_pic32mzw1', 'WDRV_PIC32MZW1', True)
    drvWifiPic32mzw1Component.addCapability('libdrvPic32mzw1Mac', 'MAC')

    ## Serial Bridge Application
    appSerBridge = Module.CreateComponent('appWincSerialBridge', 'Serial Bridge Application', '/Wireless/SerialBridge/', 'apps/serial_bridge/config/app_winc_serial_bridge.py')
	