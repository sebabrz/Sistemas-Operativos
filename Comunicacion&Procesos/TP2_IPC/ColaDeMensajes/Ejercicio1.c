#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/msg.h>

#define KEY ((key_t)(1243))

struct mensaje {
    long tipo;
    int dato;
};

int main() {
    srandom(time(NULL));

    int msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        printf("La cola no pudo crearse\n");
        exit(1);
    }

    pid_t pid;

    for (int i = 1; i < 3; i++) {
        pid = fork();

        if (pid == 0) {
            int cantidadMensajes = 0;
            while (1) {
                struct mensaje msg;
                int longitud = sizeof(struct mensaje) - sizeof(long);

                if (msgrcv(msqid, &msg, longitud, i, 0) == -1) {
                    perror("msgrcv");
                    exit(1);
                }

                printf("[%d] Soy el proceso %d Recibi el mensaje %d soy de tipo %ld \n", cantidadMensajes, i, msg.dato, msg.tipo);
                cantidadMensajes++;
            }
        }
    }

    int cantidadMensajes = 0;

    while (1) {
        struct mensaje msg;
        int longitud = sizeof(struct mensaje) - sizeof(long);

        msg.tipo = random() % 2 + 1;
        msg.dato = random() % 9;

        if (msgsnd(msqid, &msg, longitud, 0) == -1) {
            printf("Error al mandar el mensaje\n");
        }

        printf("[%d] Se envio mensajes a la cola de tipo %ld el mensaje es %d \n", cantidadMensajes, msg.tipo, msg.dato);
        cantidadMensajes++;

        sleep(1);
    }

    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}