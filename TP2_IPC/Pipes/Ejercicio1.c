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

/*
    Comunicacion entre dos procesos (padre e hijo) mediante pipes.
    El padre envia el mensaje "Hola" y el hijo lo lee del pipe,
    lo muestra por pantalla y responde con "Hola Mundo!".
*/

int main()
{

    int pipeFD1[2];
    int pipeFD2[2];
    char mensaje1[100];
    char mensaje2[100];

    pid_t pid;

    if (pipe(pipeFD1) == -1) {
        printf("Error al crear el pipe");
        exit(1);
    }
    if (pipe(pipeFD2) == -1) {
        perror("Error al crear pipeFD2");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        printf("Error al crear proceso");
        exit(1);
    }

    if (pid > 0) {
        //---- Proceso padre ----
        printf("Soy el proceso padre ID: %d \n", getpid());

        close(pipeFD1[READ]); //cierro el extremo de lectura
        close(pipeFD2[WRITE]);

        char *mensaje1 = "Hola";
        write(pipeFD1[WRITE], mensaje1, sizeof(mensaje1)); //escribo sobre el extremo de escritura

        close(pipeFD1[WRITE]);
        printf("Mensaje enviado.\n");

        wait(NULL);
        read(pipeFD2[READ], mensaje2, sizeof(mensaje2));

        close(pipeFD2[READ]);

        printf("El mensaje del hijo es: %s \n", mensaje2);

    } else if (pid == 0) {
        //---- Proceso hijo ----
        printf("Soy el proceso hijo ID: %d \n", getpid());

        close(pipeFD1[WRITE]); //cierro el extremo de escritura
        close(pipeFD2[READ]);

        read(pipeFD1[READ], mensaje1, sizeof(mensaje1)); //escribo sobre el extremo de lectura

        close(pipeFD1[READ]);

        printf("El mensaje del padre es: %s \n", mensaje1);
        printf("Mensajes recibidos.\n");

        char *mensaje2 = "Hola Mundo!";
        write(pipeFD2[WRITE], mensaje2, strlen(mensaje2) + 1);
        close(pipeFD2[WRITE]);
    }

    return 0;
}
