#ifndef HALL_SENSOR_H
#define HALL_SENSOR_H

#include <Arduino.h>

class HallSensor {
private:
    static HallSensor *instance; // puntero estático a la instancia
    uint8_t pin;
    volatile unsigned long pulseCount;
    unsigned long lastTime;
    float pulsesPerRevolution;

    static void handlePulseISR(); // ISR estática compatible con Teensy

public:
    HallSensor(uint8_t pin, float pulsesPerRev = 1.0);

    void begin();
    void resetCount();

    unsigned long getPulseCount();
    float getFrequency(); // Hz
    int getRPM();       // RPM
};

#endif
