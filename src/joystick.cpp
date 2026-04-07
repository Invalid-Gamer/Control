#include <Arduino.h>
#include <global.h>
#include <joystick.h>

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
        if(advancedLog){Serial.println("Menu right (Now selected: " + String(currentMenuOption) + ")");}
    } else if(currentPos.y < limit_left) {
        switch(currentMenuOption) {
            case INFO: currentMenuOption = AUTO; break;
            case AUTO: currentMenuOption = HAUTO; break;
            case HAUTO: currentMenuOption = MANUAL; break;
            case MANUAL: currentMenuOption = INFO; break;
        }
        if(advancedLog){Serial.println("Menu left (Now selected: " + String(currentMenuOption) + ")");}
    } else if (currentPos.btn) {
        currentCtrlMode = currentMenuOption;
        if(advancedLog){Serial.println("Button pressed (Selected Mode: " + String(currentCtrlMode) + ")");}
    }
}

void joyStickMode() {
    JoystickRaw currentPos = getRawJoystick();
    if(currentPos.btn) {
        currentMenuOption = currentCtrlMode;
        if(advancedLog){Serial.println("Button pressed (Exited Mode: " + String(currentCtrlMode) + ")");}
        currentCtrlMode = OFF;
    }
    if(currentCtrlMode == AUTO || currentCtrlMode == INFO) {
        if(currentPos.x > limit_right) {
            if(advancedLog){Serial.println("Mode left");}
        } else if(currentPos.x > limit_left) {
            if(advancedLog){Serial.println("Mode right");}
        }
    }
}