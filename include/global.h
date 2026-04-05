#ifndef PINS_H
#define PINS_H

// Pin Belegungen
extern const int Btn_Pin = 17; // Button vom Joystick
extern const int Vrx_Pin = 15; // X-Richtung Joystick
extern const int Vry_Pin = 32; // Y-Richtung Joystick
extern const int Bzr_Pin = 16; // Buzzer

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
void piep();

#endif