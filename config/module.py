######################  WiFi Driver  ######################
def loadModule():
    print("Load Module: Harmony WiFi Driver")

    ## WIFI Driver
    drvWifiWincComponent = Module.CreateComponent("drvWifiWinc", "WINC", "/Wireless/Drivers/", "driver/winc/config/drv_winc.py")
    drvWifiWincComponent.addCapability("wdrv_winc", "WDRV_WINC")
    drvWifiWincComponent.addCapability("libdrvWincMac", "MAC")