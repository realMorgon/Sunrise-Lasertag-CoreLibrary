#include "LaserTagCore.h"

#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>

int sync(String serverIP, int port, const String type){

    String url = "http://" + String(serverIP) + ":" + String(port) + "/api" + "/sync?time=" + String(millis()) + "&type=" + String(type);
    return sendHTTP(url).body.toInt();

}

HTTPResponse sendHTTP(String url){
    HTTPClient http;
    HTTPResponse res;

    http.begin(url);

    res.code = http.POST("");

    if (res.code == 200) {
        res.body = http.getString();
        Serial.println("Antwort: " + res.body);
    }

    http.end();
    return res;
}

bool connectWifi(){

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
        return true;
    } else {
        Serial.println("\nZeitüberschreitung beim Verbinden mit WLAN!");
        return false;
    }
}
