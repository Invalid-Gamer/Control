#include <Arduino.h>
#include <config.h>
#include <shell.h>
#include <global.h>

void shell() { // Die Shell Schleife
    currentOpMode = SHELL;
    Serial.println("Shell V0.0.1");
    while(currentOpMode == SHELL) {
        Serial.print("$ ");
        while(true) {
            if(Serial.available() > 0) {
                piep();
                String shellInput = Serial.readString();
                shellInput.trim();
                if (shellInput == "ping") {
                    Serial.println("Pong!");
                    break;
                } else if (shellInput == "exit") {
                    Serial.println("Exiting shell");
                    currentOpMode = REGULAR;
                    break;
                }
            }
        }
    }
    Serial.println("Shell exited.");
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