#ifndef network_H
#define network_H
#include <communication.h>
#include <WiFi.h>
#include <global.h>
#include <WiFiUdp.h>
#include <Arduino.h>

class NetworkMgr {
    public:
        NetworkMgr();
        bool begin();
        void disconnect();
        bool isConnected();
        String getStatus();
        String getMacAddress();

        void sendMovement(JoystickRaw raw);
        void updateTCP();
        void handleIncomingTCP();
        String handleRawTCP();
        // Nur public wegen shell
        void sendTCP(String type, String value);
        void sendTCP(String type, int value);
        void sendUDP(ControlPacket packet);
        void sendUDP(String value);

    private:
        WiFiUDP _udp;
        WiFiClient _tcp;

        void _checkTCP();
        void _connectTCP();
};
extern NetworkMgr network;

#endif