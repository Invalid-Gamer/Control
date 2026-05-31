#include <Arduino.h>
#include <global.h>
#include <DisplayMgr.h>
#include <config.h>
#include <shell.h>
#include <joystick.h>

String WiFi_SSID = "";
String WiFi_Pass = "";
String Target_IP = "";

ConnectionType credentialHandler(bool setMode) {
    if (setMode && (connectionType == WIFI || connectionType == BLUETOOTH)){
        if (!setMode) {logging.error("Still no connectionType configured! Can't continue without!");}
        // Zukünftig (WIFI/BLUETOOTH), kann von Shell aus ausgeführt werden.
    }
    if(connectionType == WIFI) {
        int currentChoice = 0;
        NetworkConfig wifiProfiles = getWiFiProfiles(true);
        String Profiles[3];
        for (int i = 0; i < 3; i++) {
            NetworkProfile currentProfile = wifiProfiles.profiles[i];
            if (currentProfile.exists && currentProfile.name[0] != '\0') {
                Profiles[i] = String(currentProfile.name);
            } else {
                Profiles[i] = "[EMPTY]";
            }
        }
        CustomMenu menu;
        menu.title = "Netzwerkprofile";
        InactivityHandler();
        while (true) { 
            JoystickRaw currentPos = getRawJoystick();
            if(currentChoice != 3) {
                menu.value = Profiles[currentChoice];
            } else {
                menu.value = "ENTER SHELL";
            }
            showCustomMenu(menu);
            if(currentPos.x > limit_right) {
                lastInteraction = millis();
                currentChoice += 1;
                if (currentChoice == 4) {
                    currentChoice = 0;
                }
                piep(1);
                delay(menuWaitingDelay);
            } else if (currentPos.x < limit_left){
                lastInteraction = millis();
                if(currentChoice == 0) {
                    currentChoice = 3;
                } else {
                    currentChoice -= 1;
                }
                piep(1);
                delay(menuWaitingDelay);
            } else if (currentPos.btn) {
                lastInteraction = millis();
                if(currentChoice == 3) {
                    shell();
                } else {
                    if(Profiles[currentChoice] != "[EMPTY]") {
                        WiFi_SSID = wifiProfiles.profiles[currentChoice].ssid;
                        WiFi_Pass = wifiProfiles.profiles[currentChoice].password;
                        Target_IP = wifiProfiles.profiles[currentChoice].IP;
                        showStatus("Profil ausgewählt");
                        piep(2);
                        break;
                    } else {
                        showStatus("Unkonfiguriertes Profil");
                        removeStatus(1);
                    }   
                }
                piep(2);
                delay(menuWaitingDelay);
            }
            serialHandler();
        }
        logging.debug("Exiting Network shell");
        logging.debug(WiFi_SSID+" "+WiFi_Pass+" "+Target_IP);
        return WIFI;
    }
}