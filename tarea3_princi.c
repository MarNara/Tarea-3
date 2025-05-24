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
    char es_final;

    //temporal, para leer
    int temp_arriba;
    int temp_abajo;
    int temp_izquierda;
    int temp_derecha;
    //struct Juego** vecinos; // Para el grafo
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
Juego* inicializar_escenario(int id, char* nombre, char* descripcion, char* final) {
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
    esc->es_final = strdup(final);
    //esc->vecinos = NULL;
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
        Juego* esc = inicializar_escenario(atoi(campos[0]), campos[1], campos[2], campos[8]);
        
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
        
        // Establecer vecinos temporales


        esc->temp_arriba = atoi(campos[4]);
        esc->temp_abajo = atoi(campos[5]);
        esc->temp_izquierda = atoi(campos[6]);
        esc->temp_derecha = atoi(campos[7]);
        
        esc->es_final = atoi(campos[8]);
        
        // Agregar al mapa (hacer una función para que sea menos ruido visual y me confuna menos)
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
    
    // 1)
    printf("Nombre Del Escenario: %s\n", actual->nombre);
    printf("Descripción: %s\n\n", actual->descripcion);
    
    // 2) 
    printf("\nItems disponibles:\n");
    if (actual->num_items == 0) {
        printf("No hay items en este escenario.\n");
    } else {
        for (int i = 0; i < actual->num_items; i++) {
            Item* item = actual->items[i];
            printf("-Item: %s Valor: %d, Peso: %d\n", item->nombre, item->valor, item->peso);
        }
    }
    
    // 3) 
    printf("\nTiempo restante: %d\n", datos_jugador->tiempo);
    
    // 4)
    printf("\n=== TU INVENTARIO ===\n");
    if (list_first(datos_jugador->inventario) == NULL) {
        printf("Inventario vacío.\n");
    } else {
        for(Item* item_inventaro = list_first(datos_jugador->inventario); 
            item_inventaro != NULL; 
            item_inventaro = list_next(datos_jugador->inventario)) {
            printf("-Item: %s\n", item_inventaro->nombre);
        }
        printf("Puntaje Total: %d, Peso Total: %d\n", datos_jugador->totalPuntaje, datos_jugador->totalPeso);
    }
    
    // 5) 
    printf("\n------ Direccion(es) disponibl(es) ------\n");
    if (actual->arriba != NULL) printf("1) Arriba\n");
    if (actual->abajo != NULL) printf("2) Abajo\n");
    if (actual->izquierda != NULL) printf("3) Izquierda\n");
    if (actual->derecha != NULL) printf("4) Derecha\n");
}


/*analisis:
    para recoger un item primero debo mostrarle al usuario el disponible en el escenario junto a su id
    y luego el usuario si lo desea debe colocar el numero del id para tomarlo.
    
    para realizar lo siguiente debo ingresar los items al inventario con list_pushback y si hay más de un item en el
    escenario debo pedirle al ususario que separe los id por un espacio para agregarlos como en el leer el archivo con
    List* items_string = split_string(campos[3], ";");, pero con especificaciones diferentes, ya que yo quiero separar el espacio:  
    split_string(id_ingresadoPorJugador, " ");*/
    
//
void recoger_items(Jugador* datos_jugador) {
    //Juego* actual = datos_jugador->actual;
    Juego* actual = datos_jugador->actual;
    //recordar que si le coloco else puede no termin ar su ejecucion y darme error
    if(actual->num_items == 0){
        printf("No hay items disponibles en este escenario\n");
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
    printf("\nIngrese el nombre del Item que desea llevar, si es más de uno separar con ' ; ': \n");
    char id_ingresadoPorJugador[100];
    getchar();
    fgets(id_ingresadoPorJugador, sizeof(id_ingresadoPorJugador), stdin);
    id_ingresadoPorJugador[strcspn(id_ingresadoPorJugador, "\n")] = '\0';
    //
    
    List* nombres_items = split_string(id_ingresadoPorJugador, ";");
    char* nombre = list_first(nombres_items);
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
            printf("No se encontró el ítem: '%s'\n", nombre);
            continue;
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

        printf(">Recogiste: %s ,Valor: %d ,Peso: %d\n", item_encontrado->nombre, item_encontrado->valor, item_encontrado->peso);
        nombre = list_next(nombres_items);
    }
    datos_jugador->tiempo --;
    list_clean(nombres_items);
}


//avanza en una dirccion a la otra direccion(aqui necesito los vecinos)
void avanzar_una_direccion(Jugador* datos_jugador, MapaDelEsc* mapa_juego) {
    //mostrar las direcciones disponibles, preguntar al usuario si desea
    printf("Direcciones posibles desde este escenario:\n");
    if (datos_jugador->actual->arriba != NULL) printf("Arriba es: (1)\n");
    if (datos_jugador->actual->abajo != NULL) printf("Abajo es: (2)\n");
    if (datos_jugador->actual->izquierda != NULL) printf("Izquierda es: (3)\n");
    if (datos_jugador->actual->derecha != NULL) printf("Derecha es: (4)\n\n");
    printf("Seleccione el numero de la dirección si desea avanzar en esa dirección\n");

    char direccion;
    scanf(" %c", &direccion);
    //int id_de_la_direccion = -1;//esto me daba error, ya no

    Juego* escenario_actual = datos_jugador->actual;
    Juego* siguiente = NULL;//si elige una direccion y no es null, la muestra, si no, no.
    switch(direccion) {
        case '1': //arriba
            siguiente = escenario_actual->arriba;
            break;
        case '2': //abajo
            siguiente = escenario_actual->abajo;
            break;
        case '3': //izquierda
            siguiente = escenario_actual->izquierda;
            break;
        case '4': //derecha
            siguiente = escenario_actual->derecha;
            break;
        default:
            printf("Dirección no válida\n");
            return;
    }
    
    //buscar en pinterest ideas de cque dice un juego, me estafaron :(
    if(siguiente != NULL) {
        datos_jugador->actual = siguiente;
        printf("\nHas llegado a: %s\n", siguiente->nombre);
    } else {
        printf("\n¡No puedes avanzar en esa dirección!\n");
    }

    //falta lo del tiempo
    /*- Se actualiza el escenario actual, el inventario se conserva, y se descuenta el tiempo usado según el peso total transportado:
    
- Si se alcanza el escenario final, se muestran los elementos del inventario y el **puntaje final.**
- Si el **tiempo se agota**, se muestra un mensaje de derrota.*/

    datos_jugador->tiempo = (datos_jugador->totalPeso + 1) / 10;

    if(escenario_actual->es_final == 'Si' && datos_jugador->tiempo > 0){
        printf("\nHas llegado al Escenario final\n");
        printf("----------- Tu Inventario -----------\n\n");
        
    }
}

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
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
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
                                recoger_items(datos_jugador);
                                break;
                                
                            case '2':
                                limpiarPantalla();
                                //descartar_items(datos_jugador);
                                break;
                                
                            case '3':
                                limpiarPantalla();
                                avanzar_una_direccion(datos_jugador, mapa_del_juego);
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

//gcc tdas/*.c tarea3_princi.c -Wno-unused-result -o tarea3_princi
//./tarea3_princi

//graphquest.csv