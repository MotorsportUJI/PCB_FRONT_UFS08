#include "Tasks.h"
#include "Sensors/HallSensor/HallSensor.h"

extern HallSensor hallSensor;

void task_hall(void *pvParams) {
  for (;;) {
    int rpm = hallSensor.getRPM();
    xQueueSend(hall_queue, &rpm, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(PERIOD_HALL_MS));
  }
}
