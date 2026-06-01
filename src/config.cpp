#include <Arduino.h>
#include <Preferences.h>
#include <config.h>
#include <global.h>
#include <joystick.h>
#include <shell.h>

Preferences conf;
int udp_Target_Port = 0;
int tcp_Target_Port = 0;
String Device_Name = "";
bool advancedLog = false;
int menuWaitingDelay = 0;
int inactivityTimeout = 0;
uint8_t bt_address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void outputConfigToSerial() { // Alle Config Einträge ausgeben an die serielle Konsole
    conf.begin("config", true);
    Serial.println("ntwk_cfg");
    Serial.println("udp_target_port: " + String(conf.getInt("udp_target_port", 0)));
    Serial.println("tcp_target_port: " + String(conf.getInt("tcp_target_port", 0)));
    Serial.println("device_name: " + conf.getString("device_name", ""));
    Serial.println("advancedLog: " + String(conf.getBool("advancedLog", false)));
    Serial.println("menu_delay: " + String(conf.getInt("menu_delay", 0)));
    Serial.println("inac_timeout: " + String(conf.getInt("inac_timeout", 0)));
    conf.end();
}

bool loadConfig() { // Config über Preferences laden mit debug, true = Erfolg, weitermachen; false = Fehler, Programm stoppen, debug
    logging.debug("Started config load");
    conf.begin("config", true);
    udp_Target_Port = conf.getInt("udp_target_port", 0);
    tcp_Target_Port = conf.getInt("tcp_target_port", 0);
    Device_Name = conf.getString("device_name", "");
    advancedLog = conf.getBool("advancedLog", false);
    menuWaitingDelay = conf.getInt("menu_delay", 0);
    inactivityTimeout = conf.getInt("inac_timeout",0);
    conf.end();
    logging.debug("Loaded config. Current config: ");
    if(advancedLog){outputConfigToSerial();}
    if (udp_Target_Port == 0 || tcp_Target_Port == 0) {
        // Debug, Warnen dass kein Target vorhanden, Programm stoppen, auf Display anzeigen.
        logging.error("Target Port nicht vorhanden!");
        return false;
    } else if (Device_Name == "" || menuWaitingDelay == 0 || inactivityTimeout == 0) {
        // Auf Display warnen, fortfahren mit Standard-Namen
        if(Device_Name == "") {
            Device_Name = "Natasha Control";
            logging.error("Kein Device Name vorhanden! (Oder leer) Benutze Standard (Natasha Control)");
        }
        if(menuWaitingDelay == 0) {
            menuWaitingDelay = 25;
            logging.error("Kein Menu Waiting Delay Wert festgelegt! Benutze Standard von 25...");
        } 
        if(inactivityTimeout == 0) {
            inactivityTimeout = 10000;
            logging.error("Kein Inactivity Timeout Wert festgelegt! Benutze Standard von 10000...");
        }
        return true;
    } else {
        return true;
    }
}

bool writeConfig(String key, String value, bool ignoreExistance) { // Prüfen ob key existiert, ob value String ist, dann in die Config schreiben.
    bool success;
    conf.begin("config", false);
    if(conf.isKey(key.c_str()) || ignoreExistance) {
        if(conf.getType(key.c_str())==8 || ignoreExistance) {
            logging.info("Changing config at: \nkey: " + key + "; value: "+ value);
            conf.putString(key.c_str(),value);
            success = true;
        } else {
            logging.error("Failed changing config: Provided value isn't String");
            success = false;
        }
    } else{
        logging.error("Failed changing config: Provided key does not exist!");
        success = false;
    }
    conf.end();
    outputConfigToSerial();
    return success;
}

bool writeConfig(String key, int value, bool ignoreExistance) { // Prüfen ob key existiert, ob value int ist, dann in die Config schreiben.
    bool success;
    conf.begin("config", false);
    if(conf.isKey(key.c_str()) || ignoreExistance) {
        if(conf.getType(key.c_str())==4 || ignoreExistance) {
            logging.info("Changing config at: \nkey: " + key + "; value: "+ String(value));
            conf.putInt(key.c_str(),value);
            success = true;
        } else {
            logging.error("Failed changing config: Provided value isn't int");
            success = false;
        }
    } else {
        logging.error("Failed changing config: Provided key does not exist!");
        success = false;
    }
    conf.end();
    outputConfigToSerial();
    return success;
}

bool writeConfig(String key, bool value, bool ignoreExistance) {
    bool success;
    conf.begin("config", false);
    if(conf.isKey(key.c_str()) || ignoreExistance) {
        if(conf.getType(key.c_str())==1 || ignoreExistance) {
            logging.info("Changing config at: \nkey: " + key + "; value: "+ String(value));
            conf.putBool(key.c_str(), value);
            success = true;
        } else {
            logging.error("Failed changing config: Provided value isn't bool");
            success = false;
        }
    } else {
        logging.error("Failed changing config: Provided key does not exist!");
        success = false;
    }
    conf.end();
    outputConfigToSerial();
    return success;
}

bool deleteConfig(String key, bool secure) {
    bool success;
    conf.begin("config", false);
    Serial.println("FINAL CONFIRMATION: WILL BREAK CODE; DONT DO IF NOT DEV (y/n): ");
    String final_confirmation = getSerialInput(true);
    if (final_confirmation == "y") {
        conf.remove(key.c_str());
        success = true;
    } else {
        Serial.println("Aborting...");
        success = false;
    }
    conf.end();
    return success;
}

NetworkConfig getWiFiProfiles(bool checkIfExists) {
    NetworkConfig config;
    conf.begin("config", true);
    if(!conf.isKey("ntwk_cfg") && checkIfExists) {
        logging.error("No Network config found! Please configure in WiFi Shell...");
        currentShellMode = S_WIFI;
        shell();
    }
    if(!conf.isKey("ntwk_cfg") && checkIfExists) {
        logging.error("No Network configured, still cant find networkConfig! Cant resume! Entering shell!");
    } else {
        size_t readBytes = conf.getBytes("ntwk_cfg", &config, sizeof(NetworkConfig));
        conf.end();
        if(readBytes == 0) {
            for(int i = 0; i < 3; i++) {
                config.profiles[i].exists = false;
            }
            config.activeProfiles = 0;
        } else {
            int profileCounter = 0;
            for (int i = 0; i < 3; i++) {
                if (config.profiles[i].exists) {
                    profileCounter += 1;
                }
            }
            config.activeProfiles = profileCounter;
        }
    }
    return config;
}

void outputNetworkProfile(int id) {
    NetworkConfig wifiProfiles = getWiFiProfiles(false);
    Serial.println("Name: " + String(wifiProfiles.profiles[id].name));
    Serial.println("SSID: " + String(wifiProfiles.profiles[id].ssid));
    Serial.println("Pass: " + String(wifiProfiles.profiles[id].password));
    Serial.println("IP: " + String(wifiProfiles.profiles[id].IP) + "\n");
}
void outputNetworkProfiles() {
    NetworkConfig wifiProfiles = getWiFiProfiles(false);
    for (int i = 0; i < 3; i++) {
        Serial.println("Profile " + String(i+1) + ": ");
        outputNetworkProfile(i);
    }
}

void networkProfileEditor() {
    Serial.println("Choose 1 of the profiles you want to edit (1/2/3): ");
    String id = getSerialInput(true);
    NetworkConfig wifiProfiles = getWiFiProfiles(false);
    if(id != "ABORTCMD") {
        if(id.toInt() != 1 && id.toInt() != 2 && id.toInt() != 3) {
            Serial.println("Invalid Profile ID!");
        } else {
            NetworkProfile profile = wifiProfiles.profiles[id.toInt()-1];
            Serial.println("Enter Profile Name (Leave blank to delete)");
            String value = getSerialInput(true);
            if(value != "ABORTCMD") {
                if(value == "") {
                    profile.name[0] = '\0';
                    profile.exists = false;
                    Serial.println("Profile cleared.");
                } else {
                    Serial.println("Enter SSID: ");
                    String ssid = getSerialInput(true);
                    if(ssid != "ABORTCMD") {
                        Serial.println("Enter Passphrase: ");
                        String pass = getSerialInput(true);
                        if(pass != "ABORTCMD") {
                            Serial.println("Enter IP Address: ");
                            String ip = getSerialInput(true);
                            if(ip != "ABORTCMD") {
                                value.toCharArray(profile.name,16);
                                ssid.toCharArray(profile.ssid,33);
                                profile.exists = true;
                                pass.toCharArray(profile.password, 65);
                                ip.toCharArray(profile.IP,16);
                                wifiProfiles.profiles[id.toInt()-1] = profile;
                                conf.begin("config", false);
                                conf.putBytes("ntwk_cfg", &wifiProfiles, sizeof(NetworkConfig));
                                conf.end();
                            } else return;
                        } else return;
                    } else return;
                }
            } else return;
        }
    } else return;
}