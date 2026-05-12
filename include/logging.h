#ifndef logging_h
#define logging_h
#include <Arduino.h>

class Logging {
    public:
        Logging();
        void info(String text);
        void error(String text);
        void debug(String text);
};

#endif