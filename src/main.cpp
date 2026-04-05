// Libraries
#include <Arduino.h> //Arduino Pkg
#include <Wire.h> // Display
#include <LiquidCrystal_I2C.h> // Display
#include <WiFi.h> // Internet
#include <Preferences.h> // EPROM Mäßige Speicherung von Config (Nicht im Code)

// Dateiverknüpfungen
#include <config.h>
#include <shell.h>
#include <global.h>
#include <DisplayMgr.h>

void piep() { // Kurzer Buzzerton (Hier definitert weil zu wenig für eigene Datei)
  digitalWrite(Bzr_Pin, HIGH);
  delay(100);
  digitalWrite(Bzr_Pin, LOW);
}

void setup() {
  Serial.begin(115200); // Serial Comms (Preferences ändern, debug)

  // Pins Setup
  pinMode(Btn_Pin, INPUT);
  pinMode(Vrx_Pin, INPUT);
  pinMode(Vry_Pin, INPUT);
  pinMode(Bzr_Pin, OUTPUT);

  // LCD Setup
  initDisplay();

  // Lade Preferences
  loadConfig();
}

void loop() {
  
}