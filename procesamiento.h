#ifndef PROCESAMIENTO_H
#define PROCESAMIENTO_H

#include "transacciones.h"

void limpiar_nulos_numericos(Transaccion* datos, int inicio, int fin);
void imputar_moda_categoricos(Transaccion* datos, int inicio, int fin);
void normalizar(Transaccion* datos, int inicio, int fin,
                float min_monto, float max_monto,
                float min_cantidad, float max_cantidad,
                float min_descuento, float max_descuento);

void obtener_min_max_monto(Transaccion* datos, int n, float* min, float* max);
void obtener_min_max_cantidad(Transaccion* datos, int n, float* min, float* max);
void obtener_min_max_descuento(Transaccion* datos, int n, float* min, float* max);

#endif