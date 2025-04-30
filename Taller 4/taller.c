#include <stdio.h>    
#include <stdlib.h>   
#include <unistd.h>   
#include <sys/wait.h> 
#include <sys/shm.h>  
#include <sys/stat.h> 


unsigned int sizeof_dm(int rows, int cols, size_t sizeElement){
    size_t size;
    size = rows * sizeof(void *);
    size += (cols * rows * sizeElement);

    return size;
}

void create_index(void **m, int rows, int cols, size_t sizeElement){
    int i;
    size_t sizeRow = cols * sizeElement;
    m[0] = m + rows;
    
    for(i = 1; i < rows; i++){
        m[i] = (m[i - 1] + sizeRow);
    }
}



int main () {
    int Rows = 50, Cols = 50;
    size_t sizeMatriz = sizeof_dm(Rows, Cols, sizeof(double));
    int shmId = shmget(IPC_PRIVATE, sizeMatriz, 0x600);
    double **matriz = shmat(shmId, NULL, 0);
    create_index((void *)matriz, Rows, Cols, sizeMatriz);


    for(int i = 0; i < Rows; i++){
        for(int j = 0; j < Cols; j++){
            matriz[i][j] = (i * Cols) + j;
        }
    }

    for(int i = 0; i < Rows; i++){
        for(int j = 0; j < Cols; j++){
            printf("%lf", matriz[i][j]);
        }
        printf("\n");
    }

    shmdt(matriz);
    shmctl(shmId, IPC_RMID, NULL);

    
    return 0;
}