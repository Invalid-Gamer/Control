#include <Arduino.h>
#include <global.h>
#include <WiFi.h>
#include <DisplayMgr.h>
#include <config.h>
#include <communication.h>

bool setupWiFi() {
    // Display: Verbinde WLAN
    if (advancedLog){Serial.println("WiFi: Using Credentials: "); Serial.println("SSID:" + WiFi_SSID + "; Pass: " + WiFi_Pass);}
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

