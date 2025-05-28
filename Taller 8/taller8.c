#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int rondas, nHilos, filas, columnas;
int **matriz = NULL;
int **matrizAux = NULL;
int terminados = 0;

pthread_cond_t condHilos = PTHREAD_COND_INITIALIZER;
pthread_cond_t condPrincipal = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

float probabilidad(int nVerificados){
    return 0.15f + 0.05f * nVerificados;
}

void *funcionHilos(void *args){
    int id = *(int*)args;
    free(args);
    unsigned int seed = time(NULL) ^ id;

    int inicio = (filas / nHilos) * id;
    int fin = (id == nHilos - 1) ? filas : inicio + (filas / nHilos);

    for (int ronda = 0; ronda < rondas; ronda++) {
        for (int i = inicio; i < fin; i++) {
            for (int j = 0; j < columnas; j++) {
                int nExpuestos = 0, nVerificados = 0;
                for (int i2 = (i > 0 ? i-1 : i); i2 <= (i < filas-1 ? i+1 : i); i2++) {
                    for (int j2 = (j > 0 ? j-1 : j); j2 <= (j < columnas-1 ? j+1 : j); j2++) {
                        if (matriz[i2][j2] == 1 || matriz[i2][j2] == 2) nExpuestos++;
                        if (matriz[i2][j2] == 2) nVerificados++;
                    }
                }

                if (matriz[i][j] == 0 && (nExpuestos > 1 || nVerificados > 1)) {
                    matrizAux[i][j] = 1;
                } else if (matriz[i][j] == 1) {
                    float r = rand_r(&seed) / (float)RAND_MAX;
                    if (r < probabilidad(nVerificados)) {
                        matrizAux[i][j] = 2;
                    } else {
                        matrizAux[i][j] = 1;
                    }
                } else {
                    matrizAux[i][j] = matriz[i][j];
                }
            }
        }

        pthread_mutex_lock(&mutex);
        terminados++;
        if (terminados == nHilos) {
            pthread_cond_signal(&condPrincipal);
            pthread_cond_wait(&condHilos, &mutex);
        } else {
            pthread_cond_wait(&condHilos, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo_datos>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error abriendo archivo");
        return 1;
    }

    fscanf(file, "%d %d %d %d", &rondas, &nHilos, &filas, &columnas);

    matriz = malloc(filas * sizeof(int*));
    matrizAux = malloc(filas * sizeof(int*));
    for (int i = 0; i < filas; i++) {
        matriz[i] = malloc(columnas * sizeof(int));
        matrizAux[i] = malloc(columnas * sizeof(int));
        for (int j = 0; j < columnas; j++) {
            fscanf(file, "%d", &matriz[i][j]);
            matrizAux[i][j] = matriz[i][j];
        }
    }
    fclose(file);

    pthread_t *tids = malloc(nHilos * sizeof(pthread_t));

    for (int i = 0; i < nHilos; i++) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&tids[i], NULL, funcionHilos, arg);
    }

    for (int ronda = 0; ronda < rondas; ronda++) {
        pthread_mutex_lock(&mutex);
        while (terminados < nHilos) {
            pthread_cond_wait(&condPrincipal, &mutex);
        }

        for (int i = 0; i < filas; i++) {
            for (int j = 0; j < columnas; j++) {
                matriz[i][j] = matrizAux[i][j];
            }
        }

        printf("Ronda %d:\n", ronda + 1);
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j < columnas; j++) {
                printf("[%d] ", matriz[i][j]);
            }
            printf("\n");
        }

        terminados = 0;
        pthread_cond_broadcast(&condHilos);
        pthread_mutex_unlock(&mutex);
    }

    for (int i = 0; i < nHilos; i++) {
        pthread_join(tids[i], NULL);
    }

    for (int i = 0; i < filas; i++) {
        free(matriz[i]);
        free(matrizAux[i]);
    }
    free(matriz);
    free(matrizAux);
    free(tids);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condHilos);
    pthread_cond_destroy(&condPrincipal);

    return 0;
}
