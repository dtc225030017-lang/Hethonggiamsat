#!/usr/bin/env bash
set -euo pipefail
PROJECT_ROOT="${PROJECT_ROOT:-/home/pi/Duy/Hethonggiamsat}"
"${PROJECT_ROOT}/scripts/build_arm64.sh"
"${PROJECT_ROOT}/scripts/deploy_pi.sh"
ssh "${PI_TARGET:-pi@192.168.137.227}" \
  "cd /home/pi/Duy/Hethonggiamsat && DISPLAY=\${DISPLAY:-:0} LD_LIBRARY_PATH=/usr/local/qt6/lib QT_PLUGIN_PATH=/usr/local/qt6/plugins nohup ./Hethonggiamsat >> logs/launcher.log 2>&1 &"
echo "Application launch requested on Raspberry Pi"

