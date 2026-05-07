#include <logging.h>

void Logging::info(String text) {
        Serial.println("[INFO] " + text);
    }

void Logging::error(String text) {
    Serial.println("[ERROR] " + text);
}

void Logging::debug(String text) {
    if(_debug) Serial.println("[DEBUG] " + text);
}