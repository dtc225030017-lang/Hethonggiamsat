#include <Arduino.h>
#include <Wire.h>
#include "alarm_service.h"
#include "config.h"
#include "lcd_display.h"
#include "mq2_sensor.h"
#include "mqtt_service.h"
#include "sht3x_sensor.h"
#include "wifi_service.h"
namespace{Sht3xSensor sht;Mq2Sensor mq2;LcdDisplay lcd;AlarmService alarmService;WifiService wifi;MqttService mqttService;SystemState state;uint32_t sensorAt=0,lcdAt=0;void scan(){for(uint8_t a=1;a<127;a++){Wire.beginTransmission(a);if(Wire.endTransmission()==0)Serial.printf("[I2C] Found 0x%02X\n",a);}}void sample(uint32_t now){float t=NAN,h=NAN;state.shtOk=sht.read(t,h);if(state.shtOk){state.temperature=t;state.humidity=h;}const uint16_t threshold=mqttService.mq2Threshold();auto r=mq2.read(threshold);state.mq2Digital=false;state.mq2Detected=r.detected;state.mq2Raw=r.raw;state.mq2Millivolts=r.millivolts;state.mq2Baseline=0;state.mq2EffectiveThreshold=threshold;state.mq2Ok=r.valid;state.mq2Ready=r.ready;bool changed=false;if(r.ready&&r.valid)changed=alarmService.update(r.detected,now);state.alarm=alarmService.state();if(changed)mqttService.publishAlarmTransition(state.alarm==AlarmState::ALARM,r.raw,threshold);Serial.printf("[SENSOR] T=%.1f H=%.1f MQ2_RAW=%u MQ2_MV=%u threshold=%u status=%s\n",state.temperature,state.humidity,r.raw,r.millivolts,threshold,r.detected?"DETECTED":"NORMAL");}}
void setup(){Serial.begin(115200);delay(1000);Serial.println("[SYS] Boot");Wire.begin(Config::SDA_PIN,Config::SCL_PIN);scan();lcd.begin();sht.begin();mq2.begin();alarmService.begin();wifi.begin();mqttService.begin(&state,&alarmService);}
void loop(){uint32_t now=millis();wifi.loop(now);state.wifiOk=wifi.isConnected();mqttService.loop(now);if(now-sensorAt>=Config::SENSOR_MS){sensorAt=now;sample(now);}if(now-lcdAt>=Config::LCD_MS){lcdAt=now;lcd.update(state);}yield();}
