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
        Serial.println("Help:\nconfig - Configmanagement\nwifi - WiFi-Management\ndisplay - Display Management\nsetCtrlMode - change Control Mode\ndebug - Debug tool for Network, Joystick, etc.\nping - Ping\nreload - Restart the Remote\nexit - Exit shell or subprocess");
    } else if(cmd == "config") {
        currentShellMode = CONFIG;
        Serial.println("Config Shell " + configShellVer);
    } else if (cmd == "wifi") {
        currentShellMode = WIFI;
        Serial.println("WIFI Shell " + WiFiShellVer);
    } else if(cmd == "display") {
        currentShellMode = SCREEN;
        Serial.println("Display Shell "+ displayShellVer);
    } else if (cmd == "setCtrlMode") {
        Serial.println("Control Mode Choose: OFF/MANUAL/HAUTO/AUTO :");
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
                Serial.println("Control Mode doesn't exist: " + set);
                return;
            }
            Serial.println("Control Mode succesfully set to " + set);
        }
    } else if (cmd == "debug") {
        currentShellMode = DEBUG;
        Serial.println("Debug Shell "+ debugShellVer);
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
        Serial.println("Command not found: " + cmd);
    }
}

//----------------------------------
//--------------CONFIG--------------
//----------------------------------

// CONFIG Write/ADD
void shellWriteConf(bool ignoreExistance) {
    Serial.println("Enter config key to Write to: ");
    String key = getSerialInput(true);
    if(key != "ABORTCMD") {
        Serial.println("Enter datatype of config entry (String/int/bool):");
        String dtype = getSerialInput(true);
        if(dtype == "String") {
            Serial.println("Enter String: ");
            String value = getSerialInput(true);
            if(value != "ABORTCMD"){
                writeConfig(key, value, ignoreExistance);
            }
        } else if(dtype == "int") {
            Serial.println("Enter int: ");
            String value = getSerialInput(true);
            if(value != "ABORTCMD") {
                writeConfig(key, (int)value.toInt(), ignoreExistance);
            }
        } else if(dtype == "bool") {
            Serial.println("Enter ONLY 1/0:");
            String value = getSerialInput(true);
            if(value != "ABORTCMD") {
                writeConfig(key, (bool)value.toInt(), ignoreExistance);
            }
        } else {
            Serial.println("Unknown type. Stopping command execution!");
        }
    }
}

// Config Remove/DELETE
void shellRemoveConf(bool entireDelete) {
    if(entireDelete){
        Serial.println("Enter config key to remove ENTIRELY: ");
    } else {
        Serial.println("Enter config key to remove contents: ");
    }
    String key = getSerialInput(true);
    if(key != "ABORTCMD") {
        if(entireDelete) {
            Serial.println("Confirm deletion - This CANT BE UNDONE! CODE UNUSABLE (y/n): ");
            String antwort = getSerialInput(true);
            if(antwort == "y") {
                bool happened = deleteConfig(key, true);
                if(happened) {
                    Serial.println("Config Entry " + key + " Deleted. Carefull now.");
                }
            } else {
                Serial.println("Aborting...");
                return;
            }
        } else {
            Serial.println("Enter datatype of config entry (String/int):");
            String dtype = getSerialInput(true);
            if (dtype == "String") {
                writeConfig(key, "", false);
            } else if(dtype == "int") {
                writeConfig(key, 0, false);
            } else if (dtype == "ABORTCMD") {
                return;
            } else {
                Serial.println("Unknown type. Stopping command execution!");
            }
        }
    }
}

// CONFIG Shell
void handleConfigCommands(String cmd) {
    if(cmd == "help") {
        Serial.println("Config Shell Help:\nload - load Config into Variables\nshow - Show current config\nwrite - Write into existing Config entry (+ SECURE_OFF to add new entries/change datatype of existing entries)\nremove - Delete entry out of config (+ SECURE_OFF to totally delete keys DANGER - CODE WONT WORK)\nexit - Return to Standard-Shell");
    } else if (cmd == "load") {
        bool Ergebnis = loadConfig();
        if(Ergebnis){Serial.println("Config loaded successfully");} else {Serial.println("Config didn't load successfully");}
    } else if (cmd == "show") {
        outputConfigToSerial();
    } else if(cmd == "write") {
        shellWriteConf(false);
    } else if(cmd == "write SECURE_OFF") {
        shellWriteConf(true);
    } else if(cmd == "remove") {
        shellRemoveConf(false);
    } else if(cmd =="remove SECURE_OFF") {
        shellRemoveConf(true);
    } else if (cmd == "exit") {
        Serial.println("Resuming to normal shell");
        currentShellMode = BASE;
    } else {
        Serial.println("Command not found: " + cmd);
    }
}

//--------------------------------
//--------------WIFI--------------
//--------------------------------

// WIFI Shell
void handleWiFiCommands(String cmd) {
    if(cmd == "help") {
        Serial.println("WiFi Shell Help:\n--To change WiFi Credentials go to config shell!--\nstatus - WiFi Status\nconnect - Connect with WiFi\ndisconnect - Disconnect WiFi\ngetMacAddr - Returns MAC Address\nexit - Return to Standard-Shell");
    } else if(cmd == "status") {
        Serial.println(getWiFiStatus());
    } else if(cmd == "connect") {
        bool Ergebnis = setupWiFi();
        if(Ergebnis) {
            Serial.println("WiFi connected");
        } else {
            Serial.println("Error! Inspect debug log!");
        }
    } else if(cmd == "disconnect") {
        disconnectWiFi();
        Serial.println("WiFi disconnected.");
    } else if(cmd == "getMacAddr") {
        Serial.println(getMacAddress());
    } else if(cmd == "exit") {
        Serial.println("Resuming to normal shell");
        currentShellMode = BASE;
    } else {
        Serial.println("Commmand not found: " + cmd);
    }
}

//-----------------------------------
//--------------DISPLAY--------------
//-----------------------------------

// DISPLAY SHELL
void handleDisplayCommands(String cmd) {
    if(cmd == "help") {
        Serial.println("Display Shell Help:\nbacklight - turn backlight On/Off\nclear - clear the display\nshowStatus - show status on display (Max. 32 Characters)\nexit - Return to Standard-Shell");
    } else if(cmd == "backlight") {
        Serial.println("On/Off? (ONLY 1/0):");
        String value = getSerialInput(true);
        if(value != "ABORTCMD"){
            displaySetBacklight((bool)value.toInt());
            Serial.println("Backlight set.");
        }
    } else if(cmd == "clear") {
        removeStatus(0);
        clearDisplay();
        Serial.println("Display cleared");
    } else if(cmd == "showStatus") {
        Serial.println("Enter Display content? (MAX 32 Characters): ");
        String text = getSerialInput(true);
        if (text != "ABORTCMD"){
            showStatus(text);
            Serial.println("Status set");
        }
    } else if(cmd == "exit") {
        Serial.println("Resuming to normal shell");
        showStatus("In Shell");
        currentShellMode = BASE;
    } else {
        Serial.println("Command not found: " + cmd);
    }
}

//---------------------------------
//--------------DEBUG--------------
//---------------------------------

//DEBUG Shell
void handleDebugCommands(String cmd) {
    if(cmd == "help") {
        Serial.println("Debug Shell Help:\n---Debug Shell logs optional functions like Joystick movement, network outgoing and ingoing.---\njoystick - raw Joystick datastream\nexit - Return to Standard-Shell");
    } else if(cmd == "joystick") {
        Serial.println("Joystick debug starting in 3 seconds! To exit, type cancel!");
        delay(3000);
        while(true) {
            JoystickRaw joystickData = getRawJoystick();
            Serial.println("x: " + String(joystickData.x) + "; y: " + String(joystickData.y)+ "; Btn: " + String(joystickData.btn));
            if(Serial.available() > 0) {
                String input = Serial.readString();
                input.trim();
                if (input == "cancel") {
                    Serial.println("Exiting Joystick debug");
                    break;
                }
            }
        }
    } else if(cmd == "network") {
        Serial.println("Network: Incoming/Outgoing?: ");
        String answer = getSerialInput(true);
        if(answer != "ABORTCMD") {
            Serial.println(answer);
            if(answer == "Incoming" || answer == "incoming") {
                Serial.println("Incoming network debug starting in 3 seconds! To exit, type cancel!");
            while(true) {
                Serial.println(handleRawTCP());
                if(Serial.available() > 0) {
                    String input = Serial.readString();
                    input.trim();
                    if(input == "cancel") {
                        Serial.println("Exiting incoming network debug");
                        break;
                    }
                }
            }
            } else if(answer == "Outgoing" || answer == "outgoing") {

            } else {
                Serial.println("Invalid answer: " + answer + " Aborting...");
            }
        }
    } else if(cmd == "exit") {
        Serial.println("Resuming to normal shell");
        currentShellMode = BASE;
    }
}

//---------------------------------
//--------------SHELL--------------
//---------------------------------

//Nur EingabeChar setzen
void printShellChar() {
    if(currentShellMode == BASE) {
        Serial.print("$ ");
    } else if(currentShellMode == CONFIG) {
        Serial.print("Config $ ");
    } else if(currentShellMode == WIFI) {
        Serial.print("WiFi $ ");
    } else if(currentShellMode == SCREEN) {
        Serial.print("Display $ ");
    } else if(currentShellMode == DEBUG) {
        Serial.print("Debug $ ");
    }
}

//MAIN SHELL FUNCTION
void shell() {
    showStatus("In Shell");
    currentOpMode = SHELL;
    currentShellMode = BASE;
    Serial.println("Shell " + baseShellVer);
    piep(2);
    while(currentOpMode == SHELL) {
        printShellChar();
        String shellInput = getSerialInput(false);
        Serial.println(shellInput);
        if(currentShellMode == BASE) {
            handleBaseCommands(shellInput);
        } else if(currentShellMode == CONFIG) {
            handleConfigCommands(shellInput);
        } else if(currentShellMode == WIFI) {
            handleWiFiCommands(shellInput);
        } else if(currentShellMode == SCREEN) {
            handleDisplayCommands(shellInput);
        } else if(currentShellMode == DEBUG) {
            handleDebugCommands(shellInput);
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