#pragma once
#include <Arduino.h>
namespace Config {
constexpr char DEVICE_ID[]="ESP32_AIR_01";
constexpr uint8_t SDA_PIN=21,SCL_PIN=22,LCD_ADDR=0x27,SHT_ADDR=0x44,MQ2_PIN=34,GREEN_PIN=26,RED_PIN=27,BUZZER_PIN=25;
// MQ-2 AO: use the proven 30-sample average and 2500 ADC threshold.
constexpr uint8_t MQ2_SAMPLES=30;
constexpr uint16_t MQ2_SAMPLE_DELAY_MS=5, MQ2_DEFAULT_THRESHOLD=2500;
constexpr uint32_t SENSOR_MS=1000, LCD_MS=1000, TELEMETRY_MS=1000;
constexpr uint32_t ALARM_CLEAR_DELAY_MS=3000, MQTT_RETRY_MS=5000;
constexpr float TEMP_HIGH_DEFAULT=50.0F,HUM_HIGH_DEFAULT=90.0F,HUM_LOW_DEFAULT=20.0F;
}
