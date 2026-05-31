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
#include <joystick.h>

Logging logging; // Globale logging Instanz
ConnectionType connectionType; // WIFI / BLUETOOTH
OperatingMode currentOpMode; // OFF,MANUAL,HAUTO,AUTO,INFO
ControlMode currentCtrlMode; // SETUP, REGULAR, SHELL
MenuOption currentMenuOption = M_MANUAL; // Hauptmenü

// Variablen
bool loadedConfig;
bool connectionEstablished;
bool displayBacklightState = true;
unsigned long long lastInteraction;

void troubleshoot(void (*callback)(), bool doContinue) {
  Serial.println("Encountered an Error!");
  Serial.println("Starting Callback with Loglevel debug... ");
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
  Serial.println("Encountered an Error!");
  Serial.println("Starting Callback with Loglevel debug... ");
  delay(1000);
  advancedLog = true;
  bool successed = callback();
  advancedLog = false;
  if(!doContinue && !successed) {
    Serial.println("Can't continue, launching shell!");
    delay(500);
    shell();
  } else if(successed) {
    Serial.println("Second run accomplished. Resuming");
  }
}

void piep(int amount) { // Kurzer Buzzerton (Hier definitert weil zu wenig für eigene Datei)
  for (int i = 0; i<amount; i++) { // Mehrmals buzzern?
    digitalWrite(Bzr_Pin, HIGH);
    delay(100);
    digitalWrite(Bzr_Pin, LOW);
    delay(100);
  }
}

void setup() {
  currentOpMode = SETUP;
  Serial.begin(9600); // Serial Comms (Preferences ändern, debug)
  Serial.println("Serial up");
  // Pins Setup
  pinMode(Btn_Pin, INPUT_PULLUP);
  pinMode(Vrx_Pin, INPUT);
  pinMode(Vry_Pin, INPUT);
  pinMode(Bzr_Pin, OUTPUT);
  // LCD Setup
  initDisplay();
  // Lade Preferences
  logging.info("Loading config");
  loadedConfig = loadConfig();
  if(!loadedConfig) {troubleshoot(loadConfig, false);}
  lastInteraction = millis();
  // Credential Handler: BT/WIFI, Welches Netzwerkprofil?
  connectionType = credentialHandler(true);
  connectionEstablished = setupConnection();
  if(!connectionEstablished){troubleshoot(setupConnection, false);}
  logging.info("Setup finished!");
  showStatus("Setup abgeschlossen");
  removeStatus(1);
  currentOpMode = REGULAR;
}

void loop() {
  serialHandler(); // Sucht nach "shell" in Serial
  if (isCommConnected()) {
    if(currentCtrlMode == OFF) {
      joyStickMenu();
    } else {
      joyStickMode();
      update();
    }
    updateDisplay();
    if(currentCtrlMode == MANUAL || currentCtrlMode == HAUTO) {
      JoystickRaw joyStickPos = getRawJoystick();
      sendMovementData(joyStickPos);
    }
  } else {
    connectionEstablished = setupConnection();
    if(!connectionEstablished){troubleshoot(setupConnection, false);}
  }
}