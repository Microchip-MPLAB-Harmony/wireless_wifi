import os

def instantiateComponent(appExampleApp):
    print('Power Save Mode Application')
    configName = Variables.get('__CONFIGURATION_NAME')
    templatePath = os.path.relpath(os.path.normpath(os.path.join(Variables.get("__PROJECT_SRC_PATH"), '../../templates')), start=Module.getPath())

    appExampleApp.addDependency('wdrv_winc', 'WDRV_WINC', True, True);

    appWifiSsid = appExampleApp.createStringSymbol('APP_WIFI_SSID', None)
    appWifiSsid.setLabel('WiFi SSID')
    appWifiSsid.setDefaultValue('MicrochipDemoAp')

    appWifiMode = appExampleApp.createComboSymbol('APP_WIFI_MODE', None, ['STA', 'AP'])
    appWifiMode.setLabel('WiFi Mode')
    appWifiMode.setVisible(True)
    appWifiMode.setDefaultValue('STA')

    AppWifiChannel = appExampleApp.createIntegerSymbol('APP_WIFI_CHANNEL', None)
    AppWifiChannel.setLabel('WiFi Channel');
    AppWifiChannel.setVisible(False)
    AppWifiChannel.setDependencies(setVisibilityOnAP, ['APP_WIFI_MODE'])
    AppWifiChannel.setDefaultValue(1)
    AppWifiChannel.setMin(1)
    AppWifiChannel.setMax(11)

    appWifiAuthMethodSta = appExampleApp.createComboSymbol('APP_WIFI_AUTH_METHOD_STA', None, ['Open', 'WEP', 'WPA'])
    appWifiAuthMethodSta.setLabel('WiFi Authentication')
    appWifiAuthMethodSta.setVisible(True)
    appWifiAuthMethodSta.setDefaultValue('Open')
    appWifiAuthMethodSta.setDependencies(setVisibilityOnSTARefresh, ['APP_WIFI_MODE'])

    appWifiAuthMethodAp = appExampleApp.createComboSymbol('APP_WIFI_AUTH_METHOD_AP', None, ['Open', 'WEP'])
    appWifiAuthMethodAp.setLabel('WiFi Authentication')
    appWifiAuthMethodAp.setVisible(False)
    appWifiAuthMethodAp.setDefaultValue('Open')
    appWifiAuthMethodAp.setDependencies(setVisibilityOnAPRefresh, ['APP_WIFI_MODE'])

    appWifiAuthMenu = appExampleApp.createMenuSymbol('APP_WIFI_AUTH_MENU', None)
    appWifiAuthMenu.setLabel("WiFi Authentication Settings")
    appWifiAuthMenu.setVisible(False)
    appWifiAuthMenu.setDependencies(setVisibilityOnNotOpen, ['APP_WIFI_AUTH_METHOD_STA', 'APP_WIFI_AUTH_METHOD_AP'])

    appWifiWepKeyIdx = appExampleApp.createComboSymbol('APP_WIFI_WEP_KEY_IDX', appWifiAuthMenu, ['1', '2', '3', '4'])
    appWifiWepKeyIdx.setLabel('WEP Key Index')
    appWifiWepKeyIdx.setVisible(False)
    appWifiWepKeyIdx.setDefaultValue('1')
    appWifiWepKeyIdx.setDependencies(setVisibilityOnWEP, ['APP_WIFI_AUTH_METHOD_STA', 'APP_WIFI_AUTH_METHOD_AP'])

    appWifiWepKey = appExampleApp.createStringSymbol('APP_WIFI_WEP_KEY', appWifiAuthMenu)
    appWifiWepKey.setLabel('WEP Key')
    appWifiWepKey.setVisible(False)
    appWifiWepKey.setDefaultValue('1234567890')
    appWifiWepKey.setDependencies(setVisibilityOnWEP, ['APP_WIFI_AUTH_METHOD_STA', 'APP_WIFI_AUTH_METHOD_AP'])

    appWifiWpaPsk = appExampleApp.createStringSymbol('APP_WIFI_WPA_PSK', appWifiAuthMenu)
    appWifiWpaPsk.setLabel('WPA PSK')
    appWifiWpaPsk.setVisible(False)
    appWifiWpaPsk.setDefaultValue('12345678')
    appWifiWpaPsk.setDependencies(setVisibilityOnWPA, ['APP_WIFI_AUTH_METHOD_STA', 'APP_WIFI_AUTH_METHOD_AP'])

    appWifiApDhcpSrvEnable = appExampleApp.createBooleanSymbol('APP_WIFI_AP_DHCP_SRV_ENABLE', None)
    appWifiApDhcpSrvEnable.setLabel("DHCP Server Enabled?")
    appWifiApDhcpSrvEnable.setVisible(False)
    appWifiApDhcpSrvEnable.setDefaultValue(False)
    appWifiApDhcpSrvEnable.setDependencies(setVisibilityOnAP, ['APP_WIFI_MODE'])

    appWifiApDhcpSrvAddr = appExampleApp.createStringSymbol('APP_WIFI_AP_DHCP_SRV_ADDR', appWifiApDhcpSrvEnable)
    appWifiApDhcpSrvAddr.setLabel('Server IP Address')
    appWifiApDhcpSrvAddr.setVisible(False)
    appWifiApDhcpSrvAddr.setDefaultValue('192.168.1.1')
    appWifiApDhcpSrvAddr.setDependencies(setVisibilityOnDhcpSrvEnable, ['APP_WIFI_AP_DHCP_SRV_ENABLE'])

    appWifiApDhcpSrvNetmask = appExampleApp.createStringSymbol('APP_WIFI_AP_DHCP_SRV_NETMASK', appWifiApDhcpSrvEnable)
    appWifiApDhcpSrvNetmask.setLabel('Server Netmask')
    appWifiApDhcpSrvNetmask.setVisible(False)
    appWifiApDhcpSrvNetmask.setDefaultValue('255.255.255.0')
    appWifiApDhcpSrvNetmask.setDependencies(setVisibilityOnDhcpSrvEnable, ['APP_WIFI_AP_DHCP_SRV_ENABLE'])

    appWifiTcpSock = appExampleApp.createBooleanSymbol('APP_WIFI_TCP_SOCK_ENABLE', None)
    appWifiTcpSock.setLabel('Enable TCP Socket?')
    appWifiTcpSock.setVisible(True)
    appWifiTcpSock.setDefaultValue(False)

    appWifiTcpSockPort = appExampleApp.createIntegerSymbol('APP_WIFI_TCP_SOCK_PORT', appWifiTcpSock)
    appWifiTcpSockPort.setLabel('Port');
    appWifiTcpSockPort.setVisible(False)
    appWifiTcpSockPort.setDefaultValue(80)
    appWifiTcpSockPort.setMin(1)
    appWifiTcpSockPort.setMax(65535)
    appWifiTcpSockPort.setDependencies(setVisibilityOnTcpSockEnable, ['APP_WIFI_TCP_SOCK_ENABLE'])

    appWifiTcpSockBufSize = appExampleApp.createIntegerSymbol('APP_WIFI_TCP_SOCK_BUF_SIZE', appWifiTcpSock)
    appWifiTcpSockBufSize.setLabel('Buffer Size');
    appWifiTcpSockBufSize.setVisible(False)
    appWifiTcpSockBufSize.setDefaultValue(1460)
    appWifiTcpSockBufSize.setDependencies(setVisibilityOnTcpSockEnable, ['APP_WIFI_TCP_SOCK_ENABLE'])

    appWifiWpsModeEnable = appExampleApp.createBooleanSymbol('APP_WIFI_WPS_ENABLE', None)
    appWifiWpsModeEnable.setLabel('Enable WPS?')
    appWifiWpsModeEnable.setVisible(True)
    appWifiWpsModeEnable.setDefaultValue(False)

    appWifiWpsMode = appExampleApp.createComboSymbol('APP_WIFI_WPS_MODE', appWifiWpsModeEnable, ['PBC', 'PIN'])
    appWifiWpsMode.setLabel('WPS Mode')
    appWifiWpsMode.setVisible(False)
    appWifiWpsMode.setDefaultValue('PBC')
    appWifiWpsMode.setDependencies(setVisibilityOnWpsEnable, ['APP_WIFI_WPS_ENABLE'])

    appWifiWpsPin = appExampleApp.createStringSymbol('APP_WIFI_WPS_PIN_NUMBER', appWifiWpsModeEnable)
    appWifiWpsPin.setLabel('PIN Number')
    appWifiWpsPin.setVisible(False)
    appWifiWpsPin.setDefaultValue('12345670')
    appWifiWpsPin.setDependencies(setVisibilityOnWpsPinEnable, ['APP_WIFI_WPS_MODE'])

    appExampleConfHdr = appExampleApp.createFileSymbol('APP_EXAMPLE_CONF_HEADER', None)
    appExampleConfHdr.setSourcePath(os.path.join(templatePath, 'example_conf.h.ftl'))
    appExampleConfHdr.setOutputName('example_conf.h')
    appExampleConfHdr.setDestPath('.')
    appExampleConfHdr.setProjectPath('config/' + configName)
    appExampleConfHdr.setType('HEADER')
    appExampleConfHdr.setMarkup(True)
    appExampleConfHdr.setOverwrite(True)

def setVisibilityOnNotOpen(symbol, event):
    if event['value'] == 'Open':
        symbol.setVisible(False)
    else:
        symbol.setVisible(True)

def setVisibilityOnWEP(symbol, event):
    if event['value'] == 'WEP':
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def setVisibilityOnWPA(symbol, event):
    if event['value'] == 'WPA':
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def setVisibilityOnSTARefresh(symbol, event):
    if event['value'] == 'STA':
        symbol.setVisible(True)
        symbol.setValue(symbol.getValue())
    else:
        symbol.setVisible(False)

def setVisibilityOnAPRefresh(symbol, event):
    if event['value'] == 'AP':
        symbol.setVisible(True)
        symbol.setValue(symbol.getValue())
    else:
        symbol.setVisible(False)

def setVisibilityOnAP(symbol, event):
    if event['value'] == 'AP':
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def setVisibilityOnTcpSockEnable(symbol, event):
    if event['value'] == True:
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def setVisibilityOnDhcpSrvEnable(symbol, event):
    if event['value'] == True:
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def setVisibilityOnWpsEnable(symbol, event):
    if event['value'] == True:
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

    symbol.setValue(symbol.getValue())

def setVisibilityOnWpsPinEnable(symbol, event):
    if event['value'] == 'PBC':
        symbol.setVisible(False)
    else:
        symbol.setVisible(symbol.getComponent().getSymbolValue('APP_WIFI_WPS_ENABLE'))
