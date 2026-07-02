#define _POSIX_C_SOURCE 200809L
#include "../include/test.h"
#include "../include/base_opencl.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int pruebas_totales = 0;
int pruebas_buenas  = 0;

// Función para buscar patrones usando la versión vieja de pthreads
extern void buscar_un_patron(const char* dna, int dna_len, pattern_t* pttn_struct);

// Función para buscar patrones usando la versión vieja de pthreads
typedef struct {
    const char* dna_string;
    int dna_len;
    pattern_t* patterns;
    int start_index;
    int end_index;
} thread_args_viejo_t;

void* thread_worker_viejo(void* arg) {
    thread_args_viejo_t* data = (thread_args_viejo_t*) arg;
    for (int i = data->start_index; i < data->end_index; i++) {
        buscar_un_patron(data->dna_string, data->dna_len, &data->patterns[i]);
    }
    return NULL;
}

// Función para buscar patrones usando la versión vieja de pthreads 
void search_patterns_pthreads_viejo(const char* dna, int n, pattern_t* patterns, int k, int num_threads) {
    pthread_t threads[num_threads];
    thread_args_viejo_t args[num_threads];
    int p_per_thread = k / num_threads;
    for (int i = 0; i < num_threads; i++) {
        args[i].dna_string  = dna;
        args[i].dna_len     = n;
        args[i].patterns    = patterns;
        args[i].start_index = i * p_per_thread;
        args[i].end_index   = (i == num_threads - 1) ? k : (i + 1) * p_per_thread;
        pthread_create(&threads[i], NULL, thread_worker_viejo, &args[i]);
    }
    for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);
}

void inicializar_patron(pattern_t *p, const char *patStr) {
    p->pattern  = (char*)patStr;
    p->length   = (int)strlen(patStr);
    p->state    = QUEUED;
    p->found_at = -1;
}

// Función para verificar el resultado de una prueba 
void verificar(const char *nom, pattern_t p, int est_esperado, int pos_esperada) {
    pruebas_totales++;
    int ok = (p.state == est_esperado) &&
             (est_esperado != MATCH || p.found_at == pos_esperada);
    printf("%s %s\n", ok ? "PASS" : "FAIL", nom);
    if (!ok)
        printf("      estado=%d pos=%d | esperado estado=%d pos=%d\n",
               p.state, p.found_at, est_esperado, pos_esperada);
    if (ok) pruebas_buenas++;
}

// Función para calcular la diferencia en segundos entre dos instantes de tiempo
double obtener_segundos(struct timespec t0, struct timespec t1) {
    return (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
}

//casos base para validar que todas las versiones de busqueda funcionan correctamente
void probar_casos_base(opencl_env_t *env) {
    printf("\n Casos Base (Se evaluan 4 versiones) \n");

    struct { const char *nom, *dna, *pat; int est, pos; } casos[] = {
        {"Patron al inicio",    "ATGCGT", "ATG",  MATCH,   0},
        {"Patron al final",     "ATGCGT", "CGT",  MATCH,   3},
        {"Inexistente",         "ATGCGT", "AAA",  MISSING,-1},
        {"Patron == secuencia", "ATCG",   "ATCG", MATCH,   0},
        {"Mas largo que ADN",   "AT",     "ATCG", MISSING,-1},
        {"Primera ocurrencia",  "AAAAAA", "AAA",  MATCH,   0},
        {"Patron justo al final","TTTATG", "ATG",  MATCH,   3},
        {"Mismo largo diferente","ATCG",   "ATCA", MISSING,-1}
    };

    int num_casos = sizeof(casos) / sizeof(casos[0]);

    for (int i = 0; i < num_casos; i++) {
        int len = (int)strlen(casos[i].dna);
        char buf[100];
        pattern_t p;

        inicializar_patron(&p, casos[i].pat);
        search_patterns_sequential(casos[i].dna, len, &p, 1);
        snprintf(buf, sizeof buf, "%s [Sec]", casos[i].nom);
        verificar(buf, p, casos[i].est, casos[i].pos);

        inicializar_patron(&p, casos[i].pat);
        search_patterns_pthread(casos[i].dna, len, &p, 1, 4);
        snprintf(buf, sizeof buf, "%s [Pth_Nuevo]", casos[i].nom);
        verificar(buf, p, casos[i].est, casos[i].pos);

        inicializar_patron(&p, casos[i].pat);
        search_patterns_pthreads_viejo(casos[i].dna, len, &p, 1, 4);
        snprintf(buf, sizeof buf, "%s [Pth_Viejo]", casos[i].nom);
        verificar(buf, p, casos[i].est, casos[i].pos);

        inicializar_patron(&p, casos[i].pat);
        search_patterns_opencl(env, casos[i].dna, len, &p, 1);
        snprintf(buf, sizeof buf, "%s [OCL]", casos[i].nom);
        verificar(buf, p, casos[i].est, casos[i].pos);
    }
}

// Función para probar casos aleatorios y comparar resultados entre todas las versiones 
void probar_aleatorios(opencl_env_t *env) {
    printf("\n Prueba Aleatoria \n");
    int N = 50000, L = 10;
    srand(42);

    char *dna = vector_alloc(N); dna_generation(dna, N);
    char *pat = vector_alloc(L);
    for (int i = 0; i < L; i++) pat[i] = "ATCG"[rand() % 4];
    pat[L] = '\0';

    pattern_t sec, pth, ocl, viejo;
    inicializar_patron(&sec,  pat); inicializar_patron(&pth,  pat);
    inicializar_patron(&ocl,  pat); inicializar_patron(&viejo, pat);

    search_patterns_sequential(dna, N, &sec, 1);
    search_patterns_pthread(dna, N, &pth, 1, 4);
    search_patterns_pthreads_viejo(dna, N, &viejo, 1, 4);
    search_patterns_opencl(env, dna, N, &ocl, 1);

    pruebas_totales++;
    int ok = (sec.state == pth.state   && sec.found_at == pth.found_at   &&
              sec.state == ocl.state   && sec.found_at == ocl.found_at   &&
              sec.state == viejo.state && sec.found_at == viejo.found_at);
    printf("%s Aleatorio N=%d L=%d (Todas coinciden)\n", ok ? "PASS" : "FAIL", N, L);
    if (ok) pruebas_buenas++;
    free(dna); free(pat);
}

// Funcion para probar la correcta division de trabajo en MPI y la ejecución de mpirun
void probar_mpi(void) {
    printf("\n MPI Validacion \n");

	// Validamos la correcta division de trabajo en MPI 
    int configs[][2] = {{100000,2},{100000,4},{100000,8}};
    for (int c = 0; c < 3; c++) {
        int N = configs[c][0], P = configs[c][1];
        thread_args_t ch[8];
        for (int r = 0; r < P; r++) compute_mpi_chunks(r, P, N, &ch[r]);
        int ok = (ch[0].start_index == 0 && ch[P-1].end_index == N);
        for (int r = 0; r < P-1 && ok; r++) ok = (ch[r].end_index == ch[r+1].start_index);
        pruebas_totales++;
        printf("%s MPI Chunks P=%d\n", ok ? "PASS" : "FAIL", P);
        if (ok) pruebas_buenas++;
    }
	// Validamos la ejecución de mpirun y la correcta busqueda de patrones en paralelo
    int procs[] = {2, 4};
    for (int p = 0; p < 2; p++) {
        char cmd[128];
		// Ejecutamos el programa con mpirun y redirigimos la salida de error a /dev/null para evitar mensajes de advertencia
        snprintf(cmd, sizeof cmd,
            "mpirun --oversubscribe -np %d ./bin/dna_search -m 3 -n 10000 -k 10 -l 6 2>/dev/null",
            procs[p]);
        FILE *f = popen(cmd, "r"); // Abrimos un proceso para ejecutar el comando y leer su salida
        pruebas_totales++;
        if (!f) { printf("FAIL MPI mpirun %dP no disponible\n", procs[p]); continue; }

        char line[256]; int pat_lines = 0;
		// Leemos la salida del comando línea por línea y contamos cuántas líneas contienen "PATTERN_"
        while (fgets(line, sizeof line, f))
            if (strncmp(line, "PATTERN_", 8) == 0) pat_lines++;
        pclose(f);
        int ok = (pat_lines == 10);
        printf("%s MPI mpirun %dP (lineas=%d esperado=10)\n",
               ok ? "PASS" : "FAIL", procs[p], pat_lines);
        if (ok) pruebas_buenas++;
    }
}

void probar_rendimiento(opencl_env_t *env) {
    printf("\n Rendimiento N=1000000 K=100 L=10 \n");
    printf("%-18s %-10s %-10s %-10s\n", "Version", "Tiempo(s)", "Speedup", "Efic(%)");
    printf("%-18s %-10s %-10s %-10s\n", "-------", "--------", "-------", "------");
    
    const int N = 1000000, K = 100, L2 = 10;
    struct timespec t0, t1;
    
    srand(42);
    char *dna = vector_alloc(N); dna_generation(dna, N);

    // secuencial 
    pattern_t *ps = pattern_alloc(K, L2); srand(42); pattern_generation(ps, L2, K);
    clock_gettime(CLOCK_MONOTONIC, &t0);

    search_patterns_sequential(dna, N, ps, K);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double t_sec = obtener_segundos(t0, t1);
    printf("%-18s %-10.4f %-10.2f %-10.1f\n", "Secuencial", t_sec, 1.0, 100.0);
    for (int i=0; i<K; i++) free(ps[i].pattern);
    free(ps);

    // Pthreads Viejo Paralelo con 4 hilos
    pattern_t *pv = pattern_alloc(K, L2); srand(42); pattern_generation(pv, L2, K);
    clock_gettime(CLOCK_MONOTONIC, &t0);

    search_patterns_pthreads_viejo(dna, N, pv, K, 4);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double t_pth_v = obtener_segundos(t0, t1);
    printf("%-18s %-10.4f %-10.2f %-10.1f\n", "Pthreads-Viejo-4H", t_pth_v, t_sec / t_pth_v, ((t_sec / t_pth_v) / 4.0) * 100.0);
    for (int i=0; i<K; i++) free(pv[i].pattern);
    free(pv);

    // Pthreads Nuevo
    int hlist[] = {1, 2, 4, 8};
    for (int h = 0; h < 4; h++) {
        pattern_t *pp = pattern_alloc(K, L2); srand(42); pattern_generation(pp, L2, K);
        clock_gettime(CLOCK_MONOTONIC, &t0);

        search_patterns_pthread(dna, N, pp, K, hlist[h]);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double t_pth = obtener_segundos(t0, t1);
        double speedup = t_sec / t_pth;
        double ef = (speedup / hlist[h]) * 100.0;
        
        char lbl[25]; snprintf(lbl, sizeof lbl, "Pthreads-Nuevo-%dH", hlist[h]);
        printf("%-18s %-10.4f %-10.2f %-10.1f\n", lbl, t_pth, speedup, ef);
        for (int i=0; i<K; i++) free(pp[i].pattern);
        free(pp);
    }

    // OpenCL
    pattern_t *po = pattern_alloc(K, L2); srand(42); pattern_generation(po, L2, K);
    clock_gettime(CLOCK_MONOTONIC, &t0);
    search_patterns_opencl(env, dna, N, po, K);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double t_ocl = obtener_segundos(t0, t1);
    printf("%-18s %-10.4f %-10.2f %-10.1f\n", "OpenCL", t_ocl, t_sec / t_ocl, (t_sec / t_ocl) * 100.0);
    
    for (int i=0; i<K; i++) free(po[i].pattern);
    free(po);
    free(dna);
}

void run_app_tests(void) {
    pruebas_totales = 0;
    pruebas_buenas  = 0;

    opencl_env_t env = init_opencl_env("assets/kernels/kernel.cl");

    probar_casos_base(&env);
    probar_aleatorios(&env);
    probar_mpi();

    probar_rendimiento(&env);

    finalize_opencl(&env);

    printf("\nPruebas pasadas: %d/%d\n\n", pruebas_buenas, pruebas_totales);
}
