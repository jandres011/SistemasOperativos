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
        printf("%d\n", numero);
    }
    fclose(infile);
    return totalNumeros;
}

int leerTotal() {
    FILE *infile;
    int sumap1 = 0, sumap2 = 0, total = 0;
    infile = fopen("out.txt", "r");
    if (!infile) error("Error padre archivo resultados");
    fscanf(infile, "%d", &sumap1);
    fscanf(infile, "%d", &sumap2);
    total = sumap1 + sumap2;
    fclose(infile);
    return total;
}

int main() {
    remove("out.txt"); // Eliminar el archivo si ya existe

    int *vector;
    int cantidadNumeros = leerNumeros("test1.in", &vector);
    int delta = cantidadNumeros / 2;
    int i, j;

    for (i = 0; i < 2; i++) {
        if (fork() == 0) {
            int inicio = delta * i;
            int final = inicio + delta;
            int acumulador = 0;

            for (j = inicio; j < final; j++) {
                acumulador += vector[j];
            }

            FILE *text = fopen("out.txt", "a");
            if (!text) {
                error("Error al abrir out.txt");
            }
            fprintf(text, "%d\n", acumulador);

            exit(0);
        }
    }

    for (int i = 0; i < 2; i++){
        wait(0);
    }

    int sumaTotal = leerTotal();
    printf("La suma total es igual %d \n", sumaTotal);

    return 0;
}
