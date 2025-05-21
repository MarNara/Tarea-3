#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/map.h"
#include <string.h>

int comparar_claves(void* a, void* b) {
    return strcmp((char*)a, (char*)b);
}
// Definición de la estructura para el estado del puzzle
typedef struct {
    int square[3][3]; // Matriz 3x3 que representa el tablero
    int x;    // Posición x del espacio vacío
    int y;    // Posición y del espacio vacío
    List* actions; //Secuencia de movimientos para llegar al estado
} State;

typedef struct{
    char id[100];
    char nombre[100];
    char descripcion[1000];
    List* items;
    int arriba;
    int abajo;
    int izquierda;
    int derecha; 
    char esFinal[100];
} Juego;

typedef struct{
    char nombre[100];
    int valor;
    int peso;
}listaItems;

typedef struct{
    int tiempo;
    List* inventario; 
} Jugador;

void leer_escenarios(Map * esce_ID, List* lista_items) {
  // Intenta abrir el archivo CSV que contiene datos de el grafo
    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo"); // Informa si el archivo no puede abrirse
        return;
    }

    
    char **campos;
  // Lee cada línea del archivo CSV hasta el final
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {

    //printf("Items: \n");
    //crear la lista de items con peso y valor para mostrar en el main
        Juego* escenarios = (Juego *)malloc(sizeof(Juego));
        if (escenarios== NULL) EXIT_FAILURE;

        strncpy(escenarios->id, campos[0], sizeof(escenarios->id) - 1);
        strncpy(escenarios->nombre, campos[1], sizeof(escenarios->nombre) - 1);
        strncpy(escenarios->descripcion, campos[2], sizeof(escenarios->descripcion) - 1);
        //crear la lista de los items, por peso valor y asi
        escenarios->items = list_create();

        List* items_string = split_string(campos[3], ";");

        for(char *item = list_first(items_string); item != NULL; 
            item = list_next(items_string)){

            List* values = split_string(item, ",");
            char* item_name = list_first(values);
            int item_value = atoi(list_next(values));
            int item_weight = atoi(list_next(values));
            
            listaItems* listItems_struct =  malloc(sizeof(listaItems));
            strcpy(listItems_struct->nombre, item_name);
            listItems_struct->valor = item_value;
            listItems_struct->peso = item_weight;

            //agregar los items separados a la lista de items de escenarios del laberinto
            list_pushBack(lista_items, listItems_struct);
            list_clean(values);
            free(values);
        }



        escenarios->arriba = atoi(campos[4]);
        escenarios->abajo = atoi(campos[4]);
        escenarios->izquierda = atoi(campos[4]);
        escenarios->derecha = atoi(campos[4]);
        strncpy(escenarios->esFinal, campos[8], sizeof(escenarios->esFinal) - 1);

        map_insert(esce_ID, escenarios->id, escenarios);

    }
        
    printf("El archivo ha sido cargado correctamente\n");
    fclose(archivo); // Cierra el archivo después de leer todas las líneas
    presioneTeclaParaContinuar();
}



int distancia_L1(State* state) {
    int ev=0;
    int k=1;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++){
            int val=state->square[i][j];
            if (val==0) continue;
            int ii=(val)/3;
            int jj=(val)%3;
            ev+= abs(ii-i) + abs(jj-j);
        }
    return ev;
}


// Función para imprimir el estado del puzzle
void imprimirEstado(const State *estado) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (estado->square[i][j] == 0)
                printf("x "); // Imprime un espacio en blanco para el espacio vacío
            else
                printf("%d ", estado->square[i][j]);
        }
        printf("\n");
    }
}

void mostrar_escenario_actual();

int main() {
    int opcion;
    int archivo_cargado = 0;
    Map* esce_ID = map_create(comparar_claves);
    List* lista_items = list_create();
    Map* inventarioJugador = map_create(comparar_claves);
    do {
        limpiarPantalla();
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1. Cargar Laberinto desde Archivo CSV\n");
        printf("2. Iniciar Partida\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar(); // Limpia buffer

        switch (opcion) {
            case 1:
                limpiarPantalla();
                printf("\n--- Cargando laberinto desde archivo CSV ---\n");
                leer_escenarios(esce_ID, lista_items); // Solo imprime por ahora
                archivo_cargado = 1;
                break;

            case 2:
                limpiarPantalla();
                if (!archivo_cargado) {
                    printf("¡Debes cargar el archivo CSV antes de comenzar la partida!\n");
                } else {
                    printf("Iniciando partida...\n");
                
                    printf("No hay más elementos en el Heap\n");

                    char opcion2;
                    do {
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
                            //recogerItems(esce_ID, lista_items);
                            break;
                        case '2':
                            //bfs(estado_inicial);
                            break;
                        case '3':
                            //best_first(estado_inicial);
                            break;
                        case '4':
                            //best_first(estado_inicial);
                            break;
                        }
                        presioneTeclaParaContinuar();
                        limpiarPantalla();

                        } while (opcion2 != '5'); 
                        break;
                        
                        
                }
            break;
            case 0:
                printf("Saliendo del juego. ¡Hasta luego!\n");
                break;

            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }

    } while (opcion != 0);

    return 0;
}

//gcc tdas/*.c tarea3_princi.c -Wno-unused-result -o tarea3_princi
//./tarea3_princi

//graphquest.csv