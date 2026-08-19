#pragma once
class Sht3xSensor{public:bool begin();bool read(float&,float&);private:bool connected_=false;};
