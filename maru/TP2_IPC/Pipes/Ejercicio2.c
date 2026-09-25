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
#define CANT 5


/*
    Conversacion intercalada entre padre e hijo mediante pipes.
    El padre envia un mensaje y espera la respuesta del hijo antes de
    volver a escribir; el hijo responde a lo que recibe. Cuando alguno
    de los dos decide terminar, envia un mensaje de salida.
*/

int main()
{

    int pipeFD1[2];
    int pipeFD2[2];
    char mensajes[CANT][100] = {"Hola", "bien vos?", "es lo que hay", "salir", "no"};
    char lectura[100];
    char respuestas[CANT][100] = {"Hola pa como estas", "no quiero estudiar mas!", "ok chau", "no", "salir"};

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
        close(pipeFD1[READ]);
        close(pipeFD2[WRITE]);

        for (int i = 0; i < CANT; i++) {
            write(pipeFD1[WRITE], mensajes[i], strlen(mensajes[i]) + 1);
            if (strcmp(mensajes[i], "salir") == 0) {
                break;
            }

            read(pipeFD2[READ], lectura, sizeof(lectura));
            printf("El mensaje del hijo es: %s \n", lectura);

            if (strcmp(lectura, "salir") == 0) {
                break;
            }
        }

        close(pipeFD1[WRITE]);
        close(pipeFD2[READ]);
        wait(NULL);

    } else if (pid == 0) {
        //---- Proceso hijo ----
        close(pipeFD1[WRITE]); //cierro el extremo de escritura
        close(pipeFD2[READ]);

        for (int j = 0; j < CANT; j++) {
            read(pipeFD1[READ], lectura, sizeof(lectura));
            if (strcmp(lectura, "salir") == 0) {
                break;
            }
            printf("El mensaje del padre es: %s \n", lectura); //escribo sobre el extremo de lectura

            write(pipeFD2[WRITE], respuestas[j], strlen(respuestas[j]) + 1);
            if (strcmp(respuestas[j], "salir") == 0) {
                break;
            }
        }

        close(pipeFD1[READ]);
        close(pipeFD2[WRITE]);
        exit(0);
    }

    return 0;
}