"""
Copyright (C) 2020-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
"""

################################################################################
#### Global Variables ####
################################################################################

global interruptsChildren
interruptsChildren = ATDF.getNode('/avr-tools-device-file/devices/device/interrupts').getChildren()

################################################################################
#### Business Logic ####
################################################################################

def importIncFile(component, configName, incFileEntry):
    incFilePath  = incFileEntry[0]
    isEnabled    = incFileEntry[1][0]
    callback     = incFileEntry[1][1]
    dependencies = incFileEntry[1][2]

    incFilePathTup = incFilePath.split('/')

    if len(incFilePathTup) == 1:
        secName = ''
        incFile = incFilePathTup[0]
    else:
        secName = incFilePathTup[0]
        incFile = incFilePathTup[1]

    symName = incFile.replace('.', '_').upper()
    secSName = secName + '/'
    secDName = secSName

    incFileSym = component.createFileSymbol(symName, None)
    incFileSym.setSourcePath('driver/pic32mzw1/' + secSName + 'include/' + incFile)
    incFileSym.setOutputName(incFile)
    incFileSym.setDestPath('driver/wifi/pic32mzw1/include/' + secDName)
    incFileSym.setProjectPath('config/' + configName + '/driver/wifi/pic32mzw1/include/' + secDName)
    incFileSym.setType('HEADER')
    incFileSym.setOverwrite(True)
    incFileSym.setEnabled(isEnabled)

    if callback and dependencies:
        incFileSym.setDependencies(callback, dependencies)

def importSrcFile(component, configName, srcFileEntry):
    srcFilePath  = srcFileEntry[0]
    isEnabled    = srcFileEntry[1][0]
    callback     = srcFileEntry[1][1]
    dependencies = srcFileEntry[1][2]

    srcFilePathTup = srcFilePath.rsplit('/', 1)

    if len(srcFilePathTup) == 1:
        secName = ''
        srcFile = srcFilePathTup[0]
    else:
        secName = srcFilePathTup[0]
        srcFile = srcFilePathTup[1]

    srcFilePrefix   = ''
    symName = srcFile.replace('.', '_').upper()
    secSName = secName + '/'
    secDName = secSName

    srcFileSym = component.createFileSymbol(symName, None)
    srcFileSym.setSourcePath('driver/pic32mzw1/' + secSName + srcFile)
    srcFileSym.setOutputName(srcFile.rsplit('/', 1)[-1])
    srcFileSym.setDestPath('driver/wifi/pic32mzw1/' + secDName)
    srcFileSym.setProjectPath('config/' + configName + '/driver/wifi/pic32mzw1/' + secDName)
    srcFileSym.setType('SOURCE')
    srcFileSym.setEnabled(isEnabled)

    if callback and dependencies:
        srcFileSym.setDependencies(callback, dependencies)

def setIncPath(component, configName, incPathEntry):
    incPath      = incPathEntry[0]
    isEnabled    = incPathEntry[1][0]
    callback     = incPathEntry[1][1]
    dependencies = incPathEntry[1][2]

    incPathSym = component.createSettingSymbol('PIC32MZW1_INC_PATH' + incPath.replace('.', '_').replace('/', '_').upper(), None)
    incPathSym.setValue('../src/config/' + configName + '/driver/wifi/pic32mzw1/include' + incPath + ';')
    incPathSym.setCategory('C32')
    incPathSym.setKey('extra-include-directories')
    incPathSym.setAppend(True, ';')
    incPathSym.setEnabled(isEnabled)
    incPathSym.setDependencies(callback, dependencies)

def onAttachmentConnected(source, target):
    if source['id'] == 'sys_debug':
        pic32mzw1UseSysDebug = source['component'].getSymbolByID('DRV_WIFI_PIC32MZW1_USE_SYS_DEBUG')
        pic32mzw1UseSysDebug.setValue(True)

def onAttachmentDisconnected(source, target):
    if source['id'] == 'sys_debug':
        pic32mzw1UseSysDebug = source['component'].getSymbolByID('DRV_WIFI_PIC32MZW1_USE_SYS_DEBUG')
        pic32mzw1UseSysDebug.setValue(False)

################################################################################
#### Component ####
################################################################################

def instantiateComponent(drvPic32mzw1Component):
    print('PIC32MZW1 Driver Component')
    configName = Variables.get('__CONFIGURATION_NAME')

    Database.activateComponents(['HarmonyCore', 'lib_crypto', 'lib_wolfcrypt', 'tcpipNetConfig', 'tcpipStack', 'lib_wolfssl'])

    drvPic32mzw1Component.addDependency('Crypto_PIC32MZW1_Dependency', 'LIB_CRYPTO', True, True)
    drvPic32mzw1Component.addDependency('BA414E_Dependency', 'DRV_BA414E', False, True)
    drvPic32mzw1Component.addDependency('BigIntSw_Dependency', 'LIB_WOLFCRYPT', False, True)
    drvPic32mzw1Component.addDependency('sys_debug', 'SYS_DEBUG', True, False)
    drvPic32mzw1Component.addDependency('Wolfssl_PIC32MZW1_Dependency', 'TLS Provider', False, False)
    drvPic32mzw1Component.setDependencyEnabled('Wolfssl_PIC32MZW1_Dependency', False)

    drvPic32mzw1Component.setCapabilityEnabled('libdrvPic32mzw1Mac', True)

    Database.setSymbolValue('core', 'RFMAC_INTERRUPT_ENABLE', True, 1)
    Database.setSymbolValue('core', 'RFMAC_INTERRUPT_HANDLER_LOCK', True, 1)
    Database.setSymbolValue('core', 'RFMAC_INTERRUPT_HANDLER', 'WDRV_PIC32MZW_TasksRFMACISR', 1)

    Database.setSymbolValue('core', 'RFTM0_INTERRUPT_ENABLE', True, 1)
    Database.setSymbolValue('core', 'RFTM0_INTERRUPT_HANDLER_LOCK', True, 1)
    Database.setSymbolValue('core', 'RFTM0_INTERRUPT_HANDLER', 'WDRV_PIC32MZW_TasksRFTimer0ISR', 1)

    Database.setSymbolValue('core', 'RFSMC_INTERRUPT_ENABLE', True, 1)
    Database.setSymbolValue('core', 'RFSMC_INTERRUPT_HANDLER_LOCK', True, 1)
    Database.setSymbolValue('core', 'RFSMC_INTERRUPT_HANDLER', 'WDRV_PIC32MZW_TasksRFSMCISR', 1)

    # Enable dependent Harmony core components
    if Database.getSymbolValue('HarmonyCore', 'ENABLE_DRV_COMMON') == False:
        Database.setSymbolValue('HarmonyCore', 'ENABLE_DRV_COMMON', True)

    if Database.getSymbolValue('HarmonyCore', 'ENABLE_SYS_COMMON') == False:
        Database.setSymbolValue('HarmonyCore', 'ENABLE_SYS_COMMON', True)

    if Database.getSymbolValue('HarmonyCore', 'ENABLE_SYS_INT') == False:
        Database.setSymbolValue('HarmonyCore', 'ENABLE_SYS_INT', True)

    if Database.getSymbolValue('HarmonyCore', 'ENABLE_OSAL') == False:
        Database.setSymbolValue('HarmonyCore', 'ENABLE_OSAL', True)

    if Database.getSymbolValue('lib_wolfcrypt', 'wolfcrypt_sha256') == False:
        Database.setSymbolValue('lib_wolfcrypt', 'wolfcrypt_sha256', True)

    pic32mzw1UseSysDebug = drvPic32mzw1Component.createBooleanSymbol('DRV_WIFI_PIC32MZW1_USE_SYS_DEBUG', None)
    pic32mzw1UseSysDebug.setVisible(False)
    pic32mzw1UseSysDebug.setDefaultValue(False)

    # Log Level
    pic32mzw1LogLevel = drvPic32mzw1Component.createComboSymbol('DRV_WIFI_PIC32MZW1_LOG_LEVEL', None, ['None', 'Error', 'Inform', 'Trace', 'Verbose'])
    pic32mzw1LogLevel.setLabel('Driver Log Level')
    pic32mzw1LogLevel.setVisible(True)
    pic32mzw1LogLevel.setDefaultValue('Trace')
    pic32mzw1LogLevel.setDependencies(setEnableLogLevel, ['DRV_WIFI_PIC32MZW1_USE_SYS_DEBUG', 'sys_debug.SYS_DEBUG_USE_CONSOLE'])

    # RTOS Configuration
    pic32mzw1RtosMenu = drvPic32mzw1Component.createMenuSymbol('DRV_WIFI_PIC32MZW1_RTOS_MENU', None)
    pic32mzw1RtosMenu.setLabel('RTOS Configuration')
    pic32mzw1RtosMenu.setDescription('RTOS Configuration')
    pic32mzw1RtosMenu.setVisible(False)
    pic32mzw1RtosMenu.setVisible((Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != 'BareMetal') and (Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != None))
    pic32mzw1RtosMenu.setDependencies(setVisibilityRTOSMenu, ['HarmonyCore.SELECT_RTOS'])

    # RTOS Execution Mode
    pic32mzw1InstnExecMode = drvPic32mzw1Component.createComboSymbol('DRV_WIFI_PIC32MZW1_RTOS', pic32mzw1RtosMenu, ['Standalone'])
    pic32mzw1InstnExecMode.setLabel('Run Driver Tasks as')
    pic32mzw1InstnExecMode.setVisible(False)
    pic32mzw1InstnExecMode.setDescription('WiFi PIC32MZW1 Driver RTOS Execution Mode')
    pic32mzw1InstnExecMode.setDefaultValue('Standalone')

    # WiFi Driver Task Stack Size
    pic32mzw1TaskSize = drvPic32mzw1Component.createIntegerSymbol('DRV_WIFI_PIC32MZW1_RTOS_STACK_SIZE', pic32mzw1RtosMenu)
    pic32mzw1TaskSize.setLabel('Task Size')
    pic32mzw1TaskSize.setVisible(True)
    pic32mzw1TaskSize.setDescription('WiFi PIC32MZW1 Driver RTOS Task Stack Size')
    pic32mzw1TaskSize.setDefaultValue(1024)
    pic32mzw1TaskSize.setDependencies(setVisibilityRTOSTaskConfig, ['DRV_WIFI_PIC32MZW1_RTOS'])

    # WiFi Driver Task Priority
    pic32mzw1TaskPriority = drvPic32mzw1Component.createIntegerSymbol('DRV_WIFI_PIC32MZW1_RTOS_TASK_PRIORITY', pic32mzw1RtosMenu)
    pic32mzw1TaskPriority.setLabel('Task Priority')
    pic32mzw1TaskPriority.setVisible(True)
    pic32mzw1TaskPriority.setDescription('WiFi PIC32MZW1 Driver RTOS Task Priority')
    pic32mzw1TaskPriority.setDefaultValue(1)
    pic32mzw1TaskPriority.setDependencies(setVisibilityRTOSTaskConfig, ['DRV_WIFI_PIC32MZW1_RTOS'])

    # Support SAE (WPA3 Personal)?
    pic32mzw1SupportSae = drvPic32mzw1Component.createBooleanSymbol('DRV_WIFI_PIC32MZW1_SUPPORT_SAE', None)
    pic32mzw1SupportSae.setLabel('Support WPA3 Personal?')
    pic32mzw1SupportSae.setDescription('Support for WPA3 Personal security. Requires BA414E Cryptographic Accelerator from crypto release v3.6.2 or later. Also requires wolfCrypt support.')
    pic32mzw1SupportSae.setVisible(True)
    pic32mzw1SupportSae.setDefaultValue(True)
    pic32mzw1SupportSae.setDependencies(setWpa3dependency, ['DRV_WIFI_PIC32MZW1_SUPPORT_SAE'])

    # Require BA414E hardware driver?
    pic32mzw1RequireBa414e = drvPic32mzw1Component.createBooleanSymbol('DRV_WIFI_PIC32MZW1_REQUIRE_BA414E', None)
    pic32mzw1RequireBa414e.setVisible(False)
    pic32mzw1RequireBa414e.setDefaultValue(True)
    pic32mzw1RequireBa414e.setDependencies(setRequireBa414e, ['DRV_WIFI_PIC32MZW1_SUPPORT_SAE'])

    # Require BigIntSw?
    pic32mzw1RequireBigIntSw = drvPic32mzw1Component.createBooleanSymbol('DRV_WIFI_PIC32MZW1_REQUIRE_BIGINTSW', None)
    pic32mzw1RequireBigIntSw.setVisible(False)
    pic32mzw1RequireBigIntSw.setDefaultValue(True)
    pic32mzw1RequireBigIntSw.setDependencies(setRequireBigIntSw, ['DRV_WIFI_PIC32MZW1_SUPPORT_SAE'])

    # Support Enterprise security?
    pic32mzw1SupportEntSec = drvPic32mzw1Component.createBooleanSymbol('DRV_WIFI_PIC32MZW1_SUPPORT_ENTERPRISE', None)
    pic32mzw1SupportEntSec.setLabel('Support Enterprise?')
    pic32mzw1SupportEntSec.setDescription('Support for WPA, WPA2 and WPA3 Enterprise security. Requires wolfSSL support.')
    pic32mzw1SupportEntSec.setVisible(True)
    pic32mzw1SupportEntSec.setDefaultValue(False)

    # Require Wolfssl
    pic32mzw1RequireWolfssl = drvPic32mzw1Component.createBooleanSymbol('DRV_WIFI_PIC32MZW1_REQUIRE_WOLFSSL', None)
    pic32mzw1RequireWolfssl.setVisible(False)
    pic32mzw1RequireWolfssl.setDefaultValue(False)
    pic32mzw1RequireWolfssl.setDependencies(setRequireWolfssl, ['DRV_WIFI_PIC32MZW1_SUPPORT_ENTERPRISE'])

    # WiFi Regulatory Domain name
    pic32mzw1RegDomain = drvPic32mzw1Component.createComboSymbol('DRV_WIFI_PIC32MZW1_REG_DOMAIN', None, ['None', 'GEN', 'USA', 'EMEA', 'CUST1', 'CUST2'])
    pic32mzw1RegDomain.setLabel('Regulatory Domain')
    pic32mzw1RegDomain.setVisible(True)
    pic32mzw1RegDomain.setDefaultValue('GEN')

    pic32mzw1AlarmPeroid1ms = drvPic32mzw1Component.createIntegerSymbol('DRV_WIFI_PIC32MZW1_ALARM_PERIOD_1MS', None)
    pic32mzw1AlarmPeroid1ms.setVisible(False)
    pic32mzw1AlarmPeroid1ms.setDefaultValue(0)
    pic32mzw1AlarmPeroid1ms.setDependencies(updateAlarmPeriod1ms, ['core.CONFIG_SYS_CLK_PBCLK3_FREQ'])

    pic32mzw1AlarmPeroidMax = drvPic32mzw1Component.createIntegerSymbol('DRV_WIFI_PIC32MZW1_ALARM_PERIOD_MAX', None)
    pic32mzw1AlarmPeroidMax.setVisible(False)
    pic32mzw1AlarmPeroidMax.setDefaultValue(0)
    pic32mzw1AlarmPeroidMax.setDependencies(updateAlarmPeriodMax, ['core.CONFIG_SYS_CLK_PBCLK3_FREQ'])

    ############################################################################
    #### Code Generation ####
    ############################################################################

    condAlways = [True, None, []]

    wdrvIncFiles = [
        ['drv_pic32mzw1.h',             condAlways],
        ['drv_pic32mzw1_crypto.h',      condAlways],
        ['drv_pic32mzw1_tls.h',         condAlways],
        ['wdrv_pic32mzw.h',             condAlways],
        ['wdrv_pic32mzw_api.h',         condAlways],
        ['wdrv_pic32mzw_assoc.h',       condAlways],
        ['wdrv_pic32mzw_authctx.h',     condAlways],
        ['wdrv_pic32mzw_bssctx.h',      condAlways],
        ['wdrv_pic32mzw_bssfind.h',     condAlways],
        ['wdrv_pic32mzw_cfg.h',         condAlways],
        ['wdrv_pic32mzw_client_api.h',  condAlways],
        ['wdrv_pic32mzw_common.h',      condAlways],
        ['wdrv_pic32mzw_debug.h',       condAlways],
        ['wdrv_pic32mzw_mac.h',         condAlways],
        ['wdrv_pic32mzw_regdomain.h',   condAlways],
        ['wdrv_pic32mzw_softap.h',      condAlways],
        ['wdrv_pic32mzw_sta.h',         condAlways],
        ['wdrv_pic32mzw_ps.h',          condAlways],
        ['wdrv_pic32mzw_custie.h',      condAlways],
        ['wdrv_pic32mzw_ie.h',          condAlways]
    ]

    for incFileEntry in wdrvIncFiles:
        importIncFile(drvPic32mzw1Component, configName, incFileEntry)

    wdrvSrcFiles = [
        ['drv_pic32mzw1_crypto.c',      condAlways],
        ['drv_pic32mzw1_tls.c',         condAlways],
        ['wdrv_pic32mzw.c',             condAlways],
        ['wdrv_pic32mzw_assoc.c',       condAlways],
        ['wdrv_pic32mzw_authctx.c',     condAlways],
        ['wdrv_pic32mzw_bssctx.c',      condAlways],
        ['wdrv_pic32mzw_bssfind.c',     condAlways],
        ['wdrv_pic32mzw_cfg.c',         condAlways],
        ['wdrv_pic32mzw_int.c',         condAlways],
        ['wdrv_pic32mzw_regdomain.c',   condAlways],
        ['wdrv_pic32mzw_softap.c',      condAlways],
        ['wdrv_pic32mzw_sta.c',         condAlways],
        ['wdrv_pic32mzw_ps.c',          condAlways],
        ['wdrv_pic32mzw_custie.c',      condAlways],
        ['wdrv_pic32mzw_ie.c',          condAlways]
    ]

    for srcFileEntry in wdrvSrcFiles:
        importSrcFile(drvPic32mzw1Component, configName, srcFileEntry)

    wdrvIncPaths = [
        ['/', condAlways]
    ]

    for incPathEntry in wdrvIncPaths:
        setIncPath(drvPic32mzw1Component, configName, incPathEntry)

    pic32mzw1_a = drvPic32mzw1Component.createLibrarySymbol(None, None)
    pic32mzw1_a.setDestPath('driver/wifi/pic32mzw1/lib')
    pic32mzw1_a.setOutputName('pic32mzw1.a')
    pic32mzw1_a.setSourcePath('driver/pic32mzw1/pic32mzw1.a')

    drvpic32mzw1SystemDefFile = drvPic32mzw1Component.createFileSymbol('DRV_WIFI_PIC32MZW1_DEF', None)
    drvpic32mzw1SystemDefFile.setType('STRING')
    drvpic32mzw1SystemDefFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES')
    drvpic32mzw1SystemDefFile.setSourcePath('driver/pic32mzw1/templates/system/system_definitions.h.ftl')
    drvpic32mzw1SystemDefFile.setMarkup(True)

    drvpic32mzw1SystemDefObjFile = drvPic32mzw1Component.createFileSymbol('DRV_WIFI_PIC32MZW1_SYSTEM_DEF_OBJECT', None)
    drvpic32mzw1SystemDefObjFile.setType('STRING')
    drvpic32mzw1SystemDefObjFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS')
    drvpic32mzw1SystemDefObjFile.setSourcePath('driver/pic32mzw1/templates/system/system_definitions_objects.h.ftl')
    drvpic32mzw1SystemDefObjFile.setMarkup(True)

    drvpic32mzw1SystemConfFile = drvPic32mzw1Component.createFileSymbol('DRV_WIFI_PIC32MZW1_SYSTEM_CFG', None)
    drvpic32mzw1SystemConfFile.setType('STRING')
    drvpic32mzw1SystemConfFile.setOutputName('core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION')
    drvpic32mzw1SystemConfFile.setSourcePath('driver/pic32mzw1/templates/system/system_config.h.ftl')
    drvpic32mzw1SystemConfFile.setMarkup(True)

    drvpic32mzw1SystemInitDataFile = drvPic32mzw1Component.createFileSymbol('DRV_WIFI_PIC32MZW1_INIT_DATA', None)
    drvpic32mzw1SystemInitDataFile.setType('STRING')
    drvpic32mzw1SystemInitDataFile.setOutputName('core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA')
    drvpic32mzw1SystemInitDataFile.setSourcePath('driver/pic32mzw1/templates/system/system_initialize_data.c.ftl')
    drvpic32mzw1SystemInitDataFile.setMarkup(True)

    drvpic32mzw1SystemInitFile = drvPic32mzw1Component.createFileSymbol('DRV_WIFI_PIC32MZW1_SYS_INIT', None)
    drvpic32mzw1SystemInitFile.setType('STRING')
    drvpic32mzw1SystemInitFile.setOutputName('core.LIST_SYSTEM_INIT_C_SYS_INITIALIZE_DRIVERS')
    drvpic32mzw1SystemInitFile.setSourcePath('driver/pic32mzw1/templates/system/system_initialize.c.ftl')
    drvpic32mzw1SystemInitFile.setMarkup(True)

    drvpic32mzw1SystemTaskFile = drvPic32mzw1Component.createFileSymbol('DRV_WIFI_PIC32MZW1_SYSTEM_TASKS_C', None)
    drvpic32mzw1SystemTaskFile.setType('STRING')
    drvpic32mzw1SystemTaskFile.setOutputName('core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS')
    drvpic32mzw1SystemTaskFile.setSourcePath('driver/pic32mzw1/templates/system/system_tasks.c.ftl')
    drvpic32mzw1SystemTaskFile.setMarkup(True)

    drvpic32mzw1SystemRtosTasksFile = drvPic32mzw1Component.createFileSymbol('DRV_WIFI_PIC32MZW1_SYS_RTOS_TASK', None)
    drvpic32mzw1SystemRtosTasksFile.setType('STRING')
    drvpic32mzw1SystemRtosTasksFile.setOutputName('core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS')
    drvpic32mzw1SystemRtosTasksFile.setSourcePath('driver/pic32mzw1/templates/system/system_rtos_tasks.c.ftl')
    drvpic32mzw1SystemRtosTasksFile.setMarkup(True)
    drvpic32mzw1SystemRtosTasksFile.setEnabled((Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != 'BareMetal'))
    drvpic32mzw1SystemRtosTasksFile.setDependencies(setEnabledRTOSTask, ['HarmonyCore.SELECT_RTOS'])

def setVisibilityRTOSMenu(symbol, event):
    if (event['value'] == None):
        symbol.setVisible(False)
        print('WiFi Driver Bare Metal')
    elif (event['value'] != 'BareMetal'):
        symbol.setVisible(True)
        print('WiFi Driver RTOS')
    else:
        symbol.setVisible(False)
        print('WiFi Driver Bare Metal')

def setVisibilityRTOSTaskConfig(symbol, event):
    if (event['value'] == 'Standalone'):
        symbol.setVisible(True)
        print('WiFi Standalone')
    else:
        symbol.setVisible(False)
        print('WiFi Combined')

def setRequireBa414e(symbol, event):
    drvPic32mzw1Component = symbol.getComponent()
    if (event['value'] == True):
        symbol.setValue(True)
        drvPic32mzw1Component.setDependencyEnabled('BA414E_Dependency', True)
        print('BA414E required')
    else:
        symbol.setValue(False)
        drvPic32mzw1Component.setDependencyEnabled('BA414E_Dependency', False)
        print('BA414E not required')

def setRequireBigIntSw(symbol, event):
    drvPic32mzw1Component = symbol.getComponent()
    if (event['value'] == True):
        symbol.setValue(True)
        drvPic32mzw1Component.setDependencyEnabled('BigIntSw_Dependency', True)
        print('BigIntSw required')
    else:
        symbol.setValue(False)
        drvPic32mzw1Component.setDependencyEnabled('BigIntSw_Dependency', False)
        print('BigIntSw not required')

def setRequireWolfssl(symbol, event):
    drvPic32mzw1Component = symbol.getComponent()
    if (event['value'] == True):
        symbol.setValue(True)
        drvPic32mzw1Component.setDependencyEnabled('Wolfssl_PIC32MZW1_Dependency', True)
        print('WolfSSL required')
    else:
        symbol.setValue(False)
        drvPic32mzw1Component.setDependencyEnabled('Wolfssl_PIC32MZW1_Dependency', False)
        print('WolfSSL not required')

def setWpa3dependency(symbol, event):
    autoConnectwpa3 = [["drvWifiPic32mzw1", "BA414E_Dependency", "drv_ba414e", "drv_ba414e"]]
    if (event["value"] == True):
        res = Database.activateComponents(["drv_ba414e"], "BASIC CONFIGURATION")
        res = Database.connectDependencies(autoConnectwpa3)

def setEnabledRTOSTask(symbol, event):
    symbol.setEnabled((Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != 'BareMetal'))

def setEnableLogLevel(symbol, event):
    if event['value'] == True:
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)

def updateAlarmPeriod1ms(symbol, event):
    rate = (int(event['value']) / 256) / 1000
    symbol.setValue(rate)

def updateAlarmPeriodMax(symbol, event):
    max = 65536 / ((int(event['value']) / 256) / 1000)
    symbol.setValue(max)
