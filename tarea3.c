#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
    char id[100];
    char nombre[100];
    int valor;
    int peso;
    
}listaItems;

typedef struct{
    int tiempo;
    List* inventario;
    Juego* actual;// en el que esta el jugador
    int totalPeso;
    int totalPuntaje; 
} Jugador;

void leer_escenarios(Map *esce_ID, List *lista_items) {
    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    // Saltar encabezados
    leer_linea_csv(archivo, ',');

    char **campos;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        // Validar que haya suficientes campos
        if (campos[0] == NULL || campos[1] == NULL) {
            printf("Error: línea con formato incorrecto\n");
            continue;
        }

        Juego *escenario = (Juego *)malloc(sizeof(Juego));
        if (escenario == NULL) {
            printf("Error de memoria\n");
            fclose(archivo);
            return;
        }

        // Limpiar espacios y copiar campos
        
        // Por:
        trim(campos[0]);
        // Elimina todos los espacios no numéricos
        char id_limpio[20];
        int num_id = atoi(campos[0]);  // Convierte a número primero
        snprintf(id_limpio, sizeof(id_limpio), "%d", num_id);  // Vuelve a string sin espacios
        strncpy(escenario->id, id_limpio, sizeof(escenario->id) - 1);


        escenario->id[sizeof(escenario->id) - 1] = '\0';

        

        trim(campos[1]);
        strncpy(escenario->nombre, campos[1], sizeof(escenario->nombre) - 1);

        trim(campos[2]);
        strncpy(escenario->descripcion, campos[2], sizeof(escenario->descripcion) - 1);

        // Procesar items
        escenario->items = list_create();
        if (campos[3] != NULL) {
            List *items_string = split_string(campos[3], ";");
            
            // Reemplazo del for por while equivalente
            char *item = list_first(items_string);
            while (item != NULL) {
                List *values = split_string(item, ",");
                if (list_size(values) >= 4) {
                    listaItems *nuevo_item = malloc(sizeof(listaItems));
                    if (nuevo_item != NULL) {
                        trim(list_first(values));
                        strncpy(nuevo_item->id, list_first(values), sizeof(nuevo_item->id) - 1);
                        
                        trim(list_next(values));
                        strncpy(nuevo_item->nombre, list_next(values), sizeof(nuevo_item->nombre) - 1);
                        
                        nuevo_item->valor = atoi(list_next(values));
                        nuevo_item->peso = atoi(list_next(values));
                        
                        list_pushBack(lista_items, nuevo_item);
                        list_pushBack(escenario->items, nuevo_item);
                    }
                }
                list_clean(values);
                free(values);
                
                item = list_next(items_string);  // Avanzar al siguiente item
            }
            list_clean(items_string);
            free(items_string);
        }

        // Procesar direcciones
        escenario->arriba = atoi(campos[4]);
        escenario->abajo = atoi(campos[5]);
        escenario->izquierda = atoi(campos[6]);
        escenario->derecha = atoi(campos[7]);
        
        trim(campos[8]);
        strncpy(escenario->esFinal, campos[8], sizeof(escenario->esFinal) - 1);

        map_insert(esce_ID, escenario->id, escenario);
    }

    printf("El archivo ha sido cargado correctamente\n");
    fclose(archivo);
    presioneTeclaParaContinuar();
}


//mostraaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaar
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
    printf("Nombre Del Escenario: %s\n", datos_jugador->actual->nombre);
    printf("Descripción Escenario: %s\n\n", datos_jugador->actual->descripcion);

    //2)
    printf("------ Items disponibles de este escenario ------\n");
    if(list_first(datos_jugador->actual->items) == NULL){
        printf("No hay items disponibles en este escenario\n");
        printf("\n");
    }
    else{
        listaItems* contenido_item = list_first(datos_jugador->actual->items);
        while(contenido_item != NULL){
            printf("Item: %s, Valor: %d, Peso: %d\n", contenido_item->nombre, contenido_item->valor, contenido_item->peso);
            contenido_item = list_next(datos_jugador->actual->items);
        }
        printf("\n");
    }

    //3)
    printf("Tiempo restantes: %d\n\n", datos_jugador->tiempo);

    //4)
    printf("------ Items ingresados al inventario ------\n");
    listaItems* item_del_inventario = list_first(datos_jugador->inventario);
    if(item_del_inventario == NULL){
        printf("No has ingresado ningun item al inventario\n");
        printf("\n");
    }
    else{
        datos_jugador->totalPuntaje = 0;
        datos_jugador->totalPeso = 0;
        while(item_del_inventario != NULL){
            printf("Item: %s, Valor: %s, Peso: %s\n", item_del_inventario->nombre, item_del_inventario->valor, item_del_inventario->peso);
            item_del_inventario = list_next(datos_jugador->inventario);
            datos_jugador->totalPuntaje += item_del_inventario->valor;
            datos_jugador->totalPeso += item_del_inventario->peso;
        }
        printf("Total peso: %d\n", datos_jugador->totalPeso);
        printf("Puntaje obtenido: %d\n", datos_jugador->totalPuntaje);

        printf("\n");
    }

    //5)
    printf("Direcciones posibles desde este escenario:\n");
    if (datos_jugador->actual->arriba != -1) printf("Arriba: %d\n", datos_jugador->actual->arriba);
    if (datos_jugador->actual->abajo != -1) printf("Abajo: %d\n", datos_jugador->actual->abajo);
    if (datos_jugador->actual->izquierda != -1) printf("Izquierda: %d\n", datos_jugador->actual->izquierda);
    if (datos_jugador->actual->derecha != -1) printf("Derecha: %d\n", datos_jugador->actual->derecha);
    if(datos_jugador->actual->arriba == -1 && datos_jugador->actual->abajo == -1 && datos_jugador->actual->izquierda == -1 && datos_jugador->actual->derecha == -1){
        printf("No hay direcciones disponibles para este escenario\n");
    }
}

void recoger_items(Jugador* datos_jugador, Map* esce_ID){
    /*analisis:
    para recoger un item primero debo mostrarle al usuario el disponible en el escenario junto a su id
    y luego el usuario si lo desea debe colocar el numero del id para tomarlo.
    
    para realizar lo siguiente debo ingresar los items al inventario con list_pushback y si hay más de un item en el
    escenario debo pedirle al ususario que separe los id por un espacio para agregarlos como en el leer el archivo con
    List* items_string = split_string(campos[3], ";");, pero con especificaciones diferentes, ya que yo quiero separar el espacio:  
    split_string(id_ingresadoPorJugador, " ");*/
    
    //recordar que si le coloco else puede no termin ar su ejecucion y darme error
    if(list_first(datos_jugador->actual->items) == NULL ){
        printf("No hay items disponibles en este escenario\n");
        return;
    }
    
    //mostrar los items disponibles
    puts("========================================");
    puts("     Item(s) disponibles");
    puts("========================================");
    listaItems* contenido_item = list_first(datos_jugador->actual->items);
    while(contenido_item != NULL){
        printf("ID: %s,  Item: %s,  Valor: %s,  Peso: %s\n",contenido_item->id, contenido_item->nombre, contenido_item->valor, contenido_item->peso);
        contenido_item = list_next(datos_jugador->actual->items);
    } 

    /*falta agregar items y ver si lo agrego a mi inventario debo eliminarlo del "juego"*/
    //preguntar al jugador
    printf("Ingrese el/los id(s) que desea ingresar \n");
    char id_ingresadoPorJugador[100];
    getchar();
    fgets(id_ingresadoPorJugador, sizeof(id_ingresadoPorJugador), stdin);

    //separar los ids con la función del profe
    List* lis_items_elegido = split_string(id_ingresadoPorJugador, " ");

    //guardar los items en el inventario
    List* new_list_items_escenario = list_create();
    List* items_actuales = datos_jugador->actual->items;
    listaItems* items = list_first(items_actuales);
    while(items != NULL){
        int item_tomado = 0;//actua como false
        char* id_ids = list_first(lis_items_elegido);//sacar id de a uno
        while(id_ids != NULL){
            if(strcmp(datos_jugador->actual->id, id_ids)){
                list_pushBack(datos_jugador->inventario, id_ids);
                item_tomado = 1;//actua como true
                break;
            }
            id_ids = list_next(lis_items_elegido);
        
        }
        items = list_next(items_actuales);
        if(item_tomado == 0){
            list_pushBack(new_list_items_escenario, items);
        }
    }
    //actualizar los items del juego, como el jugador toma uno, ese item desaparece de los items del juego
    datos_jugador->actual->items = new_list_items_escenario;
    datos_jugador->tiempo -= 1;

    printf("item agregado al inventario: %s\n", datos_jugador->inventario);
    printf("tiempo restante del jugador: %s\n", datos_jugador->tiempo);

    

    
}

void avanzar_una_direccion(Jugador* datos_jugador, Map* esce_ID) {
    // Mostrar direcciones disponibles
    printf("\nDirecciones posibles desde '%s':\n", datos_jugador->actual->nombre);
    if (datos_jugador->actual->arriba != -1) printf("(1) Arriba -> Escenario %d\n", datos_jugador->actual->arriba);
    if (datos_jugador->actual->abajo != -1) printf("(2) Abajo -> Escenario %d\n", datos_jugador->actual->abajo);
    if (datos_jugador->actual->izquierda != -1) printf("(3) Izquierda -> Escenario %d\n", datos_jugador->actual->izquierda);
    if (datos_jugador->actual->derecha != -1) printf("(4) Derecha -> Escenario %d\n", datos_jugador->actual->derecha);
    
    printf("\nSeleccione el número de la dirección: ");
    
    char direccion;
    scanf(" %c", &direccion);
    
    int nuevo_id = -1;
    const char* nombre_direccion = "";
    
    // Validar dirección seleccionada
    switch (direccion) {
        case '1':
            nuevo_id = datos_jugador->actual->arriba;
            nombre_direccion = "ARRIBA";
            break;
        case '2':
            nuevo_id = datos_jugador->actual->abajo;
            nombre_direccion = "ABAJO";
            break;
        case '3':
            nuevo_id = datos_jugador->actual->izquierda;
            nombre_direccion = "IZQUIERDA";
            break;
        case '4':
            nuevo_id = datos_jugador->actual->derecha;
            nombre_direccion = "DERECHA";
            break;
        default:
            printf("\n¡Opción no válida!\n");
            return;
    }
    
    // Validar si hay salida en esa dirección
    if (nuevo_id == -1) {
        printf("\n¡No hay salida en esa dirección!\n");
        return;
    }
    
    // Convertir el ID numérico a string (eliminando espacios potenciales)
    char id_str[20];
    snprintf(id_str, sizeof(id_str), "%d", nuevo_id);
    
    // Buscar el nuevo escenario en el mapa
    MapPair* pair = map_search(esce_ID, id_str);
    if (pair == NULL) {
        // Intentar buscar con espacios por si acaso
        snprintf(id_str, sizeof(id_str), " %d", nuevo_id);
        pair = map_search(esce_ID, id_str);
        
        if (pair == NULL) {
            printf("\nError: No se encontró el escenario %d\n", nuevo_id);
            return;
        }
    }
    
    // Actualizar la posición del jugador
    datos_jugador->actual = (Juego*)pair->value;
    datos_jugador->tiempo--;
    
    printf("\n¡Has avanzado %s hacia '%s'!\n", nombre_direccion, datos_jugador->actual->nombre);
}

void imprimir_ids_escenarios(Map *esce_ID) {
    MapPair *pair = map_first(esce_ID);
    while (pair != NULL) {
        printf("ID en mapa: '%s'\n", (char*)pair->key);
        pair = map_next(esce_ID);
    }
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
                            recoger_items(datos_jugador, esce_ID);
                            break;
                        case '2':
                            limpiarPantalla();
                            //descartar_items(estado_inicial);
                            break;
                        case '3':
                            limpiarPantalla();
                            avanzar_una_direccion(datos_jugador, esce_ID);
                            break;
                        case '4':
                            limpiarPantalla();
                            //reiniciar_partida(estado_inicial);
                            break;
                        }
                        mostrar_escenario_actual(datos_jugador);
                        imprimir_ids_escenarios(esce_ID);
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