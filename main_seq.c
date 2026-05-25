#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "leer_csv.h"
#include "procesamiento.h"

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
    
  
    struct timeval inicio, fin;
    gettimeofday(&inicio, NULL);
   
    float min_monto, max_monto, min_cant, max_cant, min_desc, max_desc;
    obtener_min_max_monto(datos, n, &min_monto, &max_monto);
    obtener_min_max_cantidad(datos, n, &min_cant, &max_cant);
    obtener_min_max_descuento(datos, n, &min_desc, &max_desc);
    

    limpiar_nulos_numericos(datos, 0, n);
    imputar_moda_categoricos(datos, 0, n);
    normalizar(datos, 0, n,
               min_monto, max_monto,
               min_cant, max_cant,
               min_desc, max_desc);
    
    gettimeofday(&fin, NULL);
    double tiempo_seq = (fin.tv_sec - inicio.tv_sec) + 
                        (fin.tv_usec - inicio.tv_usec) / 1e6;
    
    printf("Tiempo total de procesamiento SECUENCIAL: %.6f segundos\n", tiempo_seq);
    

    printf("\n--- Muestra de los primeros 5 registros PROCESADOS ---\n");
    for (int i = 0; i < (n < 5 ? n : 5); i++) {
        printf("ID %d | %s | %s | Monto(%.3f) | Cantidad(%.3f) | %s | Descuento(%.3f)\n",
               datos[i].id, datos[i].fecha, datos[i].categoria,
               datos[i].monto, datos[i].cantidad, datos[i].region, datos[i].descuento);
    }
    
    liberar_transacciones(datos);
    return 0;
}