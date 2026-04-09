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
    if(title.length() < 16) {
        int add = 15 - title.length();
        for (int i = 0; i < add; i++) {
            title += " ";
        }
    }
    lcd.print(title);
    lcd.setCursor(0, 1);
}

void changeBottomDisplay(String content) {
    lcd.setCursor(0, 1);
    if(content.length() < 16) {
        int add = 16 - content.length();
        for (int i = 0; i < add ; i++) {
            content += " ";
        }
    }
    content = content.substring(0, 16);
    lcd.print(content);
}

void updateDisplay() {
    if(stayUntil  > millis()||statusDisplaying){
        log("Update Display: Passiv aufgrund showStatus." + String(stayUntil));
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
                    switch(currentMenuOption) {
                        case OFF:
                            changeBottomDisplay("");
                            break;
                        case MANUAL:
                            changeBottomDisplay("Manual");
                            break;
                        case HAUTO:
                            changeBottomDisplay("Halb-Auto");
                            break;
                        case AUTO:
                            changeBottomDisplay("Automatik");
                            break;
                        case INFO:
                            changeBottomDisplay("Informationen");
                            break;
                    }
                    break;
                case MANUAL: {
                    displayMode("Manual");
                    String bottomContent = "Bat:" + String(battValue) + "A V:" + String(velValue);
                    changeBottomDisplay(bottomContent);
                    break;
                }
                case HAUTO: {
                    displayMode("Halb-Auto");
                    String bottomContent = "Bat:" + String(battValue) + "A V:" + String(velValue);
                    changeBottomDisplay(bottomContent);
                    break;
                }
                case AUTO: {
                    displayMode("Automatik"); 
                    String bottomContent = "Bat:" + String(battValue) + "A V:" + String(velValue);
                    changeBottomDisplay(bottomContent);
                    break;
                }
                case INFO: {
                    displayMode("Informationen");
                    String bottomContent = "Bat:" + String(battValue) + "A V:" + String(velValue);
                    changeBottomDisplay(bottomContent);
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