#include <Arduino.h>
#include <global.h>
#include <joystick.h>

JoystickRaw getRawJoystick() {
    JoystickRaw data;
    data.x = analogRead(Vrx_Pin);
    data.y = analogRead(Vry_Pin);
    data.btn = digitalRead(Btn_Pin);
    return data;
}

void joyStickMenu() {
    JoystickRaw currentPos = getRawJoystick();
    if(currentPos.x > limit_right) {
        if(advancedLog){Serial.println("Menu right");}
    } else if(currentPos.y < limit_left) {
        if(advancedLog){Serial.println("Menu left");}
    } else if (currentPos.btn) {
        if(advancedLog){Serial.println("Button pressed");}
    }
}