#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
extern Preferences conf;

extern String WiFi_SSID, WiFi_Pass; // Captain WiFi
extern String Target_IP; // Captain Target IP
extern int udp_Target_Port; // Captain UDP Target Port
extern int tcp_Target_Port; // Captain TCP Target Port
extern String Device_Name; // WiFi Module Name

bool loadConfig();
void outputConfigToSerial();
bool writeConfig(String key, String value, bool ignoreExistance);
bool writeConfig(String key, int value, bool ignoreExistance);
bool writeConfig(String key, bool value, bool ignoreExistance);
bool deleteConfig(String key, bool secure);
String getSerialInput(bool timeout);

#endif