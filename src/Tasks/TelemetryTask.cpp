#include "Tasks.h"

void task_telemetry(void *pvParams) {
    Data local_data;
    for (;;) {
        while (xQueueReceive(telemetry_queue, &local_data, 0) == pdTRUE) {
            
        // Aquí Joaquin implementara la lógica de telemetría

        }

        vTaskDelay(pdMS_TO_TICKS(PERIOD_TELEMETRY_MS)); // Esperar 0.5 segundo antes de la siguiente iteración
    }
}
