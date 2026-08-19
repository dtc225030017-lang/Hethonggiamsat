#!/usr/bin/env bash
set -euo pipefail

PI_TARGET="${PI_TARGET:-pi@192.168.137.227}"
PI_ROOT="/home/pi/Duy/Hethonggiamsat"

exec ssh -T "${PI_TARGET}" \
  "pkill -x Hethonggiamsat 2>/dev/null || true; cd '${PI_ROOT}' && exec env DISPLAY=:0 XAUTHORITY=/home/pi/.Xauthority LD_LIBRARY_PATH=/usr/local/qt6/lib QT_PLUGIN_PATH=/usr/local/qt6/plugins ./Hethonggiamsat"

