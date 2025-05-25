# Tarea-3

Compilar:
- gcc tdas/*.c tarea3.c -Wno-unused-result -o tarea3

Ejecutar:
- ./tarea3

//graphquest.csv


3. **Compila el código**
    - Abre el archivo principal (por ejemplo, `main.c`).
    - Abre la terminal integrada (`Terminal > Nueva terminal`).
    - En la terminal, compila el programa con el siguiente comando (ajusta el nombre si el archivo principal tiene otro nombre):
        
        ```bash
        gcc *.c -o gestion_pacientes
        ```
        
4. **Ejecuta el programa**

## Juego del laberinto:
Descripción: Este juego consiste en recorrer un laberinto, El jugador comienza en un escenario inicial (Entrada principal) y debe avanzar hasta un escenario final de salida, tomando decisiones estratégicas sobre el camino a seguir y qué ítems recoger.
Cada decisión de movimiento consume tiempo, y el peso total de los ítems en el inventario ralentiza al jugador. El objetivo es maximizar el puntaje obtenido por los ítems recogidos antes de que se acabe el tiempo.

## Cómo compilar y ejecutar la tarea:
-Ingresar al visual studio code. -Clonar en repositorio GitHub el link enviado al aula virtual:

### -----------Para compilar y ejecutar el codigo--------------

Abre el archivo principal (por ejemplo, tarea2.c).

Abre la terminal con ctrl + ñ.

En la terminal bash, compila el programa con el siguiente comando :

- gcc tdas/*.c tarea3.c -Wno-unused-result -o tarea3
Luego debe escribir lo siguiente para ejecutar la tarea:

 ./tarea3
Archivo para cargar canciones:

song_dataset_.csv
las opciones que funcionan correctamente y las que no lo hacen indicando posibles causas; Las opiones que funcionan son las siguientes: (1) Cargar Canciones. (2) Buscar por Género. (3) Buscar por Artista. (4) Buscar por Tempo. (5) Crear Lista de Reproducción. (6) Agregar Canción a Lista. (7) Mostrar Canciones de una Lista . (8) Salir.

------OBSERVACIÓN------

Al pasar a otra opcion debo presionar cualquier tecla.
Sería bueno implementar una función para controlar casos donde el nombre del un item tenga letras minusculas o mayusculas pero sean la misma palabra, como por ejemplo: Cuchillo = cuchillo.
Tambien estaria bien agregar un buscar por pagina que me mueva directamente a la pagina.
------Nota------

La biblioteca windows.h si bien no sale en las bibliotecas standar que el profesor dijo que se podian ocupar, la unica funcion que ocupamos fue "SetConsoleOutputCP(65001);" que lo que hace es cambiar al encoding a utf-8, de esta manera lograbamos hacer que los acentos se mostraran correctamente en la terminal.
Ejemplos mostrando como interactuar con la aplicación.
----------------------- EJEMPLO (1) Cargar Canciones ---------------------------

----------------------- EJEMPLO (2) Buscar por Género ---------------------------

----------------------- EJEMPLO (3) Buscar por Artista ---------------------------

----------------------- EJEMPLO (4) Buscar por Tempo ---------------------------

----------------------- Ejemplo (5) Crear Lista de Reproducción ---------------------------

----------------------- Ejemplo (7) Mostrar Canciones de una Lista ---------------------------

----------------------- Ejemplo (8) Salir ---------------------------

Ingrese su opción: 8
Saliendo del programa...
