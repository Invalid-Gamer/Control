#include <Arduino.h>
#include <global.h>
#include <WiFi.h>
#include <DisplayMgr.h>
#include <config.h>
#include <communication.h>

bool setupWiFi() {
    // Display: Verbinde WLAN
    WiFi.setHostname(Device_Name.c_str());
    if (advancedLog){Serial.println("WiFi: Using Credentials: "); Serial.println("SSID:" + WiFi_SSID + "; Pass: " + WiFi_Pass + "Current Hostname: " + String(WiFi.getHostname()));}
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