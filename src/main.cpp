#include <Arduino.h>
#include "Tasks/Tasks.h"
#include "Sensors/AnalogSensor/AnalogSensor.h"
#include "Sensors/HallSensor/HallSensor.h"
#include "controllers/OBDReader/OBDReader.h"
#include "controllers/CANbus/CANbus.h"
#include "controllers/SDLogger/SDLogger.h"
#include "config.h"

// ==== Instancias globales ====
QueueHandle_t galgas_queue = nullptr;
QueueHandle_t presion_freno_queue = nullptr;
QueueHandle_t hall_queue = nullptr;
SemaphoreHandle_t data_mutex = nullptr;
Data actual_data = {};

AnalogSensor presion_freno(PIN_PRESION_FRENO, 12, 410, 3685, 0, 104);

AnalogSensor galga_din1_right(PIN_GALGA_DIN1_RIGHT);
AnalogSensor galga_din2_right(PIN_GALGA_DIN2_RIGHT);
AnalogSensor galga_din3_right(PIN_GALGA_DIN3_RIGHT);
AnalogSensor galga_din1_left(PIN_GALGA_DIN1_LEFT);
AnalogSensor galga_din2_left(PIN_GALGA_DIN2_LEFT);
AnalogSensor galga_din3_left(PIN_GALGA_DIN3_LEFT);

HallSensor hallSensor(PIN_HALL_SENSOR);

SDLogger sdLogger;

OBDReader obd;
CANread can(CAN_BUS_BAUDRATE);

void setup() {
  Serial.begin(9600);
  delay(100);

  // Inicialización de sensores
  presion_freno.begin();

  galga_din1_right.begin();
  galga_din2_right.begin();
  galga_din3_right.begin();
  galga_din1_left.begin();
  galga_din2_left.begin();
  galga_din3_left.begin();

  hallSensor.begin();

  obd.begin(OBD_CAN_BAUDRATE);
  can.begin();

  galgas_queue = xQueueCreate(QUEUE_GALGAS_LENGTH, sizeof(int)*6);          // Cola para 10 lecturas de galgas
  presion_freno_queue = xQueueCreate(QUEUE_PRESION_LENGTH, sizeof(int));    // Cola para 10 lecturas de presión de freno
  hall_queue = xQueueCreate(QUEUE_HALL_LENGTH, sizeof(int));              // Cola para 10 lecturas de RPM del Hall

  data_mutex = xSemaphoreCreateMutex();

  actual_data = {};

  xTaskCreate(task_galgas, "Galgas", STACK_GALGAS, NULL, PRIO_GALGAS, NULL);           // Tarea de lectura del sensor de galgas
  xTaskCreate(task_presion_freno, "PresionFreno", STACK_PRESION, NULL, PRIO_PRESION, NULL); // Tarea de lectura del sensor de presión de freno
  xTaskCreate(task_hall, "Hall", STACK_HALL, NULL, PRIO_HALL, NULL);                     // Tarea de lectura del sensor Hall
  xTaskCreate(task_data_collector, "DataCollector", STACK_DATACOLLECT, NULL, PRIO_DATACOLLECT, NULL); // Tarea de recopilación de datos
  xTaskCreate(task_screen, "Screen", STACK_SCREEN, NULL, PRIO_SCREEN, NULL);               // Tarea de actualización de la pantalla
  xTaskCreate(task_telemetry, "Telemetry", STACK_TELEMETRY, NULL, PRIO_TELEMETRY, NULL);           // Tarea de telemetría
  xTaskCreate(task_sd_logger, "SDLogger", STACK_SD_LOGGER, &sdLogger, PRIO_SD_LOGGER, NULL);           // Tarea de registro en SD

  vTaskStartScheduler(); // Inicia el planificador de FreeRTOS
}

void loop() {
  // No se usa con FreeRTOS
}
