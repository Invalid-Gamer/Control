#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <global.h>
#include <DisplayMgr.h>

const String mainMenuTitle = "Fernsteuerung   "; // Immer bis 16 Zeichen auffüllen!
int stayUntil;
bool statusDisplaying = false;

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

void displayMode(String title) {
    lcd.setCursor(0, 0);
    lcd.print(title);
    lcd.setCursor(0, 1);
}

void updateDisplay() {
    if(stayUntil  > millis()||statusDisplaying){
        if(advancedLog){Serial.println("Update Display: Passiv aufgrund showStatus." + String(stayUntil));}
        return;
    }
    String text;
    switch(currentOpMode) {
        case SETUP: {
            displayMode("Setup...");
            break;
        }
        case SHELL: {
            displayMode("In Shell");
            break;
        }
        case REGULAR:
            switch(currentCtrlMode) {
                case OFF:
                    displayTitle();
                    break;
                case MANUAL: {
                    displayMode("Manual");
                    break;
                }
                case HAUTO: {
                    displayMode("Halb-Auto");
                    break;
                }
                case AUTO: {
                    displayMode("Automatik"); 
                    break;
                }
            }
            break;
    }
}

void showStatus(String content) {
    lcd.clear();
    lcd.setCursor(0,0);
    String line1 = "";
    String line2 = "";
    if (content.length() > 16) {
        line1 = content.substring(0,16);
        line2 = content.substring(16,32);
    } else {
        line1 = content;
        line2 = "";
    }
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
    statusDisplaying = true;
}

void removeStatus(int time) {
    if(time == 0){
        lcd.clear();
    } else {
        time = time * 1000;
        stayUntil = millis() + time;
    }
    statusDisplaying = false;
    updateDisplay();
}

void displaySetBacklight(bool value) {
    if(value) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
}
void clearDisplay() {
    lcd.clear();
}