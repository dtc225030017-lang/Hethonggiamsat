# Nhật Ký Thay Đổi (Changelog)

Tất cả các thay đổi đáng chú ý của dự án **Hệ Thống Giám Sát Chất Lượng Không Khí** sẽ được ghi lại trong tài liệu này.

Định dạng dựa trên [Keep a Changelog](https://keepachangelog.com/vi/1.0.0/),
và dự án này tuân thủ [Semantic Versioning](https://semver.org/lang/vi/).

---

## [1.0.0] - 2026-08-19

### Đã Thêm (Added)
- **Qt 6.5.1 C++17 Application**:
  - Giao diện Dashboard thời gian thực với đồng hồ đo nhiệt độ, độ ẩm và nồng độ khí gas MQ-2.
  - Biểu đồ thời gian thực hiển thị dữ liệu lịch sử lên đến 300 mẫu.
  - Hệ thống xác thực người dùng phân quyền đa cấp (`ADMIN`, `USER`) bảo mật bằng **PBKDF2-HMAC-SHA256** (120.000 vòng).
  - Tự động sinh tài khoản `admin` với mật khẩu ngẫu nhiên tại `data/initial_admin.txt` (permission 0600) trong lần khởi chạy đầu.
  - Bộ quản lý cơ sở dữ liệu SQLite3 với transaction và prepared statements.
  - Hệ thống cảnh báo thời gian thực với cơ chế lọc nhiễu rung (hysteresis 50 ADC, 5 mẫu bình thường).
  - Watchdog tự động cảnh báo `DATA STALE` khi mất tín hiệu cảm biến quá 8 giây.
  - Chức năng xuất dữ liệu lịch sử ra định dạng file CSV.
  - Hỗ trợ chạy các chế độ kiểm thử tự động: `--self-test`, `--mqtt-test`, `--integration-test`.
- **ESP32 Firmware (PlatformIO)**:
  - Tích hợp cảm biến độ chính xác cao SHT3x (I2C) đo nhiệt độ, độ ẩm.
  - Tích hợp cảm biến khí gas/khói MQ-2 qua kênh ADC có bộ lọc trung bình động.
  - Màn hình LCD 1602 I2C hiển thị thông số tại trạm đo.
  - Cơ chế tự cảnh báo cục bộ qua còi Buzzer và LED mà không phụ thuộc vào kết nối mạng.
  - Tự động kết nối WiFi và gửi telemetry định kỳ qua MQTT broker Mosquitto dạng JSON.
  - Hỗ trợ cấu hình mạng qua WiFiManager và nạp firmware qua script `UPLOAD_ESP32.bat`.
- **Tài Liệu & DevOps**:
  - Bộ tài liệu kỹ thuật hoàn chỉnh: Kiến trúc hệ thống (`ARCHITECTURE.md`), Sơ đồ cơ sở dữ liệu (`DATABASE_SCHEMA.md`), Hướng dẫn triển khai (`DEPLOYMENT.md`), Danh mục MQTT topics (`MQTT_TOPICS.md`), Báo cáo kiểm thử (`TEST_REPORT.md`), và Sơ đồ phần cứng (`HARDWARE.md`).
  - Bộ công cụ cấu hình cross-compilation ARM64 cho Raspberry Pi 4 và tích hợp Qt Creator Kit.
  - GitHub Actions CI/CD workflows tự động build và kiểm thử mã nguồn.

---
