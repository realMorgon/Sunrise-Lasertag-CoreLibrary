#ifndef LASERTAG_CORE_H
#define LASERTAG_CORE_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <cstdint>

String readSerialString();

class LasertagDevice {
private:
    WiFiUDP _udp;

    uint8_t _UUID[16];

    String _serverIP;
    int _port;

    int _packetCount;

public:
    LasertagDevice(uint8_t UUID[16], String serverIP, int port);

    bool connectWifi();
    void sync(const uint8_t type);

    void sendUDP(const uint8_t event_type, const uint8_t* message, const int messageSize);
};

enum LasertagEvent : uint8_t {
    // Out of game events
    SYNC = 0x01,
    // In game events
    SHOOT = 0x11,
    GOT_HIT = 0x12
};

enum LasertagDeviceType : uint8_t {
    PHASER = 0x31,
    VEST = 0x32
};

#endif
