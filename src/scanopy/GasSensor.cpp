#include "GasSensor.h"


GasSensor::GasSensor(int pin, int averageConcentration, int safeConcentration)
{
    gasPin = pin;
    _averageConcentration = averageConcentration;
    _safeConcentration = safeConcentration;
}

void GasSensor::begin()
{
    pinMode(gasPin, INPUT_PULLUP);
    calibrate(gasPin);
}

// Read concentration of GasSensor in ppm
float GasSensor::readConcentration()
{
    read(gasPin);

    return gasConcentration;
}

// Read safety status of gas concentration
String GasSensor::readSafety()
{

    String safetySatus;

    read(gasPin);

    if (gasConcentration <= _safeConcentration)
    {
        return "SAFE";
    }
    else if (gasConcentration > _safeConcentration && gasConcentration <= _averageConcentration)
    {
        return "AVERAGE";
    }
    else
    {
        return "DANGEROUS";
    }
}

//Do calibration
void GasSensor::calibrate(int pin)
{
}
// Do the gas reading in ppm
void GasSensor::read(int pin)
{
    // do the gas reading, set the concentration

    gasConcentration = 0;
}
