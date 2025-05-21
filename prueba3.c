#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/extra.h"

// Declaración de función (suponiendo que está en otro archivo .c)
void leer_escenarios(); // Ya implementada por ti

int main() {
    int opcion;
    int archivo_cargado = 0;

    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1. Cargar Laberinto desde Archivo CSV\n");
        printf("2. Iniciar Partida\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar(); // Limpia buffer

        switch (opcion) {
            case 1:
                printf("\n--- Cargando laberinto desde archivo CSV ---\n");
                leer_escenarios(); // Solo imprime por ahora
                archivo_cargado = 1;
                break;

            case 2:
                if (!archivo_cargado) {
                    printf("¡Debes cargar el archivo CSV antes de comenzar la partida!\n");
                } else {
                    printf("Iniciando partida...\n");
                    // Aquí deberías llamar a tu función de iniciar el juego
                    // iniciar_partida();
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

//gcc tdas/*.c prueba3.c -Wno-unused-result -o prueba3
//./prueba3

//gcc tdas/*.c prueba3.c tarea3_lecturacsv.c -Wno-unused-result -o prueba3
//gcc tdas/*.c tarea3.c tarea3_lecturacsv.c -Wno-unused-result -o tarea3