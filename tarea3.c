#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
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

typedef struct Juego Juego;
// estructura del juego, los nodo/escenario
struct Juego {
    int id;
    char* nombre;
    char* descripcion;
    Item** items;
    int num_items;
    Juego* arriba;
    Juego* abajo;
    Juego* izquierda;
    Juego* derecha;
    char* es_final;

    //temporal, para leer
    int temp_arriba;
    int temp_abajo;
    int temp_izquierda;
    int temp_derecha;
};

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
Juego* inicializar_escenario(int id, char* nombre, char* descripcion, char* es_fin) {
    Juego* esc = (Juego*)malloc(sizeof(Juego));
    esc->id = id;
    esc->nombre = strdup(nombre);
    esc->descripcion = strdup(descripcion);
    esc->items = NULL;
    esc->num_items = 0;
    esc->arriba = NULL;
    esc->abajo = NULL;
    esc->izquierda = NULL;
    esc->derecha = NULL;
    esc->es_final = strdup(es_fin);
    esc->temp_arriba = 0;
    esc->temp_abajo = 0;
    esc->temp_izquierda = 0;
    esc->temp_derecha = 0;
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
 
// función para agregar un escenario al mapa
void agregar_escenario(MapaDelEsc* mapa, Juego* esc) {
    mapa->num_escena++;
    mapa->escenarios = realloc(mapa->escenarios, mapa->num_escena * sizeof(Juego*));
    mapa->escenarios[mapa->num_escena - 1] = esc;
    
    // si es el primer escenario lo colocamos como el inicio de la partida
    if (mapa->num_escena == 1) {
        mapa->inicio = esc;
    }
}

//-----------------------------------funcion para cargar el archivo-----------------------------------------------------------------

//cargar los archivos y convertir el grafo en un mapa
MapaDelEsc* cargar_archivo() {
    MapaDelEsc* mapa = inicializar_mapa();
    char nombre_archivo[100];
    printf("Ingrese el nombre del archivo CSV: \n");
    scanf("%99s", nombre_archivo);
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return mapa;
    }

    char **campos;
    campos = leer_linea_csv(archivo, ','); // Lee los encabezados

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        // aqui creo un nuevo escenario
        Juego* esc = inicializar_escenario(atoi(campos[0]), campos[1], campos[2], campos[8]);

        // separar los items
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
        
        // conectar vecinos temporales
        esc->temp_arriba = atoi(campos[4]);
        esc->temp_abajo = atoi(campos[5]);
        esc->temp_izquierda = atoi(campos[6]);
        esc->temp_derecha = atoi(campos[7]);

        // agregar al mapa (hacer una función para que sea menos ruido visual y me confuna menos)
        agregar_escenario(mapa, esc);
    }
    
    fclose(archivo);
    
    // falta establecer vecinos entre escenarios

    for (int i = 0; i < mapa->num_escena; i++) {
        Juego* actual = mapa->escenarios[i];
        for (int j = 0; j < mapa->num_escena; j++) {
            Juego* vecino_direccion = mapa->escenarios[j];

            if (vecino_direccion->id == actual->temp_arriba)
                actual->arriba = vecino_direccion;
            if (vecino_direccion->id == actual->temp_abajo)
                actual->abajo = vecino_direccion;
            if (vecino_direccion->id == actual->temp_izquierda)
                actual->izquierda = vecino_direccion;
            if (vecino_direccion->id == actual->temp_derecha)
                actual->derecha = vecino_direccion;
        }
    }
    
    printf("El archivo se a cargado correctamente\n");
    presioneTeclaParaContinuar();
    return mapa;
    
}

//-----------------------------------funciones de las opciones--------------------------------------------------------------------------

//mostrar
/*
    Organización: 1)imprimir la descripcion del escenario, para eso necesito el nombre.

    2)luego debo mostrar la lista de items disponibles que tiene el escenario.

    3)despues debo mostrar el tiempo que le queda al jugador para hacer su proximo movimiento
    (Esto se va actualizando solo??).
     
    4)Luego debo mostrar el inventario del jugador (ítems recogidos,
     peso total(es tambien una suma??) y puntaje acumulado(el puntaje es la suma de los valores de los items??))
     
    5)Por ultimo mostrar las Acciones posibles desde este escenario: direcciones disponibles (arriba, abajo, izquierda, 
     derecha).
}*/
//esta función muestra el escenario actual en el que se encuentra el jugador........................................................
void mostrar_escenario_actual(Jugador* datos_jugador) {

    if (datos_jugador->actual == NULL) {
        printf("No hay escenario actual asignado.\n");
        return;
    }

    Juego* actual = datos_jugador->actual;
    
    puts("\n========================================");
    puts("     Estado Actual");
    puts("========================================");
    
    // 1)
    printf("NOMBRE DEL ESCENARIO: %s\n", actual->nombre);
    printf("DESCRIPCION: %s\n", actual->descripcion);
    
    // 2) 
    printf("\n==== ITEMS DISPONIBLES ====\n");
    if (actual->num_items == 0) {
        printf("No hay items en este escenario.\n");
    } else {
        for (int i = 0; i < actual->num_items; i++) {
            Item* item = actual->items[i];
            printf("-Item: %s, Valor: %d, Peso: %d\n", item->nombre, item->valor, item->peso);
        }
    }
    
    // 3) 
    printf("\nTiempo restante: %d\n", datos_jugador->tiempo);
    
    // 4)
    printf("\n==== TU INVENTARIO ====\n");
    if (list_first(datos_jugador->inventario) == NULL) {
        printf("Inventario vacio.\n");
    } else {
        Item* item_inventaro = list_first(datos_jugador->inventario);
        while (item_inventaro != NULL){
            printf("-Item: %s\n", item_inventaro->nombre);
            item_inventaro = list_next(datos_jugador->inventario);
        }
        printf("Puntaje Total: %d, Peso Total: %d\n", datos_jugador->totalPuntaje, datos_jugador->totalPeso);
    }
    
    // 5) 
    printf("\n==== DIRECCION(ES) DISPONIBLE(S) ====\n");
    if (actual->arriba != NULL) printf("1) Arriba\n");
    if (actual->abajo != NULL) printf("2) Abajo\n");
    if (actual->izquierda != NULL) printf("3) Izquierda\n");
    if (actual->derecha != NULL) printf("4) Derecha\n");
    if(datos_jugador->actual->arriba == NULL && datos_jugador->actual->abajo == NULL && datos_jugador->actual->izquierda == NULL && datos_jugador->actual->derecha == NULL){
        printf("No hay direcciones disponibles para este escenario\n");
    }
    
}


/*analisis:
    para recoger un item primero debo mostrarle al usuario el disponible en el escenario junto a su id
    y luego el usuario si lo desea debe colocar el numero del id para tomarlo.
    para realizar lo siguiente debo ingresar los items al inventario con list_pushback y si hay más de un item en el
    escenario debo pedirle al ususario que separe los id por un espacio para agregarlos como en el leer el archivo con
    List* items_string = split_string(campos[3], ";");, pero con especificaciones diferentes, ya que yo quiero separar el espacio:  
    split_string(id_ingresadoPorJugador, " ");*/
    
//esta función recoje los items del escenario en el que se encuentra el jugador..............................................................................................................
void recoger_items(Jugador* datos_jugador) {
    Juego* actual = datos_jugador->actual;
    //recordar que si le coloco else puede no termin ar su ejecucion y darme error
    if(actual->num_items == 0){
        printf("No hay items disponibles en este escenario\n");
        presioneTeclaParaContinuar();
        return;
    }
    
    //mostrar los items disponibles
    puts("========================================");
    puts("     Item(s) disponible(s)");
    puts("========================================");
    //Item* contenido_item = actual->items[0];//el primer elemento del arreglo
    for (int i = 0; i < actual->num_items; i++) {
        Item* item = actual->items[i];
        printf("-Item: %s, Valor: %d, Peso: %d\n", item->nombre, item->valor, item->peso);
    }

    //falta agregar items y ver si lo agrego a mi inventario debo eliminarlo del "juego"
    //preguntar al jugador
    printf("\nIngrese el nombre del Item que desea llevar, si es mas de uno separar con ';': \n");
    char id_ingresadoPorJugador[100];
    getchar();
    fgets(id_ingresadoPorJugador, sizeof(id_ingresadoPorJugador), stdin);
    id_ingresadoPorJugador[strcspn(id_ingresadoPorJugador, "\n")] = '\0';
    
    List* nombres_items = split_string(id_ingresadoPorJugador, ";");
    char* nombre = list_first(nombres_items);
    int recogidos;
    while(nombre != NULL) {
        Item* item_encontrado = NULL;
        int indice_item = -1;

        for (int i = 0; i < actual->num_items; i++) {
            if (strcmp(actual->items[i]->nombre, nombre) == 0) {
                item_encontrado = actual->items[i];
                indice_item = i;
                break;
            }
        }

        if (item_encontrado == NULL) {
            printf("No se encontro el item: '%s'\n", nombre);
            return;
        }
        else{
            recogidos++;
            
        }

        // agregar al inventario y actualizar el peso y valor total
        list_pushBack(datos_jugador->inventario, item_encontrado);
        datos_jugador->totalPeso += item_encontrado->peso;
        datos_jugador->totalPuntaje += item_encontrado->valor;

        // eliminar del escenario para que se entienda que ya me lo lleve (en ningun juego hay un item infinito en el lugar)
        for (int j = indice_item; j < actual->num_items - 1; j++) {
            actual->items[j] = actual->items[j + 1];
        }
        actual->num_items--;

        printf("> Recogiste: %s, Valor: %d ,Peso: %d\n", item_encontrado->nombre, item_encontrado->valor, item_encontrado->peso);
        nombre = list_next(nombres_items);
    }
    //restar una vez
    if(recogidos > 0){
        datos_jugador->tiempo--;
    }


    list_clean(nombres_items);
    presioneTeclaParaContinuar();
}


//esta función descarta uno o más items ................................................................................................................................
void descartar_items(Jugador* datos_jugador, MapaDelEsc* mapa_juego) {
    //mostrar inventario
    printf("\n=== TU INVENTARIO ===\n");
    if (list_first(datos_jugador->inventario) == NULL) {
        printf("Inventario vacio.\n");
        presioneTeclaParaContinuar();
        return;
    } else {
        Item* item_inventario = list_first(datos_jugador->inventario);
        while (item_inventario != NULL) {
            printf("-Item: %s\n", item_inventario->nombre);
            item_inventario = list_next(datos_jugador->inventario);
        }
        printf("Puntaje Obtenido: %d, Peso Total: %d\n", datos_jugador->totalPuntaje, datos_jugador->totalPeso);
    }

    //pedir nombres de items a descartar
    printf("\nIngrese el nombre del Item que desea descartar, si es mas de uno separar con ';': \n");
    char id_ingresadoPorJugador[100];
    getchar();
    fgets(id_ingresadoPorJugador, sizeof(id_ingresadoPorJugador), stdin);
    id_ingresadoPorJugador[strcspn(id_ingresadoPorJugador, "\n")] = '\0';

    List* nombres_descartar = split_string(id_ingresadoPorJugador, ";");
    List* nueva_lista_inventario = list_create();  // Nueva lista para los items que se conservan
    int items_eliminados = 0;

    //recorrer inventario actual
    Item* item_actual = list_first(datos_jugador->inventario);
    while (item_actual != NULL) {
        int descartar = 0;
        char* nombre_descartar = list_first(nombres_descartar);
        
        //ver si este item debe ser descartado
        while (nombre_descartar != NULL) {
            if (strcmp(item_actual->nombre, nombre_descartar) == 0) {
                descartar = 1;
                break;
            }
            nombre_descartar = list_next(nombres_descartar);
        }

        if (descartar) {
            printf("> Descartaste: %s, Valor: %d, Peso: %d\n", item_actual->nombre, item_actual->valor, item_actual->peso);
            datos_jugador->totalPeso -= item_actual->peso;
            datos_jugador->totalPuntaje -= item_actual->valor;
            items_eliminados++;
            
            //buscar el escenario original para devolver el item
            Juego* escenario_original = NULL;
            for (int i = 0; i < mapa_juego->num_escena; i++) {
                if (mapa_juego->escenarios[i]->id == item_actual->id_item) {
                    escenario_original = mapa_juego->escenarios[i];
                    break;
                }
            }

            if (escenario_original != NULL) {
                escenario_original->num_items++;
                escenario_original->items = realloc(escenario_original->items, escenario_original->num_items * sizeof(Item*));
                escenario_original->items[escenario_original->num_items - 1] = item_actual;
            }
        } else {
            //si no se descarta, agregar a la nueva lista
            list_pushBack(nueva_lista_inventario, item_actual);
        }
        
        item_actual = list_next(datos_jugador->inventario);
    }

    //reemplazar el inventario con la nueva lista
    list_clean(datos_jugador->inventario);
    free(datos_jugador->inventario);
    datos_jugador->inventario = nueva_lista_inventario;

    //actualizar tiempo si se descartaron items
    if (items_eliminados > 0) {
        datos_jugador->tiempo--;
    } else {
        printf("\nNo se descarto ningun item.\n");
    }

    // Liberar memoria
    list_clean(nombres_descartar);
    free(nombres_descartar);
    
    presioneTeclaParaContinuar();
}



//esta función avanza en una dirccion a la otra direccion (aqui necesito los vecinos de cada dirección para moverse)......................................
void avanzar_una_direccion(Jugador* datos_jugador) {
    //mostrar las direcciones disponibles, preguntar al usuario si desea
    printf("Direcciones posibles desde este escenario:\n");
    if (datos_jugador->actual->arriba != NULL) printf("Arriba es: (1)\n");
    if (datos_jugador->actual->abajo != NULL) printf("Abajo es: (2)\n");
    if (datos_jugador->actual->izquierda != NULL) printf("Izquierda es: (3)\n");
    if (datos_jugador->actual->derecha != NULL) printf("Derecha es: (4)\n\n");
    printf("Seleccione el numero de la direccion a la que desea avanzar:\n");

    char direccion;
    scanf(" %c", &direccion);
    Juego* escenario_actual = datos_jugador->actual;
    Juego* siguiente = NULL;//si elige una direccion y no es null, la muestra, si no, no.
    switch(direccion) {
        case '1':
            siguiente = escenario_actual->arriba;
            break;
        case '2': 
            siguiente = escenario_actual->abajo;
            break;
        case '3': 
            siguiente = escenario_actual->izquierda;
            break;
        case '4': 
            siguiente = escenario_actual->derecha;
            break;
        default:
            printf("Direccion no valida\n");
            return;
    }

    if(siguiente == NULL) {
        printf("\n¡No puedes avanzar en esa direccion!\n");
        presioneTeclaParaContinuar();
        return;
    }
    
    //falta lo del tiempo
    /*- Se actualiza el escenario actual, el inventario se conserva, y se descuenta el tiempo usado según el peso total transportado:
- Si se alcanza el escenario final, se muestran los elementos del inventario y el **puntaje final.**
- Si el **tiempo se agota**, se muestra un mensaje de derrota.*/
    // calcular el tiempo
    int total_Peso = datos_jugador->totalPeso;
    int tiempo_Arestar = (int)ceil((total_Peso + 1) / 10.0);  // recordar que es divison decimal
    datos_jugador->tiempo -= tiempo_Arestar;
    
    if(datos_jugador->tiempo <= 0){
        printf("Tu tiempo se acabo, NO puedes seguir avanzando, Has perdido\n");
        return;
    }
    
    datos_jugador->actual = siguiente;//actualizar posicion
    printf("\nHas llegado a: %s\n", siguiente->nombre);

    // revisar si llegó al escenario final
    if (strcmp(datos_jugador->actual->es_final, "Si") == 0) {
        printf("\n================= ¡Has llegado al escenario final! =================\n");
        printf("\n=== TU INVENTARIO ===\n\n");
        if (list_first(datos_jugador->inventario) == NULL) {
            printf("Inventario vacío.\n");
        } else {
            Item* item_inventaro = list_first(datos_jugador->inventario);
            while (item_inventaro != NULL) {
                printf("-Item: %s\n", item_inventaro->nombre);
                item_inventaro = list_next(datos_jugador->inventario);
            }
            printf("Puntaje Obtenido: %d, Peso Total: %d\n", datos_jugador->totalPuntaje, datos_jugador->totalPeso);
        }
        return;
    }
    presioneTeclaParaContinuar();
}


//Esta funcion se encarga de reiniciar la partida.....................................................................................
void reiniciar_partida(Jugador* datos_jugador,  MapaDelEsc* mapa_juego){
    /*Analisis:
    Se reinicia el juego desde el escenario inicial, con inventario vacío y tiempo completo.
    si se reinicia la partida solo seria para el jugador, no el juego entero:(
    inicializar los datos de la estructura del jugador
    */
    //como no me dejaba regresar los items, debo buscar los id de los escenarios y de los items para regresar cada uno a su escenario original
    Item* item_actual = list_first(datos_jugador->inventario);
    while (item_actual != NULL) {
        for (int i = 0; i < mapa_juego->num_escena; i++) {
            Juego* escenario = mapa_juego->escenarios[i];
            if (escenario->id == item_actual->id_item) {
                escenario->num_items++;
                escenario->items = realloc(escenario->items, escenario->num_items * sizeof(Item*));
                escenario->items[escenario->num_items - 1] = item_actual;
                break;
            }
        }
        item_actual = list_next(datos_jugador->inventario);
    }

    //inicializar los datos de la estructura del jugador
    datos_jugador->tiempo = 10;
    list_clean(datos_jugador->inventario);
    datos_jugador->inventario = list_create();
    datos_jugador->actual = mapa_juego->inicio;
    datos_jugador->totalPeso = 0;
    datos_jugador->totalPuntaje = 0;
    printf("Has reiniciado la partida con exito\n");
    presioneTeclaParaContinuar();
}

//-----------------------------------función del main---------------------------------------------------------------------------------------

//aqui se muestra el menú principal y el menú del juego, permite al jugador elegir opciones
int main() {
    int opcion;
    int archivo_cargado = 0;
    MapaDelEsc* mapa_del_juego = NULL;  //inicializar como NULL
    
    // inicializar jugador 
    Jugador* datos_jugador = (Jugador*)malloc(sizeof(Jugador));
    if(datos_jugador == NULL) return EXIT_FAILURE;
    
    datos_jugador->tiempo = 10;
    datos_jugador->inventario = list_create();
    datos_jugador->actual = NULL;  //no hay escenario
    datos_jugador->totalPeso = 0;
    datos_jugador->totalPuntaje = 0; 
    
    do {
        limpiarPantalla();
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1. Cargar Laberinto desde Archivo CSV\n");
        printf("2. Iniciar Partida\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {
            case 1: {
                limpiarPantalla();
                printf("\n--- Cargando laberinto desde archivo CSV ---\n");
                
                // liberar mapa anterior si existe
                if(mapa_del_juego != NULL) {
                    free(mapa_del_juego);
                }
                
                mapa_del_juego = cargar_archivo();
                
                if(mapa_del_juego != NULL && mapa_del_juego->inicio != NULL) {
                    archivo_cargado = 1;
                    datos_jugador->actual = mapa_del_juego->inicio;  
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
                        limpiarPantalla();
                        mostrar_escenario_actual(datos_jugador);
                        
                        printf("\n***** MENU DEL JUEGO ******\n");
                        puts("========================================");
                        puts("     Opciones del Jugador");
                        puts("========================================");
                        puts("1) Recoger Item(s)");
                        puts("2) Descartar Item(s)");
                        puts("3) Avanzar en una Direccion");
                        puts("4) Reiniciar Partida");
                        puts("5) Salir");
                        printf("Ingrese su opcion: ");
                        scanf(" %c", &opcion2);
                        

                        switch (opcion2) {
                            case '1':
                                limpiarPantalla();
                                recoger_items(datos_jugador);
                                break;
                                
                            case '2':
                                limpiarPantalla();
                                descartar_items(datos_jugador, mapa_del_juego);
                                break;
                                
                            case '3':
                                limpiarPantalla();
                                avanzar_una_direccion(datos_jugador);
                                break;
                                
                            case '4':
                                limpiarPantalla();
                                reiniciar_partida(datos_jugador, mapa_del_juego);
                                break;

                            case '5':
                                printf("Saliendo del juego. ¡Hasta luego!\n");
                                //liberar memoria
                                if(mapa_del_juego != NULL) {
                                    free(mapa_del_juego);
                                }
                                return 0;
                            default:
                                printf("Opcion no valida. Intente nuevamente.\n");
                            presioneTeclaParaContinuar();
                        }


                    } while (opcion2 != '5');  
                    break;
                }
               
            }
        }
    }while(1);  
}
