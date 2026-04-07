#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

const int limit_left = 1000;
const int limit_right = 2600;

struct JoystickRaw {
    int x;
    int y;
    bool btn;
};

JoystickRaw getRawJoystick();
void joyStickMenu();
void joyStickMode();

#endif