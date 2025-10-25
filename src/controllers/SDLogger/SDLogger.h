#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include <time.h>
#include "Tasks/Tasks.h"

class SDLogger {
public:
    SDLogger();

    // Escribe una fila con los valores del struct Data.
    // Devuelve true si se escribió correctamente.
    bool escribir(const Data &d);

    // Indica si la SD se inicializó correctamente.
    bool ok() const { return initialized; }

private:
    String filename;
    bool initialized = false;

    void createFileWithHeader();
    String dataToCsv(const Data &d);
};

#endif