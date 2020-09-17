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
def instantiateComponent(mqttComponent):

    # Enable dependent Harmony core components
    if (Database.getSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON", True)

    if (Database.getSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON", True)

    # Basic Configuration
    sysMqttBasicMenu = mqttComponent.createMenuSymbol("SYS_MQTT_BASIC_CONFIG_MENU", None)
    sysMqttBasicMenu.setLabel("Basic Configuration")
    sysMqttBasicMenu.setVisible(True)
		
    mqttBrokerName = mqttComponent.createStringSymbol("SYS_MQTT_BROKER_NAME", sysMqttBasicMenu)
    mqttBrokerName.setLabel("Broker Name")
    mqttBrokerName.setVisible(True)
    mqttBrokerName.setDescription("MQTT Broker Name")
    mqttBrokerName.setDefaultValue("test.mosquitto.org")

    mqttPort = mqttComponent.createIntegerSymbol("SYS_MQTT_MQTT_PORT", sysMqttBasicMenu)
    mqttPort.setLabel("Server Port")
    mqttPort.setMin(1)
    mqttPort.setMax(65535)
    mqttPort.setDescription("MQTT Broker Port")
    mqttPort.setDefaultValue(1883)

    mqttEnableTls = mqttComponent.createBooleanSymbol("SYS_MQTT_ENABLE_TLS", sysMqttBasicMenu)
    mqttEnableTls.setLabel("Enable TLS")
    mqttEnableTls.setDescription("For TLS MQTT Connection")
    mqttEnableTls.setDefaultValue(False)
    mqttEnableTls.setDependencies(mqttTlsAutoMenu, ["SYS_MQTT_ENABLE_TLS"])
	
    mqttClientId = mqttComponent.createStringSymbol("SYS_MQTT_CLIENT_ID", sysMqttBasicMenu)
    mqttClientId.setLabel("Client Id")
    mqttClientId.setVisible(True)
    mqttClientId.setDescription("MQTT Client Id which should be unique for the MQTT Broker. If empty, the id will be randomly generated")

    # Advanced Configuration
    sysMqttAdvMenu = mqttComponent.createMenuSymbol("SYS_MQTT_ADVANCED_CONFIG_MENU", None)
    sysMqttAdvMenu.setLabel("Advanced Configuration")
    sysMqttAdvMenu.setVisible(True)
		
    mqttReConnect = mqttComponent.createBooleanSymbol("SYS_MQTT_RECONNECT", sysMqttAdvMenu)
    mqttReConnect.setLabel("Enable Auto Reconnect")
    mqttReConnect.setDefaultValue(True)
    mqttReConnect.setDescription("In case of disconnection, enabling Auto Reconnect ensures that the Client reconnects to the MQTT Broker")
	
    mqttEnableCleanSession = mqttComponent.createBooleanSymbol("SYS_MQTT_CLEAN_SESSION", sysMqttAdvMenu)
    mqttEnableCleanSession.setLabel("Enable Clean Sesison")
    mqttEnableCleanSession.setDescription("Enable Clean Sesison for non persistent connection")
    mqttEnableCleanSession.setDefaultValue(True)

    mqttKeepAliveInterval = mqttComponent.createIntegerSymbol("SYS_MQTT_KEEPALIVE_INTERVAL", sysMqttAdvMenu)
    mqttKeepAliveInterval.setLabel("KeepAlive Interval")
    mqttKeepAliveInterval.setMin(0)
    mqttKeepAliveInterval.setMax(65535)
    mqttKeepAliveInterval.setDescription("KeepAlive Interval ensures that the broker and the client are aware of being connected")
    mqttKeepAliveInterval.setDefaultValue(60)

    mqttExtraCredentials = mqttComponent.createBooleanSymbol("SYS_MQTT_EXTRA_CRED", sysMqttAdvMenu)
    mqttExtraCredentials.setLabel("Extra Credentials")
    mqttExtraCredentials.setVisible(True)
    mqttExtraCredentials.setDefaultValue(False)

    mqttUserName = mqttComponent.createStringSymbol("SYS_MQTT_USER_NAME", mqttExtraCredentials)
    mqttUserName.setLabel("Username")
    mqttUserName.setVisible(True)
    mqttUserName.setDescription("Username")

    mqttPassword = mqttComponent.createStringSymbol("SYS_MQTT_PASSWORD", mqttExtraCredentials)
    mqttPassword.setLabel("Password")
    mqttPassword.setVisible(True)
    mqttPassword.setDescription("Password")
	
    sysMqttLwtEnable = mqttComponent.createBooleanSymbol("SYS_MQTT_LWT_ENABLE", sysMqttAdvMenu)
    sysMqttLwtEnable.setLabel("Last Will and Testament")
    sysMqttLwtEnable.setVisible(True)
    sysMqttLwtEnable.setDescription("Used by Broker to notify other clients about our ungracefully disconnection")
    sysMqttLwtEnable.setDefaultValue(False)

	# Topic Name
    sysLwtTopicName = mqttComponent.createStringSymbol("SYS_MQTT_LWT_TOPIC_NAME", sysMqttLwtEnable)
    sysLwtTopicName.setLabel("Lwt Topic")
    sysLwtTopicName.setVisible(True)
    sysLwtTopicName.setDescription("Topic Name")

    # Qos
    sysLwtTopicLen = mqttComponent.createComboSymbol("SYS_MQTT_LWT_QOS", sysMqttLwtEnable, ["At most once (0)", "At least once (1)", "Exactly once (2)"])
    sysLwtTopicLen.setLabel("Lwt QOS")
    sysLwtTopicLen.setVisible(True)
    sysLwtTopicLen.setDefaultValue("At least once (1)")
    
	#Retain
    sysLwtRetain = mqttComponent.createBooleanSymbol("SYS_MQTT_LWT_RETAIN", sysMqttLwtEnable)
    sysLwtRetain.setLabel("Lwt Retain Flag")
    sysLwtRetain.setVisible(True)
    sysLwtRetain.setDefaultValue(False)
	
	#Message
    sysLwtMsg = mqttComponent.createStringSymbol("SYS_MQTT_LWT_MESSAGE", sysMqttLwtEnable)
    sysLwtMsg.setLabel("Lwt Message")
    sysLwtMsg.setVisible(True)
    sysLwtTopicName.setDescription("Lwt Message")
	
    sysMqttSubEnable = mqttComponent.createBooleanSymbol("SYS_MQTT_SUB_ENABLE", None)
    sysMqttSubEnable.setLabel("Subscribe Topic")
    sysMqttSubEnable.setVisible(True)
    sysMqttSubEnable.setDefaultValue(False)

	# Topic Name
    sysSubTopicName = mqttComponent.createStringSymbol("SYS_MQTT_SUB_TOPIC_NAME", sysMqttSubEnable)
    sysSubTopicName.setLabel("Topic Name")
    sysSubTopicName.setVisible(True)
    sysSubTopicName.setDescription("Topic Name")

    # Qos
    sysSubTopicLen = mqttComponent.createComboSymbol("SYS_MQTT_SUB_QOS", sysMqttSubEnable, ["At most once (0)", "At least once (1)", "Exactly once (2)"])
    sysSubTopicLen.setLabel("QOS")
    sysSubTopicLen.setVisible(True)
    sysSubTopicLen.setDefaultValue("At least once (1)")
    
    sysMqttPubEnable = mqttComponent.createBooleanSymbol("SYS_MQTT_PUB_ENABLE", None)
    sysMqttPubEnable.setLabel("Publish to Topic")
    sysMqttPubEnable.setVisible(True)
    sysMqttPubEnable.setDefaultValue(False)

	# Topic Name
    sysPubTopicName = mqttComponent.createStringSymbol("SYS_MQTT_PUB_TOPIC_NAME", sysMqttPubEnable)
    sysPubTopicName.setLabel("Topic Name")
    sysPubTopicName.setVisible(True)
    sysPubTopicName.setDescription("Topic Name")

    # Qos
    sysPubTopicLen = mqttComponent.createComboSymbol("SYS_MQTT_PUB_QOS", sysMqttPubEnable, ["At most once (0)", "At least once (1)", "Exactly once (2)"])
    sysPubTopicLen.setLabel("QOS")
    sysPubTopicLen.setVisible(True)
    sysPubTopicLen.setDefaultValue("At least once (1)")
        
    sysMqttPubRetain = mqttComponent.createBooleanSymbol("SYS_MQTT_PUB_RETAIN", sysMqttPubEnable)
    sysMqttPubRetain.setLabel("Retain Message")
    sysMqttPubRetain.setVisible(True)
    sysMqttPubRetain.setDefaultValue(False)
	
    sysMqttDebugLogEnable = mqttComponent.createBooleanSymbol("SYS_MQTT_APPDEBUG_ENABLE", None)
    sysMqttDebugLogEnable.setLabel("Enable Debug Logs")
    sysMqttDebugLogEnable.setDefaultValue(False)
    
    sysMqttDebugBasicMenu = mqttComponent.createMenuSymbol("SYS_MQTT_APPDEBUG_LEVEL_CONFIG_MENU", sysMqttDebugLogEnable)
    sysMqttDebugBasicMenu.setLabel("Debug Level Configuration")
    sysMqttDebugBasicMenu.setVisible(False)
    sysMqttDebugBasicMenu.setDependencies(sysMqttDebugMenuVisible, ["SYS_MQTT_APPDEBUG_ENABLE"])
		
    sysMqttDebugErrLevel = mqttComponent.createBooleanSymbol("SYS_MQTT_APPDEBUG_ERR_LEVEL", sysMqttDebugBasicMenu)
    sysMqttDebugErrLevel.setLabel("Enable Error Level")
    sysMqttDebugErrLevel.setDefaultValue(True)
#    sysMqttDebugErrLevel.setDependencies(sysMqttDebugMenuVisible, ["SYS_MQTT_APPDEBUG_ENABLE"])

    sysMqttDebugDbgLevel = mqttComponent.createBooleanSymbol("SYS_MQTT_APPDEBUG_DBG_LEVEL", sysMqttDebugBasicMenu)
    sysMqttDebugDbgLevel.setLabel("Enable Debug Level")
    sysMqttDebugDbgLevel.setDefaultValue(False)
#    sysMqttDebugDbgLevel.setDependencies(sysMqttDebugMenuVisible, ["SYS_MQTT_APPDEBUG_ENABLE"])
	
    sysMqttDebugInfoLevel = mqttComponent.createBooleanSymbol("SYS_MQTT_APPDEBUG_INFO_LEVEL", sysMqttDebugBasicMenu)
    sysMqttDebugInfoLevel.setLabel("Enable Info Level")
    sysMqttDebugInfoLevel.setDefaultValue(False)
#    sysMqttDebugInfoLevel.setDependencies(sysMqttDebugMenuVisible, ["SYS_MQTT_APPDEBUG_ENABLE"])
	
    sysMqttDebugFuncLevel = mqttComponent.createBooleanSymbol("SYS_MQTT_APPDEBUG_FUNC_LEVEL", sysMqttDebugBasicMenu)
    sysMqttDebugFuncLevel.setLabel("Enable Function Entry/Exit Level")
    sysMqttDebugFuncLevel.setDefaultValue(False)
#    sysMqttDebugFuncLevel.setDependencies(sysMqttDebugMenuVisible, ["SYS_MQTT_APPDEBUG_ENABLE"])
	
    sysMqttDebugFlowBasicMenu = mqttComponent.createMenuSymbol("SYS_MQTT_APPDEBUG_FLOW_CONFIG_MENU", sysMqttDebugLogEnable)
    sysMqttDebugFlowBasicMenu.setLabel("Debug Flow Configuration")
    sysMqttDebugFlowBasicMenu.setVisible(True)
    sysMqttDebugFlowBasicMenu.setDependencies(sysMqttDebugMenuVisible, ["SYS_MQTT_APPDEBUG_ENABLE"])
		
    sysMqttDebugCfgFlow = mqttComponent.createBooleanSymbol("SYS_MQTT_APPDEBUG_CFG_FLOW", sysMqttDebugFlowBasicMenu)
    sysMqttDebugCfgFlow.setLabel("Enable MQTT Cfg Flow")
    sysMqttDebugCfgFlow.setDefaultValue(True)

    sysMqttDebugDataFlow = mqttComponent.createBooleanSymbol("SYS_MQTT_APPDEBUG_DATA_FLOW", sysMqttDebugFlowBasicMenu)
    sysMqttDebugDataFlow.setLabel("Enable MQTT Data Flow")
    sysMqttDebugDataFlow.setDefaultValue(True)
	
    sysMqttDebugPahoFlow = mqttComponent.createBooleanSymbol("SYS_MQTT_APPDEBUG_PAHO_FLOW", sysMqttDebugFlowBasicMenu)
    sysMqttDebugPahoFlow.setLabel("Enable MQTT Paho Flow")
    sysMqttDebugPahoFlow.setDefaultValue(True)
	
    sysMqttDebugPreStr = mqttComponent.createStringSymbol("SYS_MQTT_APPDEBUG_PRESTR", sysMqttDebugLogEnable)
    sysMqttDebugPreStr.setLabel("Prefix String")
    sysMqttDebugPreStr.setVisible(True)
    sysMqttDebugPreStr.setDescription("Prefix String")
    sysMqttDebugPreStr.setDefaultValue("MQTT_SRVC")
    sysMqttDebugPreStr.setDependencies(sysMqttDebugMenuVisible, ["SYS_MQTT_APPDEBUG_ENABLE"])

	
    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    mqttHeaderFile = mqttComponent.createFileSymbol("SYS_MQTT_HEADER", None)
    mqttHeaderFile.setSourcePath("../wireless/system/mqtt/sys_mqtt.h")
    mqttHeaderFile.setOutputName("sys_mqtt.h")
    mqttHeaderFile.setDestPath("system/mqtt/")
    mqttHeaderFile.setProjectPath("config/" + configName + "/system/mqtt/")
    mqttHeaderFile.setType("HEADER")
    mqttHeaderFile.setOverwrite(True)

    print("Mqtt Service Component Header Path %s", mqttHeaderFile.getProjectPath())
	
    mqttSourceFile = mqttComponent.createFileSymbol("SYS_MQTT_SOURCE", None)
    mqttSourceFile.setSourcePath("../wireless/system/mqtt/src/sys_mqtt.c")
    mqttSourceFile.setOutputName("sys_mqtt.c")
    mqttSourceFile.setDestPath("system/mqtt/src")
    mqttSourceFile.setProjectPath("config/" + configName + "/system/mqtt/")
    mqttSourceFile.setType("SOURCE")
    mqttSourceFile.setOverwrite(True)

    mqttPahoHeaderFile = mqttComponent.createFileSymbol("SYS_MQTT_PAHO_HEADER", None)
    mqttPahoHeaderFile.setSourcePath("../wireless/system/mqtt/sys_mqtt_paho.h")
    mqttPahoHeaderFile.setOutputName("sys_mqtt_paho.h")
    mqttPahoHeaderFile.setDestPath("system/mqtt/")
    mqttPahoHeaderFile.setProjectPath("config/" + configName + "/system/mqtt/")
    mqttPahoHeaderFile.setType("HEADER")
    mqttPahoHeaderFile.setOverwrite(True)

    print("Mqtt Service Component Header Path %s", mqttHeaderFile.getProjectPath())
	
    mqttPahoSourceFile = mqttComponent.createFileSymbol("SYS_MQTT_PAHO_SOURCE", None)
    mqttPahoSourceFile.setSourcePath("../wireless/system/mqtt/src/sys_mqtt_paho.c")
    mqttPahoSourceFile.setOutputName("sys_mqtt_paho.c")
    mqttPahoSourceFile.setDestPath("system/mqtt/src")
    mqttPahoSourceFile.setProjectPath("config/" + configName + "/system/mqtt/")
    mqttPahoSourceFile.setType("SOURCE")
    mqttPahoSourceFile.setOverwrite(True)
	
    mqttSystemDefFile = mqttComponent.createFileSymbol("SYS_MQTT_DEF", None)
    mqttSystemDefFile.setType("STRING")
    mqttSystemDefFile.setOutputName("core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES")
    mqttSystemDefFile.setSourcePath("../wireless/system/mqtt/templates/system/system_definitions.h.ftl")
    mqttSystemDefFile.setMarkup(True)
	
    mqttSystemConfigFile = mqttComponent.createFileSymbol("SYS_CONSOLE_SYS_CONFIG", None)
    mqttSystemConfigFile.setType("STRING")
    mqttSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_SYSTEM_SERVICE_CONFIGURATION")
    mqttSystemConfigFile.setSourcePath("../wireless/system/mqtt/templates/system/system_config.h.ftl")
    mqttSystemConfigFile.setMarkup(True)

    mqttSystemInitDataFile = mqttComponent.createFileSymbol("SYS_MQTT_INIT_DATA", None)
    mqttSystemInitDataFile.setType("STRING")
    mqttSystemInitDataFile.setOutputName("core.LIST_SYSTEM_INIT_C_SYSTEM_INITIALIZATION")
    mqttSystemInitDataFile.setSourcePath("../wireless/system/mqtt/templates/system/system_initialize_data.c.ftl")
    mqttSystemInitDataFile.setMarkup(True)

    mqttSystemInitFile = mqttComponent.createFileSymbol("SYS_MQTT_INIT", None)
    mqttSystemInitFile.setType("STRING")
    mqttSystemInitFile.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_SYSTEM_SERVICES")
    mqttSystemInitFile.setSourcePath("../wireless/system/mqtt/templates/system/system_initialize.c.ftl")
    mqttSystemInitFile.setMarkup(True)
	
############################################################################
#### Dependency ####
############################################################################

def sysMqttDebugMenuVisible(symbol, event):
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

def mqttTlsAutoMenu(symbol, event):
    if (event["value"] == True):
        Database.setSymbolValue("sysNetPic32mzw1", "SYS_NET_ENABLE_TLS", True)
    else:
        Database.setSymbolValue("sysNetPic32mzw1", "SYS_NET_ENABLE_TLS", False)

def finalizeComponent(mqttComponent):
    res = Database.activateComponents(["sysNetPic32mzw1"])
    res = Database.activateComponents(["lib_pahomqtt"],"System Component", True)
#    res = Database.activateComponents(["sysAppDebugPic32mzw1"],"System Component", True)
	
#    if(Database.getSymbolValue("sysMqttPic32mzw1", "SYS_MQTT_APPDEBUG_ENABLE") == True):
#       res = Database.activateComponents(["sysAppDebugPic32mzw1"])

