#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
extern Preferences conf;

extern String WiFi_SSID, WiFi_Pass; // Captain WiFi
extern String Target_IP; // Captain Target IP
extern int Target_Port; // Captain Target Port
extern String Device_Name; // WiFi Module Name

bool loadConfig();
void outputConfigToSerial();
bool writeConfig(String key, String value);
bool writeConfig(String key, int value);
bool writeConfig(String key, bool value);

#endif