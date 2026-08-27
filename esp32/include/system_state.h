#pragma once
#include <Arduino.h>
enum class AlarmState:uint8_t{NORMAL,ALARM};
enum class AlarmKind:uint8_t{NONE,MQ2_HIGH,TEMP_HIGH,HUMIDITY_HIGH,HUMIDITY_LOW};
enum class BuzzerMode:uint8_t{AUTO,ON,MUTED};
inline const char* alarmKindName(AlarmKind kind){switch(kind){case AlarmKind::MQ2_HIGH:return "MQ2_HIGH";case AlarmKind::TEMP_HIGH:return "TEMP_HIGH";case AlarmKind::HUMIDITY_HIGH:return "HUMIDITY_HIGH";case AlarmKind::HUMIDITY_LOW:return "HUMIDITY_LOW";default:return "NONE";}}
struct SystemState{float temperature=NAN,humidity=NAN;uint16_t mq2Raw=0,mq2Millivolts=0,mq2Baseline=0,mq2EffectiveThreshold=0;bool mq2Digital=false,mq2Detected=false;uint8_t mq2PinLevel=LOW;bool shtOk=false,mq2Ok=false,mq2Ready=false,wifiOk=false,mqttOk=false;AlarmState alarm=AlarmState::NORMAL;AlarmKind alarmKind=AlarmKind::NONE;};
