#include "wifi.hpp"

Wifi::Wifi()
{
    this->payload_base = "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
    this->payload = "";
    this->host = "script.google.com";
    this->httpPort = 443;
    this->fingerprint = "";
    this->url = String("/macro/s/") + GScriptId + "/exec";
    this->client = nullptr;
}

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
    WiFi.begin(ssid,password);
    //Holds the wifi until initialize
    while (WiFi.status() != WL_CONNECTED)
    { 
    }
    return true;
}

int Wifi::createTLSConnection()
{
    bool flag = false;
    int retval;


    this->client = new HTTPSRedirect(this->httpPort); 
    this->client-> setInsecure();
    this->client->setPrintResponseBody(true);
    this->client->setContentTypeHeader("application/json");
    // Try to connect for a maximum of 5 times
    for (int i=0; i<5; i++)
    { 
        retval = client->connect(this->host, this->httpPort);
        if (retval == 1)
        {
            flag = true;
            break;
        }
        else
        {
            return false;
        }
        if (!flag)
        {
            return false;
        }
        delete this->client;    
        this->client = nullptr; 
    }



    return true;
}