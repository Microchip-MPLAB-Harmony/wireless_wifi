######################  WiFi Driver  ######################
def loadModule():
    print("Load Module: Harmony WiFi Driver")

    ## WIFI Driver
    drvWifiWincComponent = Module.CreateComponent("drvWifiWinc", "Wi-Fi WINC Driver", "/Harmony/Drivers/", "driver/winc/config/drv_winc.py")
    drvWifiWincComponent.addCapability("wdrv_winc", "WDRV_WINC")

    ## Serial Bridge Application
    appWincAtCmd = Module.CreateComponent("appWincSerialBridge", "Serial Bridge Application", "/Applications/WINC/", "app/serial_bridge/config/app_winc_serial_bridge.py")
