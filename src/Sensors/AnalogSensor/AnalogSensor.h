#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

#include <Arduino.h>

class AnalogSensor {
  private:
    uint8_t pin;
    uint16_t resolution;
    long inMin, inMax;
    long outMin, outMax;

  public:
    AnalogSensor(uint8_t pin,
                 uint16_t resolution = 12,
                 long inMin = 0,
                 long inMax = 4095,
                 long outMin = 0,
                 long outMax = 1023);
    void begin();
    int readRaw();
    int read();
};

#endif
