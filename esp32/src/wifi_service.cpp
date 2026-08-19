#include "wifi_service.h"
#include <WiFi.h>
#include "network_settings.h"
void WifiService::begin(){NetworkSettings::begin();}
void WifiService::loop(uint32_t now){(void)now;NetworkSettings::process();static bool was=false;bool on=isConnected();if(on&&!was)Serial.printf("[WIFI] Connected, IP=%s\n",WiFi.localIP().toString().c_str());if(!on&&was)Serial.println("[WIFI] Disconnected; local alarm active");was=on;}
bool WifiService::isConnected()const{return WiFi.status()==WL_CONNECTED;}

