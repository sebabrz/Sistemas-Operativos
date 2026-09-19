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
#include <sys/msg.h>

#define KEY ((key_t) (1243))

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

struct mensaje{
    long tipo;
    char texto[20];
};

int main() {

    pid_t pid1;
    pid_t pid2;

    int msqid = msgget(KEY, IPC_CREAT | 0666);

    if (msqid == -1) {
        printf ("La cola no pudo crearse");
        exit(1);
    }

    pid1 = fork();

    if (pid1 < 0) {
        printf ("Error");
        exit (1);
    }

    if (pid1 == 0) {
        //---- Proceso hijo 1 ----
        printf("Soy el hijo 1 mi pid es: %d mi padre es %d \n" , getpid(), getppid());

        struct mensaje mensaje1;
        int longitud1 = sizeof(struct mensaje) - sizeof(long);


        msgrcv(msqid, &mensaje1 , longitud1, 1, 0);

        printf ("Recibi el mensaje: %s \n", mensaje1.texto);

    } else if (pid1 > 0) {
        //---- Proceso padre----
        printf("Soy el proceso padre mi pid es: %d \n" , getpid());

        //llenar la cola de mensajes, con dos mensajes diferentes de tipos diferentes.

        struct mensaje mensaje1;
        mensaje1.tipo = 1;
        //mensaje1.texto = "Hola hijo 1";
        strcpy(mensaje1.texto, "Hola hijo 2");

        struct mensaje mensaje2;
        mensaje2.tipo = 2;
        //mensaje2.texto = "Hola hijo 2";
        strcpy(mensaje2.texto, "Hola hijo 2");

        int longitud1 = sizeof(struct mensaje) - sizeof(long);
        int longitud2 = sizeof(struct mensaje) - sizeof(long);

        msgsnd(msqid, &mensaje1, longitud1, 0);
        msgsnd(msqid, &mensaje2, longitud2, 0);


        pid2 = fork();

        if (pid2 < 0) {
            printf ("Error");
            exit (1);
        }

        if (pid2 == 0) {
            //---- Proceso hijo 2----
            printf("Soy el hijo 2 mi pid es: %d mi padre es %d \n" , getpid(), getppid());

            struct mensaje mensaje2;
            int longitud2= sizeof(struct mensaje) - sizeof(long);


            msgrcv(msqid, &mensaje2 , longitud2, 2, 0);

            printf ("Recibi el mensaje: %s \n", mensaje2.texto);
        }

        //espera por ambos hijos
        wait(NULL); 
        wait(NULL);
    }

    return 0;
}