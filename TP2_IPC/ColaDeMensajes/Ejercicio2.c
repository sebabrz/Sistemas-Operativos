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

#define KEY ((key_t)(1243))
#define CANT 30

/*
    Extension del ejercicio anterior de colas de mensajes: se agrega
    un cuarto proceso que puede leer cualquiera de los dos tipos de
    mensaje. Al finalizar, cada proceso indica la cantidad de mensajes
    que leyo.
*/

/*
    - IPC_NOWAIT: si no hay ningun mensaje del tipo pedido en la cola,

    - msgrcv() no bloquea al proceso -> devuelve -1 (errno = ENOMSG) al instante.

    - Sin este flag, msgrcv() se queda esperando (bloqueado) hasta que llegue un mensaje de ese tipo.
*/

struct mensaje {
    long tipo;
    int dato;
};

int main()
{

    pid_t pid1;
    pid_t pid2;
    pid_t pid3;

    srandom(time(NULL));

    int msqid = msgget(KEY, IPC_CREAT | 0666);

    int tipo1 = 0;
    int tipo2 = 0;

    if (msqid == -1) {
        printf("La cola no pudo crearse");
        exit(1);
    }
    
    printf("Soy el proceso padre mi pid es: %d \n", getpid());

    // llenar la cola de mensajes, con dos mensajes diferentes de tipos diferentes.
    for (int i = 0; i < CANT; i++) {
        struct mensaje mensajes[CANT];
        int tip = random() % 2 + 1;
        mensajes[i].tipo = tip;
        // strcpy(mensajes[i].texto, "Hola hijo %d, soy de tipo %d", mensajes[i].tipo, mensajes[i].tipo);
        mensajes[i].dato = random() % 5;
        int longitud = sizeof(struct mensaje) - sizeof(long);
        msgsnd(msqid, &mensajes[i], longitud, 0);
    }

    pid3 = fork();

    if (pid3 < 0) {
        printf("Error");
        exit(1);
    }

    if (pid3 == 0) {
        int leidos = 0;
        printf("Soy el hijo 3 mi pid es: %d mi padre es %d \n", getpid(), getppid());

        struct mensaje mensajes3[CANT];
        int longitud3;
        for (int i = 0; i < CANT; i++) {
            longitud3 = sizeof(struct mensaje) - sizeof(long);
            if (msgrcv(msqid, &mensajes3[i], longitud3, 0, IPC_NOWAIT) == -1) {
                break;
            } else {
                leidos++;
            }
            printf("Proceso 3: Recibi el mensaje: %d, soy de tipo %ld \n", mensajes3[i].dato, mensajes3[i].tipo);
        }
        printf("Proceso 3: Lei %d procesos\n", leidos);
        exit(0);

    } else {

        pid1 = fork();

        if (pid1 < 0) {
            printf("Error");
            exit(1);
        }

        if (pid1 == 0) {
            //---- Proceso hijo 1 ----
            int leidos = 0;
            printf("Soy el hijo 1 mi pid es: %d mi padre es %d \n", getpid(), getppid());

            struct mensaje mensajes1[CANT];
            int longitud1;
            for (int i = 0; i < CANT; i++) {
                longitud1 = sizeof(struct mensaje) - sizeof(long);
                if (msgrcv(msqid, &mensajes1[i], longitud1, 1, IPC_NOWAIT) == -1) {
                    break;
                } else {
                    leidos++;
                }
                printf("Proceso1: Recibi el mensaje: %d, soy de tipo %ld \n", mensajes1[i].dato, mensajes1[i].tipo);
            }
            printf("Proceso1: Lei %d procesos \n", leidos);
            exit(0);

        } else if (pid1 > 0) {

            pid2 = fork();

            if (pid2 < 0) {
                printf("Error");
                exit(1);
            }

            if (pid2 == 0) {
                //---- Proceso hijo 2----
                int leidos = 0;
                printf("Soy el hijo 2 mi pid es: %d mi padre es %d \n", getpid(), getppid());

                struct mensaje mensajes2[CANT];
                int longitud2;
                for (int i = 0; i < CANT; i++) {
                    longitud2 = sizeof(struct mensaje) - sizeof(long);

                    if (msgrcv(msqid, &mensajes2[i], longitud2, 2, IPC_NOWAIT) == -1) {
                        break;
                    } else {
                        leidos++;
                    }

                    printf("Proceso2: Recibi el mensaje: %d, soy de tipo %ld \n", mensajes2[i].dato, mensajes2[i].tipo);
                }
                printf("Proceso2: Lei %d procesos\n", leidos);
                exit(0);
            }

            // espera por ambos hijos
            wait(NULL);
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}