/*
    INTEGRANTES:
        Angulo Mateo
        Escobar Andrés
        Mosquera Juan
        
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <stdbool.h>

void Kill(int pid, int sig){
    usleep(10000);
    kill(pid, sig);
}

void handler(int sig){
}

int main(int argc, char* argv[]){

    
    int nProceso;
    int n = atoi(argv[1]);
    pid_t root = getpid();
    pid_t *hijos = (pid_t*)malloc(5 * sizeof(pid_t));
    pid_t *nietos = (pid_t*)malloc(5 * sizeof(pid_t));
    bool soyHijo;

    signal(SIGUSR1, handler);

    for(nProceso = 0; nProceso < 5; nProceso++){
        hijos[nProceso] = fork();
        if(!hijos[nProceso]){
            soyHijo = true;
            nietos[nProceso] = fork();
            if(!nietos[nProceso]){
                soyHijo = false;
                break;
            }
            break;
        }
    }

    for(int i = 0; i < n; i++) {

        if(nProceso == 5){
       
            printf("\n");
            printf("Ronda: %d\n", i+1);
            printf("\n");
            printf("Padre [%d]\n", getpid());
            Kill(hijos[nProceso - 1], SIGUSR1);
            pause();
            printf("Padre [%d]\n", getpid());  

        }else if(soyHijo){

            pause();
            printf("Hijo %d [%d]\n", nProceso, getpid());
            Kill(nietos[nProceso], SIGUSR1);
            pause();
            printf("Hijo %d [%d]\n", nProceso, getpid());
            int to = (nProceso == 0) ? getppid() : hijos[nProceso - 1];
            Kill(to, SIGUSR1);

        } else{


                pause();
                printf("Nieto %d [%d]\n", nProceso, getpid());
                if(nProceso == 2){
                    pid_t bisnieto = fork();
                
                    if(!bisnieto){
                        pause();
                        printf("Bisnieto [%d]\n", getpid());
                        Kill(getppid(), SIGUSR1);
                    }else{
                        Kill(bisnieto, SIGUSR1);
                        pause();
                        printf("Nieto %d [%d]\n", nProceso, getpid());
                        Kill(getppid(), SIGUSR1);
                    }
                }else{
                    Kill(getppid(), SIGUSR1);
                }
        }
    }

    
    free(nietos); free(hijos);
    return EXIT_SUCCESS;
}