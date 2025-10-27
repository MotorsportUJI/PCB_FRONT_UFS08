#include "Tasks.h"
#include "Sensors/HallSensor/HallSensor.h"

extern HallSensor hallSensor;

void task_hall(void *pvParams) {
  for (;;) {
    int rpm = hallSensor.getRPM();
    xQueueOverwrite(hall_queue, &rpm);
    vTaskDelay(pdMS_TO_TICKS(PERIOD_HALL_MS));
  }
}
