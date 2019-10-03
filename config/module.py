######################  WiFi Driver  ######################
def loadModule():
    print("Load Module: Harmony WiFi Driver")

    ## WIFI Driver
    drvWifiWincComponent = Module.CreateComponent("drvWifiWinc", "WINC", "/Harmony/Drivers/", "driver/winc/config/drv_winc.py")
    drvWifiWincComponent.addCapability("wdrv_winc", "WDRV_WINC")
    drvWifiWincComponent.addCapability("libdrvWincMac", "MAC")

    ## Serial Bridge Application
    appWincAtCmd = Module.CreateComponent("appWincSerialBridge", "Serial Bridge Application", "/Applications/WINC/", "apps/serial_bridge/config/app_winc_serial_bridge.py")
