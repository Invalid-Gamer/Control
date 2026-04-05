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
#include <communication.h>

OperatingMode currentOpMode;

// Variablen
bool loadedConfig;
bool WiFiConnected;

void troubleshoot(void (*callback)(), bool doContinue) {
  Serial.println("Fehler is aufgetreten!");
  Serial.println("Starte Callback mit Loglevel debug... ");
  delay(1000);
  advancedLog = true;
  callback();
  advancedLog = false;
  if(!doContinue) {
    Serial.println("Can't continue, launching shell!");
    delay(500);
    shell();
  }
}

void troubleshoot(bool (*callback)(), bool doContinue) {
  Serial.println("Fehler is aufgetreten!");
  Serial.println("Starte Callback mit Loglevel debug... ");
  delay(1000);
  advancedLog = true;
  callback();
  advancedLog = false;
  if(!doContinue) {
    Serial.println("Can't continue, launching shell!");
    delay(500);
    shell();
  }
}

void piep() { // Kurzer Buzzerton (Hier definitert weil zu wenig für eigene Datei)
  digitalWrite(Bzr_Pin, HIGH);
  delay(100);
  digitalWrite(Bzr_Pin, LOW);
}

void setup() {
  currentOpMode = SETUP;
  Serial.begin(115200); // Serial Comms (Preferences ändern, debug)
  Serial.println("Serial up");
  // Pins Setup
  pinMode(Btn_Pin, INPUT);
  pinMode(Vrx_Pin, INPUT);
  pinMode(Vry_Pin, INPUT);
  pinMode(Bzr_Pin, OUTPUT);
  // LCD Setup
  initDisplay();
  // Lade Preferences
  Serial.println("Loading config");
  loadedConfig = loadConfig();
  if(!loadedConfig) {troubleshoot(loadConfig, false);}
  WiFiConnected = setupWiFi();
  if(!WiFiConnected){troubleshoot(setupWiFi, false);}
}

void loop() {
  
}