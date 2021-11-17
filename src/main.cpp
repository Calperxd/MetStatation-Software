#include <Arduino.h>
#include "dht11.hpp"
#include "rain.hpp"
#include "mag.hpp"
//#include "encoder.hpp"


#define DHTPIN 14        				// Pino do DHT 11 em D5
#define DHTTYPE DHT11					// Tipo do produto
DHT dht(DHTPIN,DHTTYPE);
DHTinfo dhtinfo;
SensorRain sensorrain;
Magnetometer magnetometer;




DHTinfo dht_read()
{
	dhtinfo.Humidity = dht.readHumidity();
	dhtinfo.temperature = dht.readTemperature();
	return dhtinfo;
}

void setup() 
{
	Serial.begin(9600);				//Baud Rate UART
	dht.begin();					//Start dht
}

void loop() 
{
	dhtinfo = dht_read();
	Serial.print(sensorrain.GetSensorReading());
	delay(500);
}