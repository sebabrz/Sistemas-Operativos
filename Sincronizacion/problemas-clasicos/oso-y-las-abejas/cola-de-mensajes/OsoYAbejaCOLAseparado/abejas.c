#include "def.h"

int main(int argc, char *argv[]){
        key_t key_q = generateKey();

        int cola = msgget(key_q, 0666);

        if (cola == -1) {
            perror("Error al obtener la cola de mensajes");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        srand(time(NULL));

        struct mensaje_t msgRcv, msgSnd, msgMutex;

        if (argc != 2) {
        fprintf(stderr, "Usage: %s <id>\n", argv[0]);
        exit(1);
    }

    int id = atoi(argv[1]);

    while (1) {
        sleep(1);  // Simula el tiempo de producción de miel

        // Espera a que haya espacio en el tarro
        msgrcv(cola, &msgRcv, msgSize, VACIO, 0);

        msgrcv(cola, &msgRcv, msgSize, TURNO, 0); //Para colocar miel necesito que haya algun turno habilitado.
        msgSnd.type = TURNO;
        msgsnd(cola,&msgSnd,msgSize,0); // devuelvo el turno inmediatamente.

        // Produce miel
        msgSnd.type = LLENO;
        msgSnd.miel = 1;
        printf("Abeja %d GUARDA miel\n", id);
        msgsnd(cola, &msgSnd, msgSize, 0);

        msgrcv(cola, &msgMutex, msgSize, MUTEX, 0); // tomo el mutex para realizar una operacion que debe estar protegida

        msgrcv(cola, &msgRcv, msgSize, TURNO, 0); //Para chequear si debo despertar o no al oso tambien necesito que haya algun turno habilitado.
        msgSnd.type = TURNO;
        msgsnd(cola,&msgSnd,msgSize,0); // devuelvo el turno inmediatamente.

        // Verifica si el tarro está lleno
        if (msgrcv(cola, &msgRcv, msgSize, VACIO, IPC_NOWAIT) == -1) {
            // Si no hay más mensajes VACIO, el tarro está lleno
            printf("Abeja %d despierta al oso\n", id);
            msgSnd.type = DESPERTAR_OSO;
            msgsnd(cola, &msgSnd, msgSize, 0);
        } else {
            // Si aún hay espacio, devuelve el mensaje VACIO
            msgsnd(cola, &msgRcv, msgSize, 0);
        }
        msgsnd(cola,&msgMutex,msgSize,0); // devuelvo el mutex
    }

    return 0;
}
