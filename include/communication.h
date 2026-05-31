#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <joystick.h>

struct ControlPacket {// Für wie die Datensätze aufgebaut sein sollten habe ich KI benutzt.
    uint16_t x;
    uint16_t y;
};

bool setupConnection();
void disconnectComm();
void updateMode();
String getConnectionStatus();
bool isCommConnected();
void sendMovementData(JoystickRaw raw);
void update();

#endif