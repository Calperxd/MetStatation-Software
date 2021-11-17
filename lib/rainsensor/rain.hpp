#pragma once
#include "Arduino.h"



class SensorRain
{
private:
    static const int sensorMin = 0;             // Min Value of sensor
    static const int sensorMax = 1024;          // Max Value of sensor
    static const int analogInPin = A0;          // Pin of DHT
public:
    int GetSensorReading();
    SensorRain(/* args */);
    ~SensorRain();
};

