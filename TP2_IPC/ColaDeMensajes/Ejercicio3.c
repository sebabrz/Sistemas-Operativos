#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, exit, random
#include <unistd.h> // fork, exec, getpid, sleep
#include <sys/types.h> // pid_t y otros tipos
#include <time.h> // time, medicion de tiempo
#include <string.h> // manejo de strings
#include <sys/msg.h>

#define KEY ((key_t)(13))
#define ALTA 1
#define MEDIA 2
#define BAJA 3

/*
    Simulacion de pedidos de comida respetando prioridades, todo
    bloqueante (sin IPC_NOWAIT):
    - Procesos 0 y 1: piden hamburguesa cada 1s, prioridad baja.
    - Proceso 2: pide papas cada 5s, prioridad media.
    - Proceso 3: pide pizza cada 10s, prioridad alta.
    - El receptor (proceso original) lee con msgtyp = -BAJA, que trae
      siempre el pedido de mayor prioridad disponible y bloquea si no
      hay ninguno. Entre pedido y pedido espera 3 a 5 segundos.
*/

struct pedido {
    long prioridad;
    char comida[30];
};

int main() {
    srandom(time(NULL));

    int msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        printf("La cola no pudo crearse\n");
        exit(1);
    }

    pid_t pid;

    // [0] y [1] hamburguesa, [2] papas, [3] pizza.
    long prioridades[4] = { BAJA, BAJA, MEDIA, ALTA };
    int  tiempos[4]     = { 1,    1,    5,     10   };
    char nombres[4][30] = { "hamburguesa", "hamburguesa", "papas", "pizza" };

    for (int i = 0; i < 4; i++) {
        pid = fork();

        if (pid < 0) {
            printf("Error");
            exit(1);
        }

        if (pid == 0) {
            //soy uno de los 4 procesos que piden comida
            struct pedido pedido;
            int longitud = sizeof(struct pedido) - sizeof(long);

            pedido.prioridad = prioridades[i];
            strcpy(pedido.comida, nombres[i]);

            while (1) {
                sleep(tiempos[i]);

                printf("Se envio del pedido de %s", pedido.comida);

                if (msgsnd(msqid, &pedido, longitud, 0) == -1) {
                    printf("Error al mandar el pedido\n");
                }
            }
        }
    }

    // ===== receptor: despues de crear los 4 =====
    struct pedido receptor;
    int longitudR = sizeof(struct pedido) - sizeof(long);
    int mostrados = 0;

    while (1) {
        if (msgrcv(msqid, &receptor, longitudR, -3, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        mostrados++;
        printf("Pedido Nro %d: %s, Prioridad: %ld\n", mostrados, receptor.comida, receptor.prioridad);

        sleep(random() % 3 + 3);
    }

    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}