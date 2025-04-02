#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void error(char *msg) {
    perror(msg);
    exit(-1);
}

int leerNumeros(char *filename, int **vec) {
    int c, numero, totalNumeros;
    FILE *infile;
    infile = fopen(filename, "r");
    if (!infile) { error("Error fopen\n"); }
    fscanf(infile, "%d", &totalNumeros);

    *vec = (int *)calloc(totalNumeros, sizeof(int));
    if (!*vec) { error("Error calloc"); }
    for (c = 0; c < totalNumeros; c++) {
        fscanf(infile, "%d", &numero);
        (*vec)[c] = numero;
    }
    fclose(infile);
    return totalNumeros;
}

long long leerTotal(int numeroHijos) {
    FILE *infile;
    long long numero;
    long long total = 0;
    infile = fopen("out.txt", "r");
    if (!infile) error("Error padre archivo resultados");
    for (int i = 0; i < numeroHijos; i++) {
        fscanf(infile, "%lld", &numero);
        total += numero;
    }
    fclose(infile);
    return total;
}

int main() {
    remove("out.txt"); 

    int *vector;
    int cantidadNumeros = leerNumeros("test3.in", &vector);
    int i, j, nHijos;

    printf("Número de hijos a realizar la suma: ");
    scanf("%d", &nHijos);

    int delta = cantidadNumeros / nHijos;

    for (i = 0; i < nHijos; i++) {
        if (fork() == 0) {
            int inicio = delta * i;
            int final = (i == nHijos - 1)? cantidadNumeros: inicio + delta;
            printf("Hijo %d: procesando de %d a %d\n", i, inicio, final - 1);
            long long acumulador = 0;

            for (j = inicio; j < final; j++) {
                acumulador += vector[j];
            }

            FILE *text = fopen("out.txt", "a");
            if (!text) {
                error("Error al abrir out.txt");
            }
            fprintf(text, "%lld\n", acumulador);
            fclose(text);
            
            exit(0);
        }
    }

    for (int i = 0; i < nHijos; i++){
        wait(0);
    }

    long long sumaTotal = leerTotal(nHijos);
    printf("La suma total es igual %lld \n", sumaTotal);

    return 0;
}
