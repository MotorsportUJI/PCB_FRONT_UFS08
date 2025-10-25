#include "OBDReader.h"

// --- Puntero estático para callback ---
static OBDReader *instancePtr = nullptr;

OBDReader::OBDReader() {

    can2 = new FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16>();

    responseMutex = xSemaphoreCreateBinary();
    xSemaphoreGive(responseMutex);
    dataMutex = xSemaphoreCreateMutex();
    instancePtr = this;
}

void OBDReader::begin(uint32_t baudrate) {
    can2->begin();
    can2->setBaudRate(baudrate);
    can2->enableFIFO();
    can2->onReceive(canISR);
    can2->setFIFOFilter(REJECT_ALL);
    can2->setMBFilter(REJECT_ALL);
    can2->enableFIFOInterrupt();
    can2->mailboxStatus();

    // Crear tarea de envío periódica
    xTaskCreate(taskSendPIDs, "OBD_Send", 4096, this, 3, NULL);
}

// --- Enviar un PID ---
void OBDReader::sendPID(uint8_t mode, uint8_t pid) {
    txMsg.id = 0x7DF;
    txMsg.len = 8;
    txMsg.buf[0] = 0x02;
    txMsg.buf[1] = mode;
    txMsg.buf[2] = pid;
    for (int i = 3; i < 8; i++) txMsg.buf[i] = 0;
    can2->write(txMsg);
}

// --- Callback de interrupción CAN ---
void OBDReader::canISR(const CAN_message_t &msg) {
    if (instancePtr == nullptr) return;
    BaseType_t yield = pdFALSE;
    if (xSemaphoreTakeFromISR(instancePtr->responseMutex, &yield) == pdTRUE) {
        instancePtr->handleResponse(msg);
        xSemaphoreGiveFromISR(instancePtr->responseMutex, &yield);
    }
    portYIELD_FROM_ISR(yield);
}

// --- Procesamiento de respuesta ---
void OBDReader::handleResponse(const CAN_message_t &msg) {
    if (!(msg.id >= 0x7E8 && msg.id <= 0x7EF)) return;
    if (msg.len < 4) return;
    if (msg.buf[1] != 0x41) return;

    uint8_t pid = msg.buf[2];
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    switch (pid) {
        case 0x0C: // RPM
            rpm = ((msg.buf[3] * 256) + msg.buf[4]) / 4.0;
            break;
        case 0x11: // Throttle
            throttle = (msg.buf[3] * 100.0) / 255.0;
            break;
        case 0x05: // Coolant Temp
            coolantTemp = msg.buf[3] - 40.0;
            break;
    }
    xSemaphoreGive(dataMutex);
}

// --- Tarea periódica de envío ---
void OBDReader::taskSendPIDs(void *pvParams) {
    OBDReader *self = static_cast<OBDReader *>(pvParams);

    const TickType_t delayTicks = pdMS_TO_TICKS(500); // cada 500 ms
    for (;;) {
        // Siempre pedir RPM
        self->sendPID(0x01, 0x0C);
        vTaskDelay(delayTicks);

        // Alternar entre throttle y coolant
        if (self->sendThrottleNext)
            self->sendPID(0x01, 0x11);
        else
            self->sendPID(0x01, 0x05);

        self->sendThrottleNext = !self->sendThrottleNext;

        vTaskDelay(delayTicks);
    }
}

// --- Getters protegidos ---
int OBDReader::getRPM() {
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    int val = rpm;
    xSemaphoreGive(dataMutex);
    return val;
}

uint8_t OBDReader::getThrottle() {
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    uint8_t val = throttle;
    xSemaphoreGive(dataMutex);
    return val;
}

uint8_t OBDReader::getCoolantTemp() {
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    float val = coolantTemp;
    xSemaphoreGive(dataMutex);
    return val;
}

// --- Leer códigos de error (modo bloqueante opcional) ---
void OBDReader::readDTCs() {
    sendPID(0x03, 0x00);
    uint32_t start = millis();
    CAN_message_t msg;
    Serial.println("Leyendo DTCs...");
    while (millis() - start < 300) {
        if (can2->read(msg)) {
            if (msg.id >= 0x7E8 && msg.id <= 0x7EF && msg.buf[1] == 0x43) {
                for (int i = 3; i < msg.len; i += 2) {
                    uint16_t code = (msg.buf[i] << 8) | msg.buf[i + 1];
                    if (code == 0) continue;
                    Serial.printf("DTC: %04X\n", code);
                }
            }
        }
    }
}
