#include <Arduino.h>
#include <global.h>
#include <joystick.h>
#include <communication.h>
#include <config.h>

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
        if(advancedLog){Serial.println("Menu right (Now selected: " + String(currentMenuOption) + ")");}
    } else if(currentPos.x < limit_left) {
        switch(currentMenuOption) {
            case INFO: currentMenuOption = AUTO; break;
            case AUTO: currentMenuOption = HAUTO; break;
            case HAUTO: currentMenuOption = MANUAL; break;
            case MANUAL: currentMenuOption = INFO; break;
        }
        piep(1);
        delay(menuWaitingDelay);
        if(advancedLog){Serial.println("Menu left (Now selected: " + String(currentMenuOption) + ")");}
    } else if (currentPos.btn) {
        piep(2);
        currentCtrlMode = currentMenuOption;
        updateTCP();
        if(advancedLog){Serial.println("Button pressed (Selected Mode: " + String(currentCtrlMode) + ")");}
    }
}

void joyStickMode() {
    JoystickRaw currentPos = getRawJoystick();
    if(currentPos.btn) {
        currentMenuOption = currentCtrlMode;
        if(advancedLog){Serial.println("Button pressed (Exited Mode: " + String(currentCtrlMode) + ")");}
        piep(2);
        currentCtrlMode = OFF;
        updateTCP();
    }
    if(currentCtrlMode == AUTO || currentCtrlMode == INFO) {
        if(currentPos.x > limit_right) {
            piep(1);
            delay(menuWaitingDelay);
            if(advancedLog){Serial.println("Mode left");}
        } else if(currentPos.x < limit_left) {
            piep(1);
            delay(menuWaitingDelay);
            if(advancedLog){Serial.println("Mode right");}
        }
    }
}