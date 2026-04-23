#ifndef SHELL_H
#define SHELL_H
#include <Arduino.h>

const String baseShellVer = "V1.0";
const String configShellVer = "V0.4";
const String WiFiShellVer = "V0.2";
const String displayShellVer = "V0.1.1";
const String debugShellVer = "V0.2";

void shell();
void serialHandler();

enum ShellMode {
    BASE,
    CONFIG,
    WIFI,
    SCREEN,
    DEBUG
};
extern ShellMode currentShellMode;

#endif 