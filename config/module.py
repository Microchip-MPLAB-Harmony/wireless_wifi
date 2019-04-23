######################  WiFi Driver  ######################
def loadModule():
    print("Load Module: Harmony WiFi Driver")
    
    ## WIFI Driver
    drvWincComponent = Module.CreateComponent("drvWifiWinc", "Wi-Fi Driver", "/Harmony/Drivers/", "driver/winc/config/drv_winc.py")
    drvWincComponent.addCapability("libdrvWifi","WIFI Driver",True)