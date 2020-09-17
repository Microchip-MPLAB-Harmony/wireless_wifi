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
def instantiateComponent(syswifiComponent):

    syswifiEnable = syswifiComponent.createBooleanSymbol("SYS_WIFI_ENABLE", None)
    #syswifiEnable.setLabel("Enable WiFi Service")
    syswifiEnable.setVisible(False)
    syswifiEnable.setDefaultValue(True)

    syswifiMode = syswifiComponent.createComboSymbol("SYS_WIFI_MODE", None, ["STA", "AP"])
    syswifiMode.setLabel("Device Mode")
    syswifiMode.setDescription("Select the Device Boot Mode ")
    syswifiMode.setDefaultValue("STA")


    syswifistaEnable = syswifiComponent.createBooleanSymbol("SYS_WIFI_STA_ENABLE", syswifiMode)
    syswifistaEnable.setLabel("STA Mode")
    syswifistaEnable.setDefaultValue(True)
    #syswifistaEnable.setVisible(True)
#    syswifistaEnable.setDependencies(syswifiSTAMenu, ["SYS_WIFI_MODE"])
    syswifistaEnable.setDescription("Enable STA mode Configuration ")
    syswifistaEnable.setDependencies(syswifiSTAautoMenu, ["SYS_WIFI_STA_ENABLE"])


    syswifistaSsid = syswifiComponent.createStringSymbol("SYS_WIFI_STA_SSID_NAME", syswifistaEnable)
    syswifistaSsid.setLabel("SSID")
    syswifistaSsid.setVisible(True)
    syswifistaSsid.setDescription("Enter STA Mode SSID")
    syswifistaSsid.setDefaultValue("DEMO_AP")

    syswifistaAuth = syswifiComponent.createComboSymbol("SYS_WIFI_STA_AUTH", syswifistaEnable, ["OPEN", "WPA2","WPA2WPA3","WPA3"])
    syswifistaAuth.setLabel("Security type")
    syswifistaAuth.setDescription("Enter STA Mode Security type")
    syswifistaAuth.setDefaultValue("WPA2")

    syswifistaPwd = syswifiComponent.createStringSymbol("SYS_WIFI_STA_PWD_NAME", syswifistaEnable)
    syswifistaPwd.setLabel("Password")
    syswifistaPwd.setVisible(True)
    syswifistaPwd.setDescription("Enter STA Mode Password")
    syswifistaPwd.setDefaultValue("password")
    syswifistaPwd.setDependencies(syswifiSTASecurityMenu, ["SYS_WIFI_STA_AUTH"])

    syswifistaAuto = syswifiComponent.createBooleanSymbol("SYS_WIFI_STA_AUTOCONNECT", syswifistaEnable)
    syswifistaAuto.setLabel("Auto Connect")
    syswifistaAuto.setDescription("Enable Auto Connect Feature")
    syswifistaAuto.setDefaultValue(True)

    syswifiapEnable = syswifiComponent.createBooleanSymbol("SYS_WIFI_AP_ENABLE", syswifiMode)
    syswifiapEnable.setLabel("AP Mode")
    syswifiapEnable.setDefaultValue(True)
    #syswifiapEnable.setVisible(Database.getSymbolValue("sysWifiPic32mzw1","SYS_WIFI_MODE") == "STA")
    #syswifiapEnable.setDependencies(syswifiAPMenu, ["SYS_WIFI_MODE"])
    syswifiapEnable.setDescription("Enable AP Mode Configuration")
    syswifiapEnable.setDependencies(syswifiAPautoMenu, ["SYS_WIFI_AP_ENABLE"])

    syswifiapSsid = syswifiComponent.createStringSymbol("SYS_WIFI_AP_SSID_NAME", syswifiapEnable)
    syswifiapSsid.setLabel("SSID")
    syswifiapSsid.setVisible(True)
    syswifiapSsid.setDescription("Enter AP Mode SSID")
    syswifiapSsid.setDefaultValue("DEMO_AP_SOFTAP")

    syswifiapAuth = syswifiComponent.createComboSymbol("SYS_WIFI_AP_AUTH", syswifiapEnable, ["OPEN", "WPA2","WPAWPA2(Mixed)","WPA2WPA3","WPA3"])
    syswifiapAuth.setLabel("Security")
    syswifiapAuth.setDescription("Enter AP Mode Security")
    syswifiapAuth.setDefaultValue("WPA2")

    syswifiapPwd = syswifiComponent.createStringSymbol("SYS_WIFI_AP_PWD_NAME", syswifiapEnable)
    syswifiapPwd.setLabel("Password")
    syswifiapPwd.setVisible(True)
    syswifiapPwd.setDescription("Enter AP Mode Password")
    syswifiapPwd.setDefaultValue("password")
    syswifiapPwd.setDependencies(syswifiAPSecurityMenu, ["SYS_WIFI_AP_AUTH"])

    syswifiapSsidv = syswifiComponent.createBooleanSymbol("SYS_WIFI_AP_SSIDVISIBILE", syswifiapEnable)
    syswifiapSsidv.setLabel("SSID Visibility")
    syswifiapSsidv.setDefaultValue(True)
    syswifiapSsidv.setDescription("Enable AP Mode SSID Visibility")
    syswifiapSsidv.setDependencies(syswifiMenuVisible, ["SYS_WIFI_ENABLE"])
    syswifiapSsidv.setDependencies(syswifiMenuVisible, ["SYS_WIFI_AP_ENABLE"])

    syswifiapChannel = syswifiComponent.createIntegerSymbol("SYS_WIFI_AP_CHANNEL", syswifiapEnable)
    syswifiapChannel.setLabel("Channel Number")
    syswifiapChannel.setMin(1)
    syswifiapChannel.setMax(13)
    syswifiapChannel.setDescription("Enable AP Mode Channel")
    syswifiapChannel.setDefaultValue(1)

    # Advanced Configuration
    syswifiAdvMenu = syswifiComponent.createCommentSymbol("SYS_WIFI_ADVANCED_CONFIG_MENU", None)
    syswifiAdvMenu.setLabel("Advanced Configuration")
    syswifiAdvMenu.setVisible(True)
    #syswifiAdvMenu.setDefaultValue(False)

    syswificountrycode = syswifiComponent.createComboSymbol("SYS_WIFI_COUNTRYCODE", syswifiAdvMenu, ["GEN", "USA", "EMEA", "CUST1", "CUST2"])
    syswificountrycode.setLabel("Country Code")
    syswificountrycode.setDefaultValue("GEN")
    syswificountrycode.setDescription("Enable Country Code")
    syswificountrycode.setDependencies(syswifiMenuVisible, ["SYS_WIFI_ENABLE"])

    syswifiCB = syswifiComponent.createIntegerSymbol("SYS_WIFI_MAX_CBS", syswifiAdvMenu)
    syswifiCB.setLabel("Number of Clients")
    syswifiCB.setMin(1)
    syswifiCB.setMax(5)
    syswifiCB.setDefaultValue(2)
    syswifiCB.setDescription("Enter Number of Clients want callback from Wi-Fi Service")
    syswifiCB.setDependencies(syswifiMenuVisible, ["SYS_WIFI_ENABLE"])

    syswifiprovEnable = syswifiComponent.createBooleanSymbol("SYS_WIFI_PROVISION_ENABLE", syswifiAdvMenu)
    syswifiprovEnable.setLabel("Enable WiFi Provisioning Service")
    syswifiprovEnable.setDefaultValue(True)
    syswifiprovEnable.setVisible(True)
    syswifiprovEnable.setDescription("Enable WiFi Provisioning Service")
    syswifiprovEnable.setDependencies(syswifiprovMenuVisible, ["SYS_WIFI_PROVISION_ENABLE"])
    
    syswifiDebugLogEnable = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_ENABLE", syswifiAdvMenu)
    syswifiDebugLogEnable.setLabel("Enable Debug Logs")
    syswifiDebugLogEnable.setVisible(True)
    syswifiDebugLogEnable.setDefaultValue(False)
#    syswifiDebugLogEnable.setDependencies(syswifiDebugMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])

    syswifiDebugBasicMenu = syswifiComponent.createMenuSymbol("SYS_WIFI_APPDEBUG_LEVEL_CONFIG_MENU", syswifiDebugLogEnable)
    syswifiDebugBasicMenu.setLabel("Debug Level Configuration")
    syswifiDebugBasicMenu.setVisible(True)
    syswifiDebugBasicMenu.setDependencies(syswifiMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])

    syswifiDebugErrLevel = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_ERR_LEVEL", syswifiDebugBasicMenu)
    syswifiDebugErrLevel.setLabel("Enable Error Level")
    syswifiDebugErrLevel.setVisible(True)
    syswifiDebugErrLevel.setDefaultValue(False)
    #syswifiDebugErrLevel.setDependencies(syswifiMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])

    syswifiDebugDbgLevel = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_DBG_LEVEL", syswifiDebugBasicMenu)
    syswifiDebugDbgLevel.setLabel("Enable Debug Level")
    syswifiDebugDbgLevel.setVisible(True)
    syswifiDebugDbgLevel.setDefaultValue(False)
    #syswifiDebugDbgLevel.setDependencies(syswifiMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])
	
    syswifiDebugInfoLevel = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_INFO_LEVEL", syswifiDebugBasicMenu)
    syswifiDebugInfoLevel.setLabel("Enable Info Level")
    syswifiDebugInfoLevel.setVisible(True)
    syswifiDebugInfoLevel.setDefaultValue(False)
    #syswifiDebugInfoLevel.setDependencies(syswifiMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])
	
    syswifiDebugFuncLevel = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_FUNC_LEVEL", syswifiDebugBasicMenu)
    syswifiDebugFuncLevel.setLabel("Enable Function Entry/Exit Level")
    syswifiDebugFuncLevel.setVisible(True)
    syswifiDebugFuncLevel.setDefaultValue(False)
    #syswifiDebugFuncLevel.setDependencies(syswifiMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])
	
    #syswifiDebugFlow = syswifiComponent.createIntegerSymbol("SYS_WIFI_APPDEBUG_FLOW", syswifiDebugLogEnable)
    #syswifiDebugFlow.setLabel("Flow")
    #syswifiDebugFlow.setMin(1)
    #syswifiDebugFlow.setMax(65535)
    #syswifiDebugFlow.setDefaultValue(65535)
    #syswifiDebugFlow.setDependencies(syswifiMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])

    syswifiDebugFlowBasicMenu = syswifiComponent.createMenuSymbol("SYS_WIFI_APPDEBUG_FLOW_CONFIG_MENU", syswifiDebugLogEnable)
    syswifiDebugFlowBasicMenu.setLabel("Debug Flow Configuration")
    syswifiDebugFlowBasicMenu.setVisible(True)
    #syswifiDebugFlowBasicMenu.setDefaultValue(False)
    syswifiDebugFlowBasicMenu.setDependencies(syswifiMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])

    syswifiDebugFlowCfgFlow = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_CFG_FLOW", syswifiDebugFlowBasicMenu)
    syswifiDebugFlowCfgFlow.setLabel("Enable WiFi Cfg Flow")
    syswifiDebugFlowCfgFlow.setDefaultValue(False)
    syswifiDebugFlowCfgFlow.setVisible(True)

    syswifiDebugConnectFlow = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_CONNECT_FLOW", syswifiDebugFlowBasicMenu)
    syswifiDebugConnectFlow.setLabel("Enable WiFi Connect Flow")
    syswifiDebugConnectFlow.setDefaultValue(False)
    syswifiDebugConnectFlow.setVisible(True)
	
    syswifiDebugProvFlow = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_PROVISIONING_FLOW", syswifiDebugFlowBasicMenu)
    syswifiDebugProvFlow.setLabel("Enable WiFi Provisioning Flow")
    syswifiDebugProvFlow.setDefaultValue(False)
    syswifiDebugProvFlow.setVisible(True)

    syswifiDebugProvCMDFlow = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_PROVISIONINGCMD_FLOW", syswifiDebugFlowBasicMenu)
    syswifiDebugProvCMDFlow.setLabel("Enable WiFi Provisioning Command Flow")
    syswifiDebugProvCMDFlow.setDefaultValue(False)
    syswifiDebugProvCMDFlow.setVisible(True)

    syswifiDebugProvSOCKFlow = syswifiComponent.createBooleanSymbol("SYS_WIFI_APPDEBUG_PROVISIONINGSOCK_FLOW", syswifiDebugFlowBasicMenu)
    syswifiDebugProvSOCKFlow.setLabel("Enable WiFi Provisioning Socket Flow")
    syswifiDebugProvSOCKFlow.setDefaultValue(False)
    syswifiDebugProvSOCKFlow.setVisible(True)

    syswifiDebugPreStr = syswifiComponent.createStringSymbol("SYS_WIFI_APPDEBUG_PRESTR", syswifiDebugLogEnable)
    syswifiDebugPreStr.setLabel("Prefix String")
    syswifiDebugPreStr.setVisible(True)
    syswifiDebugPreStr.setDescription("Prefix String")
    syswifiDebugPreStr.setDefaultValue("WIFI_SRVC")
    syswifiDebugPreStr.setDependencies(syswifiMenuVisible, ["SYS_WIFI_APPDEBUG_ENABLE"])


    # RTOS Configuration
    syswifiRtosMenu = syswifiComponent.createMenuSymbol("SYS_WIFI_RTOS_MENU", syswifiAdvMenu)
    syswifiRtosMenu.setLabel("RTOS Configuration")
    syswifiRtosMenu.setDescription("RTOS Configuration")
    syswifiRtosMenu.setVisible(False)
    syswifiRtosMenu.setVisible((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal") and (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != None))
    syswifiRtosMenu.setDependencies(syswifishowRTOSMenu, ["HarmonyCore.SELECT_RTOS"])


    # Menu for RTOS options
    syswifiInstnExecMode = syswifiComponent.createComboSymbol("SYS_WIFI_RTOS", syswifiRtosMenu, ["Standalone"]) 
    syswifiInstnExecMode.setLabel("Run Library Tasks as")
    syswifiInstnExecMode.setVisible(False)
    syswifiInstnExecMode.setDescription("Rtos Options")
    syswifiInstnExecMode.setDefaultValue("Standalone")

    # RTOS Task Stack Size
    syswifiTaskSize = syswifiComponent.createIntegerSymbol("SYS_WIFI_RTOS_TASK_STACK_SIZE", syswifiRtosMenu)
    syswifiTaskSize.setLabel("Stack Size")
    syswifiTaskSize.setVisible(True)
    syswifiTaskSize.setDescription("Rtos Task Stack Size")
    syswifiTaskSize.setDefaultValue(1024)
    syswifiTaskSize.setDependencies(syswifiRTOSStandaloneMenu, ["SYS_WIFI_RTOS"])

    # RTOS Task Priority
    syswifiTaskPriority = syswifiComponent.createIntegerSymbol("SYS_WIFI_RTOS_TASK_PRIORITY", syswifiRtosMenu)
    syswifiTaskPriority.setLabel("Task Priority")
    syswifiTaskPriority.setVisible(True)
    syswifiTaskPriority.setDescription("Rtos Task Priority")
    syswifiTaskPriority.setDefaultValue(1)
    syswifiTaskPriority.setDependencies(syswifiRTOSStandaloneMenu, ["SYS_WIFI_RTOS"])

    # RTOS Use Task Delay?
    syswifiUseTaskDelay = syswifiComponent.createBooleanSymbol("SYS_WIFI_RTOS_USE_DELAY", syswifiRtosMenu)
    syswifiUseTaskDelay.setLabel("Use Task Delay?")
    syswifiUseTaskDelay.setVisible(True)
    syswifiUseTaskDelay.setDescription("Rtos Use Task Delay?")
    syswifiUseTaskDelay.setDefaultValue(True)
    syswifiUseTaskDelay.setDependencies(syswifiRTOSStandaloneMenu, ["SYS_WIFI_RTOS"])

    # RTOS Task Delay
    syswifiTaskDelay = syswifiComponent.createIntegerSymbol("SYS_WIFI_RTOS_DELAY", syswifiRtosMenu)
    syswifiTaskDelay.setLabel("Task Delay")
    syswifiTaskDelay.setVisible(True)
    syswifiTaskDelay.setDescription("WiFi Driver Task Delay")
    syswifiTaskDelay.setDefaultValue(1)
    syswifiTaskDelay.setDependencies(syswifiRTOSTaskDelayMenu, ["SYS_WIFI_RTOS", "SYS_WIFI_RTOS_USE_DELAY"])

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    #syswifiHeaderFile = syswifiComponent.createFileSymbol("SYS_WIFI_HEADER", None)
    #syswifiHeaderFile.setSourcePath("system/wifi/sys_wifi.h")
    #syswifiHeaderFile.setOutputName("sys_wifi.h")
    #syswifiHeaderFile.setDestPath("system/wifi/")
    #syswifiHeaderFile.setProjectPath("config/" + configName + "/system/wifi/")
    #syswifiHeaderFile.setType("HEADER")
    #syswifiHeaderFile.setOverwrite(True)

    #syswifiSourceFile = syswifiComponent.createFileSymbol("SYS_WIFI_SOURCE", None)
    #syswifiSourceFile.setSourcePath("system/wifi/src/sys_wifi.c")
    #syswifiSourceFile.setOutputName("sys_wifi.c")
    #syswifiSourceFile.setDestPath("system/wifi/src")
    #syswifiSourceFile.setProjectPath("config/" + configName + "/system/wifi/")
    #syswifiSourceFile.setType("SOURCE")
    #syswifiSourceFile.setOverwrite(True)

    syswifiSourceFile = syswifiComponent.createFileSymbol("SYS_WIFI_SOURCE", None)
    syswifiSourceFile.setSourcePath("system/wifi/templates/src/sys_wifi.c.ftl")
    syswifiSourceFile.setOutputName("sys_wifi.c")
    syswifiSourceFile.setDestPath("system/wifi/src")
    syswifiSourceFile.setProjectPath("config/" + configName + "/system/wifi/")
    syswifiSourceFile.setType("SOURCE")
    syswifiSourceFile.setMarkup(True)
    syswifiSourceFile.setEnabled(True)

    syswifiHeaderFile = syswifiComponent.createFileSymbol("SYS_WIFI_HEADER", None)
    syswifiHeaderFile.setSourcePath("system/wifi/templates/sys_wifi.h.ftl")
    syswifiHeaderFile.setOutputName("sys_wifi.h")
    syswifiHeaderFile.setDestPath("system/wifi/")
    syswifiHeaderFile.setProjectPath("config/" + configName + "/system/wifi/")
    syswifiHeaderFile.setType("HEADER")
    syswifiHeaderFile.setMarkup(True)
    syswifiHeaderFile.setEnabled(True)

    syswifiSystemDefFile = syswifiComponent.createFileSymbol("SYS_WIFI_DEF", None)
    syswifiSystemDefFile.setType("STRING")
    syswifiSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    syswifiSystemDefFile.setSourcePath("system/wifi/templates/system/system_definitions.h.ftl")
    syswifiSystemDefFile.setMarkup(True)

    syswifiSystemInitFile = syswifiComponent.createFileSymbol("SYS_WIFI_INIT", None)
    syswifiSystemInitFile.setType("STRING")
    syswifiSystemInitFile.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_SYSTEM_SERVICES")
    syswifiSystemInitFile.setSourcePath("system/wifi/templates/system/system_initialize.c.ftl")
    syswifiSystemInitFile.setMarkup(True)

    syswifiSystemConfFile = syswifiComponent.createFileSymbol("SYS_WIFI_CONFIGURATION_H", None)
    syswifiSystemConfFile.setType("STRING")
    syswifiSystemConfFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    syswifiSystemConfFile.setSourcePath("system/wifi/templates/system/system_config.h.ftl")
    syswifiSystemConfFile.setMarkup(True)


    syswifiSystemTaskFile = syswifiComponent.createFileSymbol("SYS_WIFI_SYSTEM_TASKS_C", None)
    syswifiSystemTaskFile.setType("STRING")
    syswifiSystemTaskFile.setOutputName("core.LIST_SYSTEM_TASKS_C_CALL_LIB_TASKS")
    syswifiSystemTaskFile.setSourcePath("system/wifi/templates/system/system_tasks.c.ftl")
    syswifiSystemTaskFile.setMarkup(True)

    syswifiSystemDefFile = syswifiComponent.createFileSymbol('SYS_WIFI_SYSTEM_DEF', None)
    syswifiSystemDefFile.setType('STRING')
    syswifiSystemDefFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS')
    syswifiSystemDefFile.setSourcePath('system/wifi/templates/system/system_definitions_objects.h.ftl')
    syswifiSystemDefFile.setMarkup(True)

    syswifiSystemRtosTasksFile = syswifiComponent.createFileSymbol("SYS_WIFI_SYS_RTOS_TASK", None)
    syswifiSystemRtosTasksFile.setType("STRING")
    syswifiSystemRtosTasksFile.setOutputName("core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS")
    syswifiSystemRtosTasksFile.setSourcePath("system/wifi/templates/system/system_rtos_tasks.c.ftl")
    syswifiSystemRtosTasksFile.setMarkup(True)
    syswifiSystemRtosTasksFile.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))
    syswifiSystemRtosTasksFile.setDependencies(genRtosTask, ["HarmonyCore.SELECT_RTOS"])

    #Set back active to root.
    Database.setActiveGroup(None)
############################################################################
#### Dependency ####
############################################################################

def onAttachmentConnected(source, target):
    localComponent = source["component"]
    remoteComponent = target["component"]
    remoteID = remoteComponent.getID()
    connectID = source["id"]
    targetID = target["id"]


def onAttachmentDisconnected(source, target):
    localComponent = source["component"]
    remoteComponent = target["component"]
    remoteID = remoteComponent.getID()
    connectID = source["id"]
    targetID = target["id"]

def syswifiMenuVisible(symbol, event):
    if (event["value"] == True):
        print("WIFI Menu Visible.")
        symbol.setVisible(True)
    else:
        print("WIFI Menu Invisible.")
        symbol.setVisible(False)

def syswifishowRTOSMenu(symbol, event):
    if (event["value"] == None):
        symbol.setVisible(False)
        print("SYS WIFI: OSAL Disabled")
    elif (event["value"] != "BareMetal"):
        # If not Bare Metal
        symbol.setVisible(True)
        print("SYS WIFI rtos")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_MALLOC_FUNC", "malloc")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_FREE_FUNC", "free")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_CALLOC_FUNC", "calloc")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_DRAM_SIZE", 65000)
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_USER_NOTIFICATION", True)
        Database.setSymbolValue("core", "XC32_HEAP_SIZE", 160000)
        Database.setSymbolValue("core", "WIFI_CLOCK_ENABLE", True)
    else:
        symbol.setVisible(False)
        print("SYS WIFI Bare Metal")
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_MALLOC_FUNC", "pvPortMalloc")
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_FREE_FUNC", "vPortFree")
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_CALLOC_FUNC", "APP_Calloc")
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_DRAM_SIZE", 75000)
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_USER_NOTIFICATION", True)
        #Database.setSymbolValue("core", "XC32_HEAP_SIZE", 10240)
        #Database.setSymbolValue("FreeRTOS", "FREERTOS_TOTAL_HEAP_SIZE",160000)
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_MALLOC_FUNC", "malloc")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_FREE_FUNC", "free")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_CALLOC_FUNC", "calloc")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_DRAM_SIZE", 65000)
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_USER_NOTIFICATION", True)
        Database.setSymbolValue("core", "XC32_HEAP_SIZE", 150000)
        Database.setSymbolValue("core", "WIFI_CLOCK_ENABLE", True)


def syswifiRTOSStandaloneMenu(symbol, event):
    if (event["value"] == 'Standalone'):        
        symbol.setVisible(True)
        print("SYS WIFI Standalone")
    else:
        symbol.setVisible(False)
        print("SYS WIFI Combined")
def syswifiSTASecurityMenu(symbol, event):
    sysWiFisecurity = Database.getSymbolValue("sysWifiPic32mzw1","SYS_WIFI_STA_AUTH")
    if(sysWiFisecurity == "OPEN"):        
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)

def syswifiAPSecurityMenu(symbol, event):
    sysWiFisecurity = Database.getSymbolValue("sysWifiPic32mzw1","SYS_WIFI_AP_AUTH")
    if(sysWiFisecurity == "OPEN"):        
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)

def syswifiSTAMenu(symbol, event):

    if(Database.getSymbolValue("sysWifiPic32mzw1","SYS_WIFI_MODE") == "STA"):        
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def syswifiAPMenu(symbol, event):

    if(Database.getSymbolValue("sysWifiPic32mzw1","SYS_WIFI_MODE") == "AP"):        
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def syswifiSTAautoMenu(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipDns"],"APPLICATION LAYER", False)
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDns", "libtcpipDns")
        res = Database.activateComponents(["tcpipDhcp"],"APPLICATION LAYER", False)
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDhcp", "libtcpipDhcp")
        if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT") != True):
           Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT", True)
        if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT") != True):
           Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", True)
    else:
        Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT", False)
        Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", False)
        #res = Database.deactivateComponents(["tcpipDhcp"])
        #res = Database.deactivateComponents(["tcpipDns"])

def syswifiAPautoMenu(symbol, event):
    tcpipAutoConfigAppsGroup = Database.findGroup("APPLICATION LAYER")
    if (event["value"] == True):
        res = Database.activateComponents(["tcpipDhcps"],"APPLICATION LAYER", False)    
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDhcps", "libtcpipDhcps")
        res = Database.activateComponents(["tcpipDnss"],"APPLICATION LAYER", False)
        tcpipAutoConfigAppsGroup.setAttachmentVisible("tcpipDnss", "libtcpipDnss")
        if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER") != True):
           Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER", True)
        if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER") != True):
           Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER", True)
    else:
        Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER", False)
        Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER", False)
        #res = Database.deactivateComponents(["tcpipDhcps"])
        #res = Database.deactivateComponents(["tcpipDnss"])


def syswifiprovMenuVisible(symbol, event):
    if (event["value"] == True):
        if(Database.getComponentByID("sysWifiProvPic32mzw1") == None):    
                res = Database.activateComponents(["sysWifiProvPic32mzw1"])
    else:
        res = Database.deactivateComponents(["sysWifiProvPic32mzw1"])

def syswifiRTOSTaskDelayMenu(symbol, event):
    sysWiFiRtos = Database.getSymbolValue("sysWifiPic32mzw1","SYS_WIFI_RTOS")
    sysWiFiRtosRtosUseDelay = Database.getSymbolValue("sysWifiPic32mzw1","SYS_WIFI_RTOS_USE_DELAY")
    if((sysWiFiRtos == 'Standalone') and sysWiFiRtosRtosUseDelay):        
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)
def genRtosTask(symbol, event):
    symbol.setEnabled((Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"))
########################################################################################################
def finalizeComponent(syswifiComponent):
    res = Database.activateComponents(["Root"])
    syswifisysComponent = Database.createGroup(None,"System Component")
    res = Database.activateComponents(["dfp"],"System Component", True)
    res = Database.activateComponents(["system"],"System Component", True)
    res = Database.activateComponents(["core"],"System Component", True)
    if(Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_PROVISION_ENABLE") == True):
       res = Database.activateComponents(["nvm"],"System Component", True)
    res = Database.activateComponents(["core_timer"],"System Component", True)
    res = Database.activateComponents(["uart1"],"System Component", True)
    res = Database.activateComponents(["lib_crypto"],"System Component", True)
    res = Database.activateComponents(["lib_wolfcrypt"],"System Component", True)
    res = Database.activateComponents(["sys_time"],"System Component", True)
    res = Database.activateComponents(["sys_console"],"System Component", True)
    res = Database.activateComponents(["sys_command"],"System Component", True)
    res = Database.activateComponents(["sys_debug"],"System Component", True)
    #Driver layer
    res = Database.activateComponents(["tcpipAutoConfigApps"],"System Component", True)
    res = Database.activateComponents(["tcpipAutoConfigBasic"],"System Component", True)
    res = Database.activateComponents(["tcpipAutoConfigDriver"],"System Component", True)
    res = Database.activateComponents(["tcpipAutoConfigNetwork"],"System Component", True)
    res = Database.activateComponents(["drvWifiPic32mzw1"],"System Component", True)
    if(Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"):
       res = Database.activateComponents(["drv_ba414e"],"System Component", True)    

#Application layer
    res = Database.activateComponents(["tcpip_apps_config"],"System Component", True)
    if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT") != True):
            Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT", True)
            res=syswifisysComponent.addComponent("TCP/IP STACK")
            res=syswifisysComponent.addComponent("tcpipStack")
            res=syswifisysComponent.addComponent("tcpipNetConfig")
            res=syswifisysComponent.addComponent("HarmonyCore")
            res=syswifisysComponent.addComponent("core")
            res=syswifisysComponent.addComponent("dfp")
            res=syswifisysComponent.addComponent("FreeRTOS")
            res=syswifisysComponent.addComponent("sys_console")
            res=syswifisysComponent.addComponent("sys_command")
            res=syswifisysComponent.addComponent("sys_debug")

    if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER") != True):
            Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER", True)

    if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT") != True):
            Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", True)

    if(Database.getSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER") != True):
            Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER", True)

    if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ARP") != True):
        Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ARP", True)

    if(Database.getSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv4") != True):
        Database.setSymbolValue("tcpip_network_config", "TCPIP_AUTOCONFIG_ENABLE_ICMPv4", True)

    if(Database.getSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD") != True):
        Database.setSymbolValue("tcpip_basic_config", "TCPIP_AUTOCONFIG_ENABLE_TCPIPCMD", True)
        #add connections
        autoConnectTableTime = [["sys_time", "sys_time_TMR_dependency", "core_timer", "CORE_TIMER_TMR"]]
        autoConnectTablenetMAC = [["tcpipNetConfig_0", "NETCONFIG_MAC_Dependency", "drvWifiPic32mzw1","libdrvPic32mzw1Mac"]]
        autoConnectTablecrypto = [["lib_crypto", "LIB_CRYPTO_WOLFCRYPT_Dependency", "lib_wolfcrypt","lib_wolfcrypt"]]
        autoConnectTableuart = [["sys_console_0", "sys_console_UART_dependency", "uart1","UART1_UART"]]
        autoConnectTablecondeb = [["sys_console_0", "sys_console", "sys_debug","sys_debug_SYS_CONSOLE_dependency"]]
        autoConnectTableconcmd = [["sys_console_0", "sys_console", "sys_command","sys_command_SYS_CONSOLE_dependency"]]
        autoConnectTablecontcp = [["sys_console_0", "sys_console", "tcpipStack","Core_SysConsole_Dependency"]]
        res = Database.connectDependencies(autoConnectTablenetMAC)
        res = Database.connectDependencies(autoConnectTablecrypto)
        res = Database.connectDependencies(autoConnectTableuart)
        res = Database.connectDependencies(autoConnectTablecontcp)
        res = Database.connectDependencies(autoConnectTablecondeb)
        res = Database.connectDependencies(autoConnectTableTime)
        res = Database.connectDependencies(autoConnectTableconcmd)
        if(Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"):
           autoConnectTableconba414 = [["drvWifiPic32mzw1", "BA414E_Dependency", "drv_ba414e","drv_ba414e"]]
           res = Database.connectDependencies(autoConnectTableconba414)


    if(Database.getSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP") != True):
        Database.setSymbolValue("tcpip_transport_config", "TCPIP_AUTOCONFIG_ENABLE_TCP", True)       
    

#    res = Database.activateComponents(["TCP/IP STACK"],"System Component", True)
    #syswifiComponent.setCapabilityEnabled("syswifiSrvc", True)

    # Enable dependent Harmony core components
    if (Database.getSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON", True)

    if (Database.getSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON", True)


    # memory configurations
    if(Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") == "BareMetal"):
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_MALLOC_FUNC", "malloc")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_FREE_FUNC", "free")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_CALLOC_FUNC", "calloc")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_DRAM_SIZE", 65000)
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_USER_NOTIFICATION", True)
        Database.setSymbolValue("core", "XC32_HEAP_SIZE", 150000)
        Database.setSymbolValue("core", "WIFI_CLOCK_ENABLE", True)
        Database.setSymbolValue("core", "EWPLL_ENABLE", True)        
        Database.setSymbolValue("tcpipIcmp", "TCPIP_ICMP_CLIENT_USER_NOTIFICATION", True)
        Database.setSymbolValue("tcpipIcmp", "TCPIP_STACK_USE_ICMP_CLIENT", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_hw", True)
        Database.setSymbolValue("uart1", "UART_TX_RING_BUFFER_SIZE", 1024)
        Database.setSymbolValue("core", "CRYPTO_CLOCK_ENABLE", True)
        Database.setSymbolValue("core", "RNG_CLOCK_ENABLE", True)
        Database.setSymbolValue("sys_console", "SYS_CONSOLE_PRINT_BUFFER_SIZE", 256)
    else:
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_MALLOC_FUNC", "pvPortMalloc")
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_FREE_FUNC", "vPortFree")
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_CALLOC_FUNC", "APP_Calloc")
        #Database.setSymbolValue("tcpipStack", "TCPIP_STACK_DRAM_SIZE", 75000)
        #Database.setSymbolValue("core", "XC32_HEAP_SIZE", 10240)
        #Database.setSymbolValue("FreeRTOS", "FREERTOS_TOTAL_HEAP_SIZE",160000)
        #Database.setSymbolValue("core", "WIFI_CLOCK_ENABLE", True)
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_MALLOC_FUNC", "malloc")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_FREE_FUNC", "free")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_CALLOC_FUNC", "calloc")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_DRAM_SIZE", 65000)
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_USER_NOTIFICATION", True)
        Database.setSymbolValue("core", "XC32_HEAP_SIZE", 150000)
        Database.setSymbolValue("core", "WIFI_CLOCK_ENABLE", True)
        Database.setSymbolValue("core", "OSCCON_NOSC_VALUE", "SPLL")
        Database.setSymbolValue("core", "EWPLL_ENABLE", True)
        Database.setSymbolValue("tcpipIcmp", "TCPIP_ICMP_CLIENT_USER_NOTIFICATION", True)
        Database.setSymbolValue("tcpipIcmp", "TCPIP_STACK_USE_ICMP_CLIENT", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_hw", True)
        Database.setSymbolValue("uart1", "UART_TX_RING_BUFFER_SIZE", 1024)
        Database.setSymbolValue("core", "CRYPTO_CLOCK_ENABLE", True)
        Database.setSymbolValue("core", "RNG_CLOCK_ENABLE", True)
        Database.setSymbolValue("sys_console", "SYS_CONSOLE_PRINT_BUFFER_SIZE", 256)
        Database.setSymbolValue("sys_command", "SYS_COMMAND_RTOS_STACK_SIZE", 4096)
        Database.setSymbolValue("FreeRTOS", "FREERTOS_MEMORY_MANAGEMENT_CHOICE", "Heap_3")
        Database.setSymbolValue("tcpipStack", "TCPIP_STACK_USE_HEAP_CONFIG", "TCPIP_STACK_HEAP_TYPE_EXTERNAL_HEAP")

    if(Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_PROVISION_ENABLE") == True):
       res = Database.activateComponents(["sysWifiProvPic32mzw1"])
    if(Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_APPDEBUG_ENABLE") == True):
       res = Database.activateComponents(["sysAppDebugPic32mzw1"])
########################################################################################################    
def destroyComponent(component):
    if(Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_PROVISION_ENABLE") == True):
       res = Database.deactivateComponents(["sysWifiProvPic32mzw1"])
       res = Database.deactivateComponents(["nvm"])
    if(Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_STA_ENABLE") == True):
        Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_CLIENT", False)
        Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_CLIENT", False)
    if(Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_AP_ENABLE") == True):
        Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DHCP_SERVER", False)
        Database.setSymbolValue("tcpip_apps_config", "TCPIP_AUTOCONFIG_ENABLE_DNS_SERVER", False)
    if(Database.getSymbolValue("sysWifiPic32mzw1", "SYS_WIFI_APPDEBUG_ENABLE") == True):
       res = Database.deactivateComponents(["sysAppDebugPic32mzw1"])
    res = Database.deactivateComponents(["System Component"])