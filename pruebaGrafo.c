#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/map.h"
#include <string.h>

// funcion de comparación para los ids de los escenarios
int compare_escenario_ids(void* id1, void* id2) {
    int* a = (int*)id1;
    int* b = (int*)id2;
    return *a == *b;
}

// estructura del item
typedef struct {
    char* nombre;
    int valor;
    int peso;
} Item;


// estructura del juego, los nodo/escenario
typedef struct Juego {
    int id;
    char* nombre;
    char* descripcion;
    Item** items;
    int num_items;
    int arriba;
    int abajo;
    int izquierda;
    int derecha;
    int es_final;
    struct Juego** conexiones; // Para el grafo
} Juego;

// estructura del grafo con mapa 
typedef struct {
    Juego** escenarios;
    int num_escenarios;
    Juego* inicio;
} MapaDelEsc;



// inicializar la estructura item aparte para desglosar el cargar archivos
Item* crear_item(char* nombre, int valor, int peso) {
    Item* item = (Item*)malloc(sizeof(Item));
    item->nombre = strdup(nombre);
    item->valor = valor;
    item->peso = peso;
    return item;
}

// Función para liberar un ítem
void liberar_item(Item* item) {
    free(item->nombre);
    free(item);
}


// esta funcion inicializa la estructura item
Juego* crear_escenario(int id, char* nombre, char* descripcion) {
    Juego* esc = (Juego*)malloc(sizeof(Juego));
    esc->id = id;
    esc->nombre = strdup(nombre);
    esc->descripcion = strdup(descripcion);
    esc->items = NULL;
    esc->num_items = 0;
    esc->arriba = -1;
    esc->abajo = -1;
    esc->izquierda = -1;
    esc->derecha = -1;
    esc->es_final = 0;
    esc->conexiones = NULL;
    return esc;
}

// Función para liberar un escenario
void liberar_escenario(Juego* esc) {
    free(esc->nombre);
    free(esc->descripcion);
    for (int i = 0; i < esc->num_items; i++) {
        liberar_item(esc->items[i]);
    }
    free(esc->items);
    free(esc->conexiones);
    free(esc);
}

// esta funcion inicializa la estructura item
MapaDelEsc* crear_mapa() {
    MapaDelEsc* mapa = (MapaDelEsc*)malloc(sizeof(MapaDelEsc));
    mapa->escenarios = NULL;
    mapa->num_escenarios = 0;
    mapa->inicio = NULL;
    return mapa;
}

// Función para liberar un mapa
void liberar_mapa(MapaDelEsc* mapa) {
    for (int i = 0; i < mapa->num_escenarios; i++) {
        liberar_escenario(mapa->escenarios[i]);
    }
    free(mapa->escenarios);
    free(mapa);
}

// Función para agregar un escenario al mapa
void agregar_escenario(MapaDelEsc* mapa, Juego* esc) {
    mapa->num_escenarios++;
    mapa->escenarios = realloc(mapa->escenarios, mapa->num_escenarios * sizeof(Juego*));
    mapa->escenarios[mapa->num_escenarios - 1] = esc;
    
    // Si es el primer escenario lo colocamos como el inicio de la partida
    if (mapa->num_escenarios == 1) {
        mapa->inicio = esc;
    }
}

MapaDelEsc* cargar_archivo() {
    MapaDelEsc* mapa = crear_mapa();
    
    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return mapa;
    }

    char **campos;
    campos = leer_linea_csv(archivo, ','); // Lee los encabezados

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        // Crear nuevo escenario
        Juego* esc = crear_escenario(atoi(campos[0]), campos[1], campos[2]);
        
        // Procesar items
        List* items_list = split_string(campos[3], ";");
        esc->num_items = list_size(items_list);
        esc->items = malloc(esc->num_items * sizeof(Item*));
        
        int item_idx = 0;
        for(char *item_str = list_first(items_list); item_str != NULL; item_str = list_next(items_list)) {
            List* values = split_string(item_str, ",");
            char* nombre = list_first(values);
            int valor = atoi(list_next(values));
            int peso = atoi(list_next(values));
            
            esc->items[item_idx++] = crear_item(nombre, valor, peso);
            list_clean(values);
            free(values);
        }
        list_clean(items_list);
        free(items_list);
        
        // Establecer conexiones
        esc->arriba = atoi(campos[4]);
        esc->abajo = atoi(campos[5]);
        esc->izquierda = atoi(campos[6]);
        esc->derecha = atoi(campos[7]);
        
        esc->es_final = atoi(campos[8]);
        
        // Agregar al mapa
        agregar_escenario(mapa, esc);
    }
    
    fclose(archivo);
    
    // falta establecer conexiones entre escenarios
    for (int i = 0; i < mapa->num_escenarios; i++) {
        Juego* actual = mapa->escenarios[i];
        // reservar espacio para 4 conexiones (arriba, abajo, izquierda, derecha)
        actual->conexiones = malloc(4 * sizeof(Juego*));
        
        // buscar escenarios vecinos
        for (int j = 0; j < mapa->num_escenarios; j++) {
            Juego* otro = mapa->escenarios[j];
            if (otro->id == actual->arriba) actual->conexiones[0] = otro;
            if (otro->id == actual->abajo) actual->conexiones[1] = otro;
            if (otro->id == actual->izquierda) actual->conexiones[2] = otro;
            if (otro->id == actual->derecha) actual->conexiones[3] = otro;
        }
    }
    
    return mapa;
}

/*
void mostrar_mapa(Mapa* mapa) {
    printf("=== MAPA DEL JUEGO ===\n");
    printf("Total de escenarios: %d\n", mapa->num_escenarios);
    printf("Escenario inicial: %d\n", mapa->inicio ? mapa->inicio->id : -1);
    
    for (int i = 0; i < mapa->num_escenarios; i++) {
        Escenario* esc = mapa->escenarios[i];
        printf("\nEscenario ID: %d\n", esc->id);
        printf("Nombre: %s\n", esc->nombre);
        printf("Descripción: %s\n", esc->descripcion);
        
        printf("Items (%d):\n", esc->num_items);
        for (int j = 0; j < esc->num_items; j++) {
            printf("  - %s (%d pts, %d kg)\n", esc->items[j]->nombre, 
                   esc->items[j]->valor, esc->items[j]->peso);
        }
        
        printf("Conexiones:\n");
        if (esc->arriba != -1) printf("  Arriba: %d\n", esc->arriba);
        if (esc->abajo != -1) printf("  Abajo: %d\n", esc->abajo);
        if (esc->izquierda != -1) printf("  Izquierda: %d\n", esc->izquierda);
        if (esc->derecha != -1) printf("  Derecha: %d\n", esc->derecha);
        
        if (esc->es_final) printf("ES ESCENARIO FINAL\n");
    }
}*/

void mostrar_mapa(MapaDelEsc* mapa) {
    printf("=== MAPA DEL JUEGO ===\n");
    printf("Total de escenarios: %d\n", mapa->num_escenarios);
    printf("Escenario inicial: %d\n", mapa->inicio ? mapa->inicio->id : -1);
    
    for (int i = 0; i < mapa->num_escenarios; i++) {
        Juego* esc = mapa->escenarios[i];
        printf("\nEscenario ID: %d\n", esc->id);
        printf("Nombre: %s\n", esc->nombre);
        printf("Descripción: %s\n", esc->descripcion);
        
        printf("Items (%d):\n", esc->num_items);
        for (int j = 0; j < esc->num_items; j++) {
            printf("  - %s (%d pts, %d kg)\n", esc->items[j]->nombre, 
                   esc->items[j]->valor, esc->items[j]->peso);
        }
        
        printf("Conexiones:\n");
        if (esc->arriba != -1) printf("  Arriba: %d\n", esc->arriba);
        if (esc->abajo != -1) printf("  Abajo: %d\n", esc->abajo);
        if (esc->izquierda != -1) printf("  Izquierda: %d\n", esc->izquierda);
        if (esc->derecha != -1) printf("  Derecha: %d\n", esc->derecha);
        
        if (esc->es_final) printf("ES ESCENARIO FINAL\n");
    }
}

/*
int main() {
    Mapa* mapa = cargar_archivo();
    mostrar_mapa(mapa);
    liberar_mapa(mapa);
    return 0;
}*/

int main() {
    MapaDelEsc* mapa = cargar_archivo();
    mostrar_mapa(mapa);
    liberar_mapa(mapa);
    // Liberar memoria (necesitarías implementar map_destroy)
    // ...
    
    return 0;
}


//gcc tdas/*.c pruebaGrafo.c -Wno-unused-result -o pruebaGrafo
//./pruebaGrafo