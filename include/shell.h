#ifndef SHELL_H
#define SHELL_H
#include <Arduino.h>

void shell();
void serialHandler();

enum ShellMode {
    BASE,
    CONFIG,
    WIFI,
    SCREEN
};
extern ShellMode currentShellMode;

#endif 