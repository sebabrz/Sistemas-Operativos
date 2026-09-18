#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // malloc, exit, random
#include <unistd.h>     // fork, exec, getpid, sleep
#include <sys/wait.h>   // wait, waitpid
#include <pthread.h>    // hilos POSIX
#include <sys/types.h>  // pid_t y otros tipos
#include <time.h>       // time, medicion de tiempo
#include <stdint.h>     // enteros de tamaño fijo
#include <sys/mman.h>   // memoria compartida (mmap)
#include <fcntl.h>      // flags de apertura (O_CREAT, etc.)
#include <sys/stat.h>   // permisos de archivos
#include <sys/shm.h>    // memoria compartida System V
#include <string.h>     // manejo de strings
#include <sys/time.h>   // gettimeofday
#include <math.h>       // funciones matematicas
#include <dirent.h>     // lectura de directorios

#define READ 0
#define WRITE 1
#define CANT 10

/*
    Usar un pipe para mandar datos de un proceso padre
    a un proceso hijo
*/

int main() {

    int pipes[2];
    int buffer[CANT];
    srandom(time(NULL));

    pid_t pid;

    pid = fork();

    if (pipe(pipes) == -1) {
        perror("Error al crear pipe");
        exit(1);
    }
    if (pid < 0) {
        printf("Error al crear proceso");
        exit(1);
    }

    if (pid > 0) {  
        //---- Proceso padre ----
        close(pipes[READ]);

        for (int i = 0; i < CANT; i++) {
            buffer[i] = random() % 9;
            write(pipes[WRITE], &buffer[i], sizeof(buffer[i]));
        }
        
        close(pipes[WRITE]);
        wait(NULL);
    } else if (pid == 0) {
        //---- Proceso hijo ----
        close(pipes[WRITE]);

        for (int j = 0; j < CANT; j++) {
            read(pipes[READ], &buffer[j], sizeof(buffer[j]));
            printf("El mensaje del padre es: %d \n", buffer[j]);
        }

        close(pipes[READ]);
        exit(0);
    }

    return 0;
}