#!/usr/bin/env bash
set -euo pipefail

SDKTOOL="/usr/libexec/qtcreator/sdktool"
SETTINGS="/home/pi/.config/QtProject/qtcreator"

if ! grep -q 'SDK.Hethonggiamsat.Qt.6.5.1.ARM64' "${SETTINGS}/qtversion.xml"; then
  "${SDKTOOL}" -s "${SETTINGS}" addQt \
    --id Hethonggiamsat.Qt.6.5.1.ARM64 \
    --name 'Qt 6.5.1 Raspberry Pi ARM64' \
    --qmake /home/pi/Qt6Cross/qt6/pi/bin/qmake \
    --type Qt4ProjectManager.QtVersion.Desktop \
    --abis arm-linux-generic-elf-64bit
fi

if ! grep -q 'Hethonggiamsat.GdbMultiarch.ARM64' "${SETTINGS}/debuggers.xml"; then
  "${SDKTOOL}" -s "${SETTINGS}" addDebugger \
    --id Hethonggiamsat.GdbMultiarch.ARM64 \
    --name 'GDB Multiarch - Raspberry Pi ARM64' \
    --engine 1 --binary /usr/bin/gdb-multiarch \
    --abis arm-linux-generic-elf-64bit
fi

if ! grep -q 'Hethonggiamsat.RPi.ARM64' "${SETTINGS}/profiles.xml"; then
  "${SDKTOOL}" -s "${SETTINGS}" addKit \
    --id Hethonggiamsat.RPi.ARM64 \
    --name 'Hethonggiamsat - Raspberry Pi 4 ARM64' \
    --devicetype Desktop --device 'Desktop Device' \
    --sysroot /home/pi/Qt6Cross/rpi-sysroot \
    --Ctoolchain 'ProjectExplorer.ToolChain.Custom:{8ff72218-187e-4f6f-987b-b0678b19df87}' \
    --Cxxtoolchain 'ProjectExplorer.ToolChain.Custom:{ebe78c9d-03de-4562-9a02-5f0d81e9ecda}' \
    --qt Hethonggiamsat.Qt.6.5.1.ARM64 \
    --debuggerid Hethonggiamsat.GdbMultiarch.ARM64 \
    --cmake '{2a9aa124-be04-4841-a68e-33a4c2453339}' \
    --cmake-generator 'Ninja:::' \
    --cmake-config 'CMAKE_TOOLCHAIN_FILE:FILEPATH=/home/pi/Qt6Cross/qt6/pi-build/toolchain.cmake' \
    --cmake-config 'CMAKE_PREFIX_PATH:PATH=/home/pi/Qt6Cross/qt6/pi' \
    --cmake-config 'CMAKE_BUILD_TYPE:STRING=Release' \
    'PE.Profile.BuildDevice' 'QString:Desktop Device'
fi

echo 'PASS: Qt Creator ARM64 kit is configured.'
