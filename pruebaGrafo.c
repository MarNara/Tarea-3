#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tdas/list.h"
#include "tdas/map.h"
#include "tdas/heap.h"
#include "tdas/extra.h"

#define MAX_LINEA 1024
#define MAX_ITEMS 10

int compararClaves_strcmp(void* a, void* b) {
    return strcmp((char*)a, (char*)b);
}

// ------------------ ESTRUCTURAS ------------------

typedef struct Juego_Item {
    char nombre[50];
    int valor;
    int peso;
} Juego_Item;

typedef struct Nodo {
    char id[20];
    char nombre[100];
    char descripcion[256];
    struct Nodo* arriba;
    struct Nodo* abajo;
    struct Nodo* izquierda;
    struct Nodo* derecha;
    int esFinal;
    List* items;
} Nodo;



void cargarEscenarios(const char* nombreArchivo, Map* mapaNodos, Nodo** nodoInicial) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo.\n");
        *nodoInicial = NULL;
        return;
    }

    char linea[1024];
    fgets(linea, sizeof(linea), archivo); // Saltar encabezado

    while (fgets(linea, sizeof(linea), archivo)) {
        Nodo* nodo = malloc(sizeof(Nodo));
        if (!nodo) continue;

        char idStr[20], nombre[100], descripcion[256];
        char itemsStr[256], arribaID[20], abajoID[20], izquierdaID[20], derechaID[20], esFinalStr[10];

        sscanf(linea, "%19[^,],%99[^,],%255[^,],%255[^,],%19[^,],%19[^,],%19[^,],%19[^,],%9[^\n]",
               idStr, nombre, descripcion, itemsStr, arribaID, abajoID, izquierdaID, derechaID, esFinalStr);

        strcpy(nodo->id, trim(idStr));
        strcpy(nodo->nombre, nombre);
        strcpy(nodo->descripcion, descripcion);
        nodo->esFinal = (strcmp(esFinalStr, "Si") == 0 || strcmp(esFinalStr, "si") == 0) ? 1 : 0;

        nodo->arriba = (strcmp(arribaID, "-1") != 0) ? (Nodo*)strdup(trim(arribaID)) : NULL;
        nodo->abajo = (strcmp(abajoID, "-1") != 0) ? (Nodo*)strdup(trim(abajoID)) : NULL;
        nodo->izquierda = (strcmp(izquierdaID, "-1") != 0) ? (Nodo*)strdup(trim(izquierdaID)) : NULL;
        nodo->derecha = (strcmp(derechaID, "-1") != 0) ? (Nodo*)strdup(trim(derechaID)) : NULL;

        nodo->items = list_create();
        if (strlen(itemsStr) > 0) {
            List* listaTokens = split_string(itemsStr, ";");

            char* itemStr = list_first(listaTokens);
            while (itemStr != NULL) {
                List* partes = split_string(itemStr, ",");

                char* nombre = list_first(partes);
                char* valorStr = list_next(partes);
                char* pesoStr = list_next(partes);

                if (nombre) {
                    Juego_Item* nuevo = malloc(sizeof(Juego_Item));
                    strncpy(nuevo->nombre, nombre, sizeof(nuevo->nombre) - 1);
                    nuevo->nombre[sizeof(nuevo->nombre) - 1] = '\0';
                    nuevo->valor = valorStr ? atoi(valorStr) : 0;
                    nuevo->peso = pesoStr ? atoi(pesoStr) : 0;
                    list_pushBack(nodo->items, nuevo);
                }

                char* ptr = list_first(partes);
                while (ptr != NULL) {
                    free(ptr);
                    ptr = list_next(partes);
                }
                free(partes);

                itemStr = list_next(listaTokens);
            }

            char* ptr = list_first(listaTokens);
            while (ptr != NULL) {
                free(ptr);
                ptr = list_next(listaTokens);
            }
            free(listaTokens);
        }

        char* clave = strdup(nodo->id);
        map_insert(mapaNodos, clave, nodo);
    }

    fclose(archivo);

    // Resolver conexiones (punteros)
    MapPair* par = map_first(mapaNodos);
    while (par != NULL) {
        Nodo* nodo = (Nodo*)par->value;

        char* arribaID = (char*)nodo->arriba;
        char* abajoID = (char*)nodo->abajo;
        char* izquierdaID = (char*)nodo->izquierda;
        char* derechaID = (char*)nodo->derecha;

        MapPair* buscado;
        nodo->arriba = (arribaID && (buscado = map_search(mapaNodos, trim(arribaID)))) ? buscado->value : NULL;
        nodo->abajo = (abajoID && (buscado = map_search(mapaNodos, trim(abajoID)))) ? buscado->value : NULL;
        nodo->izquierda = (izquierdaID && (buscado = map_search(mapaNodos, trim(izquierdaID)))) ? buscado->value : NULL;
        nodo->derecha = (derechaID && (buscado = map_search(mapaNodos, trim(derechaID)))) ? buscado->value : NULL;

        par = map_next(mapaNodos);
    }

    // Buscar nodo inicial (ID "1" limpio con trim)
    char claveInicial[] = "1";
    MapPair* inicio = map_search(mapaNodos, trim(claveInicial));
    *nodoInicial = (inicio) ? (Nodo*)inicio->value : NULL;
}




void mostrarNodo(Nodo* nodo) {
    if (!nodo) return;

    printf("==== Nodo ID: %s ====\n", nodo->id);
    printf("Nombre: %s\n", nodo->nombre);
    printf("Descripción: %s\n", nodo->descripcion);
    printf("Es final: %s\n", nodo->esFinal ? "Sí" : "No");

    printf("Conexiones:\n");
    printf("  Arriba: %s\n", nodo->arriba ? nodo->arriba->id : "Ninguna");
    printf("  Abajo: %s\n", nodo->abajo ? nodo->abajo->id : "Ninguna");
    printf("  Izquierda: %s\n", nodo->izquierda ? nodo->izquierda->id : "Ninguna");
    printf("  Derecha: %s\n", nodo->derecha ? nodo->derecha->id : "Ninguna");

    printf("Ítems:\n");
    void* itemPtr = list_first(nodo->items);
    while (itemPtr != NULL) {
        Juego_Item* item = (Juego_Item*)itemPtr;
        printf("  - %s (valor: %d, peso: %d)\n", item->nombre, item->valor, item->peso);
        itemPtr = list_next(nodo->items);
    }

    printf("\n");
}



int main() {
    Map* mapaNodos = map_create(compararClaves_strcmp);
    Nodo* inicio = NULL;
    cargarEscenarios("graphquest.csv", mapaNodos, &inicio);

    if (!inicio) {
    printf("Error: no se pudo cargar el nodo inicial.\n");

    // Mostrar las claves almacenadas en el mapa
    printf("\nClaves en el mapa:\n");
    MapPair* par = map_first(mapaNodos);
    while (par != NULL) {
        printf("  -> '%s'\n", (char*)par->key);
        par = map_next(mapaNodos);
    }

    return 1;
}


    mostrarNodo(inicio);
    return 0;
}


/*
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
}*/



//gcc tdas/*.c pruebaGrafo.c -Wno-unused-result -o pruebaGrafo
//./pruebaGrafo