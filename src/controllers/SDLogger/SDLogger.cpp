#include "SDLogger.h"
#include <SPI.h>

SDLogger::SDLogger() {
    initialized = false;

    // Iniciar la SD (usa ::SD para evitar choque con el nombre de la clase)
    if (!::SD.begin(BUILTIN_SDCARD)) {
        // Si falla la inicialización, no continuar.
        return;
    }

    // Obtener la hora actual (si no está configurada, devolverá epoch)
    time_t now = time(nullptr);
    struct tm tm_now;
    if (localtime_r(&now, &tm_now) == nullptr) {
        // Si no se puede obtener la hora, usar 000000...
        filename = "000000000000.csv";
    } else {
        // Formato pedido: dia mes año segundo minuto hora -> DDMMYYYYSSMMHH.csv
        char buf[71];
        snprintf(buf, sizeof(buf), "%02d%02d%04d%02d%02d%02d.csv",
                 tm_now.tm_mday,
                 tm_now.tm_mon + 1,
                 tm_now.tm_year + 1900,
                 tm_now.tm_sec,
                 tm_now.tm_min,
                 tm_now.tm_hour);
        filename = String(buf);
    }

    createFileWithHeader();
    initialized = true;
}

void SDLogger::createFileWithHeader() {
    // Abrir archivo en modo añadir/crear
    File file = ::SD.open(filename.c_str(), FILE_WRITE);
    if (!file) return;

    // Si el archivo está vacío (nuevo), escribir la cabecera
    if (file.size() == 0) {
        // Cabecera con los nombres de las variables en el mismo orden que Data
        file.print("timestamp,");
        file.print("temp_cable_case,");
        file.print("temp_firewall,");
        file.print("temp_suelto,");
        file.print("temp_fan1,");
        file.print("temp_fan2,");
        file.print("temp_esc_1,");
        file.print("temp_esc_2,");
        file.print("temp_oil,");
        file.print("temp_gas,");
        file.print("temp_refri_1,");
        file.print("temp_refri_2,");
        file.print("gal_ext_RA_1,gal_ext_RA_2,gal_ext_RA_3,gal_ext_RA_4,");
        file.print("gal_din1_rightb,gal_din2_rightb,gal_din3_rightb,");
        file.print("gal_din1_leftb,gal_din2_leftb,gal_din3_leftb,");
        file.print("gal_din1_rightf,gal_din2_rightf,gal_din3_rightf,");
        file.print("gal_din1_leftf,gal_din2_leftf,gal_din3_leftf,");
        file.print("hall_rear_right,");
        file.print("hall_front,");
        file.print("pres_admision,pres_oil,pres_gas,pres_comprest_air,pres_freno,");
        file.print("accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,");
        file.print("gas_level,gear,obd_rpm,obd_throttle,obd_temp");
        file.println();
    }

    file.close();
}

String SDLogger::dataToCsv(const Data &d) {
    // Buffer suficiente para una línea CSV
    char buf[512];
    int len = snprintf(buf, sizeof(buf),
        "%ld"
        "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,"                          // temps 11
        "%u,%u,%u,%u,"                                               // gal_ext_RA_1..4
        "%u,%u,%u,"                                                   // gal_din1_rightb..3
        "%u,%u,%u,"                                                   // gal_din1_leftb..3
        "%u,%u,%u,"                                                   // gal_din1_rightf..3 (uint16)
        "%u,%u,%u,"                                                   // gal_din1_leftf..3 (uint16)
        "%u,%u,"                                                      // hall_rear_right, hall_front
        "%u,%u,%u,%u,%u,"                                             // presiones
        "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,"                              // accel + gyro
        "%u,%u,%d,%u,%u",
        millis(),
        d.temp_cable_case,
        d.temp_firewall,
        d.temp_suelto,
        d.temp_fan1,
        d.temp_fan2,
        d.temp_esc_1,
        d.temp_esc_2,
        d.temp_oil,
        d.temp_gas,
        d.temp_refri_1,
        d.temp_refri_2,
        d.gal_ext_RA_1,
        d.gal_ext_RA_2,
        d.gal_ext_RA_3,
        d.gal_ext_RA_4,
        d.gal_din1_rightb,
        d.gal_din2_rightb,
        d.gal_din3_rightb,
        d.gal_din1_leftb,
        d.gal_din2_leftb,
        d.gal_din3_leftb,
        d.gal_din1_rightf,
        d.gal_din2_rightf,
        d.gal_din3_rightf,
        d.gal_din1_leftf,
        d.gal_din2_leftf,
        d.gal_din3_leftf,
        d.hall_rear_right,
        d.hall_front,
        d.pres_admision,
        d.pres_oil,
        d.pres_gas,
        d.pres_comprest_air,
        d.pres_freno,
        d.accel_x, d.accel_y, d.accel_z,
        d.gyro_x, d.gyro_y, d.gyro_z,
        d.gas_level,
        d.gear,
        d.obd_rpm,
        d.obd_throttle,
        d.obd_temp
    );

    if (len < 0) return String();
    return String(buf);
}

bool SDLogger::escribir(const Data &d) {
    if (!initialized) return false;

    File file = ::SD.open(filename.c_str(), FILE_WRITE);
    if (!file) return false;

    // Mover al final para añadir la línea
    file.seek(file.size());

    String line = dataToCsv(d);
    if (line.length() == 0) {
        file.close();
        return false;
    }

    file.println(line);
    file.close();
    return true;
}