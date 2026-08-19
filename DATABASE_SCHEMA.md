# Database schema

File runtime: `/home/pi/Duy/Hethonggiamsat/data/hethonggiamsat.sqlite`, mode
0600, WAL + `synchronous=NORMAL` + foreign keys.

- `tai_khoan`: id, username unique, password_hash, ho_ten, role ADMIN/USER,
  enabled, created_at, updated_at.
- `du_lieu_cam_bien`: id, timestamp, temperature, humidity, mq2_raw, mq2_mv,
  status, device_id; index timestamp.
- `canh_bao`: start/end, type, value, threshold, max_value, ACTIVE/ENDED,
  acknowledged, acknowledged_by; index start_time.
- `cau_hinh`: key/value/updated_at; không chứa password đăng nhập.

SQL nhận input đều dùng prepared statement. Telemetry mặc định chỉ lưu mỗi 5
giây; alarm được lưu ngay và cập nhật max thay vì insert mỗi giây.

