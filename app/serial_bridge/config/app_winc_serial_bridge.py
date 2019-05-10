def onAttachmentConnected(source, target):
    if source['id'] == 'uart':
        plibUsed = source['component'].getSymbolByID('APP_USART_PLIB')
        plibUsed.clearValue()
        plibUsed.setValue(target['component'].getID().upper(), 1)

def onAttachmentDisconnected(source, target):
    if source['id'] == 'uart':
        plibUsed = source['component'].getSymbolByID('APP_USART_PLIB')
        plibUsed.clearValue()

def instantiateComponent(appWincSerialBridgeApp):
    print('WINC Serial Bridge Application')
    configName = Variables.get('__CONFIGURATION_NAME')

    appWincSerialBridgeApp.addDependency('uart', 'UART', False, True);

    appWincSerialBridgeApp.addDependency('wdrv_winc', 'WDRV_WINC', False, True);

    appPLIB = appWincSerialBridgeApp.createStringSymbol('APP_USART_PLIB', None)
    appPLIB.setLabel('PLIB Used')
    appPLIB.setReadOnly(True)
    appPLIB.setDefaultValue('')

    appPlatformSercomDataHdr = appWincSerialBridgeApp.createFileSymbol('APP_PLATFORM_SERCOM_DATA_FILE_SRC', None)
    appPlatformSercomDataHdr.setSourcePath('app/serial_bridge/templates/platform_sercom_data.c.ftl')
    appPlatformSercomDataHdr.setOutputName('platform_sercom_data.c')
    appPlatformSercomDataHdr.setDestPath('platform')
    appPlatformSercomDataHdr.setProjectPath('config/' + configName + '/platform/')
    appPlatformSercomDataHdr.setType('SOURCE')
    appPlatformSercomDataHdr.setMarkup(True)
    appPlatformSercomDataHdr.setOverwrite(True)

    appPlatformSercomDataSrc = appWincSerialBridgeApp.createFileSymbol('APP_PLATFORM_SERCOM_DATA_FILE_HEADER', None)
    appPlatformSercomDataSrc.setSourcePath('app/serial_bridge/templates/platform_sercom_data.h.ftl')
    appPlatformSercomDataSrc.setOutputName('platform_sercom_data.h')
    appPlatformSercomDataSrc.setDestPath('platform')
    appPlatformSercomDataSrc.setProjectPath('config/' + configName + '/platform/')
    appPlatformSercomDataSrc.setType('HEADER')
    appPlatformSercomDataSrc.setMarkup(True)
    appPlatformSercomDataSrc.setOverwrite(True)

    appPlatformSercomDataPath = appWincSerialBridgeApp.createSettingSymbol('APP_PLATFORM_SERCOM_DATA_FILE_PATH', None)
    appPlatformSercomDataPath.setValue('../src/config/' + configName + '/platform;')
    appPlatformSercomDataPath.setCategory('C32')
    appPlatformSercomDataPath.setKey('extra-include-directories')
    appPlatformSercomDataPath.setAppend(True, ';')
    appPlatformSercomDataPath.setEnabled(True)
