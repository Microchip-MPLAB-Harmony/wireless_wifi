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
######################  WiFi Driver  ######################
def loadModule():
    print('Load Module: Harmony WiFi Driver')

    ## WIFI Driver
    drvWifiWincComponent = Module.CreateComponent('drvWifiWinc', 'WINC', '/Wireless/Drivers', 'driver/winc/config/drv_winc.py')
    drvWifiWincComponent.addCapability('wdrv_winc', 'WDRV_WINC', True)
    drvWifiWincComponent.addCapability('libdrvWincMac', 'MAC')

    drvWifiWincLiteComponent = Module.CreateComponent('drvWifiWincLite', 'WINC (Lite)', '/Wireless/Drivers', 'driver/winc/config/drv_winc_lite.py')
    drvWifiWincLiteComponent.addCapability('wdrv_winc', 'WDRV_WINC', True)
    drvWifiWincLiteComponent.addCapability('libdrvWincMac', 'MAC')

    drvWifiWincS02Component = Module.CreateComponent('drvWifiWincS02', 'WINCS02', '/Wireless/Drivers', 'driver/wincs02/config/drv_wincs02.py')
    drvWifiWincS02Component.addCapability('wdrv_wincs02', 'WDRV_WINC', True)

    if 'PIC32MZ1025W104132' in Variables.get('__PROCESSOR') or 'PIC32MZ2051W104132' in Variables.get('__PROCESSOR') or 'WFI32E01' in Variables.get('__PROCESSOR') or 'WFI32E02' in Variables.get('__PROCESSOR') or 'WFI32E03' in Variables.get('__PROCESSOR'):
        drvWifiPic32mzw1Component = Module.CreateComponent('drvWifiPic32mzw1', 'PIC32MZW1', '/Wireless/Drivers/', 'driver/pic32mzw1/config/drv_pic32mzw1.py')
        drvWifiPic32mzw1Component.addCapability('wdrv_pic32mzw1', 'WDRV_PIC32MZW1', True)
        drvWifiPic32mzw1Component.addCapability('libdrvPic32mzw1Mac', 'MAC')

    ## Ethernet MAC driver for PIC32MZW1
        drvPic32mEthmacComponent = Module.CreateComponent('drvPic32mEthmac', 'ETHMAC', '/Harmony/Drivers/MAC Driver/Internal/', 'driver/ethmac/config/drv_intmac_ethmac.py')
        drvPic32mEthmacComponent.addCapability('libdrvPic32mEthmac','MAC')
        drvPic32mEthmacComponent.addDependency('ETHMAC_PHY_Dependency', 'PHY', None, False, True)
        drvPic32mEthmacComponent.setDisplayType("MAC Layer")

    ## Serial Bridge Application
    appSerBridge = Module.CreateComponent('appWincSerialBridge', 'Serial Bridge Application', '/Wireless/SerialBridge/', 'apps/serial_bridge/config/app_winc_serial_bridge.py')
