#include <Arduino.h>
#include <Preferences.h>
#include <config.h>

// Get Config Values from Preferences Save.
void loadConfig() {
    conf.begin("config", true);
    WiFi_SSID = conf.getString("wifi_ssid", "");
    WiFi_Pass = conf.getString("wifi_pass", "");
    Target_IP = conf.getString("Target_IP", "");
    Target_Port = conf.getInt("Target_Port", 0);
    Device_Name = conf.getString("Device_Name","");
    conf.end();
    if (WiFi_SSID == "" || WiFi_Pass == "") {
        // Debug, Warnen dass WiFi nicht vorhanden, Programm stoppen, auf Display anzeigen.
    }
    if (Target_IP == "" || Target_Port == 0) {
        // Debug, Warnen dass kein Target vorhanden, Programm stoppen, auf Display anzeigen.
    }
    if (Device_Name == "") {
        // Auf Display warnen, fortfahren mit Standard-Namen
        Device_Name = "Natasha Control";
    }
}