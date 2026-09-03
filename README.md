# Hệ thống giám sát chất lượng không khí

Đây là hệ thống IoT gồm firmware **ESP32** và ứng dụng desktop **Qt 6 Widgets** chạy trên Linux/Raspberry Pi. ESP32 đọc SHT3x và MQ-2, điều khiển LCD/LED/còi tại chỗ, rồi trao đổi telemetry, cảnh báo và cấu hình với ứng dụng qua MQTT. Ứng dụng hiển thị dữ liệu, lưu SQLite, quản lý tài khoản và xuất CSV.

Tài liệu này được đối chiếu từ mã nguồn và cấu hình hiện có tại thời điểm cập nhật; không suy diễn các chức năng chưa được lập trình.

## 1. Phạm vi thực thi

```text
SHT3x ─I2C─┐
MQ-2 AO ──┼─> ESP32 ─Wi-Fi/MQTT─> MQTT broker <─libmosquitto─> Qt desktop app
LCD/LED/còi┘                                      │                 │
                                                  └────────────> SQLite + CSV
```

- Firmware ESP32: Arduino/PlatformIO cho board `esp32dev`.
- Ứng dụng desktop: C++17, Qt 6 (`Core`, `Gui`, `Widgets`, `Sql`, `Network`, `DBus`) và `libmosquitto`.
- CSDL: SQLite, tạo tự động khi khởi động.
- MQTT broker không được triển khai trong repository; phải được cài/cấu hình độc lập.

## 2. Phần cứng và chu kỳ firmware

| Thiết bị | Kết nối/chân theo mã | Vai trò |
|---|---|---|
| SHT3x | I2C SDA GPIO21, SCL GPIO22; thử địa chỉ `0x44`, sau đó `0x45` | Nhiệt độ, độ ẩm |
| LCD 16x2 I2C | GPIO21/GPIO22, địa chỉ `0x27` | Hiển thị số đo/trạng thái |
| MQ-2 | AO vào GPIO34, ADC 12-bit, attenuation 11 dB | Khí/khói |
| Buzzer | GPIO25 | Báo động âm thanh |
| LED xanh | GPIO26 | Báo trạng thái bình thường |
| LED đỏ | GPIO27 | Báo trạng thái cảnh báo |

Firmware lấy mẫu theo chu kỳ 1 giây. Mỗi lần đọc MQ-2 lấy trung bình 30 mẫu ADC, cách nhau 5 ms; giá trị mV được đọc riêng qua `analogReadMilliVolts`. Firmware hiện luôn đặt `mq2_mode` là `AO`; chân digital `DO` không được sử dụng. Mã không thực hiện quy trình làm nóng hay hiệu chuẩn MQ-2: `mq2_ready` luôn là `true` sau khi đọc hợp lệ.

Ngưỡng mặc định trên ESP32 và desktop là: MQ-2 2500 ADC, nhiệt độ cao 50 °C, độ ẩm cao 90 %, độ ẩm thấp 20 %.

## 3. Xử lý cảnh báo trên ESP32

Trong mỗi chu kỳ, firmware chọn **một** loại cảnh báo theo thứ tự ưu tiên:

1. `MQ2_HIGH` khi MQ-2 hợp lệ, sẵn sàng và ADC trung bình `>=` ngưỡng.
2. `TEMP_HIGH` khi nhiệt độ `>=` ngưỡng.
3. `HUMIDITY_HIGH` khi độ ẩm `>=` ngưỡng.
4. `HUMIDITY_LOW` khi độ ẩm `<=` ngưỡng.

Nếu có cảnh báo, LED đỏ và còi (khi buzzer ở chế độ `AUTO`) bật, LED xanh tắt. Khi tất cả giá trị bình thường liên tục 3 giây, trạng thái trở về bình thường: LED đỏ/còi tắt, LED xanh bật. Cảnh báo cục bộ vẫn hoạt động khi Wi-Fi hoặc MQTT mất kết nối.

Lệnh còi từ MQTT:

- `ON`: ép còi bật.
- `OFF`: tắt tiếng (LED vẫn phản ánh trạng thái cảnh báo).
- `AUTO`: còi phản ánh trạng thái cảnh báo.

## 4. Giao thức MQTT

ESP32 dùng QoS mặc định của `PubSubClient`; ứng dụng Qt publish/subscribe QoS 1. MQTT không TLS trong mã. ESP32 publish trạng thái retained; desktop không publish telemetry retained.

| Topic | Chiều | Payload/chức năng |
|---|---|---|
| `air/sensor/data` | ESP32 → app | JSON telemetry mỗi ~1 giây |
| `air/status` | ESP32 → app | `ONLINE` retained khi kết nối; LWT retained `OFFLINE` |
| `air/alarm` | ESP32 → app | JSON khi trạng thái/loại cảnh báo chuyển đổi |
| `air/config/mq2_ao_threshold` | app → ESP32 | Số nguyên 1–4095 |
| `air/config/temp_high` | app → ESP32 | Số thực -20–100 |
| `air/config/humidity_high` | app → ESP32 | Số thực 0–100 |
| `air/config/humidity_low` | app → ESP32 | Số thực 0–100 |
| `air/command/buzzer` | app → ESP32 | `ON`, `OFF`, hoặc `AUTO` |
| `air/command/status` | app → ESP32 | `GET`; ESP32 trả `ONLINE` và telemetry ngay |

Ví dụ telemetry thực tế:

```json
{
  "device_id": "ESP32_AIR_01",
  "temperature": 29.4,
  "humidity": 67.2,
  "mq2_mode": "AO",
  "mq2_digital": false,
  "mq2_raw": 1234,
  "mq2_mv": 1040,
  "mq2_threshold": 2500,
  "mq2_ready": true,
  "alarm": false,
  "alarm_type": "NONE",
  "uptime_ms": 123456
}
```

Ứng dụng chỉ nhận `air/sensor/data`, `air/status`, `air/alarm`. JSON không hợp lệ trên hai topic JSON sinh lỗi hiển thị/log. Thời gian của bản ghi desktop là lúc ứng dụng nhận MQTT, không phải timestamp do ESP32 gửi (payload không chứa thời gian đo).

## 5. Ứng dụng desktop

Khi mở, ứng dụng tạo khóa đơn instance tại `data/Hethonggiamsat.lock`, khởi tạo log và SQLite, sau đó hiển thị đăng nhập. Đăng xuất quay lại màn hình đăng nhập trong cùng tiến trình.

### Phân quyền

- `ADMIN`: cấu hình ngưỡng/MQTT/còi; thêm, sửa, khóa tài khoản; đổi mật khẩu mọi tài khoản; xuất CSV.
- `USER`: xem Dashboard, lịch sử dữ liệu và lịch sử cảnh báo; có thể đổi **mật khẩu của chính mình** qua service, nhưng UI hiện tại không có màn hình thao tác đổi mật khẩu cho USER.

Lần chạy đầu, nếu chưa có tài khoản, tạo `admin` với mật khẩu từ biến môi trường `HETHONGGIAMSAT_BOOTSTRAP_PASSWORD`; nếu biến rỗng, mật khẩu ngẫu nhiên 16 ký tự. Thông tin ban đầu được ghi vào `data/initial_admin.txt` với quyền owner read/write. Mật khẩu lưu bằng `PBKDF2-HMAC-SHA256`, salt 16 byte, 120000 vòng lặp, khóa 32 byte. Mật khẩu phải có tối thiểu 8 ký tự, có chữ và số.

### Các màn hình

- **Tổng quan**: nhiệt độ, độ ẩm, MQ-2 ADC/mV; 3 đồ thị RAM tối đa 300 điểm/đồ thị; trạng thái ESP32/MQTT; lần nhận dữ liệu cuối. Sau hơn 8 giây không có telemetry, trạng thái ESP32 hiển thị dữ liệu quá hạn.
- **Dữ liệu**: tra cứu khoảng thời gian, tối đa 1000 bản ghi trên bảng. ADMIN xuất tối đa 10000 bản ghi CSV UTF-8.
- **Cảnh báo**: tra cứu 7 ngày gần đây mặc định, tối đa 1000 sự kiện, với thời gian bắt đầu/kết thúc, loại, giá trị, ngưỡng, cực trị và trạng thái `ACTIVE`/`ENDED`.
- **Cài đặt** (ADMIN): lưu ngưỡng, chu kỳ lưu DB, host/port/username MQTT; sau đó khởi động lại MQTT client và publish bốn ngưỡng. Mật khẩu MQTT chỉ ghi lại nếu ô mật khẩu không rỗng.
- **Tài khoản** (ADMIN): thêm, sửa vai trò/họ tên/trạng thái, đổi mật khẩu. ADMIN đang đăng nhập không thể tự khóa hoặc tự hạ quyền.

### Lưu dữ liệu và cảnh báo

Desktop giữ cấu hình trong SQLite; mật khẩu MQTT riêng ở `data/mqtt_credentials.ini`. Mặc định desktop kết nối `127.0.0.1:1883` với username `air_app`. ESP32 có cấu hình mạng/MQTT riêng qua WiFiManager/Preferences và tài khoản mặc định từ `esp32/include/secrets.h`; broker phải cho phép hai client/tài khoản này kết nối.

Telemetry được lưu khi tới chu kỳ `storage_interval_seconds` (mặc định 5 giây) hoặc luôn lưu khi đang cảnh báo. Các trường lưu: thời gian UTC, nhiệt độ, độ ẩm, MQ-2 raw, MQ-2 mV, `NORMAL`/`ALARM`, device ID.

Desktop tin cờ cảnh báo từ ESP32 cho telemetry AO. Nó không áp dụng lại `Mq2Filter` cho luồng này (filter hiện chỉ được kiểm tra bằng `--self-test`). Một cảnh báo được tạo khi nhận trạng thái alarm; được đóng sau 3 telemetry liên tiếp bình thường. Một alarm ngoại vi từ `air/alarm` chỉ được lưu nếu loại không bắt đầu bằng `MQ2`, `TEMP` hay `HUMIDITY`, nhằm tránh nhân đôi alarm đã tạo từ telemetry. Schema có cột `acknowledged` nhưng mã/UI hiện không có chức năng xác nhận cảnh báo.

## 6. SQLite schema

| Bảng | Cột/chức năng |
|---|---|
| `tai_khoan` | `id`, `username` duy nhất, `password_hash`, `ho_ten`, `role` (`ADMIN`/`USER`), `enabled`, thời gian tạo/cập nhật |
| `du_lieu_cam_bien` | `id`, `timestamp`, `temperature`, `humidity`, `mq2_raw`, `mq2_mv`, `status`, `device_id`; có index thời gian |
| `canh_bao` | `id`, `start_time`, `end_time`, `alarm_type`, `value`, `threshold`, `max_value`, `status`, cùng cột acknowledge chưa dùng; có index thời gian |
| `cau_hinh` | key/value và `updated_at` cho cấu hình desktop |

SQLite dùng WAL, `synchronous=NORMAL` và foreign key ON. File DB là `data/hethonggiamsat.sqlite`; thư mục `data` và DB được giới hạn quyền owner ở mức chương trình.

## 7. Mã nguồn và phụ thuộc

```text
include/, src/  lõi ứng dụng Qt: DB, xác thực, MQTT, alarm, repository, CSV
ui/             các cửa sổ/trang Qt Widgets
esp32/          firmware PlatformIO (Arduino)
scripts/        cross-build ARM64, deploy/running qua SSH, Qt Creator helper
data/, logs/    dữ liệu runtime (chỉ .gitkeep được theo dõi)
```

`CMakeLists.txt` yêu cầu CMake 3.18+, compiler C++17, Qt6 Core/Gui/Widgets/Sql/Network/DBus và headers/library Mosquitto. Firmware dùng Adafruit SHT31 2.2.2+, LiquidCrystal_I2C 1.1.4+, PubSubClient 2.8+, ArduinoJson 7.4.2+ và WiFiManager 2.0.17+.

## 8. Build, chạy và kiểm thử

Build native cần Qt6 development, SQLite driver Qt và `libmosquitto-dev`:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
QT_QPA_PLATFORM=offscreen ./build/Hethonggiamsat --self-test
```

Chế độ chạy:

```bash
./Hethonggiamsat                       # GUI bình thường
./Hethonggiamsat --self-test           # DB/auth/config/repository/alarm/MQ-2 filter
./Hethonggiamsat --mqtt-test           # thử kết nối MQTT tối đa 8 giây và gửi GET status
./Hethonggiamsat --integration-test    # hiển thị MainWindow 10 giây
```

`--mqtt-test` và `--integration-test` dùng thư mục gốc từ `HETHONGGIAMSAT_ROOT`, nếu không có là `/home/pi/Duy/Hethonggiamsat`. GUI bình thường cũng dùng đường dẫn này và có thể đổi bằng biến môi trường đó.

Firmware build/upload từ thư mục `esp32`:

```bash
pio run -e esp32dev
pio run -e esp32dev -t upload
```

Trước khi build firmware ngoài CI, tạo `esp32/include/secrets.h` từ `secrets.h.example` và điền thông tin Wi-Fi/MQTT. Không commit file này, DB, credentials runtime hay log.

Các script `scripts/build_arm64.sh`, `deploy_pi.sh`, `build_deploy_run.sh` phục vụ một môi trường triển khai cụ thể (toolchain Qt cross ARM64 và SSH tới Pi); các địa chỉ/đường dẫn mặc định trong script không phải cấu hình chung của sản phẩm.

## 9. Giới hạn cần phản ánh đúng trong báo cáo

- Không có web/mobile app, REST API, cloud backend, nhiều ESP32 theo thiết bị, TLS MQTT, hay migration/version schema.
- Không có biểu đồ lịch sử từ SQLite; đồ thị dashboard chỉ là dữ liệu nhận trong phiên hiện tại.
- Không có acknowledge cảnh báo dù DB có cột liên quan.
- Không có hiệu chuẩn/làm nóng MQ-2 bằng logic firmware; MQ-2 `ready` luôn true khi đọc.
- Firmware lưu cấu hình ngưỡng và network trong ESP32 Preferences; desktop chỉ lưu cấu hình của chính desktop vào SQLite và gửi ngưỡng khi ADMIN bấm lưu.
- Validation desktop yêu cầu `humidity_low < humidity_high`; firmware nhận riêng từng ngưỡng và không kiểm tra quan hệ này khi nhận MQTT.
