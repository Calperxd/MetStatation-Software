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
#define PluviINTERRUPT 12				// D6 - Pino de interrupção da báscula
#define pinoChuva 13 					//D7
#define DHTTYPE DHT11					// Tipo do produto



DHT dht(DHTPIN,DHTTYPE);
DHTinfo dhtinfo;
SensorRain sensorrain;
Magnetometer magnetometer;
// Enter network credentials:
const char* ssid     = "JS";
const char* password = "dhcp8hgkg";

float pulsePluvi = 0;


//MPU6050
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int minVal=265;
int maxVal=402;
double x;
double y;
double z;


void ICACHE_RAM_ATTR interrupcao()
{
	Serial.println("Interrupcao");
	Serial.println(pulsePluvi);

	if(digitalRead(pinoChuva) == LOW)
	{
		Serial.println("Choveu");
		pulsePluvi++;
	}
	delayMicroseconds(100);
}

void Gyro(void)
{
	Wire.begin(D1,D2);
	Wire.beginTransmission(MPU_addr);
	Wire.write(0x6B);
	Wire.write(0);
	Wire.endTransmission(true);
	delay(1000);
}

void Gyro_Read(void)
{
	Wire.beginTransmission(MPU_addr);
	Wire.write(0x3B);
	Wire.endTransmission(false);
	Wire.requestFrom(MPU_addr,14,true);

	AcX=Wire.read()<<8|Wire.read();
	AcY=Wire.read()<<8|Wire.read();
	AcZ=Wire.read()<<8|Wire.read();

	int xAng = map(AcX,minVal,maxVal,-90,90);
	int yAng = map(AcY,minVal,maxVal,-90,90);
	int zAng = map(AcZ,minVal,maxVal,-90,90);
	
	x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
	y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
	z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

	Serial.print("AngleX= ");
	Serial.println(x);
	
	Serial.print("AngleY= ");
	Serial.println(y);
	
	Serial.print("AngleZ= ");
	Serial.println(z);
	Serial.println("-----------------------------------------");
    delay(400);
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
	String GAS_ID = "AKfycby_WjFzErWMlZn6jWav-_IVhY3ZGUsRxAT_tDUko-RbznWqtz-fajY141gRUbgSVHAk";     // ScriptID
	const int httpsPort = 443;
	String parameters = "teste_1=" + String(dhtinfo.temperature) + "&teste_2=" + String(dhtinfo.Humidity) + "&teste_3=" + String(pulsePluvi/2*0.125) + "&teste_4=" + String(x) + "&teste_5=" + String(y) + "&teste_6=" + String(z);
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

void wifi()
{
	Serial.begin(9600);				//Baud Rate UART
	WiFi.begin(ssid, password);
	Serial.println("Connecting");
	while (WiFi.status() != WL_CONNECTED) 
	{
		delay(1000);
		Serial.print(".");
	}
}
void pluvi()
{
	attachInterrupt(digitalPinToInterrupt(PluviINTERRUPT), interrupcao, FALLING);
}

void setup() 
{
	wifi();
	dht.begin();
	Gyro();
	pluvi();
}

void loop() 
{
	bool itsRaining = false;
	dhtinfo = dht_read();
	if (isnan(dhtinfo.temperature) || isnan(dhtinfo.Humidity))
	{
		Serial.println("Erro Leitura");
	}
	else
	{
		Serial.println(dhtinfo.temperature);
		Serial.println(dhtinfo.Humidity);
	}
	if (digitalRead(pinoChuva) == LOW)
	{
		itsRaining = true; 
	}
	Gyro_Read();
	POST();
	delay(500);
}