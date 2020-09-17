# coding: utf-8
"""*****************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************"""

################################################################################
#### Global Variables ####
################################################################################

################################################################################
#### Business Logic ####
################################################################################

################################################################################
#### Component ####
################################################################################
def instantiateComponent(syswifiprovComponent):

    res = Database.activateComponents(["HarmonyCore"])

    # Enable dependent Harmony core components
    if (Database.getSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON", True)

    if (Database.getSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON", True)


    syswifiprovNvmAdd = syswifiprovComponent.createStringSymbol("SYS_WIFIPROV_NVMADDR", None)
    syswifiprovNvmAdd.setLabel("WiFi Configuration Stored At NVM Address")
    syswifiprovNvmAdd.setVisible(True)
    syswifiprovNvmAdd.setDescription("Enter 4KB Aligned NVM Address for storing WiFi Configuration")
    syswifiprovNvmAdd.setDefaultValue("0x900F0000")

    syswifiprovstaEnable = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_STA_ENABLE", None)
    syswifiprovstaEnable.setVisible(False)
    syswifiprovstaEnable.setDefaultValue((Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_STA_ENABLE") == True))
    syswifiprovstaEnable.setDependencies(syswifiprovCustomSet, ["sysWifiPic32mzw1.SYS_WIFI_STA_ENABLE"])

    syswifiprovapEnable = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_AP_ENABLE", None)
    syswifiprovapEnable.setVisible(False)
    syswifiprovapEnable.setDefaultValue((Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_AP_ENABLE") == True))
    syswifiprovapEnable.setDependencies(syswifiprovCustomSet, ["sysWifiPic32mzw1.SYS_WIFI_AP_ENABLE"])

    syswifiprovdebugEnable = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_APPDEBUG_ENABLE", None)
    syswifiprovdebugEnable.setVisible(False)
    syswifiprovdebugEnable.setDefaultValue((Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_APPDEBUG_ENABLE") == True))
    syswifiprovdebugEnable.setDependencies(syswifiprovCustomSet, ["sysWifiPic32mzw1.SYS_WIFI_APPDEBUG_ENABLE"])
    syswifiprovdebugEnable.setDependencies(syswifiprovCustomSet, ["sysWifiPic32mzw1.SYS_WIFI_PROVISION_ENABLE"])

    syswifiprovSave = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_SAVECONFIG", None)
    syswifiprovSave.setLabel("Save Configuration in NVM")
    syswifiprovSave.setDefaultValue(True)
    syswifiprovSave.setDescription("Enable Option to store Wi-Fi Configuration to NVM")
    syswifiprovSave.setDependencies(syswifiprovMenuVisible, ["SYS_WIFIPROV_ENABLE"])

    syswifiprovMethod = syswifiprovComponent.createMenuSymbol("SYS_WIFIPROV_METHOD", None)
    syswifiprovMethod.setLabel("WiFi Provisioning Methods")
    syswifiprovMethod.setVisible(True)

    syswifiprovcmd = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_CMD", syswifiprovMethod)
    syswifiprovcmd.setLabel("Command Line (CLI) ")
    syswifiprovcmd.setDefaultValue(True)
    syswifiprovcmd.setDescription("Enable WiFi Provisioning via CLI")
    syswifiprovcmd.setDependencies(syswifiprovMenuVisible, ["SYS_WIFIPROV_ENABLE"])

    syswifiprovhttp = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_HTTP", syswifiprovMethod)
    syswifiprovhttp.setLabel("HTTP")
    syswifiprovhttp.setVisible(True)
    syswifiprovhttp.setDescription("Enable WiFi Provisioning via HTTP")
    syswifiprovhttp.setDefaultValue(False)
    syswifiprovhttp.setDependencies(syswifiprovMenuVisible, ["SYS_WIFIPROV_ENABLE"])
    syswifiprovhttp.setDependencies(syswifiprovHTTPMenuVisible, ["SYS_WIFIPROV_HTTP"])

    syswifiprovhttpnetenb = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_ENABLE_HTTPNET", syswifiprovhttp)
    syswifiprovhttpnetenb.setLabel("Enable HTTPNET")
    syswifiprovhttpnetenb.setVisible(True)
    syswifiprovhttpnetenb.setDescription("Defult HTTP enabled,Enable HTTPNET")
    syswifiprovhttpnetenb.setDefaultValue(False)
    #syswifiprovhttpnetenb.setDependencies(syswifiprovHTTPMenuVisible, ["SYS_WIFIPROV_HTTP"])

    syswifiprovhttptype = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_HTTP_SECURE", syswifiprovhttpnetenb)
    syswifiprovhttptype.setLabel("Enable Secure Connection with HTTPNET")
    syswifiprovhttptype.setVisible(True)
    syswifiprovhttptype.setDescription("HTTP - support only NonSecure and HTTPNET - support both Secure and NonSecure")
    syswifiprovhttptype.setDefaultValue(False)
    #syswifiprovhttptype.setDependencies(syswifiprovHTTPMenuVisible, ["SYS_WIFIPROV_HTTP"])
    
    syswifiprovhttpPort = syswifiprovComponent.createIntegerSymbol("SYS_WIFIPROV_HTTPPORT", syswifiprovhttpnetenb)
    syswifiprovhttpPort.setLabel("Server Port")
    syswifiprovhttpPort.setMin(1)
    syswifiprovhttpPort.setMax(65535)
    syswifiprovhttpPort.setDefaultValue(80)
    #syswifiprovhttpPort.setDependencies(syswifiprovMenuVisible, ["SYS_WIFIPROV_HTTP"])


    syswifiprovsocket = syswifiprovComponent.createBooleanSymbol("SYS_WIFIPROV_SOCKET", syswifiprovMethod)
    syswifiprovsocket.setLabel("TCP Socket ")
    syswifiprovsocket.setDefaultValue(True)
    syswifiprovsocket.setDescription("Enable WiFi Provisioning via SOcket")
    syswifiprovsocket.setDependencies(syswifiprovMenuVisible, ["SYS_WIFIPROV_ENABLE"])

    syswifiprovsocketPort = syswifiprovComponent.createIntegerSymbol("SYS_WIFIPROV_SOCKETPORT", syswifiprovsocket)
    syswifiprovsocketPort.setLabel("Socket Server Port")
    syswifiprovsocketPort.setMin(1)
    syswifiprovsocketPort.setMax(65535)
    syswifiprovsocketPort.setDefaultValue(6666)
    syswifiprovsocketPort.setDescription("Enter TCP Server SOcket Port Number ")
    syswifiprovsocketPort.setDependencies(syswifiprovMenuVisible, ["SYS_WIFIPROV_SOCKET"])

    if(Database.getSymbolValue("tcpipHttp", "TCPIP_HTTP_CUSTOM_TEMPLATE_SL") == True):
       Database.setSymbolValue("tcpipHttp", "TCPIP_HTTP_CUSTOM_TEMPLATE_SL", False)
    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")


    syswifiprovSystemConfFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_CONFIGURATION_H", None)
    syswifiprovSystemConfFile.setType("STRING")
    syswifiprovSystemConfFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    syswifiprovSystemConfFile.setSourcePath("system/wifiprov/templates/system/system_config.h.ftl")
    syswifiprovSystemConfFile.setMarkup(True)
    syswifiprovSystemConfFile.setDependencies(syswifiprovGenSourceFile, ["SYS_WIFIPROV_ENABLE"])

    syswifiprovHeaderFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_SOURCE", None)
    syswifiprovHeaderFile.setSourcePath("system/wifiprov/templates/src/sys_wifiprov.c.ftl")
    syswifiprovHeaderFile.setOutputName("sys_wifiprov.c")
    syswifiprovHeaderFile.setDestPath("system/wifiprov/src")
    syswifiprovHeaderFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovHeaderFile.setType("SOURCE")
    syswifiprovHeaderFile.setMarkup(True)
    syswifiprovHeaderFile.setEnabled(True)
    syswifiprovHeaderFile.setDependencies(syswifiprovGenSourceFile, ["SYS_WIFIPROV_ENABLE"])

    syswifiprovSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_HEADER", None)
    syswifiprovSourceFile.setSourcePath("system/wifiprov/templates/sys_wifiprov.h.ftl")
    syswifiprovSourceFile.setOutputName("sys_wifiprov.h")
    syswifiprovSourceFile.setDestPath("system/wifiprov/")
    syswifiprovSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovSourceFile.setType("HEADER")
    syswifiprovSourceFile.setMarkup(True)
    syswifiprovSourceFile.setEnabled(True)
    syswifiprovSourceFile.setDependencies(syswifiprovGenSourceFile, ["SYS_WIFIPROV_ENABLE"])

    syswifiprovcjsonHeaderFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_JSON_SOURCE", None)
    syswifiprovcjsonHeaderFile.setSourcePath("system/wifiprov/templates/src/sys_wifiprov_json.c")
    syswifiprovcjsonHeaderFile.setOutputName("sys_wifiprov_json.c")
    syswifiprovcjsonHeaderFile.setDestPath("system/wifiprov/src")
    syswifiprovcjsonHeaderFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovcjsonHeaderFile.setType("SOURCE")
    syswifiprovcjsonHeaderFile.setMarkup(True)
    syswifiprovcjsonHeaderFile.setEnabled(True)
    syswifiprovcjsonHeaderFile.setDependencies(syswifiprovGenSourceFile, ["SYS_WIFIPROV_SOCKET"])

    syswifiprovcjsonSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_JSON_HEADER", None)
    syswifiprovcjsonSourceFile.setSourcePath("system/wifiprov/templates/sys_wifiprov_json.h")
    syswifiprovcjsonSourceFile.setOutputName("sys_wifiprov_json.h")
    syswifiprovcjsonSourceFile.setDestPath("system/wifiprov/")
    syswifiprovcjsonSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovcjsonSourceFile.setType("HEADER")
    syswifiprovcjsonSourceFile.setMarkup(True)
    syswifiprovcjsonSourceFile.setEnabled(True)
    syswifiprovcjsonSourceFile.setDependencies(syswifiprovGenSourceFile, ["SYS_WIFIPROV_SOCKET"])


    syswifiprovcustomhttpSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_CUSTOM_HTTP_SOURCE", None)
    syswifiprovcustomhttpSourceFile.setSourcePath("system/wifiprov/templates/src/custom_http_app.c.ftl")
    syswifiprovcustomhttpSourceFile.setOutputName("custom_http_app.c")
    syswifiprovcustomhttpSourceFile.setDestPath("system/wifiprov/")
    syswifiprovcustomhttpSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovcustomhttpSourceFile.setType("SOURCE")
    syswifiprovcustomhttpSourceFile.setMarkup(True)
    syswifiprovcustomhttpSourceFile.setEnabled(False)
    syswifiprovcustomhttpSourceFile.setDependencies(syswifiprovHTTPGenSourceFile, ["SYS_WIFIPROV_HTTP"])


    syswifiprovhttppSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_HTTP_SOURCE", None)
    syswifiprovhttppSourceFile.setSourcePath("system/wifiprov/templates/src/http_print.c")
    syswifiprovhttppSourceFile.setOutputName("http_print.c")
    syswifiprovhttppSourceFile.setDestPath("system/wifiprov/")
    syswifiprovhttppSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovhttppSourceFile.setType("SOURCE")
    syswifiprovhttppSourceFile.setMarkup(True)
    syswifiprovhttppSourceFile.setEnabled(False)
    syswifiprovhttppSourceFile.setDependencies(syswifiprovHTTPGenSourceFile, ["SYS_WIFIPROV_HTTP"])
	
    syswifiprovmpfsSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_MPFS_SOURCE", None)
    syswifiprovmpfsSourceFile.setSourcePath("system/wifiprov/templates/src/mpfs_img2.c")
    syswifiprovmpfsSourceFile.setOutputName("mpfs_img2.c")
    syswifiprovmpfsSourceFile.setDestPath("system/wifiprov/")
    syswifiprovmpfsSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovmpfsSourceFile.setType("SOURCE")
    syswifiprovmpfsSourceFile.setMarkup(True)
    syswifiprovmpfsSourceFile.setEnabled(False)
    syswifiprovmpfsSourceFile.setDependencies(syswifiprovHTTPGenSourceFile, ["SYS_WIFIPROV_HTTP"])

    syswifiprovcustomhttpnetSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_CUSTOM_HTTP_NET_SOURCE", None)
    syswifiprovcustomhttpnetSourceFile.setSourcePath("system/wifiprov/templates/src/custom_http_net_app.c.ftl")
    syswifiprovcustomhttpnetSourceFile.setOutputName("custom_http_net_app.c")
    syswifiprovcustomhttpnetSourceFile.setDestPath("system/wifiprov/")
    syswifiprovcustomhttpnetSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovcustomhttpnetSourceFile.setType("SOURCE")
    syswifiprovcustomhttpnetSourceFile.setMarkup(True)
    syswifiprovcustomhttpnetSourceFile.setEnabled(False)
    syswifiprovcustomhttpnetSourceFile.setDependencies(syswifiprovHTTPNETGenSourceFile, ["SYS_WIFIPROV_HTTP"])


    syswifiprovhttppnetSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_HTTP_NET_SOURCE", None)
    syswifiprovhttppnetSourceFile.setSourcePath("system/wifiprov/templates/src/http_net_print.c")
    syswifiprovhttppnetSourceFile.setOutputName("http_net_print.c")
    syswifiprovhttppnetSourceFile.setDestPath("system/wifiprov/")
    syswifiprovhttppnetSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovhttppnetSourceFile.setType("SOURCE")
    syswifiprovhttppnetSourceFile.setMarkup(True)
    syswifiprovhttppnetSourceFile.setEnabled(False)
    syswifiprovhttppnetSourceFile.setDependencies(syswifiprovHTTPNETGenSourceFile, ["SYS_WIFIPROV_HTTP"])

    syswifiprovhttppnetSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_HTTP_NET_HEADER", None)
    syswifiprovhttppnetSourceFile.setSourcePath("system/wifiprov/templates/http_net_print.h")
    syswifiprovhttppnetSourceFile.setOutputName("http_net_print.h")
    syswifiprovhttppnetSourceFile.setDestPath("system/wifiprov/")
    syswifiprovhttppnetSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovhttppnetSourceFile.setType("HEADER")
    syswifiprovhttppnetSourceFile.setMarkup(True)
    syswifiprovhttppnetSourceFile.setEnabled(False)
    syswifiprovhttppnetSourceFile.setDependencies(syswifiprovHTTPNETGenSourceFile, ["SYS_WIFIPROV_HTTP"])

    syswifiprovmpfsnetSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_MPFS_NET_SOURCE", None)
    syswifiprovmpfsnetSourceFile.setSourcePath("system/wifiprov/templates/src/mpfs_net_img.c")
    syswifiprovmpfsnetSourceFile.setOutputName("mpfs_net_img.c")
    syswifiprovmpfsnetSourceFile.setDestPath("system/wifiprov/")
    syswifiprovmpfsnetSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovmpfsnetSourceFile.setType("SOURCE")
    syswifiprovmpfsnetSourceFile.setMarkup(True)
    syswifiprovmpfsnetSourceFile.setEnabled(False)
    syswifiprovmpfsnetSourceFile.setDependencies(syswifiprovHTTPNETGenSourceFile, ["SYS_WIFIPROV_HTTP"])
############################################################################
#### Dependency ####
############################################################################

def onAttachmentConnected(source, target):
    localComponent = source["component"]
    remoteComponent = target["component"]
    remoteID = remoteComponent.getID()
    connectID = source["id"]
    targetID = target["id"]

    if (connectID == "sys_wifiprov_WIFI_dependency"):
        plibUsed = localComponent.getSymbolByID("SYS_WIFI_SYS")
        plibUsed.clearValue()
        plibUsed.setValue(remoteID.upper())

def syswifiprovCustomSet(symbol, event):
    symbol.clearValue()
    if (event["value"] == True):
        symbol.setValue(True,2)
    else:
        symbol.setValue(False,2)
        

def onAttachmentDisconnected(source, target):
    localComponent = source["component"]
    remoteComponent = target["component"]
    remoteID = remoteComponent.getID()
    connectID = source["id"]
    targetID = target["id"]

def syswifiprovMenuVisible(symbol, event):
    if (event["value"] == True):
        print("WiFi Provisioning Menu Visible.")
        symbol.setVisible(True)
    else:
        print("WiFi Provisioning Menu Invisible.")
        symbol.setVisible(False)

def enableTcpipAutoConfigApps(enable):

    if(enable == True):
        tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
        if (tcpipAutoConfigAppsGroup == None):
            tcpipAutoConfigAppsGroup = Database.createGroup("TCP/IP STACK", "APPLICATION LAYER")
            
        tcpipAutoConfigTransportGroup = Database.findGroup("TRANSPORT LAYER")
        if (tcpipAutoConfigTransportGroup == None):
            tcpipAutoConfigTransportGroup = Database.createGroup("TCP/IP STACK", "TRANSPORT LAYER")

        tcpipAutoConfigNetworkGroup = Database.findGroup("NETWORK LAYER")
        if (tcpipAutoConfigNetworkGroup == None):
            tcpipAutoConfigNetworkGroup = Database.createGroup("TCP/IP STACK", "NETWORK LAYER")
            
        tcpipAutoConfigDriverGroup = Database.findGroup("DRIVER LAYER")
        if (tcpipAutoConfigDriverGroup == None):
            tcpipAutoConfigDriverGroup = Database.createGroup("TCP/IP STACK", "DRIVER LAYER")

        tcpipAutoConfigBasicGroup = Database.findGroup("BASIC CONFIGURATION")
        if (tcpipAutoConfigBasicGroup == None):
            tcpipAutoConfigBasicGroup = Database.createGroup("TCP/IP STACK", "BASIC CONFIGURATION")
            
        if(Database.getComponentByID("tcpip_transport_config") == None):
            res = tcpipAutoConfigTransportGroup.addComponent("tcpip_transport_config")
            res = Database.activateComponents(["tcpip_transport_config"], "TRANSPORT LAYER", False) 
        
        if(Database.getComponentByID("tcpip_network_config") == None):
            res = tcpipAutoConfigNetworkGroup.addComponent("tcpip_network_config")
            res = Database.activateComponents(["tcpip_network_config"], "NETWORK LAYER", False) 
            
        if(Database.getComponentByID("tcpip_driver_config") == None):
            res = tcpipAutoConfigDriverGroup.addComponent("tcpip_driver_config")
            res = Database.activateComponents(["tcpip_driver_config"], "DRIVER LAYER", False)       
            
        if(Database.getComponentByID("tcpip_basic_config") == None):
            res = tcpipAutoConfigBasicGroup.addComponent("tcpip_basic_config")
            res = Database.activateComponents(["tcpip_basic_config"], "BASIC CONFIGURATION", False)         
            
        if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK") != True):
            Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_STACK", True, 2)
            
        if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG") != True):
            Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_NETCONFIG", True, 2)

        if(Database.getComponentByID("drv_memory") != None):
           Database.setSymbolValue("drv_memory_0", "DRV_MEMORY_DEVICE_TYPE", "SYS_FS_MEDIA_TYPE_NVM")

def syswifiprovGenSourceFile(sourceFile, event):
    sourceFile.setEnabled(event["value"])

def syswifiprovHTTPGenSourceFile(sourceFile, event):
    if((event["value"] == True) and (Database.getSymbolValue("sysWifiProvPic32mzw1", "SYS_WIFIPROV_ENABLE_HTTPNET") == False)):
      sourceFile.setEnabled(True)
    else:
      sourceFile.setEnabled(False)

def syswifiprovHTTPNETGenSourceFile(sourceFile, event):
    if((event["value"] == True) and (Database.getSymbolValue("sysWifiProvPic32mzw1", "SYS_WIFIPROV_ENABLE_HTTPNET") == True)):
      sourceFile.setEnabled(True)
    else:
      sourceFile.setEnabled(False)

def syswifiprovHTTPMenuVisible(symbol, event):
    syswifiprovHTTPAutoConfigGroup = Database.findGroup("System Component")
    syswifiprovtcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    syswifiprovtcpipAutoConfigStackGroup = Database.findGroup("TCP/IP STACK")
    enableTcpipAutoConfigApps(True)
    
    if (event["value"] == True):
        if(Database.getComponentByID("sys_fs") == None):    
            res = Database.activateComponents(["sys_fs"],"System Component", True)
            syswifiprovHTTPAutoConfigGroup.setAttachmentVisible("sys_fs", "SYS_FS")
            Database.setSymbolValue("sys_fs", "SYS_FS_FAT", False)
            Database.setSymbolValue("sys_fs", "SYS_FS_MPFS", True)
            Database.setSymbolValue("sys_fs", "SYS_FS_MAX_FILES", 10)
        ###HTTP
        if(Database.getSymbolValue("sysWifiProvPic32mzw1", "SYS_WIFIPROV_ENABLE_HTTPNET") == False and (Database.getComponentByID("tcpipHttp") == None)):
            if (Database.getComponentByID("tcpipHttpNet") != None):
              res = Database.deactivateComponents(["tcpipSntp"])
              res = Database.deactivateComponents(["netPres"])
              res = Database.deactivateComponents(["lib_wolfssl"])
              res = Database.deactivateComponents(["tcpipHttpNet"])
            res = Database.activateComponents(["tcpipHttp"],"APPLICATION LAYER", False) 
            syswifiprovtcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttp", "libtcpipHttp")
            if(Database.getSymbolValue("tcpipHttp", "TCPIP_HTTP_CUSTOM_TEMPLATE_SL") == True):
               Database.setSymbolValue("tcpipHttp", "TCPIP_HTTP_CUSTOM_TEMPLATE", False)
            if(Database.getSymbolValue("tcpipHttp", "TCPIP_HTTP_CUSTOM_TEMPLATE_SL") == True):
               Database.setSymbolValue("tcpipHttp", "TCPIP_HTTP_CUSTOM_TEMPLATE_SL", False)
            if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
               Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True, 2)
        
        ###HTTPNET
        if((Database.getSymbolValue("sysWifiProvPic32mzw1", "SYS_WIFIPROV_ENABLE_HTTPNET") == True) and Database.getComponentByID("tcpipHttpNet") == None):
          if (Database.getComponentByID("tcpipHttp") != None):
            res = Database.deactivateComponents(["tcpipHttp"])
          if(Database.getComponentByID("tcpipHttpNet") == None):
            syswifiprovtcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
            res = Database.activateComponents(["tcpipHttpNet"],"APPLICATION LAYER", True)
            Database.setSymbolValue("tcpipHttpNet", "TCPIP_HTTP_NET_MAX_DATA_LEN", 200)
            #Database.setSymbolValue("tcpipHttpNet", "TCPIP_HTTP_NET_LISTEN_PORT", Database.getSymbolValue("sysWifiProvPic32mzw1", "SYS_WIFIPROV_HTTPPORT"))
            syswifiprovtcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipHttpNet", "libtcpipHttpNet")
            #Database.setSymbolValue("tcpipHttpNet", "TCPIP_HTTP_NET_CUSTOM_TEMPLATE", False)
            if(Database.getComponentByID("netPres") == None):
              syswifiprovHTTPAutoConfigGroup = Database.findGroup("System Component")
              res = Database.activateComponents(["netPres"],"System Component", True)
            if(Database.getComponentByID("lib_wolfssl") == None):
              res = Database.activateComponents(["lib_wolfssl"],"System Component", True)
              autoConnectTableWolfSslNetPres = [["lib_wolfssl", "lib_wolfssl", "netPres_0","NetPres_Crypto_Dependency"]]
              autoConnectTableWolfCryptWolfSsl = [["lib_wolfssl", "WolfSSL_Crypto_Dependency", "lib_wolfcrypt","lib_wolfcrypt"]]
              res = Database.connectDependencies(autoConnectTableWolfCryptWolfSsl)
              res = Database.connectDependencies(autoConnectTableWolfSslNetPres)
            Database.setSymbolValue("netPres", "NET_PRES_CERT_STORE_STUBS_SERVER", False)
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_CERT", 0)
            Database.setSymbolValue("netPres_0", "NET_PRES_SUPPORT_ENCRYPTION0", True)
            Database.setSymbolValue("netPres_0", "NET_PRES_ENC_PROVIDE_IDX0", 0)
            Database.setSymbolValue("netPres_0", "NET_PRES_SUPPORT_SERVER_ENC_IDX0", True)
            Database.setSymbolValue("netPres", "NET_PRES_RTOS_STACK_SIZE", 20480)
            Database.setSymbolValue("netPres", "NET_PRES_CERT_STORE_STUBS_CLIENT", False)
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_CLIENT_CERT_FILENAME", "wolfssl/certs_test.h")
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_CLIENT_CERT_VARIABLE", "client_cert_der_2048")
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE", "sizeof_client_cert_der_2048")
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_SUPPORT", True)
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_CERT_FILENAME", "wolfssl/certs_test.h")
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_CERT_VARIABLE", "server_cert_der_2048")
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE", "sizeof_server_cert_der_2048")
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_KEY_FILENAME", "wolfssl/certs_test.h")
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_KEY_VARIABLE", "server_key_der_2048")
            Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE", "sizeof_server_key_der_2048")
            if(Database.getComponentByID("tcpipSntp") == None):
               res = Database.activateComponents(["tcpipSntp"],"APPLICATION LAYER", True)
            autoConnectTableWolfSslNetPres = [["lib_wolfssl", "TLS Provider", "NetPres_Crypto_Dependency","TLS Provider"]]
            autoConnectTableWolfCryptWolfSsl = [["lib_wolfcrypt", "LIB_WOLFCRYPT", "WolfSSL_Crypto_Dependency","LIB_WOLFCRYPT"]]
            res = Database.connectDependencies(autoConnectTableWolfSslNetPres)
            res = Database.connectDependencies(autoConnectTableWolfCryptWolfSsl)

        if(Database.getSymbolValue("sysWifiProvPic32mzw1", "SYS_WIFIPROV_HTTP_SECURE") == True):
          Database.setSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON", True)
          Database.setSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF", False)
          Database.setSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT", False)
        else:
          Database.setSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_OFF", True)
          Database.setSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_DEFAULT", False)
          Database.setSymbolValue("tcpipHttpNet","TCPIP_HTTP_NET_CONFIG_FLAG_SECURE_ON", False)
        Database.setSymbolValue("tcpipHttpNet", "TCPIP_HTTP_NET_LISTEN_PORT", Database.getSymbolValue("sysWifiProvPic32mzw1", "SYS_WIFIPROV_HTTPPORT"))
        Database.setSymbolValue("tcpipHttpNet", "TCPIP_HTTP_NET_CUSTOM_TEMPLATE", False)
        Database.setSymbolValue("tcpipHttpNet", "TCPIP_HTTP_NET_CUSTOM_TEMPLATE_SL", False)
        Database.setSymbolValue("tcpipHttpNet", "TCPIP_HTTP_NET_WEBPAGE_DIRECTORY_PATH", Module.getPath()+"system/wifiprov/web_pages_net")
        if(Database.getComponentByID("drv_memory") == None):
          res = Database.activateComponents(["drv_memory"],"System Component", True)
          autoConnectTableconmem = [["drv_memory_0", "drv_memory_MEMORY_dependency", "nvm","NVM_MEMORY"]]
          autoConnectTableconfs = [["drv_memory_0", "drv_media", "sys_fs","sys_fs_DRV_MEDIA_dependency"]]
          res = Database.connectDependencies(autoConnectTableconmem)
          res = Database.connectDependencies(autoConnectTableconfs)
          Database.setSymbolValue("drv_memory_0", "DRV_MEMORY_DEVICE_TYPE", "SYS_FS_MEDIA_TYPE_NVM")
          Database.setSymbolValue("nvm", "MEMORY_MEDIA_SIZE", 64)
          Database.setSymbolValue("nvm", "START_ADDRESS", "90010000")
        symbol.setValue(True,2)
    else:
        res = Database.deactivateComponents(["drv_memory"])
        res = Database.deactivateComponents(["sys_fs"])
        if (Database.getComponentByID("tcpipHttp") != None):
          res = Database.deactivateComponents(["tcpipHttp"])
        if (Database.getComponentByID("tcpipHttpNet") != None):
          res = Database.deactivateComponents(["tcpipHttpNet"])
          res = Database.deactivateComponents(["tcpipSntp"])
          res = Database.deactivateComponents(["netPres"])
          res = Database.deactivateComponents(["lib_wolfssl"])
        symbol.setValue(False,2)

def destroyComponent(component):
    if(Database.getSymbolValue("sysWifiProvPic32mzw1", "SYS_WIFIPROV_HTTP") == True):
      res = Database.deactivateComponents(["drv_memory"])
      res = Database.deactivateComponents(["sys_fs"])
      if (Database.getComponentByID("tcpipHttp") != None):
        res = Database.deactivateComponents(["tcpipHttp"])
      if (Database.getComponentByID("tcpipHttpNet") != None):
        res = Database.deactivateComponents(["tcpipSntp"])
        res = Database.deactivateComponents(["netPres"])
        res = Database.deactivateComponents(["lib_wolfssl"])
        res = Database.deactivateComponents(["tcpipHttpNet"])