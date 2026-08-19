# Architecture

```text
ESP32 (SHT3x/MQ-2/LCD/LED/Buzzer)
        ↕ MQTT air/...
Mosquitto :1883 trên Raspberry Pi
        ↕ libmosquitto
Qt Widgets: Auth → Dashboard/Chart → History/Alarm → Settings/Users
        ↕ prepared SQL + transaction
SQLite data/hethonggiamsat.sqlite
```

Host x86_64 chỉ build bằng GCC 12.2.0 + sysroot Bookworm. Binary cuối là ARM64
và chạy trên Pi. `MqttService` tách MQTT khỏi UI; `SensorRepository` giới hạn
tần suất ghi; `AlarmService` dùng trạng thái ACTIVE/ENDED, hysteresis 50 ADC và
5 mẫu bình thường; chart giữ 300 mẫu. Watchdog đánh dấu DATA STALE sau 8 giây.

ESP32 tự cảnh báo cục bộ, không chờ phản hồi từ Pi. USER chỉ có dashboard và
lịch sử; các service ghi cấu hình/tài khoản luôn kiểm tra ADMIN, không chỉ ẩn UI.

