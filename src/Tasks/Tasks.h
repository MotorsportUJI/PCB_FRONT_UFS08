#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include "config.h"

// ===== Estructuras de datos =====
struct Data {
    uint8_t temp_cable_case, temp_firewall, temp_suelto;
    uint8_t temp_fan1, temp_fan2;
    uint8_t temp_esc_1, temp_esc_2;
    uint8_t temp_oil, temp_gas;
    uint8_t temp_refri_1, temp_refri_2;

    uint8_t gal_ext_RA_1, gal_ext_RA_2, gal_ext_RA_3, gal_ext_RA_4;
    uint8_t gal_din1_rightb, gal_din2_rightb, gal_din3_rightb;
    uint8_t gal_din1_leftb, gal_din2_leftb, gal_din3_leftb;
    uint16_t gal_din1_rightf, gal_din2_rightf, gal_din3_rightf;
    uint16_t gal_din1_leftf, gal_din2_leftf, gal_din3_leftf;

    uint16_t hall_rear_right;
    uint16_t hall_front;

    uint8_t pres_admision, pres_oil, pres_gas;
    uint8_t pres_comprest_air;
    uint8_t pres_freno;

    float accel_x, accel_y, accel_z;
    float gyro_x, gyro_y, gyro_z;

    uint8_t gas_level;
    uint8_t gear;

    int obd_rpm;
    uint8_t obd_throttle;
    uint8_t obd_temp;

};

// ===== Variables globales compartidas =====
extern QueueHandle_t galgas_queue;
extern QueueHandle_t presion_freno_queue;
extern QueueHandle_t hall_queue;
extern QueueHandle_t sd_logger_queue;
extern QueueHandle_t telemetry_queue;
extern QueueHandle_t screen_queue;

// ===== Declaraciones de tareas =====
void task_galgas(void *pvParams);
void task_presion_freno(void *pvParams);
void task_hall(void *pvParams);
void task_data_collector(void *pvParams);
void task_screen(void *pvParams);
void task_telemetry(void *pvParams);
void task_sd_logger(void *pvParams);

#endif
