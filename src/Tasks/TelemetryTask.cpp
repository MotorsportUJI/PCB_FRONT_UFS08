#include "Tasks.h"

void task_telemetry(void *pvParams) {
    Data local_data;
    for (;;) {
        xSemaphoreTake(data_mutex, portMAX_DELAY);
        local_data = actual_data;
        xSemaphoreGive(data_mutex);

        // Aquí Joaquin implementara la lógica de telemetría 

        vTaskDelay(pdMS_TO_TICKS(PERIOD_TELEMETRY_MS)); // Esperar 0.5 segundo antes de la siguiente iteración
    }
}
