#include <logging.h>
#include <global.h>

Logging::Logging() {}

void Logging::info(String text) {
        Serial.println("[INFO] " + text);
    }

void Logging::error(String text) {
    Serial.println("[ERROR] " + text);
}

void Logging::debug(String text) {
    if(advancedLog) Serial.println("[DEBUG] " + text);
}