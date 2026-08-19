#include "lcd_display.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "config.h"
namespace{LiquidCrystal_I2C lcd(Config::LCD_ADDR,16,2);void line(uint8_t r,const char*s){char b[17];snprintf(b,sizeof(b),"%-16.16s",s);lcd.setCursor(0,r);lcd.print(b);}}
bool LcdDisplay::begin(){Wire.beginTransmission(Config::LCD_ADDR);ok_=Wire.endTransmission()==0;if(ok_){lcd.init();lcd.backlight();line(0,"AIR MONITOR");line(1,"Dang khoi dong");}Serial.println(ok_?"[I2C] LCD 0x27 OK":"[I2C] LCD ERROR");return ok_;}
void LcdDisplay::update(const SystemState&s){if(!ok_)return;char a[17],b[17];if(s.shtOk)snprintf(a,sizeof(a),"T:%.1fC H:%.0f%%",s.temperature,s.humidity);else snprintf(a,sizeof(a),"SHT3X ERROR");if(!s.mq2Ready)snprintf(b,sizeof(b),"MQ2: WARMING");else snprintf(b,sizeof(b),"MQ:%u %s",s.mq2Raw,s.mq2Detected?"ALARM":"OK");line(0,a);line(1,b);}
