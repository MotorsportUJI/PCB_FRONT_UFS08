#include "AnalogSensor.h"
#include <Arduino.h>
#include <math.h> // Para pow()

// ---------------------------
// Constructor
// ---------------------------
AnalogSensor::AnalogSensor(uint8_t pin, uint16_t resolution, long inMin, long inMax, long outMin, long outMax)
  : pin(pin), resolution(resolution), inMin(inMin), inMax(inMax), outMin(outMin), outMax(outMax) {}


// ---------------------------
// Inicialización
// ---------------------------
void AnalogSensor::begin() {
  pinMode(pin, INPUT);
  analogReadResolution(resolution);
}

// ---------------------------
// Lecturas
// ---------------------------
int AnalogSensor::readRaw() {
  return analogRead(pin);
}


int AnalogSensor::read() {
  int raw = analogRead(pin);
  // Mapea al rango definido por outMin y outMax
  return map(raw, inMin, inMax, outMin, outMax);
}
