#include "alarm_service.h"
#include "config.h"
void AlarmService::begin(){pinMode(Config::GREEN_PIN,OUTPUT);pinMode(Config::RED_PIN,OUTPUT);pinMode(Config::BUZZER_PIN,OUTPUT);apply(millis());}
bool AlarmService::update(bool detected,uint32_t now){AlarmState before=state_;if(detected){clearStarted_=0;if(state_==AlarmState::NORMAL){state_=AlarmState::ALARM;Serial.println("[ALARM] MQ2 DO DETECTED - buzzer/red ON");}}else if(state_==AlarmState::ALARM){if(clearStarted_==0)clearStarted_=now;if(now-clearStarted_>=Config::ALARM_CLEAR_DELAY_MS){state_=AlarmState::NORMAL;clearStarted_=0;Serial.println("[ALARM] MQ2 normal for 3s - buzzer/red OFF, green ON");}}else clearStarted_=0;apply(now);return before!=state_;}
void AlarmService::setBuzzerCommand(BuzzerMode m,uint32_t now){mode_=m;apply(now);}
void AlarmService::apply(uint32_t now){(void)now;bool active=state_==AlarmState::ALARM;bool buzz=mode_==BuzzerMode::ON||(active&&mode_==BuzzerMode::AUTO);digitalWrite(Config::GREEN_PIN,active?LOW:HIGH);digitalWrite(Config::RED_PIN,active?HIGH:LOW);digitalWrite(Config::BUZZER_PIN,buzz?HIGH:LOW);}
