#include "LaserTagCore.h"

#include <WiFi.h>
#include <HTTPClient.h>

int sync(char* serverIP, int port){

  String url = "http://" + String(serverIP) + ":" + String(port) + "/api" + "/sync?time=" + String(millis());
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

bool connectWifi(char* ssid, char* password){

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wifi");

  int attempts = 0;
  int maxAttempts = 20;

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
  if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to Gameserver via WiFi");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println("\nTimeout connecting to Network!");
        return false;
    }
}