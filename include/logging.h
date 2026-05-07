#ifndef logging_h
#define logging_h
#include <Arduino.h>

class Logging {
    public:
        Logging(bool debugLog) : _debug(debugLog) {}
        void info(String text);
        void error(String text);
        void debug(String text);

    private:
        bool _debug;
};

#endif