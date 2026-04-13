# Control
Work in Progress

* ToDo: 
    * shell debug Network outgoing
    * TCP Verbindung: Während mode 0 nur leere pkgs? Irgendwie aufrecht erhalten!

* Dokumentation:
    * Aufbau:
        * main.cpp: Globale Funktionen und setup/loop
        * config.cpp: Config laden über Preferences.h
        * shell.cpp: Shell für config, debug, alles eigentlich
        * communication.cpp: UDP und TCP Comms
        * DisplayManager.cpp: Menüs, alles was auf dem Display angezeigt wird
        * joystick.cpp: Steuerung; Joystick eingabe, Aktionsverarbeitung
        * global.h: Globale Variablen/Funktionen
