#include <Arduino.h>
#include "dht11.hpp"
#include "rain.hpp"
#include "mag.hpp"
#include "wifi.hpp"
//#include "encoder.hpp"


#define DHTPIN 14        				// Pino do DHT 11 em D5
#define DHTTYPE DHT11					// Tipo do produto
DHT dht(DHTPIN,DHTTYPE);
DHTinfo dhtinfo;
SensorRain sensorrain;
Magnetometer magnetometer;
// Enter network credentials:
const char* ssid     = "legal";
const char* password = "senha123";




DHTinfo dht_read()
{
	dhtinfo.Humidity = dht.readHumidity();
	dhtinfo.temperature = dht.readTemperature();
	return dhtinfo;
}

void setup() 
{
	bool ret;
	Serial.begin(9600);				//Baud Rate UART
	dht.begin();					//Start dht
	int n = WiFi.scanNetworks();
	for (int i = 0; i < n; i++)
	{
    	Serial.println(WiFi.SSID(i));
	}

	ret = WiFi.begin(ssid, password);
	Serial.println("Connecting");
	while (WiFi.status() != WL_CONNECTED) 
	{
		delay(1000);
		Serial.print(".");
	}
	Serial.println(WiFi.status());
	Wifi wifi;


}

void loop() 
{
	dhtinfo = dht_read();
	Serial.println(dhtinfo.temperature);
	//Serial.print(sensorrain.GetSensorReading());
	delay(500);
}