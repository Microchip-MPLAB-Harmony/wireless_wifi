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
def instantiateComponent(appdebugComponent):

    # Enable dependent Harmony core components
    if (Database.getSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_SYS_COMMON", True)

    if (Database.getSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON") == False):
        Database.clearSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON")
        Database.setSymbolValue("HarmonyCore", "ENABLE_DRV_COMMON", True)
	
    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    appdebugHeaderFile = appdebugComponent.createFileSymbol("SYS_APPDEBUG_HEADER", None)
    appdebugHeaderFile.setSourcePath("../wireless/system/appdebug/sys_appdebug.h")
    appdebugHeaderFile.setOutputName("sys_appdebug.h")
    appdebugHeaderFile.setDestPath("system/appdebug/")
    appdebugHeaderFile.setProjectPath("config/" + configName + "/system/appdebug/")
    appdebugHeaderFile.setType("HEADER")
    appdebugHeaderFile.setOverwrite(True)

    print("Network Service Component Header Path %s", appdebugHeaderFile.getProjectPath())
	
    appdebugSourceFile = appdebugComponent.createFileSymbol("SYS_APPDEBUG_SOURCE", None)
    appdebugSourceFile.setSourcePath("../wireless/system/appdebug/src/sys_appdebug.c")
    appdebugSourceFile.setOutputName("sys_appdebug.c")
    appdebugSourceFile.setDestPath("system/appdebug/src")
    appdebugSourceFile.setProjectPath("config/" + configName + "/system/appdebug/")
    appdebugSourceFile.setType("SOURCE")
    appdebugSourceFile.setOverwrite(True)

    appdebugSystemConfigFile = appdebugComponent.createFileSymbol("SYS_CONSOLE_SYS_CONFIG", None)
    appdebugSystemConfigFile.setType("STRING")
    appdebugSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_SYSTEM_SERVICE_CONFIGURATION")
    appdebugSystemConfigFile.setSourcePath("../wireless/system/appdebug/templates/system/system_config.h.ftl")
    appdebugSystemConfigFile.setMarkup(True)

    appdebugSystemInitFile = appdebugComponent.createFileSymbol("SYS_APPDEBUG_INIT", None)
    appdebugSystemInitFile.setType("STRING")
    appdebugSystemInitFile.setOutputName("core.LIST_SYSTEM_INIT_C_INITIALIZE_SYSTEM_SERVICES")
    appdebugSystemInitFile.setSourcePath("../wireless/system/appdebug/templates/system/system_initialize.c.ftl")
    appdebugSystemInitFile.setMarkup(True)
	
    appdebugSystemDefFile = appdebugComponent.createFileSymbol('SYS_APPDEBUG_SYSTEM_DEF', None)
    appdebugSystemDefFile.setType('STRING')
    appdebugSystemDefFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS')
    appdebugSystemDefFile.setSourcePath('../wireless/system/appdebug/templates/system/system_definitions_objects.h.ftl')
    appdebugSystemDefFile.setMarkup(True)
	
    appdebugSystemDefFile = appdebugComponent.createFileSymbol('SYS_APPDEBUG_DEF', None)
    appdebugSystemDefFile.setType('STRING')
    appdebugSystemDefFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES')
    appdebugSystemDefFile.setSourcePath('../wireless/system/appdebug/templates/system/system_definitions.h.ftl')
    appdebugSystemDefFile.setMarkup(True)
	
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

