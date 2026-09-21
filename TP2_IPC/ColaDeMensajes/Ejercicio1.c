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
#include <sys/msg.h>

#define KEY ((key_t)(143))
#define CANT 10

/*
    Cola de mensajes con tipos: el proceso principal crea la cola y
    envia varios mensajes de dos tipos distintos. Cada uno de los
    otros dos procesos lee iterativamente uno de los tipos de mensaje
    y lo muestra por pantalla, esperando si no hay mensajes disponibles.
*/

/*
    Una caracteristica de las colas de mensajes
    es que los mensajes pueden clasificarse
    utilizando tipos.
    Crear tres procesos tal que:

    - El procesos principal debe enviar varios mensajes
    de dos tipos diferentes utilizando una
    cola de mensajes creada previamente.

    - Cada uno de los restantes procesos lee iterativamente
    uno de los tipos de mensaje y lo muestra por pantalla.

    - Si no existen mensajes en la cola se debe esperar.

*/

struct mensaje {
    long tipo;
    int dato;
};

int main()
{

    pid_t pid1;
    pid_t pid2;

    srandom(time(NULL));

    int msqid = msgget(KEY, IPC_CREAT | 0666);

    int tipo1 = 0;
    int tipo2 = 0;

    if (msqid == -1) {
        printf("La cola no pudo crearse");
        exit(1);
    }

    printf("Soy el proceso padre mi pid es: %d \n", getpid());

    //llenar la cola de mensajes, con dos mensajes diferentes de tipos diferentes.
    for (int i = 0; i < CANT; i++) {

        struct mensaje mensajes[CANT];
        int tip = random() % 2 + 1;
        mensajes[i].tipo = tip;

        if (tip == 1) {
            tipo1++;
        } else {
            tipo2++;
        }

        // strcpy(mensajes[i].texto, "Hola hijo %d, soy de tipo %d", mensajes[i].tipo, mensajes[i].tipo);
        mensajes[i].dato = random() % 5;
        int longitud = sizeof(struct mensaje) - sizeof(long);
        msgsnd(msqid, &mensajes[i], longitud, 0);
    }

    pid1 = fork();

    if (pid1 < 0) {
        printf("Error");
        exit(1);
    }

    if (pid1 == 0) {
        //---- Proceso hijo 1 ----

        printf("Soy el hijo 1 mi pid es: %d mi padre es %d \n", getpid(), getppid());

        struct mensaje mensajes1[tipo1];
        int longitud1;

        for (int i = 0; i < tipo1; i++) {
            longitud1 = sizeof(struct mensaje) - sizeof(long);
            msgrcv(msqid, &mensajes1[i], longitud1, 1, 0);
            printf("Recibi el mensaje: %d, soy de tipo %ld \n", mensajes1[i].dato, mensajes1[i].tipo);
        }

        exit(0);

    } else if (pid1 > 0) {

        pid2 = fork();

        if (pid2 < 0) {
            printf("Error");
            exit(1);
        }

        if (pid2 == 0) {
            //---- Proceso hijo 2----

            printf("Soy el hijo 2 mi pid es: %d mi padre es %d \n", getpid(), getppid());

            struct mensaje mensajes2[tipo2];
            int longitud2;

            for (int i = 0; i < tipo2; i++) {
                longitud2 = sizeof(struct mensaje) - sizeof(long);
                msgrcv(msqid, &mensajes2[i], longitud2, 2, 0);
                printf("Recibi el mensaje: %d, soy de tipo %ld \n", mensajes2[i].dato, mensajes2[i].tipo);
            }
            
            exit(0);
        }

        //espera por ambos hijos
        wait(NULL);
        wait(NULL);
    }

    return 0;
}