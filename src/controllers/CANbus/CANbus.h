#pragma once

#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <FreeRTOS.h>
#include <semphr.h>

struct BackTemps {
    uint8_t tempCableCase = 0;
    uint8_t tempFirewall = 0;
    uint8_t tempFan1 = 0;
    uint8_t tempFan2 = 0;
    uint8_t tempSuelo = 0;
    uint8_t tempEsc1 = 0;
    uint8_t tempEsc2 = 0;
    uint8_t tempOil = 0;
    uint8_t tempGas = 0;
    uint8_t tempRefri1 = 0;
    uint8_t tempRefri2 = 0;
};

struct BackGal {
    uint8_t galExtRA1 = 0;
    uint8_t galExtRA2 = 0;
    uint8_t galExtRA3 = 0;
    uint8_t galExtRA4 = 0;
    uint8_t galDinRight1 = 0;
    uint8_t galDinRight2 = 0;
    uint8_t galDinRight3 = 0;
    uint8_t galDinLeft1 = 0;
    uint8_t galDinLeft2 = 0;
    uint8_t galDinLeft3 = 0;
};

struct RearHall {
    uint16_t hallRearRight = 0;
    uint8_t gasLevel = 0;
    uint8_t gear = 0;
};



struct BackPressures {
    uint8_t presAdmision = 0;
    uint8_t presOil = 0;
    uint8_t presGas = 0;
    uint8_t presCompresAir = 0;

};

struct InertialData {
    float gyroX = 0;
    float gyroY = 0;
    float gyroZ = 0;
    float accelX = 0;
    float accelY = 0;
    float accelZ = 0;
};

class CANread {
public:
    explicit CANread(uint32_t baudrate);
    bool begin();
    void end();

    void sendPressureBreak(uint8_t pressure);

    BackTemps getBackTemps();
    BackGal getBackGal();
    RearHall getRearHall();
    BackPressures getBackPressures();
    InertialData getInertialData();

private:
    static constexpr uint32_t ID_BACK_RIGHT_TEMPS   = 0x011;
    static constexpr uint32_t ID_BACK_RIGHT_GAL     = 0x101;
    static constexpr uint32_t ID_REAR_RIGHT_HALL    = 0x021;
    static constexpr uint32_t ID_BACK_LEFT_TEMPS    = 0x012;
    static constexpr uint32_t ID_BACK_LEFT_PRESS    = 0x022;
    static constexpr uint32_t ID_INERTIAL           = 0x102;

    static void isrHandler(const CAN_message_t &msg);
    void processMessageFromISR(const CAN_message_t &msg);

    uint32_t baudrate_;
    bool initialized_ = false;

    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canBus_;
    static CANread *instance_;

    SemaphoreHandle_t dataLock_ = nullptr;

    BackTemps backTemps_;
    BackGal backGal_;
    RearHall rearHall_;
    BackPressures backPressures_;
    InertialData inertial_;
};