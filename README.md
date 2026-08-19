# HỆ THỐNG GIÁM SÁT CHẤT LƯỢNG KHÔNG KHÍ

Ứng dụng quản lý Qt 6.5.1 chạy trên Raspberry Pi 4, nhận telemetry ESP32 qua
Mosquitto, lưu SQLite và cung cấp dashboard, biểu đồ, lịch sử, cảnh báo, CSV,
cấu hình ngưỡng cùng quản lý tài khoản ADMIN/USER.

## Build và chạy nhanh

```bash
cd /home/pi/Duy/Hethonggiamsat
./scripts/build_arm64.sh
./scripts/deploy_pi.sh
ssh pi@192.168.137.227
cd /home/pi/Duy/Hethonggiamsat
LD_LIBRARY_PATH=/usr/local/qt6/lib QT_PLUGIN_PATH=/usr/local/qt6/plugins DISPLAY=:0 ./Hethonggiamsat
```

Trong Qt Creator, Kit `Hethonggiamsat - Raspberry Pi 4 ARM64` đã được cấu hình.
Sau khi khởi động lại Qt Creator, nút Build sẽ cross-build và deploy; nút Run sẽ
khởi chạy ứng dụng trực tiếp trên Raspberry Pi.

Lần chạy đầu tạo user `admin` với mật khẩu ngẫu nhiên. Đọc duy nhất một lần tại
`data/initial_admin.txt` (permission 0600), đăng nhập rồi đổi mật khẩu. Password
database dùng PBKDF2-HMAC-SHA256 120.000 vòng, không lưu plaintext.

Self-test trên Pi:

```bash
QT_QPA_PLATFORM=offscreen LD_LIBRARY_PATH=/usr/local/qt6/lib \
QT_PLUGIN_PATH=/usr/local/qt6/plugins ./Hethonggiamsat --self-test
```

Smoke-test broker bằng chính MQTT client của ứng dụng:

```bash
QT_QPA_PLATFORM=offscreen LD_LIBRARY_PATH=/usr/local/qt6/lib \
QT_PLUGIN_PATH=/usr/local/qt6/plugins ./Hethonggiamsat --mqtt-test
```

`--integration-test` chạy MainWindow/MQTT/SQLite offscreen trong 10 giây để CI
bơm một telemetry thật qua broker và xác nhận dữ liệu đầu ra.

Chi tiết: [ARCHITECTURE.md](ARCHITECTURE.md), [MQTT_TOPICS.md](MQTT_TOPICS.md),
[DATABASE_SCHEMA.md](DATABASE_SCHEMA.md), [DEPLOYMENT.md](DEPLOYMENT.md) và
[TEST_REPORT.md](TEST_REPORT.md).

Source firmware ESP32 chuẩn được quản lý trên Windows tại
`C:\Users\admin\Documents\PlatformIO\Projects\Hethonggiamsat-esp32`. Máy ảo
chỉ giữ source Qt/cross-build để tránh hai bản firmware bị lệch nhau.
