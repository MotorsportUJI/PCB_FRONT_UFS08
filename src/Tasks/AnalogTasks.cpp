#include "Tasks.h"
#include "Sensors/AnalogSensor/AnalogSensor.h"

extern AnalogSensor galga_din1_right, galga_din2_right, galga_din3_right;
extern AnalogSensor galga_din1_left, galga_din2_left, galga_din3_left;
extern AnalogSensor presion_freno;
extern AnalogSensor analogFiltered;

void task_galgas(void *pvParams) {
  static int galgas[6];
  for (;;) {
    galgas[0] = galga_din1_right.readRaw();
    galgas[1] = galga_din2_right.readRaw();
    galgas[2] = galga_din3_right.readRaw();
    galgas[3] = galga_din1_left.readRaw();
    galgas[4] = galga_din2_left.readRaw();
    galgas[5] = galga_din3_left.readRaw();


    xQueueSend(galgas_queue, galgas, portMAX_DELAY);

    vTaskDelay(pdMS_TO_TICKS(PERIOD_GALGAS_MS));
  }
}

void task_presion_freno(void *pvParams) {
  for (;;) {
    int pres_freno = presion_freno.read();
    xQueueSend(presion_freno_queue, &pres_freno, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(PERIOD_PRESION_MS));
  }
}
