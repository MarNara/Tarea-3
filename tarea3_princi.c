#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/map.h"
#include <string.h>

int compararClaves_strcmp(void* a, void* b) {
    return strcmp((char*)a, (char*)b);
}


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
    Juego* actual;// en el que esta el jugador 
} Jugador;

void leer_escenarios(Map * esce_ID, List* lista_items) {
  // Intenta abrir el archivo CSV que contiene datos de el grafo
    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo"); // Informa si el archivo no puede abrirse
        return;
    }

    leer_linea_csv(archivo, ',');//saltarme la primera linea, eran los encabezados
    
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

/*
    if (datos_jugador->actual->abajo != -1) printf("Abajo: %d\n", datos_jugador->actual->abajo);
    if (datos_jugador->actual->izquierda != -1) printf("Izquierda: %d\n", datos_jugador->actual->izquierda);
    if (datos_jugador->actual->derecha != -1) printf("Derecha: %d\n", datos_jugador->actual->derecha);*/

        escenarios->arriba = atoi(campos[4]); 
        escenarios->abajo = atoi(campos[5]);
        escenarios->izquierda = atoi(campos[6]);
        escenarios->derecha = atoi(campos[7]);
        strncpy(escenarios->esFinal, campos[8], sizeof(escenarios->esFinal) - 1);

        map_insert(esce_ID, escenarios->id, escenarios);
        if (escenarios->arriba != -1) printf("Arriba: %d\n", escenarios->arriba);
        if (escenarios->abajo != -1) printf("Abajo: %d\n", escenarios->abajo);
        if (escenarios->izquierda != -1) printf("Izquierda: %d\n", escenarios->izquierda);
        if (escenarios->derecha != -1) printf("Derecha: %d\n", escenarios->derecha);

    }
    
        
    printf("El archivo ha sido cargado correctamente\n");
    fclose(archivo); // Cierra el archivo después de leer todas las líneas
    presioneTeclaParaContinuar();
}


void mostrar_escenario_actual(Jugador* datos_jugador){
    puts("========================================");
    puts("     Estado Actual");
    puts("========================================");
    

    /*Organización: 1)imprimir la descripcion del escenario, para eso necesito el nombre.

    2)luego debo mostrar la lista de items disponibles que tiene el escenario.

    3)despues debo mostrar el tiempo que le queda al jugador para hacer su proximo movimiento
    (Esto se va actualizando solo??).
     
    4)Luego debo mostrar el inventario del jugador (ítems recogidos,
     peso total(es tambien una suma??) y puntaje acumulado(el puntaje es la suma de los valores de los items??))
     
    5)Por ultimo mostrar las Acciones posibles desde este escenario: direcciones disponibles (arriba, abajo, izquierda, 
     derecha).*/

     //1)
    printf("Nombre Escsenario: %s\n", datos_jugador->actual->nombre);
    printf("Descripción Escsenario: %s\n", datos_jugador->actual->descripcion);

    //2)
    if(list_first(datos_jugador->actual->items) == NULL){
        printf("No a seleccionado ningún item\n");
    }
    else{
        listaItems* contenido_item = list_first(datos_jugador->actual->items);
        while(contenido_item != NULL){
            printf("Item: %s, Valor: %s, Peso: %s\n", contenido_item->nombre, contenido_item->valor, contenido_item->peso);
            contenido_item = list_next(datos_jugador->actual->items);
        }
    }

    //3)
    printf("Tiempo restantes: %d\n", datos_jugador->tiempo);

    //4)
    listaItems* item_del_inventario = list_first(datos_jugador->inventario);
    if(item_del_inventario == NULL){
        printf("No has ingresado ningun item al inventario\n");
    }
    else{
        while(item_del_inventario != NULL){
            printf("Item: %s, Valor: %s, Peso: %s\n", item_del_inventario->nombre, item_del_inventario->valor, item_del_inventario->peso);
            item_del_inventario = list_next(datos_jugador->inventario);
        }
    }

    //5)
    printf("Acciones posibles desde este escenario:\n");
    if (datos_jugador->actual->arriba != -1) printf("Arriba: %d\n", datos_jugador->actual->arriba);
    if (datos_jugador->actual->abajo != -1) printf("Abajo: %d\n", datos_jugador->actual->abajo);
    if (datos_jugador->actual->izquierda != -1) printf("Izquierda: %d\n", datos_jugador->actual->izquierda);
    if (datos_jugador->actual->derecha != -1) printf("Derecha: %d\n", datos_jugador->actual->derecha);

}

int main() {
    int opcion;
    int archivo_cargado = 0;
    //parametros para las funciones
    Map* esce_ID = map_create(compararClaves_strcmp);
    List* lista_items = list_create();
    //datos del jugador
    Jugador* datos_jugador = (Jugador*)malloc(sizeof(Jugador));
    if(datos_jugador == NULL) EXIT_FAILURE;
    datos_jugador->tiempo = 10;
    datos_jugador->inventario = list_create();
    
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
                //aqui puedo saber en que escenario está el jugador
                MapPair* pair = map_search(esce_ID, " 1");//es el primer id, en caso de que este vacio, que es lo más probable porque esta empezando.
                Juego* inicio = NULL;
                if (pair != NULL){
                    inicio = (Juego*)pair->value;
                }
                else{
                    printf("No se encontro el escenario inicial\n");
                    exit(EXIT_FAILURE);
                }
                datos_jugador->actual = inicio;
                break;

            case 2:
                limpiarPantalla();
                if (!archivo_cargado) {
                    printf("¡Debes cargar el archivo CSV antes de comenzar la partida!\n");
                } else {
                    printf("Iniciando partida...\n");
                    mostrar_escenario_actual(datos_jugador);//solo necesito los datos del jugador actualmente, no todos los datos :(
                    printf("No hay más elementos en el estado actual\n");

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
                            limpiarPantalla();
                            //recoger_items(datos_jugador, esce_ID, lista_items);
                            break;
                        case '2':
                            limpiarPantalla();
                            //descartar_items(estado_inicial);
                            break;
                        case '3':
                            limpiarPantalla();
                            //avanzar_una_direccion(estado_inicial);
                            break;
                        case '4':
                            limpiarPantalla();
                            //reiniciar_partida(estado_inicial);
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