#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void* funcionContar(void* arg);
void* funcionContarSecuencia(void* arg);
int* crearRango(int hiloID, int totalHilos, int sizeADN);

int contadorA = 0, contadorT = 0, contadorC = 0, contadorG = 0, contadorSecuencia = 0;
int sizeADN;
char* adn;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[]) {
    int nHilos = atoi(argv[1]);
    if (nHilos <= 1) {
        perror("El número de hilos debe ser mayor o igual a 1\n");
        return EXIT_FAILURE;
    }

    pthread_t* tids = malloc(sizeof(pthread_t) * nHilos);

    FILE* infile = fopen("adn3_100000.txt", "r");
    if (!infile) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }

    if (fscanf(infile, "%d", &sizeADN) != 1 || sizeADN <= 0) {
        perror("Error al leer el tamaño del ADN");
        fclose(infile);
        return EXIT_FAILURE;
    }

    adn = (char*) malloc(sizeof(char) * sizeADN);
    if (adn == NULL) {
        perror("Error al asignar memoria para el ADN");
        fclose(infile);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < sizeADN; i++) {
        int c = fgetc(infile);
        if (c == EOF) break;
        adn[i] = (char)c;
    }

    fclose(infile);

    printf("ADN leído: ");
    for (int i = 0; i < sizeADN; i++) {
        printf("%c", adn[i]);
    }
    printf("\n");

    

    for (int i = 0; i < nHilos; i++){
        if(i == 0){
            pthread_create(&tids[i], NULL, funcionContar, NULL);
        }
        else {
            int* rango = crearRango(i, nHilos - 1, sizeADN);
            pthread_create(&tids[i], NULL, funcionContarSecuencia, (void*)rango);
        }
    }

    for(int i = 0; i < nHilos; i++){
        pthread_join(tids[i], NULL);
    }

    printf("Conteo:\nA: %d\nT: %d\nC: %d\nG: %d\n", contadorA, contadorT, contadorC, contadorG);
    printf("La secuencia gcgtga se repite: %d veces \n", contadorSecuencia);

    free(tids);
    free(adn);
    return EXIT_SUCCESS;
}

void* funcionContar(void* arg) {
    for (int i = 0; i < sizeADN; i++) {
        switch (adn[i]) {
            case 'A':
                contadorA++;
                break;
            case 'T':
                contadorT++;
                break;
            case 'C':
                contadorC++;
                break;
            case 'G':
                contadorG++;
                break;
            default:
                break;
        }
    }

    pthread_exit(NULL);
}

int* crearRango(int hiloID, int totalHilos, int sizeADN){
    int* rango = (int*) malloc(2 * sizeof(int));
    int base = sizeADN / totalHilos;
    int resto = sizeADN % totalHilos;

    int inicio = hiloID * base + (hiloID < resto ? hiloID : resto);
    int fin = inicio + base + (hiloID < resto ? 1 : 0);

    rango[0] = inicio;
    rango[1] = fin;
    return rango;
}


void* funcionContarSecuencia(void* arg) {
    int* rango = (int*)arg;
    int inicio = rango[0], final = rango[1];
    int contador = 0;
    char secuencia[7];
    secuencia[6] = '\0';

    for(int i = inicio; i < final - 6; i++){
        secuencia[0] = adn[i];
        secuencia[1] = adn[i+1];
        secuencia[2] = adn[i+2];
        secuencia[3] = adn[i+3];
        secuencia[4] = adn[i+4];
        secuencia[5] = adn[i+5];

        if(strcmp(secuencia, "GCGTGA") == 0){
            contador++;
        } 
    }
    pthread_mutex_lock(&mutex); 
    contadorSecuencia += contador;
    pthread_mutex_unlock(&mutex);

    free(rango);
}
