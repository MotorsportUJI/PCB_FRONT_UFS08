#pragma once

// Prioridades (mayor = más urgente)
#define PRIO_GALGAS        1
#define PRIO_PRESION       3
#define PRIO_HALL          1
#define PRIO_DATACOLLECT   4
#define PRIO_SCREEN        2
#define PRIO_TELEMETRY     1
#define PRIO_SD_LOGGER     2

// Periodos (ms)
#define PERIOD_GALGAS_MS      500
#define PERIOD_PRESION_MS     100
#define PERIOD_HALL_MS        200
#define PERIOD_DATACOLLECT_MS 100
#define PERIOD_SCREEN_MS      200
#define PERIOD_TELEMETRY_MS   500
#define PERIOD_SD_LOGGER_MS   300

// Tamaños de pila para las tareas (ajustables)
#define STACK_GALGAS       2048
#define STACK_PRESION      2048
#define STACK_HALL         2048
#define STACK_DATACOLLECT  4096
#define STACK_SCREEN       4096
#define STACK_TELEMETRY    4096
#define STACK_SD_LOGGER    4096

// Longitudes de colas
#define QUEUE_GALGAS_LENGTH    1
#define QUEUE_PRESION_LENGTH   1
#define QUEUE_HALL_LENGTH      1
#define QUEUE_SCREEN_LENGTH    1
#define QUEUE_TELEMETRY_LENGTH 1
#define QUEUE_SD_LOGGER_LENGTH 20

// Pines de sensores
#define PIN_HALL_SENSOR        40
#define PIN_PRESION_FRENO      A11
#define PIN_GALGA_DIN1_RIGHT   A0
#define PIN_GALGA_DIN2_RIGHT   A1
#define PIN_GALGA_DIN3_RIGHT   A2
#define PIN_GALGA_DIN1_LEFT    A3
#define PIN_GALGA_DIN2_LEFT    A4
#define PIN_GALGA_DIN3_LEFT    A5

// Baudrates
#define CAN_BUS_BAUDRATE       500000
#define OBD_CAN_BAUDRATE       500000



