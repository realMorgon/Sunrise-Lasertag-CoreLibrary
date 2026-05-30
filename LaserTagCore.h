#ifndef LASERTAG_CORE_H
#define LASERTAG_CORE_H

#include <WiFi.h>
#include <HTTPClient.h>

struct HTTPResponse {
    int code;
    String body;
};

bool connectWifi(char* ssid, char* password);
int sync(char* serverIP, int port, const char* type);
HTTPResponse sendHTTP(String url);

#endif