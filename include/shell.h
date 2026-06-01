#ifndef SHELL_H
#define SHELL_H
#include <Arduino.h>

const String baseShellVer = "V1.0";
const String configShellVer = "V1.0";
const String WiFiShellVer = "V1.0";
const String BluetoothShellVer = "V0.0";
const String displayShellVer = "V1.0";
const String debugShellVer = "V1.0";

void shell();
void serialHandler();
String getSerialInput(bool timeout);

enum ShellMode {
    BASE,
    CONFIG,
    S_WIFI,
    S_BLUETOOTH,
    SCREEN,
    DEBUG
};
extern ShellMode currentShellMode;

#endif 