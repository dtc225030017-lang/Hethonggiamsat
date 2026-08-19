# Hướng Dẫn Đóng Góp (Contributing Guidelines)

Cảm ơn bạn đã quan tâm và muốn đóng góp cho dự án **Hệ Thống Giám Sát Chất Lượng Không Khí**!

Dưới đây là các quy chuẩn kỹ thuật và quy trình đóng góp giúp mã nguồn luôn đảm bảo chất lượng, tính nhất quán và dễ bảo trì.

---

## 1. Quy Trình Làm Việc (Git Workflow)

1. **Fork** repository về tài khoản cá nhân của bạn trên GitHub.
2. **Clone** repository về máy phát triển:
   ```bash
   git clone https://github.com/blynkapp04-ui/Hethonggiamsat.git
   cd Hethonggiamsat
   ```
3. Tạo một **branch mới** từ `main` theo quy ước đặt tên:
   - `feature/ten-tinh-nang`: Thêm tính năng mới (ví dụ: `feature/export-pdf`, `feature/lora-telemetry`).
   - `fix/ten-loi`: Sửa lỗi (ví dụ: `fix/mqtt-reconnect-delay`, `fix/chart-memory-leak`).
   - `docs/ten-tai-lieu`: Cập nhật tài liệu kỹ thuật.
   - `ci/ten-workflow`: Cải tiến CI/CD.
4. Thực hiện các thay đổi, commit theo chuẩn Conventional Commits (xem mục 2).
5. Đảm bảo chạy kiểm tra định dạng và build test thành công trước khi push.
6. Push branch lên fork và tạo **Pull Request (PR)** vào branch `main` của repository gốc.

---

## 2. Quy Chuẩn Commit (Conventional Commits)

Thông điệp commit cần tuân thủ cấu trúc sau:

```text
<type>(<scope>): <mô tả ngắn gọn bằng tiếng Việt hoặc tiếng Anh>

[Mô tả chi tiết tùy chọn]

[Tham chiếu Issue, ví dụ: Closes #12]
```

### Các `type` hợp lệ:
- `feat`: Tính năng mới cho người dùng hoặc hệ thống.
- `fix`: Sửa lỗi trong mã nguồn.
- `docs`: Thêm hoặc chỉnh sửa tài liệu (`README`, `ARCHITECTURE.md`, v.v.).
- `style`: Định dạng mã nguồn (dấu cách, căn lề, clang-format, không ảnh hưởng logic).
- `refactor`: Tái cấu trúc mã nguồn không thay đổi hành vi bên ngoài.
- `perf`: Cải thiện hiệu năng xử lý.
- `test`: Thêm hoặc cập nhật unit test, integration test.
- `ci`: Thay đổi cấu hình CI/CD (`.github/workflows/`).
- `chore`: Cập nhật cấu hình build, dependencies, tooling.

### Ví dụ:
```text
feat(alarm): thêm cơ chế lọc rung ngưỡng khí gas MQ-2
fix(mqtt): xử lý timeout khi broker Mosquitto khởi động lại
docs(api): cập nhật danh sách MQTT topics và JSON schema
```

---

## 3. Quy Chuẩn Mã Nguồn (Coding Standards)

- **Ngôn ngữ**: C++17 cho ứng dụng Qt và ESP32 Firmware.
- **Định dạng code**: Tuân thủ `.clang-format` và `.editorconfig`.
  - Tự động định dạng bằng `clang-format`:
    ```bash
    clang-format -i src/*.cpp include/*.h ui/*.cpp ui/*.h esp32/src/*.cpp esp32/include/*.h
    ```
- **Quy ước đặt tên**:
  - Tên file: `snake_case.cpp`, `snake_case.h`.
  - Tên class: `PascalCase` (ví dụ: `DatabaseManager`, `MqttService`, `DashboardPage`).
  - Tên hàm & biến: `camelCase` (ví dụ: `publishTelemetry()`, `isConnected`).
  - Biến thành viên (member variables): `m_variableName` hoặc `m_camelCase`.
  - Hằng số & Macro: `UPPER_SNAKE_CASE` (ví dụ: `MAX_CHART_SAMPLES`, `MQTT_DEFAULT_PORT`).
- **An toàn bộ nhớ & Tài nguyên**:
  - Ưu tiên sử dụng Smart Pointers (`std::unique_ptr`, `std::shared_ptr`) hoặc cây quan hệ cha-con của Qt (`QObject` hierarchy).
  - Không hardcode mật khẩu, token hoặc thông tin nhạy cảm vào mã nguồn.

---

## 4. Kiểm Thử & Xác Nhận (Verification)

Mọi đóng góp cần vượt qua các bài kiểm tra trước khi được chấp thuận:

1. **Build trên Host / Cross-build**:
   ```bash
   # Kiểm tra cú pháp và build host
   mkdir -p build && cd build
   cmake ..
   cmake --build .
   ```
2. **Self-test Qt Application**:
   ```bash
   ./Hethonggiamsat --self-test
   ./Hethonggiamsat --mqtt-test
   ```
3. **Build ESP32 Firmware**:
   ```bash
   cd esp32
   pio run
   ```

---

## 5. Quy Trình Duyệt Pull Request (PR Review)

- Điền đầy đủ thông tin vào mẫu [Pull Request Template](.github/PULL_REQUEST_TEMPLATE.md).
- Tất cả các bước kiểm tra tự động trên GitHub Actions (CI) phải có trạng thái **Passed** (`green`).
- PR sẽ được review bởi maintainer dự án trước khi thực hiện merge.
