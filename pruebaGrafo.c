#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINEA 1024
#define MAX_ITEMS 10

typedef struct {
    char nombre[50];
    int valor;
    int peso;
} Item;

typedef struct Nodo {
    int id;
    char nombre[100];
    char descripcion[256];
    Item items[MAX_ITEMS];
    int num_items;
    int arriba;
    int abajo;
    int izquierda;
    int derecha;
    int esFinal;
    struct Nodo* siguiente;
} Nodo;

// esta funcion se encarga de leer items separados por ';' y dentro cada item por ',' para luego separar cada uno como un valor
void separar_items(char* str, Item* items, int* num_items) {
    *num_items = 0;
    char* token = strtok(str, ";");
    while (token != NULL && *num_items < MAX_ITEMS) {
        // Cada token es algo tipo "Cuchillo,3,1"
        char* subtoken = strtok(token, ",");
        if (subtoken) {
            strncpy(items[*num_items].nombre, subtoken, 49);
            items[*num_items].nombre[49] = '\0';

            subtoken = strtok(NULL, ",");
            if (subtoken != NULL) {
                items[*num_items].valor = atoi(subtoken);
            } else {
                items[*num_items].valor = 0;
            }

            subtoken = strtok(NULL, ",");
            if (subtoken != NULL) {
                items[*num_items].peso = atoi(subtoken);
            } else {
                items[*num_items].peso = 0;
            }

            (*num_items)++;
        }
        token = strtok(NULL, ";");
    }
}

Nodo* cargarEscenarios(const char* nombreArchivo) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        printf("Error al abrir el archivo.\n");
        return NULL;
    }

    char linea[MAX_LINEA];
    // Leer encabezado y descartarlo
    fgets(linea, MAX_LINEA, archivo);

    Nodo* lista = NULL;

    while (fgets(linea, MAX_LINEA, archivo)) {
        linea[strcspn(linea, "\r\n")] = 0;

        char* campos[9];
        int campoActual = 0;
        char* ptr = linea;//la linea de caracteres, la que contiene la descripción
        int enComillas = 0;
        char campoTemp[MAX_LINEA] = {0};
        int j = 0;

        int i = 0;
        while (ptr[i] != '\0') {//recorrer la linea de la descripción
            if (ptr[i] == '"') {
                if (enComillas){
                    enComillas = 0;
                }else{
                    enComillas = 1;
                }
                     
            } else if (ptr[i] == ',' && !enComillas) {
                campoTemp[j] = '\0';
                campos[campoActual] = malloc(strlen(campoTemp) + 1);
                strcpy(campos[campoActual++], campoTemp);
                j = 0;
                campoTemp[0] = '\0';
            } else {
                campoTemp[j++] = ptr[i];
            }
            i++;
        }


        // Último campo
        campoTemp[j] = '\0';
        campos[campoActual] = malloc(strlen(campoTemp) + 1);
        strcpy(campos[campoActual++], campoTemp);

        if (campoActual != 9) {
            printf("Línea malformada: %s\n", linea);
            for (int i = 0; i < campoActual; i++) free(campos[i]);
            continue;
        }

        Nodo* nodo = (Nodo*)malloc(sizeof(Nodo));
        nodo->id = atoi(campos[0]);
        strncpy(nodo->nombre, campos[1], 99);
        nodo->nombre[99] = '\0';
        strncpy(nodo->descripcion, campos[2], 255);
        nodo->descripcion[255] = '\0';

        nodo->num_items = 0;
        if (strlen(campos[3]) > 0) {
            separar_items(campos[3], nodo->items, &(nodo->num_items));
        }

        nodo->arriba = atoi(campos[4]);
        nodo->abajo = atoi(campos[5]);
        nodo->izquierda = atoi(campos[6]);
        nodo->derecha = atoi(campos[7]);
        nodo->esFinal = (strcmp(campos[8], "Si") == 0 || strcmp(campos[8], "si") == 0);

        // Insertar a la lista enlazada
        nodo->siguiente = lista;
        lista = nodo;

        // Liberar campos
        for (int i = 0; i < 9; i++) {
            free(campos[i]);
        }
    }

    fclose(archivo);
    return lista;
}

void imprimirEscenarios(Nodo* lista) {
    Nodo* actual = lista;
    while (actual != NULL) {
        printf("ID: %d\nNombre: %s\nDescripcion: %s\nItems:\n", actual->id, actual->nombre, actual->descripcion);
        for (int i = 0; i < actual->num_items; i++) {
            printf("  - %s (valor %d, peso %d)\n", actual->items[i].nombre, actual->items[i].valor, actual->items[i].peso);
        }
        printf("Arriba: %d, Abajo: %d, Izquierda: %d, Derecha: %d\n", actual->arriba, actual->abajo, actual->izquierda, actual->derecha);
        printf("EsFinal: %d\n\n", actual->esFinal);
        actual = actual->siguiente;
    }
}

int main() {
    Nodo* escenarios = cargarEscenarios("graphquest.csv");
    if (!escenarios) {
        return 1;
    }

    imprimirEscenarios(escenarios);

    // Aquí deberías liberar memoria cuando ya no necesites los nodos

    return 0;
}

//gcc tdas/*.c pruebaGrafo.c -Wno-unused-result -o pruebaGrafo
//./pruebaGrafo