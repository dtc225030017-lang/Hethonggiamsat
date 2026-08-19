# Deployment

Host thực tế:

- toolchain `/home/pi/Qt6Cross/qt6/pi-build/toolchain.cmake`
- Qt target `/home/pi/Qt6Cross/qt6/pi`
- compiler `/opt/cross-pi-gcc/bin/aarch64-linux-gnu-g++`

Chạy `scripts/build_deploy_run.sh`. Script kiểm tra ARM64, chỉ tạo/chạm vào
`/home/pi/Duy/Hethonggiamsat`, và chỉ dừng process tên chính xác
`Hethonggiamsat`.

## Qt Creator

Kit `Hethonggiamsat - Raspberry Pi 4 ARM64` đã được cài cho Qt Creator 6.0.2.
Project có sẵn `CMakeLists.txt.user` và dùng build directory
`build-arm64-qtcreator`.

- Nút **Build** gọi `scripts/qtcreator_build_deploy.sh`: Ninja cross-build,
  kiểm tra ELF ARM64 rồi deploy qua SSH.
- Nút **Run** tự Build trước, sau đó gọi `scripts/run_from_qtcreator.sh` và chạy
  binary trên màn hình `:0` của Pi với Qt runtime `/usr/local/qt6`.
- Cần khởi động lại Qt Creator sau lần cài Kit đầu tiên.

Qt Creator settings cũ được backup tại
`backups/qtcreator_20260813/`; các Kit Desktop/project cũ không bị sửa.

Runtime:

```bash
export LD_LIBRARY_PATH=/usr/local/qt6/lib
export QT_PLUGIN_PATH=/usr/local/qt6/plugins
export DISPLAY=:0
./Hethonggiamsat
```

Log: `logs/hethonggiamsat.log`, launcher: `logs/launcher.log`. Nếu GUI không mở,
kiểm tra `DISPLAY`, phiên desktop và plugin `platforms/libqxcb.so`. Nếu MQTT
không kết nối, kiểm tra service Mosquitto, ACL `air/...`, username và secret
file mode 0600. Chỉ tạo systemd sau khi chạy GUI thủ công ổn định.
