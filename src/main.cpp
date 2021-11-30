#include <Arduino.h>
#include "dht11.hpp"
#include "rain.hpp"
#include "mag.hpp"
#include "wifi.hpp"
#include <ESP8266HTTPClient.h>
#include "WiFiClient.h"
#include "ESP8266WiFi.h"
#include "Wire.h"
//#include "encoder.hpp"


#define DHTPIN 14        				// Pino do DHT 11 em D5
#define DHTTYPE DHT11					// Tipo do produto
#define PluviINTERRUPT 13 				// D7 - Pino de interrupção da báscula
#define pinoChuva 14 					// D5 Pino digital do sensor de chuva
DHT dht(DHTPIN,DHTTYPE);
DHTinfo dhtinfo;
SensorRain sensorrain;
Magnetometer magnetometer;
// Enter network credentials:
const char* ssid     = "Dalzira";
const char* password = "96261194";
//Define pinos
float pulsePluvi = 0;
//MPU6050
const int MPU = 0x68;
int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;



void Gyro(void)
{
	Wire.begin();
	Wire.write(0);
	Wire.endTransmission(true);
}

void Gyro_Read(void)
{
	Wire.beginTransmission(MPU);
	Wire.write(0x3B);
	Wire.endTransmission(false);
	Wire.requestFrom(MPU,14,true);
	AcX = Wire.read() <<8 | Wire.read();     
	AcY = Wire.read() <<8 | Wire.read();
	AcZ = Wire.read() <<8 | Wire.read();
	Tmp = Wire.read() <<8 | Wire.read();
	GyX = Wire.read() <<8 | Wire.read();
	GyY = Wire.read() <<8 | Wire.read();
	GyZ = Wire.read() <<8 | Wire.read();
}




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
	String GAS_ID = "AKfycbwSfoM2Bc0RX7-2aNFsUpTvurlR0Y4hFj43IndotJ4IqmVnvfVpFc6R-t7I9xZ9j3cT";     // ScriptID
	const int httpsPort = 443;
	int param_1 = 1;			// temperature
	int param_2 = 2;			// humidity
	int param_3 = 3;														// Port
	String parameters = "teste1=" + String(dhtinfo.temperature) + "&teste2=" + String(dhtinfo.Humidity) + "&teste3=" + String(param_3);
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

void ICACHE_RAM_ATTR interrupcao()
{
	if(pinoChuva == LOW)
	{
		pulsePluvi++;
	}
	delayMicroseconds(100);
}

void setup() 
{
	attachInterrupt(digitalPinToInterrupt(PluviINTERRUPT), interrupcao, FALLING); //Interrupção
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
	bool itsRaining = false;
	dhtinfo = dht_read();
	if (isnan(dhtinfo.temperature) || isnan(dhtinfo.Humidity))
	{
		Serial.println("Erro Leitura");
	}
	
	if (digitalRead(pinoChuva) == LOW)
	{
		itsRaining = true; 
	}
	Gyro_Read();
	POST();
	delay(500);
}