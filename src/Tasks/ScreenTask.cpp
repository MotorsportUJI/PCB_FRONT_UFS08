#include "Tasks.h"

void task_screen(void *pvParams) {
    Data local_data;
    bool estado = false;
    int cont = 0;
    // Mensaje de diagnóstico para confirmar que la tarea arrancó
    Serial.println("Screen task started");
    for (;;) {
        // Diagnóstico: mostrar cuantos mensajes hay justo antes de recibir
        UBaseType_t waiting = uxQueueMessagesWaiting(screen_queue);
        Serial.printf("Screen: queue=%p, mensajes esperando=%u\n", (void*)screen_queue, (unsigned)waiting);

        // Esperar hasta PERIOD_SCREEN_MS por un dato en la cola. Si llega, procesarlo.
        if (xQueueReceive(screen_queue, &local_data, pdMS_TO_TICKS(PERIOD_SCREEN_MS)) == pdTRUE) {

            TickType_t tstart = xTaskGetTickCount();
            Serial.print("Enviando datos a pantalla...\r\n");

            // Agrupar comandos en buffers para enviar con menos llamadas al puerto serie
            char buf[256];
            int off = 0;

            // picture toggle logic (igual comportamiento que antes)
            int pic = 6;
            if (local_data.obd_temp > 100) {
                pic = estado ? 6 : 7;
                cont++;
                if (cont == 10) {
                    cont = 0;
                    estado = !estado;
                }
            } else {
                pic = 6;
            }

            // Page1: pic, temp1, gear, nRpm, j0
            off += snprintf(buf + off, sizeof(buf) - off, "page1.pic=%d", pic);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page1.temp1.val=%d", local_data.obd_temp);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page1.gear.val=%d", local_data.gear);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page1.nRpm.val=%d", local_data.obd_rpm);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            int j0 = 0;
            if (local_data.obd_rpm > 0) j0 = local_data.obd_rpm * 100 / 11500;
            off += snprintf(buf + off, sizeof(buf) - off, "page1.j0.val=%d", j0);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");

            // Enviar page1 en una sola escritura
            if (off > 0) {
                Serial8.write((const uint8_t*)buf, off);
                // Si el buffer contenía ya los 0xFF embebidos, no hay que escribirlos de nuevo
            }

            // Dejar que el scheduler ejecute otras tareas brevemente
            vTaskDelay(pdMS_TO_TICKS(1));

            // Page2: temp1, gear, rpm, j0, temp2, tempc, tempf, presad, presac, throt
            off = 0;
            off += snprintf(buf + off, sizeof(buf) - off, "page2.temp1.val=%d", local_data.temp_refri_1);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.gear.val=%d", local_data.gear);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.rpm.val=%d", local_data.obd_rpm);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.j0.val=%d", j0);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.temp2.val=%d", local_data.temp_refri_2);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.tempc.val=%d", local_data.obd_temp);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.tempf.val=%d", local_data.temp_firewall);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.presad.val=%d", local_data.pres_admision);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.presac.val=%d", local_data.pres_freno);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");
            off += snprintf(buf + off, sizeof(buf) - off, "page2.throt.val=%d", local_data.obd_throttle);
            off += snprintf(buf + off, sizeof(buf) - off, "\xFF\xFF\xFF");

            if (off > 0) {
                Serial8.write((const uint8_t*)buf, off);
            }

            // Dar tiempo para que otras tareas se ejecuten y evitar monopolizar la CPU/uart
            vTaskDelay(pdMS_TO_TICKS(1));
            TickType_t tend = xTaskGetTickCount();
            TickType_t delta = tend - tstart;
            unsigned long ms = (unsigned long)(delta * portTICK_PERIOD_MS);
            Serial.printf("Screen: procesado en %lu ms\n", ms);
            Serial.println("Screen: done");
        }
    }
}