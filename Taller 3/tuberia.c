#include <stdio.h>    
#include <stdlib.h>   
#include <unistd.h>   
#include <sys/wait.h>
#include <string.h>

int main() {
    int tub[2][2]; 
    pipe(tub[0]);
    pipe(tub[1]);
    int i;         
    pid_t root = getpid(); 
    char buffer[128];

    for (i = 0; i < 3; ++i) {
        if(!fork()) break;
    }

    if(i == 0) {
        close(tub[0][0]);
        close(tub[1][0]);
        close(tub[1][1]);
        FILE *infile = fopen("file.txt", "r");
        if (infile == NULL) {
            perror("Error abrir");
            return -1;
        }

        while (fgets(buffer, sizeof(buffer), infile) != NULL) {
            write(tub[0][1], buffer, strlen(buffer));
        }

        fclose(infile);
        close(tub[0][1]);
              

        
    }

    if (i == 1) {
        close(tub[0][1]);
        close(tub[1][0]);

        while (read(tub[0][0], buffer, sizeof(buffer)) > 0) {
            write(tub[1][1], buffer, strlen(buffer));
        }
        close(tub[0][0]);
        close(tub[1][1]);
    }

    if (i == 2) {
        close(tub[0][0]);
        close(tub[0][1]);
        close(tub[1][1]);
        while (read(tub[1][0], buffer, sizeof(buffer)) > 0) {
            printf("%s\n", buffer);
        }
        close(tub[1][0]);
    }
    


    else{
        close(tub[0][0]);
        close(tub[0][1]);
        close(tub[1][0]);
        close(tub[1][1]);
        for (int j = 0; j < 3; j++) {
            wait(NULL);       
        }

    }
    
    return 0;
}