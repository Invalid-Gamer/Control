# Control
Work in Progress

* ToDo: 
    * shell: Add quit command to exit shell from everywhere

## Dokumentation:
Dieser Code gehört zu einer ESP 32-Fernbedienung um ein [Auto](https://github.com/P5-Natascha/Captain) zu steuern.
Außerdem ist die Fernbedienung mit einer eigenen Debug Shell ausgestattet.

* Aufbau:
    * main.cpp: Globale Funktionen und setup/loop
    * config.cpp: Config laden über Preferences.h
    * shell.cpp: Shell für config, debug, alles eigentlich
    * communication.cpp: UDP und TCP Comms
    * DisplayManager.cpp: Menüs, alles was auf dem Display angezeigt wird
    * joystick.cpp: Steuerung; Joystick eingabe, Aktionsverarbeitung
    * global.h: Globale Variablen/Funktionen
