# coding: utf-8
"""*****************************************************************************
Copyright (C) 2020 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
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
def instantiateComponent(netComponent):

    netSuppIntf = netComponent.createKeyValueSetSymbol("SYS_NET_SUPP_INTF", None)
    netSuppIntf.setLabel("Supported Interfaces")
    netSuppIntf.addKey("WIFI_ONLY", "0", "Wifi Intf Supported")
    netSuppIntf.addKey("WIFI_ETHERNET", "1", "Wifi and Ethernet Intf Supported")
    netSuppIntf.setDisplayMode("Key")
    netSuppIntf.setOutputMode("Key")
    netSuppIntf.setDefaultValue(0)
    netSuppIntf.setDependencies(netIntfAutoMenu, ["SYS_NET_SUPP_INTF"])

    netDebugEnable = netComponent.createBooleanSymbol("SYS_NET_ENABLE_DEBUG", None)
    netDebugEnable.setLabel("Debug")
    netDebugEnable.setDescription("Debug - Logs and CLI commands")
    netDebugEnable.setDefaultValue(True)
	
    netCliCmdEnable = netComponent.createBooleanSymbol("SYS_NET_ENABLE_CLICMD", netDebugEnable)
    netCliCmdEnable.setLabel("Enable CLI Commands")
    netCliCmdEnable.setDefaultValue(True)

    netDebugLogEnable = netComponent.createBooleanSymbol("SYS_NET_APPDEBUG_ENABLE", netDebugEnable)
    netDebugLogEnable.setLabel("Enable Debug Logs")
    netDebugLogEnable.setDefaultValue(False)
    
    netDebugBasicMenu = netComponent.createMenuSymbol("SYS_NET_APPDEBUG_LEVEL_CONFIG_MENU", netDebugLogEnable)
    netDebugBasicMenu.setLabel("Debug Level Configuration")
    netDebugBasicMenu.setVisible(False)
    netDebugBasicMenu.setDependencies(netDebugMenuVisible, ["SYS_NET_APPDEBUG_ENABLE"])
		
    netDebugErrLevel = netComponent.createBooleanSymbol("SYS_NET_APPDEBUG_ERR_LEVEL", netDebugBasicMenu)
    netDebugErrLevel.setLabel("Enable Error Level")
    netDebugErrLevel.setDefaultValue(True)
#    netDebugErrLevel.setDependencies(netDebugMenuVisible, ["SYS_NET_APPDEBUG_ENABLE"])

    netDebugDbgLevel = netComponent.createBooleanSymbol("SYS_NET_APPDEBUG_DBG_LEVEL", netDebugBasicMenu)
    netDebugDbgLevel.setLabel("Enable Debug Level")
    netDebugDbgLevel.setDefaultValue(False)
#    netDebugDbgLevel.setDependencies(netDebugMenuVisible, ["SYS_NET_APPDEBUG_ENABLE"])
	
    netDebugInfoLevel = netComponent.createBooleanSymbol("SYS_NET_APPDEBUG_INFO_LEVEL", netDebugBasicMenu)
    netDebugInfoLevel.setLabel("Enable Info Level")
    netDebugInfoLevel.setDefaultValue(False)
#    netDebugInfoLevel.setDependencies(netDebugMenuVisible, ["SYS_NET_APPDEBUG_ENABLE"])
	
    netDebugFuncLevel = netComponent.createBooleanSymbol("SYS_NET_APPDEBUG_FUNC_LEVEL", netDebugBasicMenu)
    netDebugFuncLevel.setLabel("Enable Function Entry/Exit Level")
    netDebugFuncLevel.setDefaultValue(False)
#    netDebugFuncLevel.setDependencies(netDebugMenuVisible, ["SYS_NET_APPDEBUG_ENABLE"])
	
    netDebugFlowBasicMenu = netComponent.createMenuSymbol("SYS_NET_APPDEBUG_FLOW_CONFIG_MENU", netDebugLogEnable)
    netDebugFlowBasicMenu.setLabel("Debug Flow Configuration")
    netDebugFlowBasicMenu.setVisible(True)
    netDebugFlowBasicMenu.setDependencies(netDebugMenuVisible, ["SYS_NET_APPDEBUG_ENABLE"])
		
    netDebugCfgFlow = netComponent.createBooleanSymbol("SYS_NET_APPDEBUG_CFG_FLOW", netDebugFlowBasicMenu)
    netDebugCfgFlow.setLabel("Enable NET Cfg Flow")
    netDebugCfgFlow.setDefaultValue(True)

    netDebugDataFlow = netComponent.createBooleanSymbol("SYS_NET_APPDEBUG_DATA_FLOW", netDebugFlowBasicMenu)
    netDebugDataFlow.setLabel("Enable NET Data Flow")
    netDebugDataFlow.setDefaultValue(True)
	
    netDebugPreStr = netComponent.createStringSymbol("SYS_NET_APPDEBUG_PRESTR", netDebugLogEnable)
    netDebugPreStr.setLabel("Prefix String")
    netDebugPreStr.setVisible(True)
    netDebugPreStr.setDescription("Prefix String")
    netDebugPreStr.setDefaultValue("NET_SRVC")
    netDebugPreStr.setDependencies(netDebugMenuVisible, ["SYS_NET_APPDEBUG_ENABLE"])

    netInstance0 = netComponent.createBooleanSymbol("SYS_NET_IDX0", None)
    netInstance0.setLabel("Instance 0")
    netInstance0.setDescription("Debug - Logs and CLI commands")
    netInstance0.setDefaultValue(True)

    netIntf0 = netComponent.createComboSymbol("SYS_NET_INTF", netInstance0, ["WIFI", "ETHERNET"])
    netIntf0.setLabel("Intf")
    netIntf0.setDefaultValue("WIFI")

    netIpProt0 = netComponent.createComboSymbol("SYS_NET_IPPROT", netInstance0, ["UDP", "TCP"])
    netIpProt0.setLabel("Ip Protocol")
    netIpProt0.setDefaultValue("TCP")
	
    netMode0 = netComponent.createComboSymbol("SYS_NET_MODE", netInstance0, ["CLIENT", "SERVER"])
    netMode0.setLabel("Mode")
    netMode0.setDefaultValue("CLIENT")

    netReConnect0 = netComponent.createBooleanSymbol("SYS_NET_RECONNECT", netInstance0)
    netReConnect0.setLabel("Enable Auto Connect")
    netReConnect0.setDefaultValue(True)
	
    netEnableTls0 = netComponent.createBooleanSymbol("SYS_NET_ENABLE_TLS", netInstance0)
    netEnableTls0.setLabel("Enable TLS")
    netEnableTls0.setDefaultValue(False)
    netEnableTls0.setDependencies(netTLSautoMenu, ["SYS_NET_ENABLE_TLS"])

	
    netPort0 = netComponent.createIntegerSymbol("SYS_NET_PORT", netInstance0)
    netPort0.setLabel("Server Port")
    netPort0.setMin(1)
    netPort0.setMax(65535)
	
    netHostName0 = netComponent.createStringSymbol("SYS_NET_HOST_NAME", netInstance0)
    netHostName0.setLabel("Host Name/ IP Address")
    netHostName0.setVisible(True)
    netHostName0.setDescription("Host Name/ IP")
    netHostName0.setDefaultValue("192.168.1.1")
	
    netInstance1 = netComponent.createBooleanSymbol("SYS_NET_IDX1", None)
    netInstance1.setLabel("Instance 1")
    netInstance1.setDescription("Debug - Logs and CLI commands")
    netInstance1.setDefaultValue(False)

    netIntf1 = netComponent.createComboSymbol("SYS_NET_IDX1_INTF", netInstance1, ["WIFI", "ETHERNET"])
    netIntf1.setLabel("Intf")
    netIntf1.setDefaultValue("WIFI")
    netIntf1.setVisible(False)
    netIntf1.setDependencies(netInst1MenuVisible, ["SYS_NET_IDX1"])

    netIpProt1 = netComponent.createComboSymbol("SYS_NET_IDX1_IPPROT", netInstance1, ["UDP", "TCP"])
    netIpProt1.setLabel("Ip Protocol")
    netIpProt1.setDefaultValue("TCP")
    netIpProt1.setVisible(False)
    netIpProt1.setDependencies(netInst1MenuVisible, ["SYS_NET_IDX1"])
	
    netMode1 = netComponent.createComboSymbol("SYS_NET_IDX1_MODE", netInstance1, ["CLIENT", "SERVER"])
    netMode1.setLabel("Mode")
    netMode1.setDefaultValue("CLIENT")
    netMode1.setVisible(False)
    netMode1.setDependencies(netInst1MenuVisible, ["SYS_NET_IDX1"])

    netReConnect1 = netComponent.createBooleanSymbol("SYS_NET_IDX1_RECONNECT", netInstance1)
    netReConnect1.setLabel("Enable Auto Connect")
    netReConnect1.setDefaultValue(True)
    netReConnect1.setVisible(False)
    netReConnect1.setDependencies(netInst1MenuVisible, ["SYS_NET_IDX1"])
	
    netEnableTls1 = netComponent.createBooleanSymbol("SYS_NET_IDX1_ENABLE_TLS", netInstance1)
    netEnableTls1.setLabel("Enable TLS")
    netEnableTls1.setDefaultValue(False)
    netEnableTls1.setDependencies(netTLSautoMenu, ["SYS_NET_IDX1_ENABLE_TLS"])
    netEnableTls1.setVisible(False)
    netEnableTls1.setDependencies(netInst1MenuVisible, ["SYS_NET_IDX1"])

    netPort1 = netComponent.createIntegerSymbol("SYS_NET_IDX1_PORT", netInstance1)
    netPort1.setLabel("Server Port")
    netPort1.setMin(1)
    netPort1.setMax(65535)
    netPort1.setVisible(False)
    netPort1.setDependencies(netInst1MenuVisible, ["SYS_NET_IDX1"])
	
    netHostName1 = netComponent.createStringSymbol("SYS_NET_IDX1_HOST_NAME", netInstance1)
    netHostName1.setLabel("Host Name/ IP Address")
    netHostName1.setVisible(True)
    netHostName1.setDescription("Host Name/ IP")
    netHostName1.setDefaultValue("192.168.1.1")
    netHostName1.setVisible(False)
    netHostName1.setDependencies(netInst1MenuVisible, ["SYS_NET_IDX1"])
	
    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    netHeaderFile = netComponent.createFileSymbol("SYS_NET_HEADER", None)
    netHeaderFile.setSourcePath("../wireless/system/net/sys_net.h")
    netHeaderFile.setOutputName("sys_net.h")
    netHeaderFile.setDestPath("system/net/")
    netHeaderFile.setProjectPath("config/" + configName + "/system/net/")
    netHeaderFile.setType("HEADER")
    netHeaderFile.setOverwrite(True)

    print("Network Service Component Header Path %s", netHeaderFile.getProjectPath())
	
    netSourceFile = netComponent.createFileSymbol("SYS_NET_SOURCE", None)
    netSourceFile.setSourcePath("../wireless/system/net/src/sys_net.c")
    netSourceFile.setOutputName("sys_net.c")
    netSourceFile.setDestPath("system/net/src")
    netSourceFile.setProjectPath("config/" + configName + "/system/net/")
    netSourceFile.setType("SOURCE")
    netSourceFile.setOverwrite(True)

    netSystemDefFile = netComponent.createFileSymbol("SYS_NET_DEF", None)
    netSystemDefFile.setType("STRING")
    netSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    netSystemDefFile.setSourcePath("../wireless/system/net/templates/system/system_definitions.h.ftl")
    netSystemDefFile.setMarkup(True)
	
#	netSystemConfigFile = netComponent.createFileSymbol("SYS_NET_SYS_CONFIG", None)
#	netSystemConfigFile.setType("STRING")
#	netSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_SYSTEM_SERVICE_CONFIGURATION")
#	netSystemConfigFile.setSourcePath("../wireless/system/net/templates/system/system_config.h.ftl")
#	netSystemConfigFile.setMarkup(True)

    netSystemConfigFile = netComponent.createFileSymbol("SYS_CONSOLE_SYS_CONFIG", None)
    netSystemConfigFile.setType("STRING")
    netSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_SYSTEM_SERVICE_CONFIGURATION")
    netSystemConfigFile.setSourcePath("../wireless/system/net/templates/system/system_config.h.ftl")
    netSystemConfigFile.setMarkup(True)

    netSystemInitDataFile = netComponent.createFileSymbol("SYS_NET_INIT_DATA", None)
    netSystemInitDataFile.setType("STRING")
    netSystemInitDataFile.setOutputName("core.LIST_SYSTEM_INIT_C_SYSTEM_INITIALIZATION")
    netSystemInitDataFile.setSourcePath("../wireless/system/net/templates/system/system_initialize_data.c.ftl")
    netSystemInitDataFile.setMarkup(True)

    netSystemInitFile = netComponent.createFileSymbol("SYS_NET_INIT", None)
    netSystemInitFile.setType("STRING")
    netSystemInitFile.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_SYSTEM_SERVICES")
    netSystemInitFile.setSourcePath("../wireless/system/net/templates/system/system_initialize.c.ftl")
    netSystemInitFile.setMarkup(True)
	
	############################################################################
#### Dependency ####
############################################################################

def netDebugMenuVisible(symbol, event):
    if (event["value"] == True):
        print("Debug Log Menu Visible")
        symbol.setVisible(True)
    else:
        print("Debug Log Menu Invisible")
        symbol.setVisible(False)

def netInst1MenuVisible(symbol, event):
    if (event["value"] == True):
        print("Skt Instance 1 Menu Visible")
        symbol.setVisible(True)
    else:
        print("Skt Instance 1 Menu Invisible")
        symbol.setVisible(False)

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

def netIntfAutoMenu(symbol, event):
#    if (event["value"] == "WIFI_ETHERNET"):
    if (event["value"] == 1):
        res = Database.activateComponents(["drvPic32mEthmac"],"System Component", True)
        res = Database.activateComponents(["drvMiim"],"System Component", True)
        res = Database.activateComponents(["drvExtPhyLan8740"],"System Component", True)
#        res = Database.activateComponents(["tcpipNetConfig"],"System Component", True)
    else:
        res = Database.deactivateComponents(["drvExtPhyLan8740"])
        res = Database.deactivateComponents(["drvMiim"])
        res = Database.deactivateComponents(["drvPic32mEthmac"])

def netTLSautoMenu(symbol, event):
    if (event["value"] == True):
        res = Database.activateComponents(["lib_wolfssl"],"System Component", True)
        res = Database.activateComponents(["tcpipSntp"],"System Component", True)
        autoConnectTableWolfSSLcrypto = [["lib_wolfssl", "WolfSSL_Crypto_Dependency", "lib_wolfcrypt","lib_wolfcrypt"]]
        res = Database.connectDependencies(autoConnectTableWolfSSLcrypto)
        Database.setSymbolValue("netPres_0", "NET_PRES_SUPPORT_ENCRYPTION0", True)
        Database.setSymbolValue("netPres_0", "NET_PRES_ENC_PROVIDE_IDX0", 0)
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_CERT", 0)
        Database.setSymbolValue("netPres", "NET_PRES_RTOS_STACK_SIZE", 20480)
        Database.setSymbolValue("netPres", "NET_PRES_BLOB_CLIENT_SUPPORT", True)
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_CLIENT_CERT_FILENAME", "wolfssl/certs_test.h")
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_CLIENT_CERT_VARIABLE", "client_cert_der_2048")
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_CLIENT_CERT_LEN_VARIABLE", "sizeof_client_cert_der_2048")
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_SUPPORT", True)
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_CERT_FILENAME", "wolfssl/certs_test.h")
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_CERT_VARIABLE", "server_cert_der_2048")
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_CERT_LEN_VARIABLE", "sizeof_server_cert_der_2048")
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_KEY_FILENAME", "wolfssl/certs_test.h")
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_KEY_VARIABLE", "server_key_der_2048")
        # Database.setSymbolValue("netPres", "NET_PRES_BLOB_SERVER_KEY_LEN_VARIABLE", "sizeof_server_key_der_2048")
        Database.setSymbolValue("lib_wolfssl", "wolfsslDTLS", False)
        Database.setSymbolValue("lib_wolfssl", "wolfsslTLS13", True)
        Database.setSymbolValue("lib_wolfssl", "wolfsslHkdf", True)
        Database.setSymbolValue("lib_wolfssl", "wolfsslNoErrorStrings", True)
        Database.setSymbolValue("lib_wolfssl", "wolfsslUseFastMath", True)
        Database.setSymbolValue("lib_wolfssl", "wolfsslFfdheGroup2048", True)
        # Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_hw", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_md4", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_md5_hw", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_sha1_hw", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_sha264_hw", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_hmac", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_hkdf", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_tdes", False)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_aes_hw", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_aes_ecb_hw", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_aes_ccm", False)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_kdf", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_eccencrypt", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_rsaPss", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_dh", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_keygen", False)
        # Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_errorstrings", True)
        # Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_debug", True)
        if (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"):
            Database.setSymbolValue("lib_wolfcrypt", "wolfSslRTOSSupport", "FreeRTOS")
        else:
            Database.setSymbolValue("lib_wolfcrypt", "wolfSslRTOSSupport", "Single Threaded")
    else:
        Database.setSymbolValue("netPres_0", "NET_PRES_SUPPORT_ENCRYPTION0", False)
        Database.setSymbolValue("netPres_0", "NET_PRES_ENC_PROVIDE_IDX0", 0)
        res = Database.deactivateComponents(["lib_wolfssl"])
        res = Database.deactivateComponents(["tcpipSntp"])

def finalizeComponent(netComponent):
    #Database.setSymbolValue("core", "XC32_HEAP_SIZE", 160000)
    triggerDict = {}
    triggerDict = Database.sendMessage("core", "HEAP_SIZE", {"heap_size" : 160000})

    res = Database.activateComponents(["sysWifiPic32mzw1"])
    res = Database.activateComponents(["netPres"],"System Component", True)

    # Enable dependent Harmony core components
    if (Database.getSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON", True)

    if (Database.getSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON", True)

    if (Database.getSymbolValue("HarmonyCore", "SELECT_RTOS") != "BareMetal"):
        Database.setSymbolValue("lib_wolfcrypt", "wolfSslRTOSSupport", "FreeRTOS")
        #Database.setSymbolValue("sys_command", "SYS_COMMAND_RTOS_STACK_SIZE", 4096)
        #Database.setSymbolValue("FreeRTOS", "FREERTOS_MEMORY_MANAGEMENT_CHOICE", "Heap_3")
    else:
        Database.setSymbolValue("lib_wolfcrypt", "wolfSslRTOSSupport", "Single Threaded")
		
#    Database.setSymbolValue("tcpipStack", "TCPIP_STACK_DRAM_SIZE", 80000)
#    Database.setSymbolValue("core", "RNG_CLOCK_ENABLE", True)
#    Database.setSymbolValue("core", "CRYPTO_CLOCK_ENABLE", True)

