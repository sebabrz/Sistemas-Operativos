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

        for (int i = 0; i < ITER; i++) {
            sleep(rand() % 10);
            printf(YELLOW"Alumno %d llega \n"RESET, id);
            // el cliente ve si hay sillas
            msgrcv(cola, &msgMutex, msgSize, MUTEX, 0); // tomo el mutex para realizar una operacion que debe estar protegida
            if (msgrcv(cola, &msgRcv, msgSize, semSilla, IPC_NOWAIT) == 0) {
                // si hay silla s, se sienta
                msgSnd.type = semAlumno;
                msgsnd(cola,&msgSnd,msgSize,0); //
                msgsnd(cola,&msgMutex,msgSize,0); // devuelvo el mutex

                printf(YELLOW"alumno %d espera assitente... \n"RESET, id);
                msgrcv(cola, &msgRcv, msgSize, semAula, 0);

                // assitente ayudando alumno
                msgSnd.type = semSilla;
                msgsnd(cola,&msgSnd,msgSize,0); //
            } else {
                // si no hay sillas, se va
                msgsnd(cola,&msgMutex,msgSize,0); // devuelvo el mutex
                printf(YELLOW"alumno %d se va \n"RESET, id);
                sleep(rand() % 10);
                }
        }

    printf(YELLOW"Alumno %d TERMINO \n"RESET, id);

    return 0;
}
