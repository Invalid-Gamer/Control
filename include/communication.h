#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <joystick.h>
extern unsigned long lastUdp;

struct ControlPacket {// Für wie die Datensätze aufgebaut sein sollten habe ich KI benutzt.
    uint16_t x;
    uint16_t y;
    uint8_t mode;
};

bool setupWiFi();
void disconnectWiFi();
void updateTCP();
String getWiFiStatus();
void sendMovementData(JoystickRaw raw, int currentMode);

#endif