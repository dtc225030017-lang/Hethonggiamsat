#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="${PROJECT_ROOT:-/home/pi/Duy/Hethonggiamsat}"
TOOLCHAIN="${TOOLCHAIN:-/home/pi/Qt6Cross/qt6/pi-build/toolchain.cmake}"
QT_ARM64="${QT_ARM64:-/home/pi/Qt6Cross/qt6/pi}"
BUILD_DIR="${BUILD_DIR:-${PROJECT_ROOT}/build-arm64-qtcreator}"

cmake_args=(
  -S "${PROJECT_ROOT}"
  -B "${BUILD_DIR}"
  -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN}"
  -DCMAKE_PREFIX_PATH="${QT_ARM64}"
  -DCMAKE_BUILD_TYPE=Release
)
if [[ -n "${CMAKE_GENERATOR:-}" ]]; then
  cmake_args+=( -G "${CMAKE_GENERATOR}" )
fi
cmake "${cmake_args[@]}"
cmake --build "${BUILD_DIR}" --parallel
file "${BUILD_DIR}/Hethonggiamsat" | grep -q 'ARM aarch64'
echo "PASS: ${BUILD_DIR}/Hethonggiamsat is ARM64"
