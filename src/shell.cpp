#include <Arduino.h>
#include <config.h>
#include <shell.h>
#include <global.h>
#include <communication.h>
#include <DisplayMgr.h>
#include <joystick.h>
#include <network.h>

struct parsedCommand {
    String command;
    String arguments[10];
    int argumentCount;
};

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
            output = "ABORTCMD";
            break;
        }
    }
    return output;
}

parsedCommand handleCommandString(String input) {
    parsedCommand result;
    result.argumentCount = 0;
    result.command = input;          // ganze Zeile

    String token = "";
    bool inQuotes = false;
    bool haveToken = false;

    auto flush = [&]() {
        if (!haveToken) return;
        if (result.argumentCount < 10) {
            result.arguments[result.argumentCount++] = token;
        }
        token = "";
        haveToken = false;
    };

    for (unsigned int i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '"') {
            inQuotes = !inQuotes;
            haveToken = true;
        } else if ((c == ' ' || c == '\t') && !inQuotes) {
            flush();
        } else {
            token += c;
            haveToken = true;
        }
    }
    flush();
    return result;
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
void handleConfigCommands(parsedCommand cmd) {
    if (cmd.argumentCount < 2) {
        Serial.println("Config Shell Help:\nload - load Config into Variables\nshow - Show current config\nwrite - Write into existing Config entry (+ SECURE_OFF to add new entries/change datatype of existing entries)\nremove - Delete entry out of config (+ SECURE_OFF to totally delete keys DANGER - CODE WONT WORK)\nexit - Return to Standard-Shell");
        return;
    }
    if(cmd.arguments[1] == "help") {
        Serial.println("Config Shell Help:\nload - load Config into Variables\nshow - Show current config\nwrite - Write into existing Config entry (+ SECURE_OFF to add new entries/change datatype of existing entries)\nremove - Delete entry out of config (+ SECURE_OFF to totally delete keys DANGER - CODE WONT WORK)\nexit - Return to Standard-Shell");
    } else if (cmd.arguments[1] == "load") {
        bool Ergebnis = loadConfig();
        if(Ergebnis){Serial.println("Config loaded successfully");} else {Serial.println("Config didn't load successfully");}
    } else if (cmd.arguments[1] == "show") {
        outputConfigToSerial();
    } else if(cmd.arguments[1] == "write" && cmd.arguments[2] != "SECURE_OFF") {
        shellWriteConf(false);
    } else if(cmd.arguments[1] == "write" && cmd.arguments[2] == "SECURE_OFF") {
        shellWriteConf(true);
    } else if(cmd.arguments[1] == "remove" && cmd.arguments[2] != "SECURE_OFF") {
        shellRemoveConf(false);
    } else if(cmd.arguments[1] =="remove" && cmd.arguments[2] == "SECURE_OFF") {
        shellRemoveConf(true);
    } else {
        Serial.println("Command not found: " + cmd.arguments[1]);
    }
}

//--------------------------------
//--------------WIFI--------------
//--------------------------------

// WiFi Shell
void handleWiFiCommands(parsedCommand cmd) {
    if (cmd.argumentCount < 2) {
        Serial.println("WiFi Shell Help:\nprofiles - Manage WiFi Profiles\nstatus - WiFi Status\nconnect - Connect with WiFi\ndisconnect - Disconnect WiFi\ngetMacAddr - Returns MAC Address\nexit - Return to Standard-Shell");
        return;
    }
    if(cmd.arguments[1] == "help") {
        Serial.println("WiFi Shell Help:\nprofiles - Manage WiFi Profiles\nstatus - WiFi Status\nconnect - Connect with WiFi\ndisconnect - Disconnect WiFi\ngetMacAddr - Returns MAC Address\nexit - Return to Standard-Shell");
    } else if(cmd.arguments[1] == "profiles") {
        if (cmd.argumentCount < 3) {
            Serial.println("WiFi Profile Management:\nlist - List all Profiles\nedit - Edit a profile (ADD/DELETE)");
            return;
        }
        if(cmd.arguments[2] == "list") {
            outputNetworkProfiles();
        } else if(cmd.arguments[2] == "edit") {
            networkProfileEditor();
        }
    } else if(cmd.arguments[1] == "status") {
        Serial.println(network.getStatus());
    } else if(cmd.arguments[1] == "connect") {
        bool Ergebnis = setupConnection();
        if(Ergebnis) {
            Serial.println("WiFi connected");
        } else {
            Serial.println("Error! Inspect debug log!");
        }
    } else if(cmd.arguments[1] == "disconnect") {
        disconnectComm();
        Serial.println("WiFi disconnected.");
    } else if(cmd.arguments[1] == "getMacAddr") {
        Serial.println(network.getMacAddress());
    } else {
        Serial.println("Commmand not found: " + cmd.arguments[1]);
    }
}

//-----------------------------------
//-------------BLUETOOTH-------------
//-----------------------------------

// Bluetooth Shell
void handleBluetoothCommands(parsedCommand cmd) {
    if (cmd.argumentCount < 2) {
        Serial.println("Bluetooth Shell Help:\n---Bluetooth Functionality isn't set up yet!\nexit - Return to Standard-Shell");
        return;
    }
    if(cmd.arguments[1] == "help") {
        Serial.println("Bluetooth Shell Help:\n---Bluetooth Functionality isn't set up yet!\nexit - Return to Standard-Shell");
    } else {
        Serial.println("Command not found: " + cmd.arguments[1]);
    }
}

//-----------------------------------
//--------------DISPLAY--------------
//-----------------------------------

// DISPLAY SHELL
void handleDisplayCommands(parsedCommand cmd) {
    if (cmd.argumentCount < 2) {
        Serial.println("Display Shell Help:\nbacklight - turn backlight On/Off\nclear - clear the display\nshowStatus - show status on display (Max. 32 Characters)\nexit - Return to Standard-Shell");
        return;
    }
    if(cmd.arguments[1] == "help") {
        Serial.println("Display Shell Help:\nbacklight - turn backlight On/Off\nclear - clear the display\nshowStatus - show status on display (Max. 32 Characters)\nexit - Return to Standard-Shell");
    } else if(cmd.arguments[1] == "backlight") {
        if (cmd.argumentCount < 3) {
            Serial.println("On/Off? (ONLY 1/0)");
            return;
        }
        if (cmd.arguments[2] != "1" && cmd.arguments[2] != "0") {
            Serial.println("Invalid argument. Use 1 for On and 0 for Off.");
            return;
        }
        displaySetBacklight((bool)cmd.arguments[2].toInt());
        Serial.println("Backlight set.");
    } else if(cmd.arguments[1] == "clear") {
        removeStatus(0);
        clearDisplay();
        Serial.println("Display cleared");
    } else if(cmd.arguments[1] == "showStatus") {
        if (cmd.argumentCount < 3) {
            Serial.println("Enter Display content? (MAX 32 Characters)");
            return;
        }
        Serial.println("Enter Display content? (MAX 32 Characters): ");
        showStatus(cmd.argumentCount > 2 ? cmd.arguments[2] : "");
        Serial.println("Status set");
    } else {
        Serial.println("Command not found: " + cmd.arguments[1]);
    }
}

//---------------------------------
//--------------DEBUG--------------
//---------------------------------

//DEBUG Shell
void handleDebugCommands(parsedCommand cmd) {
    if (cmd.argumentCount < 2) {
        Serial.println("Debug Shell Help:\n---Debug Shell logs optional functions like Joystick movement, network outgoing and ingoing.---\njoystick - raw Joystick datastream\nexit - Return to Standard-Shell");
        return;
    }
    if(cmd.arguments[1] == "help") {
        Serial.println("Debug Shell Help:\n---Debug Shell logs optional functions like Joystick movement, network outgoing and ingoing.---\njoystick - raw Joystick datastream\nexit - Return to Standard-Shell");
    } else if(cmd.arguments[1] == "joystick") {
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
    } else if(cmd.arguments[1] == "network") {
        if (cmd.argumentCount < 3) {
            Serial.println("Incoming/Outgoing?");
            return;
        }
        if(cmd.arguments[2] == "Incoming" || cmd.arguments[2] == "incoming") {
            Serial.println("Incoming network debug starting in 3 seconds! To exit, type cancel!");
            while(true) {
                Serial.println(network.handleRawTCP());
                if(Serial.available() > 0) {
                    String input = Serial.readString();
                    input.trim();
                    if(input == "cancel") {
                        Serial.println("Exiting incoming network debug");
                        break;
                    }
                }
            }
        } else if(cmd.arguments[2] == "Outgoing" || cmd.arguments[2] == "outgoing") {
            Serial.println("Outgoing network debug - Choose one of the following options:\n1 - Send custom packets\n2 - Send real packages like manual mode\n3 - Simulate TCP\nexit - Exit the subprocess");
            String answer = getSerialInput(true);
            if(answer == "1") {
                Serial.println("Enter TCP/UDP: ");
                answer = getSerialInput(true);
                if(answer == "TCP" || answer == "tcp") {
                    Serial.println("Enter packet key:");
                    String key = getSerialInput(true);
                    if(key != "ABORTCMD") {
                        Serial.println("Enter packet value:");
                        String value = getSerialInput(true);
                        if(value != "ABORTCMD") {
                            Serial.println("Sending: " + key + ":" + String(value));
                            network.sendTCP(key,value);
                        } else {
                            return;
                        }
                    } else {
                        return;
                    }
                } else if(answer == "UDP" || answer == "udp") {
                    Serial.println("Enter custom packet data:");
                    answer = getSerialInput(true);
                    if(answer != "ABORTCMD") {
                        Serial.println("Sending " + answer);
                        network.sendUDP(answer);
                    } else {
                        return;
                    }
                } else {
                    Serial.println("Invalid answer: " + answer + " Aborting...");
                }
            } else if (answer == "2") {
                Serial.println("Starting in 3 seconds... Use joystick like in manual mode. Sent UDP packets will be listed here. To exit, type cancel");
                delay(3000);
                currentCtrlMode = MANUAL;
                while(true) {
                    JoystickRaw joyStickPos = getRawJoystick();
                    ControlPacket packet = {(uint16_t)joyStickPos.x, (uint16_t)joyStickPos.y };
                    Serial.print("X: " + String(packet.x) + " " + String(joyStickPos.x));
                    Serial.println("Y: " + String(packet.y) + " " + String(joyStickPos.y));
                    sendMovementData(joyStickPos);
                    delay(20);
                    if(Serial.available() > 0) {
                        String input = Serial.readString();
                        input.trim();
                        if(input == "cancel") {
                            Serial.println("Exiting outgoing network debug");
                            break;
                        }
                    }
                }
                currentCtrlMode = OFF;
            } else if (answer == "3") {
                Serial.println("Enter currentCtrlMode to be sent (OFF,MANUAL,HAUTO,AUTO,INFO)");
                answer = getSerialInput(true);
                int value = 0;
                if(answer == "OFF") {
                    value = 0;
                } else if(answer == "MANUAL") {
                    value = 1;
                } else if(answer == "HAUTO") {
                    value = 2;
                } else if(answer == "AUTO") {
                    value = 3;
                } else if(answer == "INFO") {
                    value = 4;
                } else if(answer == "ABORTCMD") {
                    return;
                } else {
                    Serial.println("Invalid input: " + answer + " Aborting...");
                    return;
                }
                network.sendTCP("mode",value);
                Serial.println("Sending: mode:" + value);
            } else if (answer == "exit") {
                Serial.println("Exiting to Debug shell");
            } else if (answer == "ABORTCMD") {
                return;
            }
        } else {
            Serial.println("Invalid argument: " + cmd.arguments[2] + " Aborting...");
        }
    }
}

//--------------------------------
//--------------BASE--------------
//--------------------------------

// Help Menu
void helpMenu(parsedCommand cmd) {
    if (cmd.argumentCount < 2) {
        Serial.println("Help:\nconfig - Configmanagement\nwifi - WiFi-Management\nbluetooth - Bluetooth Management\ndisplay - Display Management\nsetCtrlMode - change Control Mode\nsetConnMode - Change Connection Mode\ndebug - Debug tool for Network, Joystick, etc.\nping - Ping\nreload - Restart the Remote\nexit - Exit shell or subprocess");
        return;
    }
    parsedCommand simulatedHelpCmd;
    simulatedHelpCmd.arguments[1] = "help";
    if (cmd.arguments[1] == "config") {
        handleConfigCommands(simulatedHelpCmd);
    } else if (cmd.arguments[1] == "wifi") {
        handleWiFiCommands(simulatedHelpCmd);
    } else if (cmd.arguments[1] == "bluetooth") {
        handleBluetoothCommands(simulatedHelpCmd);
    } else if (cmd.arguments[1] == "display") {
        handleDisplayCommands(simulatedHelpCmd);
    } else if (cmd.arguments[1] == "debug") {
        handleDebugCommands(simulatedHelpCmd);
    } else {
        Serial.println("Help menu not found: " + cmd.arguments[1]);
    }
}

// BASE Shell 
void handleBaseCommands(parsedCommand cmd) {
    if (cmd.arguments[0] == "help"){
        helpMenu(cmd);
    } else if(cmd.arguments[0] == "config") {
        Serial.println("Config Shell " + configShellVer);
        handleConfigCommands(cmd);
    } else if (cmd.arguments[0] == "wifi") {
        Serial.println("WIFI Shell " + WiFiShellVer);
        if(connectionType == BLUETOOTH) {
            Serial.println("Warning! Device is in Bluetooth Mode. Change Connection Mode in Base Shell using setConnMode");
        }
        handleWiFiCommands(cmd);
    } else if(cmd.arguments[0] == "bluetooth") {
        Serial.println("Bluetooth Shell " + BluetoothShellVer);
        if(connectionType == WIFI) {
            Serial.println("Warning! Device is in WiFi Mode. Change Connection Mode in Base Shell using setConnMode");
        }
        handleBluetoothCommands(cmd);
    } else if(cmd.arguments[0] == "display") {
        Serial.println("Display Shell "+ displayShellVer);
        handleDisplayCommands(cmd);
    } else if (cmd.arguments[0] == "setCtrlMode") {
        if (cmd.argumentCount < 2) {
            Serial.println("Control Mode Choose: OFF/MANUAL/HAUTO/AUTO");
            return;
        }
        if(cmd.arguments[1] == "OFF") {
            currentCtrlMode = OFF;
        } else if(cmd.arguments[1] == "MANUAL") {
            currentCtrlMode = MANUAL;
        } else if(cmd.arguments[1] == "HAUTO") {
            currentCtrlMode = HAUTO;
       } else if(cmd.arguments[1] == "AUTO") {
            currentCtrlMode = AUTO;
        } else {
            Serial.println("Control Mode doesn't exist: " + cmd.arguments[1]);
            return;
        }
        Serial.println("Control Mode succesfully set to " + cmd.arguments[1]);
    } else if (cmd.arguments[0] == "setConnMode") {
        if (cmd.argumentCount < 2) {
            Serial.println("Connection Mode Choose: WIFI/BLUETOOTH");
            return;
        }
        if(cmd.arguments[1] == "WIFI") {
            connectionType = WIFI;
            Serial.println("Please select Connection Profile on screen!");
            credentialHandler(false);
        } else if(cmd.arguments[1] == "BLUETOOTH") {
            connectionType = BLUETOOTH;
            Serial.println("Please select Connection Profile on screen!");
            credentialHandler(false);
        }
    } else if (cmd.arguments[0] == "debug") {
        Serial.println("Debug Shell "+ debugShellVer);
        handleDebugCommands(cmd);
    } else if (cmd.arguments[0] == "ping") {
        Serial.println("Pong!");
    } else if (cmd.arguments[0] == "reload") {
        Serial.println("Are you sure? (y/n):");
        String confirmation = getSerialInput(true);
        if(confirmation == "y") {
            Serial.println("Reloading in 3 seconds...");
            delay(3000);
            ESP.restart();
        } else {
            Serial.println("Aborting...");
        }
    } else if (cmd.arguments[0] == "exit") {
        Serial.println("Terminating shell");
        currentOpMode = REGULAR;
    } else if (cmd.arguments[0] == "") {
      return;  
    } else {
        Serial.println("Command not found: " + cmd.arguments[0]);
    }
}

//---------------------------------
//--------------SHELL--------------
//---------------------------------

//MAIN SHELL FUNCTION
void shell() {
    showStatus("In Shell");
    currentOpMode = SHELL;
    Serial.println("Shell " + baseShellVer);
    piep(2);
    while(currentOpMode == SHELL) {
        Serial.print("$ ");
        String shellInput = getSerialInput(false);
        parsedCommand parsed = handleCommandString(shellInput);
        Serial.println(parsed.command);
        if (shellInput != "quit") {
            handleBaseCommands(parsed);
        } else {
            Serial.println("Terminating shell.");
            currentOpMode = REGULAR;
        }
    }
    Serial.println("Shell exited.");
    currentOpMode = REGULAR;
    removeStatus(0);
}

void handleSingleShellCommand(String input) {
    parsedCommand parsed = handleCommandString(input);
    handleBaseCommands(parsed);
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