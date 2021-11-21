#include <Arduino.h>
#include "dht11.hpp"
#include "rain.hpp"
#include "mag.hpp"
#include "wifi.hpp"
#include "HTTPSRedirect.h"
//#include "encoder.hpp"


#define DHTPIN 14        				// Pino do DHT 11 em D5
#define DHTTYPE DHT11					// Tipo do produto
DHT dht(DHTPIN,DHTTYPE);
DHTinfo dhtinfo;
SensorRain sensorrain;
Magnetometer magnetometer;
// Enter network credentials:
const char* ssid     = "Omega-D87F";
const char* password = "12345678";


//Google docs
const char* host = "script.google.com";
const char *GScriptId = "AKfycbzYw5G-oxvnwHpAJfDsS0PWNrO0KTBMiCW78lHUcEO6ZnFHvSw";
const int httpsPort = 443;
const char* fingerprint = "";
// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec?value=Hello";
// Fetch Google Calendar events for 1 week ahead
String url2 = String("/macros/s/") + GScriptId + "/exec?cal";
// Read from Google Spreadsheet
String url3 = String("/macros/s/") + GScriptId + "/exec?read";
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"Sheet1\", \
                    \"values\": ";
String payload = "";
HTTPSRedirect* client = nullptr;
// used to store the values of free stack and heap
// before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets
// upon instantiation
unsigned int free_heap_before = 0;
unsigned int free_stack_before = 0;




DHTinfo dht_read()
{
	dhtinfo.Humidity = dht.readHumidity();
	dhtinfo.temperature = dht.readTemperature();
	return dhtinfo;
}

void setup() 
{
	//bool ret;
	bool flag = false;
	Serial.begin(9600);				//Baud Rate UART
	dht.begin();					//Start dht
	
	ret = WiFi.begin(ssid, password);
	Serial.println("Connecting");
	while (WiFi.status() != WL_CONNECTED) 
	{
		delay(1000);
		Serial.print(".");
	}
	// To free the heap 
	free_heap_before = ESP.getFreeHeap();
  	free_stack_before = ESP.getFreeContStack();
	// Use HTTPSRedirect class to create a new TLS connection
	client = new HTTPSRedirect(httpsPort);
	client->setInsecure();
	client->setPrintResponseBody(true);
	client->setContentTypeHeader("application/json");
	// Try to connect for a maximum of 5 times
	for (int i=0; i<5; i++)
	{
		int retval = client->connect(host, httpsPort);
		if (retval == 1) 
		{
			flag = true;
			break;
		}
		else
			Serial.println("Connection failed. Retrying...");
	}

	if (!flag)
	{
		Serial.print("Could not connect to server: ");
		Serial.println(host);
		Serial.println("Exiting...");
		return;
	}
  	// Send memory data to Google Sheets
	payload = payload_base + "\"" + free_heap_before + "," + free_stack_before + "\"}";
	client->POST(url2, host, payload, false);
	payload = payload_base + "\"" + ESP.getFreeHeap() + "," + ESP.getFreeContStack() + "\"}";
	client->POST(url2, host, payload, false);

	// Note: setup() must finish within approx. 1s, or the the watchdog timer
	// will reset the chip. Hence don't put too many requests in setup()
	// ref: https://github.com/esp8266/Arduino/issues/34
	// fetch spreadsheet data
	client->GET(url, host);

	// Send memory data to Google Sheets
	payload = payload_base + "\"" + ESP.getFreeHeap() + "," + ESP.getFreeContStack() + "\"}";
	client->POST(url2, host, payload, false);
	// fetch spreadsheet data
	client->GET(url2, host);
	// Send memory data to Google Sheets
	payload = payload_base + "\"" + ESP.getFreeHeap() + "," + ESP.getFreeContStack() + "\"}";
	client->POST(url2, host, payload, false);
	Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
	Serial.printf("Free stack: %u\n", ESP.getFreeContStack());
	delete client;
	client = nullptr;

}

void loop() 
{
	dhtinfo = dht_read();
	if (isnan(dhtinfo.temperature) || isnan(dhtinfo.Humidity)){
		Serial.println("ERRO");
	}
	else{
		Serial.print("Temperatura: ");
		Serial.println(dhtinfo.temperature);
		Serial.print("Umidade: ");
		Serial.println(dhtinfo.Humidity);
	//Serial.print(sensorrain.GetSensorReading());
	}
	delay(500);
}