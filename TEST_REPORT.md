# Final test report — 2026-08-13

## PASS

- Host cross-build Qt: GCC/G++ 12.2.0, Qt 6.5.1.
- Binary: ELF 64-bit ARM aarch64; deploy đúng
  `/home/pi/Duy/Hethonggiamsat/Hethonggiamsat`.
- Pi `ldd`: Qt 6.5.1 từ `/usr/local/qt6`, libmosquitto và mọi dependency resolve.
- ARM self-test: auth PBKDF2, ADMIN/USER logic, settings, SQLite sensor, alarm.
- Broker: service active; account `air_app` và `air_esp32`; ACL publish/subscribe
  `air/...` PASS; cấu hình cũ giữ nguyên và có backup ACL.
- App MQTT smoke-test: `APP_MQTT_TEST_RC=0`.
- Integration telemetry: MQTT → MainWindow → SQLite, `BEFORE=0 AFTER=1`, giá trị
  32.3°C / 75.3% / MQ2 1209 / 1134mV đúng.
- Alarm integration: một record, max 2200, status ENDED, có end_time.
- GUI XCB: Login chạy trên `DISPLAY=:0`, single process `Hethonggiamsat`.
- ESP32 PlatformIO compile trên Windows: SUCCESS; broker retained status xác
  nhận `air/status ONLINE` ngày 2026-08-13.

## Giới hạn kiểm thử từ máy ảo

ESP32 không được USB passthrough vào Ubuntu VM nên máy ảo không đọc Serial trực
tiếp. Thiết bị thật đã kết nối broker bằng tài khoản `air_esp32` và publish
retained `ONLINE`; upload/Serial Monitor được thực hiện từ project PlatformIO
trên Windows.
