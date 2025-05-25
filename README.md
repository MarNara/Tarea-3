# Tarea-3

## Juego del laberinto:
Descripción: Este juego consiste en recorrer un laberinto, El jugador comienza en un escenario inicial (Entrada principal) y debe avanzar hasta un escenario final de salida, tomando decisiones estratégicas sobre el camino a seguir y qué ítems recoger.
Cada decisión de movimiento consume tiempo, y el peso total de los ítems en el inventario ralentiza al jugador. El objetivo es maximizar el puntaje obtenido por los ítems recogidos antes de que se acabe el tiempo.

## Cómo compilar y ejecutar la tarea:
-Ingresar al visual studio code. -Clonar en repositorio GitHub el link enviado al aula virtual:

### -----------Para compilar y ejecutar el codigo--------------

Abre el archivo principal (por ejemplo, tarea2.c).

Abre la terminal con ctrl + ñ.

En la terminal bash, compila el programa con el siguiente comando :
```bash
gcc tdas/*.c tarea3.c -Wno-unused-result -o tarea3
```
Luego debe escribir lo siguiente para ejecutar la tarea:
```bash
./tarea3
```
Archivo para cargar canciones:
```bash
graphquest.csv
```


## las opciones que funcionan correctamente y las que no lo hacen indicando posibles causas:
Las opiones que funcionan son las siguientes: 
Del menú principal: (1) Cargar Laberinto desde Archivo CSV. (2) Iniciar Partida.

Del menú del juego: (1) Recoger Item(s). (2) Descartar Item(s). (3) Avanzar en una Direccion. (4) Reiniciar Partida. (5) Salir.

------OBSERVACIÓN------

Al pasar a otra opcion debo presionar enter a veces una vez y aveces dos veces.
Sería bueno implementar una función para controlar casos donde el nombre del un item tenga letras minusculas o mayusculas pero sean la misma palabra, como por ejemplo: Cuchillo = cuchillo.
También implementar una biblioteca pero de las que no estaba permitida para que se impriman bien las palabras con acentos.

## Ejemplos mostrando como interactuar con la aplicación.
### Para menú principal
**----------------------- EJEMPLO (1) Cargar Laberinto desde Archivo CSV ---------------------------**
```bash
===== MENU PRINCIPAL =====
1. Cargar Laberinto desde Archivo CSV
2. Iniciar Partida
Seleccione una opcion: 1
```
//despues del enter
```bash
--- Cargando laberinto desde archivo CSV ---
Ingrese el nombre del archivo CSV:
graphquest.csv
El archivo se a cargado correctamente
Presione una tecla para continuar...
```
**----------------------- EJEMPLO (2) Iniciar Partida ---------------------------**
```bash
===== MENU PRINCIPAL =====
1. Cargar Laberinto desde Archivo CSV
2. Iniciar Partida
Seleccione una opcion: 2
```
//despues de presionar enter
```bash
========================================
     Estado Actual
========================================
NOMBRE DEL ESCENARIO: Entrada principal
DESCRIPCION: Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.

==== ITEMS DISPONIBLES ====
No hay items en este escenario.

Tiempo restante: 10

==== TU INVENTARIO ====
Inventario vacio.

==== DIRECCION(ES) DISPONIBLE(S) ====
2) Abajo

***** MENU DEL JUEGO ******
========================================
     Opciones del Jugador
========================================
1) Recoger Item(s)
2) Descartar Item(s)
3) Avanzar en una Direccion
4) Reiniciar Partida
5) Salir
Ingrese su opcion:
```
### Para menú del juego
**----------------------- EJEMPLO (1) Recoger Item(s) ---------------------------**
```bash
Ingrese su opcion: 1
```
//después del enter
```bash
========================================
     Item(s) disponible(s)
========================================
-Item: Cuchillo, Valor: 3, Peso: 1
-Item: Pan, Valor: 2, Peso: 1

Ingrese el nombre del Item que desea llevar, si es mas de uno separar con ';':
Cuchillo; Pan
> Recogiste: Cuchillo, Valor: 3 ,Peso: 1
> Recogiste: Pan, Valor: 2 ,Peso: 1
Presione una tecla para continuar...
```
**----------------------- EJEMPLO (2) Descartar Item(s) ---------------------------**
```bash
Ingrese su opcion: 2
```
//despues del enter
```bash
=== TU INVENTARIO ===
-Item: Cuchillo
-Item: Pan
-Item: Copa dorada
Puntaje Obtenido: 13, Peso Total: 5

Ingrese el nombre del Item que desea descartar, si es mas de uno separar con ';':
Cuchillo
> Descartaste: Cuchillo, Valor: 3, Peso: 1
Presione una tecla para continuar...
```

**----------------------- Ejemplo (3) Avanzar en una Direccion ---------------------------**
```bash
Ingrese su opcion: 3
```
//después del enter
```bash
Direcciones posibles desde este escenario:
Abajo es: (2)
Seleccione el numero de la direccion a la que desea avanzar:
2

Has llegado a: Cocina
Presione una tecla para continuar...
```
**----------------------- Ejemplo (4) Reiniciar Partida ---------------------------**
```bash
Ingrese su opcion: 4
```
//despues del enter

```bash
Has reiniciado la partida con exito
Presione una tecla para continuar...
```
Despues de esto la partida se ve como cuando se inicia la partida con el dos del menú principal, así:
```bash
========================================
     Estado Actual
========================================
NOMBRE DEL ESCENARIO: Entrada principal
DESCRIPCION: Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.

==== ITEMS DISPONIBLES ====
No hay items en este escenario.

Tiempo restante: 10

==== TU INVENTARIO ====
Inventario vacio.

==== DIRECCION(ES) DISPONIBLE(S) ====
2) Abajo

***** MENU DEL JUEGO ******
========================================
     Opciones del Jugador
========================================
1) Recoger Item(s)
2) Descartar Item(s)
3) Avanzar en una Direccion
4) Reiniciar Partida
5) Salir
Ingrese su opcion:
```
**----------------------- Ejemplo (5) Salir ---------------------------**
```bash
Ingrese su opcion: 5
```
//despues del enter

```bash
Saliendo del juego. ┬íHasta luego!
```
