# MQTT contract

Broker Pi: `192.168.137.227:1883`, anonymous bị tắt.

| Topic | Hướng | Payload |
|---|---|---|
| `air/sensor/data` | ESP32 → Pi | JSON telemetry theo đặc tả |
| `air/status` | ESP32 → Pi | retained `ONLINE`; LWT `OFFLINE` |
| `air/alarm` | ESP32 → Pi | JSON `{type,value,threshold}` |
| `air/config/mq2_threshold` | Pi → ESP32 | số ADC, retained |
| `air/config/temp_high` | Pi → ESP32 | °C, retained |
| `air/config/humidity_high` | Pi → ESP32 | %, retained |
| `air/config/humidity_low` | Pi → ESP32 | %, retained |
| `air/command/buzzer` | Pi → ESP32 | `ON`, `OFF`, `AUTO` |
| `air/command/status` | Pi → ESP32 | `GET` |

Tài khoản broker của Qt và ESP32 tách biệt theo ACL. Credential sinh khi triển
khai nằm trên Pi trong `data/` với permission 0600, không commit vào source.

