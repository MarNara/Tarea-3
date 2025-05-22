#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include <string.h>

// Definición de la estructura para el estado del puzzle
typedef struct {
    int square[3][3]; // Matriz 3x3 que representa el tablero
    int x;    // Posición x del espacio vacío
    int y;    // Posición y del espacio vacío
    List* actions; //Secuencia de movimientos para llegar al estado
} State;

typedef struct{
    int id;
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
    int tiempo;
    List* inventario; 

} Jugador;

void leer_escenarios() {
  // Intenta abrir el archivo CSV que contiene datos de el grafo
  FILE *archivo = fopen("graphquest.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV


  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    /*printf("ID: %d\n", atoi(campos[0]));
    printf("Nombre: %s\n", campos[1]);
    printf("Descripción: %s\n", campos[2]);
    */
    List* items = split_string(campos[3], ";");

    //printf("Items: \n");
    //crear la lista de items con peso y valor para mostrar en el main
    for(char *item = list_first(items); item != NULL; 
          item = list_next(items)){

        List* values = split_string(item, ",");
        char* item_name = list_first(values);
        int item_value = atoi(list_next(values));
        int item_weight = atoi(list_next(values));
        //printf("  - %s (%d pts, %d kg)\n", item_name, item_value, item_weight);
        list_clean(values);
        free(values);
    }

    int arriba = atoi(campos[4]);
    int abajo = atoi(campos[5]);
    int izquierda = atoi(campos[6]);
    int derecha = atoi(campos[7]);

    if (arriba != -1) printf("Arriba: %d\n", arriba);
    if (abajo != -1) printf("Abajo: %d\n", abajo);
    if (izquierda != -1) printf("Izquierda: %d\n", izquierda);
    if (derecha != -1) printf("Derecha: %d\n", derecha);

    
    int is_final = atoi(campos[8]);
    if (is_final) printf("Es final\n");

    list_clean(items);
    free(items);
    
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
                leer_escenarios(); // Solo imprime por ahora
                archivo_cargado = 1;
                break;

            case 2:
                limpiarPantalla();
                if (!archivo_cargado) {
                    printf("¡Debes cargar el archivo CSV antes de comenzar la partida!\n");
                } else {
                    printf("Iniciando partida...\n");
                    // Estado inicial del puzzle
                    State estado_inicial = {
                    {{0, 2, 8}, // Primera fila (0 representa espacio vacío)
                    {1, 3, 4}, // Segunda fila
                    {6, 5, 7}, // Tercera fila
                    },  
                    0, 1   // Posición del espacio vacío (fila 0, columna 1)
                    };
                    estado_inicial.actions = list_create();

                    // Imprime el estado inicial
                    printf("Estado inicial del puzzle:\n");
                    imprimirEstado(&estado_inicial);

                    printf("Distancia L1:%d\n", distancia_L1(&estado_inicial));

                    //Ejemplo de heap (cola con prioridad)
                    printf("\n***** EJEMPLO USO DE HEAP ******\nCreamos un Heap e insertamos 3 elementos con distinta prioridad\n");
                    Heap* heap = heap_create();
                    char* data = strdup("Cinco");
                    printf("Insertamos el elemento %s con prioridad -5\n", data);
                    heap_push(heap, data, -5 /*prioridad*/);
                    data = strdup("Seis");
                    printf("Insertamos el elemento %s con prioridad -6\n", data);
                    heap_push(heap, data, -6 /*prioridad*/);
                    data = strdup("Siete");
                    printf("Insertamos el elemento %s con prioridad -7\n", data);
                    heap_push(heap, data, -7 /*prioridad*/);

                    printf("\nLos elementos salen del Heap ordenados de mayor a menor prioridad\n");
                    while (heap_top(heap) != NULL){
                        printf("Top: %s\n", (char*) heap_top(heap));      
                        heap_pop(heap);
                    }
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
                            //dfs(estado_inicial);
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

//gcc tdas/*.c tarea3.c -Wno-unused-result -o tarea3
//./tarea3

//graphquest.csv