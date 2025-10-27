#include "Tasks.h"
#include "controllers/SDLogger/SDLogger.h"

void task_sd_logger(void *pvParams) {
    SDLogger sdLogger = *static_cast<SDLogger*>(pvParams);
    Data actual_data;
    for (;;) {
        if (sdLogger.ok()) {
            while (xQueueReceive(sd_logger_queue, &actual_data, 0) == pdTRUE) {
                sdLogger.escribir(actual_data);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(PERIOD_SD_LOGGER_MS));  // Esperar el periodo definido
    }
}
