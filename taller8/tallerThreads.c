#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TAM 100

char** matriz;
int nSolicitudes, contadorBajaPrioridad = 0, contadorCritico = 0, contadorUrgente = 0;
int terminado = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t condPrincipal = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct{
    char buffer[1024];
    int prioridad;
}Solicitud;

Solicitud solicitudesBajaPrioridad[TAM];
Solicitud solicitudesUrgente[TAM];
Solicitud solicitudesCritica[TAM];

void* etapa_1(void*);

void* etapa_2(void*);

int main(int argc, char* argv[]){

    pthread_t h1;
    pthread_t h2;
    pthread_create(&h1, NULL, etapa_1, NULL);
    pthread_create(&h2, NULL, etapa_2, NULL);
    
    pthread_mutex_lock(&mutex);

    FILE *fp = fopen("archivo.txt", "r");
    if(!fp){
        perror("Error al leer archivo");
        return 1;
    }

    fscanf(fp, "%d", &nSolicitudes);
    fgetc(fp);
    
    char buffer[1024];
    matriz = (char**)calloc(nSolicitudes, sizeof(char*));


    size_t size = 0;
    for(int i = 0; i < nSolicitudes; i++){
        fgets(buffer, sizeof(buffer), fp);
        size = strlen(buffer);
        matriz[i] = (char*)malloc(sizeof(char) * (size+1));

        if(buffer[size-1] == '\n')
            buffer[size-1] = '\0';

        strcpy(matriz[i], buffer);
    }
    fclose(fp);

    if(terminado != 2){
        pthread_cond_wait(&condPrincipal, &mutex);
    }

    printf("_________________________\n");
    printf("\033[32m[SOLICITUDES CRITICAS]\033[0m\n");
    printf("\n");
    for(int i = 0; i < contadorCritico; i++){
        printf("%s\n", solicitudesCritica[i].buffer);
    }

    printf("_________________________\n");
    printf("\033[32m[SOLICITUDES URGENTES]\033[0m\n");
    printf("\n");
    for(int i = 0; i < contadorUrgente; i++){
        printf("%s\n", solicitudesUrgente[i].buffer);
    }


    printf("_________________________\n");
    printf("\033[32m[SOLICITUDES BAJA PRIORIDAD]\033[0m\n");
    printf("\n");
    for(int i = 0; i < contadorBajaPrioridad; i++){
        printf("%s\n", solicitudesBajaPrioridad[i].buffer);
    }
    printf("\n");

    pthread_mutex_unlock(&mutex);
    
    pthread_join(h1, NULL);
    pthread_join(h2, NULL);


    free(matriz);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&condPrincipal);
    return EXIT_SUCCESS;
}

void* etapa_1(void*){
    pthread_mutex_lock(&mutex);
    while(terminado != 0){
        pthread_cond_wait(&cond, &mutex);
    }
    for(int i = 0; i < nSolicitudes; i++){
        char* frase = matriz[i];
        if(strstr(frase, "INVALID")){
            strncpy(solicitudesBajaPrioridad[contadorBajaPrioridad].buffer, frase, sizeof(solicitudesBajaPrioridad[0].buffer));
            solicitudesBajaPrioridad[contadorBajaPrioridad].prioridad = 0;
            contadorBajaPrioridad++;
        }  

        if(strncmp(frase, "REQ:", 4) != 0) continue;

        if(!strstr(frase, "URGENTE;")){
            strncpy(solicitudesBajaPrioridad[contadorBajaPrioridad].buffer, frase, sizeof(solicitudesBajaPrioridad[0].buffer));
            solicitudesBajaPrioridad[contadorBajaPrioridad].prioridad = 0;
            contadorBajaPrioridad++;
        }   
    }
    terminado++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void* etapa_2(void*){
    pthread_mutex_lock(&mutex);
    while(terminado != 1){
        pthread_cond_wait(&cond, &mutex);
    }
    for(int i = 0; i < nSolicitudes; i++){
        char* frase = matriz[i];
        if(strstr(frase, "URGENTE;")){
            if(strstr(frase, "servidor") || strstr(frase, "caída") || strstr(frase, "bloqueo")){
                strncpy(solicitudesCritica[contadorCritico].buffer, frase, sizeof(solicitudesCritica[0].buffer));
                solicitudesCritica[contadorCritico].prioridad = 2;
                contadorCritico++;
            } else{
                strncpy(solicitudesUrgente[contadorUrgente].buffer, frase, sizeof(solicitudesUrgente[0].buffer));
                solicitudesUrgente[contadorUrgente].prioridad = 1;
                contadorUrgente++;
            }
        } 
    }
    terminado++;
    pthread_cond_signal(&condPrincipal);
    pthread_mutex_unlock(&mutex);
}
