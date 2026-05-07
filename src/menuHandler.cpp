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
        if(currentMenuOption == M_SETTINGS) {
            currentMenuOption = M_MANUAL;
        } else {
            currentMenuOption = (MenuOption)((int)currentMenuOption + 1);
        }
        piep(1);
        delay(menuWaitingDelay);
        logging.debug("Menu right (Now selected: " + String(currentMenuOption) + ")");
    } else if(currentPos.x < limit_left) {
        if(currentMenuOption == M_MANUAL) {
            currentMenuOption = M_SETTINGS;
        } else {
            currentMenuOption = (MenuOption)((int)currentMenuOption - 1);
        }
        piep(1);
        delay(menuWaitingDelay);
        logging.debug("Menu left (Now selected: " + String(currentMenuOption) + ")");
    } else if (currentPos.btn) {
        piep(2);
        switch (currentMenuOption) {
            case M_MANUAL: currentCtrlMode = MANUAL; break;
            case M_HAUTO: currentCtrlMode = HAUTO; break;
            case M_AUTO: currentCtrlMode = AUTO; break;
            case M_INFO: currentCtrlMode = INFO; break;
            case M_SETTINGS: break;
        }
        updateMode();
        logging.debug("Button pressed (Selected Mode: " + String(currentCtrlMode) + ")");
    }
}

void joyStickMode() {
    JoystickRaw currentPos = getRawJoystick();
    if(currentPos.btn) {
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
