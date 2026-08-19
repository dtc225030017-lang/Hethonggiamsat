#pragma once
#include <Arduino.h>
namespace NetworkSettings{void begin();void process();const char* mqttHost();uint16_t mqttPort();const char* mqttUsername();const char* mqttPassword();}
