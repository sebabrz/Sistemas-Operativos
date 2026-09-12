#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int profundidad, anchura;

        printf("Profundidad: ");
        scanf("%d", &profundidad);
        printf("Anchura: ");
        scanf("%d", &anchura);

    printf("PID=%d PPID=%d (raiz)\n", getpid(), getppid());

    for (int nivel = 0; nivel < profundidad; nivel++) {
        pid_t pid;

        for (int i = 0; i < anchura; i++) {
            pid = fork();
            if (pid == 0) {
                printf("PID=%d PPID=%d (nivel %d)\n", getpid(), getppid(), nivel+1);
                if (i > 0) {
                    exit(0); // hoja, no genera mas
                }
                break; // soy el primer hijo -> sigo al proximo nivel
            }
        }

        if (pid > 0) {
            // soy el que acaba de generar los "anchura" hijos de este nivel
            while (wait(NULL) > 0);
            break; // no sigo yo, ya delegue en mi primer hijo
        }
        // si llegue hasta aca, soy el primer hijo (pid==0), continuo el for de nivel
    }

    return 0;
}