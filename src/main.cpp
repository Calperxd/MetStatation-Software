#include <Arduino.h>
#include "dht11.hpp"


#define DHTPIN 14        				// Pino do DHT 11 em D5
#define DHTTYPE DHT11					// Tipo do produto
DHT dht(DHTPIN,DHTTYPE);

DHTinfo dhtinfo;
const int analogInPin = A0;
int sensorValue = 0;



DHTinfo dht_read()
{
	dhtinfo.Humidity = dht.readHumidity();
	dhtinfo.temperature = dht.readTemperature();
	return dhtinfo;
}

int rainSensor_read()
{
	sensorValue = analogRead(analogInPin);
	return 0;
}

void setup() 
{
	Serial.begin(9600);				//Baud Rate UART
	dht.begin();					//Start UART
}

void loop() 
{
	dhtinfo = dht_read();
	Serial.print(dhtinfo.temperature);
	Serial.print(dhtinfo.Humidity);
	delay(500);
}