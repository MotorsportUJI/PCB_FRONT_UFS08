#include "Tasks.h"
#include "controllers/SDLogger/SDLogger.h"

void task_sd_logger(void *pvParams) {
    SDLogger sdLogger = *static_cast<SDLogger*>(pvParams);
    for (;;) {
        if (sdLogger.ok()) {
            xSemaphoreTake(data_mutex, portMAX_DELAY);
            sdLogger.escribir(actual_data);
            xSemaphoreGive(data_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(PERIOD_SD_LOGGER_MS));  // Esperar el periodo definido
    }
}
