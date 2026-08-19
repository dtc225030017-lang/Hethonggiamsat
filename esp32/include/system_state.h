#pragma once
#include <Arduino.h>
enum class AlarmState:uint8_t{NORMAL,ALARM};
enum class BuzzerMode:uint8_t{AUTO,ON,MUTED};
struct SystemState{float temperature=NAN,humidity=NAN;uint16_t mq2Raw=0,mq2Millivolts=0,mq2Baseline=0,mq2EffectiveThreshold=0;bool mq2Digital=false,mq2Detected=false;uint8_t mq2PinLevel=LOW;bool shtOk=false,mq2Ok=false,mq2Ready=false,wifiOk=false,mqttOk=false;AlarmState alarm=AlarmState::NORMAL;};
