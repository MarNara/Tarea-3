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
    struct Juego** conexiones; // Para el grafo
} Juego;

// estructura del grafo con mapa 
typedef struct {
    Juego** escenarios;
    int num_escenarios;
    Juego* inicio;
} MapaDelEsc;


typedef struct{
    int tiempo;
    List* inventario;
    MapaDelEsc* actual;// en el que esta el jugador
    int totalPeso;
    int totalPuntaje; 
} Jugador;

//-----------------------------inicializar estructuras para evitar errores---------------------------------


// inicializar la estructura item aparte para desglosar el cargar archivos
Item* crear_item(char* nombre, int valor, int peso, int id_DelItem) {
    Item* item = (Item*)malloc(sizeof(Item));
    item->nombre = strdup(nombre);
    item->valor = valor;
    item->peso = peso;
    item->id_item = id_DelItem;
    return item;
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

// esta funcion inicializa la estructura item
MapaDelEsc* crear_mapa() {
    MapaDelEsc* mapa = (MapaDelEsc*)malloc(sizeof(MapaDelEsc));
    mapa->escenarios = NULL;
    mapa->num_escenarios = 0;
    mapa->inicio = NULL;
    return mapa;
}


/*int tiempo;
    List* inventario;
    Juego* actual;// en el que esta el jugador
    int totalPeso;
    int totalPuntaje; 
} Jugador;

//inicializar jugador
Jugador* crear_item(int tiempo, Juego* actual, int totalPeso, int totalPuntaje) {
    Jugador* jugador_naranja = (Jugador*)malloc(sizeof(Jugador));
    jugador_naranja->tiempo = 10;
    jugador_naranja->inventario = list_create();
    jugador_naranja->actual = ;

    return Jugador;
}*/

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
    free(esc->conexiones);
    free(esc);
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

//cargar los archivos y convertir el grafo en un mapa
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
            
            esc->items[item_idx++] = crear_item(nombre, valor, peso, esc->id);
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
    printf("El archivo se a cargado correctamente\n");
    return mapa;
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




    /*void mostrar_escenario_actual(Jugador* datos_jugador) {
    Juego* actual = datos_jugador->actual;
    
    if(actual->num_items == 0) {
        printf("No hay items en este escenario.\n");
        return;
    }
    
    printf("Items en el escenario:\n");
    for(int i = 0; i < actual->num_items; i++) {
        Item* item = actual->items[i];
        printf("- %s (Valor: %d, Peso: %d)\n", item->nombre, item->valor, item->peso);
    }
}*/
    Juego* actual = datos_jugador->actual;
     //1)
    printf("Nombre Del Escenario: %s\n", actual->nombre);
    printf("Descripción Escenario: %s\n\n", actual->descripcion);
    
    //2)
    printf("------ Items disponibles de este escenario ------\n");
    if(actual->num_items == 0){
        printf("No hay items disponibles en este escenario\n");
        printf("\n");
    }
    else{
        for(int i = 0; i < actual->num_items; i++) {
            Item* contenido_item = actual->items[i];
            printf("Item: %s, Valor: %d, Peso: %d\n", contenido_item->nombre, contenido_item->valor, contenido_item->peso);
        }
        printf("\n");
    }

    //3)
    printf("Tiempo restantes: %d\n\n", datos_jugador->tiempo);

    //4)
    printf("------ Items ingresados al inventario ------\n");
    Item* item_del_inventario = list_first(datos_jugador->inventario);
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
    MapaDelEsc* actual = datos_jugador->actual;
    printf("Direcciones posibles desde este escenario:\n");
    if (actual->arriba != -1) printf("Arriba: %d\n",actual->arriba);
    if (actual->abajo != -1) printf("Abajo: %d\n", actual->abajo);
    if (actual->izquierda != -1) printf("Izquierda: %d\n", actual->izquierda);
    if (actual->derecha != -1) printf("Derecha: %d\n", actual->derecha);
    if(actual->arriba == -1 && actual->abajo == -1 && actual->izquierda == -1 && actual->derecha == -1){
        printf("No hay direcciones disponibles para este escenario\n");
    }
}

/*
void recoger_items(Jugador* datos_jugador, Map* esce_ID){
    /*analisis:
    para recoger un item primero debo mostrarle al usuario el disponible en el escenario junto a su id
    y luego el usuario si lo desea debe colocar el numero del id para tomarlo.
    
    para realizar lo siguiente debo ingresar los items al inventario con list_pushback y si hay más de un item en el
    escenario debo pedirle al ususario que separe los id por un espacio para agregarlos como en el leer el archivo con
    List* items_string = split_string(campos[3], ";");, pero con especificaciones diferentes, ya que yo quiero separar el espacio:  
    split_string(id_ingresadoPorJugador, " ");
    
    Juego* actual = datos_jugador->actual;
    //recordar que si le coloco else puede no termin ar su ejecucion y darme error
    if(actual->num_items == 0){
        printf("No hay items disponibles en este escenario\n");
        return;
    }
    
    //mostrar los items disponibles
    puts("========================================");
    puts("     Item(s) disponibles");
    puts("========================================");
    Item* contenido_item = actual->items[0];//el primer elemento del arreglo
    for(int i = 0; i < actual->num_items; i++){
        printf("ID: %s,  Item: %s,  Valor: %s,  Peso: %s\n",contenido_item->id_item, contenido_item->nombre, contenido_item->valor, contenido_item->peso);
    } 

    //falta agregar items y ver si lo agrego a mi inventario debo eliminarlo del "juego"
    //preguntar al jugador
    printf("Ingrese el/los id(s) que desea ingresar \n");
    char id_ingresadoPorJugador[100];
    getchar();
    fgets(id_ingresadoPorJugador, sizeof(id_ingresadoPorJugador), stdin);

    //separar los ids con la función del profe
    List* lis_items_elegido = split_string(id_ingresadoPorJugador, " ");

    //guardar los items en el inventario
    List* new_list_items_escenario = list_create();
    Item* contenido_item = datos_jugador->actual->items[0];

    for (int i = 0; i < datos_jugador->actual->num_items; i++) {
        Item* item = datos_jugador->actual->items[i];
        int item_tomado = 0;//actua como false
        int* id_ids = list_first(lis_items_elegido);//sacar id de a uno
        while(id_ids != NULL){
            int id_ptr = *id_ids;
            if(datos_jugador->actual->id == id_ptr){
                list_pushBack(datos_jugador->inventario, id_ptr);
                item_tomado = 1;//actua como true
                break;
            }
            id_ids = list_next(lis_items_elegido);
        
        }
        if(item_tomado == 0){
            list_pushBack(new_list_items_escenario, datos_jugador->actual->items);
        }
    }
    //actualizar los items del juego, como el jugador toma uno, ese item desaparece de los items del juego
    actual->items = new_list_items_escenario;
    datos_jugador->tiempo --;

    printf("item agregado al inventario: %s\n", datos_jugador->inventario);
    printf("tiempo restante del jugador: %s\n", datos_jugador->tiempo);

}*/

void avanzar_una_direccion(Jugador* datos_jugador, MapaDelEsc* mapa_juego){
    Juego* actual = datos_jugador->actual;
    /*mostrar las direcciones disponibles, preguntar al usuario si desea*/
    printf("Direcciones posibles desde este escenario:\n");
    if (actual!= -1) printf("Arriba es: (1)\n");
    if (actual->abajo != -1) printf("Abajo es: (2)\n");
    if (actual->izquierda != -1) printf("Izquierda es: (3)\n");
    if (actual->derecha != -1) printf("Derecha es: (4)\n\n");
    printf("seleccione el numero de la direccion si desea avanzar en esa dirección\n");

    char direccion;
    scanf(" %c", &direccion);
    int id_de_la_direccion = -1;//colocar la id de la direccion que elegimos
    
    switch (direccion) {
        case '1':
            id_de_la_direccion = actual->arriba;
            printf("Has avanzado a la dirección 'ARRIBA'\n");
            break;
        case '2':
            id_de_la_direccion = actual->abajo;
            printf("Has avanzado a la dirección 'ABAJO'\n"); 
            break;
        case '3':
            id_de_la_direccion = actual->izquierda; 
            printf("Has avanzado a la dirección 'IZQUIERDA'\n");
            break;
        case '4':
            id_de_la_direccion =actual->derecha;
            printf("Has avanzado a la dirección 'DERECHA'\n"); 
            break;
        default:
            printf("ha ingresado una dirección inválida\n");
            
    }
    
    if(id_de_la_direccion == -1){
        printf("No hay direcciones disponibles para este escenario\n");
    }
    //convertir id_de_la_direccion a string ya que primero lo use como número para las direcciones y ahora lo debo 
    //usar para cambiar de escenario
    char id_direccion_str[100];
    snprintf(id_direccion_str, sizeof(id_direccion_str), "%d", id_de_la_direccion);

    MapPair* next = map_search(mapa_juego, id_direccion_str);
    if (next == NULL){
       printf("No se a encontrado el escenaro con ID: %s\n", id_direccion_str);
       return;
    }
    datos_jugador->actual = (Juego*)next->value;
    datos_jugador->tiempo --;

}

int main() {
    int opcion;
    int archivo_cargado = 0;
    //parametros para las funciones
    MapaDelEsc* mapa_del_juego = crear_mapa(comparar_claves_ids);
    //datos del jugador
    Jugador* datos_jugador = (Jugador*)malloc(sizeof(Jugador));
    if(datos_jugador == NULL) EXIT_FAILURE;
    datos_jugador->tiempo = 10;
    datos_jugador->inventario = list_create();
    datos_jugador->actual = mapa_del_juego;
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
        getchar(); // Limpia buffer

        switch (opcion) {
            case 1:
                limpiarPantalla();
                printf("\n--- Cargando laberinto desde archivo CSV ---\n");
                MapaDelEsc* mapa_archivo = cargar_archivo();
                liberar_mapa(mapa_archivo);
                
                archivo_cargado = 1;
                //aqui puedo saber en que escenario está el jugador
                
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
                            //recoger_items(datos_jugador, esce_ID);
                            break;
                        case '2':
                            limpiarPantalla();
                            //descartar_items(estado_inicial);
                            break;
                        case '3':
                            limpiarPantalla();
                            avanzar_una_direccion(datos_jugador, mapa_del_juego);
                            break;
                        case '4':
                            limpiarPantalla();
                            //reiniciar_partida(estado_inicial);
                            break;
                        }
                        mostrar_escenario_actual(datos_jugador);
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