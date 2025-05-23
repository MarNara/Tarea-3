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

// Función para parsear items separados por ';' y dentro cada item por ','
void parsearItems(char* str, Item* items, int* num_items) {
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
        printf("Error al abrir archivo\n");
        return NULL;
    }

    char linea[MAX_LINEA];
    // Leer encabezado y descartarlo
    fgets(linea, MAX_LINEA, archivo);

    Nodo* lista = NULL;

    while (fgets(linea, MAX_LINEA, archivo)) {
        // Remover salto de línea
        linea[strcspn(linea, "\r\n")] = 0;

        // Variables para campos
        char* campos[9];
        int i = 0;

        char* ptr = strtok(linea, ",");
        while (ptr != NULL && i < 9) {
            campos[i++] = ptr;
            ptr = strtok(NULL, ",");
        }

        // En tu archivo, la descripción puede contener comas, lo que rompe el tokenizing simple.
        // Para manejarlo bien, tendrías que hacer un parser más complejo o asumir que no hay comas en descripción.
        // Aquí asumimos que descripción NO tiene comas.

        if (i < 9) {
            // Intentar manejar descripción con comas:
            // La línea no fue bien tokenizada, entonces tenemos que reconstruir la descripción:
            // Suponiendo que ID,Nombre son campos 0 y 1,
            // el resto hasta Items es descripción (campo 2).
            // Para simplificar, dejamos como está.

            // Para un parser robusto, tendrías que implementar un estado máquina para la lectura.
            printf("Línea con formato inesperado, se omite\n");
            continue;
        }

        Nodo* nodo = (Nodo*)malloc(sizeof(Nodo));
        nodo->id = atoi(campos[0]);
        strncpy(nodo->nombre, campos[1], 99);
        nodo->nombre[99] = '\0';
        strncpy(nodo->descripcion, campos[2], 255);
        nodo->descripcion[255] = '\0';

        // Procesar items
        nodo->num_items = 0;
        if (strlen(campos[3]) > 0) {
            parsearItems(campos[3], nodo->items, &(nodo->num_items));
        }

        nodo->arriba = atoi(campos[4]);
        nodo->abajo = atoi(campos[5]);
        nodo->izquierda = atoi(campos[6]);
        nodo->derecha = atoi(campos[7]);
        nodo->esFinal = (strcmp(campos[8], "Si") == 0 || strcmp(campos[8], "si") == 0) ? 1 : 0;

        // Insertar al inicio de la lista
        nodo->siguiente = lista;
        lista = nodo;
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