#include <Arduino.h> //Arduino Pkg
#include <Wire.h> // Display
#include <LiquidCrystal_I2C.h> // Display
#include <WiFi.h> // Internet
#include <Preferences.h> // EPROM Mäßige Speicherung von Config (Nicht im Code)

// Preferences: WiFi etc. - konstante Variablen - über Serial überschreibbar, nicht im Code vorhanden
Preferences prefs;
String WiFi_SSID, WiFi_Pass; // Captain WiFi
String Target_IP, Target_Port; // Captain Target
String Device_Name; // WiFi Module Name

// Joystick Config
const int Btn_Pin = 17; // Button vom Joystick
const int Vrx_Pin = 15; // X-Richtung
const int Vry_Pin = 32; // Y-Richtung

// LCD Config
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C Addresse, 16x2 Characters Init.

void setup() {
  Serial.begin(115200); // Serial Comms (Preferences ändern, debug)

  // Pins Setup
  pinMode(Btn_Pin, INPUT);
  pinMode(Vrx_Pin, INPUT);
  pinMode(Vry_Pin, INPUT);

  // LCD Setup
  lcd.init();
  lcd.backlight();
}

void loop() {
  
}