#include <Arduino.h>
#include "dht11.hpp"
#include "rain.hpp"
#include "mag.hpp"
#include "wifi.hpp"
#include <ESP8266HTTPClient.h>
#include "WiFiClient.h"
#include "ESP8266WiFi.h"
//#include "encoder.hpp"


#define DHTPIN 14        				// Pino do DHT 11 em D5
#define DHTTYPE DHT11					// Tipo do produto
DHT dht(DHTPIN,DHTTYPE);
DHTinfo dhtinfo;
SensorRain sensorrain;
Magnetometer magnetometer;
// Enter network credentials:
const char* ssid     = "Dalzira";
const char* password = "96261194";






DHTinfo dht_read()
{
	dhtinfo.Humidity = dht.readHumidity();
	dhtinfo.temperature = dht.readTemperature();
	return dhtinfo;
}

void POST(void)
{
	WiFiClientSecure client;
	const char* host = "script.google.com";															// Google Host
	const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";		// Sha1
	String GAS_ID = "AKfycbwRHur_AFGoLR_USAhtTLwx1h6KdwsXXdJkdjPAfUmJV2NBLhvxiWHlT5Ts0ydN013-";     // ScriptID
	const int httpsPort = 443;
	int param_1 = 1;			// temperature
	int param_2 = 2;			// humidity
	int param_3 = 3;														// Port
	String parameters = "temp=" + String(dhtinfo.temperature) + "hum=" + String(dhtinfo.Humidity) + "&teste3=" + String(param_3);
	client.setInsecure();
	if (!client.connect(host, httpsPort)) 
	{
    	Serial.println("connection failed");
    	return;
	}
	String url = "/macros/s/" + GAS_ID + "/exec?" + parameters;
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
			"Host: " + host + "\r\n" +
			"User-Agent: BuildFailureDetectorESP8266\r\n" +
			"Connection: close\r\n\r\n");
	while (client.connected()) 
	{
		String line = client.readStringUntil('\n');
		if (line == "\r") 
		{
			Serial.println("headers received");
			break;
		}
	}
	String line = client.readStringUntil('\n');
	if (line.startsWith("{\"state\":\"success\"")) 
	{
		Serial.println("CI successfull!");
	} else 
	{
		Serial.println("CI has failed");
	}
}

void setup() 
{
	bool ret;
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
}

void loop() 
{
	dhtinfo = dht_read();
	if (isnan(dhtinfo.temperature) || isnan(dhtinfo.Humidity))
	{
		Serial.println("Erro Leitura");
	}
	else
	{
		POST();
		Serial.println(dhtinfo.temperature);
		Serial.println(dhtinfo.Humidity);
	//Serial.print(sensorrain.GetSensorReading());
	}

	delay(500);
}