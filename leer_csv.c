#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leer_csv.h"


static int dividir_linea(const char* linea, char* campos[], int max_campos) {
    int num_campos = 0;
    const char* inicio = linea;
    const char* fin;
    
    while (num_campos < max_campos) {
        fin = strchr(inicio, ',');
        if (!fin) {

            int len = strlen(inicio);
            if (len > 0 && inicio[len-1] == '\n') len--;
            campos[num_campos] = malloc(len + 1);
            strncpy(campos[num_campos], inicio, len);
            campos[num_campos][len] = '\0';
            num_campos++;
            break;
        }
        int len = fin - inicio;
        campos[num_campos] = malloc(len + 1);
        strncpy(campos[num_campos], inicio, len);
        campos[num_campos][len] = '\0';
        num_campos++;
        inicio = fin + 1;
    }
    return num_campos;
}

Transaccion* leer_csv(const char* nombre_archivo, int* num_transacciones) {
    FILE* archivo = fopen(nombre_archivo, "r");
    if (!archivo) {
        perror("Error al abrir el archivo CSV");
        return NULL;
    }
    

    char buffer[1024];
    int total_lineas = 0;
    fgets(buffer, sizeof(buffer), archivo); 
    while (fgets(buffer, sizeof(buffer), archivo)) {
        total_lineas++;
    }
    rewind(archivo);
    fgets(buffer, sizeof(buffer), archivo); 
    
    // Reservar memoria para todas las transacciones
    Transaccion* datos = (Transaccion*)malloc(total_lineas * sizeof(Transaccion));
    if (!datos) {
        fclose(archivo);
        return NULL;
    }
    
    int idx = 0;
    while (fgets(buffer, sizeof(buffer), archivo) && idx < total_lineas) {
        char* campos[7];
        int n = dividir_linea(buffer, campos, 7);
        if (n != 7) {
      
            for (int i = 0; i < n; i++) free(campos[i]);
            continue;
        }
        

        datos[idx].id = atoi(campos[0]);
        

        strncpy(datos[idx].fecha, campos[1], 10);
        datos[idx].fecha[10] = '\0';
        

        strncpy(datos[idx].categoria, campos[2], MAX_CADENA-1);
        datos[idx].categoria[MAX_CADENA-1] = '\0';
        

        if (strlen(campos[3]) == 0) {
            datos[idx].monto = -1.0f;
        } else {
            datos[idx].monto = atof(campos[3]);
        }
        

        if (strlen(campos[4]) == 0) {
            datos[idx].cantidad = -1.0f;
        } else {
            datos[idx].cantidad = atof(campos[4]);
        }
        

        strncpy(datos[idx].region, campos[5], MAX_CADENA-1);
        datos[idx].region[MAX_CADENA-1] = '\0';
        

        if (strlen(campos[6]) == 0) {
            datos[idx].descuento = -1.0f;
        } else {
            datos[idx].descuento = atof(campos[6]);
        }
        

        for (int i = 0; i < 7; i++) free(campos[i]);
        idx++;
    }
    
    fclose(archivo);
    *num_transacciones = idx;
    return datos;
}

void liberar_transacciones(Transaccion* datos) {
    free(datos);
}