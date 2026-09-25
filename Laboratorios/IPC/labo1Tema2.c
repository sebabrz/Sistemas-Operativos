#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define READ 0
#define WRITE 1

struct mensaje {
    char vocal;
    int cantidad;
};

int main() {

    char vocales[5] = {'a', 'e', 'i', 'o', 'u'};
    int pipeFD[5][2];

    for (int i = 0; i < 5; i++) {
        if (pipe(pipeFD[i]) == -1) {
            perror("Error al crear pipe");
            exit(1);
        }
    }

    pid_t pid;

    for (int i = 0; i < 5; i++) {

        pid = fork();

        if (pid < 0) {
            printf("Error al crear proceso\n");
            exit(1);
        }

        if (pid == 0) {
            // ===== soy el hijo i, cuento la vocal vocales[i] =====

            // cierro todos los extremos de lectura (no me sirve ninguno)
            for (int j = 0; j < 5; j++) {
                close(pipeFD[j][READ]);
            }
            // cierro los extremos de escritura de los OTROS pipes
            for (int j = 0; j < 5; j++) {
                if (j != i) {               //Si soy I no cierro mi extremo de escritura.
                    close(pipeFD[j][WRITE]);
                }
            }

            FILE *archivo = fopen("asd.txt", "r");

            int cantidad = 0;
            int c;
            while ((c = fgetc(archivo)) != EOF) {
                if (c == vocales[i]) {
                    cantidad++;
                }
            }

            struct mensaje msj;
            msj.vocal = vocales[i];
            msj.cantidad = cantidad;

            write(pipeFD[i][WRITE], &msj, sizeof(msj));
            close(pipeFD[i][WRITE]);
            exit(0);
        }
    }

    // PADRE; cierro todos los extremos de escritura, no los uso
    for (int i = 0; i < 5; i++) {
        close(pipeFD[i][WRITE]);
    }

    struct mensaje msj;
    for (int i = 0; i < 5; i++) {
        read(pipeFD[i][READ], &msj, sizeof(msj));
        printf("Se encontraron %d apariciones de la vocal %c\n", msj.cantidad, msj.vocal);
        close(pipeFD[i][READ]);
    }

    //Los 5 procesos calculan en paralelo , pero el padre los muestrad 
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    return 0;
}