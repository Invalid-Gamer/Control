#ifndef GLOBAL_H
#define GLOBAL_H

#include <logging.h>
extern Logging logging;

// Pin Belegungen
const int Btn_Pin = 25; // Button vom Joystick
const int Vrx_Pin = 32; // X-Richtung Joystick (getauscht damit kabelführung klappt)
const int Vry_Pin = 33; // Y-Richtung Joystick (getauscht damit kabelführung klappt)
const int Bzr_Pin = 16; // Buzzer
extern bool advancedLog; // Loglevel boolean
extern float battValue;
extern float lenkungValue;
extern float ampereValue;
extern unsigned int abstandVorne;
extern unsigned int abstandHinten;
extern bool displayBacklightState;
extern unsigned long long lastInteraction;
extern int inactivityTimeout;

// Globale Variablen
enum ConnectionType {
    WIFI,
    BLUETOOTH
};

enum OperatingMode { // Global: Was muss angezeigt werden etc.
    SETUP,
    SHELL,
    REGULAR
};

enum ControlMode { // Global und für Joystick/Network: Was muss gesendet werden?
    OFF,
    MANUAL,
    HAUTO,
    AUTO,
    INFO
};

enum MenuOption {
    M_MANUAL,
    M_HAUTO,
    M_AUTO,
    M_INFO,
    M_SETTINGS
};

extern ConnectionType connectionType;
extern OperatingMode currentOpMode;
extern ControlMode currentCtrlMode;
extern MenuOption currentMenuOption;

// Globale Funktionen
void piep(int amount);
void troubleshoot(void (*callback()), bool doContinue);
void troubleshoot(bool (*callback()), bool doContinue);

#endif