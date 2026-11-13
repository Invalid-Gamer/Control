#ifndef SHELL_H
#define SHELL_H
#include <Arduino.h>

const String baseShellVer = "V2.0";
const String configShellVer = "V2.0";
const String WiFiShellVer = "V2.0";
const String BluetoothShellVer = "V0.2.0";
const String displayShellVer = "V2.0";
const String debugShellVer = "V2.0";

void shell();
void handleSingleShellCommand(String input);
void serialHandler();
String getSerialInput(bool timeout);

#endif 