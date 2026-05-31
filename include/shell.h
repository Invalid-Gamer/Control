#ifndef SHELL_H
#define SHELL_H
#include <Arduino.h>

const String baseShellVer = "V1.0";
const String configShellVer = "V0.4";
const String WiFiShellVer = "V0.2";
const String BluetoothShellVer = "V0.1";
const String displayShellVer = "V0.1.1";
const String debugShellVer = "V0.2";

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