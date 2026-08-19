#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="${PROJECT_ROOT:-/home/pi/Duy/Hethonggiamsat}"
BUILD_DIR="${BUILD_DIR:-${PROJECT_ROOT}/build-arm64-qtcreator}"
PI_TARGET="${PI_TARGET:-pi@192.168.137.227}"
PI_ROOT="/home/pi/Duy/Hethonggiamsat"
BINARY="${BUILD_DIR}/Hethonggiamsat"

file "${BINARY}" | grep -q 'ARM aarch64'
ssh "${PI_TARGET}" "mkdir -p '${PI_ROOT}/data' '${PI_ROOT}/logs'"
ssh "${PI_TARGET}" "pkill -x Hethonggiamsat 2>/dev/null || true"
scp "${BINARY}" "${PI_TARGET}:${PI_ROOT}/Hethonggiamsat.new"
scp "${PROJECT_ROOT}/README.md" "${PROJECT_ROOT}/ARCHITECTURE.md" \
    "${PROJECT_ROOT}/MQTT_TOPICS.md" "${PROJECT_ROOT}/DATABASE_SCHEMA.md" \
    "${PROJECT_ROOT}/DEPLOYMENT.md" "${PI_TARGET}:${PI_ROOT}/"
ssh "${PI_TARGET}" "mv '${PI_ROOT}/Hethonggiamsat.new' '${PI_ROOT}/Hethonggiamsat' && chmod 0755 '${PI_ROOT}/Hethonggiamsat'"
echo "PASS: deployed only to ${PI_TARGET}:${PI_ROOT}"
