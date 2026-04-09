#ifndef PINS_H
#define PINS_H

// Pin Belegungen
const int Btn_Pin = 25; // Button vom Joystick
const int Vrx_Pin = 32; // X-Richtung Joystick (getauscht damit kabelführung klappt)
const int Vry_Pin = 33; // Y-Richtung Joystick (getauscht damit kabelführung klappt)
const int Bzr_Pin = 16; // Buzzer
extern bool advancedLog; // Loglevel boolean
extern float battValue;
extern int velValue;

// Globale Variablen
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

extern OperatingMode currentOpMode;
extern ControlMode currentCtrlMode;
extern ControlMode currentMenuOption;

// Globale Funktionen
void log(String text);
void piep(int amount);
void troubleshoot(void (*callback()), bool doContinue);
void troubleshoot(bool (*callback()), bool doContinue);

#endif