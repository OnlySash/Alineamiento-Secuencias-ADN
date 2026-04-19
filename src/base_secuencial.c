#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char nucleotidos[] = "ATCG";

void generar_cadena(char* cadena,int largo){
    for (int i = 0; i < largo; i++) {
        cadena[i] = nucleotidos[rand() % 4];
    }
    cadena[largo] = '\0';
}

int main() {
    int largo = 20;
    char cadena[largo];
    generar_cadena(cadena,largo);
    for(int i = 0; i < largo; i++ ){
    printf("%c",cadena[i]);
    }
    return 0;
}