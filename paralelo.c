#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "leer_csv.h"
#include "procesamiento.h"

#define NUM_HILOS 3

typedef struct {
    Transaccion* datos;
    int inicio;
    int fin;
    int id_hilo;
    float min_monto, max_monto;
    float min_cant, max_cant;
    float min_desc, max_desc;
    double tiempo_hilo;
} ArgHilo;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* procesar_bloque(void* arg) {
    ArgHilo* a = (ArgHilo*)arg;
    struct timeval t0, t1;
    gettimeofday(&t0, NULL);
    limpiar_nulos_numericos(a->datos, a->inicio, a->fin);
    imputar_moda_categoricos(a->datos, a->inicio, a->fin);
    normalizar(a->datos, a->inicio, a->fin,
               a->min_monto, a->max_monto,
               a->min_cant, a->max_cant,
               a->min_desc, a->max_desc);
    gettimeofday(&t1, NULL);
    a->tiempo_hilo = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    pthread_mutex_lock(&mutex);
    printf("  Hilo %d | filas %5d - %5d | Tiempo: %.6f s\n",
           a->id_hilo, a->inicio, a->fin - 1, a->tiempo_hilo);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.csv>\n", argv[0]);
        return 1;
    }
    int n = 0;
    Transaccion* datos = leer_csv(argv[1], &n);
    if (!datos) {
        fprintf(stderr, "Error al leer el archivo.\n");
        return 1;
    }
    printf("Se cargaron %d transacciones.\n", n);
    float min_monto, max_monto, min_cant, max_cant, min_desc, max_desc;
    obtener_min_max_monto(datos, n, &min_monto, &max_monto);
    obtener_min_max_cantidad(datos, n, &min_cant, &max_cant);
    obtener_min_max_descuento(datos, n, &min_desc, &max_desc);
    pthread_t hilos[NUM_HILOS];
    ArgHilo args[NUM_HILOS];
    int bloque = n / NUM_HILOS;
    printf("\n=== MODO PARALELO (%d hilos) ===\n", NUM_HILOS);
    struct timeval t0, t1;
    gettimeofday(&t0, NULL);
    for (int i = 0; i < NUM_HILOS; i++) {
        args[i].datos = datos;
        args[i].inicio = i * bloque;
        args[i].fin = (i == NUM_HILOS - 1) ? n : (i + 1) * bloque;
        args[i].id_hilo = i + 1;
        args[i].min_monto = min_monto;
        args[i].max_monto = max_monto;
        args[i].min_cant = min_cant;
        args[i].max_cant = max_cant;
        args[i].min_desc = min_desc;
        args[i].max_desc = max_desc;
        pthread_create(&hilos[i], NULL, procesar_bloque, &args[i]);
    }
    for (int i = 0; i < NUM_HILOS; i++)
        pthread_join(hilos[i], NULL);
    gettimeofday(&t1, NULL);
    double total = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1e6;
    printf("---------------------------\n");
    printf("TIEMPO TOTAL PARALELO: %.6f segundos\n", total);
    printf("\n--- Muestra de los primeros 5 registros PROCESADOS ---\n");
    for (int i = 0; i < (n < 5 ? n : 5); i++) {
        printf("ID %d | %s | %s | Monto(%.3f) | Cant(%.3f) | %s | Desc(%.3f)\n",
               datos[i].id, datos[i].fecha, datos[i].categoria,
               datos[i].monto, datos[i].cantidad,
               datos[i].region, datos[i].descuento);
    }
    pthread_mutex_destroy(&mutex);
    liberar_transacciones(datos);
    return 0;
}
