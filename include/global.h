#ifndef PINS_H
#define PINS_H

// Pin Belegungen
const int Btn_Pin = 17; // Button vom Joystick
const int Vrx_Pin = 15; // X-Richtung Joystick
const int Vry_Pin = 32; // Y-Richtung Joystick
const int Bzr_Pin = 16; // Buzzer
extern bool advancedLog; // Loglevel boolean

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
    AUTO
};
extern OperatingMode currentOpMode;
extern ControlMode currentCtrlMode;

// Globale Funktionen
void piep(int amount);
void troubleshoot(void (*callback()), bool doContinue);
void troubleshoot(bool (*callback()), bool doContinue);

#endif