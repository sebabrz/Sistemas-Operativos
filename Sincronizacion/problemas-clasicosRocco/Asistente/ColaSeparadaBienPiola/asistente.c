#include "def.h"

int main(){
    // Clave única para la cola de pedidos
        key_t key_q = generateKey();

        int cola = msgget(key_q, 0666);

        if (cola == -1) {
            perror("Error al obtener la cola de mensajes");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        srand(time(NULL));

        struct mensaje_t msgRcv, msgSnd, msgMutex;

    for (int i = 0; i < ITER; i++) {
        // barbero espera clientes
        msgrcv(cola, &msgMutex, msgSize, MUTEX, 0); // tomo el mutex para realizar una operacion que debe estar protegida
        if (msgrcv(cola, &msgRcv, msgSize, semAlumno, IPC_NOWAIT) == -1) {
            msgsnd(cola,&msgMutex,msgSize,0); // devuelvo el mutex
            msgrcv(cola, &msgRcv, msgSize, semAlumno, 0);
            printf(PURPLE"Asistente se despierta \n"RESET);

            printf(PURPLE"Asistente ayudando a alumno\n"RESET);
            sleep(rand() % 5);

            msgSnd.type = semAula;
            msgsnd(cola,&msgSnd,msgSize,0); //
        // barbero corta pelo
    }else{
            msgsnd(cola,&msgMutex,msgSize,0); // devuelvo el mutex
            printf(PURPLE"Asistente ayudando a alumno\n"RESET);
            sleep(rand() % 5);

            msgSnd.type = semAula;
            msgsnd(cola,&msgSnd,msgSize,0); //
    }

    }

    printf(PURPLE"ASistente TERMINO \n"RESET);

    return 0;
}
