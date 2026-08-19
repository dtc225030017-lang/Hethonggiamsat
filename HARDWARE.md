# Tài Liệu Phần Cứng (Hardware Specification & Pinout)

Tài liệu này mô tả chi tiết phần cứng, sơ đồ chân kết nối (Pinout) và nguyên lý hoạt động của trạm đo cảm biến ESP32 và máy chủ xử lý Raspberry Pi 4.

---

## 1. Danh Sách Linh Kiện (Bill of Materials - BOM)

| STT | Tên Linh Kiện | Số Lượng | Giao Tiếp / Thông Số | Chức Năng |
| :-: | :--- | :-: | :--- | :--- |
| 1 | **ESP32 NodeMCU (30-pin/38-pin)** | 1 | WiFi 2.4GHz, Bluetooth, ADC, I2C | Vi điều khiển thu thập và gửi dữ liệu |
| 2 | **Cảm biến SHT30 / SHT31 / SHT35** | 1 | I2C (Địa chỉ mặc định: `0x44` hoặc `0x45`) | Đo nhiệt độ (-40°C ~ 125°C) và độ ẩm (0% ~ 100% RH) |
| 3 | **Cảm biến khí gas & khói MQ-2** | 1 | Analog (ADC 12-bit) / Digital Output | Phát hiện LPG, i-butan, propan, mê-tan, cồn, khói |
| 4 | **Màn hình LCD 1602 kèm module I2C PCF8574** | 1 | I2C (Địa chỉ mặc định: `0x27` hoặc `0x3F`) | Hiển thị thông số thời gian thực tại trạm đo |
| 5 | **Còi chip chủ động (Active Buzzer 5V/3.3V)** | 1 | GPIO Digital Output (hoặc qua Transistor NPN 2N2222) | Báo động âm thanh khi nồng độ khí vượt ngưỡng |
| 6 | **Đèn LED cảnh báo (Đỏ / Xanh)** | 2 | GPIO Digital Output qua trở 220Ω - 330Ω | Chỉ thị trạng thái bình thường (Xanh) / Cảnh báo (Đỏ) |
| 7 | **Nguồn cấp 5V - 2A** | 1 | Micro-USB hoặc chân 5V/VIN | Cung cấp nguồn ổn định cho hệ thống cảm biến |
| 8 | **Raspberry Pi 4 Model B (4GB / 8GB)** | 1 | Ethernet / WiFi, HDMI, USB, ARM64 | Máy chủ chạy MQTT Broker Mosquitto, App Qt6 & SQLite |

---

## 2. Sơ Đồ Đấu Nối Chân ESP32 (Pinout Mapping)

```text
               +-----------------------------+
               |         ESP32 DevKit        |
               |                             |
  SHT3x (SDA) -| GPIO 21 (I2C SDA)           |
  SHT3x (SCL) -| GPIO 22 (I2C SCL)           |
  LCD1602 SDA -| GPIO 21 (I2C SDA)           |
  LCD1602 SCL -| GPIO 22 (I2C SCL)           |
    MQ-2 (AO) -| GPIO 34 (ADC1_CH6)          |
    Buzzer (+) -| GPIO 25 (Digital OUT)       |
     LED Red  -| GPIO 26 (Digital OUT)       |
   LED Green  -| GPIO 27 (Digital OUT)       |
          GND -| GND                         |
        5V/VIN-| 5V VCC                      |
               +-----------------------------+
```

### Bảng Chi Tiết Kết Nối:

| Chân Linh Kiện | Chân ESP32 Kết Nối | Ghi Chú |
| :--- | :--- | :--- |
| **SHT3x VCC** | 3.3V hoặc 5V | Khuyến nghị 3.3V |
| **SHT3x GND** | GND | Nối mass chung |
| **SHT3x SDA** | **GPIO 21** | Đường dữ liệu I2C chung |
| **SHT3x SCL** | **GPIO 22** | Đường xung nhịp I2C chung |
| **LCD 1602 VCC** | 5V (VIN) | Module LCD 1602 hoạt động tốt nhất ở 5V |
| **LCD 1602 GND** | GND | Nối mass chung |
| **LCD 1602 SDA** | **GPIO 21** | Đường dữ liệu I2C chung |
| **LCD 1602 SCL** | **GPIO 22** | Đường xung nhịp I2C chung |
| **MQ-2 VCC** | 5V (VIN) | Cảm biến gia nhiệt cần nguồn 5V ổn định |
| **MQ-2 GND** | GND | Nối mass chung |
| **MQ-2 AO (Analog Out)** | **GPIO 34** | Kênh ADC1 (không bị xung đột với WiFi) |
| **Active Buzzer (+)** | **GPIO 25** | Kích HIGH để phát chuông |
| **Buzzer (-)** | GND | Nối mass |
| **LED Đỏ (Cảnh báo)** | **GPIO 26** | Nối tiếp qua điện trở 220Ω tới GND |
| **LED Xanh (Hoạt động)** | **GPIO 27** | Nối tiếp qua điện trở 220Ω tới GND |

---

## 3. Lưu Ý Kỹ Thuật Khi Triển Khai

1. **Khởi động gia nhiệt cảm biến MQ-2 (Preheating)**:
   - Cảm biến MQ-2 sử dụng dây tóc nung nóng để phản ứng với khí gas.
   - Cần tối thiểu 2-5 phút hoạt động liên tục sau khi cấp nguồn để số đo ổn định và chính xác.
2. **Kênh ADC trên ESP32**:
   - Sử dụng chân **GPIO 34** thuộc khối **ADC1**. Không sử dụng các chân thuộc khối ADC2 (GPIO 0, 2, 4, 12-15, 25-27 cho ADC) để đọc analog khi WiFi đang hoạt động vì ADC2 bị WiFi driver chiếm dụng.
3. **Nguồn điện cấp**:
   - Cảm biến MQ-2 tiêu thụ dòng tức thời lên tới 150-200mA cho sợi đốt. Nên sử dụng củ sạc nguồn 5V chất lượng cao có dòng từ 1.5A - 2A trở lên để tránh hiện tượng sụt áp gây reset ESP32 (Brownout detector).
4. **Cơ chế Cảnh Báo Độc Lập**:
   - ESP32 được lập trình để tự kích hoạt còi và LED ngay tại chỗ khi nồng độ khí gas hoặc nhiệt độ vượt ngưỡng an toàn cục bộ, bảo đảm phản ứng tức thì ngay cả khi mất kết nối WiFi/MQTT tới máy chủ Raspberry Pi.
