#!/usr/bin/env bash
set -euo pipefail

export PROJECT_ROOT="/home/pi/Duy/Hethonggiamsat"
export BUILD_DIR="${PROJECT_ROOT}/build-arm64-qtcreator"
export CMAKE_GENERATOR="Ninja"

"${PROJECT_ROOT}/scripts/build_arm64.sh"
"${PROJECT_ROOT}/scripts/deploy_pi.sh"

echo "Qt Creator: ARM64 build and Raspberry Pi deployment completed."

