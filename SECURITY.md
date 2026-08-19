# Chính Sách Bảo Mật (Security Policy)

## Phiên Bản Được Hỗ Trợ

Các phiên bản sau đây hiện đang được theo dõi và hỗ trợ cập nhật bản vá bảo mật:

| Phiên Bản | Được Hỗ Trợ |
| --------- | ----------- |
| 1.0.x     | :white_check_mark: |
| < 1.0.0   | :x: |

---

## Báo Cáo Lỗ Hổng Bảo Mật (Reporting a Vulnerability)

Chúng tôi rất coi trọng vấn đề bảo mật của dự án. Nếu bạn phát hiện ra bất kỳ lỗ hổng bảo mật nào trong hệ thống (bao gồm ứng dụng Qt, cơ chế xác thực, MQTT broker, hoặc Firmware ESP32), vui lòng làm theo hướng dẫn sau:

1. **Không mở public issue** trên GitHub để mô tả chi tiết lỗ hổng nhằm tránh rủi ro bị khai thác trước khi có bản vá.
2. Tạo một **Private Vulnerability Report** thông qua mục [Security Advisories](https://github.com/blynkapp04-ui/Hethonggiamsat/security/advisories/new) trên GitHub repository của dự án.
3. Cung cấp các thông tin chi tiết:
   - Mô tả về lỗ hổng và mức độ ảnh hưởng.
   - Các bước cụ thể để tái hiện (Proof of Concept nếu có).
   - Đề xuất khắc phục (nếu có).

---

## Quy Chuẩn Bảo Mật Của Dự Án

- **Mật khẩu người dùng**: Sử dụng thuật toán băm an toàn **PBKDF2-HMAC-SHA256** với 120.000 vòng lặp và muối ngẫu nhiên (salt). Tuyệt đối không lưu mật khẩu dạng plaintext.
- **Phân quyền truy cập**: Phân tách nghiêm ngặt giữa vai trò `ADMIN` và `USER`. Kiểm tra quyền hạn tại tầng backend service logic, không chỉ ẩn hiển thị UI.
- **Truy cập cơ sở dữ liệu**: Dùng Prepared Statements (Parameterized Queries) của Qt SQL để phòng chống SQL Injection.
- **Bảo mật MQTT**: Hỗ trợ xác thực người dùng (username/password), phân vùng topic theo prefix an toàn `air/...`.
- **Bảo mật file nhạy cảm**: File mật khẩu khởi tạo ban đầu `data/initial_admin.txt` được gán quyền nghiêm ngặt `0600` (chỉ chủ sở hữu đọc/ghi) và được xoá ngay sau khi đổi mật khẩu.
