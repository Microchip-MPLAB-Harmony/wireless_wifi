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

    appWincSerialBridgeApp.addDependency('wdrv_winc', 'WDRV_WINC', True, True);

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
