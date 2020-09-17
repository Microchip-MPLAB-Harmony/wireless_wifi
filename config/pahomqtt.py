#/*****************************************************************************
# Copyright (C) 2013-2019 Microchip Technology Inc. and its subsidiaries.
#
# Microchip Technology Inc. and its subsidiaries.
#
# Subject to your compliance with these terms, you may use Microchip software 
# and any derivatives exclusively with Microchip products. It is your 
# responsibility to comply with third party license terms applicable to your 
# use of third party software (including open source software) that may 
# accompany Microchip software.
# 
# THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
# EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
# WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
# PURPOSE.
# 
# IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
# INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
# WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
# BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
# FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
# ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
# THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
# *****************************************************************************/
import inspect
import os
import sys
import glob
import ntpath

pahomqttOverridableSrcFiles = []
pahomqttOverridableHeaderFiles = []

def instantiateComponent(pahomqttComponent):
    setupFiles(pahomqttComponent)
    pahomqttSrcPath = pahomqttComponent.createSettingSymbol("pahomqttSrcPath", None)
    pahomqttSrcPath.setValue("../src/third_party/paho.mqtt.embedded-c")
    pahomqttSrcPath.setCategory("C32")
    pahomqttSrcPath.setKey("extra-include-directories")
    pahomqttSrcPath.setAppend(True, ";")

def get_script_dir(follow_symlinks=True):
    if getattr(sys, 'frozen', False): # py2exe, PyInstaller, cx_Freeze
        path = os.path.abspath(sys.executable)
    else:
        path = inspect.getabsfile(get_script_dir)
    if follow_symlinks:
        path = os.path.realpath(path)
    return os.path.dirname(path)

def trimFileNameList(rawList) :
    newList = []
    for file in rawList:
        filename = ntpath.basename(file)
        newList.append(filename)
    return newList
    
    
def addFileName(fileName, prefix, component, srcPath, destPath, enabled, projectPath):
    #print("Adding file: " + prefix + fileName.replace('.', '_'))
    filename = component.createFileSymbol(prefix + fileName.replace('.', '_'), None)
    filename.setProjectPath(projectPath)
    filename.setSourcePath(srcPath + fileName)
    filename.setOutputName(fileName)

    if prefix == 'misc' or prefix == 'imp':
        filename.setDestPath(destPath)
        filename.setType("IMPORTANT")
    elif fileName[-2:] == '.h':
        filename.setDestPath(destPath)
        filename.setType("HEADER")
    else:
        filename.setDestPath(destPath)
        filename.setType("SOURCE")

    filename.setEnabled(enabled)
    
    
def setupFiles(basecomponent) :

    pahomqttSourceFiles = get_script_dir() + "/../../paho.mqtt.embedded-c/MQTTPacket/src/*.c"
    pahomqttHeaderFiles = get_script_dir() + "/../../paho.mqtt.embedded-c/MQTTPacket/src/*.h"
    
    pahomqttClientSourceFiles = get_script_dir() + "/../../paho.mqtt.embedded-c/MQTTClient-C/src/*.c"
    pahomqttClientHeaderFiles = get_script_dir() + "/../../paho.mqtt.embedded-c/MQTTClient-C/src/*.h"

    pahomqttPlatformSourceFiles = get_script_dir() + "/../system/mqtt/src/MCHP*.c"
    pahomqttPlatformHeaderFiles = get_script_dir() + "/../system/mqtt/MCHP*.h"

    pmsfl = trimFileNameList(glob.glob(pahomqttSourceFiles))
    pmhfl = trimFileNameList(glob.glob(pahomqttHeaderFiles))
    
    pmcsfl = trimFileNameList(glob.glob(pahomqttClientSourceFiles))
    pmchfl = trimFileNameList(glob.glob(pahomqttClientHeaderFiles))

    pmpsfl = trimFileNameList(glob.glob(pahomqttPlatformSourceFiles))
    pmphfl = trimFileNameList(glob.glob(pahomqttPlatformHeaderFiles))

    for file in pmsfl:
        addFileName(file, "pahomqtt", basecomponent, "../paho.mqtt.embedded-c/MQTTPacket/src/", "../../third_party/paho.mqtt.embedded-c/MQTTPacket/src/", True, "paho.mqtt.embedded-c/MQTTPacket/src")
        
    for file in pmhfl:
        addFileName(file, "pahomqtt", basecomponent, "../paho.mqtt.embedded-c/MQTTPacket/src/", "../../third_party/paho.mqtt.embedded-c/MQTTPacket/src/", True, "paho.mqtt.embedded-c/MQTTPacket/src")

    for file in pmcsfl:
        addFileName(file, "pahomqtt", basecomponent, "../paho.mqtt.embedded-c/MQTTClient-C/src/", "../../third_party/paho.mqtt.embedded-c/MQTTClient-C/src/", True, "paho.mqtt.embedded-c/MQTTClient-C/src")
        
    for file in pmchfl:
        addFileName(file, "pahomqtt", basecomponent, "../paho.mqtt.embedded-c/MQTTClient-C/src/", "../../third_party/paho.mqtt.embedded-c/MQTTClient-C/src/", True, "paho.mqtt.embedded-c/MQTTClient-C/src")

    for file in pmpsfl:
        addFileName(file, "pahomqtt", basecomponent, "system/mqtt/src/", "../../third_party/paho.mqtt.embedded-c/MQTTClient-C/Platforms/", True, "paho.mqtt.embedded-c/MQTTClient-C/Platforms")
        
    for file in pmphfl:
        addFileName(file, "pahomqtt", basecomponent, "system/mqtt/", "../../third_party/paho.mqtt.embedded-c/MQTTClient-C/Platforms/", True, "paho.mqtt.embedded-c/MQTTClient-C/Platforms/")

