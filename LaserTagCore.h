#ifndef LASERTAG_CORE_H
#define LASERTAG_CORE_H

#include <WiFi.h>
#include <HTTPClient.h>

struct HTTPResponse {
    int code;
    String body;
};

bool connectWifi();
int sync(String serverIP, int port, const String type);
HTTPResponse sendHTTP(String url);

#endif
