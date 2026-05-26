#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "procesamiento.h"

// Reemplaza -1 en monto, cantidad, descuento por el promedio del bloque
void limpiar_nulos_numericos(Transaccion* datos, int inicio, int fin) {
    // Calcular sumas y contar no nulos para cada campo
    float suma_monto = 0.0f;
    int count_monto = 0;
    float suma_cantidad = 0.0f;
    int count_cantidad = 0;
    float suma_descuento = 0.0f;
    int count_descuento = 0;
    
    for (int i = inicio; i < fin; i++) {
        if (datos[i].monto != -1.0f) {
            suma_monto += datos[i].monto;
            count_monto++;
        }
        if (datos[i].cantidad != -1.0f) {
            suma_cantidad += datos[i].cantidad;
            count_cantidad++;
        }
        if (datos[i].descuento != -1.0f) {
            suma_descuento += datos[i].descuento;
            count_descuento++;
        }
    }
    
    // Calcular promedios (si no hay datos no nulos, se deja 0)
    float promedio_monto = (count_monto > 0) ? (suma_monto / count_monto) : 0.0f;
    float promedio_cantidad = (count_cantidad > 0) ? (suma_cantidad / count_cantidad) : 0.0f;
    float promedio_descuento = (count_descuento > 0) ? (suma_descuento / count_descuento) : 0.0f;
    
    // Reemplazar centinelas
    for (int i = inicio; i < fin; i++) {
        if (datos[i].monto == -1.0f) datos[i].monto = promedio_monto;
        if (datos[i].cantidad == -1.0f) datos[i].cantidad = promedio_cantidad;
        if (datos[i].descuento == -1.0f) datos[i].descuento = promedio_descuento;
    }
}

// Calcula la moda de un campo categórico en el bloque
static void moda_campo(Transaccion* datos, int inicio, int fin, 
                       char* destino, size_t destino_size,
                       const char* (*getter)(Transaccion*)) {
    // Arreglos para almacenar valores únicos y sus frecuencias
    char* valores[100];
    int frecuencias[100];
    int num_unicos = 0;
    
    for (int i = inicio; i < fin; i++) {
        const char* valor = getter(&datos[i]);
        if (strlen(valor) == 0) continue;
        
        int encontrado = -1;
        for (int j = 0; j < num_unicos; j++) {
            if (strcmp(valores[j], valor) == 0) {
                encontrado = j;
                break;
            }
        }
        if (encontrado == -1) {
            valores[num_unicos] = (char*)valor;
            frecuencias[num_unicos] = 1;
            num_unicos++;
        } else {
            frecuencias[encontrado]++;
        }
    }
    
    // Encontrar el de mayor frecuencia
    int max_freq = 0;
    int idx_moda = 0;
    for (int i = 0; i < num_unicos; i++) {
        if (frecuencias[i] > max_freq) {
            max_freq = frecuencias[i];
            idx_moda = i;
        }
    }
    
    if (num_unicos > 0) {
        strncpy(destino, valores[idx_moda], destino_size - 1);
        destino[destino_size - 1] = '\0';
    } else {
        strncpy(destino, "Desconocido", destino_size - 1);
        destino[destino_size - 1] = '\0';
    }
}

static const char* get_categoria(Transaccion* t) { return t->categoria; }
static const char* get_region(Transaccion* t) { return t->region; }

void imputar_moda_categoricos(Transaccion* datos, int inicio, int fin) {
    char moda_cat[MAX_CADENA];
    char moda_reg[MAX_CADENA];
    
    moda_campo(datos, inicio, fin, moda_cat, MAX_CADENA, get_categoria);
    moda_campo(datos, inicio, fin, moda_reg, MAX_CADENA, get_region);
    
    for (int i = inicio; i < fin; i++) {
        if (strlen(datos[i].categoria) == 0) {
            strncpy(datos[i].categoria, moda_cat, MAX_CADENA - 1);
            datos[i].categoria[MAX_CADENA - 1] = '\0';
        }
        if (strlen(datos[i].region) == 0) {
            strncpy(datos[i].region, moda_reg, MAX_CADENA - 1);
            datos[i].region[MAX_CADENA - 1] = '\0';
        }
    }
}

void normalizar(Transaccion* datos, int inicio, int fin,
                float min_monto, float max_monto,
                float min_cantidad, float max_cantidad,
                float min_descuento, float max_descuento) {
    // Evitar división por cero si todos los valores son iguales
    float rango_monto = max_monto - min_monto;
    float rango_cantidad = max_cantidad - min_cantidad;
    float rango_descuento = max_descuento - min_descuento;
    
    for (int i = inicio; i < fin; i++) {
        if (rango_monto != 0.0f)
            datos[i].monto = (datos[i].monto - min_monto) / rango_monto;
        else
            datos[i].monto = 0.5f;
        
        if (rango_cantidad != 0.0f)
            datos[i].cantidad = (datos[i].cantidad - min_cantidad) / rango_cantidad;
        else
            datos[i].cantidad = 0.5f;
        
        if (rango_descuento != 0.0f)
            datos[i].descuento = (datos[i].descuento - min_descuento) / rango_descuento;
        else
            datos[i].descuento = 0.5f;
    }
}

// Funciones para calcular mínimos y máximos globales

void obtener_min_max_monto(Transaccion* datos, int n, float* min, float* max) {
    *min = 1e9f;
    *max = -1e9f;
    for (int i = 0; i < n; i++) {
        if (datos[i].monto != -1.0f) {
            if (datos[i].monto < *min) *min = datos[i].monto;
            if (datos[i].monto > *max) *max = datos[i].monto;
        }
    }
}

void obtener_min_max_cantidad(Transaccion* datos, int n, float* min, float* max) {
    *min = 1e9f;
    *max = -1e9f;
    for (int i = 0; i < n; i++) {
        if (datos[i].cantidad != -1.0f) {
            if (datos[i].cantidad < *min) *min = datos[i].cantidad;
            if (datos[i].cantidad > *max) *max = datos[i].cantidad;
        }
    }
}

void obtener_min_max_descuento(Transaccion* datos, int n, float* min, float* max) {
    *min = 1e9f;
    *max = -1e9f;
    for (int i = 0; i < n; i++) {
        if (datos[i].descuento != -1.0f) {
            if (datos[i].descuento < *min) *min = datos[i].descuento;
            if (datos[i].descuento > *max) *max = datos[i].descuento;
        }
    }
}