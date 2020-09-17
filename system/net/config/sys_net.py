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

    netIpProt = netComponent.createComboSymbol("SYS_NET_IPPROT", None, ["UDP", "TCP"])
    netIpProt.setLabel("Ip Protocol")
    netIpProt.setDefaultValue("TCP")
	
    netMode = netComponent.createComboSymbol("SYS_NET_MODE", None, ["CLIENT", "SERVER"])
    netMode.setLabel("Mode")
    netMode.setDefaultValue("CLIENT")

    netReConnect = netComponent.createBooleanSymbol("SYS_NET_RECONNECT", None)
    netReConnect.setLabel("Enable Auto Connect")
    netReConnect.setDefaultValue(True)
	
    netEnableTls = netComponent.createBooleanSymbol("SYS_NET_ENABLE_TLS", None)
    netEnableTls.setLabel("Enable TLS")
    netEnableTls.setDefaultValue(False)
    netEnableTls.setDependencies(netTLSautoMenu, ["SYS_NET_ENABLE_TLS"])

	
    netPort = netComponent.createIntegerSymbol("SYS_NET_PORT", None)
    netPort.setLabel("Server Port")
    netPort.setMin(1)
    netPort.setMax(65535)
	
    netHostName = netComponent.createStringSymbol("SYS_NET_HOST_NAME", None)
    netHostName.setLabel("Host Name/ IP Address")
    netHostName.setVisible(True)
    netHostName.setDescription("Host Name/ IP")
    netHostName.setDefaultValue("192.168.1.1")
	
    netDebugLogEnable = netComponent.createBooleanSymbol("SYS_NET_APPDEBUG_ENABLE", None)
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
        Database.setSymbolValue("lib_wolfcrypt", "wolfSslRTOSSupport", "FreeRTOS")
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_keygen", False)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_errorstrings", True)
        Database.setSymbolValue("lib_wolfcrypt", "wolfcrypt_debug", True)
    else:
        Database.setSymbolValue("netPres_0", "NET_PRES_SUPPORT_ENCRYPTION0", False)
        Database.setSymbolValue("netPres_0", "NET_PRES_ENC_PROVIDE_IDX0", 0)
        res = Database.deactivateComponents(["lib_wolfssl"])
        res = Database.deactivateComponents(["tcpipSntp"])

def finalizeComponent(netComponent):
    #Database.setSymbolValue("core", "XC32_HEAP_SIZE", 160000)
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

