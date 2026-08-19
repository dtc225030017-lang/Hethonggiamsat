#include "sht3x_sensor.h"
#include <Adafruit_SHT31.h>
#include "config.h"
namespace{Adafruit_SHT31 sensor;}
bool Sht3xSensor::begin(){connected_=sensor.begin(Config::SHT_ADDR)||sensor.begin(0x45);Serial.println(connected_?"[I2C] SHT3X OK":"[I2C] SHT3X ERROR");return connected_;}
bool Sht3xSensor::read(float&t,float&h){if(!connected_&&!begin())return false;t=sensor.readTemperature();h=sensor.readHumidity();if(!isfinite(t)||!isfinite(h)){connected_=false;Serial.println("[SENSOR] SHT3X read error");return false;}return true;}

