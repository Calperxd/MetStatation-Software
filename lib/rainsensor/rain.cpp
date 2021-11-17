#include "rain.hpp"


SensorRain::SensorRain(/* args */)
{
}

SensorRain::~SensorRain()
{
}

int SensorRain::GetSensorReading()
{
    return analogRead(this->analogInPin);
}