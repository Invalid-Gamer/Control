#include <Arduino.h>
#include <config.h>
#include <shell.h>
#include <global.h>
#include <communication.h>
#include <DisplayMgr.h>
#include <joystick.h>

ShellMode currentShellMode;

// Needed Functions
String getSerialInput(bool timeout) {
    String input = "";
    String output = "ABORTCMD";
    unsigned long start = millis();
    while(input == "") {
        if(Serial.available() > 0) {
            input = Serial.readString();
            input.trim();
            output = input;
            break;
        }
        if((millis() - start) > 30000 && timeout) {
            Serial.println("Waited too long for input! Returning to normal shell.");
            currentShellMode = BASE;
            output = "ABORTCMD";
            break;
        }
    }
    return output;
}

//--------------------------------
//--------------BASE--------------
//--------------------------------

// BASE Shell 
void handleBaseCommands(String cmd) {
    if (cmd == "help"){
        Serial.println("Help:\nconfig - Konfigurationsmanagement\nwifi - Gehe in das WiFi Management\ndisplay - Display Management\nsetCtrlMode - Control Mode ändern\nping - Überprüfe die Responsivität der shell\nreload - Startet den ESP neu!\nexit - Verlasse die shell oder ein Unterprogramm");
    } else if(cmd == "config") {
        currentShellMode = CONFIG;
        Serial.println("Config Shell V0.1.1");
    } else if (cmd == "wifi") {
        currentShellMode = WIFI;
        Serial.println("WIFI Shell V0.1");
    } else if(cmd == "display") {
        currentShellMode = SCREEN;
        Serial.println("Display Shell V0.1");
    } else if (cmd == "setCtrlMode") {
        Serial.println("Control Mode setzen. Auswahl: OFF/MANUAL/HAUTO/AUTO :");
        String set = getSerialInput(true);
        if(set != "ABORTCMD") {
            if(set == "OFF") {
                currentCtrlMode = OFF;
            } else if(set == "MANUAL") {
                currentCtrlMode = MANUAL;
            } else if(set == "HAUTO") {
                currentCtrlMode = HAUTO;
            } else if(set == "AUTO") {
                currentCtrlMode = AUTO;
            } else {
                Serial.println("Dieser Control Mode existiert nicht: " + set);
                return;
            }
            Serial.println("Control Mode erfolgreich auf " + set + " gesetzt.");
        }
    } else if (cmd == "ping") {
        Serial.println("Pong!");
    } else if (cmd == "reload") {
        Serial.println("Are you sure? (y/n):");
        String confirmation = getSerialInput(true);
        if(confirmation == "y") {
            Serial.println("Reloading in 3 seconds...");
            delay(3000);
            ESP.restart();
        } else {
            Serial.println("Aborting...");
        }
    } else if (cmd == "exit") {
        Serial.println("Exiting shell");
        currentOpMode = REGULAR;
    } else {
        Serial.println("Command nicht gefunden: " + cmd);
    }
}

//----------------------------------
//--------------CONFIG--------------
//----------------------------------

// CONFIG Write
void shellWriteConf() {
    Serial.println("Enter config key to Write to: ");
    String key = getSerialInput(true);
    if(key != "ABORTCMD") {
        Serial.println("Enter datatype of config entry (String/int/bool):");
        String dtype = getSerialInput(true);
        if(dtype == "String") {
            Serial.println("Enter String: ");
            String value = getSerialInput(true);
            if(value != "ABORTCMD"){
                writeConfig(key, value);
            }
        } else if(dtype == "int") {
            Serial.println("Enter int: ");
            String value = getSerialInput(true);
            if(value != "ABORTCMD") {
                writeConfig(key, (int)value.toInt());
            }
        } else if(dtype == "bool") {
            Serial.println("Enter ONLY 1/0:");
            String value = getSerialInput(true);
            if(value != "ABORTCMD") {
                writeConfig(key, (bool)value.toInt());
            }
        } else {
            Serial.println("Unknown type. Stopping command execution!");
        }
    }
}
// CONFIG Shell
void handleConfigCommands(String cmd) {
    if(cmd == "help") {
        Serial.println("Config Shell Help:\nload - lädt den Config aus dem Speicher erneut in die Variablen\nshow - Zeigt aktuelle config Tabelle\nwrite - Schreibe in einen existierenden Config Eintrag\nexit - Kehre zur Standard-Shell zurück");
    } else if (cmd == "load") {
        bool Ergebnis = loadConfig();
        if(Ergebnis){Serial.println("Config loaded successfully");} else {Serial.println("Config didn't load successfully");}
    } else if (cmd == "show") {
        outputConfigToSerial();
    } else if(cmd == "write") {
        shellWriteConf();
    } else if (cmd == "exit") {
        Serial.println("Resuming to normal shell");
        currentShellMode = BASE;
    } else {
        Serial.println("Command nicht gefunden: " + cmd);
    }
}

//--------------------------------
//--------------WIFI--------------
//--------------------------------

// WIFI Shell
void handleWiFiCommands(String cmd) {
    if(cmd == "help") {
        Serial.println("WiFi Shell Help:\n--Um WiFi Credentials zu ändern, gehe in die Config Shell!--\nstatus - WiFi Status\nconnect - Mit WLAN Verbinden\ndisconnect - Verbindung trennen\nexit - Kehre zur Standard-Shell zurück");
    } else if(cmd == "status") {
        Serial.println(getWiFiStatus());
    } else if(cmd == "connect") {
        bool Ergebnis = setupWiFi();
        if(Ergebnis) {
            Serial.println("WiFi connected");
        } else {
            Serial.println("Fehler! Siehe Debug Log!");
        }
    } else if(cmd == "disconnect") {
        disconnectWiFi();
        Serial.println("WLAN Verbindung getrennt.");
    } else if(cmd == "exit") {
        Serial.println("Resuming to normal shell");
        currentShellMode = BASE;
    } else {
        Serial.println("Commmand nicht gefunden: " + cmd);
    }
}

//-----------------------------------
//--------------DISPLAY--------------
//-----------------------------------

// DISPLAY SHELL
void handleDisplayCommands(String cmd) {
    if(cmd == "help") {
        Serial.println("Display Shell Help:\nbacklight - schalte das backlight An/Aus\nclear - lösche alle Daten auf dem Display\nshowStatus - Zeige einen Status auf dem Display (Max. 32 Zeichen)\nexit - Kehre zur Standard-Shell zurück");
    } else if(cmd == "backlight") {
        Serial.println("An/Aus? (NUR 1/0):");
        String value = getSerialInput(true);
        if(value != "ABORTCMD"){
            displaySetBacklight((bool)value.toInt());
            Serial.println("Backlight geändert.");
        }
    } else if(cmd == "clear") {
        removeStatus(0);
        clearDisplay();
        Serial.println("Display cleared");
    } else if(cmd == "showStatus") {
        Serial.println("Was möchtest du anzeigen? (MAX 32 Zeichen): ");
        String text = getSerialInput(true);
        if (text != "ABORTCMD"){
            showStatus(text);
            Serial.println("Status gesetzt");
        }
    } else if(cmd == "exit") {
        Serial.println("Resuming to normal shell");
        showStatus("In Shell");
        currentShellMode = BASE;
    } else {
        Serial.println("Command nicht gefunden: " + cmd);
    }
}

//---------------------------------
//--------------SHELL--------------
//---------------------------------

//Nur EingabeChar setzen
void printShellChar() {
    if(currentShellMode == BASE) {
        Serial.println("$ ");
    } else if(currentShellMode == CONFIG) {
        Serial.println("Config $ ");
    } else if(currentShellMode == WIFI) {
        Serial.println("WiFi $ ");
    } else if(currentShellMode == SCREEN) {
        Serial.println("Display $ ");
    }
}

//MAIN SHELL FUNCTION
void shell() {
    showStatus("In Shell");
    currentOpMode = SHELL;
    currentShellMode = BASE;
    Serial.println("Shell V0.3");
    piep(2);
    while(currentOpMode == SHELL) {
        printShellChar();
        String shellInput = getSerialInput(false);
        if(currentShellMode == BASE) {
            handleBaseCommands(shellInput);
        } else if(currentShellMode == CONFIG) {
            handleConfigCommands(shellInput);
        } else if(currentShellMode == WIFI) {
            handleWiFiCommands(shellInput);
        } else if(currentShellMode == SCREEN) {
            handleDisplayCommands(shellInput);
        }
    }
    Serial.println("Shell exited.");
    currentOpMode = REGULAR;
    removeStatus(0);
}

// Serial Überwachung zur Aktivierung der Shell
void serialHandler() {
    if(Serial.available() > 0) {
        String shellInput = Serial.readString();
        shellInput.trim();
        if(shellInput == "shell") {
            shell();
        } else {
            Serial.println("Unknown command: " + shellInput + " Use shell to enter shell!");
        }
    }
}