#include <Arduino.h>
#include <global.h>
#include <WiFi.h>
#include <DisplayMgr.h>
#include <config.h>
#include <communication.h>
#include <WiFiUdp.h>
#include <joystick.h>

WiFiUDP udp;
WiFiClient tcp;

unsigned long lastUdp = 0;
int lastMode = -1;
float battValue = 0;
float lenkungValue = 0;


bool setupWiFi() {
    showStatus("Verbinde WLAN... ");
    WiFi.setHostname(Device_Name.c_str());
    log("WiFi: Using Credentials: "); log("SSID:" + WiFi_SSID + "; Pass: " + WiFi_Pass + "; Current Hostname: " + String(WiFi.getHostname()));
    WiFi.begin(WiFi_SSID.c_str(),WiFi_Pass.c_str());
    unsigned long start = millis();
    log("Waiting for connection at: " + String(start));
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        if(millis() - start > 20000) {
            log("WiFi Connection timed out!"); Serial.println("Waited for: " + String(millis() - start));
            return false;
        }
    }
    log("WiFi successfully connected after: " + String(millis() - start));
    showStatus("WLAN Verbunden");
    removeStatus(1);
    return true;
}

void disconnectWiFi() {
    log("WiFi disconnected");
    WiFi.disconnect();
}

String getWiFiStatus() {
    switch (WiFi.status()) { // KI generiert
        case WL_CONNECTED:  {     
        String info = "Verbunden!";
        info += "\n SSID: " + WiFi.SSID();
        info += "\n IP: " + WiFi.localIP().toString();
        info += "\n Signal: " + String(WiFi.RSSI()) + "dBm";
        info += "\n Hostname: " + String(WiFi.getHostname());
        info += "\n Mac Address: " + String(WiFi.macAddress());
        return info;
        }
        case WL_NO_SSID_AVAIL:   return "SSID nicht gefunden (WL_NO_SSID_AVAIL)";
        case WL_CONNECT_FAILED:  return "Verbindung fehlgeschlagen (WL_CONNECT_FAILED)";
        case WL_IDLE_STATUS:     return "Leerlauf (WL_IDLE_STATUS)";
        case WL_DISCONNECTED:    return "Getrennt (WL_DISCONNECTED)";
        default:                 return "Unbekannter Status (" + String(WiFi.status()) + ")";
    }
}

bool isWiFiConnected() {
    if (WiFi.status() == WL_CONNECTED) {
        return true;
    } else {
        return false;
    }
}

String getMacAddress() {
    return ("Mac Address: " + String(WiFi.macAddress()));
}

void connectTCP() {
    log("Baue TCP Verbindung auf");
    tcp.connect(Target_IP.c_str(), tcp_Target_Port);
    unsigned long start = millis();
    while(!tcp.connected()) {
        delay(10);
        if(millis() - start > 5000) {
            Serial.println("Unable to connect TCP! Aborting...");
            return;
        }
    }
}

void checkTCP() {
    if(!tcp.connected()) {
        connectTCP();
    }
    if(!tcp.connected()) {
        currentCtrlMode = OFF;
        showStatus("TCP Getrennt");
        removeStatus(5);
    }
}

void sendTCP(String type, String value) {
    checkTCP();
    tcp.println(type + ":" + value);
    log("Sende TCP. type: " + type + "; value: " + value);
}

void sendTCP(String type, int value) {
    checkTCP();
    tcp.println(type + ":" + value);
    log("Sending TCP. type: " + type + "; value: " + value);
}

void updateTCP() {
    sendTCP("mode",currentCtrlMode);
}

void sendUDP(ControlPacket packet) {
    udp.beginPacket(Target_IP.c_str(),udp_Target_Port);
    udp.write((uint8_t*)&packet, sizeof(packet));
    udp.endPacket();
}

void sendUDP(String data) {
    udp.beginPacket(Target_IP.c_str(),udp_Target_Port);
    udp.write((uint8_t*)&data,sizeof(data));
    udp.endPacket();
}

void sendMovementData(JoystickRaw raw, int currentMode) {
    ControlPacket packet = {(uint16_t)raw.x, (uint16_t)raw.y, (uint8_t)currentMode };
    sendUDP(packet);
}

void handleIncomingTCP() {
    checkTCP();
    while(tcp.available()) {
        String line = tcp.readStringUntil('\n');
        line.trim();
        if(line.startsWith("BATT:")) {
            battValue = line.substring(5).toFloat();
            log("Received BATT: " + String(battValue));
        } else if (line.startsWith("LENK:")) {
            lenkungValue = line.substring(5).toFloat();
            log("Received LENK: " + String(lenkungValue));
        }
    }
}

String handleRawTCP() {
    String line = "No TCP available.";
    checkTCP();
    if(tcp.available()) {
        line = tcp.readString();
        line.trim();
    }
    return line;
}