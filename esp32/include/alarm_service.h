#pragma once
#include "system_state.h"
class AlarmService{public:void begin();bool update(bool,uint32_t);void setBuzzerCommand(BuzzerMode,uint32_t);AlarmState state()const{return state_;}private:void apply(uint32_t);AlarmState state_=AlarmState::NORMAL;BuzzerMode mode_=BuzzerMode::AUTO;uint32_t clearStarted_=0;};
