#pragma once
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "HTTPSRedirect.h"


class Wifi
{
private:
    static const char* ssid;       // ssid
    static const char* password;   // Password
    static const char* GScriptId;  // Script ID
    static const char* payload_base;
    static const char* payload;
    static const char* host;
    static const int httpPort;
    static const char* fingerprint;
    static String url;
    static const HTTPSRedirect* client;
public:
    Wifi(/* args */);
    ~Wifi();
    int start(char* ssid, char* password);
};