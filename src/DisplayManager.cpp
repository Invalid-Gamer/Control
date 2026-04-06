#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <global.h>
#include <DisplayMgr.h>

const String mainMenuTitle = "Fernsteuerung";

// LCD Config
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C Addresse, 16x2 Characters Init.

void initDisplay() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void displayTitle() {
    lcd.setCursor(0, 0);
    lcd.print(mainMenuTitle);
    lcd.setCursor(0, 1);
}

void displayMode() {
    
}

void updateDisplay() {
    String text;
    if (currentOpMode == REGULAR) {
        displayTitle();
        switch(currentOpMode) {
            case SETUP: break;
            case SHELL: break;
            case REGULAR:
                displayTitle();
                switch(currentCtrlMode) {
                    case OFF:

                        break;
                    case MANUAL: break;
                    case HAUTO: break;
                    case AUTO:
                        // Variablen von Auto: Geschwindigkeit? Batterie Sensoren? 
                        break;
                }
                break;
        }
    }
}
