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

        struct mensaje_t msgRcv, msgSnd;
    while (1) {
        // Espera a que el tarro esté lleno
        msgrcv(cola, &msgRcv, msgSize, DESPERTAR_OSO, 0);

        printf("El oso se despierta\n");

        for (int i = 0;i< ABEJAS;i++){
                msgrcv(cola, &msgRcv, msgSize, TURNO, 0);
        }
        for (int i = 0; i < PORCIONES; i++) {
            msgrcv(cola, &msgRcv, msgSize, LLENO, 0);
            printf("Oso COME miel %d\n", i);

            // Devuelve un espacio vacío al tarro
            msgSnd.type = VACIO;
            msgsnd(cola, &msgSnd, msgSize, 0);
        }
        printf("El oso se comio toda la miel, ahora se va a dormir\n");

        msgSnd.type = TURNO;
        for (int i = 0;i< ABEJAS;i++){
                msgsnd(cola,&msgSnd,msgSize,0); // devuelvo los turnos
        }
        sleep(3);
    }

    return 0;
}
