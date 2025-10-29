#ifndef OBDREADER_H
#define OBDREADER_H

#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <FreeRTOS.h>
#include <semphr.h>

class OBDReader {
public:
    OBDReader();
    void begin(uint32_t baudrate = 500000);

    // Valores actualizados
    int getRPM();
    uint8_t getThrottle();
    uint8_t getCoolantTemp();

    // Lectura de errores manual
    void readDTCs();

private:
    FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> *can2;
    CAN_message_t txMsg;

    // Sincronización y datos
    SemaphoreHandle_t dataMutex;
    SemaphoreHandle_t responseMutex;

    // Variables de datos
    volatile int rpm = -1;
    volatile uint8_t throttle = -1;
    volatile uint8_t coolantTemp = -1;

    // Control del ciclo de envío
    bool sendThrottleNext = true;

    // Tarea de envío periódica
    static void taskSendPIDs(void *pvParams);

    // Función de callback de recepción CAN
    static void canISR(const CAN_message_t &msg);

    // Funciones auxiliares
    void sendPID(uint8_t mode, uint8_t pid);
    void handleResponse(const CAN_message_t &msg);
};

#endif
