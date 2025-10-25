#include "CANbus.h"

CANread *CANread::instance_ = nullptr;

CANread::CANread(uint32_t baudrate) : baudrate_(baudrate) {
    dataLock_ = xSemaphoreCreateBinary();
    if (dataLock_ != nullptr) {
        xSemaphoreGive(dataLock_);
    }
}

bool CANread::begin() {
    if (initialized_ || dataLock_ == nullptr) {
        return initialized_;
    }

    instance_ = this;

    canBus_.begin();
    canBus_.setBaudRate(baudrate_);
    canBus_.enableFIFO();
    canBus_.setFIFOFilter(REJECT_ALL);
    canBus_.setFIFOFilter(0, ID_BACK_RIGHT_TEMPS, STD);
    canBus_.setFIFOFilter(1, ID_BACK_RIGHT_GAL, STD);
    canBus_.setFIFOFilter(2, ID_REAR_RIGHT_HALL, STD);
    canBus_.setFIFOFilter(3, ID_BACK_LEFT_TEMPS, STD);
    canBus_.setFIFOFilter(4, ID_BACK_LEFT_PRESS, STD);
    canBus_.setFIFOFilter(5, ID_INERTIAL, STD);
    canBus_.onReceive(isrHandler);
    canBus_.enableFIFOInterrupt();
    canBus_.mailboxStatus();

    initialized_ = true;
    return true;
}

void CANread::end() {
    if (!initialized_) return;
    canBus_.disableFIFOInterrupt();
    initialized_ = false;
    instance_ = nullptr;
}

void CANread::sendPressureBreak(uint8_t pressure) {
    if (!initialized_) return;

    CAN_message_t msg;
    msg.id = 0x1;
    msg.len = 1;
    msg.buf[0] = pressure;

    canBus_.write(msg);
}


BackTemps CANread::getBackTemps() {
    BackTemps copy;
    if (dataLock_ && xSemaphoreTake(dataLock_, portMAX_DELAY) == pdTRUE) {
        copy = backTemps_;
        xSemaphoreGive(dataLock_);
    }
    return copy;
}

BackGal CANread::getBackGal() {
    BackGal copy;
    if (dataLock_ && xSemaphoreTake(dataLock_, portMAX_DELAY) == pdTRUE) {
        copy = backGal_;
        xSemaphoreGive(dataLock_);
    }
    return copy;
}

RearHall CANread::getRearHall() {
    RearHall copy;
    if (dataLock_ && xSemaphoreTake(dataLock_, portMAX_DELAY) == pdTRUE) {
        copy = rearHall_;
        xSemaphoreGive(dataLock_);
    }
    return copy;
}


BackPressures CANread::getBackPressures() {
    BackPressures copy;
    if (dataLock_ && xSemaphoreTake(dataLock_, portMAX_DELAY) == pdTRUE) {
        copy = backPressures_;
        xSemaphoreGive(dataLock_);
    }
    return copy;
}


InertialData CANread::getInertialData() {
    InertialData copy;
    if (dataLock_ && xSemaphoreTake(dataLock_, portMAX_DELAY) == pdTRUE) {
        copy = inertial_;
        xSemaphoreGive(dataLock_);
    }
    return copy;
}

void CANread::isrHandler(const CAN_message_t &msg) {
    if (instance_) {
        instance_->processMessageFromISR(msg);
    }
}

void CANread::processMessageFromISR(const CAN_message_t &msg) {
    if (!dataLock_) return;

    BaseType_t takeWoken = pdFALSE;
    if (xSemaphoreTakeFromISR(dataLock_, &takeWoken) != pdTRUE) {
        if (takeWoken == pdTRUE) {
            portYIELD_FROM_ISR(takeWoken);
        }
        return;
    }

    switch (msg.id) {
        case ID_BACK_RIGHT_TEMPS:
            if (msg.len >= 8) {
                backTemps_.tempCableCase = msg.buf[0];
                backTemps_.tempFirewall  = msg.buf[1];
                backTemps_.tempFan1      = msg.buf[2];
                backTemps_.tempFan2      = msg.buf[3];
                backTemps_.tempSuelo     = msg.buf[4];
                backTemps_.tempEsc1      = msg.buf[5];
                backTemps_.tempEsc2      = msg.buf[6];
                rearHall_.gear           = msg.buf[7];
            }
            break;

        case ID_BACK_RIGHT_GAL:
            if (msg.len >= 7) {
                backGal_.galExtRA1     = msg.buf[0];
                backGal_.galExtRA2     = msg.buf[1];
                backGal_.galExtRA3     = msg.buf[2];
                backGal_.galExtRA4     = msg.buf[3];
                backGal_.galDinRight1  = msg.buf[4];
                backGal_.galDinRight2  = msg.buf[5];
                backGal_.galDinRight3  = msg.buf[6];
            }
            break;

        case ID_REAR_RIGHT_HALL:
            if (msg.len >= 3) {
                rearHall_.hallRearRight = static_cast<uint16_t>(msg.buf[0] | (msg.buf[1] << 8));
                backPressures_.presCompresAir = msg.buf[2];
            }
            break;

        case ID_BACK_LEFT_TEMPS:
            if (msg.len >= 5) {
                backTemps_.tempOil    = msg.buf[0];
                backTemps_.tempGas    = msg.buf[1];
                backTemps_.tempRefri1 = msg.buf[2];
                backTemps_.tempRefri2 = msg.buf[3];
                rearHall_.gasLevel   = msg.buf[4];
            }
            if (msg.len >= 8) {
                inertial_.gyroX = static_cast<float>(msg.buf[6] | (msg.buf[7] << 8))/10.0;
            }
            break;

        case ID_BACK_LEFT_PRESS:
            if (msg.len >= 6) {
                backPressures_.presAdmision = msg.buf[0];
                backPressures_.presOil      = msg.buf[1];
                backPressures_.presGas      = msg.buf[2];
                backGal_.galDinLeft1  = msg.buf[3];
                backGal_.galDinLeft2  = msg.buf[4];
                backGal_.galDinLeft3  = msg.buf[5];
            }
            if (msg.len >= 8) {
                inertial_.gyroY = static_cast<float>(msg.buf[6] | (msg.buf[7] << 8))/10.0;
            }
            break;

        case ID_INERTIAL:
            if (msg.len >= 2) {
                inertial_.accelX = static_cast<float>(msg.buf[0] | (msg.buf[1] << 8))/10.0;
            }
            if (msg.len >= 4) {
                inertial_.accelY = static_cast<float>(msg.buf[2] | (msg.buf[3] << 8))/10.0;
            }
            if (msg.len >= 6) {
                inertial_.accelZ = static_cast<float>(msg.buf[4] | (msg.buf[5] << 8))/10.0;
            }
            if (msg.len >= 8) {
                inertial_.gyroZ = static_cast<float>(msg.buf[6] | (msg.buf[7] << 8))/10.0;
            }
            break;

        default:
            break;
    }

    BaseType_t giveWoken = pdFALSE;
    xSemaphoreGiveFromISR(dataLock_, &giveWoken);

    if (takeWoken == pdTRUE || giveWoken == pdTRUE) {
        portYIELD_FROM_ISR(pdTRUE);
    }
}