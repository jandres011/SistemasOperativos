#include <stdio.h>      
#include <unistd.h>     
#include <stdlib.h>     
#include <pthread.h>    

void* funcionContar(void* arg){
    char vector = 
    char letra;
    for(int i = 0; i < sizeADN; i++) {
        letra = adn[i];

        switch (letraADN)
        {
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
}

pthread_t tid1;

int main(int argc, char const *argv[]){
    int nHilos = atoi(argv[1]);
    if(nHilos > 1) {
        perror("Los hilos deben ser mayor a 1");
        return EXIT_FAILURE;
    }

    FILE* infile = fopen("adn1_100.txt", "r");
    int sizeADN;

    if (!infile) {
        perror("Error para leer el archivo");
        return EXIT_FAILURE;
    }

    if (fscanf(infile, "%d", &sizeADN) != 1 || sizeADN <= 0) {
        perror("No se pudo leer el tamaño del ADN correctamente");
        fclose(infile);
        return EXIT_FAILURE;
    }

    char* adn = (char*) malloc(sizeof(char) * sizeADN);
    if (adn == NULL) {
        perror("Error al asignar memoria para ADN");
        fclose(infile);
        return EXIT_FAILURE;
    }

    int i, contadorA = 0, contadorT = 0, contadorC = 0, contadorG = 0;
    char c;

    for(i = 0; i < sizeADN; i++) {
        char letraADN = fgetc(infile);
        adn[i] = letraADN;

        switch (letraADN)
        {
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

    printf("ADN leído: ");
    for (int j = 0; j < sizeADN; j++) {
        printf("%c", adn[j]);
    }

    pthread_create(&tid1, NULL, funcionContar, NULL); 
    printf("Hilo principal [%lu]\n", pthread_self()); 
    pthread_join(tid1, NULL);

    free(adn);
    fclose(infile);
    return EXIT_SUCCESS; 
}


