#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/map.h"
#include <string.h>

// funcion de comparación para los ids de los escenarios
int comparar_claves_ids(void* id1, void* id2) {
    int* a = (int*)id1;
    int* b = (int*)id2;
    return *a == *b;
}


//----------------------------------------------------------Estructuras-------------------------------------------------------------
// estructura del item
typedef struct {
    char* nombre;
    int valor;
    int peso;
    int id_item;
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
    struct Juego** vecinos; // Para el grafo
} Juego;

// estructura del grafo con mapa 
typedef struct {
    Juego** escenarios;//estrcutura de los datos del juego
    int num_escena;//para usar el for, ya que no guarde item en list, solo en un array
    Juego* inicio;
} MapaDelEsc;

//estructura de los datos del jugador
typedef struct{
    int tiempo;
    List* inventario;
    Juego* actual;// en el que esta el jugador
    int totalPeso;
    int totalPuntaje; 
} Jugador;

//-----------------------------------inicializar estructuras para evitar errores--------------------------------------------------


// inicializar la estructura item aparte para desglosar el cargar archivos
Item* inicializar_item(char* nombre, int valor, int peso, int id_DelItem) {
    Item* item = (Item*)malloc(sizeof(Item));
    item->nombre = strdup(nombre);
    item->valor = valor;
    item->peso = peso;
    item->id_item = id_DelItem;
    return item;
}
// esta funcion inicializa la estructura item
Juego* inicializar_escenario(int id, char* nombre, char* descripcion) {
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
    esc->vecinos = NULL;
    return esc;
}

// esta funcion inicializa la estructura item
MapaDelEsc* inicializar_mapa() {
    MapaDelEsc* mapa = (MapaDelEsc*)malloc(sizeof(MapaDelEsc));
    mapa->escenarios = NULL;
    mapa->num_escena = 0;
    mapa->inicio = NULL;
    return mapa;
}


/*int tiempo;
    List* inventario;
    Juego* actual;// en el que esta el jugador
    int totalPeso;
    int totalPuntaje; 
} Jugador;

//inicializar jugador (si alcanzo arreglarlo bonito, acá, si no, dejar el main feo)
Jugador* inicializar_jugador(int tiempo, Juego* actual, int totalPeso, int totalPuntaje) {
    Jugador* jugador_naranja = (Jugador*)malloc(sizeof(Jugador));
    jugador_naranja->tiempo = 10;
    jugador_naranja->inventario = list_create();
    jugador_naranja->actual = ;

    return Jugador;
}*/
 /*
// Función para liberar un ítem
void liberar_item(Item* item) {
    free(item->nombre);
    free(item);
}

// Función para liberar un escenario
void liberar_escenario(Juego* esc) {
    free(esc->nombre);
    free(esc->descripcion);
    for (int i = 0; i < esc->num_items; i++) {
        liberar_item(esc->items[i]);
    }
    free(esc->items);
    free(esc->vecinos);
    free(esc);
}


//------------------------crear un liberar memoria para map ya que me da error si solo le coloco free al liberarlo al final
// Función para liberar un mapa
void free_map(MapaDelEsc* mapa) {
    for (int i = 0; i < mapa->num_escena; i++) {
        free(mapa->escenarios[i]);//liberar_escenario(mapa->escenarios[i]);
    }
    free(mapa->escenarios);
    free(mapa);
}*/

// Función para agregar un escenario al mapa
void agregar_escenario(MapaDelEsc* mapa, Juego* esc) {
    mapa->num_escena++;
    mapa->escenarios = realloc(mapa->escenarios, mapa->num_escena * sizeof(Juego*));
    mapa->escenarios[mapa->num_escena - 1] = esc;
    
    // Si es el primer escenario lo colocamos como el inicio de la partida
    if (mapa->num_escena == 1) {
        mapa->inicio = esc;
    }
}

//cargar los archivos y convertir el grafo en un mapa
MapaDelEsc* cargar_archivo() {
    MapaDelEsc* mapa = inicializar_mapa();
    
    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return mapa;
    }

    char **campos;
    campos = leer_linea_csv(archivo, ','); // Lee los encabezados

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        // Crear nuevo escenario
        Juego* esc = inicializar_escenario(atoi(campos[0]), campos[1], campos[2]);
        
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
            
            esc->items[item_idx++] = inicializar_item(nombre, valor, peso, esc->id);
            list_clean(values);
            free(values);
        }
        list_clean(items_list);
        free(items_list);
        
        // Establecer vecinos
        esc->arriba = atoi(campos[4]);
        esc->abajo = atoi(campos[5]);
        esc->izquierda = atoi(campos[6]);
        esc->derecha = atoi(campos[7]);
        
        esc->es_final = atoi(campos[8]);
        
        // Agregar al mapa (hacer una función para que sea menos ruido visual y me confuna menos)
        agregar_escenario(mapa, esc);
    }
    
    fclose(archivo);
    
    // falta establecer vecinos entre escenarios
    for (int i = 0; i < mapa->num_escena; i++) {
        Juego* actual = mapa->escenarios[i];
        // reservar espacio para 4 vecinos (arriba, abajo, izquierda, derecha)
        actual->vecinos = malloc(4 * sizeof(Juego*));
        
        // buscar escenarios vecinos
        for (int j = 0; j < mapa->num_escena; j++) {
            Juego* actual_escenarios = mapa->escenarios[j];
            if (actual_escenarios->id == actual->arriba) actual->vecinos[0] = actual_escenarios;
            if (actual_escenarios->id == actual->abajo) actual->vecinos[1] = actual_escenarios;
            if (actual_escenarios->id == actual->izquierda) actual->vecinos[2] = actual_escenarios;
            if (actual_escenarios->id == actual->derecha) actual->vecinos[3] = actual_escenarios;
        }
    }
    printf("El archivo se a cargado correctamente\n");
    presioneTeclaParaContinuar();
    return mapa;
    
}


//mostraaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaar
/*void mostrar_escenario_actual(Jugador* datos_jugador){
    puts("========================================");
    puts("     Estado Actual");
    puts("========================================");
    

    Organización: 1)imprimir la descripcion del escenario, para eso necesito el nombre.

    2)luego debo mostrar la lista de items disponibles que tiene el escenario.

    3)despues debo mostrar el tiempo que le queda al jugador para hacer su proximo movimiento
    (Esto se va actualizando solo??).
     
    4)Luego debo mostrar el inventario del jugador (ítems recogidos,
     peso total(es tambien una suma??) y puntaje acumulado(el puntaje es la suma de los valores de los items??))
     
    5)Por ultimo mostrar las Acciones posibles desde este escenario: direcciones disponibles (arriba, abajo, izquierda, 
     derecha).
}*/
void mostrar_escenario_actual(Jugador* datos_jugador) {
    if (datos_jugador->actual == NULL) {
        printf("No hay escenario actual asignado.\n");
        return;
    }

    Juego* actual = datos_jugador->actual;
    
    puts("\n========================================");
    puts("     Estado Actual");
    puts("========================================");
    
    // 1) Información del escenario
    printf("Nombre Del Escenario: %s\n", actual->nombre);
    printf("Descripción: %s\n\n", actual->descripcion);
    
    // 2) Items disponibles
    // Mostrar items (usando array en lugar de List)
    printf("\nItems disponibles:\n");
    if (actual->num_items == 0) {
        printf("No hay items en este escenario.\n");
    } else {
        for (int i = 0; i < actual->num_items; i++) {
            Item* item = actual->items[i];
            printf("- %s (Valor: %d, Peso: %d)\n", 
                   item->nombre, item->valor, item->peso);
        }
    }
    
    // 3) Tiempo restante
    printf("\nTiempo restante: %d\n", datos_jugador->tiempo);
    
    // 4) Inventario
    printf("\n=== TU INVENTARIO ===\n");
    if (list_first(datos_jugador->inventario) == NULL) {
        printf("Inventario vacío.\n");
    } else {
        // Esto es correcto porque inventario es List*
        for(Item* item = list_first(datos_jugador->inventario); 
            item != NULL; 
            item = list_next(datos_jugador->inventario)) {
            printf("- %s (Valor: %d, Peso: %d)\n", 
                   item->nombre, item->valor, item->peso);
        }
    }
    
    // 5) Direcciones disponibles
    printf("\n------ Direcciones disponibles ------\n");
    if (actual->arriba != -1) printf("1) Arriba\n");
    if (actual->abajo != -1) printf("2) Abajo\n");
    if (actual->izquierda != -1) printf("3) Izquierda\n");
    if (actual->derecha != -1) printf("4) Derecha\n");
}



void recoger_items(Jugador* jugador, const char* nombre_item) {
    Juego* escenario = jugador->actual;
    Item* item_encontrado = NULL;
    int indice_item = -1;
    
    // Buscar el item por nombre
    for (int i = 0; i < escenario->num_items; i++) {
        if (strcmp(escenario->items[i]->nombre, nombre_item) == 0) {
            item_encontrado = escenario->items[i];
            indice_item = i;
            break;
        }
    }
    
    if (item_encontrado == NULL) {
        printf("Item no encontrado: %s\n", nombre_item);
        return;
    }
    
    // Verificar peso máximo (ejemplo: 100)
    if (jugador->totalPeso + item_encontrado->peso > 100) {
        printf("¡No puedes cargar más peso! (Peso actual: %d/100)\n", jugador->totalPeso);
        return;
    }
    
    // Agregar al inventario
    list_pushBack(jugador->inventario, item_encontrado);
    jugador->totalPeso += item_encontrado->peso;
    jugador->totalPuntaje += item_encontrado->valor;
    
    // Eliminar del escenario
    for (int i = indice_item; i < escenario->num_items - 1; i++) {
        escenario->items[i] = escenario->items[i + 1];
    }
    escenario->num_items--;
    
    printf("¡Has recogido %s! (+%d pts, +%d kg)\n", 
           nombre_item, item_encontrado->valor, item_encontrado->peso);
}



void avanzar_una_direccion(Jugador* datos_jugador, MapaDelEsc* mapa_juego, int direccion) {
    Juego* escenario_actual = datos_jugador->actual;
    Juego* siguiente = NULL;
    
    // Determinar dirección
    switch(direccion) {
        case 1: // Arriba
            siguiente = escenario_actual->vecinos[0];
            break;
        case 2: // Abajo
            siguiente = escenario_actual->vecinos[1];
            break;
        case 3: // Izquierda
            siguiente = escenario_actual->vecinos[2];
            break;
        case 4: // Derecha
            siguiente = escenario_actual->vecinos[3];
            break;
        default:
            printf("Dirección no válida\n");
            return;
    }
    
    if(siguiente != NULL) {
        datos_jugador->actual = siguiente;
        printf("\nHas llegado a: %s\n", siguiente->nombre);
    } else {
        printf("\n¡No puedes avanzar en esa dirección!\n");
    }
}

int main() {
    int opcion;
    int archivo_cargado = 0;
    MapaDelEsc* mapa_del_juego = NULL;  // Inicializado como NULL
    
    // Inicializar jugador con valores por defecto
    Jugador* datos_jugador = (Jugador*)malloc(sizeof(Jugador));
    if(datos_jugador == NULL) return EXIT_FAILURE;
    
    datos_jugador->tiempo = 10;
    datos_jugador->inventario = list_create();
    datos_jugador->actual = NULL;  // Inicialmente no hay escenario
    datos_jugador->totalPeso = 0;
    datos_jugador->totalPuntaje = 0; 
    
    do {
        limpiarPantalla();
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1. Cargar Laberinto desde Archivo CSV\n");
        printf("2. Iniciar Partida\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1: {
                limpiarPantalla();
                printf("\n--- Cargando laberinto desde archivo CSV ---\n");
                
                // Liberar mapa anterior si existe
                if(mapa_del_juego != NULL) {
                    free(mapa_del_juego);
                }
                
                mapa_del_juego = cargar_archivo();
                
                if(mapa_del_juego != NULL && mapa_del_juego->inicio != NULL) {
                    archivo_cargado = 1;
                    datos_jugador->actual = mapa_del_juego->inicio;  // Asignar escenario inicial
                    printf("Mapa cargado correctamente. Escenario inicial: %s\n", mapa_del_juego->inicio->nombre);
                } else {
                    printf("Error al cargar el mapa.\n");
                    archivo_cargado = 0;
                }
                break;
            }

            case 2: {
                limpiarPantalla();
                if (!archivo_cargado) {
                    printf("¡Debes cargar el archivo CSV antes de comenzar la partida!\n");
                } else {
                    printf("=== PARTIDA INICIADA ===\n");
                    
                    char opcion2;
                    do {
                        mostrar_escenario_actual(datos_jugador);
                        
                        printf("\n***** MENÚ DEL JUEGO ******\n");
                        puts("========================================");
                        puts("     Opciones del Jugador");
                        puts("========================================");
                        puts("1) Recoger Ítem(s)");
                        puts("2) Descartar Ítem(s)");
                        puts("3) Avanzar en una Dirección");
                        puts("4) Reiniciar Partida");
                        puts("5) Salir");
                        printf("Ingrese su opción: ");
                        scanf(" %c", &opcion2);
                        

                        switch (opcion2) {
                            case '1':
                                limpiarPantalla();
                                //recoger_items(datos_jugador, nombre_item);
                                
                                break;
                                
                            case '2':
                                limpiarPantalla();
                                
                                
                                break;
                                
                            case '3':
                                limpiarPantalla();
                                printf("Direcciones disponibles:\n");
                                if (datos_jugador->actual->arriba != -1) printf("1) Arriba\n");
                                if (datos_jugador->actual->abajo != -1) printf("2) Abajo\n");
                                if (datos_jugador->actual->izquierda != -1) printf("3) Izquierda\n");
                                if (datos_jugador->actual->derecha != -1) printf("4) Derecha\n");
                                
                                int direccion;
                                printf("Seleccione dirección: ");
                                scanf("%d", &direccion);
                                avanzar_una_direccion(datos_jugador, mapa_del_juego, direccion);
                                break;
                                
                            case '4':
                                break;
                        }
                        
                        presioneTeclaParaContinuar();
                        limpiarPantalla();

                    } while (opcion2 != '5'); 
                }
                break;
            }
            case 0:
                printf("Saliendo del juego. ¡Hasta luego!\n");
                break;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }

    } while (opcion != 0);

    // Liberar memoria
    if(mapa_del_juego != NULL) {
        free(mapa_del_juego);
    }
    
    return 0;
}


//gcc tdas/*.c pruebaGrafo.c -Wno-unused-result -o pruebaGrafo
//./pruebaGrafo