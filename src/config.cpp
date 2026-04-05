#include <Arduino.h>
#include <Preferences.h>
#include <config.h>
#include <global.h>

bool loadConfig() { // Config über Preferences laden mit debug, true = Erfolg, weitermachen; false = Fehler, Programm stoppen, debug
    conf.begin("config", true);
    WiFi_SSID = conf.getString("wifi_ssid", "");
    WiFi_Pass = conf.getString("wifi_pass", "");
    Target_IP = conf.getString("target_ip", "");
    Target_Port = conf.getInt("target_port", 0);
    Device_Name = conf.getString("device_name","");
    conf.end();
    if (WiFi_SSID == "" || WiFi_Pass == "") {
        // Debug, Warnen dass WiFi nicht vorhanden, Programm stoppen, auf Display anzeigen.
        return false;
    } else if (Target_IP == "" || Target_Port == 0) {
        // Debug, Warnen dass kein Target vorhanden, Programm stoppen, auf Display anzeigen.
        return false;
    } else if (Device_Name == "") {
        // Auf Display warnen, fortfahren mit Standard-Namen
        Device_Name = "Natasha Control";
        return true;
    } else {
        return true;
    }
}

void outputConfigToSerial() { // Alle Config Einträge ausgeben an die serielle Konsole
    conf.begin("config", true);
    Serial.println("wifi_ssid: " + conf.getString("wifi_ssid", ""));
    Serial.println("wifi_pass: " + conf.getString("wifi_pass", ""));
    Serial.println("target_ip: " + conf.getString("target_ip", ""));
    Serial.println("target_port: " + conf.getInt("target_port", 0));
    Serial.println("device_name: " + conf.getString("device_name", ""));
    conf.end();
}

bool writeConfig(String key, String value) { // Prüfen ob key existiert, ob value String ist, dann in die Config schreiben.
    conf.begin("config", false);
    if(conf.isKey(key.c_str())) {
        if(conf.getType(key.c_str())==8) {
            conf.putString(key.c_str(),value);
            return true;
        } else {
            return false;
        }
    } else{
        return false;
    }
    conf.end();
    outputConfigToSerial();
}

bool writeConfig(String key, int value) { // Prüfen ob key existiert, ob value int ist, dann in die Config schreiben.
    conf.begin("config", false);
    if(conf.isKey(key.c_str())) {
        if(conf.getType(key.c_str())==4) {
            conf.putInt(key.c_str(),value);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
    conf.end();
    outputConfigToSerial();
}
