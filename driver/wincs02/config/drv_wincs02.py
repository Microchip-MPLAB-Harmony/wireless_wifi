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

def importIncFile(component, configName, incFileEntry):
    incFilePath  = incFileEntry[0]
    isEnabled    = incFileEntry[1][0]
    callback     = incFileEntry[1][1]
    dependencies = incFileEntry[1][2]

    incFilePathTup = incFilePath.rsplit('/', 1)

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
    incFileSym.setSourcePath('driver/wincs02/' + secSName + 'include/' + incFile)
    incFileSym.setOutputName(incFile)
    incFileSym.setDestPath('driver/wifi/wincs02/include/' + secDName)
    incFileSym.setProjectPath('config/' + configName + '/driver/wifi/wincs02/' + secDName)
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
    srcFileSym.setSourcePath('driver/wincs02/' + secSName + srcFile)
    srcFileSym.setOutputName(srcFile.rsplit('/', 1)[-1])
    srcFileSym.setDestPath('driver/wifi/wincs02/' + secDName)
    srcFileSym.setProjectPath('config/' + configName + '/driver/wifi/wincs02/' + secDName)
    srcFileSym.setType('SOURCE')
    srcFileSym.setEnabled(isEnabled)

    if callback and dependencies:
        srcFileSym.setDependencies(callback, dependencies)

def setIncPath(component, configName, incPathEntry):
    incPath      = incPathEntry[0]
    isEnabled    = incPathEntry[1][0]
    callback     = incPathEntry[1][1]
    dependencies = incPathEntry[1][2]

    incPathSym = component.createSettingSymbol('WINC_INC_PATH' + incPath.replace('.', '_').replace('/', '_').upper(), None)
    incPathSym.setValue('../src/config/' + configName + '/driver/wifi/wincs02/include' + incPath + ';')
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

###############################################################################
### Component ####
###############################################################################

def instantiateComponent(drvWincComponent):
    global isDMAPresent

    print('WINCS02 Driver Component')

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

    wincRXBuffer = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_RX_BUFF_SZ', wincSpiMenu)
    wincRXBuffer.setLabel('Receive Buffer Size')
    wincRXBuffer.setDefaultValue(2048)

    # Sockets Menu
    wincSocketMenu = drvWincComponent.createMenuSymbol('DRV_WIFI_WINC_SOCKET_MENU', None)
    wincSocketMenu.setLabel('Sockets')
    wincSocketMenu.setDescription('Sockets Configuration')

    wincSocketNumber = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_SOCKET_NUM', wincSocketMenu)
    wincSocketNumber.setLabel('Number of Sockets')
    wincSocketNumber.setDefaultValue(10)

    wincSocketTxBufSz = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_SOCKET_TX_BUF_SZ', wincSocketMenu)
    wincSocketTxBufSz.setLabel('Send Buffer Size')
    wincSocketTxBufSz.setDefaultValue(1472*5)

    wincSocketRxBufSz = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_SOCKET_RX_BUF_SZ', wincSocketMenu)
    wincSocketRxBufSz.setLabel('Receive Buffer Size')
    wincSocketRxBufSz.setDefaultValue(1472*5)

    wincSocketSlabSize = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_SOCKET_SLAB_SZ', wincSocketMenu)
    wincSocketSlabSize.setLabel('Slab Size')
    wincSocketSlabSize.setDefaultValue(1472)

    wincSocketSlabNum = drvWincComponent.createIntegerSymbol('DRV_WIFI_WINC_SOCKET_SLAB_NUM', wincSocketMenu)
    wincSocketSlabNum.setLabel('Slab Number')
    wincSocketSlabNum.setDefaultValue(30)

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

    ############################################################################
    #### Code Generation ####
    ############################################################################

    condAlways = [True, None, []]

    wdrvIncFiles = [
        ['wdrv_winc.h',                                         condAlways],
        ['wdrv_winc_api.h',                                     condAlways],
        ['wdrv_winc_assoc.h',                                   condAlways],
        ['wdrv_winc_authctx.h',                                 condAlways],
        ['wdrv_winc_bssctx.h',                                  condAlways],
        ['wdrv_winc_bssfind.h',                                 condAlways],
        ['wdrv_winc_client_api.h',                              condAlways],
        ['wdrv_winc_common.h',                                  condAlways],
        ['wdrv_winc_dns.h',                                     condAlways],
        ['wdrv_winc_debug.h',                                   condAlways],
        ['wdrv_winc_dhcps.h',                                   condAlways],
        ['wdrv_winc_extcrypto.h',                               condAlways],
        ['wdrv_winc_file.h',                                    condAlways],
        ['wdrv_winc_netif.h',                                   condAlways],
        ['wdrv_winc_mqtt.h',                                    condAlways],
        ['wdrv_winc_sntp.h',                                    condAlways],
        ['wdrv_winc_socket.h',                                  condAlways],
        ['wdrv_winc_softap.h',                                  condAlways],
        ['wdrv_winc_sta.h',                                     condAlways],
        ['wdrv_winc_systime.h',                                 condAlways],
        ['wdrv_winc_tls.h',                                     condAlways],
        ['wdrv_winc_utils.h',                                   condAlways],
        ['wdrv_winc_wifi.h',                                    condAlways],
        ['dev/wdrv_winc_eint.h',                                condAlways],
    ]

    wdrvFirmwareDriverIncFiles = [
        ['nc_driver/microchip_pic32mzw2_nc_intf.h',             condAlways],
        ['nc_driver/winc_cmds.h',                               condAlways],
        ['nc_driver/winc_cmd_req.h',                            condAlways],
        ['nc_driver/winc_debug.h',                              condAlways],
        ['nc_driver/winc_dev.h',                                condAlways],
        ['nc_driver/winc_sdio_drv.h',                           condAlways],
        ['nc_driver/winc_socket.h',                             condAlways],
    ]

    for incFileEntry in wdrvIncFiles:
        importIncFile(drvWincComponent, configName, incFileEntry)

    for incFileEntry in wdrvFirmwareDriverIncFiles:
        importIncFile(drvWincComponent, configName, incFileEntry)

    wdrvSrcFiles = [
        ['wdrv_winc.c',                                         condAlways],
        ['wdrv_winc_assoc.c',                                   condAlways],
        ['wdrv_winc_authctx.c',                                 condAlways],
        ['wdrv_winc_bssctx.c',                                  condAlways],
        ['wdrv_winc_bssfind.c',                                 condAlways],
        ['wdrv_winc_dns.c',                                     condAlways],
        ['wdrv_winc_dhcps.c',                                   condAlways],
        ['wdrv_winc_extcrypto.c',                               condAlways],
        ['wdrv_winc_file.c',                                    condAlways],
        ['wdrv_winc_netif.c',                                   condAlways],
        ['wdrv_winc_mqtt.c',                                    condAlways],
        ['wdrv_winc_sntp.c',                                    condAlways],
        ['wdrv_winc_socket.c',                                  condAlways],
        ['wdrv_winc_softap.c',                                  condAlways],
        ['wdrv_winc_sta.c',                                     condAlways],
        ['wdrv_winc_systime.c',                                 condAlways],
        ['wdrv_winc_tls.c',                                     condAlways],
        ['wdrv_winc_utils.c',                                   condAlways],
        ['wdrv_winc_wifi.c',                                    condAlways],
        ['dev/wdrv_winc_eint.c',                                condAlways],
    ]

    wdrvFirmwareDriverSrcFiles = [
        ['nc_driver/winc_cmds.c',                               condAlways],
        ['nc_driver/winc_cmd_req.c',                            condAlways],
        ['nc_driver/winc_dev.c',                                condAlways],
        ['nc_driver/winc_sdio_drv.c',                           condAlways],
        ['nc_driver/winc_socket.c',                             condAlways],
    ]

    for srcFileEntry in wdrvSrcFiles:
        importSrcFile(drvWincComponent, configName, srcFileEntry)

    for srcFileEntry in wdrvFirmwareDriverSrcFiles:
        importSrcFile(drvWincComponent, configName, srcFileEntry)

    wdrvIncPaths = [
        ['/',                   condAlways],
        ['/dev',                condAlways]
    ]

    wdrvFirmwareDriverIncPaths = [
        ['/nc_driver',           condAlways],
    ]

    for incPathEntry in wdrvIncPaths:
        setIncPath(drvWincComponent, configName, incPathEntry)

    for incPathEntry in wdrvFirmwareDriverIncPaths:
        setIncPath(drvWincComponent, configName, incPathEntry)

    drvwincSystemDefFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_DEF', None)
    drvwincSystemDefFile.setType('STRING')
    drvwincSystemDefFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_INCLUDES')
    drvwincSystemDefFile.setSourcePath('driver/wincs02/templates/system/system_definitions.h.ftl')
    drvwincSystemDefFile.setMarkup(True)

    drvwincSystemDefObjFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYSTEM_DEF_OBJECT', None)
    drvwincSystemDefObjFile.setType('STRING')
    drvwincSystemDefObjFile.setOutputName('core.LIST_SYSTEM_DEFINITIONS_H_OBJECTS')
    drvwincSystemDefObjFile.setSourcePath('driver/wincs02/templates/system/system_definitions_objects.h.ftl')
    drvwincSystemDefObjFile.setMarkup(True)

    drvwincSystemConfFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYSTEM_CFG', None)
    drvwincSystemConfFile.setType('STRING')
    drvwincSystemConfFile.setOutputName('core.LIST_SYSTEM_CONFIG_H_DRIVER_CONFIGURATION')
    drvwincSystemConfFile.setSourcePath('driver/wincs02/templates/system/system_config.h.ftl')
    drvwincSystemConfFile.setMarkup(True)

    drvwincSystemInitDataFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_INIT_DATA', None)
    drvwincSystemInitDataFile.setType('STRING')
    drvwincSystemInitDataFile.setOutputName('core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA')
    drvwincSystemInitDataFile.setSourcePath('driver/wincs02/templates/system/system_initialize_data.c.ftl')
    drvwincSystemInitDataFile.setMarkup(True)

    drvwincSystemInitFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYS_INIT', None)
    drvwincSystemInitFile.setType('STRING')
    drvwincSystemInitFile.setOutputName('core.LIST_SYSTEM_INIT_C_SYS_INITIALIZE_DRIVERS')
    drvwincSystemInitFile.setSourcePath('driver/wincs02/templates/system/system_initialize.c.ftl')
    drvwincSystemInitFile.setMarkup(True)

    drvwincSystemTaskFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYSTEM_TASKS_C', None)
    drvwincSystemTaskFile.setType('STRING')
    drvwincSystemTaskFile.setOutputName('core.LIST_SYSTEM_TASKS_C_CALL_DRIVER_TASKS')
    drvwincSystemTaskFile.setSourcePath('driver/wincs02/templates/system/system_tasks.c.ftl')
    drvwincSystemTaskFile.setMarkup(True)

    drvwincDevSpiSrcFile = drvWincComponent.createFileSymbol('WDRV_WINC_DEV_SPI_SRC', None)
    drvwincDevSpiSrcFile.setSourcePath('driver/wincs02/templates/dev/wdrv_winc_spi.c.ftl')
    drvwincDevSpiSrcFile.setOutputName('wdrv_winc_spi.c')
    drvwincDevSpiSrcFile.setDestPath('/driver/wifi/wincs02/dev')
    drvwincDevSpiSrcFile.setProjectPath('config/' + configName + '/driver/wifi/wincs02/dev/spi')
    drvwincDevSpiSrcFile.setType('SOURCE')
    drvwincDevSpiSrcFile.setMarkup(True)
    drvwincDevSpiSrcFile.setOverwrite(True)

    drvwincDevSpiHdrFile = drvWincComponent.createFileSymbol('WDRV_WINC_DEV_SPI_HDR', None)
    drvwincDevSpiHdrFile.setSourcePath('driver/wincs02/templates/dev/wdrv_winc_spi.h.ftl')
    drvwincDevSpiHdrFile.setOutputName('wdrv_winc_spi.h')
    drvwincDevSpiHdrFile.setDestPath('/driver/wifi/wincs02/include/dev')
    drvwincDevSpiHdrFile.setProjectPath('config/' + configName + '/driver/wifi/wincs02/include/dev')
    drvwincDevSpiHdrFile.setType('HEADER')
    drvwincDevSpiHdrFile.setMarkup(True)
    drvwincDevSpiHdrFile.setOverwrite(True)

    drvwincSystemRtosTasksFile = drvWincComponent.createFileSymbol('DRV_WIFI_WINC_SYS_RTOS_TASK', None)
    drvwincSystemRtosTasksFile.setType('STRING')
    drvwincSystemRtosTasksFile.setOutputName('core.LIST_SYSTEM_RTOS_TASKS_C_DEFINITIONS')
    drvwincSystemRtosTasksFile.setSourcePath('driver/wincs02/templates/system/system_rtos_tasks.c.ftl')
    drvwincSystemRtosTasksFile.setMarkup(True)
    drvwincSystemRtosTasksFile.setEnabled((Database.getSymbolValue('HarmonyCore', 'SELECT_RTOS') != 'BareMetal'))
    drvwincSystemRtosTasksFile.setDependencies(setEnabledRTOSTask, ['HarmonyCore.SELECT_RTOS'])

    drvwincDevConfHdrFile = drvWincComponent.createFileSymbol('WDRV_WINC_DEV_CONF_HDR', None)
    drvwincDevConfHdrFile.setSourcePath('driver/wincs02/templates/dev/conf_winc_dev.h.ftl')
    drvwincDevConfHdrFile.setOutputName('conf_winc_dev.h')
    drvwincDevConfHdrFile.setDestPath('/driver/wifi/wincs02/include/dev')
    drvwincDevConfHdrFile.setProjectPath('config/' + configName + '/driver/wifi/wincs02/include/dev')
    drvwincDevConfHdrFile.setType('HEADER')
    drvwincDevConfHdrFile.setMarkup(True)
    drvwincDevConfHdrFile.setOverwrite(True)

def destroyComponent(drvWincComponentCommon):
    Database.sendMessage('HarmonyCore', 'ENABLE_DRV_COMMON', {'isEnabled':False})
    Database.sendMessage('HarmonyCore', 'ENABLE_SYS_COMMON', {'isEnabled':False})

    if Database.getSymbolValue('core', 'DMA_ENABLE') != None:
        Database.sendMessage('HarmonyCore', 'ENABLE_SYS_DMA', {'isEnabled':False})

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
