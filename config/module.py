######################  WiFi Driver  ######################
def loadModule():
    print('Load Module: Harmony WiFi Driver')

    ## WIFI Driver
    drvWifiWincComponent = Module.CreateComponent('drvWifiWinc', 'WINC', '/Wireless/Drivers', 'driver/winc/config/drv_winc.py')
    drvWifiWincComponent.addCapability('wdrv_winc', 'WDRV_WINC', True)
    drvWifiWincComponent.addCapability('libdrvWincMac', 'MAC')

    drvWifiWincLiteComponent = Module.CreateComponent('drvWifiWincLite', 'WINC (Lite)', '/Wireless/Drivers', 'driver/winc/config/drv_winc_lite.py')
    drvWifiWincLiteComponent.addCapability('wdrv_winc', 'WDRV_WINC', True)
    drvWifiWincLiteComponent.addCapability('libdrvWincMac', 'MAC')

    if 'PIC32MZ1025W104132' in Variables.get('__PROCESSOR'):
        drvWifiPic32mzw1Component = Module.CreateComponent('drvWifiPic32mzw1', 'PIC32MZW1', '/Wireless/Drivers/', 'driver/pic32mzw1/config/drv_pic32mzw1.py')
        drvWifiPic32mzw1Component.addCapability('wdrv_pic32mzw1', 'WDRV_PIC32MZW1', True)
        drvWifiPic32mzw1Component.addCapability('libdrvPic32mzw1Mac', 'MAC')

    ## Ethernet MAC driver for PIC32MZW1
        drvPic32mEthmacComponent = Module.CreateComponent('drvPic32mEthmac', 'ETHMAC', '/Harmony/Drivers/MAC Driver/Internal/', 'driver/ethmac/config/drv_intmac_ethmac.py')
        drvPic32mEthmacComponent.addCapability('libdrvPic32mEthmac','MAC')
        drvPic32mEthmacComponent.addDependency('ETHMAC_PHY_Dependency', 'PHY', None, True, True)

    ## Serial Bridge Application
    appSerBridge = Module.CreateComponent('appWincSerialBridge', 'Serial Bridge Application', '/Wireless/SerialBridge/', 'apps/serial_bridge/config/app_winc_serial_bridge.py')

    appdebugComponent = Module.CreateComponent('sysAppDebugPic32mzw1', "App Debug Service", '/Wireless/System Services/', 'system/appdebug/config/sys_appdebug.py')
	
    sysWifiPic32mzw1Component = Module.CreateComponent('sysWifiPic32mzw1', 'WIFI SERVICE', '/Wireless/System Services/', 'system/wifi/config/sys_wifi.py')
    #sysWifiPic32mzw1Component.setDisplayType("WiFi System Service")
    
    sysWifiProvPic32mzw1Component = Module.CreateComponent('sysWifiProvPic32mzw1', 'WIFI PROVISIONING SERVICE', '/Wireless/System Services/', 'system/wifiprov/config/sys_wifiprov.py')
    #sysWifiProvPic32mzw1Component.setDisplayType("WiFi Provision System Service")
	########################## Harmony Network Net Service #################################    
    netComponent = Module.CreateComponent("sysNetPic32mzw1", "Net Service", "/Harmony/Harmony Networking","system/net/config/sys_net.py")
    #netComponent.addCapability("libNet","net",True)    
    #netComponent.addDependency("Net_Crypto_Dependency", "TLS Provider", None, False, False)

    ############################### Third Party wolfSSL Module #####################################
    pahomqttComponent = Module.CreateComponent('lib_pahomqtt', 'Paho MQTT Library', '/Third Party Libraries/PahoMqtt/', 'config/pahomqtt.py')

    mqttComponent = Module.CreateComponent('sysMqttPic32mzw1', 'MQTT Service', '/Harmony/Harmony Networking','system/mqtt/config/sys_mqtt.py')

    ## Example Applications
    appWincExampleConfComponent = Module.CreateComponent('appWincExampleConf', 'WINC Example Application Configuration', '/Wireless/Examples/', 'config/winc_example_app_conf.py')
