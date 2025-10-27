#include "Tasks.h"

void task_screen(void *pvParams) {
    Data local_data;
    bool estado = false;
    int cont = 0;
    for (;;) {
        while (xQueueReceive(screen_queue, &local_data, 0) == pdTRUE) {

            if (local_data.obd_temp > 100) {
                if (!estado) Serial8.print("page1.pic=7");
                else Serial8.print("page1.pic=6");
                cont++;
                if (cont == 10) {
                cont = 0;
                estado = !estado;
                }
            }else{
                Serial8.print("page1.pic=6");
            }

            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            // Enviar temperatura del refrigerante (temp2) page1
            Serial8.print("page1.temp1.val=");
            Serial8.print(local_data.obd_temp);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            // Enviar temperatura del refrigerante (temp2) page2
            Serial8.print("page2.temp1.val=");
            Serial8.print(local_data.temp_refri_1);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            // Enviar marcha (gear) page1
            Serial8.print("page1.gear.val=");
            Serial8.print(local_data.gear);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            // Enviar marcha (gear) page2
            Serial8.print("page2.gear.val=");
            Serial8.print(local_data.gear);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            // Enviar RPM (rpm) page1
            Serial8.print("page1.nRpm.val=");
            Serial8.print(local_data.obd_rpm);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            // Enviar RPM (rpm) page2
            Serial8.print("page2.rpm.val=");
            Serial8.print(local_data.obd_rpm);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            //Barra RPM page1
            int j0 = local_data.obd_rpm*100/11500;
            Serial8.print("page1.j0.val=");
            Serial8.print(j0);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            //Barra RPM page2
            Serial8.print("page2.j0.val=");
            Serial8.print(j0);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            //Enviar temp2
            Serial8.print("page2.temp2.val=");
            Serial8.print(local_data.temp_refri_2);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            //Enviar tempc
            Serial8.print("page2.tempc.val=");
            Serial8.print(local_data.obd_temp);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            //Enviar tempf
            Serial8.print("page2.tempf.val=");
            Serial8.print(local_data.temp_firewall);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            //Enviar presad
            Serial8.print("page2.presad.val=");
            Serial8.print(local_data.pres_admision);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);

            //Enviar presac
            Serial8.print("page2.presac.val=");
            Serial8.print(local_data.pres_freno);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);
            
            //Enviar throttle
            Serial8.print("page2.throt.val=");
            Serial8.print(local_data.obd_throttle);
            Serial8.write(0xFF); Serial8.write(0xFF); Serial8.write(0xFF);
        }

        vTaskDelay(pdMS_TO_TICKS(PERIOD_SCREEN_MS));
    }
}