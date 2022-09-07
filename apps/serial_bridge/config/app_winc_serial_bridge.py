global sort_alphanumeric
global isDMAPresent

def handleMessage(messageID, args):
    return None

def sort_alphanumeric(l):
    import re
    convert = lambda text: int(text) if text.isdigit() else text.lower()
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ]
    return sorted(l, key = alphanum_key)

def onAttachmentConnected(source, target):
    global isDMAPresent

    localComponent = source['component']
    remoteComponent = target['component']
    remoteID = remoteComponent.getID()
    connectID = source['id']
    targetID = target['id']

    if connectID == 'drv_spi_dependency':
        drvSPIIndex = remoteComponent.getSymbolValue('INDEX')

        wifiDrvSPIInst = localComponent.getSymbolByID('APP_SPI_INST')
        wifiDrvSPIInstInx = localComponent.getSymbolByID('APP_SPI_INST_IDX')

        wifiDrvSPIInst.setValue('drv_spi_' + str(drvSPIIndex))
        wifiDrvSPIInst.setVisible(True)

        wifiDrvSPIInstInx.setValue(drvSPIIndex)

        localComponent.setDependencyEnabled('spi_dependency', False)
        localComponent.getSymbolByID('APP_DRV_SPI_MENU').setVisible(True)
    elif connectID == 'spi_dependency':
        plibUsed = localComponent.getSymbolByID('APP_PLIB')
        plibUsed.clearValue()
        plibUsed.setValue(remoteID.upper())

        dmaChannelSym = Database.getSymbolValue('core', 'DMA_CH_FOR_' + remoteID.upper() + '_Transmit')
        dmaRequestSym = Database.getSymbolValue('core', 'DMA_CH_NEEDED_FOR_' + remoteID.upper() + '_Transmit')

        # Do not change the order as DMA Channels needs to be allocated
        # after setting the plibUsed symbol
        # Both device and connected plib should support DMA
        if isDMAPresent == True and dmaChannelSym != None and dmaRequestSym != None:
            localComponent.getSymbolByID('APP_TX_RX_DMA').setReadOnly(False)

        localComponent.setDependencyEnabled('drv_spi_dependency', False)
        localComponent.getSymbolByID('APP_SPI_MENU').setVisible(True)
    elif connectID == 'uart':
        plibUsed = localComponent.getSymbolByID('APP_USART_PLIB')
        plibUsed.clearValue()
        plibUsed.setValue(remoteComponent.getID().upper(), 1)

def onAttachmentDisconnected(source, target):
    global isDMAPresent

    localComponent = source['component']
    remoteComponent = target['component']
    remoteID = remoteComponent.getID()
    connectID = source['id']
    targetID = target['id']

    if connectID == 'drv_spi_dependency':
        wifiDrvSPIInst = localComponent.getSymbolByID('APP_SPI_INST')
        wifiDrvSPIInstInx = localComponent.getSymbolByID('APP_SPI_INST_IDX')

        wifiDrvSPIInst.setValue('')
        wifiDrvSPIInst.setVisible(False)

        wifiDrvSPIInstInx.setValue(-1)

        localComponent.setDependencyEnabled('spi_dependency', True)
        localComponent.getSymbolByID('APP_DRV_SPI_MENU').setVisible(False)
    elif connectID == 'spi_dependency':

        dmaChannelSym = Database.getSymbolValue('core', 'DMA_CH_FOR_' + remoteID.upper() + '_Transmit')
        dmaRequestSym = Database.getSymbolValue('core', 'DMA_CH_NEEDED_FOR_' + remoteID.upper() + '_Transmit')

        # Do not change the order as DMA Channels needs to be cleared
        # before clearing the plibUsed symbol
        # Both device and connected plib should support DMA
        if isDMAPresent == True and dmaChannelSym != None and dmaRequestSym != None:
            localComponent.getSymbolByID('APP_TX_RX_DMA').clearValue()
            localComponent.getSymbolByID('APP_TX_RX_DMA').setReadOnly(True)

        plibUsed = localComponent.getSymbolByID('APP_PLIB')
        plibUsed.clearValue()

        localComponent.setDependencyEnabled('drv_spi_dependency', True)
        localComponent.getSymbolByID('APP_SPI_MENU').setVisible(False)
    elif connectID == 'uart':
        plibUsed = localComponent.getSymbolByID('APP_USART_PLIB')
        plibUsed.clearValue()

def updateDMAEnableCntr(symbol, event):
    result_dict = {}

    if symbol.getValue() != event['value']:
        symbol.setValue(event['value'])
        if symbol.getValue() == True:
            result_dict = Database.sendMessage('drv_spi', 'APP_DMA_ENABLED', result_dict)
        else:
            result_dict = Database.sendMessage('drv_spi', 'APP_DMA_DISABLED', result_dict)

def sysDMAEnabled(symbol, event):
    if symbol.getValue() != event['value']:
        symbol.setValue(event['value'])
        if Database.getSymbolValue('core', 'DMA_ENABLE') != None:
            Database.sendMessage('HarmonyCore', 'ENABLE_SYS_DMA', {'isEnabled':event['value']})

def instantiateComponent(appWincSerialBridgeApp):
    global isDMAPresent

    print('WINC Serial Bridge Application')

    if Database.getSymbolValue('core', 'DMA_ENABLE') == None:
        isDMAPresent = False
    else:
        isDMAPresent = True

    configName = Variables.get('__CONFIGURATION_NAME')

    eicNode = ATDF.getNode("/avr-tools-device-file/devices/device/peripherals/module@[name=\"EIC\"]/instance@[name=\"EIC\"]/parameters/param@[name=\"EXTINT_NUM\"]")

    appWincSerialBridgeApp.addDependency('uart', 'UART', False, True);
    appWincSerialBridgeApp.addDependency('wdrv_winc', 'WDRV_WINC', True, True);
    appWincSerialBridgeApp.addDependency('drv_spi_dependency', 'DRV_SPI', False, True)
    appWincSerialBridgeApp.addDependency('spi_dependency', 'SPI', False, True)

    appWincSerialBridgeApp.setDependencyEnabled('drv_spi_dependency', False)
    appWincSerialBridgeApp.setDependencyEnabled('spi_dependency', False)

    appPLIB = appWincSerialBridgeApp.createStringSymbol('APP_USART_PLIB', None)
    appPLIB.setLabel('PLIB Used')
    appPLIB.setReadOnly(True)
    appPLIB.setDefaultValue('')

    appPlatformSercomSrc = appWincSerialBridgeApp.createFileSymbol('APP_PLATFORM_SERCOM_FILE_SRC', None)
    appPlatformSercomSrc.setSourcePath('apps/serial_bridge/templates/platform_sercom.c.ftl')
    appPlatformSercomSrc.setOutputName('platform_sercom.c')
    appPlatformSercomSrc.setDestPath('platform')
    appPlatformSercomSrc.setProjectPath('config/' + configName + '/platform/')
    appPlatformSercomSrc.setType('SOURCE')
    appPlatformSercomSrc.setMarkup(True)
    appPlatformSercomSrc.setOverwrite(True)

    appPlatformSercomDataPath = appWincSerialBridgeApp.createSettingSymbol('APP_PLATFORM_SERCOM_DATA_FILE_PATH', None)
    appPlatformSercomDataPath.setValue('../src/config/' + configName + '/platform;')
    appPlatformSercomDataPath.setCategory('C32')
    appPlatformSercomDataPath.setKey('extra-include-directories')
    appPlatformSercomDataPath.setAppend(True, ';')
    appPlatformSercomDataPath.setEnabled(True)

    if any((w in Variables.get('__PROCESSOR') for w in ['SAMD21', 'SAME54'])):
        appSecConnEn = appWincSerialBridgeApp.createBooleanSymbol('APP_ENABLE_SEC_CONN', None)
        appSecConnEn.setLabel('Enable Secondary WINC Connection?')

        appSecConnMenu = appWincSerialBridgeApp.createMenuSymbol('APP_SEC_CONN_MENU', None)
        appSecConnMenu.setLabel('Secondary WINC Connection')
        appSecConnMenu.setVisible(False)
        appSecConnMenu.setDependencies(setVisibilitySecConn, ['APP_ENABLE_SEC_CONN'])

        # DRV_SPI Menu
        appDrvSpiMenu = appWincSerialBridgeApp.createMenuSymbol('APP_DRV_SPI_MENU', appSecConnMenu)
        appDrvSpiMenu.setLabel('SPI')
        appDrvSpiMenu.setDescription('SPI using DRV_SPI')
        appDrvSpiMenu.setVisible(False)

        # SPI Instance Index
        appSpiInst = appWincSerialBridgeApp.createStringSymbol('APP_SPI_INST', appDrvSpiMenu)
        appSpiInst.setLabel('SPI Driver')
        appSpiInst.setReadOnly(True)
        appSpiInst.setDefaultValue('')

        appSpiInstIdx = appWincSerialBridgeApp.createIntegerSymbol('APP_SPI_INST_IDX', appDrvSpiMenu)
        appSpiInstIdx.setVisible(False)
        appSpiInstIdx.setDefaultValue(-1)

        # SPI (PLIB) Menu
        appSpiMenu = appWincSerialBridgeApp.createMenuSymbol('APP_SPI_MENU', appSecConnMenu)
        appSpiMenu.setLabel('SPI')
        appSpiMenu.setDescription('SPI using PLIB')
        appSpiMenu.setVisible(False)

        appSymPLIB = appWincSerialBridgeApp.createStringSymbol('APP_PLIB', appSpiMenu)
        appSymPLIB.setLabel('PLIB Used')
        appSymPLIB.setReadOnly(True)
        appSymPLIB.setDefaultValue('')

        global appTXRXDMA
        appTXRXDMA = appWincSerialBridgeApp.createBooleanSymbol('APP_TX_RX_DMA', appSpiMenu)
        appTXRXDMA.setLabel('Use DMA for Transmit and Receive?')
        appTXRXDMA.setVisible(isDMAPresent)
        appTXRXDMA.setReadOnly(True)

        appTXRXDMAEn = appWincSerialBridgeApp.createBooleanSymbol('APP_TX_RX_DMA_EN', appSpiMenu)
        appTXRXDMAEn.setVisible(False)
        appTXRXDMAEn.setDefaultValue(False)
        appTXRXDMAEn.setDependencies(updateDMAEnableCntr, ['APP_TX_RX_DMA'])

        global appTXDMAChannel
        appTXDMAChannel = appWincSerialBridgeApp.createIntegerSymbol('APP_TX_DMA_CHANNEL', appSpiMenu)
        appTXDMAChannel.setLabel('DMA Channel For Transmit')
        appTXDMAChannel.setDefaultValue(0)
        appTXDMAChannel.setVisible(False)
        appTXDMAChannel.setReadOnly(True)
        appTXDMAChannel.setDependencies(requestAndAssignTxDMAChannel, ['APP_TX_RX_DMA'])

        global appTXDMAChannelComment
        appTXDMAChannelComment = appWincSerialBridgeApp.createCommentSymbol('APP_TX_DMA_CH_COMMENT', appSpiMenu)
        appTXDMAChannelComment.setLabel("Warning!!! Couldn't Allocate DMA Channel for Transmit. Check DMA Manager. !!!")
        appTXDMAChannelComment.setVisible(False)
        appTXDMAChannelComment.setDependencies(requestDMAComment, ['APP_TX_DMA_CHANNEL'])

        global appRXDMAChannel
        appRXDMAChannel = appWincSerialBridgeApp.createIntegerSymbol('APP_RX_DMA_CHANNEL', appSpiMenu)
        appRXDMAChannel.setLabel('DMA Channel For Receive')
        appRXDMAChannel.setDefaultValue(1)
        appRXDMAChannel.setVisible(False)
        appRXDMAChannel.setReadOnly(True)
        appRXDMAChannel.setDependencies(requestAndAssignRxDMAChannel, ['APP_TX_RX_DMA'])

        global appRXDMAChannelComment
        appRXDMAChannelComment = appWincSerialBridgeApp.createCommentSymbol('APP_RX_DMA_CH_COMMENT', appSpiMenu)
        appRXDMAChannelComment.setLabel("Warning!!! Couldn't Allocate DMA Channel for Receive. Check DMA Manager. !!!")
        appRXDMAChannelComment.setVisible(False)
        appRXDMAChannelComment.setDependencies(requestDMAComment, ['APP_RX_DMA_CHANNEL'])

        appSymSYSDMAEnable = appWincSerialBridgeApp.createBooleanSymbol('APP_SYS_DMA_ENABLE', appSecConnMenu)
        appSymSYSDMAEnable.setDefaultValue(False)
        appSymSYSDMAEnable.setVisible(False)
        appSymSYSDMAEnable.setDependencies(sysDMAEnabled, ['APP_TX_RX_DMA'])

        # Interrupt Source
        appIntSrcList = []

        if eicNode:
            appIntSrcList.append('EIC')
        else:
            periphNode = ATDF.getNode('/avr-tools-device-file/devices/device/peripherals')
            modules = periphNode.getChildren()

            for module in range (0, len(modules)):
                periphName = str(modules[module].getAttribute('name'))
                if periphName == 'PIO':
                    appIntSrcList.append('PIO')
                elif periphName == 'GPIO':
                    appIntSrcList.append('GPIO')

        if len(appIntSrcList):
            appIntSrc = appWincSerialBridgeApp.createComboSymbol('APP_INT_SRC', appSecConnMenu, appIntSrcList)
            appIntSrc.setLabel('Interrupt Source')
            appIntSrc.setVisible(True)

        # External Interrupt Selection
        if eicNode:
            extIntCount = int(eicNode.getAttribute('value'))

            eicSrcSelDepList = []

            for x in range(extIntCount):
                appEicSrcX = appWincSerialBridgeApp.createBooleanSymbol('APP_EIC_SRC_' + str(x), appIntSrc)
                appEicSrcX.setLabel('EIC Channel ' + str(x))

                if Database.getSymbolValue('eic', 'EIC_INT_' + str(x)):
                    appEicSrcX.setVisible(True)
                else:
                    appEicSrcX.setVisible(False)
                    appEicSrcX.setValue(False)

                appEicSrcX.setDependencies(setVisibilityEicSource, ['APP_INT_SRC', 'eic.EIC_INT'])

                eicSrcSelDepList.append('APP_EIC_SRC_' + str(x))

            appEicSrcSel = appWincSerialBridgeApp.createIntegerSymbol('APP_EIC_SRC_SELECT', appSecConnMenu)
            appEicSrcSel.setVisible(False)
            appEicSrcSel.setDependencies(setValueEicSource, eicSrcSelDepList)
            appEicSrcSel.setDefaultValue(-1)
            appEicSrcSel.setMin(-1)
            appEicSrcSel.setMax(extIntCount-1)

            appSymPinConfigComment = appWincSerialBridgeApp.createCommentSymbol('APP_EIC_CONFIG_COMMENT', appSecConnMenu)
            appSymPinConfigComment.setLabel('***EIC channel must be configured in EIC component for interrupt source***')

        elif 'GPIO' in appIntSrcList:
            # Devices using GPIO Interrupts such as PIC32MZ
            availablePinDictionary = {}
            # Send message to core to get available pins
            availablePinDictionary = Database.sendMessage('core', 'PIN_LIST', availablePinDictionary)

            appGpioIntSrc = appWincSerialBridgeApp.createKeyValueSetSymbol('APP_GPIO_SRC_SELECT', appSecConnMenu)
            appGpioIntSrc.setLabel('Interrupt Pin')
            appGpioIntSrc.setOutputMode('Key')
            appGpioIntSrc.setDisplayMode('Description')

            for pad in sort_alphanumeric(availablePinDictionary.values()):
                key = 'GPIO_PIN_' + pad
                value = list(availablePinDictionary.keys())[list(availablePinDictionary.values()).index(pad)]
                description = pad
                appGpioIntSrc.addKey(key, value, description)

            appGpioIntSrc.addKey('GPIO_PIN_NONE', '-1', 'None')
            appGpioIntSrc = appWincSerialBridgeApp.createCommentSymbol('APP_PINS_CONFIG_COMMENT', appSecConnMenu)
            appGpioIntSrc.setLabel('***Above selected pins must be configured as GPIO Output in Pin Manager***')

        elif 'PIO' in appIntSrcList:
            #PIO is used in Cortex-M7 and MPU devices
            availablePinDictionary = {}
            # Send message to core to get available pins
            availablePinDictionary = Database.sendMessage('core', 'PIN_LIST', availablePinDictionary)

            appPioIntSrc = appWincSerialBridgeApp.createKeyValueSetSymbol('APP_PIO_SRC_SELECT', appSecConnMenu)
            appPioIntSrc.setLabel('Interrupt Pin')
            appPioIntSrc.setOutputMode('Key')
            appPioIntSrc.setDisplayMode('Description')

            for pad in sort_alphanumeric(availablePinDictionary.values()):
                key = 'PIO_PIN_' + pad
                value = list(availablePinDictionary.keys())[list(availablePinDictionary.values()).index(pad)]
                description = pad
                appPioIntSrc.addKey(key, value, description)

            appPioIntSrc.addKey('PIO_PIN_NONE', '-1', 'None')
            appPioIntSrc = appWincSerialBridgeApp.createCommentSymbol('APP_PINS_CONFIG_COMMENT', appSecConnMenu)
            appPioIntSrc.setLabel('***Above selected pins must be configured as PIO Output in Pin Manager***')

        appSystemConfFile = appWincSerialBridgeApp.createFileSymbol('APP_SYSTEM_CFG', None)
        appSystemConfFile.setType('STRING')
        appSystemConfFile.setOutputName('core.LIST_SYSTEM_CONFIG_H_APPLICATION_CONFIGURATION')
        appSystemConfFile.setSourcePath('apps/serial_bridge/templates/system/system_config.h.ftl')
        appSystemConfFile.setMarkup(True)

        appSystemInitDataFile = appWincSerialBridgeApp.createFileSymbol('APP_INIT_DATA', appSecConnMenu)
        appSystemInitDataFile.setType('STRING')
        appSystemInitDataFile.setOutputName('core.LIST_SYSTEM_INIT_C_DRIVER_INITIALIZATION_DATA')
        appSystemInitDataFile.setSourcePath('apps/serial_bridge/templates/system/system_initialize_data.c.ftl')
        appSystemInitDataFile.setMarkup(True)

def setVisibilityEicSource(symbol, event):
    eicUseSym = symbol.getComponent().getSymbolByID('APP_INT_SRC')

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
    eicCurSel = symbol.getComponent().getSymbolByID('APP_EIC_SRC_SELECT')

    eicID = int(event['id'].split('_')[-1])

    if symbol.getComponent().getSymbolByID(event['id']).getValue():
        eicCurSel.setValue(eicID)
        for x in range(eicCurSel.getMax()+1):
            if x != eicID:
                symbol.getComponent().getSymbolByID('APP_EIC_SRC_' + str(x)).setVisible(False)
    elif eicCurSel.getValue() == eicID:
        for x in range(eicCurSel.getMax()+1):
            if Database.getSymbolValue('eic', 'EIC_INT_' + str(x)):
                symbol.getComponent().getSymbolByID('APP_EIC_SRC_' + str(x)).setVisible(True)

        eicCurSel.setValue(-1)

def requestAndAssignTxDMAChannel(symbol, event):
    global appTXDMAChannelComment
    component = symbol.getComponent()

    spiPeripheral = component.getSymbolValue('APP_PLIB')

    dmaChannelID = 'DMA_CH_FOR_' + str(spiPeripheral) + '_Transmit'
    dmaRequestID = 'DMA_CH_NEEDED_FOR_' + str(spiPeripheral) + '_Transmit'

    # Clear the DMA symbol. Done for backward compatibility.
    Database.clearSymbolValue('core', dmaRequestID)

    dummyDict = {}

    if event['value'] == False:
        dummyDict = Database.sendMessage('core', 'DMA_CHANNEL_DISABLE', {'dma_channel':dmaRequestID})
        appTXDMAChannelComment.setVisible(False)
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)
        dummyDict = Database.sendMessage('core', 'DMA_CHANNEL_ENABLE', {'dma_channel':dmaRequestID})

    # Get the allocated channel and assign it
    channel = Database.getSymbolValue('core', dmaChannelID)

    if channel is not None:
        symbol.setValue(channel)

def requestAndAssignRxDMAChannel(symbol, event):
    global appRXDMAChannelComment
    component = symbol.getComponent()

    spiPeripheral = component.getSymbolValue('APP_PLIB')

    dmaChannelID = 'DMA_CH_FOR_' + str(spiPeripheral) + '_Receive'
    dmaRequestID = 'DMA_CH_NEEDED_FOR_' + str(spiPeripheral) + '_Receive'

    # Clear the DMA symbol. Done for backward compatibility.
    Database.clearSymbolValue('core', dmaRequestID)

    dummyDict = {}

    if event['value'] == False:
        dummyDict = Database.sendMessage('core', 'DMA_CHANNEL_DISABLE', {'dma_channel':dmaRequestID})
        appRXDMAChannelComment.setVisible(False)
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)
        dummyDict = Database.sendMessage('core', 'DMA_CHANNEL_ENABLE', {'dma_channel':dmaRequestID})

    # Get the allocated channel and assign it
    channel = Database.getSymbolValue('core', dmaChannelID)

    if channel is not None:
        symbol.setValue(channel)

def requestDMAComment(symbol, event):
    global appTXRXDMA

    if(event['value'] == -2) and (appTXRXDMA.getValue() == True):
        symbol.setVisible(True)
        event['symbol'].setVisible(False)
    else:
        symbol.setVisible(False)

def setVisibilitySecConn(symbol, event):
    component = symbol.getComponent()

    if event['value'] == True:
        symbol.setVisible(True)
        if not component.getSymbolByID('APP_SPI_MENU').getVisible():
            component.setDependencyEnabled('drv_spi_dependency', True)
        if not component.getSymbolByID('APP_DRV_SPI_MENU').getVisible():
            component.setDependencyEnabled('spi_dependency', True)
    else:
        symbol.setVisible(False)
        component.setDependencyEnabled('drv_spi_dependency', False)
        component.setDependencyEnabled('spi_dependency', False)
