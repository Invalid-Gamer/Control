#ifndef DISPLAYMGR_H
#define DISPLAYMGR_H

#include <Arduino.h>
#include <global.h>

void initDisplay();
void updateDisplay();
void showStatus(String content);
void removeStatus(int time);
void displaySetBacklight(bool value);
void clearDisplay();

#endif