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
int velValue = 0;


bool setupWiFi() {
    showStatus("Verbinde WLAN... ");
    WiFi.setHostname(Device_Name.c_str());
    if (advancedLog){Serial.println("WiFi: Using Credentials: "); Serial.println("SSID:" + WiFi_SSID + "; Pass: " + WiFi_Pass + "; Current Hostname: " + String(WiFi.getHostname()));}
    WiFi.begin(WiFi_SSID.c_str(),WiFi_Pass.c_str());
    unsigned long start = millis();
    if(advancedLog){Serial.println("Waiting for connection at: " + String(start));}
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        if(millis() - start > 20000) {
            if(advancedLog){Serial.println("WiFi Connection timed out!"); Serial.println("Waited for: " + String(millis() - start));}
            return false;
        }
    }
    if(advancedLog){Serial.println("WiFi successfully connected after: " + String(millis() - start));}
    showStatus("WiFi connected");
    removeStatus(1);
    return true;
}

void disconnectWiFi() {
    if(advancedLog){Serial.println("WiFi disconnected");}
    WiFi.disconnect();
}

String getWiFiStatus() {
    switch (WiFi.status()) { // KI generiert
        case WL_CONNECTED:  {     
        String info = "Verbunden!";
        info += "\n IP: " + WiFi.localIP().toString();
        info += "\n Signal: " + String(WiFi.RSSI()) + "dBm";
        info += "\n Hostname: " + String(WiFi.getHostname());
        return info;
        }
        case WL_NO_SSID_AVAIL:   return "SSID nicht gefunden (WL_NO_SSID_AVAIL)";
        case WL_CONNECT_FAILED:  return "Verbindung fehlgeschlagen (WL_CONNECT_FAILED)";
        case WL_IDLE_STATUS:     return "Leerlauf (WL_IDLE_STATUS)";
        case WL_DISCONNECTED:    return "Getrennt (WL_DISCONNECTED)";
        default:                 return "Unbekannter Status (" + String(WiFi.status()) + ")";
    }
}

void connectTCP() {
    if(advancedLog){Serial.println("Baue TCP Verbindung auf");}
    tcp.connect(Target_IP.c_str(), tcp_Target_Port);
    unsigned long start = millis();
    while(!tcp.connected()) {
        delay(10);
        if(millis() - start > 2000) {
            Serial.println("TCP Verbingung hakt!");
        }
        if(millis() - start > 5000) {
            Serial.println("TCP Verbindungsaufbau nicht möglich! Abbruch...");
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
        showStatus("TCP Disconnected");
        removeStatus(5);
    }
}

void sendTCP(String type, String value) {
    checkTCP();
    tcp.println(type + ":" + value);
    if(advancedLog){Serial.println("Sende TCP. type: " + type + "; value: " + value);}
}

void sendTCP(String type, int value) {
    checkTCP();
    tcp.println(type + ":" + value);
    if(advancedLog){Serial.println("Sende TCP. type: " + type + "; value: " + value);}
}

void updateTCP() {
    sendTCP("mode",currentCtrlMode);
}

void sendMovementData(JoystickRaw raw, int currentMode) {
    ControlPacket packet = {(uint16_t)raw.x, (uint16_t)raw.y, (uint8_t)currentMode };
    udp.beginPacket(Target_IP.c_str(), udp_Target_Port);
    udp.write((uint8_t*)&packet, sizeof(packet));
    udp.endPacket();
}

void handleIncomingTCP() {
    checkTCP();
    while(tcp.available()) {
        String line = tcp.readStringUntil('\n');
        line.trim();
        if(line.startsWith("BATT:")) {
            battValue = line.substring(5).toFloat();
            if(advancedLog){Serial.println("Empfangen BATT: " + String(battValue));}
        } else if (line.startsWith("VEL:")) {
            velValue = line.substring(4).toInt();
            if(advancedLog) Serial.println("Empfangen VEL: " + String(velValue));
        }
    }
}