#include <Arduino.h>
#include <global.h>
#include <DisplayMgr.h>
#include <communication.h>
#include <joystick.h>
#include <network.h>

NetworkMgr network;

bool setupConnection() {
    if(connectionType == WIFI) {
        showStatus("Verbinde WLAN... ");
        bool success = network.begin();
        if(success) showStatus("WLAN Verbunden"); removeStatus(1);
        return success;
    } else if (connectionType == BLUETOOTH) {
        showStatus("Bluetooth existiert nicht");
        connectionType = WIFI;
        return false;
    }
}

void disconnect() {
    if(connectionType == WIFI) {
        network.disconnect();
    } else if(connectionType == BLUETOOTH) {
        // DUMMY
    }
}

void updateMode() {
    if(connectionType == WIFI) {
        network.updateTCP();
    } else if(connectionType == BLUETOOTH) {
        // DUMMY
    }
}

String getConnectionStatus() {
    if(connectionType == WIFI) {
        String answer = network.getStatus();
        return answer;
    } else if(connectionType == BLUETOOTH) {
        // DUMMY
    }
}

bool isConnected() {
    if(connectionType == WIFI) {
        bool answer = network.isConnected();
        return answer;
    } else if(connectionType == BLUETOOTH) {
        // DUMMY
    }
}

void sendMovementData(JoystickRaw raw) {
    if(connectionType == WIFI) {
        network.sendMovement(raw);
    } else if(connectionType == BLUETOOTH) {
        // DUMMY
    }
}

void update() {
    if(connectionType == WIFI) {
        network.handleIncomingTCP();
    } else if(connectionType == BLUETOOTH) {
        // DUMMY
    }
}

