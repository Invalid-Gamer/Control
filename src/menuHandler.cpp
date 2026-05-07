#include <Arduino.h>
#include <global.h>
#include <joystick.h>
#include <communication.h>
#include <config.h>
#include <DisplayMgr.h>
#include <shell.h>

String WiFi_SSID = "";
String WiFi_Pass = "";
String Target_IP = "";

JoystickRaw getRawJoystick() {
    JoystickRaw data;
    data.x = analogRead(Vrx_Pin);
    data.y = analogRead(Vry_Pin);
    data.btn = !digitalRead(Btn_Pin);
    return data;
}

void joyStickMenu() {
    JoystickRaw currentPos = getRawJoystick();
    if(currentPos.x > limit_right) {
        switch(currentMenuOption) {
            case INFO: currentMenuOption = MANUAL; break;
            case MANUAL: currentMenuOption = HAUTO; break;
            case HAUTO: currentMenuOption = AUTO; break;
            case AUTO: currentMenuOption = INFO; break;
        }
        piep(1);
        delay(menuWaitingDelay);
        logging.debug("Menu right (Now selected: " + String(currentMenuOption) + ")");
    } else if(currentPos.x < limit_left) {
        switch(currentMenuOption) {
            case INFO: currentMenuOption = AUTO; break;
            case AUTO: currentMenuOption = HAUTO; break;
            case HAUTO: currentMenuOption = MANUAL; break;
            case MANUAL: currentMenuOption = INFO; break;
        }
        piep(1);
        delay(menuWaitingDelay);
        logging.debug("Menu left (Now selected: " + String(currentMenuOption) + ")");
    } else if (currentPos.btn) {
        piep(2);
        currentCtrlMode = currentMenuOption;
        updateMode();
        logging.debug("Button pressed (Selected Mode: " + String(currentCtrlMode) + ")");
    }
}

void joyStickMode() {
    JoystickRaw currentPos = getRawJoystick();
    if(currentPos.btn) {
        currentMenuOption = currentCtrlMode;
        logging.debug("Button pressed (Exited Mode: " + String(currentCtrlMode) + ")");
        piep(2);
        currentCtrlMode = OFF;
        updateMode();
    }
    if(currentCtrlMode == AUTO || currentCtrlMode == INFO) {
        if(currentPos.x > limit_right) {
            piep(1);
            delay(menuWaitingDelay);
            logging.debug("Mode left");
        } else if(currentPos.x < limit_left) {
            piep(1);
            delay(menuWaitingDelay);
            logging.debug("Mode right");
        }
    }
}

ConnectionType credentialHandler() {
    int currentChoice = 0;
    NetworkConfig wifiProfiles = getWiFiProfiles();
    String Profiles[3];
    for (int i = 0; i < 3; i++) {
        NetworkProfile currentProfile = wifiProfiles.profiles[i];
        if (currentProfile.exists && currentProfile.ssid[0] != '\0') {
            Profiles[i] = String(currentProfile.ssid);
        } else {
            Profiles[i] = "[EMPTY]";
        }
    }
    CustomMenu menu;
    menu.title = "Netzwerkprofile";
    while (true) { 
        JoystickRaw currentPos = getRawJoystick();
        if(currentChoice != 3) {
            menu.value = Profiles[currentChoice];
        } else {
            menu.value = "ENTER SHELL";
        }
        showCustomMenu(menu);
        if(currentPos.x > limit_right) {
            currentChoice += 1;
            if (currentChoice == 4) {
                currentChoice = 0;
            }
            piep(1);
            delay(menuWaitingDelay);
        } else if (currentPos.x < limit_left){
            if(currentChoice == 0) {
                currentChoice = 3;
            } else {
                currentChoice -= 1;
            }
            piep(1);
            delay(menuWaitingDelay);
        } else if (currentPos.btn) {
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