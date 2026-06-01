
#include <stdio.h>
#include <stdlib.h>

#include "../include/base.h"
#include "../include/base_pthread.h"
#include "../include/base_mpi.h"
#include "../include/base_sequential.h"

void option_menu(){
    int option;
    printf("Realizar la búsqueda paralela de patrones en la secuencia de ADN: ");
    printf("\n1. Secuencial");
    printf("\n2. Pthreads");
    printf("\n3. MPI\n");

    scanf("%d", &option);

    switch (option) {
        case 1:
            //run_sequential();
            break;
        case 2:
            //run_pthread();
            break;
        case 3:
            run_mpi();
            break;
        default:
            printf("Opción no válida. Por favor, elige 1, 2 o 3.\n");
    }
}

int main (){
    option_menu();
    return 0; 
}