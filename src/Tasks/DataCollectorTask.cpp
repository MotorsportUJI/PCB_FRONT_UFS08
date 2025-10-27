#include "Tasks.h"
#include "controllers/OBDReader/OBDReader.h"
#include "controllers/CANbus/CANbus.h"

extern OBDReader obd;
extern CANread can;

void task_data_collector(void *pvParams) {
  int galgas[6];
  int pres_freno;
  int hall_rpm;
  TickType_t lastWake = xTaskGetTickCount();

  Data temp_data = {};
  for (;;) {

    while (xQueueReceive(galgas_queue, galgas, 0) == pdTRUE) {
      temp_data.gal_din1_rightf = galgas[0];
      temp_data.gal_din2_rightf = galgas[1];
      temp_data.gal_din3_rightf = galgas[2];
      temp_data.gal_din1_leftf = galgas[3];
      temp_data.gal_din2_leftf = galgas[4];
      temp_data.gal_din3_leftf = galgas[5];
    }
    while (xQueueReceive(presion_freno_queue, &pres_freno, 0) == pdTRUE) {
      temp_data.pres_freno = pres_freno;
      can.sendPressureBreak(pres_freno);
    }
    while (xQueueReceive(hall_queue, &hall_rpm, 0) == pdTRUE) {
      temp_data.hall_rear_right = hall_rpm;
    }

    temp_data.obd_rpm = obd.getRPM();
    temp_data.obd_throttle = obd.getThrottle();
    temp_data.obd_temp = obd.getCoolantTemp();

    BackTemps backTemps = can.getBackTemps();
    temp_data.temp_cable_case = backTemps.tempCableCase;
    temp_data.temp_firewall = backTemps.tempFirewall;
    temp_data.temp_fan1 = backTemps.tempFan1;
    temp_data.temp_fan2 = backTemps.tempFan2;
    temp_data.temp_suelto = backTemps.tempSuelo;
    temp_data.temp_esc_1 = backTemps.tempEsc1;
    temp_data.temp_esc_2 = backTemps.tempEsc2;
    temp_data.temp_oil = backTemps.tempOil;
    temp_data.temp_gas = backTemps.tempGas;
    temp_data.temp_refri_1 = backTemps.tempRefri1;
    temp_data.temp_refri_2 = backTemps.tempRefri2;

    BackGal backGal = can.getBackGal();
    temp_data.gal_ext_RA_1 = backGal.galExtRA1;
    temp_data.gal_ext_RA_2 = backGal.galExtRA2;
    temp_data.gal_ext_RA_3 = backGal.galExtRA3;
    temp_data.gal_ext_RA_4 = backGal.galExtRA4;
    temp_data.gal_din1_rightb = backGal.galDinRight1;
    temp_data.gal_din2_rightb = backGal.galDinRight2;
    temp_data.gal_din3_rightb = backGal.galDinRight3;
    temp_data.gal_din1_leftb = backGal.galDinLeft1;
    temp_data.gal_din2_leftb = backGal.galDinLeft2;
    temp_data.gal_din3_leftb = backGal.galDinLeft3;

    RearHall rearHall = can.getRearHall();
    temp_data.hall_rear_right = rearHall.hallRearRight;
    temp_data.gas_level = rearHall.gasLevel;
    temp_data.gear = rearHall.gear;

    BackPressures backPressures = can.getBackPressures();
    temp_data.pres_admision = backPressures.presAdmision;
    temp_data.pres_oil = backPressures.presOil;
    temp_data.pres_gas = backPressures.presGas;
    temp_data.pres_comprest_air = backPressures.presCompresAir;
    
    InertialData inertial = can.getInertialData();
    temp_data.accel_x = inertial.accelX;
    temp_data.accel_y = inertial.accelY;
    temp_data.accel_z = inertial.accelZ;
    temp_data.gyro_x = inertial.gyroX;
    temp_data.gyro_y = inertial.gyroY;
    temp_data.gyro_z = inertial.gyroZ;

    xQueueSend(sd_logger_queue, &temp_data, 0);
    xQueueOverwrite(telemetry_queue, &temp_data);
    xQueueOverwrite(screen_queue, &temp_data);

    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(PERIOD_DATACOLLECT_MS));
  }
}
