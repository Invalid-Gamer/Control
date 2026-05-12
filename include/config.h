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
extern int menuWaitingDelay;
extern uint8_t bt_address[6];

struct NetworkProfile {
    char name[16];
    char ssid[33];
    char password[65];
    char IP[16];
    bool exists;
};

struct NetworkConfig {
    NetworkProfile profiles[3];
    int activeProfiles;
};

bool loadConfig();
void outputConfigToSerial();
bool writeConfig(String key, String value, bool ignoreExistance);
bool writeConfig(String key, int value, bool ignoreExistance);
bool writeConfig(String key, bool value, bool ignoreExistance);
bool deleteConfig(String key, bool secure);
String getSerialInput(bool timeout);
NetworkConfig getWiFiProfiles(bool checkIfExists);
void outputNetworkProfile(int id);
void outputNetworkProfiles();
void networkProfileEditor();

#endif