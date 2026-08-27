#pragma once
#include "system_state.h"
class AlarmService{public:void begin();bool update(AlarmKind,uint32_t);void setBuzzerCommand(BuzzerMode,uint32_t);AlarmState state()const{return state_;}AlarmKind kind()const{return kind_;}private:void apply(uint32_t);AlarmState state_=AlarmState::NORMAL;AlarmKind kind_=AlarmKind::NONE;BuzzerMode mode_=BuzzerMode::AUTO;uint32_t clearStarted_=0;};
