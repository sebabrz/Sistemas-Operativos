#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, exit, random
#include <unistd.h> // fork, exec, getpid, sleep
#include <sys/wait.h> // wait, waitpid
#include <pthread.h> // hilos POSIX
#include <sys/types.h> // pid_t y otros tipos
#include <time.h> // time, medicion de tiempo
#include <stdint.h> // enteros de tamaño fijo
#include <sys/mman.h> // memoria compartida (mmap)
#include <fcntl.h> // flags de apertura (O_CREAT, etc.)
#include <sys/stat.h> // permisos de archivos
#include <sys/shm.h> // memoria compartida System V
#include <string.h> // manejo de strings
#include <sys/time.h> // gettimeofday
#include <math.h> // funciones matematicas
#include <dirent.h> // lectura de directorios

#define READ 0
#define WRITE 1
#define CANT 10
#define size 100

/*
    Variante del ejercicio anterior: el mensaje enviado por el pipe es
    un struct con un entero (tipo de mensaje) y una cadena (contenido
    del mensaje), en lugar de enteros sueltos.
*/

struct dato {
    int tipo;
    char mensaje[size];
};

int main()
{

    int pipes[2];
    struct dato msj;
    srandom(time(NULL));

    pid_t pid;

    if (pipe(pipes) == -1) {
        perror("Error al crear pipe");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        printf("Error al crear proceso");
        exit(1);
    }

    if (pid > 0) {
        //---- Proceso padre ----
        close(pipes[READ]);

        for (int i = 0; i < CANT; i++) {
            msj.tipo = random() % 9;
            strcpy(msj.mensaje, "hola");
            write(pipes[WRITE], &msj, sizeof(msj));
        }

        close(pipes[WRITE]);
        wait(NULL);

    } else if (pid == 0) {
        //---- Proceso hijo ----
        close(pipes[WRITE]);

        for (int j = 0; j < CANT; j++) {
            read(pipes[READ], &msj, sizeof(msj));
            printf("Tipo: %d, Mensaje: %s\n", msj.tipo, msj.mensaje);
        }

        close(pipes[READ]);
        exit(0);

        return 0;
    }
}
