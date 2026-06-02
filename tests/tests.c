#include <stdio.h>
#include <string.h>

#include "../include/base.h"
#include "../include/base_pthread.h"
#include "../include/base_sequential.h"
#include "../include/test.h"

#define REINICIAR_PATRON(p)                                                    \
  (p).state = QUEUED;                                                          \
  (p).found_at = -1;

#define INICIAR_PATRON(p, str)                                                 \
  (p).pattern = str;                                                           \
  (p).length = (int)strlen(str);                                               \
  (p).state = QUEUED;                                                          \
  (p).found_at = -1;

int pruebasTotales = 0;
int pruebasBuenas = 0;

void verificarResultado(char *nombre, pattern_t patron, int estadoEsperado, int posicionEsperada) {
  pruebasTotales++;
  int correcto = 1;

  if (patron.state != estadoEsperado) {
    correcto = 0;
  }

  if (estadoEsperado == MATCH && patron.found_at != posicionEsperada) {
    correcto = 0;
  }

  if (correcto == 1) {
    pruebasBuenas++;
    printf("PASS %s\n", nombre);
  } else {
    printf("FAIL %s\n", nombre);
    printf("Posicion obtenida: %d\n", patron.found_at);
  }
}

void casos_1_2_3_4(char *nombre, char *dna, char *textoPatron, int estadoEsperado, int posicionEsperada) {
  pattern_t patrones[1];
  INICIAR_PATRON(patrones[0], textoPatron);

  search_patterns_sequential(dna, strlen(dna), patrones, 1);
  char nombreSec[100];
  sprintf(nombreSec, "%s Secuencial", nombre);
  verificarResultado(nombreSec, patrones[0], estadoEsperado, posicionEsperada);

  REINICIAR_PATRON(patrones[0]);

  search_patterns_pthread(dna, strlen(dna), patrones, 1, 1);
  char nombrePthread[100];
  sprintf(nombrePthread, "%s Pthread", nombre);
  verificarResultado(nombrePthread, patrones[0], estadoEsperado, posicionEsperada);
}

void caso_5(char *nombre, char *dna, char *textoPatron, int numHilos) {
  pattern_t secuencial[1];
  pattern_t paralelo[1];

  INICIAR_PATRON(secuencial[0], textoPatron);
  INICIAR_PATRON(paralelo[0], textoPatron);

  search_patterns_sequential(dna, strlen(dna), secuencial, 1);
  search_patterns_pthread(dna, strlen(dna), paralelo, 1, numHilos);

  pruebasTotales++;
  int iguales = 1;

  if (secuencial[0].state != paralelo[0].state || secuencial[0].found_at != paralelo[0].found_at) {
    iguales = 0;
  }

  if (iguales == 1) {
    pruebasBuenas++;
    printf("PASS %s\n", nombre);
  } else {
    printf("FAIL %s\n", nombre);
  }
}

void run_app_tests() {
  printf("=== EJECUTANDO PRUEBAS UNITARIAS (TESTS) ===\n\n");
  
  pruebasTotales = 0; 
  pruebasBuenas = 0;

  casos_1_2_3_4("Caso 1", "ATGCGT", "ATG", MATCH, 0);
  casos_1_2_3_4("Caso 2", "ATGCGT", "CGT", MATCH, 3);
  casos_1_2_3_4("Caso 3", "ATGCGT", "AAA", MISSING, -1);
  casos_1_2_3_4("Caso 4", "AAAAAA", "AAA", MATCH, 0);
  caso_5("Caso 5 Comparacion", "ATGCGTATGAAA", "ATG", 4);

  printf("\nPruebas pasadas: %d/%d\n", pruebasBuenas, pruebasTotales);
}