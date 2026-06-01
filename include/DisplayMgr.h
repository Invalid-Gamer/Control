#ifndef DISPLAYMGR_H
#define DISPLAYMGR_H

#include <Arduino.h>
#include <global.h>

struct CustomMenu {
    String title;
    String value;
};

void initDisplay();
void InactivityHandler();
void allSDATAValuesMenu(int page);
void updateDisplay();
void showStatus(String content);
void removeStatus(int time);
void displaySetBacklight(bool value);
void clearDisplay();
void showCustomMenu(CustomMenu menu);

#endif