#ifndef GasSensor_H
#define GasSensor_H

#include "Arduino.h"

class GasSensor
{
  int gasPin;
  int _averageConcentration;
  int _safeConcentration;
  float gasConcentration;

public:
  GasSensor(int pin, int averageConcentration, int safeConcentration);
  void begin();
  float readConcentration();
  void calibrate(int pin);
  String readSafety();
  void read(int pin);
};

#endif