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
QueueHandle_t sd_logger_queue = nullptr;
QueueHandle_t telemetry_queue = nullptr;
QueueHandle_t screen_queue = nullptr;

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
  delay(10000);
  
  Serial.println("Iniciando sistema...");
  // Inicialización de sensores
  presion_freno.begin();

  galga_din1_right.begin();
  galga_din2_right.begin();
  galga_din3_right.begin();
  galga_din1_left.begin();
  galga_din2_left.begin();
  galga_din3_left.begin();

  hallSensor.begin();

  // Inicializar puerto serie usado por la pantalla (Nextion) si existe
  // Usamos 9600 como baud por defecto (ajustar si tu display usa otro).
  Serial.println("Inicializando Serial8 para pantalla...");
  Serial8.begin(9600);

  obd.begin(OBD_CAN_BAUDRATE);
  can.begin();

  galgas_queue = xQueueCreate(QUEUE_GALGAS_LENGTH, sizeof(int)*6);          // Cola para 10 lecturas de galgas
  presion_freno_queue = xQueueCreate(QUEUE_PRESION_LENGTH, sizeof(int));    // Cola para 10 lecturas de presión de freno
  hall_queue = xQueueCreate(QUEUE_HALL_LENGTH, sizeof(int));              // Cola para 10 lecturas de RPM del Hall
  sd_logger_queue = xQueueCreate(QUEUE_SD_LOGGER_LENGTH, sizeof(Data));    // Cola para 50 datos a registrar en SD
  telemetry_queue = xQueueCreate(QUEUE_TELEMETRY_LENGTH, sizeof(Data));        // Cola para 50 datos a enviar por telemetría
  screen_queue = xQueueCreate(QUEUE_SCREEN_LENGTH, sizeof(Data));          // Cola para 10 datos a mostrar en pantalla

  Serial.println("Creando tareas...");
  TaskHandle_t hGalgas = NULL, hPresion = NULL, hHall = NULL, hDataCollector = NULL, hScreen = NULL, hTelemetry = NULL, hSDLogger = NULL;
  BaseType_t r;
  r = xTaskCreate(task_galgas, "Galgas", STACK_GALGAS, NULL, PRIO_GALGAS, &hGalgas);           // Tarea de lectura del sensor de galgas
  Serial.printf("create Galgas: %d\n", (int)r);
  r = xTaskCreate(task_presion_freno, "PresionFreno", STACK_PRESION, NULL, PRIO_PRESION, &hPresion); // Tarea de lectura del sensor de presión de freno
  Serial.printf("create PresionFreno: %d\n", (int)r);
  r = xTaskCreate(task_hall, "Hall", STACK_HALL, NULL, PRIO_HALL, &hHall);                     // Tarea de lectura del sensor Hall
  Serial.printf("create Hall: %d\n", (int)r);
  r = xTaskCreate(task_data_collector, "DataCollector", STACK_DATACOLLECT, NULL, PRIO_DATACOLLECT, &hDataCollector); // Tarea de recopilación de datos
  Serial.printf("create DataCollector: %d\n", (int)r);
  r = xTaskCreate(task_screen, "Screen", STACK_SCREEN, NULL, PRIO_SCREEN, &hScreen);               // Tarea de actualización de la pantalla
  Serial.printf("create Screen: %d\n", (int)r);
  r = xTaskCreate(task_telemetry, "Telemetry", STACK_TELEMETRY, NULL, PRIO_TELEMETRY, &hTelemetry);           // Tarea de telemetría
  Serial.printf("create Telemetry: %d\n", (int)r);
  r = xTaskCreate(task_sd_logger, "SDLogger", STACK_SD_LOGGER, &sdLogger, PRIO_SD_LOGGER, &hSDLogger);           // Tarea de registro en SD
  Serial.printf("create SDLogger: %d\n", (int)r);

  vTaskStartScheduler(); // Inicia el planificador de FreeRTOS
}

void loop() {
  // No se usa con FreeRTOS
}
