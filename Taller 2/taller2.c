#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t h1, h2, h21, h22;
pid_t root;

void handler(int sig) {
}

int main() {
    root = getpid();
    signal(SIGUSR1, handler);

    printf("Soy el padre [%d]\n", root);

    h1 = fork();
    if (h1 == 0) {
        printf("Soy el hijo 1 [%d]\n", getpid());

        h2 = fork();
        if (h2 == 0) {
            printf("Soy el hijo 2 [%d]\n", getpid());

            h22 = fork();
            if (h22 == 0) {
                printf("Soy el hijo 22 [%d]\n", getpid());
                kill(getppid(), SIGUSR1);
                pause();
                exit(0);
            }

            h21 = fork();
            if (h21 == 0) {
                printf("Soy el hijo 21 [%d]\n", getpid());
                kill(getppid(), SIGUSR1);
                pause();
                exit(0);
            }

            pause();
            printf("Soy el hijo 2 [%d]\n", getpid());
            kill(getppid(), SIGUSR1);
            exit(0);
        }

        pause();
        printf("Soy el hijo 1 [%d]\n", getpid());
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    pause();
    printf("Soy el Padre [%d]\n", root);

    return 0;
}
