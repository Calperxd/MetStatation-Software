#pragma once
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "HTTPSRedirect.h"


class Wifi
{
private:
    String ssid;       // ssid
    String password;   // Password
    String GScriptId;  // Script ID
    String payload_base;
    String payload;
    String host;
    int httpPort;
    String fingerprint;
    String url;
    HTTPSRedirect* client;
public:
    Wifi();
    Wifi(char* ssid, char* password, char* scriptid);
    ~Wifi();
    int start(char* ssid, char* password);
    int createTLSConnection();


};