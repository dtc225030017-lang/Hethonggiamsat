## Mô Tả Thay Đổi (Description)

Vui lòng mô tả tóm tắt những thay đổi được thực hiện trong Pull Request này, lý do thực hiện và bối cảnh liên quan.

Tham chiếu Issue (nếu có): Closes #

---

## Loại Thay Đổi (Type of Change)

- [ ] :sparkles: **Tính năng mới** (New feature)
- [ ] :bug: **Sửa lỗi** (Bug fix)
- [ ] :recycle: **Tái cấu trúc mã nguồn** (Refactoring)
- [ ] :memo: **Tài liệu** (Documentation update)
- [ ] :construction_worker: **CI/CD hoặc Build tool** (CI/CD / Tooling)
- [ ] :zap: **Tối ưu hiệu năng** (Performance improvement)
- [ ] :lock: **Bảo mật** (Security patch)

---

## Kịch Bản Kiểm Thử Đã Thực Hiện (Testing Checklist)

- [ ] Đã chạy kiểm tra định dạng code với `clang-format`.
- [ ] Đã build thành công Qt application trên môi trường phát triển (Host hoặc ARM64).
- [ ] Đã chạy các lệnh kiểm tra tự động (`--self-test`, `--mqtt-test`).
- [ ] Đã build thành công firmware ESP32 bằng `pio run`.
- [ ] Đã kiểm tra không làm rò rỉ thông tin nhạy cảm (mật khẩu, khóa riêng tư, token).

---

## Danh Sách Kiểm Tra Đóng Góp (PR Checklist)

- [ ] Mã nguồn tuân thủ coding convention và kiến trúc dự án.
- [ ] Đã cập nhật tài liệu liên quan (`README.md`, `ARCHITECTURE.md`, `MQTT_TOPICS.md`, etc.) nếu có thay đổi logic hoặc API.
- [ ] Thông điệp commit tuân theo quy chuẩn Conventional Commits.
