#pragma once
#include "system_state.h"
class AlarmService;
class MqttService{public:void begin(SystemState*,AlarmService*);void loop(uint32_t);void publishTelemetry(uint32_t);void publishAlarmTransition(bool,AlarmKind,float,float);void handleMessage(char*,uint8_t*,unsigned int);uint16_t mq2Threshold()const{return threshold_;}float temperatureHigh()const{return tempHigh_;}float humidityHigh()const{return humHigh_;}float humidityLow()const{return humLow_;}private:void connect(uint32_t);void subscribe();SystemState*state_=nullptr;AlarmService*alarm_=nullptr;uint32_t lastAttempt_=0,lastTelemetry_=0;uint16_t threshold_=2500;float tempHigh_=50,humHigh_=90,humLow_=20;};
