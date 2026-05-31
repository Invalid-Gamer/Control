#include <Arduino.h>
#include <global.h>
#include <DisplayMgr.h>
#include <communication.h>
#include <joystick.h>
#include <network.h>

NetworkMgr network;

// Differenziert zwischen WIFI und Bluetooth, erhält dabei Code Integrität

bool setupConnection() { // Verbindung herstellen
    bool success = false;
    if(connectionType == WIFI) {
        showStatus("Verbinde WLAN... ");
        success = network.begin();
        if(success){showStatus("WLAN Verbunden");removeStatus(1);}
    } else if (connectionType == BLUETOOTH) {
        showStatus("Bluetooth existiert nicht");
        connectionType = WIFI;
        success = false;
    }
    return success;
}

void disconnectComm() {
    if(connectionType == WIFI) {
        network.disconnect();
    } else if(connectionType == BLUETOOTH) {
        piep(1);
    }
}

void updateMode() { // Mode an Auto senden
    if(connectionType == WIFI) {
        network.updateTCP();
    } else if(connectionType == BLUETOOTH) {
        piep(1);
    }
}

String getConnectionStatus() { // Detaillierter Verbindungsstatus im String
    String answer;
    if(connectionType == WIFI) {
        answer = network.getStatus();
    } else if(connectionType == BLUETOOTH) {
        piep(1);
    }
    return answer;
}

bool isCommConnected() { // Gibt Verbindungsaktivität in Boolean wieder
    bool answer = false;
    if(connectionType == WIFI) {
        answer = network.isConnected();
    } else if(connectionType == BLUETOOTH) {
        piep(1);
    }
    return answer;
}

void sendMovementData(JoystickRaw raw) {
    if(connectionType == WIFI) {
        network.sendMovement(raw);
    } else if(connectionType == BLUETOOTH) {
        // DUMMY
    }
}

void update() { // Sensordaten vom Auto empfangen
    if(connectionType == WIFI) {
        network.handleIncomingTCP();
    } else if(connectionType == BLUETOOTH) {
        // DUMMY
    }
}

