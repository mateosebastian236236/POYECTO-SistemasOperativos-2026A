#ifndef TRANSACCIONES_H
#define TRANSACCIONES_H

#define MAX_CADENA 50


typedef struct {
    int id;
    char fecha[11];           
    char categoria[MAX_CADENA];
    float monto;
    float cantidad;               
    char region[MAX_CADENA];
    float descuento;
} Transaccion;


Transaccion* leer_csv(const char* nombre_archivo, int* num_transacciones);
void liberar_transacciones(Transaccion* datos);

#endif