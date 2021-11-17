#include "wifi.hpp"

Wifi::Wifi(char* ssid, char* password, char* scriptid)
{
    this->ssid = ssid;
    this->password = password;
    this->GScriptId = scriptid;
    this->payload_base = "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
    this->payload = "";
    this->host = "script.google.com";
    this->httpPort = 443;
    this->fingerprint = "";
    this->url = String("/macro/s/") + GScriptId + "/exec";
    this->client = nullptr;
}

Wifi::~Wifi()
{

}

int Wifi::start(char* ssid, char* password)
{
    Wifi.begin(ssid,password);
    //Holds the wifi until initialize
    while (WiFi.status() != WL_CONNECTED)
    { 
    }
    return true;
}