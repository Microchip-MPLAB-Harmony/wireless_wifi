###############################################################################
######################### WincDriver Configurations ###########################
###############################################################################

global sort_alphanumeric
global isDMAPresent

def handleMessage(messageID, args):
    return None

def sort_alphanumeric(l):
    import re
    convert = lambda text: int(text) if text.isdigit() else text.lower()
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ]
    return sorted(l, key = alphanum_key)

def checkPrefix(symbol):
    component = symbol.getComponent()

    wincDevice  = component.getSymbolValue('DRV_WIFI_WINC_DEVICE')
    winc1500Ver = component.getSymbolValue('DRV_WIFI_WINC1500_VERSION')
    winc3400Ver = component.getSymbolValue('DRV_WIFI_WINC3400_VERSION')

    wincVerPrefix = str(wincDevice + '_')
    if (wincDevice == 'WINC1500'):
        wincVerPrefix += winc1500Ver.replace('.', '_')
    else:
        wincVerPrefix += winc3400Ver.replace('.', '_')

    if symbol.getID().startswith('WIFI_'):
        if symbol.getID().startswith('WIFI_' + wincVerPrefix):
            return True
        else:
            return False
    else:
        return True

def importIncFile(component, devVerEnabled, configName, incFileEntry, firmwarePath = None):
    incFilePath  = incFileEntry[0]
    isEnabled    = incFileEntry[1][0]
    callback     = incFileEntry[1][1]
    dependencies = incFileEntry[1][2]
    markup       = False

    incFilePathTup = incFilePath.rsplit('/', 1)

    if len(incFilePathTup) == 1:
        secName = ''
        incSFile = incFilePathTup[0]
    else:
        secName = incFilePathTup[0]
        incSFile = incFilePathTup[1]

    if firmwarePath:
        symName  = 'WIFI_' + firmwarePath.replace('.', '_').upper() + '_' + incSFile.replace('.', '_').upper()
        secSName = firmwarePath + '/' + secName + '/'
        secDName = 'drv/' + secName + '/'

        dependencies = list(dict.fromkeys(dependencies + ['DRV_WIFI_WINC_DEVICE', 'DRV_WIFI_WINC3400_VERSION', 'DRV_WIFI_WINC1500_VERSION']))

        if not callback:
            callback = setEnablePrefix
    else:
        symName = incSFile.replace('.', '_').upper()
        secSName = secName + '/include/'
        secDName = secSName

    if incSFile.endswith('.ftl'):
        incDFile = incSFile[:-4]
        markup  = True
    else:
        incDFile = incSFile

    incFileSym = component.createFileSymbol(symName, None)
    incFileSym.setSourcePath('driver/winc/' + secSName + incSFile)
    incFileSym.setOutputName(incDFile)
    incFileSym.setDestPath('driver/winc/' + secDName)
    incFileSym.setProjectPath('config/' + configName + '/driver/winc/' + secDName)
    incFileSym.setType('HEADER')
    incFileSym.setOverwrite(True)
    incFileSym.setMarkup(markup)

    if devVerEnabled:
        incFileSym.setEnabled(isEnabled)
    else:
        incFileSym.setEnabled(False)

    if callback and dependencies:
        incFileSym.setDependencies(callback, dependencies)

def importSrcFile(component, devVerEnabled, configName, srcFileEntry, firmwarePath = None):
    srcFilePath  = srcFileEntry[0]
    isEnabled    = srcFileEntry[1][0]
    callback     = srcFileEntry[1][1]
    dependencies = srcFileEntry[1][2]
    markup       = False

    srcFilePathTup = srcFilePath.rsplit('/', 1)

    if len(srcFilePathTup) == 1:
        secName = ''
        srcSFile = srcFilePathTup[0]
    else:
        secName = srcFilePathTup[0]
        srcSFile = srcFilePathTup[1]

    if firmwarePath:
        symName  = 'WIFI_' + firmwarePath.replace('.', '_').upper() + '_' + srcSFile.replace('.', '_').upper()
        secSName = firmwarePath + '/' + secName + '/'
        secDName = 'drv/' + secName + '/'

        dependencies = list(dict.fromkeys(dependencies + ['DRV_WIFI_WINC_DEVICE', 'DRV_WIFI_WINC3400_VERSION', 'DRV_WIFI_WINC1500_VERSION']))

        if not callback:
            callback = setEnablePrefix
    else:
        srcFilePrefix   = ''
        symName = srcSFile.replace('.', '_').upper()
        secSName = secName + '/'
        secDName = secSName

    if srcSFile.endswith('.ftl'):
        srcDFile = srcSFile[:-4]
        markup  = True
    else:
        srcDFile = srcSFile

    srcFileSym = component.createFileSymbol(symName, None)
    srcFileSym.setSourcePath('driver/winc/' + secSName + srcSFile)
    srcFileSym.setOutputName(srcDFile.rsplit('/', 1)[-1])
    srcFileSym.setDestPath('driver/winc/' + secDName)
    srcFileSym.setProjectPath('config/' + configName + '/driver/winc/' + secDName)
    srcFileSym.setType('SOURCE')
    srcFileSym.setMarkup(markup)

    if devVerEnabled:
        srcFileSym.setEnabled(isEnabled)
    else:
        srcFileSym.setEnabled(False)

    if callback and dependencies:
        srcFileSym.setDependencies(callback, dependencies)

def setIncPath(component, configName, incPathEntry):
    incPath      = incPathEntry[0]
    isEnabled    = incPathEntry[1][0]
    callback     = incPathEntry[1][1]
    dependencies = incPathEntry[1][2]

    incPathSym = component.createSettingSymbol('WINC_INC_PATH' + incPath.replace('.', '_').replace('/', '_').upper(), None)
    incPathSym.setValue('../src/config/' + configName + '/driver/winc' + incPath + ';')
    incPathSym.setCategory('C32')
    incPathSym.setKey('extra-include-directories')
    incPathSym.setAppend(True, ';')
    incPathSym.setEnabled(isEnabled)
    incPathSym.setDependencies(callback, dependencies)

def onAttachmentConnected(source, target):
    global isDMAPresent

    localComponent = source['component']
    remoteComponent = target['component']
    remoteID = remoteComponent.getID()
    connectID = source['id']
    targetID = target['id']

    if connectID == 'drv_spi_dependency':
        drvSPIIndex = remoteComponent.getSymbolValue('INDEX')

        wifiDrvSPIInst = localComponent.getSymbolByID('DRV_WIFI_WINC_SPI_INST')
        wifiDrvSPIInstInx = localComponent.getSymbolByID('DRV_WIFI_WINC_SPI_INST_IDX')

        wifiDrvSPIInst.setValue('drv_spi_' + str(drvSPIIndex))
        wifiDrvSPIInst.setVisible(True)

        wifiDrvSPIInstInx.setValue(drvSPIIndex)

        localComponent.setDependencyEnabled('spi_dependency', False)
        localComponent.getSymbolByID('DRV_WIFI_WINC_DRV_SPI_MENU').setVisible(True)
    elif connectID == 'sys_debug':
        wincUseSysDebug = localComponent.getSymbolByID('DRV_WIFI_WINC_USE_SYS_DEBUG')
        wincUseSysDebug.setValue(True)
    elif connectID == 'spi_dependency':
        plibUsed = localComponent.getSymbolByID('DRV_WIFI_WINC_PLIB')
        plibUsed.clearValue()
        plibUsed.setValue(remoteID.upper())

        dmaChannelSym = Database.getSymbolValue('core', 'DMA_CH_FOR_' + remoteID.upper() + '_Transmit')
        dmaRequestSym = Database.getSymbolValue('core', 'DMA_CH_NEEDED_FOR_' + remoteID.upper() + '_Transmit')

        # Do not change the order as DMA Channels needs to be allocated
        # after setting the plibUsed symbol
        # Both device and connected plib should support DMA
        if isDMAPresent == True and dmaChannelSym != None and dmaRequestSym != None:
            localComponent.getSymbolByID('DRV_WIFI_WINC_TX_RX_DMA').setReadOnly(False)

        localComponent.setDependencyEnabled('drv_spi_dependency', False)
        localComponent.getSymbolByID('DRV_WIFI_WINC_SPI_MENU').setVisible(True)

def onAttachmentDisconnected(source, target):
    global isDMAPresent

    localComponent = source['component']
    remoteComponent = target['component']
    remoteID = remoteComponent.getID()
    connectID = source['id']
    targetID = target['id']

    if connectID == 'drv_spi_dependency':
        wifiDrvSPIInst = localComponent.getSymbolByID('DRV_WIFI_WINC_SPI_INST')
        wifiDrvSPIInstInx = localComponent.getSymbolByID('DRV_WIFI_WINC_SPI_INST_IDX')

        wifiDrvSPIInst.setValue('')
        wifiDrvSPIInst.setVisible(False)

        wifiDrvSPIInstInx.setValue(-1)

        localComponent.setDependencyEnabled('spi_dependency', True)
        localComponent.getSymbolByID('DRV_WIFI_WINC_DRV_SPI_MENU').setVisible(False)
    elif connectID == 'sys_debug':
        wincUseSysDebug = localComponent.getSymbolByID('DRV_WIFI_WINC_USE_SYS_DEBUG')
        wincUseSysDebug.setValue(False)
    elif connectID == 'spi_dependency':

        dmaChannelSym = Database.getSymbolValue('core', 'DMA_CH_FOR_' + remoteID.upper() + '_Transmit')
        dmaRequestSym = Database.getSymbolValue('core', 'DMA_CH_NEEDED_FOR_' + remoteID.upper() + '_Transmit')

        # Do not change the order as DMA Channels needs to be cleared
        # before clearing the plibUsed symbol
        # Both device and connected plib should support DMA
        if isDMAPresent == True and dmaChannelSym != None and dmaRequestSym != None:
            localComponent.getSymbolByID('DRV_WIFI_WINC_TX_RX_DMA').clearValue()
            localComponent.getSymbolByID('DRV_WIFI_WINC_TX_RX_DMA').setReadOnly(True)

        plibUsed = localComponent.getSymbolByID('DRV_WIFI_WINC_PLIB')
        plibUsed.clearValue()

        localComponent.setDependencyEnabled('drv_spi_dependency', True)
        localComponent.getSymbolByID('DRV_WIFI_WINC_SPI_MENU').setVisible(False)

def updateDMAEnableCntr(symbol, event):
    result_dict = {}

    if symbol.getValue() != event['value']:
        symbol.setValue(event['value'])
        if symbol.getValue() == True:
            result_dict = Database.sendMessage('drv_spi', 'DRV_WIFI_WINC_DMA_ENABLED', result_dict)
        else:
            result_dict = Database.sendMessage('drv_spi', 'DRV_WIFI_WINC_DMA_DISABLED', result_dict)

def sysDMAEnabled(symbol, event):
    if symbol.getValue() != event['value']:
        symbol.setValue(event['value'])
        if Database.getSymbolValue('core', 'DMA_ENABLE') != None:
            Database.sendMessage('HarmonyCore', 'ENABLE_SYS_DMA', {'isEnabled':event['value']})

def instantiateComponent(drvWincComponent):
    global isDMAPresent

    print('WINC Driver Component')

    if Database.getSymbolValue('core', 'DMA_ENABLE') == None:
        isDMAPresent = False
    else:
        isDMAPresent = True

    Database.activateComponents(['HarmonyCore'])
    Database.activateComponents(['sys_time'])
    Database.activateComponents(['eic'])

    # Enable 'Generate Harmony Driver Common Files' option in MHC
    Database.sendMessage('HarmonyCore', 'ENABLE_DRV_COMMON', {'isEnabled':True})

    # Enable "Generate Harmony System Service Common Files" option in MHC
    Database.sendMessage("HarmonyCore", "ENABLE_SYS_COMMON", {"isEnabled":True})

    configName = Variables.get('__CONFIGURATION_NAME')

    eicNode = ATDF.getNode("/avr-tools-device-file/devices/device/peripherals/module@[name=\"EIC\"]/instance@[name=\"EIC\"]/parameters/param@[name=\"EXTINT_NUM\"]")

    drvWincComponent.addDependency('drv_spi_dependency', 'DRV_SPI', False, True)
    drvWincComponent.addDependency('spi_dependency', 'SPI', False, True)
    drvWincComponent.addDependency('sys_debug', 'SYS_DEBUG', True, False)
    drvWincComponent.addDependency('wdrv_winc_HarmonyCoreDependency', 'Core Service', 'Core Service', True, True)

    # WINC Device
    wincDevice = drvWincComponent.createComboSymbol('DRV_WIFI_WINC_DEVICE', None, ['WINC1500', 'WINC3400'])
    wincDevice.setLabel('WiFi Device')
    wincDevice.setVisible(True)

    # Use SYS_DEBUG Option
    wincUseSysDebug = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_USE_SYS_DEBUG', None)
    wincUseSysDebug.setVisible(False)
    wincUseSysDebug.setDefaultValue(False)

    # Log Level
    wincLogLevel = drvWincComponent.createComboSymbol('DRV_WIFI_WINC_LOG_LEVEL', None, ['None', 'Error', 'Inform', 'Trace', 'Verbose'])
    wincLogLevel.setLabel('Driver Log Level')
    wincLogLevel.setVisible(True)
    wincLogLevel.setDependencies(setEnableLogLevel, ['DRV_WIFI_WINC_USE_SYS_DEBUG', 'sys_debug.SYS_DEBUG_USE_CONSOLE'])

    # DRV_SPI Menu
    wincDrvSpiMenu = drvWincComponent.createMenuSymbol('DRV_WIFI_WINC_DRV_SPI_MENU', None)
    wincDrvSpiMenu.setLabel('SPI')
    wincDrvSpiMenu.setDescription('SPI using DRV_SPI')
    wincDrvSpiMenu.setVisible(False)

    # SPI Instance Index
    wincSpiInst = drvWincComponent.createStringSymbol('DRV_WIFI_WINC_SPI_INST', wincDrvSpiMenu)
    wincSpiInst.setLabel('SPI Driver')
    wincSpiInst.setReadOnly(True)
    wincSpiInst.setDefaultValue('')

    wincSpiInstIdx = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_SPI_INST_IDX', wincDrvSpiMenu)
    wincSpiInstIdx.setVisible(False)
    wincSpiInstIdx.setDefaultValue(-1)

    # SPI (PLIB) Menu
    wincSpiMenu = drvWincComponent.createMenuSymbol('DRV_WIFI_WINC_SPI_MENU', None)
    wincSpiMenu.setLabel('SPI')
    wincSpiMenu.setDescription('SPI using PLIB')
    wincSpiMenu.setVisible(False)

    wincSymPLIB = drvWincComponent.createStringSymbol('DRV_WIFI_WINC_PLIB', wincSpiMenu)
    wincSymPLIB.setLabel('PLIB Used')
    wincSymPLIB.setReadOnly(True)
    wincSymPLIB.setDefaultValue('')

    global wincTXRXDMA
    wincTXRXDMA = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_TX_RX_DMA', wincSpiMenu)
    wincTXRXDMA.setLabel('Use DMA for Transmit and Receive?')
    wincTXRXDMA.setVisible(isDMAPresent)
    wincTXRXDMA.setReadOnly(True)

    wincTXRXDMAEn = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_TX_RX_DMA_EN', wincSpiMenu)
    wincTXRXDMAEn.setVisible(False)
    wincTXRXDMAEn.setDefaultValue(False)
    wincTXRXDMAEn.setDependencies(updateDMAEnableCntr, ['DRV_WIFI_WINC_TX_RX_DMA'])

    global wincTXDMAChannel
    wincTXDMAChannel = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_TX_DMA_CHANNEL', wincSpiMenu)
    wincTXDMAChannel.setLabel('DMA Channel For Transmit')
    wincTXDMAChannel.setDefaultValue(0)
    wincTXDMAChannel.setVisible(False)
    wincTXDMAChannel.setReadOnly(True)
    wincTXDMAChannel.setDependencies(requestAndAssignTxDMAChannel, ['DRV_WIFI_WINC_TX_RX_DMA'])

    global wincTXDMAChannelComment
    wincTXDMAChannelComment = drvWincComponent.createCommentSymbol('DRV_WIFI_WINC_TX_DMA_CH_COMMENT', wincSpiMenu)
    wincTXDMAChannelComment.setLabel("Warning!!! Couldn't Allocate DMA Channel for Transmit. Check DMA Manager. !!!")
    wincTXDMAChannelComment.setVisible(False)
    wincTXDMAChannelComment.setDependencies(requestDMAComment, ['DRV_WIFI_WINC_TX_DMA_CHANNEL'])

    global wincRXDMAChannel
    wincRXDMAChannel = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_RX_DMA_CHANNEL', wincSpiMenu)
    wincRXDMAChannel.setLabel('DMA Channel For Receive')
    wincRXDMAChannel.setDefaultValue(1)
    wincRXDMAChannel.setVisible(False)
    wincRXDMAChannel.setReadOnly(True)
    wincRXDMAChannel.setDependencies(requestAndAssignRxDMAChannel, ['DRV_WIFI_WINC_TX_RX_DMA'])

    global wincRXDMAChannelComment
    wincRXDMAChannelComment = drvWincComponent.createCommentSymbol('DRV_WIFI_WINC_RX_DMA_CH_COMMENT', wincSpiMenu)
    wincRXDMAChannelComment.setLabel("Warning!!! Couldn't Allocate DMA Channel for Receive. Check DMA Manager. !!!")
    wincRXDMAChannelComment.setVisible(False)
    wincRXDMAChannelComment.setDependencies(requestDMAComment, ['DRV_WIFI_WINC_RX_DMA_CHANNEL'])

    wincSymSYSDMAEnable = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_SYS_DMA_ENABLE', None)
    wincSymSYSDMAEnable.setDefaultValue(False)
    wincSymSYSDMAEnable.setVisible(False)
    wincSymSYSDMAEnable.setDependencies(sysDMAEnabled, ['DRV_WIFI_WINC_TX_RX_DMA'])

    # Interrupt Source
    wincIntSrcList = []

    if eicNode:
        wincIntSrcList.append('EIC')
    else:
        periphNode = ATDF.getNode('/avr-tools-device-file/devices/device/peripherals')
        modules = periphNode.getChildren()

        for module in range (0, len(modules)):
            periphName = str(modules[module].getAttribute('name'))
            if periphName == 'PIO':
                wincIntSrcList.append('PIO')
            elif periphName == 'GPIO':
                wincIntSrcList.append('GPIO')

    if len(wincIntSrcList):
        wincIntSrc = drvWincComponent.createComboSymbol('DRV_WIFI_WINC_INT_SRC', None, wincIntSrcList)
        wincIntSrc.setLabel('Interrupt Source')
        wincIntSrc.setVisible(True)

    # External Interrupt Selection
    if eicNode:
        extIntCount = int(eicNode.getAttribute('value'))

        eicSrcSelDepList = []

        for x in range(extIntCount):
            wincEicSrcX = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_EIC_SRC_' + str(x), wincIntSrc)
            wincEicSrcX.setLabel('EIC Channel ' + str(x))

            if Database.getSymbolValue('eic', 'EIC_INT_' + str(x)):
                wincEicSrcX.setVisible(True)
            else:
                wincEicSrcX.setVisible(False)
                wincEicSrcX.setValue(False)

            wincEicSrcX.setDependencies(setVisibilityEicSource, ['DRV_WIFI_WINC_INT_SRC', 'eic.EIC_INT'])

            eicSrcSelDepList.append('DRV_WIFI_WINC_EIC_SRC_' + str(x))

        wincEicSrcSel = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_EIC_SRC_SELECT', None)
        wincEicSrcSel.setVisible(False)
        wincEicSrcSel.setDependencies(setValueEicSource, eicSrcSelDepList)
        wincEicSrcSel.setDefaultValue(-1)
        wincEicSrcSel.setMin(-1)
        wincEicSrcSel.setMax(extIntCount-1)

        wincSymPinConfigComment = drvWincComponent.createCommentSymbol('DRV_WIFI_WINC_EIC_CONFIG_COMMENT', None)
        wincSymPinConfigComment.setLabel('***EIC channel must be configured in EIC component for interrupt source***')

    elif 'GPIO' in wincIntSrcList:
        # Devices using GPIO Interrupts such as PIC32MZ
        availablePinDictionary = {}
        # Send message to core to get available pins
        availablePinDictionary = Database.sendMessage('core', 'PIN_LIST', availablePinDictionary)

        wincGpioIntSrc = drvWincComponent.createKeyValueSetSymbol('DRV_WIFI_WINC_GPIO_SRC_SELECT', None)
        wincGpioIntSrc.setLabel('Interrupt Pin')
        wincGpioIntSrc.setOutputMode('Key')
        wincGpioIntSrc.setDisplayMode('Description')

        for pad in sort_alphanumeric(availablePinDictionary.values()):
            key = 'GPIO_PIN_' + pad
            value = list(availablePinDictionary.keys())[list(availablePinDictionary.values()).index(pad)]
            description = pad
            wincGpioIntSrc.addKey(key, value, description)

        wincGpioIntSrc.addKey('GPIO_PIN_NONE', '-1', 'None')
        wincGpioIntSrc = drvWincComponent.createCommentSymbol('DRV_WINC_PINS_CONFIG_COMMENT', None)
        wincGpioIntSrc.setLabel('***Above selected pins must be configured as GPIO Output in Pin Manager***')

    elif 'PIO' in wincIntSrcList:
        #PIO is used in Cortex-M7 and MPU devices
        availablePinDictionary = {}
        # Send message to core to get available pins
        availablePinDictionary = Database.sendMessage('core', 'PIN_LIST', availablePinDictionary)

        wincPioIntSrc = drvWincComponent.createKeyValueSetSymbol('DRV_WIFI_WINC_PIO_SRC_SELECT', None)
        wincPioIntSrc.setLabel('Interrupt Pin')
        wincPioIntSrc.setOutputMode('Key')
        wincPioIntSrc.setDisplayMode('Description')

        for pad in sort_alphanumeric(availablePinDictionary.values()):
            key = 'PIO_PIN_' + pad
            value = list(availablePinDictionary.keys())[list(availablePinDictionary.values()).index(pad)]
            description = pad
            wincPioIntSrc.addKey(key, value, description)

        wincPioIntSrc.addKey('PIO_PIN_NONE', '-1', 'None')
        wincPioIntSrc = drvWincComponent.createCommentSymbol('DRV_WINC_PINS_CONFIG_COMMENT', None)
        wincPioIntSrc.setLabel('***Above selected pins must be configured as PIO Output in Pin Manager***')

    # WINC1500 Version
    winc1500Version = drvWincComponent.createComboSymbol('DRV_WIFI_WINC1500_VERSION', None, ['19.7.7'])
    winc1500Version.setLabel('Firmware Version')
    winc1500Version.setVisible(True)
    winc1500Version.setDefaultValue('19.7.7')
    winc1500Version.setDependencies(setVisibilityWincVersion, ['DRV_WIFI_WINC_DEVICE'])

    # WINC3400 Version
    winc3400Version = drvWincComponent.createComboSymbol('DRV_WIFI_WINC3400_VERSION', None, ['1.4.4'])
    winc3400Version.setLabel('Firmware Version')
    winc3400Version.setVisible(False)
    winc3400Version.setDefaultValue('1.4.4')
    winc3400Version.setDependencies(setVisibilityWincVersion, ['DRV_WIFI_WINC_DEVICE'])

    # WINC3400 BLE API Support
    winc3400UseBle = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_USE_BLUETOOTH_WINC3400', None)
    winc3400UseBle.setLabel('Use BLE?')
    winc3400UseBle.setDefaultValue(False)
    winc3400UseBle.setVisible(wincDevice.getValue() == 'WINC3400')
    winc3400UseBle.setDependencies(setVisibilityWincVersion, ['DRV_WIFI_WINC_DEVICE'])

    # WINC Driver Mode
    wincDriverMode = drvWincComponent.createComboSymbol('DRV_WIFI_WINC_DRIVER_MODE', None, ['Ethernet Mode', 'Socket Mode'])
    wincDriverMode.setLabel('Driver Mode')
    wincDriverMode.setDefaultValue('Socket Mode')
    wincDriverMode.setVisible(wincDevice.getValue() == 'WINC3400' or wincDevice.getValue() == 'WINC1500')
    wincDriverMode.setDependencies(setVisibilityWincBypass, ['DRV_WIFI_WINC_DEVICE'])
    wincDriverMode.setDependencies(setVisibilityWincFWVersion, ['DRV_WIFI_WINC3400_VERSION'])

    # WINC Use TCP/IP Stack
    wincUseTcpipStack = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_USE_TCPIP_STACK', wincDriverMode)
    wincUseTcpipStack.setLabel('Use Harmony TCP/IP Stack?')
    wincUseTcpipStack.setVisible(False)
    wincUseTcpipStack.setDefaultValue(False)
    wincUseTcpipStack.setDependencies(setVisibilityUseTcpipStack, ['DRV_WIFI_WINC_DRIVER_MODE'])

    # At startup, Hide Mac Capability
    drvWincComponent.setCapabilityEnabled('libdrvWincMac', False)

    # RTOS Configuration
    wincRtosMenu = drvWincComponent.createMenuSymbol('DRV_WIFI_WINC_RTOS_MENU', None)
    wincRtosMenu.setLabel('RTOS Configuration')
    wincRtosMenu.setDescription('RTOS Configuration')
    wincRtosMenu.setVisible(False)
    wincRtosMenu.setVisible((Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != 'BareMetal') and (Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != None))
    wincRtosMenu.setDependencies(setVisibilityRTOSMenu, ['HarmonyCore.SELECT_RTOS'])

    # RTOS Execution Mode
    wincInstnExecMode = drvWincComponent.createComboSymbol('DRV_WIFI_WINC_RTOS', wincRtosMenu, ['Standalone'])
    wincInstnExecMode.setLabel('Run Driver Tasks as')
    wincInstnExecMode.setVisible(False)
    wincInstnExecMode.setDescription('WiFi WINC Driver RTOS Execution Mode')
    wincInstnExecMode.setDefaultValue('Standalone')

    # WiFi Driver Task Stack Size
    wincTaskSize = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_RTOS_STACK_SIZE', wincRtosMenu)
    wincTaskSize.setLabel('Task Size (in bytes)')
    wincTaskSize.setVisible(True)
    wincTaskSize.setDescription('WiFi WINC Driver RTOS Task Stack Size')
    wincTaskSize.setDefaultValue(4096)
    wincTaskSize.setDependencies(setVisibilityRTOSTaskConfig, ['DRV_WIFI_WINC_RTOS'])

    # WiFi Driver Task Priority
    wincTaskPriority = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_RTOS_TASK_PRIORITY', wincRtosMenu)
    wincTaskPriority.setLabel('Task Priority')
    wincTaskPriority.setVisible(True)
    wincTaskPriority.setDescription('WiFi WINC Driver RTOS Task Priority')
    wincTaskPriority.setDefaultValue(1)
    wincTaskPriority.setDependencies(setVisibilityRTOSTaskConfig, ['DRV_WIFI_WINC_RTOS'])

    # WiFi Use Task Delay?
    wincUseTaskDelay = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_RTOS_USE_DELAY', wincRtosMenu)
    wincUseTaskDelay.setLabel('Use Task Delay?')
    wincUseTaskDelay.setVisible(True)
    wincUseTaskDelay.setDescription('WiFi WINC Driver Task Uses Delay?')
    wincUseTaskDelay.setDefaultValue(True)
    wincUseTaskDelay.setDependencies(setVisibilityRTOSTaskConfig, ['DRV_WIFI_WINC_RTOS'])

    # Set the WINC device reset assert time
    wincDrvResetAssertTime = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_RESET_ASSERT_TIME', None)
    wincDrvResetAssertTime.setLabel('Reset assert time (in ms)')
    wincDrvResetAssertTime.setVisible(True)
    wincDrvResetAssertTime.setDescription('How long the reset signal to the WINC device is held, 0 means driver default (50ms)')
    wincDrvResetAssertTime.setDefaultValue(0)

    # Enable serial bridge support within Lite driver
    wincEnableSerialBridge = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_ENABLE_SB', None)
    wincEnableSerialBridge.setLabel('Enable serial bridge?')
    wincEnableSerialBridge.setDefaultValue(False)

    # Enable serial bridge support for switching serial baud rates
    wincEnableSerBriRateChange = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_ENABLE_SB_RATE_CHANGE', wincEnableSerialBridge)
    wincEnableSerBriRateChange.setLabel('Support baud rate changes?')
    wincEnableSerBriRateChange.setVisible(False)
    wincEnableSerBriRateChange.setDefaultValue(False)
    wincEnableSerBriRateChange.setDependencies(setVisibilitySBEnabled, ['DRV_WIFI_WINC_ENABLE_SB'])

    # Enable serial bridge for sending unsolicited sync ID
    wincEnableSerBriUnsolSyncID = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_ENABLE_SB_UNSOL_SYNC_ID', wincEnableSerialBridge)
    wincEnableSerBriUnsolSyncID.setLabel('Send sync ID before request?')
    wincEnableSerBriUnsolSyncID.setVisible(False)
    wincEnableSerBriUnsolSyncID.setDefaultValue(False)
    wincEnableSerBriUnsolSyncID.setDependencies(setVisibilitySBEnabled, ['DRV_WIFI_WINC_ENABLE_SB'])

    ############################################################################
    #### Code Generation ####
    ############################################################################

    flagEthernetMode        = (wincDriverMode.getValue() == 'Ethernet Mode')
    flagSocketMode          = (wincDriverMode.getValue() == 'Socket Mode')
    flagTcpipStackPresent   = (flagEthernetMode and (wincUseTcpipStack.getValue() == 'True'))
    flagBlePresent          = (winc3400UseBle.getValue() == 'True')
    flagWinc1500            = (wincDevice.getValue() == 'WINC1500')
    flagWinc3400            = (wincDevice.getValue() == 'WINC3400')

    flagHostFileSupport     = (flagSocketMode and flagWinc1500)

    condAlways              = [True,                    None,                           []]
    condSocketMode          = [flagSocketMode,          setEnableSocketMode,            ['DRV_WIFI_WINC_DRIVER_MODE']]
    condEthernetMode        = [flagEthernetMode,        setEnableEthernetMode,          ['DRV_WIFI_WINC_DRIVER_MODE']]
    condHostFileSupport     = [flagHostFileSupport,     setEnableHostFileSupport,       ['DRV_WIFI_WINC_DEVICE', 'DRV_WIFI_WINC_DRIVER_MODE']]
    condTcpipStackPresent   = [flagTcpipStackPresent,   setEnableTcpipStackPresent,     ['DRV_WIFI_WINC_USE_TCPIP_STACK', 'DRV_WIFI_WINC_DRIVER_MODE']]
    condBle                 = [flagBlePresent,          setEnableBlePresent,            ['DRV_WIFI_WINC_DEVICE', 'DRV_WIFI_WINC_USE_BLUETOOTH_WINC3400']]
    condWinc1500            = [flagWinc1500,            setEnableWinc1500,              ['DRV_WIFI_WINC_DEVICE', 'DRV_WIFI_WINC3400_VERSION']]
    condWinc3400            = [flagWinc3400,            setEnableWinc3400,              ['DRV_WIFI_WINC_DEVICE', 'DRV_WIFI_WINC3400_VERSION']]

    wdrvIncFiles = [
        ['wdrv_winc.h',                                         condAlways],
        ['wdrv_winc_api.h',                                     condAlways],
        ['wdrv_winc_assoc.h',                                   condAlways],
        ['wdrv_winc_authctx.h',                                 condAlways],
        ['wdrv_winc_ble.h',                                     condBle],
        ['wdrv_winc_bssctx.h',                                  condAlways],
        ['wdrv_winc_bssfind.h',                                 condAlways],
        ['wdrv_winc_client_api.h',                              condAlways],
        ['wdrv_winc_common.h',                                  condAlways],
        ['wdrv_winc_custie.h',                                  condAlways],
        ['wdrv_winc_debug.h',                                   condAlways],
        ['wdrv_winc_enterprise.h',                              condAlways],
        ['wdrv_winc_ethernet.h',                                condEthernetMode],
        ['wdrv_winc_host_file.h',                               condHostFileSupport],
        ['wdrv_winc_httpprovctx.h',                             condSocketMode],
        ['wdrv_winc_mac.h',                                     condTcpipStackPresent],
        ['wdrv_winc_nvm.h',                                     condAlways],
        ['wdrv_winc_powersave.h',                               condAlways],
        ['wdrv_winc_socket.h',                                  condSocketMode],
        ['wdrv_winc_softap.h',                                  condAlways],
        ['wdrv_winc_ssl.h',                                     condSocketMode],
        ['wdrv_winc_sta.h',                                     condAlways],
        ['wdrv_winc_systime.h',                                 condAlways],
        ['wdrv_winc_wps.h',                                     condAlways],
        ['dev/wdrv_winc_gpio.h',                                condAlways]
    ]

    wdrvFirmwareDriverIncFiles = [
        ['host_drv/conf_winc.h.ftl',                            condAlways],
        ['host_drv/bsp/winc_bsp.h',                             condAlways],
        ['host_drv/common/winc_ecc_types.h',                    condAlways],
        ['host_drv/common/winc_1500_3A0_registers.h',           condWinc1500],
        ['host_drv/common/winc_3400_registers.h',               condWinc3400],
        ['host_drv/common/winc_debug.h',                        condAlways],
        ['host_drv/common/winc_defines.h',                      condAlways],
        ['host_drv/common/winc_registers.h',                    condAlways],
        ['host_drv/driver/winc_asic.h',                         condAlways],
        ['host_drv/driver/winc_drv.h',                          condAlways],
        ['host_drv/driver/winc_hif.h',                          condAlways],
        ['host_drv/driver/winc_spi.h',                          condAlways],
        ['host_drv/driver/winc_ver.h',                          condWinc3400],
        ['host_drv/include/conf_winc_defaults.h',               condAlways],
        ['host_drv/include/winc.h',                             condAlways],
        ['host_drv/m2m/winc_m2m_crypto.h',                      condAlways],
        ['host_drv/m2m/winc_m2m_fwinfo.h',                      condAlways],
        ['host_drv/m2m/winc_m2m_ota.h',                         condAlways],
        ['host_drv/m2m/winc_m2m_periph.h',                      condAlways],
        ['host_drv/m2m/winc_m2m_socket_host_if.h',              condSocketMode],
        ['host_drv/m2m/winc_m2m_ssl.h',                         condAlways],
        ['host_drv/m2m/winc_m2m_types.h',                       condAlways],
        ['host_drv/m2m/winc_m2m_wifi.h',                        condAlways],
        ['host_drv/serial_bridge/winc_serial_bridge.h',         condAlways],
        ['host_drv/socket/winc_socket.h',                       condSocketMode],
        ['host_drv/spi_flash/winc_spi_flash.h',                 condAlways],
        ['host_drv/spi_flash/winc_spi_flash_map.h',             condAlways],
        ['host_drv/spi_flash/winc_flexible_flash.h',            condWinc1500],
    ]

    bledrvFirmwareDriverIncFiles = [
        ['ble_stack/include/winc_ble_api.h',                    condBle],
        ['ble_stack/include/winc_ble_cmn_defs.h',               condBle],
        ['ble_stack/include/winc_ble_dbg_task.h',               condBle],
        ['ble_stack/include/winc_ble_device.h',                 condBle],
        ['ble_stack/include/winc_ble_error.h',                  condBle],
        ['ble_stack/include/winc_ble_event.h',                  condBle],
        ['ble_stack/include/winc_ble_gapc_task.h',              condBle],
        ['ble_stack/include/winc_ble_gapm_task.h',              condBle],
        ['ble_stack/include/winc_ble_gattc_task.h',             condBle],
        ['ble_stack/include/winc_ble_gattm_task.h',             condBle],
        ['ble_stack/include/winc_ble_interface.h',              condBle],
        ['ble_stack/include/winc_ble_ll_if.h',                  condBle],
        ['ble_stack/include/winc_ble_platform.h',               condBle],
        ['ble_stack/include/winc_ble_profiles.h',               condBle],
        ['ble_stack/include/winc_ble_wifiprov_task.h',          condBle],
    ]

    for incFileEntry in wdrvIncFiles:
        importIncFile(drvWincComponent, True, configName, incFileEntry)

    for incFileEntry in wdrvFirmwareDriverIncFiles:
        importIncFile(drvWincComponent, flagWinc1500, configName, incFileEntry, 'winc1500_19.7.7_lite')

    for incFileEntry in wdrvFirmwareDriverIncFiles:
        importIncFile(drvWincComponent, flagWinc3400, configName, incFileEntry, 'winc3400_1.4.4_lite')

    for incFileEntry in bledrvFirmwareDriverIncFiles:
        importIncFile(drvWincComponent, flagWinc3400, configName, incFileEntry, 'winc3400_1.4.4_lite')

    wdrvSrcFiles = [
        ['wdrv_winc.c',                                         condAlways],
        ['wdrv_winc_assoc.c',                                   condAlways],
        ['wdrv_winc_authctx.c',                                 condAlways],
        ['wdrv_winc_ble.c',                                     condBle],
        ['wdrv_winc_bssctx.c',                                  condAlways],
        ['wdrv_winc_bssfind.c',                                 condAlways],
        ['wdrv_winc_custie.c',                                  condAlways],
        ['wdrv_winc_enterprise.c',                              condAlways],
        ['wdrv_winc_ethernet.c',                                condEthernetMode],
        ['wdrv_winc_host_file.c',                               condHostFileSupport],
        ['wdrv_winc_httpprovctx.c',                             condSocketMode],
        ['wdrv_winc_nvm.c',                                     condAlways],
        ['wdrv_winc_powersave.c',                               condAlways],
        ['wdrv_winc_socket.c',                                  condSocketMode],
        ['wdrv_winc_softap.c',                                  condAlways],
        ['wdrv_winc_ssl.c',                                     condSocketMode],
        ['wdrv_winc_sta.c',                                     condAlways],
        ['wdrv_winc_systime.c',                                 condAlways],
        ['wdrv_winc_wps.c',                                     condAlways],
        ['dev/gpio/wdrv_winc_eint.c',                           condAlways],
        ['dev/gpio/wdrv_winc_gpio.c',                           condAlways],
    ]

    wdrvFirmwareDriverSrcFiles = [
        ['host_drv/bsp/winc_bsp.c',                             condAlways],
        ['host_drv/driver/winc_asic.c',                         condAlways],
        ['host_drv/driver/winc_drv.c',                          condAlways],
        ['host_drv/driver/winc_hif.c',                          condAlways],
        ['host_drv/driver/winc_spi.c',                          condAlways],
        ['host_drv/driver/winc_ver.c',                          condWinc3400],
        ['host_drv/m2m/winc_m2m_crypto.c',                      condAlways],
        ['host_drv/m2m/winc_m2m_fwinfo.c',                      condAlways],
        ['host_drv/m2m/winc_m2m_ota.c',                         condAlways],
        ['host_drv/m2m/winc_m2m_periph.c',                      condAlways],
        ['host_drv/m2m/winc_m2m_ssl.c',                         condSocketMode],
        ['host_drv/m2m/winc_m2m_wifi.c',                        condAlways],
        ['host_drv/serial_bridge/winc_serial_bridge.c',         condAlways],
        ['host_drv/socket/winc_socket.c',                       condSocketMode],
        ['host_drv/spi_flash/winc_spi_flash.c',                 condAlways],
        ['host_drv/spi_flash/winc_flexible_flash.c',            condWinc1500],
    ]

    bledrvFirmwareDriverSrcFiles = [
        ['ble_stack/winc_ble_event.c',                          condBle],
        ['ble_stack/winc_ble_gap.c',                            condBle],
        ['ble_stack/winc_ble_gatt_client.c',                    condBle],
        ['ble_stack/winc_ble_gatt_server.c',                    condBle],
        ['ble_stack/winc_ble_platform.c',                       condBle],
        ['ble_stack/winc_ble_security.c',                       condBle],
        ['ble_stack/external_host/winc_ble_dbg_task.c',         condBle],
        ['ble_stack/external_host/winc_ble_error.c',            condBle],
        ['ble_stack/external_host/winc_ble_gapc_task.c',        condBle],
        ['ble_stack/external_host/winc_ble_gapm_task.c',        condBle],
        ['ble_stack/external_host/winc_ble_gattc_task.c',       condBle],
        ['ble_stack/external_host/winc_ble_gattm_task.c',       condBle],
        ['ble_stack/external_host/winc_ble_interface.c',        condBle],
        ['ble_stack/external_host/winc_ble_wifiprov_task.c',    condBle],
    ]

    for srcFileEntry in wdrvSrcFiles:
        importSrcFile(drvWincComponent, True, configName, srcFileEntry)

    for srcFileEntry in wdrvFirmwareDriverSrcFiles:
        importSrcFile(drvWincComponent, flagWinc1500, configName, srcFileEntry, 'winc1500_19.7.7_lite')

    for srcFileEntry in wdrvFirmwareDriverSrcFiles:
        importSrcFile(drvWincComponent, flagWinc3400, configName, srcFileEntry, 'winc3400_1.4.4_lite')

    for srcFileEntry in bledrvFirmwareDriverSrcFiles:
        importSrcFile(drvWincComponent, flagWinc3400, configName, srcFileEntry, 'winc3400_1.4.4_lite')

    wdrvIncPaths = [
        ['/include/',               condAlways],
        ['/dev/include',            condAlways]
    ]

    wdrvFirmwareDriverIncPaths = [
        ['/drv/host_drv',           condAlways],
        ['/drv/ble_stack/include',  condBle],
    ]

    for incPathEntry in wdrvIncPaths:
        setIncPath(drvWincComponent, configName, incPathEntry)

    for incPathEntry in wdrvFirmwareDriverIncPaths:
        setIncPath(drvWincComponent, configName, incPathEntry)

    drvwincSystemDefFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_DEF', None)
    drvwincSystemDefFile.setType('STRING')
    drvwincSystemDefFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES')
    drvwincSystemDefFile.setSourcePath('driver/winc/templates/system/system_definitions.h.ftl')
    drvwincSystemDefFile.setMarkup(True)

    drvwincSystemDefObjFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYSTEM_DEF_OBJECT', None)
    drvwincSystemDefObjFile.setType('STRING')
    drvwincSystemDefObjFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS')
    drvwincSystemDefObjFile.setSourcePath('driver/winc/templates/system/system_definitions_objects.h.ftl')
    drvwincSystemDefObjFile.setMarkup(True)

    drvwincSystemConfFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYSTEM_CFG', None)
    drvwincSystemConfFile.setType('STRING')
    drvwincSystemConfFile.setOutputName('core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION')
    drvwincSystemConfFile.setSourcePath('driver/winc/templates/system/system_config.h.ftl')
    drvwincSystemConfFile.setMarkup(True)

    drvwincSystemInitDataFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_INIT_DATA', None)
    drvwincSystemInitDataFile.setType('STRING')
    drvwincSystemInitDataFile.setOutputName('core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA')
    drvwincSystemInitDataFile.setSourcePath('driver/winc/templates/system/system_initialize_data.c.ftl')
    drvwincSystemInitDataFile.setMarkup(True)

    drvwincSystemInitFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYS_INIT', None)
    drvwincSystemInitFile.setType('STRING')
    drvwincSystemInitFile.setOutputName('core.LIST_SYSTEM_INIT_C_SYS_INITIALIZE_DRIVERS')
    drvwincSystemInitFile.setSourcePath('driver/winc/templates/system/system_initialize.c.ftl')
    drvwincSystemInitFile.setMarkup(True)

    drvwincSystemTaskFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYSTEM_TASKS_C', None)
    drvwincSystemTaskFile.setType('STRING')
    drvwincSystemTaskFile.setOutputName('core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS')
    drvwincSystemTaskFile.setSourcePath('driver/winc/templates/system/system_tasks.c.ftl')
    drvwincSystemTaskFile.setMarkup(True)

    drvwincOsalSrcFile = drvWincComponent.createFileSymbol('WDRV_WINC_OSAL_SRC', None)
    drvwincOsalSrcFile.setSourcePath('driver/winc/templates/osal/wdrv_winc_osal.c.ftl')
    drvwincOsalSrcFile.setOutputName('wdrv_winc_osal.c')
    drvwincOsalSrcFile.setDestPath('/driver/winc/osal')
    drvwincOsalSrcFile.setProjectPath('config/' + configName + '/driver/winc/osal')
    drvwincOsalSrcFile.setType('SOURCE')
    drvwincOsalSrcFile.setMarkup(True)
    drvwincOsalSrcFile.setOverwrite(True)

    drvwincDevSpiSrcFile = drvWincComponent.createFileSymbol('WDRV_WINC_DEV_SPI_SRC', None)
    drvwincDevSpiSrcFile.setSourcePath('driver/winc/templates/dev/spi/wdrv_winc_spi.c.ftl')
    drvwincDevSpiSrcFile.setOutputName('wdrv_winc_spi.c')
    drvwincDevSpiSrcFile.setDestPath('/driver/winc/dev/spi')
    drvwincDevSpiSrcFile.setProjectPath('config/' + configName + '/driver/winc/dev/spi')
    drvwincDevSpiSrcFile.setType('SOURCE')
    drvwincDevSpiSrcFile.setMarkup(True)
    drvwincDevSpiSrcFile.setOverwrite(True)

    drvwincDevSpiHdrFile = drvWincComponent.createFileSymbol('WDRV_WINC_DEV_SPI_HDR', None)
    drvwincDevSpiHdrFile.setSourcePath('driver/winc/templates/dev/spi/wdrv_winc_spi.h.ftl')
    drvwincDevSpiHdrFile.setOutputName('wdrv_winc_spi.h')
    drvwincDevSpiHdrFile.setDestPath('/driver/winc/dev/include')
    drvwincDevSpiHdrFile.setProjectPath('config/' + configName + '/driver/winc/dev/include')
    drvwincDevSpiHdrFile.setType('HEADER')
    drvwincDevSpiHdrFile.setMarkup(True)
    drvwincDevSpiHdrFile.setOverwrite(True)

    drvwincSystemRtosTasksFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYS_RTOS_TASK', None)
    drvwincSystemRtosTasksFile.setType('STRING')
    drvwincSystemRtosTasksFile.setOutputName('core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS')
    drvwincSystemRtosTasksFile.setSourcePath('driver/winc/templates/system/system_rtos_tasks.c.ftl')
    drvwincSystemRtosTasksFile.setMarkup(True)
    drvwincSystemRtosTasksFile.setEnabled((Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != 'BareMetal'))
    drvwincSystemRtosTasksFile.setDependencies(setEnabledRTOSTask, ['HarmonyCore.SELECT_RTOS'])

    drvwincLiteFlag = drvWincComponent.createBooleanSymbol('DRV_WIFI_WINC_LITE_DRIVER', None)
    drvwincLiteFlag.setDefaultValue(True)
    drvwincLiteFlag.setVisible(False)

def destroyComponent(drvWincComponentCommon):
    Database.sendMessage('HarmonyCore', 'ENABLE_DRV_COMMON', {'isEnabled':False})
    Database.sendMessage('HarmonyCore', 'ENABLE_SYS_COMMON', {'isEnabled':False})

    if Database.getSymbolValue('core', 'DMA_ENABLE') != None:
        Database.sendMessage('HarmonyCore', 'ENABLE_SYS_DMA', {'isEnabled':False})

def setVisibilityWincVersion(symbol, event):
    if event['value'] == 'WINC1500':
        if symbol.getID() == 'DRV_WIFI_WINC1500_VERSION':
            symbol.setVisible(True)
        else:
            symbol.setVisible(False)
    else:
        if (symbol.getID() == 'DRV_WIFI_WINC3400_VERSION') or (symbol.getID() == 'DRV_WIFI_WINC_USE_BLUETOOTH_WINC3400'):
            symbol.setVisible(True)
        else:
            symbol.setVisible(False)

def setVisibilityWincBypass(symbol, event):
    if event['value'] == 'WINC1500' or event['value'] == 'WINC3400':
        symbol.setVisible(True)

def setVisibilityWincFWVersion(symbol, event):
    component = symbol.getComponent()
    deviceWinc = component.getSymbolValue('DRV_WIFI_WINC_DEVICE')

    if deviceWinc == 'WINC3400':
        if event['value'] == '1.2.2':
            symbol.setVisible(False)
        else:
            symbol.setVisible(True)
    else:
        symbol.setVisible(True)

def setVisibilityRTOSMenu(symbol, event):
    if (event['value'] == None):
        symbol.setVisible(False)
    elif (event['value'] != 'BareMetal'):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def setVisibilityRTOSTaskConfig(symbol, event):
    if (event['value'] == 'Standalone'):
        symbol.setVisible(True)
        print('WiFi Standalone')
    else:
        symbol.setVisible(False)
        print('WiFi Combined')

def setEnabledRTOSTask(symbol, event):
    symbol.setEnabled((Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != 'BareMetal'))

def setVisibilityUseTcpipStack(symbol, event):
    if(event['value'] == 'Ethernet Mode'):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def setVisibilityEicSource(symbol, event):
    eicUseSym = symbol.getComponent().getSymbolByID('DRV_WIFI_WINC_INT_SRC')

    if eicUseSym.getValue() == False:
        symbol.setVisible(False)
        symbol.setValue(False)
    else:
        eicID = symbol.getID().split('_')[-1]

        if Database.getSymbolValue('eic', 'EIC_INT_' + eicID):
            symbol.setVisible(True)
        else:
            symbol.setVisible(False)
            symbol.setValue(False)

def setValueEicSource(symbol, event):
    eicCurSel = symbol.getComponent().getSymbolByID('DRV_WIFI_WINC_EIC_SRC_SELECT')

    eicID = int(event['id'].split('_')[-1])

    if symbol.getComponent().getSymbolByID(event['id']).getValue():
        eicCurSel.setValue(eicID)
        for x in range(eicCurSel.getMax()+1):
            if x != eicID:
                symbol.getComponent().getSymbolByID('DRV_WIFI_WINC_EIC_SRC_' + str(x)).setVisible(False)
    elif eicCurSel.getValue() == eicID:
        for x in range(eicCurSel.getMax()+1):
            if Database.getSymbolValue('eic', 'EIC_INT_' + str(x)):
                symbol.getComponent().getSymbolByID('DRV_WIFI_WINC_EIC_SRC_' + str(x)).setVisible(True)

        eicCurSel.setValue(-1)

def setEnableSocketMode(symbol, event):
    component = symbol.getComponent()

    setSocketMode  =  component.getSymbolValue('DRV_WIFI_WINC_DRIVER_MODE')
    if ((setSocketMode == 'Socket Mode') and (checkPrefix(symbol))):
        symbol.setEnabled(True)
    else:
        symbol.setEnabled(False)

def setEnableEthernetMode(symbol, event):
    if ((event['value'] == 'Ethernet Mode') and (checkPrefix(symbol))):
        symbol.setEnabled(True)
    else:
        wincMacActive = symbol.getComponent().getCapabilityEnabled('libdrvWincMac')
        if (wincMacActive == True):
            symbol.getComponent().setCapabilityEnabled('libdrvWincMac', False)
        symbol.setEnabled(False)

def setEnableBlePresent(symbol, event):
    component = symbol.getComponent()

    wincDeviceMode   = component.getSymbolValue('DRV_WIFI_WINC_DEVICE')
    useBleDriverMode = component.getSymbolValue('DRV_WIFI_WINC_USE_BLUETOOTH_WINC3400')

    if ((wincDeviceMode == 'WINC3400') and (useBleDriverMode == True) and (checkPrefix(symbol))):
        symbol.setEnabled(True)
    else:
        symbol.setEnabled(False)

def setEnableTcpipStackPresent(symbol, event):
    component = symbol.getComponent()

    wincDrvMode   = component.getSymbolValue('DRV_WIFI_WINC_DRIVER_MODE')
    useTcpipStack = component.getSymbolValue('DRV_WIFI_WINC_USE_TCPIP_STACK')
    wincMacActive = component.getCapabilityEnabled('libdrvWincMac')

    if ((wincDrvMode == 'Ethernet Mode') and (useTcpipStack == True) and (checkPrefix(symbol))):
        symbol.setEnabled(True)
        if (wincMacActive == False):
            component.setCapabilityEnabled('libdrvWincMac', True)
    else:
        symbol.setEnabled(False)
        if (wincMacActive == True):
            component.setCapabilityEnabled('libdrvWincMac', False)

def setEnableWinc1500(symbol, event):
    component = symbol.getComponent()

    wincDevice  = component.getSymbolValue('DRV_WIFI_WINC_DEVICE')

    if ((wincDevice == 'WINC1500') and (checkPrefix(symbol))):
        symbol.setEnabled(True)
    else:
        symbol.setEnabled(False)

def setEnableWinc3400(symbol, event):
    component = symbol.getComponent()

    wincDevice  = component.getSymbolValue('DRV_WIFI_WINC_DEVICE')

    if ((wincDevice == 'WINC3400') and (checkPrefix(symbol))):
        symbol.setEnabled(True)
    else:
        symbol.setEnabled(False)

def setEnableHostFileSupport(symbol, event):
    component = symbol.getComponent()

    wincDevice  = component.getSymbolValue('DRV_WIFI_WINC_DEVICE')
    wincDrvMode = component.getSymbolValue('DRV_WIFI_WINC_DRIVER_MODE')

    if ((wincDevice == 'WINC1500') and (wincDrvMode == 'Socket Mode') and (checkPrefix(symbol))):
        symbol.setEnabled(True)
    else:
        symbol.setEnabled(False)

def setEnablePrefix(symbol, event):
    symbol.setEnabled(checkPrefix(symbol))

def setVisibilitySBEnabled(symbol, event):
    if(event['value'] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def setEnableLogLevel(symbol, event):
    if event['value'] == True:
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)

def requestAndAssignTxDMAChannel(symbol, event):
    global wincTXDMAChannelComment
    component = symbol.getComponent()

    spiPeripheral = component.getSymbolValue('DRV_WIFI_WINC_PLIB')

    dmaChannelID = 'DMA_CH_FOR_' + str(spiPeripheral) + '_Transmit'
    dmaRequestID = 'DMA_CH_NEEDED_FOR_' + str(spiPeripheral) + '_Transmit'

    # Clear the DMA symbol. Done for backward compatibility.
    Database.clearSymbolValue('core', dmaRequestID)

    dummyDict = {}

    if event['value'] == False:
        dummyDict = Database.sendMessage('core', 'DMA_CHANNEL_DISABLE', {'dma_channel':dmaRequestID})
        wincTXDMAChannelComment.setVisible(False)
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)
        dummyDict = Database.sendMessage('core', 'DMA_CHANNEL_ENABLE', {'dma_channel':dmaRequestID})

    # Get the allocated channel and assign it
    channel = Database.getSymbolValue('core', dmaChannelID)

    if channel is not None:
        symbol.setValue(channel)

def requestAndAssignRxDMAChannel(symbol, event):
    global wincRXDMAChannelComment
    component = symbol.getComponent()

    spiPeripheral = component.getSymbolValue('DRV_WIFI_WINC_PLIB')

    dmaChannelID = 'DMA_CH_FOR_' + str(spiPeripheral) + '_Receive'
    dmaRequestID = 'DMA_CH_NEEDED_FOR_' + str(spiPeripheral) + '_Receive'

    # Clear the DMA symbol. Done for backward compatibility.
    Database.clearSymbolValue('core', dmaRequestID)

    dummyDict = {}

    if event['value'] == False:
        dummyDict = Database.sendMessage('core', 'DMA_CHANNEL_DISABLE', {'dma_channel':dmaRequestID})
        wincRXDMAChannelComment.setVisible(False)
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)
        dummyDict = Database.sendMessage('core', 'DMA_CHANNEL_ENABLE', {'dma_channel':dmaRequestID})

    # Get the allocated channel and assign it
    channel = Database.getSymbolValue('core', dmaChannelID)

    if channel is not None:
        symbol.setValue(channel)

def requestDMAComment(symbol, event):
    global wincTXRXDMA

    if(event['value'] == -2) and (wincTXRXDMA.getValue() == True):
        symbol.setVisible(True)
        event['symbol'].setVisible(False)
    else:
        symbol.setVisible(False)
