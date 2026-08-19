#pragma once
#include "system_state.h"
class LcdDisplay{public:bool begin();void update(const SystemState&);private:bool ok_=false;};
