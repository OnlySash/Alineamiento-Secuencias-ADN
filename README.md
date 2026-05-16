# Alineamiento de Secuencias de ADN

Proyecto del curso Programación Paralela y Concurrente de la UCR.

El programa busca patrones de nucleótidos dentro de una secuencia de ADN usando tres versiones:

- Secuencial
- Pthreads
- MPI

## Estructura del proyecto

include/    Archivos .h  
src/        Código fuente  
tests/      Casos de prueba  
build/      Archivos compilados  
bin/        Ejecutables  
Makefile

## Compilación

Compilar todo:

make

Compilar versiones específicas:

make seq

make pth

make mpi

Limpiar archivos compilados:

make clean

## Ejecución

Los ejecutables se generan en la carpeta bin.

Parámetros:

-n  Longitud del ADN

-k  Cantidad de patrones

-l  Longitud de cada patrón

-t  Cantidad de hilos (solo Pthreads)

Ejemplos:

./bin/base_sequential -n 1000000 -k 100 -l 10

./bin/base_pthread -n 1000000 -k 100 -l 10 -t 8

mpirun -np 4 ./bin/base_mpi -n 1000000 -k 100 -l 10

## Tests

Ejecutar pruebas:

make test

./run_tests

Los tests verifican:

- Patrón al inicio
- Patrón al final
- Patrón inexistente
- Múltiples ocurrencias
- Comparación entre secuencial y pthread

## Notas

- El ADN y los patrones se generan aleatoriamente.
- La versión Pthreads divide los patrones entre los hilos.
- La versión MPI divide la secuencia entre procesos.