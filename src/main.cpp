#include <Arduino.h> //Arduino Pkg
#include <Wire.h> // Display
#include <LiquidCrystal_I2C.h> // Display
#include <WiFi.h> // Internet
#include <Preferences.h> // EPROM Mäßige Speicherung von Config (Nicht im Code)

// Preferences: WiFim etc. - konstante Variablen - über Serial überschreibbar, nicht im Code vorhanden
Preferences prefs;
String WIFI_SSID, WIFI_PASS; // Captain WiFi
String TARGET_IP, TARGET_PORT; // Captain Target
String DEVICE_NAME; // WiFi Name

// Joystick Config
const int Btn_Pin = 17; // Button vom Joystick
const int Vrx_Pin = 15; // X-Richtung
const int Vry_Pin = 32; // Y-Richtung

// LCD Config
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C Addresse, 16x2 Characters Init.

void setup() {
  // put your setup code here, to run once:
  pinMode(Btn_Pin, INPUT);
  pinMode(Vrx_Pin, INPUT);
  pinMode(Vry_Pin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}