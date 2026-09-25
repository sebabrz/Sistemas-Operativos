#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#define TAM_BUFFER 100

int main() {
    int pipe1[2]; // padre -> hijo (mensaje original)
    int pipe2[2]; // hijo -> padre (mensaje invertido)
    pid_t pid;
    char mensaje[TAM_BUFFER] = "Hi There";
    char buffer[TAM_BUFFER];

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("pipe");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // HIJO: lee de pipe1, invierte mayus/minus, escribe en pipe2
        close(pipe1[1]);
        close(pipe2[0]);

        read(pipe1[0], buffer, TAM_BUFFER);
        close(pipe1[0]);

        for (int i = 0; buffer[i] != '\0'; i++) {
            if (islower(buffer[i])) {
                buffer[i] = toupper(buffer[i]);
            } else if (isupper(buffer[i])) {
                buffer[i] = tolower(buffer[i]);
            }
        }

        write(pipe2[1], buffer, strlen(buffer) + 1);
        close(pipe2[1]);
        exit(0);
    } else {
        // PADRE: escribe en pipe1, lee de pipe2
        close(pipe1[0]);
        close(pipe2[1]);

        write(pipe1[1], mensaje, strlen(mensaje) + 1);
        close(pipe1[1]);

        read(pipe2[0], buffer, TAM_BUFFER);
        close(pipe2[0]);

        printf("Original:  %s\n", mensaje);
        printf("Invertido: %s\n", buffer);

        wait(NULL);
    }

    return 0;
}