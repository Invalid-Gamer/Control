#include <Arduino.h>
#include <config.h>
#include <shell.h>
#include <global.h>

void shell() { // Die Shell Schleife
    while(true) {
        if(Serial.available() > 0) {
            piep();
            String shellInput = Serial.readString();
            shellInput.trim();
            if (shellInput == "exit") {
                break;
            }
        }
    }
}

void serialHandler() { // Checkt ob shell aktiviert werden soll
    if(Serial.available() > 0) {
        piep();
        String shellInput = Serial.readString();
        if(shellInput == "shell") {
            shell();
        } else {
            Serial.println("Unknown command: " + shellInput + "Use shell to enter shell!");
        }
    }
}