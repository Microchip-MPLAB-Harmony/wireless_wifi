######################  WiFi Driver  ######################
def loadModule():
    print("Load Module: Harmony WiFi Driver")
    
    ## WIFI Driver
    drvWifiWincComponent = Module.CreateComponent("drvWifiWinc", "Wi-Fi WINC Driver", "/Harmony/Drivers/", "driver/winc/config/drv_winc.py")
