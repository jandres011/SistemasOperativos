#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

typedef struct {
    double feature1;
    double feature2;
    double fitness;
} Individuo;

int nHilos, generaciones, cantidadIndividuos, individuosHilo;
Individuo* individuos;
pthread_barrier_t barrera;

double fitness(double feature1, double feature2) {
    return 0.5 * (feature1 * feature2);
}

void* funcionGenerarIndividuos(void* arg) {
    int id = *(int*)arg;

    for (int gen = 0; gen < generaciones; gen++) {
        int inicio = id * individuosHilo;
        int final = (id + 1) * individuosHilo;
        if (final > cantidadIndividuos) final = cantidadIndividuos;

        for (int i = inicio; i < final; i++) {
            individuos[i].feature1 = 1 + (100 - 1) * ((double)rand() / RAND_MAX);
            individuos[i].feature2 = 1 + (100 - 1) * ((double)rand() / RAND_MAX);
            individuos[i].fitness = fitness(individuos[i].feature1, individuos[i].feature2);
        }

        pthread_barrier_wait(&barrera);
    }

    return NULL;
}

int intercambiarIndividuos(const void* a, const void* b) {
    Individuo* individuoA = (Individuo*)a;
    Individuo* individuoB = (Individuo*)b;
    if (individuoB->fitness > individuoA->fitness) return 1;
    else if (individuoB->fitness < individuoA->fitness) return -1;
    else return 0;
}

int main(int argc, char const* argv[]) {
    if (argc != 4) {
        perror("Deben ser 4 argumentos");
        return EXIT_FAILURE;
    }

    nHilos = atoi(argv[1]);
    generaciones = atoi(argv[2]);
    cantidadIndividuos = atoi(argv[3]);

    if (nHilos < 1 || generaciones < 1 || cantidadIndividuos < 10) {
        perror("Error: valores inv\xE1lidos. Hilos >= 1, Generaciones >= 1, Individuos >= 10\n");
        return EXIT_FAILURE;
    }

    individuosHilo = (cantidadIndividuos + nHilos - 1) / nHilos;
    individuos = (Individuo*)malloc(sizeof(Individuo) * cantidadIndividuos);
    pthread_t* tids = malloc(sizeof(pthread_t) * nHilos);
    int* ids = malloc(sizeof(int) * nHilos);

    srand(time(NULL));

    pthread_barrier_init(&barrera, NULL, nHilos + 1);

    for (int i = 0; i < nHilos; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, funcionGenerarIndividuos, &ids[i]);
    }

    for (int generacion = 1; generacion <= generaciones; generacion++) {
        pthread_barrier_wait(&barrera);

        printf("Estamos en la generación n° %d\n", generacion);
        qsort(individuos, cantidadIndividuos, sizeof(Individuo), intercambiarIndividuos);

        for (int j = 0; j < 10 && j < cantidadIndividuos; j++) {
            printf("Individuo %d: fitness = %.2lf\n", j + 1, individuos[j].fitness);
        }
    }

    for (int i = 0; i < nHilos; i++) {
        pthread_join(tids[i], NULL);
    }

    pthread_barrier_destroy(&barrera);
    free(individuos);
    free(tids);
    free(ids);

    return 0;
}
