#ifndef LEER_CSV_H
#define LEER_CSV_H

#include "transacciones.h"

Transaccion* leer_csv(const char* nombre_archivo, int* num_transacciones);
void liberar_transacciones(Transaccion* datos);

#endif