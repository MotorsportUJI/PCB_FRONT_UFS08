#include "HallSensor.h"

HallSensor *HallSensor::instance = nullptr;

HallSensor::HallSensor(uint8_t pin, float pulsesPerRev)
    : pin(pin), pulseCount(0), lastTime(0), pulsesPerRevolution(pulsesPerRev) {}

void HallSensor::handlePulseISR() {
    if (instance) instance->pulseCount++;
}

void HallSensor::begin() {
    instance = this; // guardamos la instancia actual (solo una soportada)
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin), handlePulseISR, RISING);
    lastTime = millis();
}

void HallSensor::resetCount() {
    pulseCount = 0;
    lastTime = millis();
}

unsigned long HallSensor::getPulseCount() {
    return pulseCount;
}

float HallSensor::getFrequency() {
    static unsigned long lastCheck = 0;
    unsigned long now = millis();
    unsigned long elapsed = now - lastCheck;

    if (elapsed < 100) return 0; // evitar cálculos demasiado seguidos

    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();

    lastCheck = now;
    float freq = (float)count / (elapsed / 1000.0);
    return freq;
}

int HallSensor::getRPM() {
    float freq = getFrequency();
    return static_cast<int>((freq / pulsesPerRevolution) * 60.0);
}
