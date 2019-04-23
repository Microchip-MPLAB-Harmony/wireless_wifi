###############################################################################
########################## WincDriver Configurations ############################
###############################################################################

ComboVal_WiFi_Driver_Mode = ["Ethernet_Mode", "Socket_Mode"]
ComboVal_WiFi_Interrupt = ["External_Interrupt", "Change_Notification"]

def instantiateComponent(drvWincComponent):
    print("WINC Driver Component")
    configName = Variables.get("__CONFIGURATION_NAME")
    
    
    # Use Wi-Fi Driver?	
    drvUseWifi = drvWincComponent.createBooleanSymbol("DRV_WIFI_USE_DRIVER", None) 
    drvUseWifi.setLabel("Use WIFI Driver?")
    drvUseWifi.setVisible(False)
    drvUseWifi.setDescription("Use WIFI Driver?")
    drvUseWifi.setDefaultValue(True)

    # WiFi Device
    wifiDeviceUse = drvWincComponent.createKeyValueSetSymbol("WIFI_DEVICE_SELECT",None)
    wifiDeviceUse.setLabel("Wi-Fi Device")
    wifiDeviceUse.setVisible(True)
    wifiDeviceUse.addKey("WINC1500", "0", "WINC1500")
    wifiDeviceUse.addKey("WINC3400", "1", "WINC3400")
    wifiDeviceUse.setDisplayMode("Description")
    wifiDeviceUse.setOutputMode("Key")
    wifiDeviceUse.setDefaultValue(0)

    # WiFi Log mode WINC_DRIVER_LOG_LEVEL
    wifiDriverDebugLogLevel = drvWincComponent.createKeyValueSetSymbol("WIFI_DRIVER_LOG_LEVEL",None)
    wifiDriverDebugLogLevel.setLabel("Wi-Fi Driver Log Level")
    wifiDriverDebugLogLevel.setVisible(True)
    wifiDriverDebugLogLevel.addKey("None", "0", "None")
    wifiDriverDebugLogLevel.addKey("Error", "1", "Error")
    wifiDriverDebugLogLevel.addKey("Inform", "2", "Inform")
    wifiDriverDebugLogLevel.addKey("Trace", "3", "Mode")
    wifiDriverDebugLogLevel.addKey("Verbose", "4", "Verbose")
    wifiDriverDebugLogLevel.setDisplayMode("Description")
    wifiDriverDebugLogLevel.setOutputMode("Key")
    wifiDriverDebugLogLevel.setDefaultValue(0)

    # External Interrupt
    wifiExternalInterrupt = drvWincComponent.createComboSymbol("WIFI_EXTERNAL_INTERRUPT_USE",None,ComboVal_WiFi_Interrupt)
    wifiExternalInterrupt.setLabel("External Interrupt Use")
    wifiExternalInterrupt.setVisible(True)
    #wifiExternalInterrupt.setDisplayMode("Description")
    #wifiExternalInterrupt.setOutputMode("Key")
    wifiExternalInterrupt.setDefaultValue("Change_Notification")

    # WINC WiFi TCP/IP Mode
    wifiSpiInstanceIndex = drvWincComponent.createIntegerSymbol("WIFI_DRIVER_SPI_INSTANCE_INX", None)
    wifiSpiInstanceIndex.setLabel("WiFi Driver SPI Instance Index")
    wifiSpiInstanceIndex.setVisible(True)

    # WINC WiFi SPI Instance Index
    wifiChangeNotification = drvWincComponent.createIntegerSymbol("WIFI_DRIVER_EXTERNAL_INETERRUPT_INSTANCE_INX", wifiExternalInterrupt)
    wifiChangeNotification.setLabel("WiFi Driver External Interrupt Instance Index?")
    wifiChangeNotification.setVisible(False)
    wifiChangeNotification.setDependencies(InterruptModeVisibility, ["WIFI_EXTERNAL_INTERRUPT_USE"])

    # WINC Firmware Version
    WiFiFirmwareVersion = drvWincComponent.createKeyValueSetSymbol("WIFI_DRV_VERSION",None)
    WiFiFirmwareVersion.setLabel("Firmware Version")
    WiFiFirmwareVersion.setVisible(True)
    WiFiFirmwareVersion.addKey("19.6.1", "0", "WINC1500-19.6.1")
    WiFiFirmwareVersion.addKey("19.5.4", "1", "WINC1500-19.5.4")
    WiFiFirmwareVersion.setDisplayMode("Description")
    WiFiFirmwareVersion.setOutputMode("Key")
    WiFiFirmwareVersion.setDefaultValue(0)

    # WINC WiFi Driver Mode
    wifiDriverMode = drvWincComponent.createComboSymbol("WIFI_DRIVER_MODE_CHOICE",None,ComboVal_WiFi_Driver_Mode)
    wifiDriverMode.setLabel("WiFi Driver Mode")
    wifiDriverMode.setDefaultValue("Socket_Mode")

    # WINC WiFi TCP/IP Mode
    wifiDriverTcpipMode = drvWincComponent.createBooleanSymbol("WIFI_DRIVER_TCPIP_MODE", wifiDriverMode)
    wifiDriverTcpipMode.setLabel("Use TCP/IP Stack?")
    wifiDriverTcpipMode.setVisible(False)
    wifiDriverTcpipMode.setDependencies(SocketModeVisibility, ["WIFI_DRIVER_MODE_CHOICE"])

    # WINC WiFi Use Simple IWPRIV Control Interface? Mode Iwpriv
    wifiDriverIwprivMode = drvWincComponent.createBooleanSymbol("WIFI_DRIVER_IWPRIV_MODE", wifiDriverTcpipMode)
    wifiDriverIwprivMode.setLabel("Use Simple IWPRIV Control Interface?")
    wifiDriverIwprivMode.setVisible(False)
    wifiDriverIwprivMode.setDependencies(IwprivModeVisibility, ["WIFI_DRIVER_TCPIP_MODE"])
    wifiDriverIwprivMode.setDefaultValue(True)

    # RTOS Configuration
    drvWincRtosMenu = drvWincComponent.createMenuSymbol("DRV_WINC_RTOS_MENU", None)
    drvWincRtosMenu.setLabel("RTOS Configuration")
    drvWincRtosMenu.setDescription("RTOS Configuration")
    drvWincRtosMenu.setVisible(False)
    drvWincRtosMenu.setVisible((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal") and (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != None))
    drvWincRtosMenu.setDependencies(drvWincshowRTOSMenu, ["HarmonyCore.SELECT_RTOS"])

    # WIFI Driver Execution mode
    drvWifiInstnExecMode = drvWincComponent.createComboSymbol("DRV_WIFI_RTOS", drvWincRtosMenu, ["Standalone"])
    drvWifiInstnExecMode.setLabel("Run this driver instance as")
    drvWifiInstnExecMode.setVisible(False)
    drvWifiInstnExecMode.setDescription("WIFI Driver Execution mode")
    drvWifiInstnExecMode.setDefaultValue("Standalone")
    #drvWifiInstnExecMode.setDependencies(drvWifiMenuVisibleSingle, ["DRV_WIFI_USE_DRIVER"])

    # WIFI Driver Task Stack Size
    drvWifiDrvTaskSize = drvWincComponent.createIntegerSymbol("DRV_WIFI_RTOS_STACK_SIZE", drvWincRtosMenu)
    drvWifiDrvTaskSize.setLabel("Task Size")
    drvWifiDrvTaskSize.setVisible(True)
    drvWifiDrvTaskSize.setDescription("WIFI Driver Task Stack Size")
    drvWifiDrvTaskSize.setDefaultValue(1024)
    drvWifiDrvTaskSize.setDependencies(drvWifiRTOSStandaloneMenu, ["DRV_WIFI_RTOS"])


    # WIFI Driver Task Priority
    drvWifiDrvTaskPriority = drvWincComponent.createIntegerSymbol("DRV_WIFI_RTOS_TASK_PRIORITY", drvWincRtosMenu)
    drvWifiDrvTaskPriority.setLabel("Task Priority")
    drvWifiDrvTaskPriority.setVisible(True)
    drvWifiDrvTaskPriority.setDescription("WIFI Driver Task Priority")
    drvWifiDrvTaskPriority.setDefaultValue(1)
    drvWifiDrvTaskPriority.setDependencies(drvWifiRTOSStandaloneMenu, ["DRV_WIFI_RTOS"])


    # WIFI Use Task Delay?
    drvWifiUseTaskDelay = drvWincComponent.createBooleanSymbol("DRV_WIFI_RTOS_USE_DELAY", drvWincRtosMenu)
    drvWifiUseTaskDelay.setLabel("Use Task Delay?")
    drvWifiUseTaskDelay.setVisible(True)
    drvWifiUseTaskDelay.setDescription("WIFI Use Task Delay?")
    drvWifiUseTaskDelay.setDefaultValue(True)
    drvWifiUseTaskDelay.setDependencies(drvWifiRTOSStandaloneMenu, ["DRV_WIFI_RTOS"])


    # WIFI Driver Task Delay
    drvWifiDrvTaskDelay = drvWincComponent.createIntegerSymbol("DRV_WIFI_RTOS_DELAY", drvWincRtosMenu)
    drvWifiDrvTaskDelay.setLabel("Task Delay")
    drvWifiDrvTaskDelay.setVisible(False)
    drvWifiDrvTaskDelay.setDescription("WIFI Driver Task Delay")
    drvWifiDrvTaskDelay.setDefaultValue(1)
    drvWifiDrvTaskDelay.setDependencies(drvWifiRTOSTaskDelayMenu, ["DRV_WIFI_RTOS", "DRV_WIFI_RTOS_USE_DELAY"])

    ############################################################################
    #### Code Generation ####
    ############################################################################

    #file DRV_WINC_CONFIG_H "$HARMONY_VERSION_PATH/framework/driver/winc/config/drv_winc_config.h" to "$PROJECT_HEADER_FILES/framework/driver/winc/config/drv_wifi_winc.h.ftl"
    ##drvWincConfigHeaderFile.setOutputName("drv_wifi_winc.h")
    ##drvWincConfigHeaderFile.setDestPath("driver/winc/config/")
    ##drvWincConfigHeaderFile.setProjectPath("config/" + configName + "/driver/winc/config/")
    ##drvWincConfigHeaderFile.setType("HEADER")
    ##drvWincConfigHeaderFile.setMarkup(True)
    ##drvWincConfigHeaderFile.setOverwrite(True)
    
    #add "<#include \"/framework/driver/winc/config/drv_wifi_winc.h.ftl\">"  to list SYSTEM_CONFIG_H_DRIVER_CONFIGURATION 
    drvWincConfigHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_CONFIG", None)
    drvWincConfigHeaderFile.setType("STRING")
    drvWincConfigHeaderFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    drvWincConfigHeaderFile.setSourcePath("driver/winc/config/drv_wifi_winc.h.ftl")
    drvWincConfigHeaderFile.setMarkup(True)
    
    
    #add "<#include \"/framework/driver/winc/config/drv_wifi_winc.c.ftl\">"  to list SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA
    drvWincInitDataSourceFtl = drvWincComponent.createFileSymbol(None, None)
    drvWincInitDataSourceFtl.setType("STRING")
    drvWincInitDataSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA")
    drvWincInitDataSourceFtl.setSourcePath("driver/winc/config/drv_wifi_winc.c.ftl")
    drvWincInitDataSourceFtl.setMarkup(True)
    
    #add "<#include \"/framework/driver/winc/config/drv_winc_init.c.ftl\">"  to list SYSTEM_INIT_C_SYS_INITIALIZE_DRIVERS
    drvWincInitSourceFtl = drvWincComponent.createFileSymbol(None, None)
    drvWincInitSourceFtl.setType("STRING")
    drvWincInitSourceFtl.setOutputName("core.LIST_SYSTEM_INIT_C_SYS_INITIALIZE_DRIVERS")
    drvWincInitSourceFtl.setSourcePath("driver/winc/config/drv_winc_init.c.ftl")
    drvWincInitSourceFtl.setMarkup(True)
    
    #add "<#include \"/framework/driver/winc/config/drv_winc_tasks.c.ftl\">"  to list SYSTEM_TASKS_C_CALL_DRIVER_TASKS
    # drvWincSysTaskSourceFtl = drvWincComponent.createFileSymbol(None, None)
    # drvWincSysTaskSourceFtl.setType("STRING")
    # drvWincSysTaskSourceFtl.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS")
    # drvWincSysTaskSourceFtl.setSourcePath("driver/winc/config/drv_winc_tasks.c.ftl")
    # drvWincSysTaskSourceFtl.setMarkup(True)
    
    #add "<#include \"/framework/driver/winc/config/drv_wifi_winc_config.h.ftl\">"  to list SYSTEM_CONFIG_H_DRIVER_CONFIGURATION 
    drvWincSysConfigSourceFtl = drvWincComponent.createFileSymbol(None, None)
    drvWincSysConfigSourceFtl.setType("STRING")
    drvWincSysConfigSourceFtl.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    drvWincSysConfigSourceFtl.setSourcePath("driver/winc/config/drv_wifi_winc_config.h.ftl")
    drvWincSysConfigSourceFtl.setMarkup(True)

    # Add wdrv_winc.h file to project
    #file WDRV_WINC_H "$HARMONY_VERSION_PATH/framework/driver/winc/include/wdrv_winc.h" to "$PROJECT_HEADER_FILES/framework/driver/winc/include/wdrv_winc.h"
    drvWincHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_H", None)
    drvWincHeaderFile.setSourcePath("driver/winc/include/wdrv_winc.h")
    drvWincHeaderFile.setOutputName("wdrv_winc.h")
    drvWincHeaderFile.setDestPath("driver/winc/include/")
    drvWincHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincHeaderFile.setType("HEADER")
    drvWincHeaderFile.setOverwrite(True)

    # Add wdrv_winc_api.h file to project
    drvWincApiHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_API_H", None)
    drvWincApiHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_api.h")
    drvWincApiHeaderFile.setOutputName("wdrv_winc_api.h")
    drvWincApiHeaderFile.setDestPath("driver/winc/include/")
    drvWincApiHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincApiHeaderFile.setType("HEADER")
    drvWincApiHeaderFile.setOverwrite(True)

    # Add wdrv_winc_assoc.h file to project
    drvWincAssocHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_ASSOC_H", None)
    drvWincAssocHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_assoc.h")
    drvWincAssocHeaderFile.setOutputName("wdrv_winc_assoc.h")
    drvWincAssocHeaderFile.setDestPath("driver/winc/include/")
    drvWincAssocHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincAssocHeaderFile.setType("HEADER")
    drvWincAssocHeaderFile.setOverwrite(True)

    # Add wdrv_winc_authctx.h file to project
    drvWincAuthenticationHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_AUTH_H", None)
    drvWincAuthenticationHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_authctx.h")
    drvWincAuthenticationHeaderFile.setOutputName("wdrv_winc_authctx.h")
    drvWincAuthenticationHeaderFile.setDestPath("driver/winc/include/")
    drvWincAuthenticationHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincAuthenticationHeaderFile.setType("HEADER")
    drvWincAuthenticationHeaderFile.setOverwrite(True)

    # Add wdrv_winc_ble.h file to project
    #drvWincBleHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_BLE_H", None)
    #drvWincBleHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_ble.h")
    #drvWincBleHeaderFile.setOutputName("wdrv_winc_ble.h")
    #drvWincBleHeaderFile.setDestPath("driver/winc/include/")
    #drvWincBleHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    #drvWincBleHeaderFile.setType("HEADER")
    #drvWincBleHeaderFile.setOverwrite(True)

    # Add wdrv_winc_bssctx.h file to project
    drvWincBSScontextHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_BSS_CONTEXT_H", None)
    drvWincBSScontextHeaderFile.setSourcePath("driver/Winc/include/wdrv_winc_bssctx.h")
    drvWincBSScontextHeaderFile.setOutputName("wdrv_winc_bssctx.h")
    drvWincBSScontextHeaderFile.setDestPath("driver/winc/include/")
    drvWincBSScontextHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincBSScontextHeaderFile.setType("HEADER")
    drvWincBSScontextHeaderFile.setOverwrite(True)

    # Add wdrv_winc_bssfind.h file to project
    drvWincBSSfindHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_BSS_FIND_H", None)
    drvWincBSSfindHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_bssfind.h")
    drvWincBSSfindHeaderFile.setOutputName("wdrv_winc_bssfind.h")
    drvWincBSSfindHeaderFile.setDestPath("driver/winc/include/")
    drvWincBSSfindHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincBSSfindHeaderFile.setType("HEADER")
    drvWincBSSfindHeaderFile.setOverwrite(True)

    # Add wdrv_winc_client_api.h file to project
    drvWincClientApiHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_CLIENT_API_H", None)
    drvWincClientApiHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_client_api.h")
    drvWincClientApiHeaderFile.setOutputName("wdrv_winc_client_api.h")
    drvWincClientApiHeaderFile.setDestPath("driver/winc/include/")
    drvWincClientApiHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincClientApiHeaderFile.setType("HEADER")
    drvWincClientApiHeaderFile.setOverwrite(True)

    # Add wdrv_winc_common.h file to project
    drvWincCommonHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_COMMON_H", None)
    drvWincCommonHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_common.h")
    drvWincCommonHeaderFile.setOutputName("wdrv_winc_common.h")
    drvWincCommonHeaderFile.setDestPath("driver/winc/include/")
    drvWincCommonHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincCommonHeaderFile.setType("HEADER")
    drvWincCommonHeaderFile.setOverwrite(True)

    # Add wdrv_winc_custie.h file to project
    drvWincCustomIEHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_CUSTOM_IE_H", None)
    drvWincCustomIEHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_custie.h")
    drvWincCustomIEHeaderFile.setOutputName("wdrv_winc_custie.h")
    drvWincCustomIEHeaderFile.setDestPath("driver/winc/include/")
    drvWincCustomIEHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincCustomIEHeaderFile.setType("HEADER")
    drvWincCustomIEHeaderFile.setOverwrite(True)

    # Add wdrv_winc_debug.h file to project
    drvWincDebugHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_DEBUG_H", None)
    drvWincDebugHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_debug.h")
    drvWincDebugHeaderFile.setOutputName("wdrv_winc_debug.h")
    drvWincDebugHeaderFile.setDestPath("driver/winc/include/")
    drvWincDebugHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincDebugHeaderFile.setType("HEADER")
    drvWincDebugHeaderFile.setOverwrite(True)

    # Add wdrv_winc_ethernet.h file to project
    drvWincEthernetHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_ETHERNET_H", None)
    drvWincEthernetHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_ethernet.h")
    drvWincEthernetHeaderFile.setOutputName("wdrv_winc_ethernet.h")
    drvWincEthernetHeaderFile.setDestPath("driver/winc/include/")
    drvWincEthernetHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincEthernetHeaderFile.setType("HEADER")
    drvWincEthernetHeaderFile.setOverwrite(True)
    drvWincEthernetHeaderFile.setEnabled((wifiDriverMode.getValue() == "Ethernet_Mode"))
    drvWincEthernetHeaderFile.setDependencies(ethernetModeFileGen, ["WIFI_DRIVER_MODE_CHOICE"])

    # Add wdrv_winc_host_file.h file to project
    drvWincHostFileHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_HOST_H", None)
    drvWincHostFileHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_host_file.h")
    drvWincHostFileHeaderFile.setOutputName("wdrv_winc_host_file.h")
    drvWincHostFileHeaderFile.setDestPath("driver/winc/include/")
    drvWincHostFileHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincHostFileHeaderFile.setType("HEADER")
    drvWincHostFileHeaderFile.setOverwrite(True)

    # Add wdrv_winc_nvm.h file to project
    drvWincNVMHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NVM_H", None)
    drvWincNVMHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_nvm.h")
    drvWincNVMHeaderFile.setOutputName("wdrv_winc_nvm.h")
    drvWincNVMHeaderFile.setDestPath("driver/winc/include/")
    drvWincNVMHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincNVMHeaderFile.setType("HEADER")
    drvWincNVMHeaderFile.setOverwrite(True)

    # Add wdrv_winc_powersave.h file to project
    drvWincPowerSaveHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_POWER_SAVE_H", None)
    drvWincPowerSaveHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_powersave.h")
    drvWincPowerSaveHeaderFile.setOutputName("wdrv_winc_powersave.h")
    drvWincPowerSaveHeaderFile.setDestPath("driver/winc/include/")
    drvWincPowerSaveHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincPowerSaveHeaderFile.setType("HEADER")
    drvWincPowerSaveHeaderFile.setOverwrite(True)

    # Add wdrv_winc_softap.h file to project
    drvWincSoftapHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_SOFT_AP_H", None)
    drvWincSoftapHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_softap.h")
    drvWincSoftapHeaderFile.setOutputName("wdrv_winc_softap.h")
    drvWincSoftapHeaderFile.setDestPath("driver/winc/include/")
    drvWincSoftapHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincSoftapHeaderFile.setType("HEADER")
    drvWincSoftapHeaderFile.setOverwrite(True)

    # Add wdrv_winc_ssl.h file to project
    drvWincSSLHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_SSL_H", None)
    drvWincSSLHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_ssl.h")
    drvWincSSLHeaderFile.setOutputName("wdrv_winc_ssl.h")
    drvWincSSLHeaderFile.setDestPath("driver/winc/include/")
    drvWincSSLHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincSSLHeaderFile.setType("HEADER")
    drvWincSSLHeaderFile.setOverwrite(True)
    #drvWincSSLHeaderFile.setEnabled((wifiDriverMode.getValue() == "Socket_Mode"))
    drvWincSSLHeaderFile.setDependencies(socketModeFileGen, ["WIFI_DRIVER_MODE_CHOICE"])

    # Add wdrv_winc_socket.h file to project
    drvWincSocketHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_SOCKET_H", None)
    drvWincSocketHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_socket.h")
    drvWincSocketHeaderFile.setOutputName("wdrv_winc_socket.h")
    drvWincSocketHeaderFile.setDestPath("driver/winc/include/")
    drvWincSocketHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincSocketHeaderFile.setType("HEADER")
    drvWincSocketHeaderFile.setOverwrite(True)
    #drvWincSocketHeaderFile.setEnabled((wifiDriverMode.getValue() == "Socket_Mode"))
    drvWincSocketHeaderFile.setDependencies(socketModeFileGen, ["WIFI_DRIVER_MODE_CHOICE"])

    # Add wdrv_winc_httpprovctx.h file to project
    drvWincHttpProvisionContextHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_HTTP_PROVISION_H", None)
    drvWincHttpProvisionContextHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_httpprovctx.h")
    drvWincHttpProvisionContextHeaderFile.setOutputName("wdrv_winc_httpprovctx.h")
    drvWincHttpProvisionContextHeaderFile.setDestPath("driver/winc/include/")
    drvWincHttpProvisionContextHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincHttpProvisionContextHeaderFile.setType("HEADER")
    drvWincHttpProvisionContextHeaderFile.setOverwrite(True)
    #drvWincHttpProvisionContextHeaderFile.setEnabled((wifiDriverMode.getValue() == "Socket_Mode"))
    drvWincHttpProvisionContextHeaderFile.setDependencies(socketModeFileGen, ["WIFI_DRIVER_MODE_CHOICE"])

    # Add wdrv_winc_sta.h file to project
    drvWincStaHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_STA_H", None)
    drvWincStaHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_sta.h")
    drvWincStaHeaderFile.setOutputName("wdrv_winc_sta.h")
    drvWincStaHeaderFile.setDestPath("driver/winc/include/")
    drvWincStaHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincStaHeaderFile.setType("HEADER")
    drvWincStaHeaderFile.setOverwrite(True)

    # Add wdrv_winc_systime.h file to project
    drvWincSystemTimeHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_SYATEM_TIME_H", None)
    drvWincSystemTimeHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_systime.h")
    drvWincSystemTimeHeaderFile.setOutputName("wdrv_winc_systime.h")
    drvWincSystemTimeHeaderFile.setDestPath("driver/winc/include/")
    drvWincSystemTimeHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincSystemTimeHeaderFile.setType("HEADER")
    drvWincSystemTimeHeaderFile.setOverwrite(True)

    # Add wdrv_winc_wps.h file to project
    drvWincWpsHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_WPS_H", None)
    drvWincWpsHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_wps.h")
    drvWincWpsHeaderFile.setOutputName("wdrv_winc_wps.h")
    drvWincWpsHeaderFile.setDestPath("driver/winc/include/")
    drvWincWpsHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincWpsHeaderFile.setType("HEADER")
    drvWincWpsHeaderFile.setOverwrite(True)

    # Add wdrv_winc_gpio.h file
    drvWincGpioHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_GPIO_H", None)
    drvWincGpioHeaderFile.setSourcePath("driver/winc/dev/include/wdrv_winc_gpio.h")
    drvWincGpioHeaderFile.setOutputName("wdrv_winc_gpio.h")
    drvWincGpioHeaderFile.setOverwrite(True)
    drvWincGpioHeaderFile.setDestPath("driver/winc/include/dev/")
    drvWincGpioHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/dev/")
    drvWincGpioHeaderFile.setType("HEADER")
    drvWincGpioHeaderFile.setEnabled(True)

    # Add wdrv_winc_spi.h file
    drvWincSpiHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_SPI_H", None)
    drvWincSpiHeaderFile.setSourcePath("driver/winc/dev/include/wdrv_winc_spi.h")
    drvWincSpiHeaderFile.setOutputName("wdrv_winc_spi.h")
    drvWincSpiHeaderFile.setOverwrite(True)
    drvWincSpiHeaderFile.setDestPath("driver/winc/include/dev/")
    drvWincSpiHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/dev/")
    drvWincSpiHeaderFile.setType("HEADER")
    drvWincSpiHeaderFile.setEnabled(True)

    # Add wdrv_winc_stack_drv.h file
    drvWincStackDrvHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_STACK_DRV_H", None)
    drvWincStackDrvHeaderFile.setSourcePath("driver/winc/stack_driver/include/wdrv_winc_stack_drv.h")
    drvWincStackDrvHeaderFile.setOutputName("wdrv_winc_stack_drv.h")
    drvWincStackDrvHeaderFile.setOverwrite(True)
    drvWincStackDrvHeaderFile.setDestPath("driver/winc/include/dev/")
    drvWincStackDrvHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincStackDrvHeaderFile.setType("HEADER")
    #drvWincStackDrvHeaderFile.setEnabled((wifiDriverTcpipMode.getValue() == "True"))
    drvWincStackDrvHeaderFile.setDependencies(tcpipModeFileGen, ["WIFI_DRIVER_TCPIP_MODE"])

    # Add wdrv_winc_pktpool.h file
    drvWincPktpoolHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_PKT_POOL_H", None)
    drvWincPktpoolHeaderFile.setSourcePath("driver/winc/stack_driver/include/wdrv_winc_pktpool.h")
    drvWincPktpoolHeaderFile.setOutputName("wdrv_winc_pktpool.h")
    drvWincPktpoolHeaderFile.setOverwrite(True)
    drvWincPktpoolHeaderFile.setDestPath("driver/winc/include/")
    drvWincPktpoolHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincPktpoolHeaderFile.setType("HEADER")
    #drvWincPktpoolHeaderFile.setEnabled((wifiDriverTcpipMode.getValue() == "True"))
    drvWincPktpoolHeaderFile.setDependencies(tcpipModeFileGen, ["WIFI_DRIVER_TCPIP_MODE"])

    #Add wdrv_winc_iwpriv.h file
    drvWincIwprivlHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_IWPRIVL_H", None)
    drvWincIwprivlHeaderFile.setSourcePath("driver/winc/stack_driver/include/wdrv_winc_iwpriv.h")
    drvWincIwprivlHeaderFile.setOutputName("wdrv_winc_iwpriv.h")
    drvWincIwprivlHeaderFile.setOverwrite(True)
    drvWincIwprivlHeaderFile.setDestPath("driver/winc/include/")
    drvWincIwprivlHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincIwprivlHeaderFile.setType("HEADER")
    #drvWincIwprivlHeaderFile.setEnabled((wifiDriverIwprivMode.getValue() == "True"))
    drvWincIwprivlHeaderFile.setDependencies(iwprivModeFileGen, ["WIFI_DRIVER_IWPRIV_MODE"])

    #Add nm_bsp.h file
    drvWincNmBspHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NM_BSP_H", None)
    drvWincNmBspHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/bsp/include/nm_bsp.h")
    drvWincNmBspHeaderFile.setOutputName("nm_bsp.h")
    drvWincNmBspHeaderFile.setOverwrite(True)
    drvWincNmBspHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/bsp/include/")
    drvWincNmBspHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/bsp/")
    drvWincNmBspHeaderFile.setType("HEADER")
    drvWincNmBspHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add nm_common.h file
    drvWincNmCommonHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NM_COMMON_H", None)
    drvWincNmCommonHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/common/include/nm_common.h")
    drvWincNmCommonHeaderFile.setOutputName("nm_common.h")
    drvWincNmCommonHeaderFile.setOverwrite(True)
    drvWincNmCommonHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/common/")
    drvWincNmCommonHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/common/")
    drvWincNmCommonHeaderFile.setType("HEADER")
    drvWincNmCommonHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add nm_debug.h file
    drvWincNmDebugHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NM_DEBUG_H", None)
    drvWincNmDebugHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/common/include/nm_debug.h")
    drvWincNmDebugHeaderFile.setOutputName("nm_debug.h")
    drvWincNmDebugHeaderFile.setOverwrite(True)
    drvWincNmDebugHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/common/")
    drvWincNmDebugHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/common/")
    drvWincNmDebugHeaderFile.setType("HEADER")
    drvWincNmDebugHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_hif.h file
    drvWincM2mHifHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_HIF_H", None)
    drvWincM2mHifHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/m2m_hif.h")
    drvWincM2mHifHeaderFile.setOutputName("m2m_hif.h")
    drvWincM2mHifHeaderFile.setOverwrite(True)
    drvWincM2mHifHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mHifHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mHifHeaderFile.setType("HEADER")
    drvWincM2mHifHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_periphl.h file
    drvWincM2mPeripheralHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_PERIPHERAL_H", None)
    drvWincM2mPeripheralHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/m2m_periph.h")
    drvWincM2mPeripheralHeaderFile.setOutputName("m2m_periph.h")
    drvWincM2mPeripheralHeaderFile.setOverwrite(True)
    drvWincM2mPeripheralHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mPeripheralHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mPeripheralHeaderFile.setType("HEADER")
    drvWincM2mPeripheralHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_types.h file
    drvWincM2mTypesHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_TYPES_H", None)
    drvWincM2mTypesHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/m2m_types.h")
    drvWincM2mTypesHeaderFile.setOutputName("m2m_types.h")
    drvWincM2mTypesHeaderFile.setOverwrite(True)
    drvWincM2mTypesHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mTypesHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mTypesHeaderFile.setType("HEADER")
    drvWincM2mTypesHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_wifi.h file
    drvWincM2mWifiHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_WIFI_H", None)
    drvWincM2mWifiHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/m2m_wifi.h")
    drvWincM2mWifiHeaderFile.setOutputName("m2m_wifi.h")
    drvWincM2mWifiHeaderFile.setOverwrite(True)
    drvWincM2mWifiHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mWifiHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mWifiHeaderFile.setType("HEADER")
    drvWincM2mWifiHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add nmasic.h file
    drvWincNmAsicHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NMASIC_H", None)
    drvWincNmAsicHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/nmasic.h")
    drvWincNmAsicHeaderFile.setOutputName("nmasic.h")
    drvWincNmAsicHeaderFile.setOverwrite(True)
    drvWincNmAsicHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincNmAsicHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincNmAsicHeaderFile.setType("HEADER")
    drvWincNmAsicHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add nmbus.h file
    drvWincNmBusHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NMBUS_H", None)
    drvWincNmBusHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/nmbus.h")
    drvWincNmBusHeaderFile.setOutputName("nmbus.h")
    drvWincNmBusHeaderFile.setOverwrite(True)
    drvWincNmBusHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincNmBusHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincNmBusHeaderFile.setType("HEADER")
    drvWincNmBusHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add nmdrv.h file
    drvWincNmDrvHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NMDRV_H", None)
    drvWincNmDrvHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/nmdrv.h")
    drvWincNmDrvHeaderFile.setOutputName("nmdrv.h")
    drvWincNmDrvHeaderFile.setOverwrite(True)
    drvWincNmDrvHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincNmDrvHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincNmDrvHeaderFile.setType("HEADER")
    drvWincNmDrvHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"]) 
    
    #Add nmspi.h file
    drvWincNmSpiHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NMSPI_H", None)
    drvWincNmSpiHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/nmspi.h")
    drvWincNmSpiHeaderFile.setOutputName("nmspi.h")
    drvWincNmSpiHeaderFile.setOverwrite(True)
    drvWincNmSpiHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincNmSpiHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincNmSpiHeaderFile.setType("HEADER")
    drvWincNmSpiHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add spi_flash.h file
    drvWincSpiFlashHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_SPI_FLASH_H", None)
    drvWincSpiFlashHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/spi_flash/include/spi_flash.h")
    drvWincSpiFlashHeaderFile.setOutputName("spi_flash.h")
    drvWincSpiFlashHeaderFile.setOverwrite(True)
    drvWincSpiFlashHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/spi_flash/")
    drvWincSpiFlashHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/spi_flash/")
    drvWincSpiFlashHeaderFile.setType("HEADER")
    drvWincSpiFlashHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add flexible_flash.h file
    drvWincFlexibleFlashHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_FLEXIBLE_FLASH_H", None)
    drvWincFlexibleFlashHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/spi_flash/include/flexible_flash.h")
    drvWincFlexibleFlashHeaderFile.setOutputName("flexible_flash.h")
    drvWincFlexibleFlashHeaderFile.setOverwrite(True)
    drvWincFlexibleFlashHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/spi_flash/")
    drvWincFlexibleFlashHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/spi_flash/")
    drvWincFlexibleFlashHeaderFile.setType("HEADER")
    drvWincFlexibleFlashHeaderFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_socket_host_if.h file
    drvWincM2mSocketHostHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_SOCKET_HOST_H", None)
    drvWincM2mSocketHostHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/socket/include/m2m_socket_host_if.h")
    drvWincM2mSocketHostHeaderFile.setOutputName("m2m_socket_host_if.h")
    drvWincM2mSocketHostHeaderFile.setOverwrite(True)
    drvWincM2mSocketHostHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/socket/")
    drvWincM2mSocketHostHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/socket/")
    drvWincM2mSocketHostHeaderFile.setType("HEADER")
    drvWincM2mSocketHostHeaderFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add wdrv_winc_host_file.h file
    drvWincHostHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_HOST_FILE_H", None)
    drvWincHostHeaderFile.setSourcePath("driver/winc/include/wdrv_winc_host_file.h")
    drvWincHostHeaderFile.setOutputName("wdrv_winc_host_file.h")
    drvWincHostHeaderFile.setOverwrite(True)
    drvWincHostHeaderFile.setDestPath("driver/winc/include/")
    drvWincHostHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/")
    drvWincHostHeaderFile.setType("HEADER")
    drvWincHostHeaderFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_ota.h file
    drvWincM2mOtaHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_OTA_H", None)
    drvWincM2mOtaHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/m2m_ota.h")
    drvWincM2mOtaHeaderFile.setOutputName("m2m_ota.h")
    drvWincM2mOtaHeaderFile.setOverwrite(True)
    drvWincM2mOtaHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mOtaHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mOtaHeaderFile.setType("HEADER")
    drvWincM2mOtaHeaderFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_ssl.h file
    drvWincM2mSslHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_SSL_H", None)
    drvWincM2mSslHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/m2m_ssl.h")
    drvWincM2mSslHeaderFile.setOutputName("m2m_ssl.h")
    drvWincM2mSslHeaderFile.setOverwrite(True)
    drvWincM2mSslHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mSslHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincM2mSslHeaderFile.setType("HEADER")
    drvWincM2mSslHeaderFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add ecc_types.h file
    drvWincEccTypesHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_ECC_TYPES_H", None)
    drvWincEccTypesHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/include/ecc_types.h")
    drvWincEccTypesHeaderFile.setOutputName("ecc_types.h")
    drvWincEccTypesHeaderFile.setOverwrite(True)
    drvWincEccTypesHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/driver/")
    drvWincEccTypesHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver/")
    drvWincEccTypesHeaderFile.setType("HEADER")
    drvWincEccTypesHeaderFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add socket.h file
    drvwinc1500_19_SocketHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_SOCKET_VER19_H", None)
    drvwinc1500_19_SocketHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/socket/include/socket.h")
    drvwinc1500_19_SocketHeaderFile.setOutputName("socket.h")
    drvwinc1500_19_SocketHeaderFile.setOverwrite(True)
    drvwinc1500_19_SocketHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/socket/")
    drvwinc1500_19_SocketHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/socket/")
    drvwinc1500_19_SocketHeaderFile.setType("HEADER")
    drvwinc1500_19_SocketHeaderFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add netinet_in.h file 
    drvwincNetInetHeaderFile = drvWincComponent.createFileSymbol("WIFI_WINC_NETINET_H", None)
    drvwincNetInetHeaderFile.setSourcePath("driver/winc/winc1500_19_6_1/socket/include/netinet_in.h")
    drvwincNetInetHeaderFile.setOutputName("netinet_in.h")
    drvwincNetInetHeaderFile.setOverwrite(True)
    drvwincNetInetHeaderFile.setDestPath("driver/winc/include/winc1500_19_6_1/socket/")
    drvwincNetInetHeaderFile.setProjectPath("config/" + configName + "/driver/winc/include/winc1500_19_6_1/socket/")
    drvwincNetInetHeaderFile.setType("HEADER")
    drvwincNetInetHeaderFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    # file WDRV_WINC_C "$HARMONY_VERSION_PATH/framework/driver/winc/src/wdrv_winc.c" to         "$PROJECT_SOURCE_FILES/framework/driver/winc/wdrv_winc.c" 
    # Add wdrv_winc.c file
    drvWincSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_C", None)
    drvWincSourceFile.setSourcePath("driver/winc/wdrv_winc.c")
    drvWincSourceFile.setOutputName("wdrv_winc.c")
    drvWincSourceFile.setOverwrite(True)
    drvWincSourceFile.setDestPath("driver/winc/src/")
    drvWincSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincSourceFile.setType("SOURCE")
    drvWincSourceFile.setEnabled(True)

    # Add wdrv_winc_assoc.c file
    drvWincAssocSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_ASSOC_C", None)
    drvWincAssocSourceFile.setSourcePath("driver/winc/wdrv_winc_assoc.c")
    drvWincAssocSourceFile.setOutputName("wdrv_winc_assoc.c")
    drvWincAssocSourceFile.setOverwrite(True)
    drvWincAssocSourceFile.setDestPath("driver/winc/src/")
    drvWincAssocSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincAssocSourceFile.setType("SOURCE")
    drvWincAssocSourceFile.setEnabled(True)

    # Add wdrv_winc_authctx.c file
    drvWincAuthSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_AUTH_C", None)
    drvWincAuthSourceFile.setSourcePath("driver/winc/wdrv_winc_authctx.c")
    drvWincAuthSourceFile.setOutputName("wdrv_winc_authctx.c")
    drvWincAuthSourceFile.setOverwrite(True)
    drvWincAuthSourceFile.setDestPath("driver/winc/src/")
    drvWincAuthSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincAuthSourceFile.setType("SOURCE")
    drvWincAuthSourceFile.setEnabled(True)

    # Add wdrv_winc_ble.c file
    #drvWincBleSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_BLE_C", None)
    #drvWincBleSourceFile.setSourcePath("driver/winc/wdrv_winc_ble.c")
    #drvWincBleSourceFile.setOutputName("wdrv_winc_ble.c")
    #drvWincBleSourceFile.setOverwrite(True)
    #drvWincBleSourceFile.setDestPath("driver/winc/src/")
    #drvWincBleSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    #drvWincBleSourceFile.setType("SOURCE")
    #drvWincBleSourceFile.setEnabled(True)

    # Add wdrv_winc_bssctx.c file
    drvWincBssSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_BSS_C", None)
    drvWincBssSourceFile.setSourcePath("driver/winc/wdrv_winc_bssctx.c")
    drvWincBssSourceFile.setOutputName("wdrv_winc_bssctx.c")
    drvWincBssSourceFile.setOverwrite(True)
    drvWincBssSourceFile.setDestPath("driver/winc/src/")
    drvWincBssSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincBssSourceFile.setType("SOURCE")
    drvWincBssSourceFile.setEnabled(True)

    # Add wdrv_winc_bssfind.c file
    drvWincBssFindSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_BSS_FIND_C", None)
    drvWincBssFindSourceFile.setSourcePath("driver/winc/wdrv_winc_bssfind.c")
    drvWincBssFindSourceFile.setOutputName("wdrv_winc_bssfind.c")
    drvWincBssFindSourceFile.setOverwrite(True)
    drvWincBssFindSourceFile.setDestPath("driver/winc/src/")
    drvWincBssFindSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincBssFindSourceFile.setType("SOURCE")
    drvWincBssFindSourceFile.setEnabled(True)

    # Add wdrv_winc_custie.c file
    drvWincCustSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_CUST_IE_C", None)
    drvWincCustSourceFile.setSourcePath("driver/winc/wdrv_winc_custie.c")
    drvWincCustSourceFile.setOutputName("wdrv_winc_custie.c")
    drvWincCustSourceFile.setOverwrite(True)
    drvWincCustSourceFile.setDestPath("driver/winc/src/")
    drvWincCustSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincCustSourceFile.setType("SOURCE")
    drvWincCustSourceFile.setEnabled(True)

    # Add wdrv_winc_ethernet.c file
    drvWincEthernetSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_ETHERNET_C", None)
    drvWincEthernetSourceFile.setSourcePath("driver/winc/wdrv_winc_ethernet.c")
    drvWincEthernetSourceFile.setOutputName("wdrv_winc_ethernet.c")
    drvWincEthernetSourceFile.setOverwrite(True)
    drvWincEthernetSourceFile.setDestPath("driver/winc/src/")
    drvWincEthernetSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincEthernetSourceFile.setType("SOURCE")
    drvWincEthernetSourceFile.setEnabled((wifiDriverMode.getValue() == "Ethernet_Mode"))
    drvWincEthernetSourceFile.setDependencies(ethernetModeFileGen, ["WIFI_DRIVER_MODE_CHOICE"])

    # Add wdrv_winc_host_file.c file
    drvWincHostSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_HOST_C", None)
    drvWincHostSourceFile.setSourcePath("driver/winc/wdrv_winc_host_file.c")
    drvWincHostSourceFile.setOutputName("wdrv_winc_host_file.c")
    drvWincHostSourceFile.setOverwrite(True)
    drvWincHostSourceFile.setDestPath("driver/winc/src/")
    drvWincHostSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincHostSourceFile.setType("SOURCE")
    drvWincHostSourceFile.setEnabled(True)

    # Add wdrv_winc_nvm.c file
    drvWincNvmSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_NVM_C", None)
    drvWincNvmSourceFile.setSourcePath("driver/winc/wdrv_winc_nvm.c")
    drvWincNvmSourceFile.setOutputName("wdrv_winc_nvm.c")
    drvWincNvmSourceFile.setOverwrite(True)
    drvWincNvmSourceFile.setDestPath("driver/winc/src/")
    drvWincNvmSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincNvmSourceFile.setType("SOURCE")
    drvWincNvmSourceFile.setEnabled(True)

    # Add wdrv_winc_powersave.c file
    drvWincPowerSaveSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_POWER_SAVE_C", None)
    drvWincPowerSaveSourceFile.setSourcePath("driver/winc/wdrv_winc_powersave.c")
    drvWincPowerSaveSourceFile.setOutputName("wdrv_winc_powersave.c")
    drvWincPowerSaveSourceFile.setOverwrite(True)
    drvWincPowerSaveSourceFile.setDestPath("driver/winc/src/")
    drvWincPowerSaveSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincPowerSaveSourceFile.setType("SOURCE")
    drvWincPowerSaveSourceFile.setEnabled(True)

    # Add wdrv_winc_httpprovctx.c file
    drvWincHttpProvisionSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_HTTP_PROVISION_C", None)
    drvWincHttpProvisionSourceFile.setSourcePath("driver/winc/wdrv_winc_httpprovctx.c")
    drvWincHttpProvisionSourceFile.setOutputName("wdrv_winc_httpprovctx.c")
    drvWincHttpProvisionSourceFile.setOverwrite(True)
    drvWincHttpProvisionSourceFile.setDestPath("driver/winc/src/")
    drvWincHttpProvisionSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincHttpProvisionSourceFile.setType("SOURCE")
    drvWincHttpProvisionSourceFile.setEnabled((wifiDriverMode.getValue() == "Socket_Mode"))
    drvWincHttpProvisionSourceFile.setDependencies(socketModeFileGen, ["WIFI_DRIVER_MODE_CHOICE"])

    # Add wdrv_winc_socket.c file
    drvWincSocketSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_SOCKET_C", None)
    drvWincSocketSourceFile.setSourcePath("driver/winc/wdrv_winc_socket.c")
    drvWincSocketSourceFile.setOutputName("wdrv_winc_socket.c")
    drvWincSocketSourceFile.setOverwrite(True)
    drvWincSocketSourceFile.setDestPath("driver/winc/src/")
    drvWincSocketSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincSocketSourceFile.setType("SOURCE")
    drvWincSocketSourceFile.setEnabled((wifiDriverMode.getValue() == "Socket_Mode"))
    drvWincSocketSourceFile.setDependencies(socketModeFileGen, ["WIFI_DRIVER_MODE_CHOICE"])

    # Add wdrv_winc_ssl.c file
    drvWincSslSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_SSL_C", None)
    drvWincSslSourceFile.setSourcePath("driver/winc/wdrv_winc_ssl.c")
    drvWincSslSourceFile.setOutputName("wdrv_winc_ssl.c")
    drvWincSslSourceFile.setOverwrite(True)
    drvWincSslSourceFile.setDestPath("driver/winc/src/")
    drvWincSslSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincSslSourceFile.setType("SOURCE")
    drvWincSslSourceFile.setEnabled((wifiDriverMode.getValue() == "Socket_Mode"))
    drvWincSslSourceFile.setDependencies(socketModeFileGen, ["WIFI_DRIVER_MODE_CHOICE"])

    # Add wdrv_winc_softap.c file
    drvWincSoftapSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_SOFT_AP_C", None)
    drvWincSoftapSourceFile.setSourcePath("driver/winc/wdrv_winc_softap.c")
    drvWincSoftapSourceFile.setOutputName("wdrv_winc_softap.c")
    drvWincSoftapSourceFile.setOverwrite(True)
    drvWincSoftapSourceFile.setDestPath("driver/winc/src/")
    drvWincSoftapSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincSoftapSourceFile.setType("SOURCE")
    drvWincSoftapSourceFile.setEnabled(True)

    # Add wdrv_winc_sta.c file
    drvWincStaSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_STA_C", None)
    drvWincStaSourceFile.setSourcePath("driver/winc/wdrv_winc_sta.c")
    drvWincStaSourceFile.setOutputName("wdrv_winc_sta.c")
    drvWincStaSourceFile.setOverwrite(True)
    drvWincStaSourceFile.setDestPath("driver/winc/src/")
    drvWincStaSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincStaSourceFile.setType("SOURCE")
    drvWincStaSourceFile.setEnabled(True)

    # Add wdrv_winc_systime.c file
    drvWincSystimeSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_SYSTEM_TIME_C", None)
    drvWincSystimeSourceFile.setSourcePath("driver/winc/wdrv_winc_systime.c")
    drvWincSystimeSourceFile.setOutputName("wdrv_winc_systime.c")
    drvWincSystimeSourceFile.setOverwrite(True)
    drvWincSystimeSourceFile.setDestPath("driver/winc/src/")
    drvWincSystimeSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincSystimeSourceFile.setType("SOURCE")
    drvWincSystimeSourceFile.setEnabled(True)

    # Add wdrv_winc_wps.c file
    drvWincWpsSourceFile = drvWincComponent.createFileSymbol("WIFI_WPS_C", None)
    drvWincWpsSourceFile.setSourcePath("driver/winc/wdrv_winc_wps.c")
    drvWincWpsSourceFile.setOutputName("wdrv_winc_wps.c")
    drvWincWpsSourceFile.setOverwrite(True)
    drvWincWpsSourceFile.setDestPath("driver/winc/src/")
    drvWincWpsSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWincWpsSourceFile.setType("SOURCE")
    drvWincWpsSourceFile.setEnabled(True)

    # Add wdrv_winc_eint.c file
    drvWincEintSourceFile = drvWincComponent.createFileSymbol("WIFI_EINT_C", None)
    drvWincEintSourceFile.setSourcePath("driver/winc/dev/gpio/wdrv_winc_eint.c")
    drvWincEintSourceFile.setOutputName("wdrv_winc_eint.c")
    drvWincEintSourceFile.setOverwrite(True)
    drvWincEintSourceFile.setDestPath("driver/winc/dev/gpio/")
    drvWincEintSourceFile.setProjectPath("config/" + configName + "/driver/winc/dev/gpio/")
    drvWincEintSourceFile.setType("SOURCE")
    drvWincEintSourceFile.setEnabled(True)

    # Add wdrv_winc_gpio.c file
    drvWincGpioSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_GPIO_C", None)
    drvWincGpioSourceFile.setSourcePath("driver/winc/dev/gpio/wdrv_winc_gpio.c")
    drvWincGpioSourceFile.setOutputName("wdrv_winc_gpio.c")
    drvWincGpioSourceFile.setOverwrite(True)
    drvWincGpioSourceFile.setDestPath("driver/winc/dev/gpio")
    drvWincGpioSourceFile.setProjectPath("config/" + configName + "/driver/winc/dev/gpio/")
    drvWincGpioSourceFile.setType("SOURCE")
    drvWincGpioSourceFile.setEnabled(True)

    # Add wdrv_winc_spi.c file
    drvWincSpiSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_SPI_C", None)
    drvWincSpiSourceFile.setSourcePath("driver/winc/dev/spi/wdrv_winc_spi.c")
    drvWincSpiSourceFile.setOutputName("wdrv_winc_spi.c")
    drvWincSpiSourceFile.setOverwrite(True)
    drvWincSpiSourceFile.setDestPath("driver/winc/dev/spi")
    drvWincSpiSourceFile.setProjectPath("config/" + configName + "/driver/winc/dev/spi/")
    drvWincSpiSourceFile.setType("SOURCE")
    drvWincSpiSourceFile.setEnabled(True)

    # Add wdrv_winc_osal.c file
    drvWincOsalSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_OSAL_C", None)
    drvWincOsalSourceFile.setSourcePath("driver/winc/osal/wdrv_winc_osal.c")
    drvWincOsalSourceFile.setOutputName("wdrv_winc_osal.c")
    drvWincOsalSourceFile.setOverwrite(True)
    drvWincOsalSourceFile.setDestPath("driver/winc/osal/")
    drvWincOsalSourceFile.setProjectPath("config/" + configName + "/driver/winc/osal/")
    drvWincOsalSourceFile.setType("SOURCE")
    drvWincOsalSourceFile.setEnabled(True)

    # Add wdrv_winc_stack_drv.c file
    drvWincStackDrvSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_STACK_DRV_C", None)
    drvWincStackDrvSourceFile.setSourcePath("driver/winc/stack_driver/wdrv_winc_stack_drv.c")
    drvWincStackDrvSourceFile.setOutputName("wdrv_winc_stack_drv.c")
    drvWincStackDrvSourceFile.setOverwrite(True)
    drvWincStackDrvSourceFile.setDestPath("driver/winc/src/stack_driver/")
    drvWincStackDrvSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/stack_driver/")
    drvWincStackDrvSourceFile.setType("SOURCE")
    drvWincStackDrvSourceFile.setEnabled((wifiDriverTcpipMode.getValue() == "True"))
    drvWincStackDrvSourceFile.setDependencies(tcpipModeFileGen, ["WIFI_DRIVER_TCPIP_MODE"])

    #Add wdrv_winc_pktpool.c file
    drvWincPktpoolSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_PKT_POOL_C", None)
    drvWincPktpoolSourceFile.setSourcePath("driver/winc/stack_driver/wdrv_winc_pktpool.c")
    drvWincPktpoolSourceFile.setOutputName("wdrv_winc_pktpool.c")
    drvWincPktpoolSourceFile.setOverwrite(True)
    drvWincPktpoolSourceFile.setDestPath("driver/winc/src/stack_driver/")
    drvWincPktpoolSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/stack_driver/")
    drvWincPktpoolSourceFile.setType("SOURCE")
    drvWincPktpoolSourceFile.setEnabled((wifiDriverTcpipMode.getValue() == "True"))
    drvWincPktpoolSourceFile.setDependencies(tcpipModeFileGen, ["WIFI_DRIVER_TCPIP_MODE"])

    #Add wdrv_winc_stack_itf.c file
    drvWincStackItfSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_STACK_ITF_C", None)
    drvWincStackItfSourceFile.setSourcePath("driver/winc/stack_driver/wdrv_winc_stack_itf.c")
    drvWincStackItfSourceFile.setOutputName("wdrv_winc_stack_itf.c")
    drvWincStackItfSourceFile.setOverwrite(True)
    drvWincStackItfSourceFile.setDestPath("driver/winc/src/stack_driver/")
    drvWincStackItfSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/stack_driver/")
    drvWincStackItfSourceFile.setType("SOURCE")
    drvWincStackItfSourceFile.setEnabled((wifiDriverTcpipMode.getValue() == "True"))
    drvWincStackItfSourceFile.setDependencies(tcpipModeFileGen, ["WIFI_DRIVER_TCPIP_MODE"])

    #Add wdrv_winc_ctrl_itf.c file
    drvWincCtrlItfSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_CTRL_ITF_C", None)
    drvWincCtrlItfSourceFile.setSourcePath("driver/winc/stack_driver/wdrv_winc_ctrl_itf.c")
    drvWincCtrlItfSourceFile.setOutputName("wdrv_winc_ctrl_itf.c")
    drvWincCtrlItfSourceFile.setOverwrite(True)
    drvWincCtrlItfSourceFile.setDestPath("driver/winc/src/stack_driver/")
    drvWincCtrlItfSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/stack_driver/")
    drvWincCtrlItfSourceFile.setType("SOURCE")
    drvWincCtrlItfSourceFile.setEnabled((wifiDriverTcpipMode.getValue() == "True"))
    drvWincCtrlItfSourceFile.setDependencies(tcpipModeFileGen, ["WIFI_DRIVER_TCPIP_MODE"])

    #Add wdrv_winc_itf_proxy.c file
    drvWincItfProxySourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_ITF_PROXY_C", None)
    drvWincItfProxySourceFile.setSourcePath("driver/winc/stack_driver/wdrv_winc_itf_proxy.c")
    drvWincItfProxySourceFile.setOutputName("wdrv_winc_itf_proxy.c")
    drvWincItfProxySourceFile.setOverwrite(True)
    drvWincItfProxySourceFile.setDestPath("driver/winc/src/stack_driver/")
    drvWincItfProxySourceFile.setProjectPath("config/" + configName + "/driver/winc/src/stack_driver/")
    drvWincItfProxySourceFile.setType("SOURCE")
    drvWincItfProxySourceFile.setEnabled((wifiDriverTcpipMode.getValue() == "True"))
    drvWincItfProxySourceFile.setDependencies(tcpipModeFileGen, ["WIFI_DRIVER_TCPIP_MODE"])

    # Add wdrv_winc_iwpriv.c file
    drvWinIwprivSourceFile  = drvWincComponent.createFileSymbol("WIFI_WINC_IWPRIV_C", None)
    drvWinIwprivSourceFile .setSourcePath("driver/winc/stack_driver/wdrv_winc_iwpriv.c")
    drvWinIwprivSourceFile .setOutputName("wdrv_winc_iwpriv.c")
    drvWinIwprivSourceFile .setOverwrite(True)
    drvWinIwprivSourceFile .setDestPath("driver/winc/src/stack_driver/")
    drvWinIwprivSourceFile .setProjectPath("config/" + configName + "/driver/winc/src/stack_driver/")
    drvWinIwprivSourceFile .setType("SOURCE")
    drvWinIwprivSourceFile .setEnabled((wifiDriverIwprivMode.getValue() == "True"))
    drvWinIwprivSourceFile .setDependencies(iwprivModeFileGen, ["WIFI_DRIVER_IWPRIV_MODE"])
       
    #Add nm_common.c file
    drvWincNmCommonSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_NM_COMMON_C", None)
    drvWincNmCommonSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/common/source/nm_common.c")
    drvWincNmCommonSourceFile.setOutputName("nm_common.c")
    drvWincNmCommonSourceFile.setOverwrite(True)
    drvWincNmCommonSourceFile.setDestPath("driver/winc/winc1500_19_6_1/common/")
    drvWincNmCommonSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/common/")
    drvWincNmCommonSourceFile.setType("SOURCE")
    drvWincNmCommonSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    
    #Add m2m_hif.c file
    drvWincM2mHifSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_HIF_C", None)
    drvWincM2mHifSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/m2m_hif.c")
    drvWincM2mHifSourceFile.setOutputName("m2m_hif.c")
    drvWincM2mHifSourceFile.setOverwrite(True)
    drvWincM2mHifSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mHifSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mHifSourceFile.setType("SOURCE")
    drvWincM2mHifSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_periphl.c file
    drvWincM2mPeripheralSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_PERIPHERAL_C", None)
    drvWincM2mPeripheralSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/m2m_periph.c")
    drvWincM2mPeripheralSourceFile.setOutputName("m2m_periph.c")
    drvWincM2mPeripheralSourceFile.setOverwrite(True)
    drvWincM2mPeripheralSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mPeripheralSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mPeripheralSourceFile.setType("SOURCE")
    drvWincM2mPeripheralSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
     
    #Add m2m_wifi.c file
    drvWincM2mWifiSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_WIFI_C", None)
    drvWincM2mWifiSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/m2m_wifi.c")
    drvWincM2mWifiSourceFile.setOutputName("m2m_wifi.c")
    drvWincM2mWifiSourceFile.setOverwrite(True)
    drvWincM2mWifiSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mWifiSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mWifiSourceFile.setType("SOURCE")
    drvWincM2mWifiSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add nmasic.c file
    drvWincNmAsicSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_NMASIC_C", None)
    drvWincNmAsicSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/nmasic.c")
    drvWincNmAsicSourceFile.setOutputName("nmasic.c")
    drvWincNmAsicSourceFile.setOverwrite(True)
    drvWincNmAsicSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincNmAsicSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincNmAsicSourceFile.setType("SOURCE")
    drvWincNmAsicSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add nmbus.c file
    drvWincNmBusSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_NMBUS_C", None)
    drvWincNmBusSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/nmbus.c")
    drvWincNmBusSourceFile.setOutputName("nmbus.c")
    drvWincNmBusSourceFile.setOverwrite(True)
    drvWincNmBusSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincNmBusSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincNmBusSourceFile.setType("SOURCE")
    drvWincNmBusSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add nmdrv.c file
    drvWincNmDrvSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_NMDRV_C", None)
    drvWincNmDrvSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/nmdrv.c")
    drvWincNmDrvSourceFile.setOutputName("nmdrv.c")
    drvWincNmDrvSourceFile.setOverwrite(True)
    drvWincNmDrvSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincNmDrvSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincNmDrvSourceFile.setType("SOURCE")
    drvWincNmDrvSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"]) 
    
    #Add nmspi.c file
    drvWincNmSpiSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_NMSPI_C", None)
    drvWincNmSpiSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/nmspi.c")
    drvWincNmSpiSourceFile.setOutputName("nmspi.c")
    drvWincNmSpiSourceFile.setOverwrite(True)
    drvWincNmSpiSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincNmSpiSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincNmSpiSourceFile.setType("SOURCE")
    drvWincNmSpiSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add spi_flash.c file
    drvWincSpiFlashSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_SPI_FLASH_C", None)
    drvWincSpiFlashSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/spi_flash/source/spi_flash.c")
    drvWincSpiFlashSourceFile.setOutputName("spi_flash.c")
    drvWincSpiFlashSourceFile.setOverwrite(True)
    drvWincSpiFlashSourceFile.setDestPath("driver/winc/winc1500_19_6_1/spi_flash/")
    drvWincSpiFlashSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/spi_flash/")
    drvWincSpiFlashSourceFile.setType("SOURCE")
    drvWincSpiFlashSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add flexible_flash.c file
    drvWincFlexibleFlashSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_FLEXIBLE_FLASH_C", None)
    drvWincFlexibleFlashSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/spi_flash/source/flexible_flash.c")
    drvWincFlexibleFlashSourceFile.setOutputName("flexible_flash.c")
    drvWincFlexibleFlashSourceFile.setOverwrite(True)
    drvWincFlexibleFlashSourceFile.setDestPath("driver/winc/winc1500_19_6_1/spi_flash/")
    drvWincFlexibleFlashSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/spi_flash/")
    drvWincFlexibleFlashSourceFile.setType("SOURCE")
    drvWincFlexibleFlashSourceFile.setDependencies(win1500FileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
        
    #Add wdrv_winc_host_file.c file
    drvWinc1500_19_HostSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_HOST_FILE_C", None)
    drvWinc1500_19_HostSourceFile.setSourcePath("driver/winc/wdrv_winc_host_file.c")
    drvWinc1500_19_HostSourceFile.setOutputName("wdrv_winc_host_file.c")
    drvWinc1500_19_HostSourceFile.setOverwrite(True)
    drvWinc1500_19_HostSourceFile.setDestPath("driver/winc/src/")
    drvWinc1500_19_HostSourceFile.setProjectPath("config/" + configName + "/driver/winc/src/")
    drvWinc1500_19_HostSourceFile.setType("SOURCE")
    drvWinc1500_19_HostSourceFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_ota.c file
    drvWincM2mOtaSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_OTA_C", None)
    drvWincM2mOtaSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/m2m_ota.c")
    drvWincM2mOtaSourceFile.setOutputName("m2m_ota.c")
    drvWincM2mOtaSourceFile.setOverwrite(True)
    drvWincM2mOtaSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mOtaSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mOtaSourceFile.setType("SOURCE")
    drvWincM2mOtaSourceFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add m2m_ssl.c file
    drvWincM2mSslSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_M2M_SSL_C", None)
    drvWincM2mSslSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/driver/source/m2m_ssl.c")
    drvWincM2mSslSourceFile.setOutputName("m2m_ssl.c")
    drvWincM2mSslSourceFile.setOverwrite(True)
    drvWincM2mSslSourceFile.setDestPath("driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mSslSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/driver/")
    drvWincM2mSslSourceFile.setType("SOURCE")
    drvWincM2mSslSourceFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])    
      
    #Add socket.c file
    drvwinc1500_19_SocketSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_SOCKET_VER19_C", None)
    drvwinc1500_19_SocketSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/socket/source/socket.c")
    drvwinc1500_19_SocketSourceFile.setOutputName("socket.c")
    drvwinc1500_19_SocketSourceFile.setOverwrite(True)
    drvwinc1500_19_SocketSourceFile.setDestPath("driver/winc/winc1500_19_6_1/socket/")
    drvwinc1500_19_SocketSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/socket/")
    drvwinc1500_19_SocketSourceFile.setType("SOURCE")
    drvwinc1500_19_SocketSourceFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add inet_addr.c file 
    drvwincInetAddrSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_NETINET_C", None)
    drvwincInetAddrSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/socket/source/inet_addr.c") 
    drvwincInetAddrSourceFile.setOutputName("inet_addr.c")
    drvwincInetAddrSourceFile.setOverwrite(True)
    drvwincInetAddrSourceFile.setDestPath("driver/winc/winc1500_19_6_1/socket/")
    drvwincInetAddrSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/socket/")
    drvwincInetAddrSourceFile.setType("SOURCE")
    drvwincInetAddrSourceFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #Add inet_ntop.c file 
    drvwincInetNtopSourceFile = drvWincComponent.createFileSymbol("WIFI_WINC_INET_NTOP_C", None)
    drvwincInetNtopSourceFile.setSourcePath("driver/winc/winc1500_19_6_1/socket/source/inet_ntop.c") 
    drvwincInetNtopSourceFile.setOutputName("inet_ntop.c")
    drvwincInetNtopSourceFile.setOverwrite(True)
    drvwincInetNtopSourceFile.setDestPath("driver/winc/winc1500_19_6_1/socket/")
    drvwincInetNtopSourceFile.setProjectPath("config/" + configName + "/driver/winc/winc1500_19_6_1/socket/")
    drvwincInetNtopSourceFile.setType("SOURCE")
    drvwincInetNtopSourceFile.setDependencies(win1500SocketModeFileGen, ["WIFI_DEVICE_SELECT","WIFI_DRV_VERSION","WIFI_DRIVER_MODE_CHOICE"])
    
    #add include directory 
    wincIncludePath = drvWincComponent.createSettingSymbol("wincIncludePath", None)
    wincIncludePath.setValue("../src/config/" + configName + "/driver/winc/include;../src/config/" + configName + "/driver/winc/include/dev;")
    wincIncludePath.setCategory("C32")
    wincIncludePath.setKey("extra-include-directories")
    wincIncludePath.setAppend(True, ";")
    
    #add include directory 
    wincIncludeDriverPath = drvWincComponent.createSettingSymbol("wincIncludeDriverPath", None)
    wincIncludeDriverPath.setValue("../src/config/" + configName + "/driver/winc/include/winc1500_19_6_1/bsp;../src/config/" + configName + "/driver/winc/include/winc1500_19_6_1/bsp/include;../src/config/" + configName + "/driver/winc/include/winc1500_19_6_1/common;../src/config/" + configName + "/driver/winc/include/winc1500_19_6_1/driver;../src/config/" + configName + "/driver/winc/include/winc1500_19_6_1/socket;../src/config/" + configName + "/driver/winc/include/winc1500_19_6_1/spi_flash;")
    wincIncludeDriverPath.setCategory("C32")
    wincIncludeDriverPath.setKey("extra-include-directories")
    wincIncludeDriverPath.setAppend(True, ";")
    
    drvwincSystemDefFile = drvWincComponent.createFileSymbol("WIFI_H_FILE", None)
    drvwincSystemDefFile.setType("STRING")
    drvwincSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    drvwincSystemDefFile.setSourcePath("driver/winc/templates/system/system_definitions.h.ftl")
    drvwincSystemDefFile.setMarkup(True)	

    drvwincSystemDefObjFile = drvWincComponent.createFileSymbol("DRV_WIFI_DEF_OBJ", None)
    drvwincSystemDefObjFile.setType("STRING")
    drvwincSystemDefObjFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS")
    drvwincSystemDefObjFile.setSourcePath("driver/winc/templates/system/system_definitions_objects.h.ftl")
    drvwincSystemDefObjFile.setMarkup(True)	

    drvwincSystemConfFile = drvWincComponent.createFileSymbol("DRV_WIFI_CONFIGURATION_H", None)
    drvwincSystemConfFile.setType("STRING")
    drvwincSystemConfFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION")
    drvwincSystemConfFile.setSourcePath("driver/winc/templates/system/system_config.h.ftl")
    drvwincSystemConfFile.setMarkup(True)	

    drvwincSystemTaskFile = drvWincComponent.createFileSymbol("DRV_SDHC_SYSTEM_TASKS_C", None)
    drvwincSystemTaskFile.setType("STRING")
    drvwincSystemTaskFile.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS")
    drvwincSystemTaskFile.setSourcePath("driver/winc/templates/system/system_tasks.c.ftl")
    drvwincSystemTaskFile.setMarkup(True)

    drvwincSystemRtosTasksFile = drvWincComponent.createFileSymbol("DRV_SDHC_SYS_RTOS_TASK", None)
    drvwincSystemRtosTasksFile.setType("STRING")
    drvwincSystemRtosTasksFile.setOutputName("core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS")
    drvwincSystemRtosTasksFile.setSourcePath("driver/winc/templates/system/system_rtos_tasks.c.ftl")
    drvwincSystemRtosTasksFile.setMarkup(True)
    drvwincSystemRtosTasksFile.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))
    drvwincSystemRtosTasksFile.setDependencies(genRtosTask, ["HarmonyCore.SELECT_RTOS"])


def drvWincshowRTOSMenu(symbol, event):

    if (event["value"] == None):
        symbol.setVisiSoftap(False)
        print("WINC: OSAL DisaSoftapd")
    elif (event["value"] != "BareMetal"):
        # If not Bare Metal
        symbol.setVisiSoftap(True)
        print("WINC rtos")
    else:
        symbol.setVisiSoftap(False)
        print("WINC Bare Metal")

def drvWifiRTOSStandaloneMenu(symbol, event):
    if (event["value"] == "Standalone"):
        symbol.setVisible(True)
        print("WIFI Standalone")
    else:
        symbol.setVisible(False)
        print("WIFI Combined")

def drvWifiRTOSTaskDelayMenu(symbol, event):
    drvWifiRtos = symbol.getComponent().getSymbolByID(event["id"])
    #drvWifiRtosUseDelay = Database.getSymbolValue("drvWinc","DRV_WIFI_RTOS_USE_DELAY")
    if(drvWifiRtos.getValue() == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def genRtosTask(symbol, event):
    symbol.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))

def drvWifiGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def SocketModeVisibility(symbol, event):
    id = symbol.getID()[-1]

    socketModeVis = symbol.getComponent().getSymbolByID(event["id"])

    if(socketModeVis.getValue() == "Ethernet_Mode"):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def IwprivModeVisibility(symbol, event):
    id = symbol.getID()[-1]

    IwprivModeVis = symbol.getComponent().getSymbolByID(event["id"])

    if(IwprivModeVis.getValue() == True):
        symbol.setVisible(True)
    else :
        symbol.setVisible(False)
def InterruptModeVisibility(symbol, event):
    id = symbol.getID()[-1]

    InterruptVis = symbol.getComponent().getSymbolByID(event["id"])
    if(InterruptVis.getValue() == "External_Interrurpt"):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def socketModeFileGen(symbol, event):
    if event["value"] == "Socket_Mode":
       symbol.setEnabled(True)
    else:
       symbol.setEnabled(False)

def ethernetModeFileGen(symbol, event):
    if event["value"] == "Ethernet_Mode":
       symbol.setEnabled(True)
    else:
       symbol.setEnabled(False)

def tcpipModeFileGen(symbol, event):
    if event["value"] == "True":
       symbol.setEnabled(True)
    else:
       symbol.setEnabled(False)

def iwprivModeFileGen(symbol, event):
    if event["value"] == "True":
       symbol.setEnabled(True)
    else:
       symbol.setEnabled(False)

def win1500FileGen(symbol, event):
    component = symbol.getComponent()

    wifi_device = component.getSymbolValue("WIFI_DEVICE_SELECT")
    drv_version = component.getSymbolValue("WIFI_DRV_VERSION")
    if ((wifi_device == "WINC1500") and (drv_version == "19.6.1")):
       symbol.setEnabled(True)
    else:
       symbol.setEnabled(False)

def win1500SocketModeFileGen(symbol, event):
    component = symbol.getComponent()

    wifi_device = component.getSymbolValue("WIFI_DEVICE_SELECT")
    drv_version = component.getSymbolValue("WIFI_DRV_VERSION")
    drv_mode    = component.getSymbolValue("WIFI_DRIVER_MODE_CHOICE")
    if ((wifi_device == "WINC1500") and (drv_version == "19.6.1") and (drv_mode == "Socket_Mode")):
       symbol.setEnabled(True)
    else:
       symbol.setEnabled(False)
