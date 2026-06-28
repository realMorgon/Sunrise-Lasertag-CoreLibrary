#include <Arduino.h>

#include "LaserTagCore.h"
#include "esp32-hal.h"

#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <cstdint>
#include <sys/types.h>

Preferences preferences;

String readSerialString() {
    while (!Serial.available()) {
        delay(10);
    }
    String input = Serial.readStringUntil('\n');
    input.trim();
    return input;
}

LasertagDevice::LasertagDevice(String serverIP, int port){

    _serverIP = serverIP;
    _port = port;

    preferences.begin("lasertag-wlan", false);

    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");

    if (ssid == "" || password == "") {
        Serial.println("\n=== Erstmaliges WLAN-Setup ===");

        Serial.print("Bitte gib die SSID (WLAN-Namen) ein: ");
        ssid = readSerialString();
        Serial.println("\nVerwende SSID: " + ssid); // Eingegebenen Namen zur Bestätigung ausgeben

        Serial.print("Bitte gib das WLAN-Passwort ein: ");
        password = readSerialString();
        Serial.println("Passwort gespeichert."); // Passwort nicht im Klartext loggen

        // Daten im Flash-Speicher sichern
        preferences.putString("ssid", ssid);
        preferences.putString("password", password);

        Serial.println("Zugangsdaten erfolgreich im NVS-Flash gespeichert!");
        Serial.println("Starte System neu...\n");
        preferences.end();
        ESP.restart(); // ESP32 neu starten, damit er jetzt normal bootet
    }

    preferences.end(); // Speicher schließen

    // Ab hier läuft der normale Verbindungsaufbau
    Serial.println("\n=== Normaler Bootvorgang ===");
    Serial.print("Verbinde mit gespeichertem WLAN: ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Verbinde mit WLAN");

    WiFi.macAddress(_mac);

    int attempts = 0;
    int maxAttempts = 50;

    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nMit WLAN verbunden.");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nZeitüberschreitung beim Verbinden mit WLAN!");
    }
}


void LasertagDevice::sync(const uint8_t type){

    int message_len = 6 + 1 + 1 + 4; // Mac, type, alignment_space, timestamp

    uint8_t message[message_len];

    memcpy(message, _mac, 6);

    message[6] = type;
    message[7] = 0x0;

    uint32_t currentTime = millis();
    message[8] = (currentTime & 0xFF);
    message[9] = (currentTime >> 8) & 0xFF;
    message[10] = (currentTime >> 16) & 0xFF;
    message[11] = (currentTime >> 24) & 0xFF;


    sendUDP(0x01, message, message_len);
}

void LasertagDevice::sendUDP(const uint8_t event_type, const uint8_t* message, const int messageSize){

    const int headerSize = 1 + 2 + 1 + 4; // event_type, packet_count, reserved, timestamp

    uint8_t header[headerSize];
    header[0] = event_type;
    header[1] = _packetCount & 0xFF;
    header[2] = _packetCount >> 8 & 0xFF;
    header[3] = 0x0 & 0xFF;

    header[4] = millis() & 0xFF;
    header[5] = millis() >> 8 & 0xFF;
    header[6] = millis() >> 16 & 0xFF;
    header[7] = millis() >> 24 & 0xFF;

    uint8_t* payload = new uint8_t[headerSize + messageSize];
    memcpy(payload, header, headerSize);
    memcpy(payload + headerSize, message, messageSize);

    _udp.beginPacket(_serverIP.c_str(), _port);
    _udp.write(payload, headerSize + messageSize);
    _udp.endPacket();

    _packetCount++;

    delete[] payload;
}
