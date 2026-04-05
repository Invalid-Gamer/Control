#include <Arduino.h>
#include <Preferences.h>
#include <config.h>
#include <global.h>

Preferences conf;
String WiFi_SSID = "";
String WiFi_Pass = "";
String Target_IP = "";
int Target_Port = 0;
String Device_Name = "";
bool advancedLog = false;

void outputConfigToSerial() { // Alle Config Einträge ausgeben an die serielle Konsole
    conf.begin("config", true);
    Serial.println("wifi_ssid: " + conf.getString("wifi_ssid", ""));
    Serial.println("wifi_pass: " + conf.getString("wifi_pass", ""));
    Serial.println("target_ip: " + conf.getString("target_ip", ""));
    Serial.println("target_port: " + String(conf.getInt("target_port", 0)));
    Serial.println("device_name: " + conf.getString("device_name", ""));
    Serial.println("advancedLog: " + String(conf.getBool("advancedLog", false)));
    conf.end();
}

bool loadConfig() { // Config über Preferences laden mit debug, true = Erfolg, weitermachen; false = Fehler, Programm stoppen, debug
    if(advancedLog){Serial.println("Started config load");}
    conf.begin("config", true);
    WiFi_SSID = conf.getString("wifi_ssid", "");
    WiFi_Pass = conf.getString("wifi_pass", "");
    Target_IP = conf.getString("target_ip", "");
    Target_Port = conf.getInt("target_port", 0);
    Device_Name = conf.getString("device_name", "");
    advancedLog = conf.getBool("advancedLog", false);
    conf.end();
    if(advancedLog){Serial.println("Loaded config. Current config: "); outputConfigToSerial();}
    if (WiFi_SSID == "" || WiFi_Pass == "") {
        // Debug, Warnen dass WiFi nicht vorhanden, Programm stoppen, auf Display anzeigen.
        if(advancedLog){Serial.println("WiFi SSID/Pass nicht vorhanden!");}
        return false;
    } else if (Target_IP == "" || Target_Port == 0) {
        // Debug, Warnen dass kein Target vorhanden, Programm stoppen, auf Display anzeigen.
        if(advancedLog){Serial.println("Target IP/Port nicht vorhanden!");}
        return false;
    } else if (Device_Name == "") {
        // Auf Display warnen, fortfahren mit Standard-Namen
        if(advancedLog){Serial.println("Kein Device Name vorhanden! (Oder leer)");}
        Device_Name = "Natasha Control";
        return true;
    } else {
        return true;
    }
}

bool writeConfig(String key, String value) { // Prüfen ob key existiert, ob value String ist, dann in die Config schreiben.
    conf.begin("config", false);
    if(conf.isKey(key.c_str())) {
        if(conf.getType(key.c_str())==8) {
            if(advancedLog){Serial.println("Changing config at: \nkey: " + key + "; value: "+ value);}
            conf.putString(key.c_str(),value);
            return true;
        } else {
            if(advancedLog){Serial.println("Failed changing config: Provided value isn't String");}
            return false;
        }
    } else{
        if(advancedLog){Serial.println("Failed changing config: Provided key does not exist!");}
        return false;
    }
    conf.end();
    outputConfigToSerial();
}

bool writeConfig(String key, int value) { // Prüfen ob key existiert, ob value int ist, dann in die Config schreiben.
    conf.begin("config", false);
    if(conf.isKey(key.c_str())) {
        if(conf.getType(key.c_str())==4) {
            if(advancedLog){Serial.println("Changing config at: \nkey: " + key + "; value: "+ String(value));}
            conf.putInt(key.c_str(),value);
            return true;
        } else {
            if(advancedLog){Serial.println("Failed changing config: Provided value isn't int");}
            return false;
        }
    } else {
        if(advancedLog){Serial.println("Failed changing config: Provided key does not exist!");}
        return false;
    }
    conf.end();
    outputConfigToSerial();
}

bool writeConfig(String key, bool value) {
    conf.begin("config", false);
    if(conf.isKey(key.c_str())) {
        if(conf.getType(key.c_str())==1) {
            if(advancedLog){Serial.println("Changing config at: \nkey: " + key + "; value: "+ String(value));}
            conf.putBool(key.c_str(), value);
            return true;
        } else {
            if(advancedLog){Serial.println("Failed changing config: Provided value isn't bool");}
            return false;
        }
    } else {
        if(advancedLog){Serial.println("Failed changing config: Provided key does not exist!");}
        return false;
    }
    conf.end();
    outputConfigToSerial();
}