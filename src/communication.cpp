#include <Arduino.h>
#include <global.h>
#include <WiFi.h>
#include <DisplayMgr.h>
#include <config.h>

bool setupWiFi() {
    // Display: Verbinde WLAN
    WiFi.begin(WiFi_SSID.c_str(),WiFi_Pass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        if(millis() - start > 20000) {

        }
    }
}

