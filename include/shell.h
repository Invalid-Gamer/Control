#ifndef SHELL_H
#define SHELL_H
#include <Arduino.h>

void shell();
void serialHandler();

enum ShellMode {
    BASE,
    CONFIG,
    WIFI
};
extern ShellMode currentShellMode;

#endif 