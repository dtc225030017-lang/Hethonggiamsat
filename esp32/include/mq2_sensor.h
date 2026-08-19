#pragma once
#include <Arduino.h>
struct Mq2Reading {
    uint16_t raw = 0;
    uint16_t millivolts = 0;
    bool detected = false;
    bool valid = false;
    bool ready = false;
};

class Mq2Sensor {
public:
    void begin();
    Mq2Reading read(uint16_t threshold);

private:
};
